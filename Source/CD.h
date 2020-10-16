#define BCD2INT(b) \
    ((b) / 16 * 10 + (b) % 16)

#define INT2BCD(i) \
    ((i) / 10 * 16 + (i) % 10)

class CstrCD {
    enum {
        CD_STAT_NO_INTR     = 0,
        CD_STAT_DATA_READY  = 1,
        CD_STAT_COMPLETE    = 2,
        CD_STAT_ACKNOWLEDGE = 3,
    };
    
    enum {
        READ_ACK = 250
    };
    
    enum {
        CdlNop     =  1,
        CdlSetloc  =  2,
        CdlReadN   =  6,
        CdlPause   =  9,
        CdlInit    = 10,
        CdlDemute  = 12,
        CdlSetmode = 14,
        CdlSeekL   = 21,
    };
    
    struct {
        ub control, status, statp, re2, mode;
    } ret;
    
    struct {
        ub data[8], p, c;
    } param;
    
    struct {
        ub data[8], p, c;
        bool done;
    } result;
    
    struct {
        ub data[4], prev[4];
    } sector;
    
    struct {
        ub data[2352];
        uw p;
    } transfer;
    
    uw irq;
    uw interruptSet;
    uw interruptReadSet;
    uw reads;
    bool occupied, readed, seeked;
    
    void interruptQueue(ub);
    void interrupt();
    void interruptRead();
    void trackRead();
    
public:
    void reset();
    void update();
    
    // Store
    void write(uw, ub);
    
    // Load
    ub read(uw);
};

extern CstrCD cd;
