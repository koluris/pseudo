class CstrCounters {
    enum class ResetToZero: uh {
        whenFFFF = 0,
        whenTarget = 1
    };
    
public:
    struct {
        uh current;
        union {
            struct {
                uh : 3;
                ResetToZero resetToZero : 1;
                uh irqWhenTarget : 1;
                uh irqWhenFFFF : 1;
                uh : 2;
                uh clockSource : 2;
                uh : 6;
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
