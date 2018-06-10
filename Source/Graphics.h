//#define GPU_DITHER           0x00000200
//#define GPU_DRAWINGALLOWED   0x00000400
//#define GPU_MASKDRAWN        0x00000800
//#define GPU_MASKENABLED      0x00001000
//#define GPU_WIDTHBITS        0x00070000
#define GPU_DOUBLEHEIGHT     0x00080000
//#define GPU_PAL              0x00100000
//#define GPU_RGB24            0x00200000
//#define GPU_INTERLACED       0x00400000
#define GPU_DISPLAYDISABLED  0x00800000
#define GPU_IDLE             0x04000000
#define GPU_READYFORVRAM     0x08000000
#define GPU_READYFORCOMMANDS 0x10000000
#define GPU_DMABITS          0x60000000
#define GPU_ODDLINES         0x80000000

#define FRAME_W \
    1024

#define FRAME_H \
    512

class CstrGraphics {
    enum {
        GPU_DMA_NONE,
        GPU_DMA_MEM2VRAM = 2,
        GPU_DMA_VRAM2MEM
    };
    
    enum {
        GPU_REG_DATA,
        GPU_REG_STATUS = 4
    };
    
    // Width
    const uh resMode[8] = {
        256, 320, 512, 640, 368, 384, 512, 640
    };
    
    // Primitive size
    const ub pSize[256] = {
        0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10
        4, 4, 4, 4, 7, 7, 7, 7, 5, 5, 5, 5, 9, 9, 9, 9, // 0x20
        6, 6, 6, 6, 9, 9, 9, 9, 8, 8, 8, 8,12,12,12,12, // 0x30
        3, 3, 3, 3, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6, // 0x40
        4, 4, 4, 4, 0, 0, 0, 0, 7, 7, 7, 7, 9, 9, 9, 9, // 0x50
        3, 3, 3, 3, 4, 4, 4, 4, 2, 2, 2, 2, 0, 0, 0, 0, // 0x60
        2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, // 0x70
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xa0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xb0
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xc0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xd0
        0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xe0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xf0
    };
    
    // VRAM Structure
    struct heap {
        uh *ptr; uw size;
    };
    
    // Command buffer
    struct {
        uw data[100], prim, size, row;
    } pipe;
    
    uw modeDMA;
    uh vpos, vdiff;
    
    int fetchMem(uh *, sw);
    void dataWrite(uw *, sw);
    
public:
    CstrGraphics() {
        vram.ptr = new uh[vram.size = ((FRAME_W * FRAME_H) * sizeof(uh))];
    }
    
    ~CstrGraphics() {
        delete[] vram.ptr;
    }
    
    // Data, Status
    struct {
        uw data, status;
    } ret;
    
    // VRAM operations
    struct {
        bool enabled;
        
        struct {
            sw p, start, end;
        } h, v;
    } vrop;
    
    // VRAM
    heap vram;
    
    void reset();
    void write(uw, uw);
    uw read(uw);
    void photoRead(uw *);
    void executeDMA(CstrBus::castDMA *);
};

extern CstrGraphics vs;
