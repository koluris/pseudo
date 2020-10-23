#define FRAME_W \
    1024

#define FRAME_H \
    512

class CstrGraphics {
    // Constant enumerations
    enum {
        GPU_REG_DATA,
        GPU_REG_STATUS = 4
    };
    
    enum {
        GPU_STAT_ODDLINES         = 0x80000000,
        GPU_STAT_DMABITS          = 0x60000000,
        GPU_STAT_READYFORCOMMANDS = 0x10000000,
        GPU_STAT_READYFORVRAM     = 0x08000000,
        GPU_STAT_IDLE             = 0x04000000,
        GPU_STAT_DISPLAYDISABLED  = 0x00800000,
        GPU_STAT_INTERLACED       = 0x00400000,
        GPU_STAT_RGB24            = 0x00200000,
        GPU_STAT_PAL              = 0x00100000,
        GPU_STAT_DOUBLEHEIGHT     = 0x00080000,
        GPU_STAT_WIDTHBITS        = 0x00070000,
        GPU_STAT_MASKENABLED      = 0x00001000,
        GPU_STAT_MASKDRAWN        = 0x00000800,
        GPU_STAT_DRAWINGALLOWED   = 0x00000400,
        GPU_STAT_DITHER           = 0x00000200,
    };
    
    enum {
        GPU_DMA_NONE,
        GPU_DMA_MEM2VRAM = 2,
        GPU_DMA_VRAM2MEM
    };
    
    // Width
    const uh resMode[8] = {
        256, 320, 512, 640, 368, 384, 512, 640
    };
    
    // Primitive size
    const ub pSize[256] = {
        0,1,3,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        4,4,4,4,7,7,7,7, 5,5,5,5,9,9,9,9,
        6,6,6,6,9,9,9,9, 8,8,8,8,12,12,12,12,
        3,3,3,3,0,0,0,0, 254,254,254,254,254,254,254,254,
        4,4,4,4,0,0,0,0, 255,255,255,255,255,255,255,255,
        3,3,3,3,4,4,4,4, 2,2,2,2,3,3,3,3,
        2,2,2,2,3,3,3,3, 2,2,2,2,3,3,3,3,
        4,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        3,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        3,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    };
    
    // Command buffer
    struct {
        uw data[256], prim, size, row;
    } pipe;
    
    // VRAM operations
    struct {
        bool enabled;
        uw *raw;
        uh *pvram;
        
        struct {
            sw start, end, p;
        } h, v;
    } vrop;
    
    uw modeDMA;
    uh vpos, vdiff;
    
    void dataWrite(uw *, sw);
    void dataRead (uw *, sw);
    int fetchMem(uh *, sw);
    
public:
    CstrGraphics() {
        vram.ptr = new uh[vram.size = FRAME_W * FRAME_H * 2]; // 2?
    }
    
    ~CstrGraphics() {
        delete[] vram.ptr;
    }
    
    // VRAM
    struct {
        uh *ptr; uw size;
    } vram;
    
    // Data, Status
    struct {
        bool disabled; uw data, status;
    } ret;
    
    bool isVideoPAL;
    bool isVideo24Bit;
    
    void reset();
    void refresh();
    void write(uw, uw);
    uw read(uw);
    void photoMove(uw *);
    void photoWrite(uw *);
    void photoRead(uw *);
    void executeDMA(CstrBus::castDMA *);
};

extern CstrGraphics vs;
