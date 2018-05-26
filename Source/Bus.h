#define dpcr\
    *(uw *)&mem.hwr.ptr[0x10f0]

#define dicr\
    *(uw *)&mem.hwr.ptr[0x10f4]


class CstrBus {
    enum {
        IRQ_QUEUED_NO,
        IRQ_QUEUED_YES
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
    } interrupts[11] = {
        { IRQ_VSYNC, 1 },
        { IRQ_GPU,   1 },
        { IRQ_CD,    4 },
        { IRQ_DMA,   1 },
        { IRQ_RTC0,  1 },
        { IRQ_RTC1,  1 },
        { IRQ_RTC2,  1 },
        { IRQ_SIO0,  8 },
        { IRQ_SIO1,  8 },
        { IRQ_SPU,   1 },
        { IRQ_PIO,   1 },
    };
    
public:
    enum {
        IRQ_VSYNC,
        IRQ_GPU,
        IRQ_CD,
        IRQ_DMA,
        IRQ_RTC0,
        IRQ_RTC1,
        IRQ_RTC2,
        IRQ_SIO0,
        IRQ_SIO1,
        IRQ_SPU,
        IRQ_PIO
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
