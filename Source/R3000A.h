struct CstrMips {
    uw base[32], pc, lo, hi, nopCounter;
    
    void reset();
    void step();
};

extern CstrMips cpu;
