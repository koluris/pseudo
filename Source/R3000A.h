struct CstrMips {
    uw base[32], pc, lo, hi, nopCounter;
    
    void reset();
    void branch(uw);
    void step(bool);
};

extern CstrMips cpu;
