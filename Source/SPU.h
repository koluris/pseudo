#ifndef MYSPU

class CstrAudio {
    enum {
        SAMPLE_RATE    =  44100,
        MAX_CHANNELS   =     24,
        SBUF_SIZE      =    256,
        MAX_VOLUME     = 0x3fff,
        ALC_BUF_AMOUNT =     16,
    };
    
    struct {
        sw temp[SBUF_SIZE * 2];
        sh  fin[SBUF_SIZE * 2];
    } sbuf;
    
    struct voice {
        bool on;
        sw pos, volumeL, volumeR;
        
        sw bNew, iSBPos, sinc, SB[32], bStop, iActFreq, iUsedFreq, bIgnoreLoop, iRawPitch, s_1, s_2, bNoise, bFMod, iOldNoise;
        ub *pStart, *pCurr, *pLoop;
    } spuVoices[MAX_CHANNELS + 1];
    
    uh regArea[10000];
    uh spuMem[256 * 1024];
    ub *spuMemC;
    uh spuCtrl = 0;
    uh spuStat = 0;
    uw spuAddr;
    int iFMod[44100];
    
    void voiceOn (uh);
    void voiceOff(uh);
    void FModOn  (uh);
    void NoiseOn (uh);
    void setPitch(int, int);
    void StartSound(voice *);
    void VoiceChangeFrequency(voice *);
    void StoreInterpolationVal(voice *, int);
    
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[ALC_BUF_AMOUNT];
    
public:
    CstrAudio() {
        spuMemC = (ub *)spuMem;
        
        // OpenAL
        device = alcOpenDevice(NULL);
        
        if (!device) {
            printf("ALC Device error\n");
        }
        
        ctx = alcCreateContext(device, NULL);
        alcMakeContextCurrent(ctx);
        
        if (!ctx) {
            printf("ALC Context error\n");
        }
        
        alGenSources(1, &source);
        alGenBuffers(ALC_BUF_AMOUNT, bfr);
        
        for (auto &item : bfr) {
            alBufferData(item, AL_FORMAT_STEREO16, sbuf.fin, SBUF_SIZE*2*2, SAMPLE_RATE);
        }
        
        alSourceQueueBuffers(source, ALC_BUF_AMOUNT, bfr);
    }
    
    ~CstrAudio() {
        alDeleteSources(1, &source);
        alDeleteBuffers(ALC_BUF_AMOUNT, bfr);
        ALCdevice *device = alcGetContextsDevice(ctx);
        alcMakeContextCurrent(NULL);
        alcCloseDevice(device);
    }
    
	void reset();
    void stream();
    void decodeStream();
    
    // Store
    void write(uw, uh);
    
    // Load
    uh read(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrAudio audio;

#endif
