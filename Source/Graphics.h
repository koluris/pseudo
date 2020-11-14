#define nullclutP (1024 * 512 + 1)

extern uh lutBGR2RGB[65536];
extern uh textBuf[];

extern sw GPUstatusRet;
extern sw GPUInfoVals[16];

#define INFO_TW        0
#define INFO_DRAWSTART 1
#define INFO_DRAWEND   2
#define INFO_DRAWOFF   3

#define GPUSTATUS_DMABITS             0x60000000
#define GPUSTATUS_READYFORCOMMANDS    0x10000000
#define GPUSTATUS_IDLE                0x04000000
#define GPUSTATUS_DISPLAYDISABLED     0x00800000
#define GPUSTATUS_RGB24               0x00200000
#define GPUSTATUS_PAL                 0x00100000

extern ub psxVub[];
extern uh *psxVuw;
extern uw *psxVul;

typedef struct {
    uw x, y;
    uw colormode;
    uw abr;
    uw mirror;
} TextureState_t;

extern TextureState_t texinfo;

typedef struct VRAMLOADTTAG
{
    sh x;
    sh y;
    sh w;
    sh h;
    sh curx;
    sh cury;
    uw *extratarget;
    bool enabled;
} VRAMLoad_t;

extern VRAMLoad_t vramWrite;

struct PSXDisplay_t {
    sh modeX;
    sh modeY;
    sh startX;
    sh startY;
    bool disabled;
    bool colordepth24;
    bool pal;
    bool changed;
};
extern struct PSXDisplay_t psxDisp, oldpsxDisp;

struct PSXDraw_t {
    sh offsetX;
    sh offsetY;
    sh clipX1;
    sh clipX2;
    sh clipY1;
    sh clipY2;
    sh texwinX1;
    sh texwinY1;
    sh texwinX2;
    sh texwinY2;
    bool texwinenabled;
    bool enabled;
    bool setmask;
    bool testmask;
};

extern struct PSXDraw_t psxDraw;

extern sh dispWidths[8];

extern sw dispLace;
extern sw dispLaceNew;
extern sw imageTransfer;
extern sw drawingLines;

extern sh imTYc,imTXc,imTY,imTX;
extern sw imSize;
extern sh imageX0,imageX1;
extern sh imageY0,imageY1;

union uPointers{
    void *v;
    ub *b;
    uh *w;
    uw *d;
    sb *c;
    sh *s;
    sw *l;
};

extern sh windowX, windowY;

extern ub texshade[256];
extern uw image[65536];
extern uw torgba[65536];

struct texturecache{
    sw textAddrX;
    sw textAddrY;
    sw textTP;
    sw clutP;
    GLuint id;
    bool Update;
};

extern struct texturecache texture[384];
extern GLuint xferTexture16;
extern GLuint xferTexture24;
extern GLuint nullid;

extern void GPUinit();
extern void GPUopen();
extern void GPUupdateLace();
extern uw GPUreadStatus();
extern void GPUwriteStatus(uw gdata);
extern uw GPUreadData();
extern void GPUwriteData(uw gdata);
extern void executeDMA(CstrBus::castDMA *dma);
