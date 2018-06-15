#define accessMem(mem, dataT) \
    *(dataT *)&mem.ptr[addr & (mem.size - 1)]


class CstrMem {
    enum {
        // This mask unifies the RAM mirrors (0, 8, A) into one unique case
        MEM_MASK = 0x00ffffff,
        
        MEM_BOUNDS_RAM = 0xf0800000 & MEM_MASK,
        MEM_BOUNDS_SCR = 0x1f800400 & MEM_MASK,
        MEM_BOUNDS_HWR = 0x1f804000 & MEM_MASK,
        MEM_BOUNDS_ROM = 0xbfc80000 & MEM_MASK,
    };
    
public:
    struct heap {
        ub *ptr; uw size;
    };
    
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
    
    // Store
    void write32(uw, uw);
    void write16(uw, uh);
    void write08(uw, ub);
    
    // Load
    uw read32(uw);
    uh read16(uw);
    ub read08(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrMem mem;
