#define SBUF_SIZE\
    512


class CstrAudio {
    typedef struct {
        sh l, r;
    } vol;
    
    typedef union {
        sh u16[1024 * 256];
        ub u08[1024 * 256 * 2];
    } hi;
    
    typedef union {
        sh s16[USHRT_MAX];
        ub u08[USHRT_MAX * 2];
    } hi2;
    
    typedef struct {
        hi2 buffer;
        uh freq, raddr, saddr;
        uw count;
        int pos, size;
        bool on;
        vol volume;
    } voice;
    
    typedef struct {
        sw temp[SBUF_SIZE * 2];
        sh fin[SBUF_SIZE * 2];
    } strSbuf;
    
    strSbuf sbuf;
    hi spuMem;
    voice spuVoices[24];
    sh spuVolumeL, spuVolumeR;
    sw spuAddr;
    bool stereo;
    
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    
#define bufnum 4
    ALuint bfr[bufnum];
    
    void stream();
    void depackVAG(voice *);
    void voiceOn(uh);
    void voiceOff(uh);
    void dataWrite(uw, uw);
    
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
        alGenBuffers(bufnum, bfr);
        
        for (int i=0; i<bufnum; i++) {
            alBufferData(bfr[i], AL_FORMAT_MONO16, sbuf.fin, SBUF_SIZE*2, 44100);
        }
        
        alSourceQueueBuffers(source, bufnum, bfr);
        
//        alDeleteSources(1, &source);
//        alDeleteBuffers(1, &buffer);
//        device = alcGetContextsDevice(context);
//        alcMakeContextCurrent(NULL);
//        alcDestroyContext(context);
//        alcCloseDevice(device);
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
