class CstrAudio {
    struct vol {
        uw l, r;
    };
    
    union hi {
        uh u16[1024 * 256];
        ub u08[1024 * 256 * 2];
    };
    
    union hi2 {
        sh s16[USHRT_MAX];
        ub u08[USHRT_MAX * 2];
    };
    
    struct voice {
        hi2 buffer;
        uw count, freq, pos, raddr, saddr, size;
        bool on;
        vol volume;
    };
    
    hi spuMem;
    voice spuVoices[24];
    uh spuAddr, spuVolumeL, spuVolumeR;
    
    void depackVAG(voice);
    void voiceOn(uh);
    void voiceOff(uh);
    void dataWrite(uw, uw);
    
public:
    CstrAudio() {
        // OpenAL
        //        ALCdevice *device;
        //        ALCcontext *ctx;
        //
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
        //
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
