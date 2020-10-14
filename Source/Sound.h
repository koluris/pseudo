class CstrAudio {
    enum {
        SPU_ALC_BUF_AMOUNT = 16,
        SPU_SAMPLE_RATE    = 44100,
        SPU_SAMPLE_SIZE    = 512,
        SPU_SAMPLE_COUNT   = SPU_SAMPLE_SIZE / 4,
        SPU_MAX_CHAN       = 24 + 1
    };
    
    const int f[5][2] = {
        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
    };
    
    uh sbuf[SPU_SAMPLE_SIZE];
    uh spuMem[256 * 1024];
    ub *spuMemC;
    uw spuAddr;
    
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[SPU_ALC_BUF_AMOUNT];
    
    struct voices {
        bool on, isNew, endLoop;
        
        // Address
        ub *saddr; // Start
        ub *paddr; // Current
        ub *raddr; // Return
        
        sh volumeL, volumeR;
        sw bpos, spos, freq, sample;
        sw s_1, s_2;
        sw bfr[28];
        
    } spuVoices[SPU_MAX_CHAN];
    
    sh setVolume(sh);
    void voiceOn (uw);
    void voiceOff(uw);
    void output();
    void stream();
    
public:
    CstrAudio() {
        spuMemC = (ub *)spuMem;
        
        for (int i = 0; i < SPU_MAX_CHAN; i++) {
            spuVoices[i].saddr = spuMemC;
            spuVoices[i].raddr = spuMemC;
            spuVoices[i].paddr = spuMemC;
        }
        
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
        alGenBuffers(SPU_ALC_BUF_AMOUNT, bfr);

        for (auto &item : bfr) {
            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE, SPU_SAMPLE_RATE);
        }

        alSourceQueueBuffers(source, SPU_ALC_BUF_AMOUNT, bfr);
    }
    
    ~CstrAudio() {
        alDeleteSources(1, &source);
        alDeleteBuffers(SPU_ALC_BUF_AMOUNT, bfr);
        ALCdevice *device = alcGetContextsDevice(ctx);
        alcMakeContextCurrent(NULL);
        alcCloseDevice(device);
    }
    
    void reset();
    void decodeStream();
    
    // Store
    void write(uw, uh);
    
    // Load
    uh read(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrAudio audio;
