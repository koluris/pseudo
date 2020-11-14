#define nullclutP (1024 * 512 + 1)

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

class CstrGraphics {
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
    
    typedef struct VRAMLOADTTAG {
        sh x;
        sh y;
        sh w;
        sh h;
        sh curx;
        sh cury;
        uw *extratarget;
        bool enabled;
    } VRAMLoad_t;
    
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
    
    struct texturecache {
        sw textAddrX;
        sw textAddrY;
        sw textTP;
        sw clutP;
        GLuint id;
        bool Update;
    };
    
    typedef struct {
        uw x, y;
        uw colormode;
        uw abr;
        uw mirror;
    } TextureState_t;
    
    struct TransparencySettings {
        sw mode;
        sw src;
        sw dst;
        ub alpha;
        bool change;
    };
    
public:
    struct TransparencySettings TransRate[4] = {
        { GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128, TRUE},
        { GL_FUNC_ADD, GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0, TRUE},
        { GL_FUNC_ADD, GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0, TRUE},
        { GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE,                  64, TRUE},
    };
    
    ub psxVub[1024 * 520 * 2];
    uh *psxVuw;
    uw *psxVul;
    
    TextureState_t texinfo;
    
    uh curText = 0;
    GLuint nullid;

    bool transparent = FALSE;
    ub gAlpha;
    
    ub texshade[256];
    uw image[65536];
    uw torgba[65536];
    
    int flip;
    sw GPUdataRet;
    sw statusRet;
    sw infoVals[16];

    uw gpuData[100];
    ub gpuCommand = 0;
    sw gpuDataC = 0;
    sw gpuDataP = 0;

    sw drawingLines;
    
    sh dispWidths[8] = {256,320,512,640,368,384,512,640};

    sw dispLace = 0;
    sw dispLaceNew;
    sw imageTransfer;

    sh imTYc,imTXc,imTY,imTX;
    sw imSize;
    sh imageX0,imageX1;
    sh imageY0,imageY1;

    GLuint xferTexture16 = 0;
    GLuint xferTexture24 = 0;
    struct texturecache texture[384];
    
    struct PSXDraw_t psxDraw;

    VRAMLoad_t vramWrite;
    struct PSXDisplay_t psxDisp, oldpsxDisp;

    void init();
    void reset();
    void updateLace();
    uw readStatus();
    void writeStatus(uw);
    uw readData();
    void writeData(uw);
    void executeDMA(CstrBus::castDMA *);
    
    void updateScreenMode();
    sw pullFromPsxRam(uw *, sw);
    void readDataMem(uw *, int);
    void writeDataMem(uw *, sw);
};

extern CstrGraphics vs;

//#define nullclutP (1024 * 512 + 1)
//
//extern uh lutBGR2RGB[65536];
//extern uh textBuf[];
//
//extern sw GPUstatusRet;
//extern sw GPUInfoVals[16];
//
//#define INFO_TW        0
//#define INFO_DRAWSTART 1
//#define INFO_DRAWEND   2
//#define INFO_DRAWOFF   3
//
//#define GPUSTATUS_DMABITS             0x60000000
//#define GPUSTATUS_READYFORCOMMANDS    0x10000000
//#define GPUSTATUS_IDLE                0x04000000
//#define GPUSTATUS_DISPLAYDISABLED     0x00800000
//#define GPUSTATUS_RGB24               0x00200000
//#define GPUSTATUS_PAL                 0x00100000
//
//extern ub psxVub[];
//extern uh *psxVuw;
//extern uw *psxVul;
//
//typedef struct {
//    uw x, y;
//    uw colormode;
//    uw abr;
//    uw mirror;
//} TextureState_t;
//
//extern TextureState_t texinfo;
//
//typedef struct VRAMLOADTTAG
//{
//    sh x;
//    sh y;
//    sh w;
//    sh h;
//    sh curx;
//    sh cury;
//    uw *extratarget;
//    bool enabled;
//} VRAMLoad_t;
//
//extern VRAMLoad_t vramWrite;
//
//struct PSXDisplay_t {
//    sh modeX;
//    sh modeY;
//    sh startX;
//    sh startY;
//    bool disabled;
//    bool colordepth24;
//    bool pal;
//    bool changed;
//};
//extern struct PSXDisplay_t psxDisp, oldpsxDisp;
//
//struct PSXDraw_t {
//    sh offsetX;
//    sh offsetY;
//    sh clipX1;
//    sh clipX2;
//    sh clipY1;
//    sh clipY2;
//    sh texwinX1;
//    sh texwinY1;
//    sh texwinX2;
//    sh texwinY2;
//    bool texwinenabled;
//    bool enabled;
//    bool setmask;
//    bool testmask;
//};
//
//extern struct PSXDraw_t psxDraw;
//
//extern sh dispWidths[8];
//
//extern sw dispLace;
//extern sw dispLaceNew;
//extern sw imageTransfer;
//extern sw drawingLines;
//
//extern sh imTYc,imTXc,imTY,imTX;
//extern sw imSize;
//extern sh imageX0,imageX1;
//extern sh imageY0,imageY1;
//
//union uPointers{
//    void *v;
//    ub *b;
//    uh *w;
//    uw *d;
//    sb *c;
//    sh *s;
//    sw *l;
//};
//
//extern sh windowX, windowY;
//
//extern ub texshade[256];
//extern uw image[65536];
//extern uw torgba[65536];
//
//struct texturecache{
//    sw textAddrX;
//    sw textAddrY;
//    sw textTP;
//    sw clutP;
//    GLuint id;
//    bool Update;
//};
//
//extern struct texturecache texture[384];
//extern GLuint xferTexture16;
//extern GLuint xferTexture24;
//extern GLuint nullid;
//
//extern void GPUinit();
//extern void GPUopen();
//extern void GPUupdateLace();
//extern uw GPUreadStatus();
//extern void GPUwriteStatus(uw gdata);
//extern uw GPUreadData();
//extern void GPUwriteData(uw gdata);
//extern void executeDMA(CstrBus::castDMA *dma);
