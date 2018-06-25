#ifndef MYSPU

enum {
    SAMPLE_RATE    =  44100,
    MAX_CHANNELS   =     24,
    SBUF_SIZE      =    256,
    MAX_VOLUME     = 0x3fff,
    ALC_BUF_AMOUNT =     16,
};

class PsxSpuFran {
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[ALC_BUF_AMOUNT];
    
    struct {
        sw temp[SBUF_SIZE * 2];
        sh  fin[SBUF_SIZE * 2];
    } sbuf;
    
public:
    PsxSpuFran() {
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
    
    ~PsxSpuFran() {
        alDeleteSources(1, &source);
        alDeleteBuffers(ALC_BUF_AMOUNT, bfr);
        ALCdevice *device = alcGetContextsDevice(ctx);
        alcMakeContextCurrent(NULL);
        alcCloseDevice(device);
    }
    
	void init();
    void reset();
    void stream();
    void fillBuffer();
};

extern void spuFranWriteRegister(uw reg, uh data);
extern uh spuFranReadRegister(uw reg);
extern void executeDMA(CstrBus::castDMA *dma);

extern PsxSpuFran psxSpuFran;

#endif
