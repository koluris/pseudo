
class CstrCD {
    enum {
        CD_STAT_NO_INTR,
        CD_STAT_COMPLETE = 2,
        CD_STAT_ACKNOWLEDGE = 3
    };
    
    enum {
        CdlNop  = 1,
        CdlInit = 10
    };
    
    struct {
        ub control, status, statp, re2;
    } ret;
    
    struct {
        ub p, c;
    } param;
    
    struct {
        ub data[8], p, c;
        bool done;
    } result;
    
    uw irq;
    uw interruptSet;
    bool occupied, reads, readed;
    
    void interruptQueue(ub);
    void interrupt();
    
public:
    void reset();
    void update();
    
    // Store
    void write(uw, ub);
    
    // Load
    ub read(uw);
};

extern CstrCD cd;
