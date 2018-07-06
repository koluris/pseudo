class CstrCD {
    struct base {
        ub ok, ptr, size, data[8];
    };
    
    ub ctrl, status, order;
    base res, param;
    
public:
    void reset();
    
    // Store
    void write(uw, ub);
    
    // Load
    ub read(uw);
};

extern CstrCD cd;
