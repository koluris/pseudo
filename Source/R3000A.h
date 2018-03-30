class CstrMips {
    bool stop;
    uw opcodeCount;
    
    void branch(uw);
    void step(bool);
    void exception(uw, bool);
    
public:
    uw base[34], copr[16], pc;
    
    void reset();
    void run();
};

extern CstrMips cpu;
