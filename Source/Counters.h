class CstrCounters {
    enum class ResetToZero: uh {
        onDest   = 1,
        onBounds = 0,
    };
    
    struct {
        // Mode breakdown
        union {
            struct {
                uh                    : 3;
                ResetToZero resetZero : 1;
                uh irqWhenDest        : 1;
                uh irqWhenBounds      : 1;
                uh                    : 2;
                uh clockSource        : 2;
                uh                    : 6;
            };
            
            uh data;
        } mode;
        
        uh current;
        uh bounds;
        uh dest;
        uw temp;
    } timer[3];
    
    // Counters rate
    const uh table[3] = {
        6, 3413, (uh)(8 * 1.5f)
    };
    
public:
    void reset();
    void update(uw);
    void write(uw, uh);
    uh read(uw);
};

extern CstrCounters rootc;
