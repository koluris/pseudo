class CstrMem {
    struct heap {
        ub *ptr; uw size;
    };
    
public:
    heap ram, rom, hwr;
    
    CstrMem() {
        ram.ptr = new ub[ram.size = 0x200000];
        rom.ptr = new ub[rom.size = 0x80000];
        hwr.ptr = new ub[hwr.size = 0x4000];
    }
    
    ~CstrMem() {
        delete[] ram.ptr;
        delete[] rom.ptr;
        delete[] hwr.ptr;
    }
    
    void reset();
    uw read32(uw);
};

extern CstrMem mem;
