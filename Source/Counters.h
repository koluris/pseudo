class CstrCounters {
    enum class SyncMode0: uh {
        pauseDuringHblank = 0,
        resetAtHblank = 1,
        resetAtHblankAndPauseOutside = 2,
        pauseUntilHblankAndFreerun = 3
    };
    enum class SyncMode1: uh {
        pauseDuringVblank = 0,
        resetAtVblank = 1,
        resetAtVblankAndPauseOutside = 2,
        pauseUntilVblankAndFreerun = 3
    };
    enum class SyncMode2: uh {
        stopCounter = 0,
        freeRun = 1,
        freeRun_ = 2,
        stopCounter_ = 3
    };

    enum class ResetToZero: uh { whenFFFF = 0, whenTarget = 1 };
    enum class IrqRepeatMode: uh { oneShot = 0, repeatedly = 1 };
    enum class IrqPulseMode: uh { shortPulse = 0, toggle = 1 };
    enum class ClockSource0: uh { systemClock = 0, dotClock = 1 };
    enum class ClockSource1: uh { systemClock = 0, hblank = 1 };
    enum class ClockSource2: uh { systemClock = 0, systemClock_8 = 1 };
    
    bool oneShotIrqOccured = false;
    
public:
    struct {
        uh current;
        union {
            struct {
                uh : 1;
                uh : 2;
                ResetToZero resetToZero : 1;
                uh irqWhenTarget : 1;
                uh irqWhenFFFF : 1;
                IrqRepeatMode irqRepeatMode : 1;
                IrqPulseMode irqPulseMode : 1;
                uh clockSource : 2;
                uh interruptRequest : 1;
                uh reachedTarget : 1;
                uh reachedFFFF : 1;
                uh : 3;
            };
            
            uh _reg;
        } mode;
        uh target;
        bool paused;
        uw cnt;
    } timer[3];
    
    void reset();
    void step(int, int);
    uh read(uw);
    void write(uw, uh);
};

extern CstrCounters rootc;
