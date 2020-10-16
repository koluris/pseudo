
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
    
    ub control;
    ub status;
    ub re2;
    ub statP;
    
    bool occupied, reads, readed;
    
    uw irq;
    uw interruptSet;
    
    struct {
      ub p, c;
    } param;
    
    struct {
      ub data[8], p, c, ok;
    } res;
    
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
