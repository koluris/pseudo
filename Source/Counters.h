class CstrCounters {
    enum class ResetToZero: uh {
        onDest   = 1,
        onBounds = 0,
    };
    
public:
    struct {
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
        uh destination;
        uw temp;
        uh bounds;
    } timer[3];
    
    void reset();
    void update(uw);
    uh read(uw);
    void write(uw, uh);
};

extern CstrCounters rootc;
