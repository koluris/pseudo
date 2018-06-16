#ifndef MYGPU

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

extern unsigned short lutBGR2RGB[65536];

extern unsigned short textBuf[];

extern int32_t GPUstatusRet;
extern int32_t GPUInfoVals[16];

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

#define GPUIsBusy (GPUstatusRet &= ~GPUSTATUS_IDLE)
#define GPUIsIdle (GPUstatusRet |= GPUSTATUS_IDLE)

#define GPUIsNotReadyForCommands (GPUstatusRet &= ~GPUSTATUS_READYFORCOMMANDS)
#define GPUIsReadyForCommands (GPUstatusRet |= GPUSTATUS_READYFORCOMMANDS)

typedef struct {
	uint32_t x, y;
	uint32_t colormode;
	uint32_t abr;
	uint32_t mirror;
} TextureState_t;

extern TextureState_t texinfo;

typedef struct VRAMLOADTTAG
{
	short x;
	short y;
	short w;
	short h;
	short curx;
	short cury;
	uint32_t* extratarget;
	bool enabled;
} VRAMLoad_t;

extern VRAMLoad_t vramWrite;

struct PSXDisplay_t {
	short modeX;
	short modeY;
	short startX;
	short startY;
	bool disabled;
	bool colordepth24;
	bool interlaced;
	bool pal;
	short rangeX1;
	short rangeY1;
	short rangeX2;
	short rangeY2;

	bool changed;
};
extern struct PSXDisplay_t psxDisp, oldpsxDisp;

struct PSXDraw_t {
	short offsetX;
	short offsetY;
	short clipX1;
	short clipX2;
	short clipY1;
	short clipY2;
	short texwinX1;
	short texwinY1;
	short texwinX2;
	short texwinY2;
	bool texwinenabled;
	bool enabled;
	bool setmask;
	bool testmask;
};
extern struct PSXDraw_t psxDraw;

extern short dispWidths[8];

extern int32_t dispLace;
extern int32_t dispLaceNew;
extern int32_t imageTransfer;
extern int32_t drawLace;
extern int32_t drawingLines;

extern short imTYc,imTXc,imTY,imTX;
extern int32_t imSize;
extern short imageX0,imageX1;
extern short imageY0,imageY1;


extern int32_t newTextX0,newTextX1,newTextX2,newTextX3;
extern int32_t newTextY0,newTextY1,newTextY2,newTextY3;
extern unsigned short textBuf[];

union uPointers{
	void *v;
	unsigned char *b;
	unsigned short *w;
	uint32_t *d;
	char *c;
	short *s;
	int32_t *l;
};
extern unsigned char psxVub[];
extern unsigned short *psxVuw;
extern uint32_t *psxVul;

extern short windowX, windowY;

extern unsigned char texshade[256];
extern uint32_t image[65536];
extern uint32_t torgba[65536];


struct texturecache{
	int32_t textAddrX;
	int32_t textAddrY;
	int32_t textTP;
	int32_t clutP;
	GLuint id;
	bool Update;
};

extern struct texturecache texture[64];
extern GLuint xferTexture16;
extern GLuint xferTexture24;
extern GLuint nullid;
#define nullclutP (1024*512+1)

void GPUinit();
void GPUopen();
void GPUupdateLace();
unsigned long GPUreadStatus();
void GPUwriteStatus(uint32_t gdata);
unsigned long GPUreadData();
void GPUwriteData(uint32_t gdata);
void executeDMA(CstrBus::castDMA *dma);

#endif
