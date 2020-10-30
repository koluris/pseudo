class CstrCounters {
    enum class ResetToZero: uh { whenFFFF = 0, whenTarget = 1 };
    enum class IrqRepeatMode: uh { oneShot = 0, repeatedly = 1 };
    enum class IrqPulseMode: uh { shortPulse = 0, toggle = 1 };
    enum class ClockSource0: uh { systemClock = 0, dotClock = 1 };
    enum class ClockSource1: uh { systemClock = 0, hblank = 1 };
    enum class ClockSource2: uh { systemClock = 0, systemClock_8 = 1 };
    
    bool oneShotIrqOccured;
    
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
            
            uh data;
        } mode;
        uh target;
        uw cnt;
    } timer[3];
    
    void reset();
    void step(ub, uw);
    uh read(uw);
    void write(uw, uh);
};

extern CstrCounters rootc;
