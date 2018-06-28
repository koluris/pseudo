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
        item.pLoop  = spuMemC;
        item.pStart = spuMemC;
        item.pCurr  = spuMemC;
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
                spuVoices[n].pStart = spuMemC + (data << 3);
                break;
            
            case 14:
                spuVoices[n].pLoop = spuMemC + (data << 3);
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

void CstrAudio::StartSound(voice *chn) {
    chn->on     = true;
    chn->create = false;
    chn->pos    = 0x10000;
    
	chn->pCurr = chn->pStart;
	chn->s_1 = 0;
	chn->s_2 = 0;
    
	chn->iSBPos = 28;
    chn->SB[29] = 0;
    chn->SB[30] = 0;
    chn->SB[31] = 0;
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
    const int f[5][2] = {
        {   0,   0 },
        {  60,   0 },
        { 115, -52 },
        {  98, -55 },
        { 122, -60 },
    };
    
    sh s;
    ub predict_nr, shift_factor, flags;
    int s_1, s_2, fa;
    
    while(!psx.suspended) {
        for (auto &chn : spuVoices) {
            if (chn.create) {
                StartSound(&chn);
            }
            
            // Channel on?
            if (chn.on == false) {
                continue;
            }
            
            if (chn.iActFreq != chn.iUsedFreq)
                VoiceChangeFrequency(&chn);
            
            for (int ns = 0; ns < SBUF_SIZE; ns++) {
                while(chn.pos >= 0x10000) {
                    if (chn.iSBPos == 28) {
                        if (chn.pCurr == (ub *)-1) {
                            chn.on = false;
                            redirect ENDX;
                        }
                        
                        chn.iSBPos = 0;
                        s_1 = chn.s_1;
                        s_2 = chn.s_2;
                        predict_nr = *chn.pCurr;
                        chn.pCurr++;
                        shift_factor = predict_nr & 0xf;
                        predict_nr >>= 4;
                        flags = *chn.pCurr;
                        chn.pCurr++;

                        for (int i = 0; i < 28; chn.pCurr++) {
                            s = (((*chn.pCurr) & 0x0f) << 12);
                            if (s & 0x8000) s |= 0xffff0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            chn.SB[i++] = fa;
                            
                            s = (((*chn.pCurr) & 0xf0) <<  8);
                            if (s & 0x8000) s |= 0xffff0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            chn.SB[i++] = fa;
                        }
                        
                        if ((flags & 4) && (!chn.bIgnoreLoop)) {
                            chn.pLoop = chn.pCurr - 16;
                        }
                        
                        if (flags & 1) {
                            if (flags != 3 || chn.pLoop == NULL) {
                                chn.pCurr = (ub *)-1;
                            }
                            else {
                                chn.pCurr = chn.pLoop;
                            }
                        }
                        chn.s_1 = s_1;
                        chn.s_2 = s_2;
                    }
                    
                    fa = chn.SB[chn.iSBPos++];
                    chn.SB[29] = fa;
                    chn.pos -= 0x10000;
                }
                
                fa = chn.SB[29];

                sbuf.temp[ns*2+0] += (+fa * chn.volumeL) / MAX_VOLUME;
                sbuf.temp[ns*2+1] += (-fa * chn.volumeR) / MAX_VOLUME;
                
                sbuf.fin[ns*2+0] = (+(sbuf.temp[ns*2+0] / 4) * 16383) / MAX_VOLUME;
                sbuf.fin[ns*2+1] = (-(sbuf.temp[ns*2+1] / 4) * 16383) / MAX_VOLUME;
                
                chn.pos += chn.sinc;
            }
            ENDX: ;
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
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf.fin, SBUF_SIZE*2*2, SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
        stream();
        
        // Clear
        sbuf = { 0 };
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
