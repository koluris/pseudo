struct CstrMips {
    uw base[32], pc, lo, hi;
    
    void reset();
    void step();
};

extern CstrMips cpu;
