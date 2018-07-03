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
    
    struct heap {
        ub *ptr; uw size;
    };
    
public:
    heap ram, rom, hwr;
    
    heap tst;
    
    CstrMem() {
        ram.ptr = new ub[ram.size = 0x200000];
        rom.ptr = new ub[rom.size = 0x80000];
        hwr.ptr = new ub[hwr.size = 0x4000];
        
        tst.ptr = new ub[tst.size = 0x80000];
    }
    
    ~CstrMem() {
        delete[] ram.ptr;
        delete[] rom.ptr;
        delete[] hwr.ptr;
        
        delete[] tst.ptr;
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
