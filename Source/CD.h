
class CstrCD {
    enum {
        CD_STAT_NO_INTR,
        CD_STAT_ACKNOWLEDGE = 3
    };
    
    enum {
        CdlNop = 1
    };
    
    ub control;
    ub status;
    ub readed;
    ub re2;
    
    bool occupied;
    
    uw irq;
    uw interruptSet;
    
    struct {
      ub p, c;
    } param;
    
    struct {
      ub p, c, ok;
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
