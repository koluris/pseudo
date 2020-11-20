#define FRAME_W \
    1024

#define FRAME_H \
    512

#define NTSC \
    (CLOCKS_PER_SEC / 59.94)

#define PAL \
    (CLOCKS_PER_SEC / 50.00)


class CstrGraphics {
    // Constant enumerations
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
        0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x04,0x04,0x04,0x04,0x07,0x07,0x07,0x07, 0x05,0x05,0x05,0x05,0x09,0x09,0x09,0x09,
        0x06,0x06,0x06,0x06,0x09,0x09,0x09,0x09, 0x08,0x08,0x08,0x08,0x0c,0x0c,0x0c,0x0c,
        0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00, 0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,
        0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04, 0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,
        0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03, 0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,
        0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    };
    
    // Command buffer
    struct {
        uw data[256], prim, size, row;
    } pipe;
    
    // VRAM operations
    struct {
        bool enabled;
        uw *raw, pvram;
        
        struct {
            sw start, end, p;
        } h, v;
    } vrop;
    
    uw modeDMA, clock, scanline, stall;
    uh vpos, vdiff;

    void dataWrite(uw *, sw);
    void dataRead (uw *, sw);
    int fetchMem(uh *, sw);
    
public:
    enum {
        GPU_INFO_TEX_WINDOW = 2,
        GPU_INFO_DRAW_AREA_START,
        GPU_INFO_DRAW_AREA_END,
        GPU_INFO_DRAW_OFFSET,
        GPU_INFO_VERSION = 7,
    };
    
    CstrGraphics() {
        vram.ptr = new uh[vram.size = FRAME_W * FRAME_H * 2];
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
        uw data, status;
    } ret;
    
    uw info[8];
    
    bool isDisabled;
    bool isInterlaced;
    bool isVideo24Bit;
    bool isVideoPAL;
    
    void reset();
    void update(uw);
    void write(uw, uw);
    uw read(uw);
    void photoMove(uw *);
    void photoWrite(uw *);
    void photoRead(uw *);
    void executeDMA(CstrBus::castDMA *);
};

extern CstrGraphics vs;
