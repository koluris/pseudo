class CstrSerial {
    ub bfr[5], cnt;
    bool fireInterrupt;
    
public:
    void reset();
    
    // Load
    uh read16();
    ub read08();
};

extern CstrSerial sio;
