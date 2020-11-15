//#include "Global.h"
//
////#define GPU_RGB16(rgb) (
//// (((rgb)&0xF80000)>>9) |
//// (((rgb)&0xF800)>>6) |
//// (((rgb)&0xF8)>>3)
//// )
//
//void gpuClearImage(void)
//{
//	unsigned rgb=PacketBuffer.S4[0];
//	float r= ((float)(((rgb)&0xF80000)>>19))/32.0;
//	float g= ((float)(((rgb)&0xF800)>>9))/32.0;
//	float b= ((float)(((rgb)&0xF8)>>3))/32.0;
//	float x0 = PacketBuffer.S2[2];
//	float y0 = PacketBuffer.S2[3];
//	float x1 = PacketBuffer.S2[4];
//	float y1 = PacketBuffer.S2[5];
//
////printf("gpuClearImage rgb=%f,%f,%f x0,y0=%f,%f x1,y1=%f,%f\n",r,g,b,x0,y0,x1,y1);
//
//	glDisable(GL_TEXTURE_2D);
//	glColor3f(r,g,b);
//	glBegin(GL_QUADS);
//		glVertex2f(x0,y0);
//		glVertex2f(x1,y0);
//		glVertex2f(x1,y1);
//		glVertex2f(x0,y1);
//	glEnd();
//}
//
//void gpuDrawF3(void)
//{
//	unsigned rgb=PacketBuffer.U4[0];
//	float r= ((float)(((rgb)&0xF80000)>>19))/32.0;
//	float g= ((float)(((rgb)&0xF800)>>9))/32.0;
//	float b= ((float)(((rgb)&0xF8)>>3))/32.0;
//	float x0 = PacketBuffer.S2[2];
//	float x1 = PacketBuffer.S2[4];
//	float x2 = PacketBuffer.S2[6];
//	float y0 = PacketBuffer.S2[3];
//	float y1 = PacketBuffer.S2[5];
//	float y2 = PacketBuffer.S2[7];
//
////printf("gpuDrawF3 rgb=%f,%f,%f x0,y0=%f,%f x1,y1=%f,%f x2,y2=%f,%f\n",r,g,b,x0,y0,x1,y1,x2,y2);
//
//	glDisable(GL_TEXTURE_2D);
//	glColor3f(r,g,b);
//	glBegin(GL_TRIANGLES);
//		glVertex2f(x0,y0);
//		glVertex2f(x1,y1);
//		glVertex2f(x2,y2);
//	glEnd();
//}
