#import "Global.h"


#ifndef MYGPU

#define GPUCOMMAND(x) ((x >> 24) & 0xff)

ub psxVub[1024 * 520 * 2];
uh *psxVuw;
uw *psxVul;
static sw GPUdataRet;
sw GPUstatusRet;
static uw gpuData[100];
static ub gpuCommand = 0;
static sw gpuDataC = 0;
static sw gpuDataP = 0;
sw drawingLines;
VRAMLoad_t vramWrite;
struct PSXDisplay_t psxDisp;
struct PSXDraw_t psxDraw;
sh dispWidths[8] = { 256, 320, 512, 640, 368, 384, 512, 640 };
sw imageTransfer;
sh imTYc, imTXc, imTY, imTX;
sw imSize;
sh imageX0, imageX1;
sh imageY0, imageY1;
uh textBuf[512 * 512];

void GPUinit() {
	psxVuw = (uh *)psxVub;
	psxVul = (uw *)psxVub;
	GPUstatusRet = 0x14802000;
}

void GPUopen() {
    glViewport(0, 0, 320, 240);
    
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0f / 256.0f, 1.0f / 256.0f, 1.0f);
    
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    
    for(int i = 0; i < 64; i++) {
        texture[i].textAddrX = 0;
        texture[i].textAddrY = 0;
        texture[i].textTP    = 0;
        texture[i].clutP     = nullclutP;
        texture[i].Update    = false;
        
        glGenTextures  (1, &texture[i].id);
        glBindTexture  (GL_TEXTURE_2D, texture[i].id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D   (GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    torgba[0] = 0x00000000;
    for (int i = 1; i < 65536; i++){
        torgba[i]  = (i & 0x001f) << 3;
        torgba[i] |= (i & 0x03e0) << 6;
        torgba[i] |= (i & 0x7c00) << 9;
        torgba[i] |= 0xfe000000;
        if (!(i & 0x8000)) torgba[i] |= 0xff000000;
    }
    torgba[0x8000] = 0xff000000;
    
    for (int i = 0;   i < 128; i++) texshade[i] = i << 1;
    for (int i = 128; i < 256; i++) texshade[i] = 255;
}

void updateDisplay() {
    if (psxDisp.modeX > 0 && psxDisp.modeY > 0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, psxDisp.modeX - 1.0, psxDisp.modeY - 1.0, 0, 1.0, -1.0);
    }
}

void GPUupdateLace() {
	GPUstatusRet ^= GPUSTATUS_ODDLINES;
    glFlush();
}

uw GPUreadStatus() {
	return GPUstatusRet | GPUSTATUS_READYFORVRAM;
}

void GPUwriteStatus(uw gdata) {
	switch((gdata >> 24) & 0xff) {
        case 0x00:
            GPUstatusRet = 0x14802000;
            return;
            
        case 0x04:
            if ((gdata & 0xffffff) == 0) imageTransfer = 0;
            if ((gdata & 0xffffff) == 2) imageTransfer = 3;
            return;
            
        case 0x08:
            psxDisp.modeX = dispWidths[(gdata & 3) | ((gdata & 0x40) >> 4)];
            
            if (gdata & 0x04)
                psxDisp.modeY = 480;
            else
                psxDisp.modeY = 240;
            
            updateDisplay();
            return;
            
        case 0x03:
        case 0x05:
        case 0x06:
        case 0x07:
            return;
    }
}

uw GPUreadData() {
	return GPUdataRet;
}

sw PullFromPsxRam(uw *pMem, sw size) {
	int count = 0;
	uh *input = (uh *)pMem;
	
	sh x2 = vramWrite.x + vramWrite.w;
	sh y2 = vramWrite.y + vramWrite.h;
	
	uh posx, posy;
    
	if (vramWrite.enabled == 0) {
		imageTransfer = 0;
		return 0;
	}
    
    size <<= 1;
    
	while(vramWrite.cury < y2) {
		posy = (uh)vramWrite.cury;
        if (posy >= 512) posy = 0;
		
        while(vramWrite.curx < x2) {
			posx = (uh) vramWrite.curx;
			if (posx >= 1024) posx = 0;
            
			psxVuw[(posy << 10) + posx] = *input;
			vramWrite.curx++;
			count++;
			input++;
			
			if (count == size) {
				if (vramWrite.curx == x2) {
					vramWrite.cury++;
					vramWrite.curx = vramWrite.x;
				}
				goto NOMOREIMAGEDATA;
			}
		}
		vramWrite.cury++;
		vramWrite.curx = vramWrite.x;
	}
    
NOMOREIMAGEDATA:
	if (vramWrite.cury >= y2) {
		vramWrite.enabled = 0;
		imageTransfer = 0;
		if (count % 2 == 1)
			count++;
	}
    
	return count >> 1;
}

void GPUwriteDataMem(uw * pMem, sw iSize) {
	ub command;
	uw gdata = 0;
	
	for (int i = 0; i < iSize;) {
		if ((imageTransfer & 1) == 1) {
			i += PullFromPsxRam(pMem, iSize - i);
			if (i >= iSize) continue;
			pMem += i;
		}
		
		gdata = *pMem;
		GPUdataRet = gdata;
		pMem++;
		i++;
		
		if (gpuDataC == 0) {
			command = (ub)(gdata >> 24) & 0xff;
			if (primTableC[command]) {
				gpuDataC = primTableC[command];
				gpuCommand = command;
				gpuData[0] = gdata;
				gpuDataP = 1;
			}
			else continue;
		}
		else {
			gpuData[gpuDataP] = gdata;
			gpuDataP++;
		}
		
        if (gpuDataP == gpuDataC) {
			gpuDataC = gpuDataP = 0;
			primTableJ[gpuCommand]((ub *)gpuData);
		}
	}
    
	GPUdataRet = gdata;
}

void GPUwriteData(uw gdata) {
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
