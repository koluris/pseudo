#ifndef MYSPU

class CstrAudio {
    enum {
        SAMPLE_RATE    =  44100,
        MAX_CHANNELS   =     24,
        SBUF_SIZE      =    512,
        MAX_VOLUME     = 0x3fff,
        ALC_BUF_AMOUNT =     16,
    };
    
    const sh f[5][2] = {
        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
    };
    
    struct voice {
        bool on, create;
        sw bfr[32], sbpos, pos, s_1, s_2, bIgnoreLoop, sinc, freq, used, pitch;
        ub *p, *saddr, *raddr;
        sw volumeL, volumeR;
    } spuVoices[MAX_CHANNELS + 1];
    
    uh spuMem[256 * 1024];
    ub *ptr;
    uw spuAddr;
    sh sbuf[SBUF_SIZE];
    
    void voiceOn(uw);
    
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[ALC_BUF_AMOUNT];
    
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
        alGenBuffers(ALC_BUF_AMOUNT, bfr);
        
        for (auto &item : bfr) {
            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SBUF_SIZE*2, SAMPLE_RATE);
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
