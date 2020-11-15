#define MAXSKIP			6
#define FRAME_HEIGHT            512
#define FRAME_OFFSET(x,y)       (((y)<<10)+(x))

extern void gpu_openVideo();

typedef union {
	sb S1[64];
	sh S2[32];
	sw S4[16];
	ub U1[64];
	uh U2[32];
	uw U4[16];
} PacketBuffer_t;

extern uw GP0;
extern uw GP1;
extern sw FrameToRead;
extern sw FrameToWrite;
extern sw FrameWidth;
extern sw FrameCount;
extern sw FrameIndex;
extern PacketBuffer_t PacketBuffer;
extern sw PacketCount;
extern sw PacketIndex;
extern sw TextureWindow[4];
extern sw DrawingArea[4];
extern sw DrawingOffset[2];
extern sw DisplayArea[8];
extern sw OtherEnv[16];
extern ub PacketSize[256];
extern uh HorizontalResolution[8];
extern uh VerticalResolution[4];
extern sw isPAL;
extern sw skip_this_frame;

void GPU_open(sw);
void GPU_reset();
void GPU_init();
void GPU_shutdown();
void GPU_writeStatus(uw);
uw GPU_readStatus();
void GPU_writeData(uw);
void GPU_writeDataMem(uw *, sw);
uw GPU_readData();
void GPU_readDataMem(uw *, sw);
sw GPU_dmaChain(uw *, uw);

extern uh *gpu_frame_buffer;
extern uh *gpu_pvram;
extern sw gpu_x_start, gpu_y_start, gpu_x_end, gpu_y_end;
extern sw gpu_px, gpu_py;
extern sw gpu_updateLace;
extern uw gpu_writeDmaWidth, gpu_writeDmaHeight;
void gpu_videoOutput();
void gpu_reset();
void gpu_openVideo();
void gpu_closeVideo();

extern void executeDMA(CstrBus::castDMA *dma);
extern void GPUupdate(uw frames);
extern void gpu_loadImage();
extern void gpu_storeImage();
