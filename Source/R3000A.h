class CstrMips {
    bool stop;
    
    void branch(uw);
    void step(bool);
    void exception(uw);
    
public:
    uw base[34], copr[16], pc;
    
    void reset();
    void run();
};

extern CstrMips cpu;
