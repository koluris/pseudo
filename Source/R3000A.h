class CstrMips {
    uw lo, hi;
    
    void branch(uw);
    void step(bool);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
};

extern CstrMips cpu;
