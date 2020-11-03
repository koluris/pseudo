class CstrAudio {
    enum {
        SPU_SAMPLE_RATE    = 44100,
        SPU_SAMPLE_SIZE    = 512,
        SPU_SAMPLE_COUNT   = SPU_SAMPLE_SIZE / 4,
        SPU_MAX_CHAN       = 24 + 1,
        SPU_ALC_BUF_AMOUNT = 16
    };
    
    const int f[5][2] = {
        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
    };
    
    uh spuMem[256 * 1024];
    uh sbuf[SPU_SAMPLE_SIZE];
    uw spuAddr;
    
    // OpenAL
    ALCdevice *device;
    ALuint source;
    ALuint bfr[SPU_ALC_BUF_AMOUNT];
    
    struct {
        bool isNew, active, repeat;
        sw spos, bpos, freq, sample;
        sh volumeL, volumeR;
        sw bfr[28], s[2];
        
        // Address
        sw saddr; // Start
        sw paddr; // Current
        sw raddr; // Return
    } spuVoices[SPU_MAX_CHAN];
    
    sh setVolume(sh);
    void voiceOn(uw);
    void freeBuffers();
    
public:
    CstrAudio() {
        // OpenAL
        device = alcOpenDevice(0);
        ALCcontext *ctx = alcCreateContext(device, 0);
        alcMakeContextCurrent(ctx);
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
        alcCloseDevice(device);
    }
    
    void reset();
    void decodeStream();
    void write(uw, uh);
    uh read(uw);
    void executeDMA(CstrBus::castDMA *);
};

extern CstrAudio audio;
