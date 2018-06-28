#import "Global.h"

#ifndef MYSPU


#define spuAcc(addr) \
    *(uh *)&mem.hwr.ptr[addr]

#define spuChannel(addr) \
    (addr >> 4) & 0x1f


CstrAudio audio;

void CstrAudio::reset() {
    spuAddr = 0xffffffff;
    
    // Channels reset
    for (auto &item : spuVoices) {
        item = { 0 };
        item.p = item.saddr = item.raddr = ptr;
    }
}

void CstrAudio::voiceOn(uw data) {
    for (int n = 0; n < MAX_CHANNELS; n++) {
        if (data & (1 << n)) {
            spuVoices[n].create = true;
            spuVoices[n].bIgnoreLoop = 0;
        }
    }
}

int setVolume(sh data) {
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

void CstrAudio::write(uw addr, uh data) {
    // Switch to low order bits
    addr = LO_BITS(addr);
    
    spuAcc(addr) = data;
    
    // Channels
	if (addr >= 0x1c00 && addr < 0x1d7e) {
		ub n = spuChannel(addr);
		
        switch(addr & 0xf) {
            case 0x0: // Volume L
                spuVoices[n].volumeL = setVolume(data);
                return;
            
            case 0x2: // Volume R
                spuVoices[n].volumeR = setVolume(data);
                return;
            
            case 0x4: // Pitch
                spuVoices[n].pitch = data;
                spuVoices[n].freq  = (44100 * data) / 4096;
                return;
            
            case 0x6: // Sound Address
                spuVoices[n].saddr = ptr + (data << 3);
                return;
            
            case 0xe: // Return Address
                spuVoices[n].raddr = ptr + (data << 3);
                spuVoices[n].bIgnoreLoop = 1;
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
        case 0x1d8c: // Sound Off 1
        case 0x1d8e: // Sound Off 2
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

void CstrAudio::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

#define audioSet(a, b) \
    rest = (*chn.p & a) << b; \
    if (rest & 0x8000) rest |= 0xffff0000; \
    fa = rest >> shift; \
    fa += ((s_1 * f[predict][0] + s_2 * f[predict][1] + 32) >> 6); \
    s_2 = s_1; \
    s_1 = fa; \
    chn.bfr[n++] = fa

void CstrAudio::decodeStream() {
    sh rest;
    sw s_1, s_2, fa;
    
    while(!psx.suspended) {
        sw temp[SBUF_SIZE] = { 0 };
        
        // Clear
        memset(&sbuf, 0, sizeof(sbuf));
        
        for (auto &chn : spuVoices) {
            NEXT_CHANNEL:
            
            if (chn.create) {
                chn.on      = true;
                chn.create  = false;
                chn.p       = chn.saddr;
                chn.pos     = 0x10000;
                chn.sbpos   = 28;
                chn.s_1     = 0;
                chn.s_2     = 0;
                chn.bfr[29] = 0;
                chn.bfr[30] = 0;
                chn.bfr[31] = 0;
            }
            
            // Channel on?
            if (chn.on == false) {
                continue;
            }
            
            chn.sinc = chn.pitch << 4; // * 16 bits
            
            if (chn.sinc == 0) {
                chn.sinc  = 1;
            }
            
            for (int i = 0; i < SBUF_SIZE; i += 2) {
                while(chn.pos >= 0x10000) {
                    if (chn.sbpos == 28) {
                        if (chn.p == (ub *)-1) {
                            chn.on = false;
                            redirect NEXT_CHANNEL;
                        }
                        
                        chn.sbpos = 0;
                        s_1 = chn.s_1;
                        s_2 = chn.s_2;
                        
                        ub shift   = *chn.p & 0xf;
                        ub predict = *chn.p++ >> 4;
                        ub op      = *chn.p++;

                        for (int n = 0; n < 28; chn.p++) {
                            audioSet(0x0f, 0xc);
                            audioSet(0xf0, 0x8);
                        }
                        
                        if ((op & 4) && (!chn.bIgnoreLoop)) {
                            chn.raddr = chn.p - 16;
                        }
                        
                        if (op & 1) {
                            if (op != 3 || chn.raddr == NULL) {
                                chn.p = (ub *)-1;
                            }
                            else {
                                chn.p = chn.raddr;
                            }
                        }
                        chn.s_1 = s_1;
                        chn.s_2 = s_2;
                    }
                    
                    fa = chn.bfr[chn.sbpos++];
                    chn.bfr[29] = fa;
                    chn.pos -= 0x10000;
                }
                
                fa = chn.bfr[29];

                temp[i + 0] += (+fa * chn.volumeL) / MAX_VOLUME;
                temp[i + 1] += (-fa * chn.volumeR) / MAX_VOLUME;
                
                chn.pos += chn.sinc;
            }
        }
        
        for (int i = 0; i < SBUF_SIZE; i += 2) {
            sbuf[i + 0] = +(temp[i + 0] / 4);
            sbuf[i + 1] = -(temp[i + 1] / 4);
        }
        
        // OpenAL
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        
        if (processed >= ALC_BUF_AMOUNT) {
            // We have to free buffers
            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
        }
        
        while(--processed < 0) {
            stream();
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        }
        
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SBUF_SIZE*2, SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
        stream();
    }
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
    
    switch(dma->chcr) {
        case 0x01000201:
            for (uw i = 0; i < size; i++) {
                spuMem[spuAddr >> 1] = *p++;
                spuAddr += 2;
                spuAddr &= 0x7ffff;
            }
            return;
            
        case 0x01000200:
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
