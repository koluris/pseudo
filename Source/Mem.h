#define accessMem(mem, dt)\
    *(dt *)&mem.ptr[addr&(mem.size-1)]

class CstrMem {
    struct heap {
        ub *ptr; uw size;
    };
    
public:
    heap ram, rom, hwr;
    
    CstrMem() {
        ram.ptr = new ub[ram.size = 0x200000];
        rom.ptr = new ub[rom.size = 0x80000];
        hwr.ptr = new ub[hwr.size = 0x2000];
    }
    
    ~CstrMem() {
        delete[] ram.ptr;
        delete[] rom.ptr;
        delete[] hwr.ptr;
    }
    
    void reset();
    
    uw read32(uw); void write32(uw, uw);
};

extern CstrMem mem;
