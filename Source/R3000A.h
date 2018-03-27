class CstrMips {
    uw lo, hi, nopCounter;
    
    void branch(uw);
    void step(bool);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
};

extern CstrMips cpu;
