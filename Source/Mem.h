#define kBaseAddrUser       0x00000000
#define kBaseAddrScratchpad 0x1f800000
#define kBaseAddrHardware   0x1f801000
#define kBaseAddrUserKSEG0  0x80000000
#define kBaseAddrUserKSEG1  0xa0000000
#define kBaseAddrROM        0xbfc00000

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
    void executeDMA(uw);
};

extern CstrMem mem;
