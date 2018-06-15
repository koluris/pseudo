#define accessMem(mem, dataT) \
    *(dataT *)&mem.ptr[addr & (mem.size - 1)]


class CstrMem {
    enum {
        MEM_BOUNDS_RAM = 0x00800000,
        MEM_BOUNDS_SCR = 0x00800400,
        MEM_BOUNDS_HWR = 0x00804000,
        MEM_BOUNDS_ROM = 0x00c80000,
        
        // This mask unifies the RAM mirrors (0, 8, A) into one unique case
        MEM_MASK = 0x00ffffff
    };
    
public:
    struct {
        ub *ptr; uw size;
    } ram, rom, hwr;
    
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
