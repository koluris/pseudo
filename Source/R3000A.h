struct CstrMips {
    uw base[34], pc, lo, hi;
    
    void reset();
    void step();
};

extern CstrMips cpu;
