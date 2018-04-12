// Basic
TYPEDEF struct { ub c, m, k, n; } COLOR;
TYPEDEF struct { sh w, h; } POINTF;
TYPEDEF struct { sh w, h; ub u, v; uh clut; } POINTFT;
TYPEDEF struct { COLOR co; sh w, h; } POINTG;
TYPEDEF struct { COLOR co; sh w, h; ub u, v; uh clut; } POINTGT;

// VertexF & LineF
TYPEDEF struct { COLOR co; POINTF v[2]; } F2;
TYPEDEF struct { COLOR co; POINTF v[3]; } F3;
TYPEDEF struct { COLOR co; POINTF v[4]; } F4;

// VertexFT
TYPEDEF struct { COLOR co; POINTFT v[3]; } FT3;
TYPEDEF struct { COLOR co; POINTFT v[4]; } FT4;

// VertexG & LineG
TYPEDEF struct { POINTG v[2]; } G2;
TYPEDEF struct { POINTG v[3]; } G3;
TYPEDEF struct { POINTG v[4]; } G4;

// VertexGT
TYPEDEF struct { POINTGT v[3]; } GT3;
TYPEDEF struct { POINTGT v[4]; } GT4;

// BlockFill & Sprites
TYPEDEF struct { COLOR co; POINTF  v[1]; sh w, h; } BLK;
TYPEDEF struct { COLOR co; POINTFT v[1]; sh w, h; } SPRT;

class CstrGraphics {
    struct {
        uw data, status;
    } ret;
    
    uh resMode[8] = {
        256, 320, 512, 640, 368, 384, 512, 640
    };
    
    // Command buffer
    struct {
        uw data[100], prim, size, row;
    } pipe;
    
    // Primitive size
    ub pSize[256] = {
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
    
    uw modeDMA;
    
    enum {
        GPU_DMA_NONE,
        GPU_DMA_MEM2VRAM = 2,
        GPU_DMA_VRAM2MEM,
    };
    
    enum {
        GPU_REG_DATA,
        GPU_REG_STATUS = 4
    };
    
public:
    void reset();
    void resize(uh, uh);
    void redraw();
    
    // Store
    void dataWrite(uw *, sw);
    void write(uw, uw);
    
    // Load
    uw read(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrGraphics vs;
