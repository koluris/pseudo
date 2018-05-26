#define dpcr\
    *(uw *)&mem.hwr.ptr[0x10f0]

#define dicr\
    *(uw *)&mem.hwr.ptr[0x10f4]


class CstrBus {
    enum {
        DMA_MDEC_IN,
        DMA_MDEC_OUT,
        DMA_GPU,
        DMA_CD,
        DMA_SPU,
        DMA_PARALLEL,
        DMA_CLEAR_OT
    };
    
public:
    struct castDMA {
        uw madr, bcr, chcr;
    };
    
    void checkDMA(uw, uw);
};

extern CstrBus bus;
