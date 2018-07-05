class CstrCD {
    ub stat;
    int order;
    bool occupied;
    
    struct {
        bool ok;
        ub p, c, data[8];
    } res;
    
    struct {
        ub p, c, data[8];
    } param;
    
public:
    void reset();
    
    // Store
    void write(uw, ub);
    
    // Load
    ub read(uw);
};

extern CstrCD cd;
