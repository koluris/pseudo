class CstrMips {
    union orderbits { // Made for multiplication/division
        ud u64; uw u32[2]; // lo, hi
    };
    
    orderbits res;
    bool stop;
    uw opcodeCount;
    
    void branch(uw);
    void step(bool);
    void exception(uw, bool);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
    void run();
};

extern CstrMips cpu;
