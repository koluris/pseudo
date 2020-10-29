struct GPUSync {
    int dotDiv;
    bool hblank;
    bool vblank;
};

class CstrCounters {
    enum class ResetWhen: uw {
        Overflow    = 0,
        Destination = 1,
    };
    
    enum class IRQRepeat: uw {
        OneShot = 0,
        Repeat  = 1,
    };
    
    enum class IRQAlternate: uw {
        Pulse     = 0,
        Alternate = 1,
    };
    
    bool in_hblank, in_vblank;
    bool prev_hblank, prev_vblank;
    uw dot_div = 0;
    
public:
    enum class SyncMode : uw {
        Pause,
        Reset,
        ResetPause,
        PauseFreeRun,
        FreeRun,
        Stop
    };
    
    enum class ClockSrc: uw {
        System,
        SystemDiv8,
        Dotclock,
        Hblank
    };
    
    void reset();
    void tick(uw, int);
    void gpu_sync(GPUSync);
    ClockSrc get_clock_source(int);
    SyncMode get_sync_mode(int);
    
    template <class T>
    T read(uw);
    
    template <class T>
    void write(uw, T);
    
    struct {
        union {
            uw raw;
            
            struct {
                uw value : 16;
                uw       : 16;
            };
        } current;
        
        union {
            uw raw;
            
            struct {
                uw sync_enable : 1;
                uw sync_mode : 2;
                ResetWhen reset : 1;
                uw irq_when_target : 1;
                uw irq_when_overflow : 1;
                IRQRepeat irq_repeat_mode : 1;
                IRQAlternate irq_pulse_mode : 1;
                uw clock_source : 2;
                uw irq_request : 1;
                uw reached_destination : 1;
                uw reached_overflow : 1;
                uw : 19;
            };
        } mode;
        
        union {
            uw raw;
            
            struct {
                uw destination : 16;
                uw             : 16;
            };
        } destination;
        
        bool paused, already_fired_irq;
        
        uw count;
    } timer[3];
};

extern CstrCounters rootc;
