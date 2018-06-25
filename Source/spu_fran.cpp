#import "Global.h"


#ifndef MYSPU

PsxSpuFran psxSpuFran;

typedef struct {
    sw bNew, iSBPos, spos, sinc, SB[32], sval, bOn, bStop, iActFreq, iUsedFreq, iLeftVolume, bIgnoreLoop, iRightVolume, iRawPitch, s_1, s_2, bNoise, bFMod, iOldNoise;
    ub *pStart, *pCurr, *pLoop;
} SPUCHAN;

uh regArea[10000];
uh spuMem[256 * 1024];
ub *spuMemC;
SPUCHAN s_chan[MAX_CHANNELS+1];

uh spuCtrl = 0;
uh spuStat = 0;
uw spuAddr;
int iFMod[44100];

void PsxSpuFran::init() {
    spuMemC = (ub *)spuMem;
    spuMemC = (ub *)spuMem;
}

void PsxSpuFran::reset() {
    spuAddr = 0xffffffff;
    memset((void *)s_chan, 0, (MAX_CHANNELS + 1) * sizeof(SPUCHAN));
    
    for(int i = 0; i < MAX_CHANNELS; i++) {
        s_chan[i].pLoop  = spuMemC;
        s_chan[i].pStart = spuMemC;
        s_chan[i].pCurr  = spuMemC;
    }
    memset(iFMod, 0, sizeof(iFMod));
}

void SoundOn(int start, int end, uh data) {
	for (int ch = start; ch < end; ch++, data >>= 1) {
		if ((data & 1) && s_chan[ch].pStart) {
			s_chan[ch].bIgnoreLoop = 0;
			s_chan[ch].bNew = 1;
		}
	}
}

void SoundOff(int start,int end, uh data) {
	for (int ch = start; ch < end; ch++, data >>= 1)
		s_chan[ch].bStop |= data & 1;
}

void FModOn(int start, int end, uh data) {
	for (int ch = start; ch < end; ch++, data >>= 1) {
		if (data & 1) {
			if (ch > 0) {
				s_chan[ch].bFMod = 1;
				s_chan[ch - 1].bFMod = 2;
			}
		}
        else {
			s_chan[ch].bFMod = 0;
		}
	}
}

void NoiseOn(int start, int end, uh data) {
	for (int ch = start; ch < end; ch++, data >>= 1)
        s_chan[ch].bNoise = data & 1;
}

int calcVolume(sh data) {
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

void setPitch(int ch, int val) {
	val = MIN(val, 0x3fff);
	s_chan[ch].iRawPitch = val;
	val = (44100 * val) / 4096;
	
    if (val < 1)
        val = 1;
	
    s_chan[ch].iActFreq = val;
}

void spuFranWriteRegister(uw reg, uh data) {
	reg &= 0xfff;
    regArea[(reg - 0xc00) >> 1] = data;
    
	if (reg >= 0x0c00 && reg < 0x0d80) {
		int ch = (reg >> 4) - 0xc0;
		
        switch(reg & 0x0f) {
            case 0:
                s_chan[ch].iLeftVolume = calcVolume(data);
                break;
            
            case 2:
                s_chan[ch].iRightVolume = calcVolume(data);
                break;
            
            case 4:
                setPitch(ch, data);
                break;
            
            case 6:
                s_chan[ch].pStart = spuMemC + (data << 3);
                break;
            
            case 14:
                s_chan[ch].pLoop = spuMemC + (data << 3);
                s_chan[ch].bIgnoreLoop = 1;
                break;
        }
		return;
	}
    
    switch(reg) {
        case 0x0da6:
            spuAddr = data << 3;
            break;
        
        case 0x0da8:
            spuMem[spuAddr >> 1] = data;
            spuAddr += 2;
            spuAddr &= 0x7ffff;
            break;
        
        case 0x0daa:
            spuCtrl = data;
            break;
        
        case 0x0dae:
            spuStat = data & 0xf800;
            break;
        
        case 0x0d88:
            SoundOn(0, 16, data);
            break;
        
        case 0x0d8a:
            SoundOn(16, 24, data);
            break;
        
        case 0x0d8c:
            SoundOff(0, 16, data);
            break;
        
        case 0x0d8e:
            SoundOff(16, 24, data);
            break;
        
        case 0x0d90:
            FModOn(0, 16, data);
            break;
        
        case 0x0d92:
            FModOn(16, 24, data);
            break;
        
        case 0x0d94:
            NoiseOn(0, 16, data);
            break;
        
        case 0x0d96:
            NoiseOn(16, 24, data);
            break;
    }
}

uh spuFranReadRegister(uw reg) {
	reg &= 0xfff;
    
    if (reg >= 0x0c00 && reg < 0x0d80) {
		int ch = (reg >> 4) - 0xc0;
		
        switch(reg & 0x0f) {
            case 12:
                if (s_chan[ch].bNew)
                        return 1;
                
                return 0;
                
            case 14:
                if (!s_chan[ch].pLoop)
                    return 0;
                
                return (s_chan[ch].pLoop - spuMemC) >> 3;
        }
    }

	switch(reg) {
        case 0x0daa:
            return spuCtrl;
            
        case 0x0dae:
            return spuStat;
            
        case 0x0da6:
            return spuAddr >> 3;
            
        case 0x0da8:
            {
                uh s = spuMem[spuAddr >> 1];
                spuAddr += 2;
                spuAddr &= 0x7ffff;
                return s;
            }
	}
	return regArea[(reg - 0xc00) >> 1];
}

void StartSound(SPUCHAN * pChannel) {
	pChannel->pCurr = pChannel->pStart;
	pChannel->s_1 = 0;
	pChannel->s_2 = 0;
	pChannel->iSBPos = 28;
	pChannel->bNew = 0;
	pChannel->bStop = 0;
	pChannel->bOn = 1;
	pChannel->spos = 0x10000;
    pChannel->SB[29] = 0;
    pChannel->SB[30] = 0;
    pChannel->SB[31] = 0;
	
}

void VoiceChangeFrequency(SPUCHAN * pChannel) {
	pChannel->iUsedFreq = pChannel->iActFreq;
	pChannel->sinc = pChannel->iRawPitch << 4;
	
    if (!pChannel->sinc)
        pChannel->sinc = 1;
}

void StoreInterpolationVal(SPUCHAN * pChannel, int fa) {
    if (pChannel->bFMod == 2) {
        pChannel->SB[29] = fa;
    }
	else {
        if ((spuCtrl & 0x4000) == 0) {
			fa = 0;
        }
		else {
            if (fa >  32767) fa =  32767;
            if (fa < -32767) fa = -32767;
        }
		pChannel->SB[29] = fa;
	}
}

void PsxSpuFran::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void PsxSpuFran::fillBuffer() {
    const int f[5][2] = {
        {   0,   0 },
        {  60,   0 },
        { 115, -52 },
        {  98, -55 },
        { 122, -60 },
    };
    
    int s_1, s_2, fa;
    int predict_nr, shift_factor, flags, s;
    
    while(!psx.suspended) {
        for (SPUCHAN *ch = s_chan; ch != s_chan + MAX_CHANNELS; ch++) {
            if (ch->bNew)
                StartSound(ch);
            
            if (!ch->bOn)
                continue;
            
            if (ch->iActFreq != ch->iUsedFreq)
                VoiceChangeFrequency(ch);
            
            for (int ns = 0; ns < SBUF_SIZE; ns++) {
                while(ch->spos >= 0x10000) {
                    if (ch->iSBPos == 28) {
                        if (ch->pCurr == (ub *)-1) {
                            ch->bOn = 0;
                            goto ENDX;
                        }
                        
                        ch->iSBPos = 0;
                        s_1 = ch->s_1;
                        s_2 = ch->s_2;
                        predict_nr = (int)*ch->pCurr;
                        ch->pCurr++;
                        shift_factor = predict_nr & 0xf;
                        predict_nr >>= 4;
                        flags = (int)*ch->pCurr;
                        ch->pCurr++;

                        for (int i = 0; i < 28; ch->pCurr++) {
                            s = ((((int)*ch->pCurr) & 0x0f) << 12);
                            if (s & 0x8000) s |= 0xffff0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            
                            s = ((((int)*ch->pCurr) & 0xf0) <<  8);
                            ch->SB[i++] = fa;
                            if (s & 0x8000) s |= 0xffff0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            
                            ch->SB[i++] = fa;
                        }
                        
                        if ((flags & 4) && (!ch->bIgnoreLoop))
                            ch->pLoop = ch->pCurr - 16;
                        
                        if (flags & 1) {
                            if (flags != 3 || ch->pLoop == NULL)
                                ch->pCurr = (ub *)-1;
                            else
                                ch->pCurr = ch->pLoop;
                        }
                        ch->s_1 = s_1;
                        ch->s_2 = s_2;
                    }
                    fa = ch->SB[ch->iSBPos++];
                    StoreInterpolationVal(ch, fa);
                    ch->spos -= 0x10000;
                }

                if (ch->bNoise) {
                    fa = 0;
                }
                else {
                    fa = ch->SB[29];
                }
                
                ch->sval = fa;
                
                if (ch->bFMod == 2) {
                    iFMod[ns] = ch->sval;
                }
                else {
                    sbuf.temp[ns*2+0] += (+ch->sval * ch->iLeftVolume ) / MAX_VOLUME;
                    sbuf.temp[ns*2+1] += (-ch->sval * ch->iRightVolume) / MAX_VOLUME;
                }
                ch->spos += ch->sinc;
            }
            ENDX: ;
        }
        
        for (int i = 0; i < SBUF_SIZE; i++) {
            sbuf.fin[i*2+0] = (+(sbuf.temp[i*2+0] / 4) * 16383) / MAX_VOLUME;
            sbuf.fin[i*2+1] = (-(sbuf.temp[i*2+1] / 4) * 16383) / MAX_VOLUME;
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

void executeDMA(CstrBus::castDMA *dma) {
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
