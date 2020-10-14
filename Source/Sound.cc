/* Base structure taken from SOPE open source emulator, and improved upon (Credits: SaD, Linuzappz) */

#include "Global.h"


#define SPU_CHANNEL(addr) \
    (addr >> 4) & 0x1f


CstrAudio audio;

void CstrAudio::reset() {
    freeBuffers();
    
    memset(&spuMem, 0, sizeof(spuMem));
    spuAddr = 0xffffffff;
    
    for (auto &item : spuVoices) {
        item = { 0 };
    }
}

sh CstrAudio::setVolume(sh data) {
    return ((data & 0x7fff) ^ 0x4000) - 0x4000;
}

void CstrAudio::voiceOn(uw data) {
    for (int n = 0; n < SPU_MAX_CHAN; n++) {
        if (data & (1 << n) && spuVoices[n].saddr) {
            spuVoices[n].isNew  = true;
            spuVoices[n].repeat = false;
        }
    }
}

#define audioSet(a, b) \
    rest = (*ch.paddr & a) << b; \
    if (rest & 0x8000) rest |= 0xffff0000; \
    rest = (rest >> shift) + ((ch.s[0] * f[predict][0] + ch.s[1] * f[predict][1] + 32) >> 6); \
    ch.s[1] = ch.s[0]; \
    ch.s[0] = MIN(MAX(rest, SHRT_MIN), SHRT_MAX); \
    ch.bfr[i++] = ch.s[0]

void CstrAudio::decodeStream() {
    while(!psx.suspended) {
        memset(&sbuf, 0, SPU_SAMPLE_SIZE);
        
        for (int n = 0; n < SPU_MAX_CHAN; n++) {
            auto &ch = spuVoices[n];
            
            if (ch.isNew) {
                ch.paddr  = ch.saddr;
                ch.spos   = 0x10000;
                ch.bpos   = 28;
                ch.sample = 0;
                ch.s[0]   = 0;
                ch.s[1]   = 0;
                
                ch.isNew  = false;
                ch.active = true;
            }
            
            if (ch.active == false) {
                continue;
            }
            
            for (int ns = 0; ns < SPU_SAMPLE_COUNT; ns++) {
                for (; ch.spos >= 0x10000; ch.spos -= 0x10000) {
                    if (ch.bpos == 28) {
                        if (ch.paddr == (ub *)-1) {
                            ch.active = false;
                            redirect SPU_NEXT_CHANNEL;
                        }
                        
                        ch.bpos = 0;
                        ub shift   = *ch.paddr & 0xf;
                        ub predict = *ch.paddr++ >> 4;
                        ub op      = *ch.paddr++;
                        
                        for (int i = 0, rest; i < 28; ch.paddr++) {
                            audioSet(0x0f, 0xc);
                            audioSet(0xf0, 0x8);
                        }
                        
                        if ((op & 4) && (!ch.repeat)) {
                            ch.raddr = ch.paddr - 16;
                        }
                        
                        if ((op & 1)) {
                            ch.paddr = (op != 3 || ch.raddr == NULL) ? (ub *)-1 : ch.raddr;
                        }
                    }
                    
                    ch.sample = ch.bfr[ch.bpos++] >> 2;
                }
                
                sbuf[(ns * 2) + 0] += (ch.sample * ch.volumeL) >> 14;
                sbuf[(ns * 2) + 1] += (ch.sample * ch.volumeR) >> 14;
                
                ch.spos += ch.freq;
            }
            
            SPU_NEXT_CHANNEL:
                continue;
        }
        
        // OpenAL
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        
        if (processed >= SPU_ALC_BUF_AMOUNT) {
            // We have to free buffers
            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
        }
        
        while(--processed < 0) {
            freeBuffers();
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        }
        
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE, SPU_SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
    }
}

void CstrAudio::freeBuffers() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void CstrAudio::write(uw addr, uh data) {
    switch(LOW_BITS(addr)) {
        case 0x1c00 ... 0x1d7e: // Channels
            {
                ub ch = SPU_CHANNEL(addr);
                
                switch(addr & 0xf) {
                    case 0x0: // Volume L
                        spuVoices[ch].volumeL = setVolume(data);
                        return;
                        
                    case 0x2: // Volume R
                        spuVoices[ch].volumeR = setVolume(data);
                        return;
                        
                    case 0x4: // Pitch
                        spuVoices[ch].freq = MIN(data, 0x3fff) << 4;
                        return;
                        
                    case 0x6: // Sound Address
                        spuVoices[ch].saddr = spuMemC + (data << 3);
                        return;
                        
                    case 0xe: // Return Address
                        spuVoices[ch].raddr = spuMemC + (data << 3);
                        spuVoices[ch].repeat = true;
                        return;
                        
                    /* unused */
                    case 0x8:
                    case 0xa:
                    case 0xc:
                        accessMem(mem.hwr, uh) = data;
                        return;
                }
                
                printx("/// PSeudo SPU write phase: 0x%x <- 0x%x", (addr & 0xf), data);
                return;
            }
            
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
        case 0x1dc0 ... 0x1dfe: // Reverb
            accessMem(mem.hwr, uh) = data;
            return;
    }
    
    printx("/// PSeudo SPU write: 0x%x <- 0x%x", addr, data);
}

uh CstrAudio::read(uw addr) {
    switch(LOW_BITS(addr)) {
        case 0x1c00 ... 0x1d7e: // Channels
            {
                ub ch = SPU_CHANNEL(addr);
                
                switch(addr & 0xf) {
                    case 0xc: // Hack
                        if (spuVoices[ch].isNew) {
                            return 1;
                        }
                        return 0;
                        
                    case 0xe: // Madman
                        if (spuVoices[ch].raddr) {
                            return (spuVoices[ch].raddr - spuMemC) >> 3;
                        }
                        return 0;
                        
                    /* unused */
                    case 0x0:
                    case 0x2:
                    case 0x4:
                    case 0x6:
                    case 0x8:
                    case 0xa:
                        return accessMem(mem.hwr, uh);
                }
                
                printx("/// PSeudo SPU read phase: 0x%x", (addr & 0xf));
                return 0;
            }
            
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
        case 0x1e00 ... 0x1e0e: // ?
            return accessMem(mem.hwr, uh);
    }
    
    printx("/// PSeudo SPU read: 0x%x", addr);
    return 0;
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    uh *p = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
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
    
    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
}
