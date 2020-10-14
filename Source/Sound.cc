/* Base structure taken from SOPE open source emulator, and improved upon (Credits: SaD, Linuzappz) */

#include "Global.h"

CstrAudio audio;

uh spuMem[256 * 1024];
ub *spuMemC;
uw spuAddr;
int freqModulation[44100];

#define spuCtrl \
    *(uh *)&mem.hwr.ptr[0x1daa]

void spuFranReadDMAMem(uh *pusPSXMem, int size) {
    if (spuAddr + (size << 1) >= 0x80000) {
        memcpy(pusPSXMem, &spuMem[spuAddr >> 1], 0x7ffff - spuAddr + 1);
        memcpy(pusPSXMem + (0x7ffff - spuAddr + 1), spuMem, (size << 1) - (0x7ffff - spuAddr + 1));
        spuAddr = (size << 1) - (0x7ffff - spuAddr + 1);
    } else {
        memcpy(pusPSXMem, &spuMem[spuAddr >> 1], (size << 1));
        spuAddr += (size << 1);
    }
}

void spuFranWriteDMAMem(uh *pusPSXMem, int size) {
    if (spuAddr + (size << 1) > 0x7ffff) {
        memcpy(&spuMem[spuAddr >> 1], pusPSXMem, 0x7ffff - spuAddr + 1);
        memcpy(spuMem, pusPSXMem + (0x7ffff - spuAddr + 1), (size << 1) - (0x7ffff - spuAddr + 1));
        spuAddr = (size << 1) - (0x7ffff - spuAddr + 1);
    } else {
        memcpy(&spuMem[spuAddr >> 1], pusPSXMem, (size << 1));
        spuAddr += (size << 1);
    }
}

void CstrAudio::SoundOn(int start, int end, uh data) {
    for (int ch = start; ch < end; ch++, data >>= 1) {
        if ((data & 1) && spuVoices[ch].pStart) {
            spuVoices[ch].bIgnoreLoop = 0;
            spuVoices[ch].bNew = 1;
        }
    }
}

void CstrAudio::SoundOff(int start,int end, uh data) {
    for (int ch = start; ch < end; ch++, data >>= 1) {
        spuVoices[ch].bStop |= (data & 1);
    }
}

void CstrAudio::FModOn(int start, int end, uh data) {
    for (int ch = start; ch < end; ch++, data >>= 1) {
        if (data & 1) {
            if (ch > 0) {
                spuVoices[ch].bFMod = 1;
                spuVoices[ch - 1].bFMod = 2;
            }
        } else {
            spuVoices[ch].bFMod = 0;
        }
    }
}

void CstrAudio::NoiseOn(int start, int end, uh data) {
    for (int ch = start; ch < end; ch++, data >>= 1) {
        spuVoices[ch].bNoise = data & 1;
    }
}

int calcVolume(sh vol) {
    if (vol & 0x8000) {
        int sInc = 1;
        
        if (vol & 0x2000) {
            sInc = -1;
        }
        
        if (vol & 0x1000) {
            vol ^= 0xffff;
        }
        
        vol = ((vol & 0x7f) + 1) / 2;
        vol += vol / (2 * sInc);
        vol *= 128;
    } else {
        if (vol & 0x4000) {
            vol = 0x3fff - (vol & 0x3fff);
        }
    }
    
    vol &= 0x3fff;
    return vol;
}

void CstrAudio::setPitch(int ch, int val) {
    val = MIN(val, 0x3fff);
    spuVoices[ch].iRawPitch = val;
    val = (44100 * val) / 4096;
    if (val < 1) {
        val = 1;
    }
    spuVoices[ch].iActFreq = val;
}

void CstrAudio::write(uw addr, uh data) {
    switch(LOW_BITS(addr)) {
        case 0x1c00 ... 0x1d7e: // Channels
            {
                int ch = (addr >> 4) & 0x1f;

                switch(addr & 0xf) {
                    case 0x0: // Volume L
                        spuVoices[ch].volumeL = calcVolume(data);
                        return;

                    case 0x2: // Volume R
                        spuVoices[ch].volumeR = calcVolume(data);
                        return;

                    case 0x4: // Pitch
                        setPitch(ch, data);
                        return;

                    case 0x6: // Sound Address
                        spuVoices[ch].pStart = spuMemC + (data<<3);
                        return;

                    case 0xe: // Return Address
                        spuVoices[ch].pLoop = spuMemC + (data<<3);
                        spuVoices[ch].bIgnoreLoop = 1;
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
            SoundOn(0,16,data);
            return;

        case 0x1d8a: // Sound On 2
            SoundOn(16,24,data);
            return;

        case 0x1d8c: // Sound Off 1
            SoundOff(0,16,data);
            return;

        case 0x1d8e: // Sound Off 2
            SoundOff(16,24,data);
            return;
            
        case 0x1d90: // FM Mode On 1
            FModOn(0,16,data);
            return;
            
        case 0x1d92: // FM Mode On 2
            FModOn(16,24,data);
            return;
            
        case 0x1d94: // Noise Mode On 1
            NoiseOn(0,16,data);
            return;
            
        case 0x1d96: // Noise Mode On 2
            NoiseOn(16,24,data);
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
                int ch = (addr >> 4) & 0x1f;

                switch(addr & 0xf) {
                    case 0xc: // Hack
                        if (spuVoices[ch].bNew) {
                            return 1;
                        }
                        return 0;
                        
                    case 0xe: // Madman
                        if (spuVoices[ch].pLoop) {
                            return (spuVoices[ch].pLoop - spuMemC) >> 3;
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
        case 0x1e00:
        case 0x1e02:
        case 0x1e04:
        case 0x1e06:
        case 0x1e08:
        case 0x1e0a:
        case 0x1e0c:
        case 0x1e0e:
            return accessMem(mem.hwr, uh);
    }

    printx("/// PSeudo SPU read: 0x%x", addr);
    return 0;
}

void CstrAudio::StartSound(int n) {
    auto &chn = spuVoices[n];
    
    chn.pCurr = chn.pStart;
    chn.s_1 = 0;
    chn.s_2 = 0;
    chn.iSBPos = 28;
    chn.bNew = 0;
    chn.bStop = 0;
    chn.bOn = 1;
    chn.SB[29] = 0;
    chn.SB[30] = 0;
    chn.spos = 0x10000;
    chn.SB[31] = 0;
}

void CstrAudio::VoiceChangeFrequency(int n) {
    auto &chn = spuVoices[n];
    
    chn.iUsedFreq = chn.iActFreq;
    chn.sinc = chn.iRawPitch << 4;
    
    if (!chn.sinc) {
        chn.sinc = 1;
    }
}

void CstrAudio::FModChangeFrequency(int n, int ns) {
    auto &chn = spuVoices[n];
    
    int NP = chn.iRawPitch;
    NP = ((32768 + freqModulation[ns]) * NP) / 32768;
    
    if (NP > 0x3fff) {
        NP = 0x3fff;
    }
    
    if (NP < 0x1) {
        NP = 0x1;
    }
    
    NP = (44100 * NP) / 4096;
    chn.iActFreq = NP;
    chn.iUsedFreq = NP;
    chn.sinc = ((NP / 10) << 16) / 4410;
    
    if (!chn.sinc) {
        chn.sinc = 1;
    }
    
    freqModulation[ns] = 0;
}

void CstrAudio::StoreInterpolationVal(int n, int fa) {
    auto &chn = spuVoices[n];
    
    if (chn.bFMod == 2) {
        chn.SB[29] = fa;
    }
    else {
        if ((spuCtrl & 0x4000) == 0) {
            fa = 0;
        }
        else {
            if (fa > 32767) {
                fa = 32767;
            }
            
            if (fa < -32767) {
                fa = -32767;
            }
        }
        
        chn.SB[29] = fa;
    }
}

void CstrAudio::init() {
    spuMemC = (ub *)spuMem;
    
    for (int i = 0; i < (MAXCHAN + 1); i++) {
        spuVoices[i].pLoop = spuMemC;
        spuVoices[i].pStart = spuMemC;
        spuVoices[i].pCurr = spuMemC;
    }
    
    memset(freqModulation, 0, sizeof(freqModulation));
}

void CstrAudio::reset() {
    spuAddr = 0xffffffff;
    for (auto &item : spuVoices) {
        item = { 0 };
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
        int s_1, s_2, fa;
        int predict_nr, shift_factor, flags, s;
        
        memset(&sbuf, 0, SPU_SAMPLE_SIZE);
        
        for (int n = 0; n < (MAXCHAN + 1); n++) {
            auto &ch = spuVoices[n];
            
            if (ch.bNew) {
                StartSound(n);
            }
            
            if (!ch.bOn) {
                continue;
            }
            
            if (ch.iActFreq != ch.iUsedFreq) {
                VoiceChangeFrequency(n);
            }
            
            for (int ns = 0; ns < SPU_SAMPLE_COUNT; ns++) {
                if (ch.bFMod == 1 && freqModulation[ns]) {
                    FModChangeFrequency(n, ns);
                }
                
                while (ch.spos >= 0x10000) {
                    if (ch.iSBPos == 28) {
                        if (ch.pCurr == (ub *)-1)
                        {
                            ch.bOn = 0;
                            goto SPU_CHANNEL_END;
                        }
                        
                        ch.iSBPos = 0;
                        s_1 = ch.s_1;
                        s_2 = ch.s_2;
                        predict_nr = (int)*ch.pCurr;
                        ch.pCurr++;
                        shift_factor = predict_nr & 0xf;
                        predict_nr >>= 4;
                        flags = (int)*ch.pCurr;
                        ch.pCurr++;

                        for (int i = 0; i < 28; ch.pCurr++) {
                            s = ((((int)*ch.pCurr) & 0x0f) << 12);
                            if (s & 0x8000) s |= 0xffff0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            ch.SB[i++] = fa;
                            
                            s = ((((int)*ch.pCurr) & 0xf0) << 8);
                            if (s & 0x8000) s |= 0xffff0000;
                            fa = s >> shift_factor;
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            ch.SB[i++] = fa;
                        }

                        if ((flags & 4) && (!ch.bIgnoreLoop)) {
                            ch.pLoop = ch.pCurr-16;
                        }
                        
                        if (flags & 1) {
                            if (flags != 3 || ch.pLoop == NULL) {
                                ch.pCurr = (ub *) - 1;
                            }
                            else {
                                ch.pCurr = ch.pLoop;
                            }
                        }
                        
                        ch.s_1 = s_1;
                        ch.s_2 = s_2;
                    }
                    
                    fa = ch.SB[ch.iSBPos++];
                    StoreInterpolationVal(n, fa);
                    ch.spos -= 0x10000;
                }
                
                if (ch.bNoise) {
                    fa = 0;
                }
                else {
                    fa = ch.SB[29];
                }
                
                ch.sval = fa >> 2;
                
                if (ch.bFMod == 2) {
                    freqModulation[ns] = ch.sval;
                }
                else {
                    sbuf[(ns * 2) + 0] += (ch.sval * ch.volumeL) >> 14;
                    sbuf[(ns * 2) + 1] += (ch.sval * ch.volumeR) >> 14;
                }
                
                ch.spos += ch.sinc;
            }
            
            SPU_CHANNEL_END: ;
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
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE, SPU_SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
        stream();
    }
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;

    switch(dma->chcr) {
        case 0x01000201: // Write
            spuFranWriteDMAMem(p, size);
            return;

        case 0x01000200: // Read
            spuFranReadDMAMem(p, size);
            return;
    }

    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
}

//void CstrAudio::reset() {
//    // Mem
//    memset(&spuMem, 0, sizeof(spuMem));
//    spuAddr = 0xffffffff;
//
//    // Channels reset
//    for (auto &item : spuVoices) {
//        item = { 0 };
//    }
//}
//
//sh CstrAudio::setVolume(sh data) { // Sweep mode
//    return ((data & 0x7fff) ^ 0x4000) - 0x4000;
//}
//
//#define audioSet(a, b) \
//    rest = (*p & a) << b; \
//    if (rest & 0x8000) rest |= 0xffff0000; \
//    rest = (rest >> shift) + ((s[0] * f[predict][0] + s[1] * f[predict][1] + 32) >> 6); \
//    s[1] = s[0]; \
//    s[0] = MIN(MAX(rest, SHRT_MIN), SHRT_MAX); \
//    chn.bfr[chn.size++] = s[0]
//
//void CstrAudio::voiceOn(uw data) {
//    for (int n = 0; n < SPU_CHANNELS; n++) {
//        if (data & (1 << n)) {
//            auto &chn = spuVoices[n];
//            chn.count = 0;
//            chn.pos   = 0;
//            chn.raddr = 0;
//            chn.size  = 0;
//
//            { // Depack VAG
//                ub *p = (ub *)&spuMem[chn.saddr >> 1];
//
//                static sw s[2] = {
//                    0,
//                    0,
//                };
//
//                for(; chn.size < SPU_CHANNEL_BUF_SIZE - 28;) {
//                    ub shift   = *p & 0xf;
//                    ub predict = *p++ >> 4;
//                    ub op      = *p++;
//
//                    for (int i = chn.size, rest; chn.size < i + 28; p++) {
//                        audioSet(0x0f, 0xc);
//                        audioSet(0xf0, 0x8);
//                    }
//
//                    switch(op) {
//                        case 3: // End
//                        case 7:
//                            redirect NEXT_CHANNEL;
//
//                        case 6: // Repeat
//                            chn.raddr = chn.size;
//                    }
//                }
//
//                printf("/// PSeudo SPU Channel size overflow\n");
//            }
//        }
//        NEXT_CHANNEL: ;
//    }
//}
//
//void CstrAudio::voiceOff(uw data) {
//    for (int n = 0; n < SPU_CHANNELS; n++) {
//        if (data & (1 << n)) {
//        }
//    }
//}
//
//void CstrAudio::stream() {
//    ALint state;
//    alGetSourcei(source, AL_SOURCE_STATE, &state);
//
//    if (state != AL_PLAYING) {
//        alSourceStream(source);
//    }
//}
//
//void CstrAudio::decodeStream() {
//    while(!psx.suspended) {
//        sw temp[SPU_SAMPLE_SIZE] = { 0 };
//
//        // Clear
//        memset(&sbuf, 0, sizeof(sbuf));
//
//        for (auto &chn : spuVoices) {
//            // Channel on?
//            if (chn.size < 1) {
//                continue;
//            }
//
//            for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
//                chn.count += chn.freq;
//
//                if (chn.count >= SPU_SAMPLE_RATE) {
//                    chn.pos   += chn.count / SPU_SAMPLE_RATE;
//                    chn.count %= SPU_SAMPLE_RATE;
//                }
//
//                // Mix Channel Samples
//                temp[i + 0] += (+chn.bfr[chn.pos] * chn.volumeL) / SPU_MAX_VOLUME;
//                temp[i + 1] += (-chn.bfr[chn.pos] * chn.volumeR) / SPU_MAX_VOLUME;
//
//                // End of Sample
//                if (chn.pos >= chn.size) {
//                    if (chn.raddr > 0) { // Repeat?
//                        chn.pos   = chn.raddr;
//                        chn.count = 0;
//                        continue;
//                    }
//
//                    chn.size = 0;
//                    break;
//                }
//            }
//        }
//
//        // Volume Mix
//        for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
//            sbuf[i + 0] = MIN(MAX(+(temp[i + 0] / 2), SHRT_MIN), SHRT_MAX);
//            sbuf[i + 1] = MIN(MAX(-(temp[i + 1] / 2), SHRT_MIN), SHRT_MAX);
//        }
//
//        // OpenAL
//        ALint processed;
//        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
//
//        if (processed >= SPU_ALC_BUF_AMOUNT) {
//            // We have to free buffers
//            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
//        }
//
//        while(--processed < 0) {
//            stream();
//            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
//        }
//
//        ALuint buffer;
//        alSourceUnqueueBuffers(source, 1, &buffer);
//        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
//        alSourceQueueBuffers(source, 1, &buffer);
//        stream();
//    }
//}
//
//#define spuChannel(addr) \
//    (addr >> 4) & 0x1f
//
//void CstrAudio::write(uw addr, uh data) {
//    switch(LOW_BITS(addr)) {
//        case 0x1c00 ... 0x1d7e: // Channels
//            {
//                ub n = spuChannel(addr);
//
//                switch(addr & 0xf) {
//                    case 0x0: // Volume L
//                        spuVoices[n].volumeL = setVolume(data);
//                        return;
//
//                    case 0x2: // Volume R
//                        spuVoices[n].volumeR = setVolume(data);
//                        return;
//
//                    case 0x4: // Pitch
//                        spuVoices[n].freq = MAX((data * SPU_SAMPLE_RATE) / 4096, 1);
//                        return;
//
//                    case 0x6: // Sound Address
//                        spuVoices[n].saddr = data << 3;
//                        return;
//
//                    case 0xe: // Return Address
//                        spuVoices[n].raddr = data << 3;
//                        return;
//
//                    /* unused */
//                    case 0x8:
//                    case 0xa:
//                    case 0xc:
//                        accessMem(mem.hwr, uh) = data;
//                        return;
//                }
//
//                printx("/// PSeudo SPU write phase: 0x%x <- 0x%x", (addr & 0xf), data);
//                return;
//            }
//
//        case 0x1d88: // Sound On 1
//            voiceOn(data);
//            return;
//
//        case 0x1d8a: // Sound On 2
//            voiceOn(data << 16);
//            return;
//
//        case 0x1d8c: // Sound Off 1
//            voiceOff(data);
//            return;
//
//        case 0x1d8e: // Sound Off 2
//            voiceOff(data << 16);
//            return;
//
//        case 0x1da6: // Transfer Address
//            spuAddr = data << 3;
//            return;
//
//        case 0x1da8: // Data
//            spuMem[spuAddr >> 1] = data;
//            spuAddr += 2;
//            spuAddr &= 0x7ffff;
//            return;
//
//        /* unused */
//        case 0x1d80: // Volume L
//        case 0x1d82: // Volume R
//        case 0x1d84: // Reverb Volume L
//        case 0x1d86: // Reverb Volume R
//        case 0x1d90: // FM Mode On 1
//        case 0x1d92: // FM Mode On 2
//        case 0x1d94: // Noise Mode On 1
//        case 0x1d96: // Noise Mode On 2
//        case 0x1d98: // Reverb Mode On 1
//        case 0x1d9a: // Reverb Mode On 2
//        case 0x1d9c: // Mute 1
//        case 0x1d9e: // Mute 2
//        case 0x1daa: // Control
//        case 0x1da2: // Reverb Address
//        case 0x1dac: // ?
//        case 0x1db0: // CD Volume L
//        case 0x1db2: // CD Volume R
//        case 0x1db4:
//        case 0x1db6:
//        case 0x1dc0 ... 0x1dfe: // Reverb
//            accessMem(mem.hwr, uh) = data;
//            return;
//    }
//
//    printx("/// PSeudo SPU write: 0x%x <- 0x%x", addr, data);
//}
//
//uh CstrAudio::read(uw addr) {
//    switch(LOW_BITS(addr)) {
//        case 0x1c00 ... 0x1d7e: // Channels
//            {
//                ub n = spuChannel(addr);
//
//                switch(addr & 0xf) {
//                    case 0xc: // Hack
//                        if (spuVoices[n].size) {
//                            return 1;
//                        }
//                        return 0;
//
//                    /* unused */
//                    case 0x0:
//                    case 0x2:
//                    case 0x4:
//                    case 0x6:
//                    case 0x8:
//                    case 0xa:
//                    case 0xe: // madman
//                        return accessMem(mem.hwr, uh);
//                }
//
//                printx("/// PSeudo SPU read phase: 0x%x", (addr & 0xf));
//                return 0;
//            }
//
//        case 0x1da6: // Transfer Address
//            return spuAddr >> 3;
//
//        /* unused */
//        case 0x1d88: // Sound On 1
//        case 0x1d8a: // Sound On 2
//        case 0x1d8c: // Sound Off 1
//        case 0x1d8e: // Sound Off 2
//        case 0x1d94: // Noise Mode On 1
//        case 0x1d96: // Noise Mode On 2
//        case 0x1d98: // Reverb Mode On 1
//        case 0x1d9a: // Reverb Mode On 2
//        case 0x1d9c: // Voice Status 0 - 15
//        case 0x1daa: // Control
//        case 0x1dac: // ?
//        case 0x1dae: // Status
//        case 0x1db8:
//        case 0x1dba:
//        case 0x1e00:
//        case 0x1e02:
//        case 0x1e04:
//        case 0x1e06:
//        case 0x1e08:
//        case 0x1e0a:
//        case 0x1e0c:
//        case 0x1e0e:
//            return accessMem(mem.hwr, uh);
//    }
//
//    printx("/// PSeudo SPU read: 0x%x", addr);
//    return 0;
//}
//
//void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
//    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
//    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
//
//    switch(dma->chcr) {
//        case 0x01000201: // Write
//            for (uw i = 0; i < size; i++) {
//                spuMem[spuAddr >> 1] = *p++;
//                spuAddr += 2;
//                spuAddr &= 0x7ffff;
//            }
//            return;
//
//        case 0x01000200: // Read
//            for (uw i = 0; i < size; i++) {
//                *p++ = spuMem[spuAddr >> 1];
//                spuAddr += 2;
//                spuAddr &= 0x7ffff;
//            }
//            return;
//    }
//
//    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
//}
