#ifndef MYSPU

class CstrAudio {
//    enum {
//        SAMPLE_RATE    =  44100,
//        MAX_CHANNELS   =     24,
//        SBUF_SIZE      =    512,
//        MAX_VOLUME     = 0x3fff,
//        ALC_BUF_AMOUNT =     16,
//    };
    
    enum {
        SPU_ALC_BUF_AMOUNT   = 16,
        SPU_CHANNELS         = 24,
        SPU_MAX_VOLUME       = 0x3fff,
        SPU_SAMPLE_RATE      = 44100,
        SPU_SAMPLE_SIZE      = 256,
    };
    
    const sh f[5][2] = {
        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
    };
    
    struct voice {
        bool on, create;
        sw bfr[32], sbpos, pos, s_1, s_2, bIgnoreLoop, sinc, freq, used, pitch;
        ub *p, *saddr, *raddr;
        sw volumeL, volumeR;
    } spuVoices[SPU_CHANNELS + 1];
    
    ub *ptr;
    uh spuMem[256 * 1024];
    uw spuAddr;
    sh sbuf[SPU_SAMPLE_SIZE];
    
    void voiceOn(uw);
    
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[SPU_ALC_BUF_AMOUNT];
    
public:
    CstrAudio() {
        ptr = (ub *)spuMem;
        
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
            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
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
