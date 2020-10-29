//class CstrCounters {
//    // Constant enumerations
//    enum {
//        PSX_CLOCK      = 33868800,
//        PSX_VSYNC_NTSC = PSX_CLOCK / 60,
//        PSX_VSYNC_PAL  = PSX_CLOCK / 50,
//        PSX_HSYNC      = PSX_CLOCK / 60 / 480 // ?
//    };
//
//    enum {
//        RTC_COUNT  = 0,
//        RTC_MODE   = 4,
//        RTC_TARGET = 8,
//        RTC_BOUND  = 0xffff
//    };
//
//    uh bounds[3];
//    sw vbk, hbk;
//
//public:
//    void reset();
//    void update(int);
//
//    // Store
//    template <class T>
//    void write(uw, T);
//};
//
//extern CstrCounters rootc;

struct GPUSync {
    int dotDiv;
    bool hblank;
    bool vblank;
};

enum class SyncMode : uint {
    /* For timers 0 and 1. */
    Pause = 0,
    Reset = 1,
    ResetPause = 2,
    PauseFreeRun = 3,

    /* For timer 2. */
    FreeRun = 4,
    Stop = 5
};

enum class ClockSrc : uint {
    System,
    SystemDiv8,
    Dotclock,
    Hblank,
};

enum class ResetWhen : uint {
    Overflow = 0,
    Target = 1
};

enum class IRQRepeat : uint {
    OneShot = 0,
    Repeatedly = 1
};

enum class IRQToggle : uint {
    Pulse = 0,
    Toggle = 1
};

union CounterValue {
    uw raw;

    struct {
        uw value : 16;
        uw : 16;
    };
};

union CounterControl {
    uw raw;

    struct {
        uw sync_enable : 1;
        uw sync_mode : 2;
        ResetWhen reset : 1;
        uw irq_when_target : 1;
        uw irq_when_overflow : 1;
        IRQRepeat irq_repeat_mode : 1;
        IRQToggle irq_pulse_mode : 1;
        uw clock_source : 2;
        uw irq_request : 1;
        uw reached_target : 1;
        uw reached_overflow : 1;
        uw : 19;
    };
};

union CounterTarget {
    uw raw;

    struct {
        uw target : 16;
        uw : 16;
    };
};

class CstrCounters {
public:
    void reset();
    
    /* Trigger an interrupt. */
    void fire_irq(int);
    
    /* Add cycles to the timer. */
    void tick(uw, int);
    void gpu_sync(GPUSync);
    
    /* Map timer to interrupt type. */
    void irq_type(int i);
    
    /* Read/Write to the timer. */
    template <class T>
    T read(uw);
    
    template <class T>
    void write(uw, T);
    
    /* Get the timer's clock source. */
    ClockSrc get_clock_source(int);
    
    /* Get the timer's sync mode. */
    SyncMode get_sync_mode(int);
    
    bool in_hblank, in_vblank;
    bool prev_hblank, prev_vblank;
    uw dot_div = 0;
    
    struct {
        CounterValue current;
        CounterControl mode;
        CounterTarget target;
        
        bool paused, already_fired_irq;
        
        uw count;
    } timers[3];
};

extern CstrCounters rootc;
