#define pcr\
    *(uw *)&mem.hwr.ptr[0x10f0]

#define icr\
    *(uw *)&mem.hwr.ptr[0x10f4]

//#define madr\
//    *(uw *)&mem.hwr.ptr[(addr & 0xfff0) | 0]
//
//#define bcr\
//    *(uw *)&mem.hwr.ptr[(addr & 0xfff0) | 4]
//
//#define chcr\
//    *(uw *)&mem.hwr.ptr[(addr & 0xfff0) | 8]


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
