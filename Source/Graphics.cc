#include "Global.h"

CstrGraphics vs;

void CstrGraphics::init() {
    psxVuw = (uh *)psxVub;
    psxVul = (uw *)psxVub;

    statusRet = 0x74000000;
    memset(infoVals, 0x00, 16 * sizeof(uw));
}

void CstrGraphics::reset() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0f / 256.0f, 1.0f / 256.0f, 1.0f);

    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < 384; i++) {
        texture[i].x = 0;
        texture[i].y = 0;
        texture[i].tp = 0;
        texture[i].clut = nullclutP;
        texture[i].update = false;
        glGenTextures  (1, &texture[i].id);
        glBindTexture  (GL_TEXTURE_2D, texture[i].id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexEnvi      (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D   (GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    
    glGenTextures  (1, &xferTexture24);
    glBindTexture  (GL_TEXTURE_2D,xferTexture24);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D   (GL_TEXTURE_2D, 0, 3, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glGenTextures  (1, &xferTexture16);
    glBindTexture  (GL_TEXTURE_2D,xferTexture16);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D   (GL_TEXTURE_2D, 0, 4, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    torgba[0] = 0x00000000;
    for (int i = 1; i <65536; i++) {
        torgba[i]  = (i & 0x001f) << 3;
        torgba[i] |= (i & 0x03e0) << 6;
        torgba[i] |= (i & 0x7c00) << 9;
        torgba[i] |= 0xfe000000;
        if (!(i & 0x8000))
            torgba[i] |= 0xff000000;
    }
    torgba[0x8000] = 0xff000000;

    for(int i = 0; i < 128; i++) {
        texshade[i] = i<<1;
    }
    
    for(int i = 128; i < 256; i++) {
        texshade[i] = 255;
    }
}

void CstrGraphics::updateScreenMode() {
    if (psxDisp.modeX > 0 && psxDisp.modeY > 0) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glScalef(1.0f, -1.0f, 1.0f);
        glOrtho(0, psxDisp.modeX, 0, psxDisp.modeY, 1.0, -1.0);
    }
}

void CstrGraphics::updateLace() {
    drawingLines ^= 1;
    [[app.openGLView openGLContext] flushBuffer];
}

uw CstrGraphics::readStatus() {
    if (drawingLines == 1) {
        return statusRet;
    }
    else {
        return statusRet | 0x80000000;
    }
}

void CstrGraphics::writeStatus(uw gdata) {
    switch((gdata >> 24) & 0xff) {
        case 0x00:
            memset(infoVals, 0x00, 16 * sizeof(uw));
            statusRet=0x14802000;
            psxDisp.disabled = 1;
            psxDraw.offsetX = psxDraw.offsetY = psxDraw.clipX1 = psxDraw.clipX2 = psxDraw.clipY1 = psxDraw.clipY2 = 0;
            texinfo.mirror=0;
            texinfo.x = texinfo.y = 0;
            texinfo.colormode = texinfo.abr = 0;
            psxDisp.colordepth24 = FALSE;
            psxDisp.changed = 1;
            return;
            
        case 0x03:
            psxDisp.disabled = gdata & 1;
            if (psxDisp.disabled) {
                statusRet |= GPUSTATUS_DISPLAYDISABLED;
            }
            else {
                statusRet &=~GPUSTATUS_DISPLAYDISABLED;
            }
            return;
        
        case 0x04:
            if ((gdata & 0xffffff) == 0) imageTransfer = 0;
            if ((gdata & 0xffffff) == 2) imageTransfer = 3;
            statusRet &=~GPUSTATUS_DMABITS;
            statusRet |= gdata << 29;
            return;
   
        case 0x05:
            oldpsxDisp.startY = psxDisp.startY;
            oldpsxDisp.startX = psxDisp.startX;

            psxDisp.startY = (gdata >> 10) & 0x3ff;
            psxDisp.startX = gdata & 0x3ff;
        
            if (oldpsxDisp.startY != psxDisp.startY || oldpsxDisp.startX != psxDisp.startX) {
                psxDisp.changed = 1;
            }
            updateScreenMode();
            return;
        
        case 0x08:
            psxDisp.modeX = dispWidths[(gdata & 0x3) | ((gdata & 0x40) >> 4)];
            
            if (gdata & 0x04) {
                psxDisp.modeY = 480;
            }
            else {
                psxDisp.modeY = 240;
            }
            
            psxDisp.colordepth24 = (gdata >> 4) & 0x1;
            
            if (psxDisp.colordepth24) {
                statusRet |= GPUSTATUS_RGB24;
            }
            else {
                statusRet &=~GPUSTATUS_RGB24;
            }
            
            psxDisp.pal = (gdata & 0x08) ? TRUE : FALSE;
            
            if (psxDisp.pal) {
                statusRet |= GPUSTATUS_PAL;
            }
            else {
                statusRet &=~GPUSTATUS_PAL;
            }
            
            psxDisp.changed = 1;
            return;
        
        case 0x10:
            gdata &= 0xff;
            
            switch(gdata) {
                case 0x02:
                    GPUdataRet = infoVals[INFO_TW];
                    return;
                
                case 0x03:
                    GPUdataRet = infoVals[INFO_DRAWSTART];
                    return;
                
                case 0x04:
                    GPUdataRet = infoVals[INFO_DRAWEND];
                    return;
                
                case 0x05:
                case 0x06:
                    GPUdataRet = infoVals[INFO_DRAWOFF];
                    return;
                
                case 0x07:
                    GPUdataRet = 0x02;
                    return;
                
                case 0x08:
                case 0x0f:
                    GPUdataRet = 0xbfc03720;
                    return;
                
                default:
                    return;
            }
            return;
            
        case 0x06:
        case 0x07:
            return;
    }
}

uw CstrGraphics::readData() {
    if (imageTransfer == 2) {
        if ((imTY >= 0) && (imTY < 512) && (imTX >= 0) && (imTX < 1024)) {
            GPUdataRet = psxVul[imTY * 512 + imTX / 2];
        }
        
        imTX  += 2;
        imTXc -= 2;
        
        if(imTXc <= 0) {
            imTX  = imageX0;
            imTXc = imageX1;
            imTYc-= 1;
            imTY += 1;
        }
        
        imSize--;
        
        if(imSize <= 0) {
            statusRet &= 0xf7ffffff;
            imageTransfer = 0;
        }
    }
    return GPUdataRet;
}

void CstrGraphics::readDataMem(uw *pMem, int iSize) {
    for(int i = 0; i < iSize; i++) {
        pMem[i] = readData();
    }
}

sw CstrGraphics::pullFromPsxRam(uw *pMem, sw size) {
    sw count = 0;
    uh *input = (uh *)pMem;
    uw *t = vramWrite.extratarget;
    uh *st = (uh *)t;

    sh x2 = vramWrite.x + vramWrite.w;
    sh y2 = vramWrite.y + vramWrite.h;
    
    uh posx, posy;

    if (vramWrite.enabled == 0) {
        imageTransfer = 0;
        return 0;
    }

    size <<= 1;

    while(vramWrite.cury < y2) {
        posy = (uh) vramWrite.cury;
        
        if (posy >= 512) {
            posy = 0;
        }
        
        while(vramWrite.curx < x2) {
            posx = (uh) vramWrite.curx;
            
            if (posx >= 1024) {
                posx = 0;
            }

            if (!psxDisp.colordepth24) {
                *t++ = torgba[*input];
            }
            else {
                *st++ = *input;
            }

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
        float x,w;

        vramWrite.enabled = 0;

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glScalef(1.0f/1024.0f, 1.0f/512.0f, 1.0f);

        if (psxDisp.colordepth24) {
            x = vramWrite.x * 2 / 3;
            w = vramWrite.w * 2 / 3;
            glBindTexture(GL_TEXTURE_2D, xferTexture24);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, vramWrite.h, GL_RGB, GL_UNSIGNED_BYTE, vramWrite.extratarget);
        }
        else {
            x = vramWrite.x;
            w = vramWrite.w;
            glBindTexture(GL_TEXTURE_2D, xferTexture16);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, vramWrite.h, GL_RGBA, GL_UNSIGNED_BYTE, vramWrite.extratarget);
        }

        glDisable(GL_BLEND);
        glDisable(GL_CLIP_PLANE0);
        glDisable(GL_CLIP_PLANE1);
        glDisable(GL_CLIP_PLANE2);
        glDisable(GL_CLIP_PLANE3);
        
        glColor3ub(255, 255, 255);
        
        glBegin(GL_POLYGON);
            glTexCoord2s(0, 0);
            glVertex2s(x, vramWrite.y);

            glTexCoord2s(w, 0);
            glVertex2s(x + w, vramWrite.y);

            glTexCoord2s(w, vramWrite.h);
            glVertex2s(x + w, vramWrite.y + vramWrite.h);

            glTexCoord2s(0, vramWrite.h);
            glVertex2s(x, vramWrite.y + vramWrite.h);
        glEnd();
        glEnable(GL_BLEND);
        
        glPopMatrix();
        
        glEnable(GL_CLIP_PLANE0);
        glEnable(GL_CLIP_PLANE1);
        glEnable(GL_CLIP_PLANE2);
        glEnable(GL_CLIP_PLANE3);
        
        free(vramWrite.extratarget);
        imageTransfer = 0;
        
        if (count % 2 == 1) {
            count++;
        }
    }
    
    return count >> 1;
}

void CstrGraphics::writeDataMem(uw * pMem, sw iSize) {
    ub command;
    uw gdata;
    int i = 0;
    
    statusRet &= ~GPUSTATUS_IDLE;
    statusRet &= ~GPUSTATUS_READYFORCOMMANDS;

    for (; i < iSize;) {
        if ((imageTransfer & 1) == 1) {
            i += pullFromPsxRam(pMem, iSize - i);
            if (i >= iSize) {
                continue;
            }
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
            else {
                continue;
            }
        }
        else {
            gpuData[gpuDataP] = gdata;
            if (gpuDataC > 128) {
                if ((gpuDataC == 254 && gpuDataP >= 3) || (gpuDataC == 255 && gpuDataP >= 4 && !(gpuDataP & 1))) {
                    if ((gpuData[gpuDataP] & 0xf000f000) == 0x50005000) {
                        gpuDataP = gpuDataC - 1;
                    }
                }
            }
            gpuDataP++;
        }
        
        if (gpuDataP == gpuDataC) {
            gpuDataC = gpuDataP = 0;
            primTableJ[gpuCommand]((ub *)gpuData);
        }
    }

    GPUdataRet = gdata;

    statusRet |= GPUSTATUS_READYFORCOMMANDS;
    statusRet |= GPUSTATUS_IDLE;
}

void CstrGraphics::writeData(uw gdata) {
    writeDataMem(&gdata, 1);
}

void CstrGraphics::executeDMA(CstrBus::castDMA *dma) {
    uw *p   = (uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    
    switch(dma->chcr) {
            case 0x01000200:
                readDataMem(p, size);
                return;
                
            case 0x01000201:
                writeDataMem(p, size);
                return;
                
            case 0x01000401:
                do {
                    uw hdr = *(uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
                    p = (uw *)&mem.ram.ptr[(dma->madr + 4) & 0x1ffffc];
                    writeDataMem(p, hdr >> 24);
                    dma->madr = hdr & 0xffffff;
                }
                while(dma->madr != 0xffffff);
                return;
                
            /* unused */
            case 0x00000401: // Disable DMA?
                return;
        }
        
        printx("/// PSeudo GPU DMA: 0x%x", dma->chcr);
}
