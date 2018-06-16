#import "Global.h"


#ifndef MYGPU

#define GPUCOMMAND(x) ((x>>24) & 0xff)

unsigned char psxVub[1024*520*2];
unsigned short *psxVuw;
uint32_t *psxVul;

static int32_t GPUdataRet;
int32_t GPUstatusRet;
int32_t GPUInfoVals[16];

static uint32_t gpuData[100];
static unsigned char gpuCommand = 0;
static int32_t gpuDataC = 0;
static int32_t gpuDataP = 0;

int drawingLines;

VRAMLoad_t vramWrite;
struct PSXDisplay_t psxDisp, oldpsxDisp;
struct PSXDraw_t psxDraw;

short dispWidths[8] = {256,320,512,640,368,384,512,640};

int dispLace = 0;
int dispLaceNew;
int imageTransfer;

short imTYc,imTXc,imTY,imTX;
int imSize;
short imageX0,imageX1;
short imageY0,imageY1;

unsigned short textBuf[512*512];
int newTextX0,newTextX1,newTextX2,newTextX3;
int newTextY0,newTextY1,newTextY2,newTextY3;

GLuint xferTexture16 = 0;
GLuint xferTexture24 = 0;

void GPUinit() {
	psxVuw = (unsigned short *)psxVub;
	psxVul = (uint32_t *)psxVub;

	GPUstatusRet = 0x74000000;
	memset(GPUInfoVals, 0x00, 16 * sizeof(uint32_t));
}

void GPUopen() {
    glViewport(0, 0, 320, 240);
    
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0f/256.0f,1.0f/256.0f,1.0f);
    
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    
    for(int i = 0; i < 64; i++) {
        texture[i].textAddrX=0;
        texture[i].textAddrY=0;
        texture[i].textTP=0;
        texture[i].clutP=nullclutP;
        texture[i].Update=FALSE;
        glGenTextures(1,&texture[i].id);
        glBindTexture(GL_TEXTURE_2D,texture[i].id);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    }
    
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    torgba[0]=0x00000000;
    for (int i=1;i<65536;i++){
        torgba[i] =(i&0x001f)<<3;
        torgba[i]|=(i&0x03e0)<<6;
        torgba[i]|=(i&0x7c00)<<9;
        torgba[i]|=0xfe000000;
        if (!(i&0x8000))
            torgba[i]|=0xff000000;
    }
    torgba[0x8000] = 0xff000000;
    
    for(int i=0; i<128; i++)
        texshade[i]=i<<1;
    for(int i=128; i<256; i++)
        texshade[i]=255;
}

void updateDisplay() {
    if (psxDisp.modeX > 0 && psxDisp.modeY > 0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, psxDisp.modeX - 1.0, psxDisp.modeY - 1.0, 0, 1.0, -1.0);
    }
}

void GPUupdateLace() {
	drawingLines ^= 1;
    glFlush();
}

unsigned long GPUreadStatus() {
	if (drawingLines == 1)
		return GPUstatusRet;
	else
        return GPUstatusRet;// | 0x80000000;
}

void GPUwriteStatus(uint32_t gdata) {
	switch((gdata >> 24) & 0xff) {
        case 0x00:
            GPUstatusRet=0x14802000;
            return;
        
        case 0x03:
            psxDisp.disabled = (gdata & 1);
            return;
        
        case 0x04:
            if((gdata&0xffffff) == 0) imageTransfer = 0;
            if((gdata&0xffffff) == 2) imageTransfer = 3;
            return;
        
        case 0x05:
        case 0x06:
        case 0x07:
            return;
        
        case 0x08:
            psxDisp.modeX = dispWidths[(gdata&0x3)|((gdata&0x40)>>4)];
            
            if (gdata&0x04)
                psxDisp.modeY = 480;
            else
                psxDisp.modeY = 240;
            
            updateDisplay();
            return;
    }
}

unsigned long GPUreadData(void)
{
//    if(imageTransfer==2)
//    {
//        if ((imTY>=0) && (imTY<512) && (imTX>=0) && (imTX<1024))
//        {
//            GPUdataRet=psxVul[imTY*512+imTX/2];
//        }
//        imTX+=2;
//        imTXc-=2;
//        if(imTXc<=0)
//        {
//            imTX=imageX0;
//            imTXc=imageX1;
//            imTYc--;
//            imTY++;
//        }
//        imSize--;
//        if(imSize <= 0)
//        {
//            GPUstatusRet&=0xf7ffffff;
//            imageTransfer=0;
//        }
//    }
	return GPUdataRet;
}

int PullFromPsxRam(uint32_t *pMem, int size)
{
	int count = 0;
	unsigned short *input = (unsigned short*)pMem;
	uint32_t *t = vramWrite.extratarget;
	uint16_t *st = (uint16_t*)t;

	short x2 = vramWrite.x + vramWrite.w;
	short y2 = vramWrite.y + vramWrite.h;
	
	unsigned short posx, posy;

	if (vramWrite.enabled == 0) {
		imageTransfer = 0;
		return 0;
	}

    size <<=1;

	while(vramWrite.cury < y2)
	{
		posy = (unsigned short) vramWrite.cury;
		if (posy >= 512)
			posy = 0;
		while(vramWrite.curx < x2)
		{
			posx = (unsigned short) vramWrite.curx;
			if (posx >= 1024)
				posx = 0;

//            if (!psxDisp.colordepth24)
//                *t++ = torgba[*input];
//            else
//                *st++ = *input;

			psxVuw[(posy<<10)+posx] = *input;
			
			vramWrite.curx++;
			count++;
			input++;
			
			if (count == size)
			{
				if (vramWrite.curx == x2)
				{
					vramWrite.cury++;
					vramWrite.curx=vramWrite.x;
				}
				goto NOMOREIMAGEDATA;
			}
		}
		vramWrite.cury++;
		vramWrite.curx=vramWrite.x;
	}

NOMOREIMAGEDATA:
	if (vramWrite.cury >= y2)
	{
		float x,w;

		vramWrite.enabled = 0;

//        glMatrixMode(GL_TEXTURE);
//        glPushMatrix();
//        glLoadIdentity();
//        glScalef(1.0f/1024.0f,1.0f/512.0f,1.0f);
//
//        if (psxDisp.colordepth24)
//        {
//            x=vramWrite.x*2/3;
//            w=vramWrite.w*2/3;
//            glBindTexture(GL_TEXTURE_2D,xferTexture24);
//            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,vramWrite.h,GL_RGB,GL_UNSIGNED_BYTE,vramWrite.extratarget);
//        }
//        else
//        {
//            x=vramWrite.x;
//            w=vramWrite.w;
//            glBindTexture(GL_TEXTURE_2D,xferTexture16);
//            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,w,vramWrite.h,GL_RGBA,GL_UNSIGNED_BYTE,vramWrite.extratarget);
//        }
//
//        glDisable(GL_BLEND);
//        glDisable(GL_CLIP_PLANE0);
//        glDisable(GL_CLIP_PLANE1);
//        glDisable(GL_CLIP_PLANE2);
//        glDisable(GL_CLIP_PLANE3);
//
//        glColor3ub(255,255,255);
//
//        glBegin(GL_POLYGON);
//            glTexCoord2s(0, 0);
//            glVertex2s(x,vramWrite.y);
//
//            glTexCoord2s(w, 0);
//            glVertex2s(x+w,vramWrite.y);
//
//            glTexCoord2s(w, vramWrite.h);
//            glVertex2s(x+w,vramWrite.y+vramWrite.h);
//
//            glTexCoord2s(0, vramWrite.h);
//            glVertex2s(x,vramWrite.y+vramWrite.h);
//        glEnd();
//        glEnable(GL_BLEND);
//
//        glPopMatrix();
//
//        glEnable(GL_CLIP_PLANE0);
//        glEnable(GL_CLIP_PLANE1);
//        glEnable(GL_CLIP_PLANE2);
//        glEnable(GL_CLIP_PLANE3);
//
//
//        free(vramWrite.extratarget);
		imageTransfer = 0;
		if (count%2 == 1)
			count++;
	}

	return count>>1;
}

void GPUwriteDataMem(uint32_t * pMem, int iSize) {
	unsigned char command;
	int i = 0;
	uint32_t gdata = 0;
	
	for(;i<iSize;)
	{
		if((imageTransfer & 1) == 1)
		{
			i += PullFromPsxRam(pMem, iSize-i);
			if (i >= iSize)
				continue;
			pMem += i;
		}
		
		gdata=*pMem;
		GPUdataRet=gdata;
		pMem++;
		i++;
		
		if(gpuDataC == 0)
		{
			command = (unsigned char) (gdata>>24) & 0xff;
			if (primTableC[command])
			{
				gpuDataC = primTableC[command];
				gpuCommand = command;
				gpuData[0] = gdata;
				gpuDataP = 1;
			}
			else
				continue;
		}
		else
		{
			gpuData[gpuDataP] = gdata;
//            if ( gpuDataC>128 )
//            {
//                if ( ( gpuDataC==254 && gpuDataP>=3 ) ||
//                    ( gpuDataC==255 && gpuDataP>=4 && ! ( gpuDataP&1 ) ) )
//                {
//                    if ( ( gpuData[gpuDataP] & 0xF000F000 ) == 0x50005000 )
//                        gpuDataP=gpuDataC-1;
//                }
//            }
			gpuDataP++;
		}
		if(gpuDataP == gpuDataC)
		{
			gpuDataC=gpuDataP=0;
			primTableJ[gpuCommand]((unsigned char *)gpuData);
		}
	}

	//GPUdataRet=gdata;
}

void GPUwriteData(uint32_t gdata) {
	GPUwriteDataMem(&gdata, 1);
}

void executeDMA(CstrBus::castDMA *dma) {
    uw *p = (uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)], size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    
    switch(dma->chcr) {
        case 0x01000200:
            //dataRead(p, size);
            return;
            
        case 0x01000201:
            GPUwriteDataMem(p, size);
            return;
            
        case 0x01000401:
            do {
                uw hdr = *(uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
                p = (uw *)&mem.ram.ptr[(dma->madr + 4) & 0x1ffffc];
                GPUwriteDataMem(p, hdr >> 24);
                dma->madr = hdr & 0xffffff;
            }
            while(dma->madr != 0xffffff);
            return;
            
            /* unused */
        case 0x00000401: // Disable DMA?
            return;
    }
    
    printx("/// PSeudo GPU DMA: $%x", dma->chcr);
}

#endif
