class CstrMips {
    const uw mask[4][4] = {
        { 0x00ffffff, 0x0000ffff, 0x000000ff, 0x00000000 },
        { 0x00000000, 0xff000000, 0xffff0000, 0xffffff00 },
        { 0xffffff00, 0xffff0000, 0xff000000, 0x00000000 },
        { 0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff },
    };
    
    const ub shift[4][4] = {
        { 0x18, 0x10, 0x08, 0x00 },
        { 0x00, 0x08, 0x10, 0x18 },
        { 0x18, 0x10, 0x08, 0x00 },
        { 0x00, 0x08, 0x10, 0x18 },
    };
    
    union { // Product & quotient (lo, hi)
        sd u64; uw u32[2];
    } res;
    
    uw opcodeCount;
    
    void step(bool);
    void branch(uw);
    void exception(uw, bool);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
    void bootstrap();
    void run();
};

extern CstrMips cpu;
