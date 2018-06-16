#ifndef MYGPU

extern uh lutBGR2RGB[65536];
extern uh textBuf[];

extern sw GPUstatusRet;

#define INFO_TW        0
#define INFO_DRAWSTART 1
#define INFO_DRAWEND   2
#define INFO_DRAWOFF   3

#define GPUSTATUS_ODDLINES            0x80000000
#define GPUSTATUS_DMABITS             0x60000000 // Two bits
#define GPUSTATUS_READYFORCOMMANDS    0x10000000
#define GPUSTATUS_READYFORVRAM        0x08000000
#define GPUSTATUS_IDLE                0x04000000
#define GPUSTATUS_DISPLAYDISABLED     0x00800000
#define GPUSTATUS_INTERLACED          0x00400000
#define GPUSTATUS_RGB24               0x00200000
#define GPUSTATUS_PAL                 0x00100000
#define GPUSTATUS_DOUBLEHEIGHT        0x00080000
#define GPUSTATUS_WIDTHBITS           0x00070000 // Three bits
#define GPUSTATUS_MASKENABLED         0x00001000
#define GPUSTATUS_MASKDRAWN           0x00000800
#define GPUSTATUS_DRAWINGALLOWED      0x00000400
#define GPUSTATUS_DITHER              0x00000200

typedef struct {
	uw x, y, colormode, abr;
} TextureState_t;

extern TextureState_t texinfo;

typedef struct VRAMLOADTTAG {
	sh x, y, w, h, curx, cury;
	uw *extratarget;
	bool enabled;
} VRAMLoad_t;

extern VRAMLoad_t vramWrite;

struct PSXDisplay_t {
	sh modeX, modeY;
};
extern struct PSXDisplay_t psxDisp;

struct PSXDraw_t {
	sh offsetX;
	sh offsetY;
	sh clipX1;
	sh clipX2;
	sh clipY1;
	sh clipY2;
};

extern struct PSXDraw_t psxDraw;

extern sh dispWidths[8];
extern sw imageTransfer;
extern sw drawLace;
extern sw drawingLines;
extern sh imTYc,imTXc,imTY,imTX;
extern sw imSize;
extern sh imageX0,imageX1;
extern sh imageY0,imageY1;
extern uh textBuf[];

union uPointers {
	void *v;
	ub *b;
	uh *w;
	uw *d;
	sb *c;
	sh *s;
	sw *l;
};
extern ub psxVub[];
extern uh *psxVuw;
extern uw *psxVul;

extern sh windowX, windowY;

extern ub texshade[256];
extern uw image[65536];
extern uw torgba[65536];


struct texturecache {
	sw textAddrX, textAddrY, textTP, clutP;
	GLuint id;
	bool Update;
};

extern struct texturecache texture[64];
extern GLuint xferTexture16;
extern GLuint xferTexture24;
extern GLuint nullid;
#define nullclutP (1024 * 512 + 1)

void GPUinit();
void GPUopen();
void GPUupdateLace();
uw GPUreadStatus();
void GPUwriteStatus(uw gdata);
uw GPUreadData();
void GPUwriteData(uw gdata);
void executeDMA(CstrBus::castDMA *dma);

#endif
