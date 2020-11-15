#include "Global.h"

#define GPU_COMMAND(x) \
    (x >> 24) & 0xff

uw GP0=0;
uw GP1=0;
sw FrameToRead=0;
sw FrameToWrite=0;
sw FrameWidth;
sw FrameCount;
sw FrameIndex;
PacketBuffer_t PacketBuffer;
sw PacketCount;
sw PacketIndex;
sw isPAL = 0;
sw TextureWindow[4];
sw DrawingArea[4];
sw DrawingOffset[2];
sw DisplayArea[8];
sw OtherEnv[16];

ub PacketSize[256] = {
	0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		0-15
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		16-31
	3, 3, 3, 3, 6, 6, 6, 6, 4, 4, 4, 4, 8, 8, 8, 8,	//		32-47
	5, 5, 5, 5, 8, 8, 8, 8, 7, 7, 7, 7, 11, 11, 11, 11,	//	48-63
	2, 2, 2, 2, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,	//		64-79
	3, 3, 3, 3, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4,	//		80-95
	2, 2, 2, 2, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 2,	//		96-111
	1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2,	//		112-127
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		128-
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		144
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//		160
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	//
};

uh HorizontalResolution[8] = {
	256, 368, 320, 384, 512, 512, 640, 640
};

uh VerticalResolution[4] = {
	240, 480, 256, 480
};

/* GPU_reset */
void GPU_init() {
    gpu_openVideo();
}

void GPU_reset()
{
    GP0 = 0x400;
	GP1 = 0x14802000;
	TextureWindow[2] = 255;
	TextureWindow[3] = 255;
	DrawingArea[2] = 256;
	DrawingArea[3] = 240;
	DisplayArea[2] = 256;
	DisplayArea[3] = 240;
	DisplayArea[6] = 256;
	DisplayArea[7] = 240;
}

sw skip_this_frame=0;
uw clockw = 0, scanline = 0;

void GPUupdate(uw frames) {
    uw lines = (clockw += frames) / 3413;
    clockw %= 3413;
    
    if ((scanline += lines) >= 262) { scanline = 0;
        draw.swapBuffers(false);
        bus.interruptSet(CstrBus::INT_VSYNC);
    }
}

void GPU_writeStatus(uw data)
{
	switch(data >> 24) {
		case 0x00:
			GPU_reset();
			break;
            
		case 0x01:
			GP1 &= ~0x08000000;
			PacketCount = FrameToRead = FrameToWrite = 0;
			break;
            
		case 0x02:
			GP1 &= ~0x08000000;
			PacketCount = FrameToRead = FrameToWrite = 0;
			break;
            
		case 0x03:
			GP1 = (GP1 & ~0x00800000) | ((data & 1) << 23);
			break;
            
		case 0x04:
			if (data == 0x04000000) PacketCount = 0;
			GP1 = (GP1 & ~0x60000000) | ((data & 3) << 29);
			break;
            
		case 0x05:
			DisplayArea[0] = data & 0x000003FF; //(short)(data & 0x3ff);
			DisplayArea[1] = (data & 0x000FFC00) >> 10; //(short)((data>>10)&0x1ff);
			break;
            
		case 0x06:
			DisplayArea[4] = data & 0x00000FFF; //(short)(data & 0x7ff);
			DisplayArea[6] = (data & 0x00FFF000) >> 12; //(short)((data>>12) & 0xfff);
			break;
            
		case 0x07:
			{		
				//int iT;
				DisplayArea[5] = data & 0x000003FF; //(short)(data & 0x3ff);
				DisplayArea[7] = (data & 0x000FFC00) >> 10; //(short)((data>>10) & 0x3ff);
				
				/*
				drawHeight = DisplayArea[7] - DisplayArea[5];

				if(isPAL) iT=48; else iT=28;

				if(DisplayArea[5]>=iT)
				{
					DisplayArea[5] = (short)(DisplayArea[5]-iT-4);
					if(DisplayArea[5]<0)
					{
						DisplayArea[5]=0;
					}
					drawHeight += DisplayArea[5];
				}
				else 
				{
					DisplayArea[5] = 0;
				}
				*/
			}
			break;
            
		case 0x08:
			OtherEnv[0x08] = (data >> 7) & 1;	//	reverse(?)
			GP1 = (GP1 & ~0x007F0000) | ((data & 0x3F) << 17) | ((data & 0x40) << 10);

			{
				DisplayArea[2] = HorizontalResolution[(GP1 >> 16) & 7];
				DisplayArea[3] = VerticalResolution[(GP1 >> 19) & 3];
                draw.resize(DisplayArea[2], DisplayArea[3]);
			}
            
			isPAL = (data & 0x08) ? 1 : 0; // if 1 - PAL mode, else NTSC
			break;
            
		case 0x09:
			OtherEnv[0x09] = data & 1;			//	gpub(?)
			break;
            
		case 0x10:
			switch (data & 0xffff) {
				case 0:
				case 1:
				case 3:
					GP0 = (DrawingArea[1] << 10) | DrawingArea[0];
					break;
				case 4:
					GP0 =
						((DrawingArea[3] - 1) << 10) | (DrawingArea[2] -
														1);
					break;
				case 6:
				case 5:
					GP0 = (DrawingOffset[1] << 11) | DrawingOffset[0];
					break;
				case 7:
					GP0 = 2;
					break;
				default:
					GP0 = 2;
			}
			break;
	}
}

uw  GPU_readStatus()
{
   //static int iNumRead=0;                              // odd/even hack
   //if((iNumRead++)==2)
    //{
     //iNumRead=0;
     GP1 ^= 0x80000000;                        // interlaced bit toggle... we do it on every 3 read status... needed by some games (like ChronoCross) with old epsxe versions (1.5.2 and older)
    //}

	return (GP1 | 0x08000000);
}

void GPU_writeData(uw data)
{
	GP1 &= ~0x14000000;
	if (FrameToWrite > 0) {
          gpu_pvram[gpu_px]=(uh)data;
          if (++gpu_px>=gpu_x_end) {
               gpu_px = gpu_x_start;
               gpu_pvram += 1024;
               if (++gpu_py>=gpu_y_end) FrameToWrite=0;
          }
          if (FrameToWrite > 0) {
               gpu_pvram[gpu_px]=data>>16;
               if (++gpu_px>=gpu_x_end) {
                    gpu_px = gpu_x_start;
                    gpu_pvram += 1024;
                    if (++gpu_py>=gpu_y_end) FrameToWrite=0;
               }
          }
	} else {
		if (PacketCount) {
			PacketCount--;
			PacketBuffer.U4[PacketIndex++] = data;
		} else {
			PacketBuffer.U4[0] = data;
			PacketCount = PacketSize[data >> 24];
			PacketIndex = 1;
		}
		if (!PacketCount)
		{
			//gpu_sendPacket();
            printf("0x%x\n", PacketBuffer.U4[0]);
            draw.primitive(GPU_COMMAND(PacketBuffer.U4[0]), (uw *)&PacketBuffer);
		}
	}
	GP1 |= 0x14000000;
}

void GPU_writeDataMem(uw * dmaAddress, sw dmaCount)
{
	uw temp;

	GP1 &= ~0x14000000;

	while (dmaCount) {
		if (FrameToWrite > 0) {
			while (dmaCount--) 
			{
				uw data = *dmaAddress++;

				if (gpu_px<1024 && gpu_py<512)
					gpu_pvram[gpu_px] = data;
				if (++gpu_px>=gpu_x_end) 
				{
					gpu_px = gpu_x_start;
					gpu_pvram += 1024;
					if (++gpu_py>=gpu_y_end) 
					{
						FrameToWrite = 0;
						GP1 &= ~0x08000000;
						break;
					}
				}
				if (gpu_px<1024 && gpu_py<512)
					gpu_pvram[gpu_px] = data>>16;
				if (++gpu_px>=gpu_x_end) 
				{
					gpu_px = gpu_x_start;
					gpu_pvram += 1024;
					if (++gpu_py>=gpu_y_end) 
					{
						FrameToWrite = 0;
						GP1 &= ~0x08000000;
						break;
					}
				}
			}
		}
		else
		{
			temp = *dmaAddress++;
			dmaCount--;
			if (PacketCount) {
				PacketBuffer.U4[PacketIndex++] = temp;
				PacketCount--;
			} else {
				PacketBuffer.U4[0] = temp;
				PacketCount = PacketSize[temp >> 24];
				PacketIndex = 1;
			}
			if (!PacketCount)
			{
				//gpu_sendPacket();
                printf("0x%x\n", GPU_COMMAND(PacketBuffer.U4[0]));
                draw.primitive(GPU_COMMAND(PacketBuffer.U4[0]), (uw *)&PacketBuffer);
			}
		}
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
}

uw GPU_readData()
{
	GP1 &= ~0x14000000;
	if (FrameToRead)
	{
		GP0 = gpu_pvram[gpu_px];
		if (++gpu_px>=gpu_x_end) {
		   gpu_px = gpu_x_start;
		   gpu_pvram += 1024;
		   if (++gpu_py>=gpu_y_end) FrameToRead=0;
		}
		GP0 |= gpu_pvram[gpu_px]<<16;
		if (++gpu_px>=gpu_x_end) {
		   gpu_px = gpu_x_start;
		   gpu_pvram +=1024;
		   if (++gpu_py>=gpu_y_end) FrameToRead=0;
		}

		if( FrameToRead == 0 ) GP1 &= ~0x08000000;
	}
	GP1 |= 0x14000000;
	return (GP0);
}

void GPU_readDataMem(uw * dmaAddress, sw dmaCount)
{
	if( FrameToRead == 0 ) return;

	GP1 &= ~0x14000000;

	do 
	{
		// lower 16 bit
		uw data = (uw)gpu_pvram[gpu_px];

		if (++gpu_px>=gpu_x_end) 
		{
			gpu_px = gpu_x_start;
			gpu_pvram += 1024;
		}

		// higher 16 bit (always, even if it's an odd width)
		data |= (uw)(gpu_pvram[gpu_px])<<16;
    
		*dmaAddress++ = data;

		if (++gpu_px>=gpu_x_end) 
		{
			gpu_px = gpu_x_start;
			gpu_pvram += 1024;
			if (++gpu_py>=gpu_y_end) 
			{
				FrameToRead = 0;
				GP1 &= ~0x08000000;
				break;
			}
		}
	} while (--dmaCount);

	GP1 = (GP1 | 0x14000000) & ~0x60000000;
}

sw GPU_dmaChain(uw * baseAddr, uw dmaVAddr)
{
	uw data, *address, count, offset;
	GP1 &= ~0x14000000;
	dmaVAddr &= 0x00FFFFFF;
	while (dmaVAddr != 0xFFFFFF) {
		address = (baseAddr + (dmaVAddr >> 2));
		data = *address++;
		count = (data >> 24);
		offset = data & 0x00FFFFFF;
		if (dmaVAddr != offset)
			dmaVAddr = offset;
		else
			dmaVAddr = 0xFFFFFF;
		while (count) {
			data = *address++;
			count--;
			//temp = PacketCount;
			if (PacketCount) {
				PacketCount--;
				PacketBuffer.U4[PacketIndex++] = data;
			} else {
				PacketBuffer.U4[0] = data;
				PacketCount = PacketSize[data >> 24];
				PacketIndex = 1;
			}
			//PacketCount = temp;
			if (!PacketCount)
			{
                //gpu_sendPacket();
                printf("0x%x\n", GPU_COMMAND(PacketBuffer.U4[0]));
                draw.primitive(GPU_COMMAND(PacketBuffer.U4[0]), (uw *)&PacketBuffer);
			}
		}
	}
	GP1 = (GP1 | 0x14000000) & ~0x60000000;
	return (0);
}

void executeDMA(CstrBus::castDMA *dma) {
    uw *p   = (uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    sw size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    
    switch(dma->chcr) {
        case 0x01000200:
            GPU_readDataMem((uw *)p, size);
            //dataRead(p, size);
            return;
            
        case 0x01000201:
            GPU_writeDataMem((uw *)p, size);
            //dataWrite(p, size);
            return;
            
        case 0x01000401:
//            do {
//                uw hdr = *(uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
//                p = (uw *)&mem.ram.ptr[(dma->madr + 4) & (mem.ram.size - 1)];
//                GPU_writeDataMem(p, hdr >> 24);
//                //dataWrite(p, hdr >> 24);
//                dma->madr = hdr & 0xffffff;
//            }
//            while(dma->madr != 0xffffff);
            //uLONG *ram32 = (uLONG *)&RAM[madr & 0x1FFFFF];
            //GPUdmaChain((uLONG *)RAM, madr & 0x1FFFFF);
            GPU_dmaChain((uw *)p, dma->madr & 0x1fffff);
            return;
            
        /* unused */
        case 0x00000401: // Disable DMA?
            return;
    }
    
#ifdef DEBUG
    printx("/// PSeudo GPU DMA: 0x%x", dma->chcr);
#endif
}
