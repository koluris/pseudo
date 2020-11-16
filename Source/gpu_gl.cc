#include "Global.h"

static uh _dummy_gpu_frame_buffer[(0x100000)/2];
uw gpu_writeDmaWidth=0, gpu_writeDmaHeight=0;
uh *gpu_frame_buffer=&_dummy_gpu_frame_buffer[0];
uh *gpu_pvram=&_dummy_gpu_frame_buffer[0];
sw gpu_x_start=0, gpu_y_start=0, gpu_x_end=0, gpu_y_end=0;
sw gpu_px=0, gpu_py=0;

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
}

void gpu_loadImage() {
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
}

void gpu_storeImage() {
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
