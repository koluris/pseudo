#define MAXCHAN 24

class CstrAudio {
    enum {
        SPU_ALC_BUF_AMOUNT = 16,
        SPU_SAMPLE_RATE    = 44100,
        SPU_SAMPLE_SIZE    = 256,
        SPU_SAMPLE_COUNT   = SPU_SAMPLE_SIZE / 4
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
        
    } spuVoices[MAXCHAN + 1];
    
    void voiceOn(uw);
    void voiceOff(uw);
    int setVolume(sh);
    void output();
    void stream();
    
public:
    CstrAudio() {
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
    
    void init();
    void reset();
    void write(uw, uh);
    uh read(uw);
    void executeDMA(CstrBus::castDMA *);
    void decodeStream();
};

extern CstrAudio audio;

//class CstrAudio {
//    enum {
//        SPU_ALC_BUF_AMOUNT   = 16,
//        SPU_CHANNELS         = 24,
//        SPU_CHANNEL_BUF_SIZE = 11 * (USHRT_MAX + 1), // 2 would cover it, but... hs-001
//        SPU_MAX_VOLUME       = 0x3fff,
//        SPU_SAMPLE_RATE      = 44100,
//        SPU_SAMPLE_SIZE      = 512,
//    };
//
//    const sh f[5][2] = {
//        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
//    };
//
//    struct voice {
//        sh bfr[SPU_CHANNEL_BUF_SIZE];
//        sw count, freq, pos, raddr, saddr, size;
//        sh volumeL, volumeR;
//    } spuVoices[SPU_CHANNELS];
//
//    uh spuMem[1024 * 256];
//    uw spuAddr;
//    sh sbuf[SPU_SAMPLE_SIZE];
//
//    sh setVolume(sh);
//    void voiceOn (uw);
//    void voiceOff(uw);
//    void stream();
//
//    // OpenAL
//    ALCdevice *device;
//    ALCcontext *ctx;
//    ALuint source;
//    ALuint bfr[SPU_ALC_BUF_AMOUNT];
//
//public:
//    CstrAudio() {
//        // OpenAL
//        device = alcOpenDevice(NULL);
//
//        if (!device) {
//            printf("ALC Device error\n");
//        }
//
//        ctx = alcCreateContext(device, NULL);
//        alcMakeContextCurrent(ctx);
//
//        if (!ctx) {
//            printf("ALC Context error\n");
//        }
//
//        alGenSources(1, &source);
//        alGenBuffers(SPU_ALC_BUF_AMOUNT, bfr);
//
//        for (auto &item : bfr) {
//            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
//        }
//
//        alSourceQueueBuffers(source, SPU_ALC_BUF_AMOUNT, bfr);
//    }
//
//    ~CstrAudio() {
//        alDeleteSources(1, &source);
//        alDeleteBuffers(SPU_ALC_BUF_AMOUNT, bfr);
//        ALCdevice *device = alcGetContextsDevice(ctx);
//        alcMakeContextCurrent(NULL);
//        alcCloseDevice(device);
//    }
//
//    void reset();
//    void decodeStream();
//
//    // Store
//    void write(uw, uh);
//
//    // Load
//    uh read(uw);
//
//    // DMA
//    void executeDMA(CstrBus::castDMA *);
//};
