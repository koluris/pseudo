#ifndef MYSPU

class CstrAudio {
    enum {
        SAMPLE_RATE    =  44100,
        MAX_CHANNELS   =     24,
        SBUF_SIZE      =    256,
        MAX_VOLUME     = 0x3fff,
        ALC_BUF_AMOUNT =     16,
    };
    
    struct voice {
        bool on, create;
        sw pos, volumeL, volumeR;
        
        sw sbpos, sinc, bfr[32], iActFreq, iUsedFreq, bIgnoreLoop, iRawPitch, s_1, s_2;
        ub *saddr, *p, *raddr;
    } spuVoices[MAX_CHANNELS + 1];
    
    uh spuMem[256 * 1024];
    ub *spuMemC;
    uw spuAddr;
    sh sbuf[SBUF_SIZE * 2];
    
    void voiceOn (uw);
    void setPitch(int, int);
    void StartSound(voice *);
    void VoiceChangeFrequency(voice *);
    
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
            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SBUF_SIZE*2*2, SAMPLE_RATE);
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
