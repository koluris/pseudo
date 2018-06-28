#import "Global.h"


#ifndef MYSPU

CstrAudio audio;

#define spuAcc(addr) \
    *(uh *)&mem.hwr.ptr[addr]

#define spuChannel(addr) \
    (addr >> 4) & 0x1f

void CstrAudio::reset() {
    spuAddr = ~(0);
    
    // Channels reset
    for (auto &item : spuVoices) {
        item = { 0 };
        item.p      = spuMemC;
        item.saddr  = spuMemC;
        item.raddr  = spuMemC;
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

void CstrAudio::setPitch(int ch, int val) {
    spuVoices[ch].iRawPitch = val;
    spuVoices[ch].iActFreq = (44100 * val) / 4096;
}

void CstrAudio::write(uw addr, uh data) {
    addr = LO_BITS(addr);
    
    spuAcc(addr) = data;
    
	if (addr >= 0x1c00 && addr < 0x1d80) {
		ub n = spuChannel(addr);
		
        switch(addr & 0xf) {
            case 0:
                spuVoices[n].volumeL = setVolume(data);
                break;
            
            case 2:
                spuVoices[n].volumeR = setVolume(data);
                break;
            
            case 4:
                setPitch(n, data);
                break;
            
            case 6:
                spuVoices[n].saddr = spuMemC + (data << 3);
                break;
            
            case 14:
                spuVoices[n].raddr = spuMemC + (data << 3);
                spuVoices[n].bIgnoreLoop = 1;
                break;
        }
		return;
	}
    
    switch(addr) {
        case 0x1da6:
            spuAddr = data << 3;
            break;
        
        case 0x1da8:
            spuMem[spuAddr >> 1] = data;
            spuAddr += 2;
            spuAddr &= 0x7ffff;
            break;
        
        case 0x1d88:
            voiceOn(data);
            break;
        
        case 0x1d8a:
            voiceOn(data << 16);
            break;
    }
}

uh CstrAudio::read(uw addr) {
    addr = LO_BITS(addr);
	
    if (addr >= 0x1c00 && addr < 0x1d80) {
        return spuAcc(addr);
    }

	switch(addr) {
        case 0x1da6:
            return spuAddr >> 3;
            
        case 0x1da8:
            {
                uh s = spuMem[spuAddr >> 1];
                spuAddr += 2;
                spuAddr &= 0x7ffff;
                return s;
            }
	}
	return spuAcc(addr);
}

void CstrAudio::VoiceChangeFrequency(voice *chn) {
	chn->sinc = chn->iRawPitch << 4;
	
    if (!chn->sinc)
        chn->sinc = 1;
}

void CstrAudio::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void CstrAudio::decodeStream() {
    const sh f[5][2] = {
        {   0,   0 },
        {  60,   0 },
        { 115, -52 },
        {  98, -55 },
        { 122, -60 },
    };
    
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
            
            if (chn.iActFreq != chn.iUsedFreq) {
                VoiceChangeFrequency(&chn);
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
                            rest = (*chn.p & 0x0f) << 12;
                            if (rest & 0x8000) rest |= 0xffff0000;
                            fa = rest >> shift;
                            fa += ((s_1 * f[predict][0] + s_2 * f[predict][1] + 32) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            chn.bfr[n++] = fa;
                            
                            rest = (*chn.p & 0xf0) <<  8;
                            if (rest & 0x8000) rest |= 0xffff0000;
                            fa = rest >> shift;
                            fa += ((s_1 * f[predict][0] + s_2 * f[predict][1] + 32) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            chn.bfr[n++] = fa;
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
