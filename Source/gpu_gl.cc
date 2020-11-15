#include "Global.h"

static uh _dummy_gpu_frame_buffer[(0x100000)/2];
uw gpu_writeDmaWidth=0, gpu_writeDmaHeight=0;
uh *gpu_frame_buffer=&_dummy_gpu_frame_buffer[0];
uh *gpu_pvram=&_dummy_gpu_frame_buffer[0];
sw gpu_x_start=0, gpu_y_start=0, gpu_x_end=0, gpu_y_end=0;
sw gpu_px=0, gpu_py=0;
sw gpu_updateLace = 0;

void gpu_openVideo()
{
    memset((void *)&_dummy_gpu_frame_buffer[0],0,0x100000);
    gpu_frame_buffer=&_dummy_gpu_frame_buffer[0];
    gpu_pvram=&_dummy_gpu_frame_buffer[0];
    gpu_px=gpu_py=0;
    gpu_x_start=gpu_y_start=0;
    gpu_x_end=gpu_y_end=0;
    gpu_writeDmaWidth=0;
    gpu_writeDmaHeight=0;
    gpu_updateLace = 0;
}

void gpu_loadImage()
{
	uh x0, y0, w0, h0;
	x0 = PacketBuffer.U2[2] & 1023;
	y0 = PacketBuffer.U2[3] & 511;
	w0 = PacketBuffer.U2[4];
	h0 = PacketBuffer.U2[5];
	FrameIndex = FRAME_OFFSET(x0, y0);
	if ((y0 + h0) > FRAME_HEIGHT) {
		h0 = FRAME_HEIGHT - y0;
	}
	FrameToWrite = w0 * h0;
	FrameCount = FrameWidth = w0;

	gpu_writeDmaWidth = w0;
	gpu_writeDmaHeight = h0;
	
	gpu_px = gpu_x_start = x0;
	gpu_py = gpu_y_start = y0;
	gpu_x_end = gpu_x_start + w0;
	gpu_y_end = gpu_y_start + h0;
	gpu_pvram = &gpu_frame_buffer[gpu_py*1024];

	gpu_updateLace = 1;
}

void gpu_storeImage()
{
	uh x0, y0, w0, h0;
	x0 = PacketBuffer.U2[2] & 1023;
	y0 = PacketBuffer.U2[3] & 511;
	w0 = PacketBuffer.U2[4];
	h0 = PacketBuffer.U2[5];
	FrameIndex = FRAME_OFFSET(x0, y0);
	if ((y0 + h0) > FRAME_HEIGHT) {
		h0 = FRAME_HEIGHT - y0;
	}
	FrameToRead = w0 * h0;
	FrameCount = FrameWidth = w0;

	gpu_px = gpu_x_start = x0;
	gpu_py = gpu_y_start = y0;
	gpu_x_end = gpu_x_start + w0;
	gpu_y_end = gpu_y_start + h0;
	gpu_pvram = &gpu_frame_buffer[gpu_py*1024];
	
	GP1 |= 0x08000000;
}

/*----------------------------------------------------------------------
gpuSendPacket
----------------------------------------------------------------------*/

//void gpu_sendPacket()
//{
//	uw temp;
//
////#ifdef WITH_DRDEBUG
////SysMessage("PCKT %x", (PacketBuffer.U4[0]>>24));
////#endif
//
//	temp = PacketBuffer.U4[0];
//	switch (temp >> 24) {
//		case 0x00:
//		case 0x01:
//			return;
//		case 0x02:
//			gpuClearImage();
//			gpu_updateLace = 1;
//			return;
//		case 0x20:
//		case 0x21:
//		case 0x22:
//		case 0x23:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
//				gpuDrawF3();
//				gpu_updateLace = 1;
//			}
//			return;
//		case 0x24:
//		case 0x25:
//		case 0x26:
//		case 0x27:
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(PacketBuffer.U4[4] >> 16);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawFT3();
//			}
//			return;
//		case 0x28:
//		case 0x29:
//		case 0x2A:
//		case 0x2B:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawF3();
//			}
//			PacketBuffer.U4[1] = PacketBuffer.U4[4];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawF3();
//			}
//			return;
//		case 0x2C:
//		case 0x2D:
//		case 0x2E:
//		case 0x2F:
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(PacketBuffer.U4[4] >> 16);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawFT3();
//			}
//			PacketBuffer.U4[1] = PacketBuffer.U4[7];
//			PacketBuffer.U4[2] = PacketBuffer.U4[8];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawFT3();
//			}
//			return;
//		case 0x30:
//		case 0x31:
//		case 0x32:
//		case 0x33:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawG3();
//			}
//			return;
//		case 0x34:
//		case 0x35:
//		case 0x36:
//		case 0x37:
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(PacketBuffer.U4[5] >> 16);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawGT3();
//			}
//			return;
//		case 0x38:
//		case 0x39:
//		case 0x3A:
//		case 0x3B:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawG3();
//			}
//			PacketBuffer.U4[0] = PacketBuffer.U4[6];
//			PacketBuffer.U4[1] = PacketBuffer.U4[7];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawG3();
//			}
//			return;
//		case 0x3C:
//		case 0x3D:
//		case 0x3E:
//		case 0x3F:
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(PacketBuffer.U4[5] >> 16);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawGT3();
//			}
//			PacketBuffer.U4[0] = PacketBuffer.U4[9];
//			PacketBuffer.U4[1] = PacketBuffer.U4[10];
//			PacketBuffer.U4[2] = PacketBuffer.U4[11];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawGT3();
//			}
//			return;
//		case 0x40:
//		case 0x41:
//		case 0x42:
//		case 0x43:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawLF();
//			}
//			return;
//		case 0x48:
//		case 0x49:
//		case 0x4A:
//		case 0x4B:
//		case 0x4C:
//		case 0x4D:
//		case 0x4E:
//		case 0x4F:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawLF();
//			}
//			if ((PacketBuffer.U4[3] & 0xF000F000) != 0x50005000) {
//				PacketBuffer.U4[1] = PacketBuffer.U4[2];
//				PacketBuffer.U4[2] = PacketBuffer.U4[3];
//				PacketCount = 1;
//				PacketIndex = 3;
//			}
//			return;
//		case 0x50:
//		case 0x51:
//		case 0x52:
//		case 0x53:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawGF();
//			}
//			return;
//		case 0x58:
//		case 0x59:
//		case 0x5A:
//		case 0x5B:
//		case 0x5C:
//		case 0x5D:
//		case 0x5E:
//		case 0x5F:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawGF();
//			}
//			if ((PacketBuffer.U4[4] & 0xF000F000) != 0x50005000) {
//				PacketBuffer.U1[3 + (2 * 4)] =
//					PacketBuffer.U1[3 + (0 * 4)];
//				PacketBuffer.U4[0] = PacketBuffer.U4[2];
//				PacketBuffer.U4[1] = PacketBuffer.U4[3];
//				PacketBuffer.U4[2] = PacketBuffer.U4[4];
//				PacketCount = 2;
//				PacketIndex = 3;
//			}
//
//			return;
//		case 0x60:
//		case 0x61:
//		case 0x62:
//		case 0x63:
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawT();
//			}
//			return;
//		case 0x64:
//		case 0x65:
//		case 0x66:
//		case 0x67:
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(GP1);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawS();
//			}
//			return;
//		case 0x68:
//		case 0x69:
//		case 0x6A:
//		case 0x6B:
//			PacketBuffer.U4[2] = 0x00010001;
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawT();
//			}
//			return;
//		case 0x6C:
//		case 0x6D:
//		case 0x6E:
//		case 0x6F:
//			PacketBuffer.U4[3] = 0x00010001;
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(GP1);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawS();
//			}
//			return;
//		case 0x70:
//		case 0x71:
//		case 0x72:
//		case 0x73:
//			PacketBuffer.U4[2] = 0x00080008;
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawT();
//			}
//			return;
//		case 0x74:
//		case 0x75:
//		case 0x76:
//		case 0x77:
//			PacketBuffer.U4[3] = 0x00080008;
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(GP1);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawS();
//			}
//			return;
//		case 0x78:
//		case 0x79:
//		case 0x7A:
//		case 0x7B:
//			PacketBuffer.U4[2] = 0x00100010;
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 2) | 1];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawT();
//			}
//			return;
//		case 0x7C:
//		case 0x7D:
//		case 0x7E:
//		case 0x7F:
//			PacketBuffer.U4[3] = 0x00100010;
////			gpuSetCLUT(PacketBuffer.U4[2] >> 16);
////			gpuSetTexture(GP1);
////			gpuDriver = (void (*)())gpuDrivers[Masking | ((temp >> 24) & 7)];
//			if (!skip_this_frame)
//			{
////				gpu_updateLace = 1;
////				gpuDrawS();
//			}
//			return;
//		case 0x80:
////			gpuMoveImage();
////			gpu_updateLace = 1;
//			return;
//		case 0xA0:
////			gpu_loadImage();
//			return;
//		case 0xC0:
////			gpu_storeImage();
//			return;
//		case 0xE1:
//			GP1 = (GP1 & ~0x000007FF) | (temp & 0x000007FF);
////			gpuSetTexture(temp);
//			return;
//		case 0xE2:
//			TextureWindow[0] = ((temp >> 10) & 0x1F) << 3;
//			TextureWindow[1] = ((temp >> 15) & 0x1F) << 3;
//			TextureWindow[2] = TextureMask[(temp >> 0) & 0x1F];
//			TextureWindow[3] = TextureMask[(temp >> 5) & 0x1F];
//			TextureWindow[0] &= ~TextureWindow[2];
//			TextureWindow[1] &= ~TextureWindow[3];
////			gpuSetTexture(GP1);
//			return;
//		case 0xE3:
////SysMessage("E3 %x", temp); // EDIT TEMP
//			DrawingArea[0] = temp & 0x3FF;
//			DrawingArea[1] = (temp >> 10) & 0x3FF;
//			return;
//		case 0xE4:
////SysMessage("E4 %x", temp); // EDIT TEMP
//			DrawingArea[2] = (temp & 0x3FF) + 1;
//			DrawingArea[3] = ((temp >> 10) & 0x3FF) + 1;
//			return;
//		case 0xE5:
//		
//			// DrawingOffset[0] = temp & 0x7FF;
//			// DrawingOffset[1] = (temp >> 11) & 0x7FF;
//		
////SysMessage("E5 %x", temp); // EDIT TEMP
//			DrawingOffset[0] = ((sw)temp<<(32-11))>>(32-11);
//			DrawingOffset[1] = ((sw)temp<<(32-22))>>(32-11);
//			return;
//		case 0xE6:
//			temp &= 3;
//			GP1 = (GP1 & ~0x00001800) | (temp << 11);
////			Masking = (temp << 2) & 0x8;
////			PixelMSB = temp << 15;
//			return;
//	}
//}
