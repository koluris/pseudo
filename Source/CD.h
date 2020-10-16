class CstrCD {
    enum {
        CD_STAT_NO_INTR,
        CD_STAT_COMPLETE = 2,
        CD_STAT_ACKNOWLEDGE = 3
    };
    
    enum {
        READ_ACK = 250
    };
    
    enum {
        CdlNop     =  1,
        CdlSetloc  =  2,
        CdlReadN   =  6,
        CdlInit    = 10,
        CdlDemute  = 12,
        CdlSetmode = 14,
        CdlSeekL   = 21,
    };
    
    struct {
        ub control, status, statp, re2;
    } ret;
    
    struct {
        ub data[8], p, c;
    } param;
    
    struct {
        ub data[8], p, c;
        bool done;
    } result;
    
    struct {
      ub data[4];
    } sector;
    
    uw irq;
    uw interruptSet;
    uw reads;
    bool occupied, readed, seeked;
    
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
