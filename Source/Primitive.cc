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
} bit[4] = {
	{ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128 },
	{ GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0 },
	{ GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0 },
	{ GL_SRC_ALPHA, GL_ONE,                  64 },
};

bool transparent = false;
ub gAlpha;

void cmdSTP(ub *data) {
}

void updateTextureInfo(uw data) {
	texinfo.x = (data << 6) & 0x3c0;
	texinfo.y = (data << 4) & 0x100;
	texinfo.colormode = (data >> 7) & 3;
	if (texinfo.colormode == 3) texinfo.colormode = 2;
	texinfo.abr = (data >> 5) & 3;
}

void cmdTexturePage(ub *data) {
	uw gdata = ((uw *)data)[0];
	sw tempABR = (gdata >> 5) & 0x3;
	if (texinfo.abr != tempABR) {
        texinfo.abr = tempABR;
        glBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
        gAlpha = bit[texinfo.abr].alpha;
		transparent = true;
	}
	updateTextureInfo(gdata);
}

void cmdTextureWindow(ub *baseAddr) {
}

void cmdDrawAreaStart(ub *data) {
	uw gdata = ((uw *)data)[0];
    
	psxDraw.clipX1 = gdata & 0x3ff;
	psxDraw.clipY1 = (gdata >> 10) & 0x1ff;
    
	GLdouble equation[4] = { 0.0, 0.0, 0.0, 0.0 };
	equation[0] = 1.0;
	equation[3] = (GLdouble) - (psxDraw.clipX1);
	GLClipPlane(GL_CLIP_PLANE0, equation);
    
	equation[0] = 0.0;
	equation[1] = 1.0;
	equation[3] = (GLdouble) - (psxDraw.clipY1);
	GLClipPlane(GL_CLIP_PLANE1, equation);
}

void cmdDrawAreaEnd(ub *data) {
	uw gdata = ((uw *)data)[0];
    
	psxDraw.clipX2 = gdata & 0x3ff;
	psxDraw.clipY2 = (gdata >> 10) & 0x1ff;
    
	GLdouble equation[4] = { 0.0, 0.0, 0.0, 0.0 };
	equation[0] = -1.0;
	equation[3] = (GLdouble)(psxDraw.clipX2);
	GLClipPlane(GL_CLIP_PLANE2, equation);
    
	equation[0] = 0.0;
	equation[1] = -1.0;
	equation[3] = (GLdouble)(psxDraw.clipY2);
	GLClipPlane(GL_CLIP_PLANE3, equation);
}

void cmdDrawOffset(ub *data) {
	uw gdata = ((uw *)data)[0];
    
	psxDraw.offsetX = (sh)(gdata & 0x7ff);
	psxDraw.offsetY = (sh)((gdata >> 11) & 0x7ff);

	psxDraw.offsetX = (sh)(((sw)psxDraw.offsetX << 21) >> 21);
	psxDraw.offsetY = (sh)(((sw)psxDraw.offsetY << 21) >> 21);
}

void primLoadImage(ub *data) {
	sh *k = (sh *)data;
	
	sh x = (k[2] << 21) >> 21;
	sh y = (k[3] << 21) >> 21;
	sh w = (k[4] << 21) >> 21;
	sh h = (k[5] << 21) >> 21;
    
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

void primStoreImage(ub *data) {
}

void primMoveImage(ub *data) {
}

void primTileS(ub *data) {
	uh *k = (uh *)data;
    
	sh x = ((k[2] << 21) >> 21) + psxDraw.offsetX;
	sh y = ((k[3] << 21) >> 21) + psxDraw.offsetY;
	sh w = ((k[4] << 21) >> 21);
	sh h = ((k[5] << 21) >> 21);
    
	if (data[3] & 2) {
        GLBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
		GLColor4ub(data[0], data[1], data[2], gAlpha = bit[texinfo.abr].alpha);
        transparent = true;
	}
    else {
		GLBlendFunc(bit[0].src, bit[0].dst);
		GLColor4ub(data[0], data[1], data[2], 255);
        transparent = false;
	}
    
	GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(x,     y);
        GLVertex2s(x + w, y);
        GLVertex2s(x,     y + h);
        GLVertex2s(x + w, y + h);
	GLEnd();
}

void primBlkFill(ub *data) {
	sh *k = (sh *)data;
    
	sh x = (k[2] << 21) >> 21;
	sh y = (k[3] << 21) >> 21;
	sh w = (k[4] << 21) >> 21;
	sh h = (k[5] << 21) >> 21;
    
	GLDisable(GL_CLIP_PLANE0);
	GLDisable(GL_CLIP_PLANE1);
	GLDisable(GL_CLIP_PLANE2);
	GLDisable(GL_CLIP_PLANE3);
    
	GLColor4ub(data[0], data[1], data[2], 255);
    
	GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(x,     y);
        GLVertex2s(x + w, y);
        GLVertex2s(x,     y + h);
        GLVertex2s(x + w, y + h);
	GLEnd();
    
	GLEnable(GL_CLIP_PLANE0);
	GLEnable(GL_CLIP_PLANE1);
	GLEnable(GL_CLIP_PLANE2);
	GLEnable(GL_CLIP_PLANE3);
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
            GLBindTexture(GL_TEXTURE_2D,texture[i].id);
            return;
        }
    }
    
	texture[ctext].textAddrX = texinfo.x;
	texture[ctext].textAddrY = texinfo.y;
	texture[ctext].textTP    = texinfo.colormode;
	texture[ctext].clutP     = clutP;
	texture[ctext].Update    = false;
    
	GLBindTexture(GL_TEXTURE_2D, texture[ctext].id);
    
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
    
	GLTexPhoto2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

#define SPRITE(n) \
    uw *gpuData = (uw *)data; \
    sh *k = (sh *)data; \
    \
    sw clutP = (gpuData[2] >> 12) & 0x7fff0; \
    sh tx = (sh)(gpuData[2] & 0xff); \
    sh ty = (sh)((gpuData[2] >> 8) & 0xff); \
    \
    sh x = ((k[2] << 21) >> 21) + psxDraw.offsetX; \
    sh y = ((k[3] << 21) >> 21) + psxDraw.offsetY; \
    \
    setupTexture(clutP); \
    \
    ub r = texshade[data[0]]; \
    ub g = texshade[data[1]]; \
    ub b = texshade[data[2]]; \
    \
    if (data[3] & 2) { \
        GLBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst); \
        transparent = true; \
         \
        if (data[3] & 1) { \
            GLColor4ub(255, 255, 255, gAlpha); \
        } \
        else { \
            GLColor4ub(r, g, b, gAlpha); \
        } \
    } \
    else { \
        glBlendFunc(bit[0].src, bit[0].dst); \
        transparent = false; \
         \
        if (data[3] & 1) { \
            GLColor4ub(255, 255, 255, 255); \
        } \
        else { \
            GLColor4ub(r, g, b, 255); \
        } \
    } \
    \
    GLStart(GL_TRIANGLE_STRIP); \
        GLTexCoord2s(tx,   ty);   GLVertex2s(x,   y); \
        GLTexCoord2s(tx+n, ty);   GLVertex2s(x+n, y); \
        GLTexCoord2s(tx,   ty+n); GLVertex2s(x,   y+n); \
        GLTexCoord2s(tx+n, ty+n); GLVertex2s(x+n, y+n); \
    GLEnd();\
    \
    if (data[3] & 2) { \
        GLEnable(GL_ALPHA_TEST); \
        glAlphaFunc(GL_EQUAL, 1); \
        GLColor4ub(255, 255, 255, 255); \
        GLStart(GL_TRIANGLE_STRIP); \
            GLTexCoord2s(tx,   ty);   GLVertex2s(x,   y); \
            GLTexCoord2s(tx+n, ty);   GLVertex2s(x+n, y); \
            GLTexCoord2s(tx,   ty+n); GLVertex2s(x,   y+n); \
            GLTexCoord2s(tx+n, ty+n); GLVertex2s(x+n, y+n); \
        GLEnd(); \
        GLDisable(GL_ALPHA_TEST); \
    } \
    GLBindTexture(GL_TEXTURE_2D, nullid)

void primSprt8(ub *data) {
    SPRITE(8);
}

void primSprt16(ub *data) {
	SPRITE(16);
}

void primSprtS(ub *data) {
	uw *gpuData = (uw *)data;
	sh *k = (sh *)data;
    
    sw clutP = (gpuData[2] >> 12) & 0x7fff0;
    sh tx    = (sh)(gpuData[2] & 0x000000ff);
    sh ty    = (sh)((gpuData[2] >> 8) & 0x000000ff);
	sh sprtW = (sh)(gpuData[3] & 0x3ff);
	sh sprtH = (sh)((gpuData[3] >> 16) & 0x1ff);
    
	sh x = (k[2] << 21) >> 21;
	sh y = (k[3] << 21) >> 21;
    
	x += psxDraw.offsetX;
	y += psxDraw.offsetY;
    
	setupTexture(clutP);
	
    ub r = texshade[data[0]];
    ub g = texshade[data[1]];
    ub b = texshade[data[2]];

    if (data[3] & 2) {
        glBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
        transparent = true;
        
        if (data[3] & 1) {
            glColor4ub(255, 255, 255, gAlpha);
        }
        else {
            glColor4ub(r, g, b, gAlpha);
        }
    }
    else {
        glBlendFunc(bit[0].src, bit[0].dst);
        transparent = false;
        
        if (data[3] & 1) {
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
	
	if (data[3] & 2) {
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

void primLineF2(ub *data) {
	sh *k = (sh *)data;
    
	sh x1 = ((k[2] << 21) >> 21) + psxDraw.offsetX;
	sh y1 = ((k[3] << 21) >> 21) + psxDraw.offsetY;
	sh x2 = ((k[4] << 21) >> 21) + psxDraw.offsetX;
	sh y2 = ((k[5] << 21) >> 21) + psxDraw.offsetY;
    
	if (data[3] & 2) {
        if (!transparent) {
            GLBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
            transparent = true;
        }
        GLColor4ub(data[0], data[1], data[2], gAlpha);
    }
    else {
        if (transparent) {
            GLBlendFunc(bit[0].src, bit[0].dst);
			transparent = false;
        }
        GLColor4ub(data[0], data[1], data[2], 255);
    }
    
    GLStart(GL_LINES);
        GLVertex2s(x1, y1);
		GLVertex2s(x2, y2);
	GLEnd();
}

void primLineG2(ub *data) {
	sh *k = (sh *)data;
	ub alpha;
    
    sh x1 = ((k[2] << 21) >> 21) + psxDraw.offsetX;
    sh y1 = ((k[3] << 21) >> 21) + psxDraw.offsetY;
    sh x2 = ((k[6] << 21) >> 21) + psxDraw.offsetX;
    sh y2 = ((k[7] << 21) >> 21) + psxDraw.offsetY;
    
	if (data[3] & 2) {
        if (!transparent) {
            GLBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
            transparent = true;
        }
		alpha = gAlpha;
	}
    else {
        if (transparent) {
            GLBlendFunc(bit[0].src, bit[0].dst);
            transparent = false;
        }
		alpha = 255;
	}
    
	GLStart(GL_LINES);
		GLColor4ub(data[0], data[1], data[ 2], alpha);
		GLVertex2s(x1, y1);
		GLColor4ub(data[8], data[9], data[10], alpha);
		GLVertex2s(x2, y2);
	GLEnd();
}

#define POLY_F(n) \
    sh *k = (sh *)data; \
    \
    sh vx[4] = { \
        ((k[2] << 21) >> 21) + psxDraw.offsetX, \
        ((k[4] << 21) >> 21) + psxDraw.offsetX, \
        ((k[6] << 21) >> 21) + psxDraw.offsetX, \
        ((k[8] << 21) >> 21) + psxDraw.offsetX, \
    }; \
    \
    sh vy[4] = { \
        ((k[3] << 21) >> 21) + psxDraw.offsetY, \
        ((k[5] << 21) >> 21) + psxDraw.offsetY, \
        ((k[7] << 21) >> 21) + psxDraw.offsetY, \
        ((k[9] << 21) >> 21) + psxDraw.offsetY, \
    }; \
    \
    if (data[3] & 2) { \
        if (!transparent) { \
            GLBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst); \
            transparent = true; \
        } \
        GLColor4ub(data[0], data[1], data[2], gAlpha = bit[texinfo.abr].alpha); \
    } \
    else { \
        if (transparent) { \
            GLBlendFunc(bit[0].src, bit[0].dst); \
            transparent = false; \
        } \
        GLColor4ub(data[0], data[1], data[2], 255); \
    } \
    \
    GLStart(GL_TRIANGLE_STRIP); \
    for (int i = 0; i < n; i++) { \
        GLVertex2s(vx[i], vy[i]); \
    } \
    GLEnd()

void primPolyF3(ub *data) {
    POLY_F(3);
}

void primPolyF4(ub *data) {
    POLY_F(4);
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
            glBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst); \
            transparent = true; \
        } \
        alpha = bit[texinfo.abr].alpha; \
    } \
    else { \
        if (transparent) { \
            glBlendFunc(bit[0].src, bit[0].dst); \
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

void primPolyFT3(ub *baseAddr)
{
	uw *gpuData = (uw *)baseAddr;
	sh *gpuPoint=((sh *) baseAddr);
	sw clutP;
	sh x1,x2,x3,y1,y2,y3;
	sh tx0,ty0,tx1,ty1,tx2,ty2;
	uh gpuDataX;

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

	tx0 = (sh)(gpuData[2] & 0xff);
	ty0 = (sh)((gpuData[2]>>8) & 0xff);
	tx1 = (sh)(gpuData[4] & 0xff);
	ty1 = (sh)((gpuData[4]>>8) & 0xff);
	tx2 = (sh)(gpuData[6] & 0xff);
	ty2 = (sh)((gpuData[6]>>8) & 0xff);
	gpuDataX=(uh)(gpuData[4]>>16);

	sw tempABR = (gpuDataX >>5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
		gAlpha = bit[texinfo.abr].alpha;
		transparent=TRUE;
	}
	updateTextureInfo(gpuDataX);

    clutP  = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	
	baseAddr[0] = texshade[baseAddr[0]];
	baseAddr[1] = texshade[baseAddr[1]];
	baseAddr[2] = texshade[baseAddr[2]];

	if(baseAddr[3]&2){
		if(!transparent){
			glBlendFunc(bit[texinfo.abr].src, bit[texinfo.abr].dst);
			transparent=TRUE;
		}
		if(baseAddr[3]&1){
			glColor4ub(255,255,255,gAlpha);
		}else{
			glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], gAlpha);
		}
	}else{
		glBlendFunc(bit[0].src, bit[0].dst);
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


void primPolyFT4(ub *baseAddr)
{
	uw *gpuData = (uw *)baseAddr;
	sh *gpuPoint=((sh *) baseAddr);
	sw clutP;
	sh x1,x2,x3,x4,y1,y2,y3,y4;
	sh tx0,ty0,tx1,ty1,tx2,ty2,tx3,ty3;
	uh gpuDataX;

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

	tx0 = (sh)(gpuData[2] & 0xff);
	ty0 = (sh)((gpuData[2]>>8) & 0xff);
	tx1 = (sh)(gpuData[4] & 0xff);
	ty1 = (sh)((gpuData[4]>>8) & 0xff);
	tx2 = (sh)(gpuData[6] & 0xff);
	ty2 = (sh)((gpuData[6]>>8) & 0xff);
	tx3 = (sh)(gpuData[8] & 0xff);
	ty3 = (sh)((gpuData[8]>>8) & 0xff);
	
	gpuDataX=(uh)(gpuData[4]>>16);

	sw tempABR = (gpuDataX >> 5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(bit[texinfo.abr].src,bit[texinfo.abr].dst);
		gAlpha=bit[texinfo.abr].alpha;
		transparent=TRUE;
	}
	updateTextureInfo(gpuDataX);
    
    clutP  = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	
	if(baseAddr[3]&2){
		if(!transparent){
			glBlendFunc(bit[texinfo.abr].src,bit[texinfo.abr].dst);
			transparent=TRUE;
		}
		if(baseAddr[3]&1){
			glColor4ub(255,255,255,gAlpha);
		}else{
			glColor4ub(texshade[baseAddr[0]],texshade[baseAddr[1]],texshade[baseAddr[2]],gAlpha);
		}
	}else{
		glBlendFunc(bit[0].src,bit[0].dst);
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

void primPolyGT3(ub *baseAddr)
{
	uw *gpuData = (uw *)baseAddr;
	sh *gpuPoint=((sh *) baseAddr);
	sw clutP;
	sh x1,x2,x3,y1,y2,y3;
	sh tx0,ty0,tx1,ty1,tx2,ty2;
	uh gpuDataX;
	ub alpha;

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


	tx0 = (sh)(gpuData[2] & 0xff);
	ty0 = (sh)((gpuData[2]>>8) & 0xff);
	tx1 = (sh)(gpuData[5] & 0xff);
	ty1 = (sh)((gpuData[5]>>8) & 0xff);
	tx2 = (sh)(gpuData[8] & 0xff);
	ty2 = (sh)((gpuData[8]>>8) & 0xff);
	
	gpuDataX=(uh)(gpuData[5]>>16);
	sw tempABR = (gpuDataX >> 5) & 0x3;
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(bit[texinfo.abr].src,bit[texinfo.abr].dst);
		gAlpha=bit[texinfo.abr].alpha;
		transparent=TRUE;
	}
	updateTextureInfo(gpuDataX);
    
    clutP = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	if(baseAddr[3]&2){
		glBlendFunc(bit[texinfo.abr].src,bit[texinfo.abr].dst);
        transparent=TRUE;
		alpha=gAlpha;
	}else{
		glBlendFunc(bit[0].src,bit[0].dst);
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

void primPolyGT4(ub *baseAddr) {
	uw *gpuData  = (uw *)baseAddr;
	sh *gpuPoint = (sh *)baseAddr;
    
	ub alpha;

	sh x1 = (gpuPoint[2]<<21)>>21;
	sh y1 = (gpuPoint[3]<<21)>>21;
	sh x2 = (gpuPoint[8]<<21)>>21;
	sh y2 = (gpuPoint[9]<<21)>>21;
	sh x3 = (gpuPoint[14]<<21)>>21;
	sh y3 = (gpuPoint[15]<<21)>>21;
	sh x4 = (gpuPoint[20]<<21)>>21;
	sh y4 = (gpuPoint[21]<<21)>>21;

	x1 += psxDraw.offsetX;
	y1 += psxDraw.offsetY;
	x2 += psxDraw.offsetX;
	y2 += psxDraw.offsetY;
	x3 += psxDraw.offsetX;
	y3 += psxDraw.offsetY;
	x4 += psxDraw.offsetX;
	y4 += psxDraw.offsetY;

	sh tx0 = (sh)(gpuData[2] & 0xff);
	sh ty0 = (sh)((gpuData[2]>>8) & 0xff);
	sh tx1 = (sh)(gpuData[5] & 0xff);
	sh ty1 = (sh)((gpuData[5]>>8) & 0xff);
	sh tx2 = (sh)(gpuData[8] & 0xff);
	sh ty2 = (sh)((gpuData[8]>>8) & 0xff);
	sh tx3 = (sh)(gpuData[11] & 0xff);
	sh ty3 = (sh)((gpuData[11]>>8) & 0xff);

	uh gpuDataX = (uh)(gpuData[5]>>16);
	sw tempABR = (gpuDataX >> 5) & 0x3;
    
	if(texinfo.abr!=tempABR){
		texinfo.abr = tempABR;
		glBlendFunc(bit[texinfo.abr].src,bit[texinfo.abr].dst);
		gAlpha=bit[texinfo.abr].alpha;
		transparent=TRUE;
	}
	updateTextureInfo(gpuDataX);
    sw clutP = (gpuData[2]>>12) & 0x7fff0;
	setupTexture(clutP);
	if(baseAddr[3]&2){
			glBlendFunc(bit[texinfo.abr].src,bit[texinfo.abr].dst);
			transparent=TRUE;
		alpha=gAlpha;
	}else{
			glBlendFunc(bit[0].src,bit[0].dst);
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

void (*primTableJ[256])(ub *) = 
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
