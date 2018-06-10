#define dpcr \
    *(uw *)&mem.hwr.ptr[0x10f0]

#define dicr \
    *(uw *)&mem.hwr.ptr[0x10f4]


class CstrBus {
    enum {
        INT_DISABLED,
        INT_ENABLED,
        
        INT_TOTAL = 11
    };
    
    enum {
        /* 0 */ DMA_MDEC_IN,
        /* 1 */ DMA_MDEC_OUT,
        /* 2 */ DMA_GPU,
        /* 3 */ DMA_CD,
        /* 4 */ DMA_SPU,
        /* 5 */ DMA_PARALLEL,
        /* 6 */ DMA_CLEAR_OT
    };
    
    struct interrupt {
        const uw code, dest; ub queued;
    } interrupts[INT_TOTAL] = {
        { INT_VSYNC, 1 },
        { INT_GPU,   1 },
        { INT_CD,    4 },
        { INT_DMA,   1 },
        { INT_RTC0,  1 },
        { INT_RTC1,  1 },
        { INT_RTC2,  1 },
        { INT_SIO0,  8 },
        { INT_SIO1,  8 },
        { INT_SPU,   1 },
        { INT_PIO,   1 },
    };
    
public:
    enum {
        INT_VSYNC,
        INT_GPU,
        INT_CD,
        INT_DMA,
        INT_RTC0,
        INT_RTC1,
        INT_RTC2,
        INT_SIO0,
        INT_SIO1,
        INT_SPU,
        INT_PIO
    };
    
    struct castDMA {
        uw madr, bcr, chcr;
    };
    
    void reset();
    void interruptsUpdate();
    void interruptSet(ub);
    void checkDMA(uw, uw);
};

extern CstrBus bus;
