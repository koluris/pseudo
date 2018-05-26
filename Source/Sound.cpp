#import "Global.h"


#define spuAcc(addr)\
    *(uh *)&mem.hwr.ptr[addr]

#define spuChannel(addr)\
    (addr >> 4) & 0x1f


CstrAudio audio;

void CstrAudio::reset() {
    // Mem
    memset(&spuMem, 0, sizeof(spuMem));
    memset(&  sbuf, 0, sizeof(sbuf));
    
    // Variables
    spuAddr    = ~(0);
    spuVolumeL = MAX_VOLUME;
    spuVolumeR = MAX_VOLUME;
    stereo     = true;
    
    // Channels reset
    for (int n = 0; n < MAX_CHANNELS; n++) {
        memset(&spuVoices[n], 0, sizeof(spuVoices[n]));
    }
}

void CstrAudio::depackVAG(voice *chn) {
    uh p = chn->saddr;
    sh s_1 = 0;
    sh s_2 = 0;
    sh temp[28];
    memset(&temp, 0, 28);
    
    while (1) {
        ub shift  = spuMem.iub[p] & 15;
        ub filter = spuMem.iub[p] >> 4;
        
        for (int i = 2; i < 16; i++) {
            sh a = ((spuMem.iub[p + i] & 0x0f) << 12);
            sh b = ((spuMem.iub[p + i] & 0xf0) <<  8);
            if (a & 0x8000) a |= 0xffff0000;
            if (b & 0x8000) b |= 0xffff0000;
            temp[i * 2 - 4] = a >> shift;
            temp[i * 2 - 3] = b >> shift;
        }
        
        for (int i = 0; i < 28; i++) {
            sh res = temp[i] + ((s_1 * f[filter][0] + s_2 * f[filter][1] + 32) >> 6);
            s_2 = s_1;
            s_1 = res;
            res = MIN(MAX(res, SHRT_MIN), SHRT_MAX);
            chn->buffer.ish[chn->size++] = res;
            
            // Overflow
            if (chn->size == USHRT_MAX) {
                printf("SPU Channel size overflow\n");
                return;
            }
        }
        
        // Fin
        ub op = spuMem.iub[p + 1];
        
        if (op == 3 || op == 7) { // Termination
            return;
        }
        if (op == 6) { // Repeat
            chn->raddr = chn->size;
        }
        
        // Advance Buffer
        p += 16;
    }
}

void CstrAudio::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void CstrAudio::decodeStream() {
    while(!psx.suspended) {
        for (int n = 0; n < MAX_CHANNELS; n++) {
            voice *chn = &spuVoices[n];
            
            // Channel on?
            if (chn->on == false) {
                continue;
            }
            
            for (int i = 0; i < SBUF_SIZE; i++) {
                chn->count += chn->freq;
                if (chn->count >= SAMPLE_RATE) {
                    chn->pos += chn->count/SAMPLE_RATE;
                    chn->count %= SAMPLE_RATE;
                }
                
                // Mix Channel Samples
                if (stereo) {
                    sbuf.temp[i*2+0] += (+chn->buffer.ish[chn->pos] * chn->volumeL) / MAX_VOLUME;
                    sbuf.temp[i*2+1] += (-chn->buffer.ish[chn->pos] * chn->volumeR) / MAX_VOLUME;
                }
                else {
                    sbuf.temp[i] += (chn->buffer.ish[chn->pos] * ((chn->volumeL + chn->volumeR) / 2)) / MAX_VOLUME;
                }
                
                // End of Sample
                if (chn->pos >= chn->size) {
                    if (chn->raddr > 0) { // Repeat?
                        chn->pos = chn->raddr;
                        chn->count = 0;
                        continue;
                    }
                    chn->on = false;
                    break;
                }
            }
        }
        // Volume Mix
        for (int i = 0; i < SBUF_SIZE; i++) {
            if (stereo) {
                sbuf.fin[i*2+0] = (+(sbuf.temp[i*2+0] / 4) * spuVolumeL) / MAX_VOLUME;
                sbuf.fin[i*2+1] = (-(sbuf.temp[i*2+1] / 4) * spuVolumeR) / MAX_VOLUME;
            }
            else {
                sbuf.fin[i] = ((sbuf.temp[i] / 4) * ((spuVolumeL + spuVolumeR) / 2)) / MAX_VOLUME;
            }
        }
        
        // OpenAL
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        
        if (processed >= ALC_BUF_AMOUNT) {
            // We have to free buffers
            printf("PSeudo /// Inadequent ALC buffer size -> %d\n", processed);
        }
        
        int size = SBUF_SIZE;
        ALuint buffer;
        
        while(size) {
            if (processed-- <= 0) {
                stream();
                alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
                continue;
            }
            
            alSourceUnqueueBuffers(source, 1, &buffer);
            alBufferData(buffer, AL_FORMAT_STEREO16, sbuf.fin, SBUF_SIZE*2*2, SAMPLE_RATE);
            alSourceQueueBuffers(source, 1, &buffer);
            
            size -= SBUF_SIZE;
        }
        stream();
        
        // Clear
        memset(&sbuf.temp, 0, sizeof(sbuf.temp));
    }
}

void CstrAudio::voiceOn(uh data) {
    for (int n = 0; n < MAX_CHANNELS; n++) {
        if (data & (1 << n)) {
            spuVoices[n].on    = true;
            spuVoices[n].count = 0;
            spuVoices[n].pos   = 0;
            spuVoices[n].raddr = 0;
            spuVoices[n].size  = 0;
            
            depackVAG(&spuVoices[n]);
        }
    }
}

void CstrAudio::voiceOff(uh data) {
    for (int n = 0; n < MAX_CHANNELS; n++) {
        if (data & (1 << n)) {
            spuVoices[n].on = false;
        }
    }
}

void CstrAudio::write(uw addr, uh data) {
    // Switch to low order bits
    addr = LO_BITS(addr);
    
    spuAcc(addr) = data;
    
    // Channels
    if (addr >= 0x1c00 && addr <= 0x1d7e) {
        ub n = spuChannel(addr);
        
        switch(addr & 0xf) {
            case 0x0: // Volume L
                spuVoices[n].volumeL = data & MAX_VOLUME;
                return;
                
            case 0x2: // Volume R
                spuVoices[n].volumeR = data & MAX_VOLUME;
                return;
                
            case 0x4: // Pitch
                spuVoices[n].freq = MAX((data * SAMPLE_RATE) / 4096, 1);
                return;
                
            case 0x6: // Sound Address
                spuVoices[n].saddr = data << 3;
                return;
                
            case 0xe: // Return Address
                spuVoices[n].raddr = data << 3;
                return;
                
            /* unused */
            case 0x8:
            case 0xa:
            case 0xc:
                return;
        }
        
        printx("PSeudo /// SPU write phase $%x <- $%x", addr, data);
    }
    
    // Reverb
    if (addr >= 0x1dc0 && addr <= 0x1dfe) {
        return;
    }
    
    // HW
    switch(addr) {
        case 0x1d80: // Volume L
            spuVolumeL = data & MAX_VOLUME;
            return;
        
        case 0x1d82: // Volume R
            spuVolumeR = data & MAX_VOLUME;
            return;
        
        case 0x1d88: // Sound On 1
            voiceOn(data);
            return;
        
        case 0x1d8a: // Sound On 2
            voiceOn(data << 16);
            return;
        
        case 0x1d8c: // Sound Off 1
            voiceOff(data);
            return;
        
        case 0x1d8e: // Sound Off 2
            voiceOff(data << 16);
            return;
        
        case 0x1da6: // Transfer Address
            spuAddr = data << 3;
            return;
            
        case 0x1da8: // Data
            spuMem.ish[spuAddr >> 1] = data;
            spuAddr += 2;
            spuAddr &= 0x3ffff;
            return;
        
        case 0x1daa: // Control
            return;
        
        case 0x1d84: // Reverb Volume L
        case 0x1d86: // Reverb Volume R
        case 0x1d90: // FM Mode On 1
        case 0x1d92: // FM Mode On 2
        case 0x1d94: // Noise Mode On 1
        case 0x1d96: // Noise Mode On 2
        case 0x1d98: // Reverb Mode On 1
        case 0x1d9a: // Reverb Mode On 2
        case 0x1d9c: // Mute 1
        case 0x1d9e: // Mute 2
        case 0x1da2: // Reverb Address
        case 0x1dac:
        case 0x1db0: // CD Volume L
        case 0x1db2: // CD Volume R
        case 0x1db4:
        case 0x1db6:
            return;
    }
    
    printx("PSeudo /// SPU write: $%x <- $%x", addr, data);
}

uh CstrAudio::read(uw addr) {
    // Switch to low order bits
    addr = LO_BITS(addr);
    
    // Channels
    if (addr >= 0x1c00 && addr <= 0x1d7e) {
        switch(addr & 0xf) {
            case 0x0:
            case 0x2:
            case 0x4:
            case 0x6:
            case 0x8:
            case 0xa:
            case 0xc:
            case 0xe:
                return spuAcc(addr);
        }
        
        printx("PSeudo /// SPU read phase: $%x", (addr & 0xf));
    }
    
    // HW
    switch(addr) {
        case 0x1da6: // Transfer Address
            return spuAddr >> 3;
            
        case 0x1d88: // Sound On 1
        case 0x1d8a: // Sound On 2
        case 0x1d8c: // Sound Off 1
        case 0x1d8e: // Sound Off 2
        case 0x1daa: // Control
        case 0x1dac: // ?
        case 0x1dae: // Status
        case 0x1e00:
        case 0x1e02:
        case 0x1e04:
        case 0x1e06:
        case 0x1e08:
        case 0x1e0a:
        case 0x1e0c:
        case 0x1e0e:
            return spuAcc(addr);
    }
    
    printx("PSeudo /// SPU read: $%x", addr);
    return 0;
}

void CstrAudio::dataWrite(uw addr, uw size) {
    while(size-- > 0) {
        spuMem.ish[spuAddr >> 1] = accessMem(mem.ram, uh); addr += 2;
        spuAddr += 2;
        spuAddr &= 0x3ffff;
    }
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    sw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
    
    switch(dma->chcr) {
        case 0x01000201: // Write DMA Mem
            dataWrite(dma->madr, size);
            return;
            
//        case 0x01000200:
//            dataMem.read(madr, size);
//            return;
    }
    
    printx("PSeudo /// SPU DMA: $%x", dma->chcr);
}
