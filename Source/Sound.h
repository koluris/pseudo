class CstrAudio {
    struct vol {
        uw l, r;
    };
    
    struct voice {
        uh buffer[USHRT_MAX * 2];
        uw count, freq, pos, raddr, saddr, size;
        bool on;
        vol volume;
    };
    
    union hi {
        uh u16[1024*256];
        ub u08[1024*256*2];
    };
    
    hi spuMem;
    
    voice spuVoices[24];
    
    uh spuAddr, spuVolumeL, spuVolumeR;
    
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    
    void voiceOn(uh);
    void voiceOff(uh);
    void dataWrite(uw, uw);
    
public:
    CstrAudio() {
        // OpenAL
//        device = alcOpenDevice(NULL);
//
//        if (device == NULL) {
//            // Handle Exception
//        }
//
//        ctx = alcCreateContext(device, NULL);
//        alcMakeContextCurrent(ctx);
//        alGetError();
//
//        ALuint source;
//        ALuint bfr;
//
//        alGenSources(1, &source);
//        alGenBuffers(1, &bfr);
//        alBufferData(bfr, AL_FORMAT_MONO8, mem.rom.ptr, 0x80000, 44100);
//
//        alSourcei(source, AL_BUFFER, bfr);
//        alSourcePlay(source);
        
        // cleanup context
//        alDeleteSources(1, &source);
//        alDeleteBuffers(1, &buffer);
//        device = alcGetContextsDevice(context);
//        alcMakeContextCurrent(NULL);
//        alcDestroyContext(context);
//        alcCloseDevice(device);
    }
    
    void reset();
    
    // Store
    void write(uw, uh);
    
    // Load
    uh read(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrAudio audio;
