class CstrMips {
    uw lo, hi;
    bool stop;
    
    void branch(uw);
    void step(bool);
    void exception(uw);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
    void run();
};

extern CstrMips cpu;
