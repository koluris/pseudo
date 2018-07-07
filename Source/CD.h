
class CstrCD {
    enum {
        CD_STAT_PLAY    = 0x80,
        CD_STAT_STANDBY = 0x02,
    };
    
    struct {
        ub ok, ptr, size, data[8];
    } res, param;
    
    ub check, mode, status, state, order;
    
public:
    void reset();
    
    // Store
    void write(uw, ub);
    
    // Load
    ub read(uw);
};

extern CstrCD cd;

/*
class CstrCD {
    enum {
        CD_CTRL_BUSY   = 0x80,
        CD_CTRL_PH     = 0x10,
        CD_CTRL_NP     = 0x08,
        CD_CTRL_MODE_1 = 0x02,
        CD_CTRL_MODE_0 = 0x01,
    };
    
    enum {
        CD_STAT_STANDBY = 0x02
    };
    
    enum {
        CD_CMD_TYPE_BLOCKING,
        CD_CMD_TYPE_NON_BLOCKING
    };
    
    struct {
        ub *res[8], *par[8];
    } ebus;
    
    struct {
        ub status, queued;
    } interrupt;
    
    ub check, status, kind;
    
    void exec(ub);
    
public:
    void reset();
    
    // Store
    void write(uw, ub);
    
    // Load
    ub read(uw);
};

extern CstrCD cd;*/
