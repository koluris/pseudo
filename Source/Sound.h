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
    
    voice spuVoices[24];
    
    uh spuAddr, spuVolumeL, spuVolumeR;
    
    void voiceOn(uh);
    void voiceOff(uh);
    
public:
    void reset();
    
    // Store
    void write(uw, uh);
    
    // Load
    uh read(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrAudio audio;
