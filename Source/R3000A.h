class CstrMips {
    union { // Product & quotient (lo, hi)
        ud u64; uw u32[2];
    } res;
    
    uw opcodeCount;
    bool stopped;
    
    void branch(uw);
    void step(bool);
    void exception(uw, bool);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
    void run();
    void stop();
};

extern CstrMips cpu;
