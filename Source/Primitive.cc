#import "Global.h"


#ifndef MYGPU

ub texshade[256];
uw torgba[65536];
uw image[65536];
sb curText = 0;
TextureState_t texinfo;
struct texturecache texture[64];
GLuint nullid;

const struct {
	sw src, dst; ub alpha;
} TransRate[4] = {
	{ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128 },
	{ GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0 },
	{ GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0 },
	{ GL_SRC_ALPHA, GL_ONE,                  64 },
};

bool transparent = false;
ub gAlpha;

void cmdSTP(ub *baseAddr) {
}

void UpdateTextureInfo(uw gdata) {
	texinfo.x = (gdata << 6) & 0x3c0;
	texinfo.y = (gdata << 4) & 0x100;
	texinfo.colormode = ( gdata >> 7 ) & 0x3;
	if (texinfo.colormode == 3) texinfo.colormode = 2;
	texinfo.abr = (gdata >> 5) & 0x3;
	texinfo.mirror = gdata&0x3000 >> 12;
}

void cmdTexturePage(ub *baseAddr) {
	uw gdata = ((uw *)baseAddr)[0];
	sw tempABR = (gdata >> 5) & 0x3;
	if (texinfo.abr != tempABR) {
        texinfo.abr = tempABR;
        glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst);
        gAlpha = TransRate[texinfo.abr].alpha;
		transparent = true;
	}
	UpdateTextureInfo(gdata);
}

void cmdTextureWindow(ub *baseAddr) {
	uw gdata = ((uw *)baseAddr)[0];
	GPUInfoVals[INFO_TW] = gdata & 0xfffff;
	uw YAlign, XAlign;
    
	if (gdata & 0x020)
		psxDraw.texwinY2 =   8;
	else if (gdata & 0x040)
		psxDraw.texwinY2 =  16;
	else if (gdata & 0x080)
		psxDraw.texwinY2 =  32;
	else if (gdata & 0x100)
		psxDraw.texwinY2 =  64;
	else if (gdata & 0x200)
		psxDraw.texwinY2 = 128;
	else
		psxDraw.texwinY2 = 256;
	
	if (gdata & 0x001)
		psxDraw.texwinX2 =   8;
	else if (gdata & 0x002)
		psxDraw.texwinX2 =  16;
	else if (gdata & 0x004)
		psxDraw.texwinX2 =  32;
	else if (gdata & 0x008)
        psxDraw.texwinX2 =  64;
	else if (gdata & 0x010)
		psxDraw.texwinX2 = 128;
	else
		psxDraw.texwinX2 = 256;
	
	YAlign = (uw)(32 - (psxDraw.texwinY2 >> 3));
	XAlign = (uw)(32 - (psxDraw.texwinX2 >> 3));
	
	psxDraw.texwinY1 = (sh)(((gdata >> 15) & YAlign) << 3);
	psxDraw.texwinX1 = (sh)(((gdata >> 10) & XAlign) << 3);
	
	if ((psxDraw.texwinX1 == 0 && psxDraw.texwinY1 == 0 && psxDraw.texwinX2 == 0 && psxDraw.texwinY2 == 0) || (psxDraw.texwinX2 == 256 && psxDraw.texwinY2 == 256)) {
        psxDraw.texwinenabled = 0;
    }
	else {
        psxDraw.texwinenabled = 1;
	}
}

void cmdDrawAreaStart(ub *baseAddr) {
	uw gdata = ((uw *)baseAddr)[0];
	GPUInfoVals[INFO_DRAWSTART] = gdata & 0x3fffff;
	psxDraw.clipX1 = gdata & 0x3ff;
	psxDraw.clipY1 = (gdata >> 10) & 0x1ff;
    
	GLdouble equation[4] = { 0.0, 0.0, 0.0, 0.0 };
	equation[0] = 1.0;
	equation[3] = (GLdouble) - (psxDraw.clipX1);
	glClipPlane(GL_CLIP_PLANE0, equation);
    
	equation[0] = 0.0;
	equation[1] = 1.0;
	equation[3] = (GLdouble) - (psxDraw.clipY1);
	glClipPlane(GL_CLIP_PLANE1, equation);
}

void cmdDrawAreaEnd(ub *baseAddr) {
	uw gdata = ((uw *)baseAddr)[0];
	GPUInfoVals[INFO_DRAWEND] = gdata & 0x3fffff;
	psxDraw.clipX2 = gdata & 0x3ff;
	psxDraw.clipY2 = (gdata >> 10) & 0x1ff;
    
	GLdouble equation[4] = { 0.0, 0.0, 0.0, 0.0 };
	equation[0] = -1.0;
	equation[3] = (GLdouble)(psxDraw.clipX2);
	glClipPlane(GL_CLIP_PLANE2, equation);
    
	equation[0] = 0.0;
	equation[1] = -1.0;
	equation[3] = (GLdouble)(psxDraw.clipY2);
	glClipPlane(GL_CLIP_PLANE3, equation);
}

void cmdDrawOffset(ub *baseAddr) {
	uw gdata = ((uw *)baseAddr)[0];
	GPUInfoVals[INFO_DRAWOFF] = gdata & 0x7fffff;

	psxDraw.offsetX = (sh)(gdata & 0x7ff);
	psxDraw.offsetY = (sh)((gdata >> 11) & 0x7ff);

	psxDraw.offsetX = (sh)(((sw)psxDraw.offsetX << 21) >> 21);
	psxDraw.offsetY = (sh)(((sw)psxDraw.offsetY << 21) >> 21);
}

void primLoadImage(ub *baseAddr) {
	sh *sgpuData = (sh *)baseAddr;
	
	sh x = (sgpuData[2] << 21) >> 21;
	sh y = (sgpuData[3] << 21) >> 21;
	sh w = (sgpuData[4] << 21) >> 21;
	sh h = (sgpuData[5] << 21) >> 21;
    
	if (w == -1024) w = 1024;
	if (h == -512) h = 512;
    
	if (x < 0 || y < 0 || w < 0 || h < 0 || x + w > 1024 || y + h > 512) {
		return;
	}
    
	vramWrite.curx = vramWrite.x = x;
	vramWrite.cury = vramWrite.y = y;
	vramWrite.w = w;
	vramWrite.h = h;
	vramWrite.extratarget = (uw *)malloc(vramWrite.w * vramWrite.h * 4);
	vramWrite.enabled = 1;
    
	imageTransfer = 1;
}

void primStoreImage(ub *baseAddr) {
}

void primMoveImage(ub *baseAddr) {
}

void primTileS(ub *baseAddr) {
	uh *gpuPoint = (uh *)baseAddr;
    
	sh x = (gpuPoint[2] << 21) >> 21;
	sh y = (gpuPoint[3] << 21) >> 21;
	sh w = (gpuPoint[4] << 21) >> 21;
	sh h = (gpuPoint[5] << 21) >> 21;
    
	x += psxDraw.offsetX;
	y += psxDraw.offsetY;
    
	if (baseAddr[3] & 2) {
        glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst);
        gAlpha = TransRate[texinfo.abr].alpha;
        transparent = TRUE;
		glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], gAlpha);
	}
    else {
		glBlendFunc(TransRate[0].src, TransRate[0].dst);
        transparent = FALSE;
		glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
	}
    
	glBegin(GL_TRIANGLE_STRIP);
        glVertex2s(x,     y);
        glVertex2s(x + w, y);
        glVertex2s(x,     y + h);
        glVertex2s(x + w, y + h);
	glEnd();
}

void primBlkFill(ub *baseAddr) {
	sh * gpuPoint = (sh *) baseAddr;
    
	sh x = (gpuPoint[2] << 21) >> 21;
	sh y = (gpuPoint[3] << 21) >> 21;
	sh w = (gpuPoint[4] << 21) >> 21;
	sh h = (gpuPoint[5] << 21) >> 21;
    
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
    
	glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
    
	glBegin(GL_TRIANGLE_STRIP);
        glVertex2s(x,     y);
        glVertex2s(x + w, y);
        glVertex2s(x,     y + h);
        glVertex2s(x + w, y + h);
	glEnd();
    
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	glEnable(GL_CLIP_PLANE2);
	glEnable(GL_CLIP_PLANE3);
}

void setupTexture(sw clutP) {
	sw tbw, ctext;
	sh color, tC;
	union uPointers psxVOffset, psxVOffset2;
	if ((texinfo.colormode & 2) == 2) clutP = nullclutP;
	ctext = curText;
    
	for (int i = 0; i < 64; i++) {
		if (texture[i].textAddrX == texinfo.x && texture[i].textAddrY == texinfo.y && clutP == texture[i].clutP) {
            if (texture[i].Update == true || texinfo.colormode != texture[i].textTP) {
                ctext = i;
                break;
            }
            glBindTexture(GL_TEXTURE_2D,texture[i].id);
            return;
        }
    }
    
	texture[ctext].textAddrX = texinfo.x;
	texture[ctext].textAddrY = texinfo.y;
	texture[ctext].textTP    = texinfo.colormode;
	texture[ctext].clutP     = clutP;
	texture[ctext].Update    = false;
    
	glBindTexture(GL_TEXTURE_2D, texture[ctext].id);
    
	if (ctext == curText) {
        curText++;
        if (curText >= 64) curText = 0;
	}
    
	tbw = 256;
	psxVOffset.w = psxVuw + (texinfo.y << 10) + texinfo.x;
    
	uw *pimage = image;

	switch(texinfo.colormode) {
        case 0:
            for (int sprCY = 0; sprCY < 256; sprCY++) {
                psxVOffset2.b = psxVOffset.b;
                
                for (int sprCX = 0; sprCX < 256; sprCX += 2) {
                    tC = (*psxVOffset2.b) & 0x0f;
                    *pimage = torgba[psxVuw[clutP + tC]];
                    pimage++;
                    
                    tC = (*psxVOffset2.b >> 4) & 0x0f;
                    *pimage = torgba[psxVuw[clutP + tC]];
                    pimage++;
                    
                    psxVOffset2.b++;
                }
                psxVOffset.w += 1024;
            }
            break;
        
        case 1:
            if (texinfo.x == 960) tbw = 128;
            
            for (int sprCY = 0; sprCY < 256; sprCY++) {
                psxVOffset2.b = psxVOffset.b;
                
                for (int sprCX = 0; sprCX < tbw; sprCX++) {
                    tC = *psxVOffset2.b;
                    image[(sprCY << 8) + sprCX] = torgba[psxVuw[clutP + tC]];
                    
                    psxVOffset2.b++;
                }
                psxVOffset.w += 1024;
            }
            break;
        
        case 2:
            if (texinfo.x == 960)
                tbw = 64;
            else if (texinfo.x == 896)
                tbw = 128;
            else if (texinfo.x == 832)
                tbw = 192;
            
            for (int sprCY = 0; sprCY < 256; sprCY++) {
                psxVOffset2.w = psxVOffset.w;
                
                for (int sprCX = 0; sprCX < tbw; sprCX++) {
                    color = *psxVOffset2.w;
                    image[(sprCY << 8) + sprCX] = torgba[color];
                    
                    psxVOffset2.w++;
                }
                psxVOffset.w += 1024;
            }
            break;
    }
    
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void setSpriteBlendMode(sw command) {
	ub *baseAddr = (ub *)&command;
	ub r = texshade[baseAddr[0]];
	ub g = texshade[baseAddr[1]];
	ub b = texshade[baseAddr[2]];
    
	if (baseAddr[3] & 2) {
		glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst);
        transparent = true;
		
        if (baseAddr[3] & 1) {
			glColor4ub(255, 255, 255, gAlpha);
		}
        else {
			glColor4ub(r, g, b, gAlpha);
		}
	}
    else {
        glBlendFunc(TransRate[0].src, TransRate[0].dst);
        transparent = false;
		
        if (baseAddr[3] & 1) {
			glColor3ub(255, 255, 255);
		}
        else {
			glColor3ub(r, g, b);
        }
	}
}

#define sprite(n) \
    uw *gpuData  = (uw *)baseAddr; \
    sh *gpuPoint = (sh *)baseAddr; \
    \
    sw clutP = (gpuData[2] >> 12) & 0x7fff0; \
    sh tx = (sh)(gpuData[2] & 0xff); \
    sh ty = (sh)((gpuData[2] >> 8) & 0xff); \
    \
    sh x = (gpuPoint[2] << 21) >> 21; \
    sh y = (gpuPoint[3] << 21) >> 21; \
    \
    x += psxDraw.offsetX; \
    y += psxDraw.offsetY; \
    \
    setupTexture(clutP); \
    \
    ub r = texshade[baseAddr[0]]; \
    ub g = texshade[baseAddr[1]]; \
    ub b = texshade[baseAddr[2]]; \
    \
    if (baseAddr[3] & 2) { \
        glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst); \
        transparent = true; \
         \
        if (baseAddr[3] & 1) { \
            glColor4ub(255, 255, 255, gAlpha); \
        } \
        else { \
            glColor4ub(r, g, b, gAlpha); \
        } \
    } \
    else { \
        glBlendFunc(TransRate[0].src, TransRate[0].dst); \
        transparent = false; \
         \
        if (baseAddr[3] & 1) { \
            glColor3ub(255, 255, 255); \
        } \
        else { \
            glColor3ub(r, g, b); \
        } \
    } \
    \
    glBegin(GL_TRIANGLE_STRIP); \
        glTexCoord2s(tx,   ty);   glVertex2s(x,   y); \
        glTexCoord2s(tx+n, ty);   glVertex2s(x+n, y); \
        glTexCoord2s(tx,   ty+n); glVertex2s(x,   y+n); \
        glTexCoord2s(tx+n, ty+n); glVertex2s(x+n, y+n); \
    glEnd();\
    \
    if (baseAddr[3] & 2) { \
        glEnable(GL_ALPHA_TEST); \
        glAlphaFunc(GL_EQUAL, 1); \
        glColor3ub(255, 255, 255); \
        glBegin(GL_TRIANGLE_STRIP); \
            glTexCoord2s(tx,   ty);   glVertex2s(x,   y); \
            glTexCoord2s(tx+n, ty);   glVertex2s(x+n, y); \
            glTexCoord2s(tx,   ty+n); glVertex2s(x,   y+n); \
            glTexCoord2s(tx+n, ty+n); glVertex2s(x+n, y+n); \
        glEnd(); \
        glDisable(GL_ALPHA_TEST); \
    } \
    glBindTexture(GL_TEXTURE_2D, nullid)

void primSprt8(ub *baseAddr) {
    sprite(8);
}

void primSprt16(ub *baseAddr) {
	sprite(16);
}

void primSprtS(ub *baseAddr) {
	uw *gpuData  = (uw *)baseAddr;
	sh *gpuPoint = (sh *)baseAddr;
    
    sw clutP = (gpuData[2] >> 12) & 0x7fff0;
    sh tx = (sh)(gpuData[2] & 0x000000ff);
    sh ty = (sh)((gpuData[2] >> 8) & 0x000000ff);
	sh sprtW = (sh)(gpuData[3] & 0x3ff);
	sh sprtH = (sh)((gpuData[3] >> 16) & 0x1ff);
    
	sh x = (gpuPoint[2] << 21) >> 21;
	sh y = (gpuPoint[3] << 21) >> 21;
    
	x += psxDraw.offsetX;
	y += psxDraw.offsetY;
    
	setupTexture(clutP);
	
    ub r = texshade[baseAddr[0]];
    ub g = texshade[baseAddr[1]];
    ub b = texshade[baseAddr[2]];

    if (baseAddr[3] & 2) {
        glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst);
        transparent = true;
        
        if (baseAddr[3] & 1) {
            glColor4ub(255, 255, 255, gAlpha);
        }
        else {
            glColor4ub(r, g, b, gAlpha);
        }
    }
    else {
        glBlendFunc(TransRate[0].src, TransRate[0].dst);
        transparent = false;
        
        if (baseAddr[3] & 1) {
            glColor3ub(255, 255, 255);
        }
        else {
            glColor3ub(r, g, b);
        }
    }

	glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2s(tx,       ty);       glVertex2s(x,       y);
        glTexCoord2s(tx+sprtW, ty);       glVertex2s(x+sprtW, y);
        glTexCoord2s(tx,       ty+sprtH); glVertex2s(x,       y+sprtH);
        glTexCoord2s(tx+sprtW, ty+sprtH); glVertex2s(x+sprtW, y+sprtH);
	glEnd();
	
	if(baseAddr[3]&2){
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_EQUAL, 1);
        //glDisable(GL_BLEND);
        glColor3ub(255,255,255);
        glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2s(tx,       ty);       glVertex2s(x,       y);
            glTexCoord2s(tx+sprtW, ty);       glVertex2s(x+sprtW, y);
            glTexCoord2s(tx,       ty+sprtH); glVertex2s(x,       y+sprtH);
            glTexCoord2s(tx+sprtW, ty+sprtH); glVertex2s(x+sprtW, y+sprtH);
        glEnd();
        //glEnable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
    }

	glBindTexture(GL_TEXTURE_2D,nullid);
}

void primLineF2(ub *baseAddr) {
	sh *gpuPoint = (sh *)baseAddr;
    
	sh x1 = (gpuPoint[2] << 21) >> 21;
	sh y1 = (gpuPoint[3] << 21) >> 21;
	sh x2 = (gpuPoint[4] << 21) >> 21;
	sh y2 = (gpuPoint[5] << 21) >> 21;
    
	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;
    
	if (baseAddr[3] & 2) {
        if (!transparent) {
            glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst);
            transparent = true;
        }
        glColor4ub(baseAddr[0],baseAddr[1],baseAddr[2], gAlpha);
    }
    else {
        if (transparent) {
            glBlendFunc(TransRate[0].src, TransRate[0].dst);
			transparent = false;
        }
        glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
    }
    
    glBegin(GL_LINES);
        glVertex2s(x1, y1);
		glVertex2s(x2, y2);
	glEnd();
}

void primLineG2(ub *baseAddr) {
	sh *gpuPoint = (sh *)baseAddr;
	ub alpha;

	sh x1 = (gpuPoint[2] << 21) >> 21;
	sh y1 = (gpuPoint[3] << 21) >> 21;
	sh x2 = (gpuPoint[6] << 21) >> 21;
	sh y2 = (gpuPoint[7] << 21) >> 21;
    
	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;

	if (baseAddr[3] & 2) {
        if (!transparent) {
            glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst);
            transparent = true;
        }
		alpha = gAlpha;
	}
    else {
        if (transparent) {
            glBlendFunc(TransRate[0].src, TransRate[0].dst);
            transparent = false;
        }
		alpha = 255;
	}
    
	glBegin(GL_LINES);
		glColor4ub(baseAddr[0], baseAddr[1], baseAddr[ 2], alpha);
		glVertex2s(x1, y1);
		glColor4ub(baseAddr[8], baseAddr[9], baseAddr[10], alpha);
		glVertex2s(x2, y2);
	glEnd();
}

#define polyf(n) \
    sh *gpuPoint = (sh *)baseAddr; \
    \
    sh vx[4] = { \
        ((gpuPoint[2] << 21) >> 21) + psxDraw.offsetX, \
        ((gpuPoint[4] << 21) >> 21) + psxDraw.offsetX, \
        ((gpuPoint[6] << 21) >> 21) + psxDraw.offsetX, \
        ((gpuPoint[8] << 21) >> 21) + psxDraw.offsetX, \
    }; \
    \
    sh vy[4] = { \
        ((gpuPoint[3] << 21) >> 21) + psxDraw.offsetY, \
        ((gpuPoint[5] << 21) >> 21) + psxDraw.offsetY, \
        ((gpuPoint[7] << 21) >> 21) + psxDraw.offsetY, \
        ((gpuPoint[9] << 21) >> 21) + psxDraw.offsetY, \
    }; \
    \
    if (baseAddr[3] & 2) { \
        if (!transparent) { \
            glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst); \
            transparent = true; \
        } \
        gAlpha = TransRate[texinfo.abr].alpha; \
        glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], gAlpha); \
    } \
    else { \
        if (transparent) { \
            glBlendFunc(TransRate[0].src, TransRate[0].dst); \
            transparent = false; \
        } \
        glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]); \
    } \
    \
    glBegin(GL_TRIANGLE_STRIP); \
    for (int i = 0; i < n; i++) { \
        glVertex2s(vx[i], vy[i]); \
    } \
    glEnd()

void primPolyF3(ub *baseAddr) {
    polyf(3);
}

void primPolyF4(ub *baseAddr) {
    polyf(4);
}

#define polyg(n) \
    sh *gpuPoint = (sh *)baseAddr; \
    ub alpha; \
    \
    sh vx[4] = { \
        ((gpuPoint[ 2] << 21) >> 21) + psxDraw.offsetX, \
        ((gpuPoint[ 6] << 21) >> 21) + psxDraw.offsetX, \
        ((gpuPoint[10] << 21) >> 21) + psxDraw.offsetX, \
        ((gpuPoint[14] << 21) >> 21) + psxDraw.offsetX, \
    }; \
    \
    sh vy[4] = { \
        ((gpuPoint[ 3] << 21) >> 21) + psxDraw.offsetY, \
        ((gpuPoint[ 7] << 21) >> 21) + psxDraw.offsetY, \
        ((gpuPoint[11] << 21) >> 21) + psxDraw.offsetY, \
        ((gpuPoint[15] << 21) >> 21) + psxDraw.offsetY, \
    }; \
    \
    ub cr[4] = { \
        baseAddr[ 0], \
        baseAddr[ 8], \
        baseAddr[16], \
        baseAddr[24], \
    }; \
    \
    ub cg[4] = { \
        baseAddr[ 1], \
        baseAddr[ 9], \
        baseAddr[17], \
        baseAddr[25], \
    }; \
    \
    ub cb[4] = { \
        baseAddr[ 2], \
        baseAddr[10], \
        baseAddr[18], \
        baseAddr[26], \
    }; \
    \
    if (baseAddr[3] & 2) { \
        if (!transparent) { \
            glBlendFunc(TransRate[texinfo.abr].src, TransRate[texinfo.abr].dst); \
            transparent = true; \
        } \
        alpha = TransRate[texinfo.abr].alpha; \
    } \
    else { \
        if (transparent) { \
            glBlendFunc(TransRate[0].src, TransRate[0].dst); \
            transparent = false; \
        } \
        alpha = 255; \
    } \
    glBegin(GL_TRIANGLE_STRIP); \
    for (int i = 0; i < n; i++) { \
        glColor4ub(cr[i], cg[i], cb[i], alpha); \
        glVertex2s(vx[i], vy[i]); \
    } \
    glEnd()

void primPolyG3(ub *baseAddr) {
    polyg(3);
}

void primPolyG4(ub *baseAddr) {
    polyg(4);
}

void primPolyFT3(unsigned char * baseAddr)
{
	uint32_t *gpuData = (uint32_t *)baseAddr;
	short *gpuPoint=((short *) baseAddr);
	int32_t clutP;
	short x1,x2,x3,y1,y2,y3;
	short tx0,ty0,tx1,ty1,tx2,ty2;
	unsigned short gpuDataX;

	x1 = (gpuPoint[2]<<21)>>21;
	y1 = (gpuPoint[3]<<21)>>21;
	x2 = (gpuPoint[6]<<21)>>21;
	y2 = (gpuPoint[7]<<21)>>21;
	x3 = (gpuPoint[10]<<21)>>21;
	y3 = (gpuPoint[11]<<21)>>21;

	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;
	x3 += psxDraw.offsetX;
	y3 += psxDraw.offsetY;

	tx0 = (short)(gpuData[2] & 0xff);
	ty0 = (short)((gpuData[2]>>8) & 0xff);
	tx1 = (short)(gpuData[4] & 0xff);
	ty1 = (short)((gpuData[4]>>8) & 0xff);
	tx2 = (short)(gpuData[6] & 0xff);
	ty2 = (short)((gpuData[6]>>8) & 0xff);
	gpuDataX=(unsigned short)(gpuData[4]>>16);

	int32_t tempABR = (gpuDataX >>5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
		gAlpha=TransRate[texinfo.abr].alpha;
		transparent=TRUE;
	}
	UpdateTextureInfo(gpuDataX);

    clutP  = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	
	baseAddr[0] = texshade[baseAddr[0]];
	baseAddr[1] = texshade[baseAddr[1]];
	baseAddr[2] = texshade[baseAddr[2]];

	if(baseAddr[3]&2){
		if(!transparent){
			glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
			transparent=TRUE;
		}
		if(baseAddr[3]&1){
			glColor4ub(255,255,255,gAlpha);
		}else{
			glColor4ub(baseAddr[0],baseAddr[1],baseAddr[2],gAlpha);
		}
	}else{
		glBlendFunc(TransRate[0].src,TransRate[0].dst);
        transparent=FALSE;
		if(baseAddr[3]&1){
			glColor3ub(255,255,255);
		}else{
			glColor3ub(baseAddr[0],baseAddr[1],baseAddr[2]);
		}
	}

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
    
	glBegin(GL_POLYGON);
		glTexCoord2s(tx0,ty0);
		glVertex2s(x1,y1);
		glTexCoord2s(tx1,ty1);
		glVertex2s(x2,y2);
		glTexCoord2s(tx2,ty2);
		glVertex2s(x3,y3);
	glEnd();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D,nullid);
}


void primPolyFT4(unsigned char * baseAddr)
{
	uint32_t *gpuData = (uint32_t *)baseAddr;
	short *gpuPoint=((short *) baseAddr);
	int32_t clutP;
	short x1,x2,x3,x4,y1,y2,y3,y4;
	short tx0,ty0,tx1,ty1,tx2,ty2,tx3,ty3;
	unsigned short gpuDataX;

	x1 = (gpuPoint[2]<<21)>>21;
	y1 = (gpuPoint[3]<<21)>>21;
	x2 = (gpuPoint[6]<<21)>>21;
	y2 = (gpuPoint[7]<<21)>>21;
	x3 = (gpuPoint[10]<<21)>>21;
	y3 = (gpuPoint[11]<<21)>>21;
	x4 = (gpuPoint[14]<<21)>>21;
	y4 = (gpuPoint[15]<<21)>>21;

	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;
	x3 += psxDraw.offsetX;
	y3 += psxDraw.offsetY;
	x4 += psxDraw.offsetX;
	y4 += psxDraw.offsetY;

	tx0 = (short)(gpuData[2] & 0xff);
	ty0 = (short)((gpuData[2]>>8) & 0xff);
	tx1 = (short)(gpuData[4] & 0xff);
	ty1 = (short)((gpuData[4]>>8) & 0xff);
	tx2 = (short)(gpuData[6] & 0xff);
	ty2 = (short)((gpuData[6]>>8) & 0xff);
	tx3 = (short)(gpuData[8] & 0xff);
	ty3 = (short)((gpuData[8]>>8) & 0xff);
	
	gpuDataX=(unsigned short)(gpuData[4]>>16);

	int32_t tempABR = (gpuDataX >> 5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
		gAlpha=TransRate[texinfo.abr].alpha;
		transparent=TRUE;
	}
	UpdateTextureInfo(gpuDataX);
    
    clutP  = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	
	if(baseAddr[3]&2){
		if(!transparent){
			glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
			transparent=TRUE;
		}
		if(baseAddr[3]&1){
			glColor4ub(255,255,255,gAlpha);
		}else{
			glColor4ub(texshade[baseAddr[0]],texshade[baseAddr[1]],texshade[baseAddr[2]],gAlpha);
		}
	}else{
		glBlendFunc(TransRate[0].src,TransRate[0].dst);
        transparent=FALSE;
		if(baseAddr[3]&1){
			glColor3ub(255,255,255);
		}else{
			glColor3ub(texshade[baseAddr[0]],texshade[baseAddr[1]],texshade[baseAddr[2]]);
		}
	}

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	glBegin(GL_POLYGON);
		glTexCoord2s(tx0,ty0);
		glVertex2s(x1,y1);
		glTexCoord2s(tx1,ty1);
		glVertex2s(x2,y2);
		glTexCoord2s(tx3,ty3);
		glVertex2s(x4,y4);
		glTexCoord2s(tx2,ty2);
		glVertex2s(x3,y3);
	glEnd();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D,nullid);
}

void primPolyGT3(unsigned char *baseAddr)
{
	uint32_t *gpuData = (uint32_t *)baseAddr;
	short *gpuPoint=((short *) baseAddr);
	int32_t clutP;
	short x1,x2,x3,y1,y2,y3;
	short tx0,ty0,tx1,ty1,tx2,ty2;
	unsigned short gpuDataX;
	unsigned char alpha;

	x1 = (gpuPoint[2]<<21)>>21;
	y1 = (gpuPoint[3]<<21)>>21;
	x2 = (gpuPoint[8]<<21)>>21;
	y2 = (gpuPoint[9]<<21)>>21;
	x3 = (gpuPoint[14]<<21)>>21;
	y3 = (gpuPoint[15]<<21)>>21;

	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;
	x3 += psxDraw.offsetX;
	y3 += psxDraw.offsetY;


	tx0 = (short)(gpuData[2] & 0xff);
	ty0 = (short)((gpuData[2]>>8) & 0xff);
	tx1 = (short)(gpuData[5] & 0xff);
	ty1 = (short)((gpuData[5]>>8) & 0xff);
	tx2 = (short)(gpuData[8] & 0xff);
	ty2 = (short)((gpuData[8]>>8) & 0xff);
	
	gpuDataX=(unsigned short)(gpuData[5]>>16);
	int32_t tempABR = (gpuDataX >> 5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
		gAlpha=TransRate[texinfo.abr].alpha;
		transparent=TRUE;
	}
	UpdateTextureInfo(gpuDataX);
    
    clutP  = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	if(baseAddr[3]&2){
		glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
        transparent=TRUE;
		alpha=gAlpha;
	}else{
		glBlendFunc(TransRate[0].src,TransRate[0].dst);
        transparent=FALSE;
		alpha=255;
	}

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	
	glBegin(GL_POLYGON);
		glColor4ub(texshade[baseAddr[0]],texshade[baseAddr[1]],texshade[baseAddr[2]],alpha);
		glTexCoord2s(tx0,ty0);
		glVertex2s(x1,y1);
		glColor4ub(texshade[baseAddr[12]],texshade[baseAddr[13]],texshade[baseAddr[14]],alpha);
		glTexCoord2s(tx1,ty1);
		glVertex2s(x2,y2);
		glColor4ub(texshade[baseAddr[24]],texshade[baseAddr[25]],texshade[baseAddr[26]],alpha);
		glTexCoord2s(tx2,ty2);
		glVertex2s(x3,y3);
	glEnd();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D,nullid);
}

void primPolyGT4(unsigned char *baseAddr)
{
	uint32_t *gpuData = (uint32_t *)baseAddr;
	short *gpuPoint=((short *) baseAddr);
	int32_t clutP;
	short x1,y1,x2,y2,x3,y3,x4,y4;
	short tx0,ty0,tx1,ty1,tx2,ty2,tx3,ty3;
	unsigned short gpuDataX;
	unsigned char alpha;

	x1 = (gpuPoint[2]<<21)>>21;
	y1 = (gpuPoint[3]<<21)>>21;
	x2 = (gpuPoint[8]<<21)>>21;
	y2 = (gpuPoint[9]<<21)>>21;
	x3 = (gpuPoint[14]<<21)>>21;
	y3 = (gpuPoint[15]<<21)>>21;
	x4 = (gpuPoint[20]<<21)>>21;
	y4 = (gpuPoint[21]<<21)>>21;

	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;
	x3 += psxDraw.offsetX;
	y3 += psxDraw.offsetY;
	x4 += psxDraw.offsetX;
	y4 += psxDraw.offsetY;

	tx0 = (short)(gpuData[2] & 0xff);
	ty0 = (short)((gpuData[2]>>8) & 0xff);
	tx1 = (short)(gpuData[5] & 0xff);
	ty1 = (short)((gpuData[5]>>8) & 0xff);
	tx2 = (short)(gpuData[8] & 0xff);
	ty2 = (short)((gpuData[8]>>8) & 0xff);
	tx3 = (short)(gpuData[11] & 0xff);
	ty3 = (short)((gpuData[11]>>8) & 0xff);

	gpuDataX=(unsigned short)(gpuData[5]>>16);
	int32_t tempABR = (gpuDataX >> 5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
		gAlpha=TransRate[texinfo.abr].alpha;
		transparent=TRUE;
	}
	UpdateTextureInfo(gpuDataX);
    clutP  = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	if(baseAddr[3]&2){
			glBlendFunc(TransRate[texinfo.abr].src,TransRate[texinfo.abr].dst);
			transparent=TRUE;
		alpha=gAlpha;
	}else{
			glBlendFunc(TransRate[0].src,TransRate[0].dst);
			transparent=FALSE;
		alpha=255;
	}

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	glBegin(GL_POLYGON);
		glColor4ub(texshade[baseAddr[0]],texshade[baseAddr[1]],texshade[baseAddr[2]],alpha);
		glTexCoord2s(tx0,ty0);
		glVertex2s(x1,y1);
		glColor4ub(texshade[baseAddr[12]],texshade[baseAddr[13]],texshade[baseAddr[14]],alpha);
		glTexCoord2s(tx1,ty1);
		glVertex2s(x2,y2);
		glColor4ub(texshade[baseAddr[36]],texshade[baseAddr[37]],texshade[baseAddr[38]],alpha);
		glTexCoord2s(tx3,ty3);
		glVertex2s(x4,y4);
		glColor4ub(texshade[baseAddr[24]],texshade[baseAddr[25]],texshade[baseAddr[26]],alpha);
		glTexCoord2s(tx2,ty2);
		glVertex2s(x3,y3);
	glEnd();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D,nullid);
}

void primNI(ub *baseAddr) {
}

ub primTableC[256] = {
	0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	4, 4, 4, 4, 7, 7, 7, 7, 5, 5, 5, 5, 9, 9, 9, 9,
	6, 6, 6, 6, 9, 9, 9, 9, 8, 8, 8, 8,12,12,12,12,
	3, 3, 3, 3, 0, 0, 0, 0, 5, 5, 5, 5, 6, 6, 6, 6,
	4, 4, 4, 4, 0, 0, 0, 0, 7, 7, 7, 7, 9, 9, 9, 9,
	3, 3, 3, 3, 4, 4, 4, 4, 2, 2, 2, 2, 0, 0, 0, 0,
	2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3,
	4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void (*primTableJ[256])(unsigned char *) = 
{
	// 00
	primNI,primNI,primBlkFill,primNI,primNI,primNI,primNI,primNI,
	// 08
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 10
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 18
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 20
	primPolyF3,primPolyF3,primPolyF3,primPolyF3,primPolyFT3,primPolyFT3,primPolyFT3,primPolyFT3,
	// 28
	primPolyF4,primPolyF4,primPolyF4,primPolyF4,primPolyFT4,primPolyFT4,primPolyFT4,primPolyFT4,
	// 30
	primPolyG3,primPolyG3,primPolyG3,primPolyG3,primPolyGT3,primPolyGT3,primPolyGT3,primPolyGT3,
	// 38
	primPolyG4,primPolyG4,primPolyG4,primPolyG4,primPolyGT4,primPolyGT4,primPolyGT4,primPolyGT4,
	// 40
	primLineF2,primLineF2,primLineF2,primLineF2,primNI,primNI,primNI,primNI,
	// 48
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 50
	primLineG2,primLineG2,primLineG2,primLineG2,primNI,primNI,primNI,primNI,
	// 58
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 60
	primTileS,primTileS,primTileS,primTileS,primSprtS,primSprtS,primSprtS,primSprtS,
	// 68
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 70
	primNI,primNI,primNI,primNI,primSprt8,primSprt8,primSprt8,primSprt8,
	// 78
	primNI,primNI,primNI,primNI,primSprt16,primSprt16,primSprt16,primSprt16,
	// 80
	primMoveImage,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 88
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 90
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// 98
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// a0
	primLoadImage,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// a8
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// b0
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// b8
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// c0
	primStoreImage,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// c8
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// d0
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// d8
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// e0
	primNI,cmdTexturePage,cmdTextureWindow,cmdDrawAreaStart,cmdDrawAreaEnd,cmdDrawOffset,cmdSTP,primNI,
	// e8
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// f0
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
	// f8
	primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI
};

#endif
