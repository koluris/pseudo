#import "Global.h"


#ifdef MYSPU

#define spuAcc(addr) \
    *(uh *)&mem.hwr.ptr[addr]

#define spuChannel(addr) \
    (addr >> 4) & 0x1f


CstrAudio audio;

void CstrAudio::reset() {
    // Mem
    memset(&spuMem, 0, sizeof(spuMem));
    memset(&  sbuf, 0, sizeof(sbuf));
    
    // Channels reset
    for (auto &item : spuVoices) {
        item = { 0 };
    }
    
    spuAddr = 0xffffffff;
    //bus.interruptSet(CstrBus::INT_SPU);
}

sh CstrAudio::setVolume(sh data) {
    sh ret = data;
    
    if (data & 0x8000) {
        if (data & 0x1000) {
            ret ^= 0xffff;
        }
        ret = ((ret & 0x7f) + 1) / 2;
        ret += ret / (2 * ((data & 0x2000) ? -1 : 1));
        ret *= 128;
    }
    else {
        if (data & 0x4000) {
            ret = 0x3fff - (data & 0x3fff);
        }
    }
    return ret & 0x3fff;
}

#define audioSet(a, b, c) \
    rest = (*p & a) << b; \
    if (rest & 0x8000) rest |= 0xffff0000; \
    temp[i + c] = rest >> shift

void CstrAudio::depackVAG(voice *chn) {
    ub *p = (ub *)&spuMem[chn->saddr / 2];
    sw rest, temp[28] = { 0 };
    
    static sw s[2] = {
        0,
        0,
    };
    
    while(chn->size < SPU_CHANNEL_BUF_SIZE) {
        ub shift   = *p & 15;
        ub predict = *p >> 4;
        
        p += 2;
        
        for (int i = 0; i < 28; i += 2, p++) {
            audioSet(0x0f, 0xc, 0);
            audioSet(0xf0, 0x8, 1);
        }
        
        // Operators
        ub op = *(p - 15);
        
        for (int i = 0; i < 28; i++) {
            rest = temp[i] + ((s[0] * f[predict][0] + s[1] * f[predict][1] + 32) >> 6);
            s[1] = s[0];
            s[0] = rest;
            chn->bfr[chn->size++] = MIN(MAX(rest, SHRT_MIN), SHRT_MAX);
        }
        
        if (op == 6) { // Repeat
            chn->raddr = chn->size;
        }
        
        if (op == 3 || op == 7) { // End
            return;
        }
    }
    
    printf("/// PSeudo SPU Channel size overflow\n");
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
        for (auto &chn : spuVoices) {
            // Channel on?
            if (chn.size <= 28) { // 28 -> static noise?
                continue;
            }
            
            for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
                chn.count += chn.freq;
                
                if (chn.count >= SPU_SAMPLE_RATE) {
                    chn.pos   += chn.count / SPU_SAMPLE_RATE;
                    chn.count %= SPU_SAMPLE_RATE;
                }
                
                // Mix Channel Samples
                sw L = sbuf[i + 0] + (chn.bfr[chn.pos] * chn.volumeL / 4) / SPU_MAX_VOLUME;
                sw R = sbuf[i + 1] - (chn.bfr[chn.pos] * chn.volumeR / 4) / SPU_MAX_VOLUME;
                
                sbuf[i + 0] = MIN(MAX(L, SHRT_MIN), SHRT_MAX);
                sbuf[i + 1] = MIN(MAX(R, SHRT_MIN), SHRT_MAX);
                
                // End of Sample
                if (chn.pos >= chn.size) {
                    if (chn.raddr > 0) { // Repeat?
                        chn.pos   = chn.raddr;
                        chn.count = 0;
                        continue;
                    }
                    //chn.on = false;
                    break;
                }
            }
        }
        
        // OpenAL
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        if (processed >= SPU_ALC_BUF_AMOUNT) {
            // We have to free buffers
            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
        }

        while(--processed < 0) {
            stream();
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        }

        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
        stream();
        
        // Clear
        memset(&sbuf, 0, sizeof(sbuf));
    }
}

void CstrAudio::voiceOn(uw data) {
    for (int n = 0; n < SPU_CHANNELS; n++) {
        if (data & (1 << n)) {
            spuVoices[n].count = 0;
            spuVoices[n].pos   = 0;
            spuVoices[n].raddr = 0;
            spuVoices[n].size  = 0;
            
            depackVAG(&spuVoices[n]);
        }
    }
}

void CstrAudio::voiceOff(uw data) {
    for (int n = 0; n < SPU_CHANNELS; n++) {
        if (data & (1 << n)) {
            //spuVoices[n].on = false;
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
                spuVoices[n].volumeL = setVolume(data);
                return;
                
            case 0x2: // Volume R
                spuVoices[n].volumeR = setVolume(data);
                return;
                
            case 0x4: // Pitch
                spuVoices[n].freq = MAX((data * SPU_SAMPLE_RATE) / 4096, 1);
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
        
        printx("/// PSeudo SPU write phase: $%x <- $%x", addr, data);
    }
    
    // Reverb
    if (addr >= 0x1dc0 && addr <= 0x1dfe) {
        return;
    }
    
    // HW
    switch(addr) {
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
            spuMem[spuAddr >> 1] = data;
            spuAddr += 2;
            spuAddr &= 0x7ffff;
            return;
            
        /* unused */
        case 0x1d80: // Volume L
        case 0x1d82: // Volume R
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
        case 0x1daa: // Control
        case 0x1da2: // Reverb Address
        case 0x1dac: // ?
        case 0x1db0: // CD Volume L
        case 0x1db2: // CD Volume R
        case 0x1db4:
        case 0x1db6:
            return;
    }
    
    printx("/// PSeudo SPU write: $%x <- $%x", addr, data);
}

uh CstrAudio::read(uw addr) {
    // Switch to low order bits
    addr = LO_BITS(addr);
    
    // Channels
    if (addr >= 0x1c00 && addr <= 0x1d7e) {
        switch(addr & 0xf) {
            /* unused */
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
        
        printx("/// PSeudo SPU read phase: $%x", (addr & 0xf));
    }
    
    // HW
    switch(addr) {
        case 0x1da6: // Transfer Address
            return spuAddr >> 3;
            
        /* unused */
        case 0x1d88: // Sound On 1
        case 0x1d8a: // Sound On 2
        case 0x1d8c: // Sound Off 1
        case 0x1d8e: // Sound Off 2
        case 0x1d94: // Noise Mode On 1
        case 0x1d96: // Noise Mode On 2
        case 0x1d98: // Reverb Mode On 1
        case 0x1d9a: // Reverb Mode On 2
        case 0x1d9c: // Voice Status 0 - 15
        case 0x1daa: // Control
        case 0x1dac: // ?
        case 0x1dae: // Status
        case 0x1db8:
        case 0x1dba:
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
    
    printx("/// PSeudo SPU read: $%x", addr);
    return 0;
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
    
    switch(dma->chcr) {
        case 0x01000201: // Write
            for (uw i = 0; i < size; i++) {
                spuMem[spuAddr >> 1] = *p++;
                spuAddr += 2;
                spuAddr &= 0x7ffff;
            }
            return;
            
        case 0x01000200: // Read
            for (uw i = 0; i < size; i++) {
                *p++ = spuMem[spuAddr >> 1];
                spuAddr += 2;
                spuAddr &= 0x7ffff;
            }
            return;
    }
    
    printx("/// PSeudo SPU DMA: $%x", dma->chcr);
}

#endif
