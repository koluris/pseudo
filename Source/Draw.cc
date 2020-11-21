#include "Global.h"

union uPointers{
    void *v;
    ub *b;
    uh *w;
    uw *d;
    sb *c;
    sh *s;
    sw *l;
};

// Basic
typedef struct { ub c, m, k, n; } COLOR;
typedef struct { sh w, h; } POINTF;
typedef struct { sh w, h; ub u, v; uh clut; } POINTFT;
typedef struct { COLOR co; sh w, h; } POINTG;
typedef struct { COLOR co; sh w, h; ub u, v; uh clut; } POINTGT;

// VertexF & LineF
typedef struct { COLOR co; POINTF v[2]; } F2;
typedef struct { COLOR co; POINTF v[3]; } F3;
typedef struct { COLOR co; POINTF v[4]; } F4;

// VertexFT
typedef struct { COLOR co; POINTFT v[3]; } FT3;
typedef struct { COLOR co; POINTFT v[4]; } FT4;

// VertexG & LineG
typedef struct { POINTG v[2]; } G2;
typedef struct { POINTG v[3]; } G3;
typedef struct { POINTG v[4]; } G4;

// VertexGT
typedef struct { POINTGT v[3]; } GT3;
typedef struct { POINTGT v[4]; } GT4;

// BlockFill & Sprites
typedef struct { COLOR co; POINTF  v[1]; sh w, h; } BLK;
typedef struct { COLOR co; POINTFT v[1]; sh w, h; } SPRT;

void cmdSTP(ub *baseAddr) {
    uw gdata = ((uw *)baseAddr)[0];
    
    vs.statusRet &= ~0x1800;
    vs.statusRet |= (gdata & 0x03) << 11;
    
    vs.psxDraw.setmask = gdata & 1;
    vs.psxDraw.testmask = (gdata & 2) >> 1;
}

void UpdateTextureInfo(uw gdata) {
    vs.texinfo.x = (gdata << 6) & 0x3c0;
    vs.texinfo.y = (gdata << 4) & 0x100;

    vs.texinfo.colormode = (gdata >> 7) & 0x3;
    
    if (vs.texinfo.colormode == 3) {
        vs.texinfo.colormode = 2;
    }

    vs.texinfo.abr = (gdata >> 5) & 0x3;
    vs.texinfo.mirror = gdata & 0x3000 >> 12;

    vs.statusRet &= ~0x07ff;
    vs.statusRet |= gdata & 0x07ff;
}

void cmdTexturePage(ub *baseAddr) {
    uw gdata = ((uw *)baseAddr)[0];

    sw tempABR = (gdata >> 5) & 0x3;
    
    if (vs.texinfo.abr != tempABR) {
        vs.texinfo.abr = tempABR;
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        vs.transparent = TRUE;
    }
    
    UpdateTextureInfo(gdata);
}

void cmdTextureWindow(ub *baseAddr) {
    uw gdata = ((uw *)baseAddr)[0];
    uw YAlign, XAlign;

    if (gdata & 0x020) {
        vs.psxDraw.texwinY2 = 8;
    }
    else if (gdata & 0x040) {
        vs.psxDraw.texwinY2 = 16;
    }
    else if (gdata & 0x080) {
        vs.psxDraw.texwinY2 = 32;
    }
    else if (gdata & 0x100) {
        vs.psxDraw.texwinY2 = 64;
    }
    else if (gdata & 0x200) {
        vs.psxDraw.texwinY2 = 128;
    }
    else {
        vs.psxDraw.texwinY2 = 256;
    }
    
    if (gdata & 0x001) {
        vs.psxDraw.texwinX2 = 8;
    }
    else if (gdata & 0x002) {
        vs.psxDraw.texwinX2 = 16;
    }
    else if (gdata & 0x004) {
        vs.psxDraw.texwinX2 = 32;
    }
    else if (gdata & 0x008) {
        vs.psxDraw.texwinX2 = 64;
    }
    else if (gdata & 0x010) {
        vs.psxDraw.texwinX2 = 128;
    }
    else {
        vs.psxDraw.texwinX2 = 256;
    }
    
    YAlign = (uw)(32 - (vs.psxDraw.texwinY2 >> 3));
    XAlign = (uw)(32 - (vs.psxDraw.texwinX2 >> 3));
    
    vs.psxDraw.texwinY1 = (sh)(((gdata >> 15) & YAlign) << 3);
    vs.psxDraw.texwinX1 = (sh)(((gdata >> 10) & XAlign) << 3);
    
    if ((vs.psxDraw.texwinX1 == 0 && vs.psxDraw.texwinY1 == 0 && vs.psxDraw.texwinX2 == 0 && vs.psxDraw.texwinY2 == 0) || (vs.psxDraw.texwinX2 == 256 && vs.psxDraw.texwinY2 == 256)) {
        vs.psxDraw.texwinenabled = 0;
    }
    else {
        vs.psxDraw.texwinenabled = 1;
    }
    
    vs.infoVals[INFO_TW] = gdata & 0xfffff;
}

void cmdDrawAreaStart(ub *baseAddr) {
    uw gdata = ((uw *)baseAddr)[0];

    vs.psxDraw.clipX1 = gdata & 0x3ff;
    vs.psxDraw.clipY1 = (gdata >> 10) & 0x1ff;

    GLdouble equation[4] = {0.0, 0.0, 0.0, 0.0};
    equation[0] = 1.0;
    equation[3] = (GLdouble) - (vs.psxDraw.clipX1);
    glClipPlane(GL_CLIP_PLANE0, equation);

    equation[0] = 0.0;
    equation[1] = 1.0;
    equation[3] = (GLdouble) - (vs.psxDraw.clipY1);
    glClipPlane(GL_CLIP_PLANE1, equation);
    
    vs.infoVals[INFO_DRAWSTART] = gdata & 0x3fffff;
}

void cmdDrawAreaEnd(ub *baseAddr) {
    uw gdata = ((uw *)baseAddr)[0];

    vs.psxDraw.clipX2 = gdata & 0x3ff;
    vs.psxDraw.clipY2 = (gdata >> 10) & 0x1ff;

    GLdouble equation[4] = {0.0, 0.0, 0.0, 0.0};
    equation[0] = -1.0;
    equation[3] = (GLdouble)(vs.psxDraw.clipX2);
    glClipPlane(GL_CLIP_PLANE2, equation);

    equation[0] = 0.0;
    equation[1] = -1.0;
    equation[3] = (GLdouble)(vs.psxDraw.clipY2);
    glClipPlane(GL_CLIP_PLANE3, equation);
    
    vs.infoVals[INFO_DRAWEND] = gdata & 0x3fffff;
}

void cmdDrawOffset(unsigned char *baseAddr) {
    uw gdata = ((uw *)baseAddr)[0];

    vs.psxDraw.offsetX = (sh)(gdata & 0x7ff);
    vs.psxDraw.offsetY = (sh)((gdata >> 11) & 0x7ff);

    vs.psxDraw.offsetX = (sh)(((int)vs.psxDraw.offsetX << 21) >> 21);
    vs.psxDraw.offsetY = (sh)(((int)vs.psxDraw.offsetY << 21) >> 21);
    
    vs.infoVals[INFO_DRAWOFF] = gdata & 0x7fffff;
}

void primLoadImage(ub *baseAddr) {
    sh *sgpuData = (sh *)baseAddr;
    
    sh x = (sgpuData[2] << 21) >> 21;
    sh y = (sgpuData[3] << 21) >> 21;
    sh w = (sgpuData[4] << 21) >> 21;
    sh h = (sgpuData[5] << 21) >> 21;

    if (w == -1024) {
        w = 1024;
    }
    if (h == -512) {
        h = 512;
    }

    if (x < 0 || y < 0 || w < 0 || h < 0 || x + w > 1024 || y + h > 512) {
        return;
    }

    vs.vramWrite.curx = vs.vramWrite.x = x;
    vs.vramWrite.cury = vs.vramWrite.y = y;
    vs.vramWrite.w = w;
    vs.vramWrite.h = h;
    vs.vramWrite.extratarget = (uw *)malloc(vs.vramWrite.w * vs.vramWrite.h * 4);
    vs.vramWrite.enabled = 1;

    vs.imageTransfer = 1;

    for (int i = 0; i < 384; i++) {
        //if (((vs.texture[i].x + 255) >= x) &&((vs.texture[i].y + 255) >= y) && (vs.texture[i].x <= (x + w)) && (vs.texture[i].y <= (y + h))) {
            vs.texture[i].update = true;
        //}
    }
}

void primStoreImage(ub * baseAddr) {
    uw *gpuData = (uw *)baseAddr;

    vs.imageX0 = (sh)((gpuData[1] & 0x03ff));
    vs.imageY0 = (sh)((gpuData[1] >> 16) & 0x01ff);
    vs.imageX1 = (sh)((gpuData[2] & 0xffff));
    vs.imageY1 = (sh)((gpuData[2] >> 16) & 0xffff);

    vs.imTX = vs.imageX0;
    vs.imTY = vs.imageY0;
    vs.imTXc = vs.imageX1;
    vs.imTYc = vs.imageY1;
    vs.imSize = vs.imageY1 * vs.imageX1 / 2;

    vs.imageTransfer = 2;
    vs.statusRet |= 0x08000000;
}

void primMoveImage(ub *baseAddr) {
    uw *p = (uw *)baseAddr;
    
    uh x0 = (p[1] >>  0) & 0x03ff;
    uh y0 = (p[1] >> 16) & 0x01ff;
    uh x1 = (p[2] >>  0) & 0x03ff;
    uh y1 = (p[2] >> 16) & 0x01ff;
    uh sx = (p[3] >>  0) & 0xffff;
    uh sy = (p[3] >> 16) & 0xffff;

    for (int i = 0; i < 384; i++) {
        //if (((vs.texture[i].x + 255) >= x1) && ((vs.texture[i].y + 255) >= y1) && (vs.texture[i].x <= (x1 + sx)) && (vs.texture[i].y <= (y1 + sy))) {
            vs.texture[i].update = true;
        //}
    }

    if ((x0 + sx) > 1024) sx = 1024 - x0;
    if ((x1 + sx) > 1024) sx = 1024 - x1;
    if ((y0 + sy) >  512) sy =  512 - y0;
    if ((y1 + sy) >  512) sy =  512 - y1;
    
    for (int j = 0; j < sy; j++) {
        for (int i = 0; i < sx; i++) {
            vs.psxVuw[((y1 + j) << 10) + x1 + i] = vs.psxVuw[((y0 + j) << 10) + x0 + i];
        }
    }
}

void primTileS(ub *baseAddr) {
    uh *gpuPoint = (uh *)baseAddr;

    sh x = (gpuPoint[2] << 21) >> 21;
    sh y = (gpuPoint[3] << 21) >> 21;
    sh w = (gpuPoint[4] << 21) >> 21;
    sh h = (gpuPoint[5] << 21) >> 21;

    x += vs.psxDraw.offsetX;
    y += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.transparent = TRUE;
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], vs.gAlpha);
    }
    else {
        glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
        vs.transparent = FALSE;
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
    sh *p = (sh *)baseAddr;
    
    sh x = (p[2] << 21) >> 21;
    sh y = (p[3] << 21) >> 21;
    sh w = (p[4] << 21) >> 21;
    sh h = (p[5] << 21) >> 21;

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
    sh tC;
    union uPointers psxVOffset, psxVOffset2;

    if ((vs.texinfo.colormode & 2) == 2) {
        clutP = nullclutP;
    }

    sw ctext = vs.curText;
    for (int i = 0; i < 384; i++) {
        if (vs.texture[i].x == vs.texinfo.x && vs.texture[i].y == vs.texinfo.y && clutP == vs.texture[i].clut) {
            if (vs.texture[i].update == true || vs.texinfo.colormode != vs.texture[i].tp) {
                ctext = i;
                break;
            }
            glBindTexture(GL_TEXTURE_2D, vs.texture[i].id);
            return;
        }
    }

    vs.texture[ctext].x = vs.texinfo.x;
    vs.texture[ctext].y = vs.texinfo.y;
    vs.texture[ctext].tp = vs.texinfo.colormode;
    vs.texture[ctext].clut = clutP;
    vs.texture[ctext].update = false;
    glBindTexture(GL_TEXTURE_2D, vs.texture[ctext].id);
    if (ctext == vs.curText) {
        vs.curText++;
        if (vs.curText >= 384) {
            vs.curText = 0;
        }
    }

    sw tbw = 256;
    psxVOffset.w = vs.psxVuw + (vs.texinfo.y << 10) + vs.texinfo.x;

    uw *pimage = vs.image;

    switch(vs.texinfo.colormode) {
        // 4bit clut
        case 0:
            for (sw sprCY = 0; sprCY < 256; sprCY++) {
                psxVOffset2.b = psxVOffset.b;
                for (sw sprCX = 0; sprCX < 256; sprCX += 2) {
                    tC = (* psxVOffset2.b) & 0x0f;
                    *pimage = vs.torgba[vs.psxVuw[clutP + tC]];
                    pimage++;

                    tC = (* psxVOffset2.b >> 4) & 0x0f;
                    *pimage = vs.torgba[vs.psxVuw[clutP + tC]];
                    pimage++;

                    psxVOffset2.b++;
                }
                psxVOffset.w += 1024;
            }
            break;

        // 8bit clut
        case 1:
            if (vs.texinfo.x == 960) {
                tbw = 128;
            }
            
            for (sw sprCY = 0; sprCY < 256; sprCY++) {
                psxVOffset2.b = psxVOffset.b;
                for (sw sprCX = 0; sprCX < tbw; sprCX++) {
                    tC = *psxVOffset2.b;
                    vs.image[(sprCY << 8) + sprCX] = vs.torgba[vs.psxVuw[clutP + tC]];
                    psxVOffset2.b++;
                }
                psxVOffset.w += 1024;
            }
            break;

        // 15bit direct
        case 2:
            if (vs.texinfo.x == 960) {
                tbw = 64;
            }
            else if (vs.texinfo.x == 896) {
                tbw = 128;
            }
            else if (vs.texinfo.x == 832) {
                tbw = 192;
            }

            for (sw sprCY = 0; sprCY < 256; sprCY++) {
                psxVOffset2.w = psxVOffset.w;
                for (sw sprCX = 0; sprCX < tbw; sprCX++) {
                    sh color = *psxVOffset2.w;
                    vs.image[(sprCY << 8) + sprCX] = vs.torgba[color];
                    psxVOffset2.w++;
                }
                psxVOffset.w += 1024;
            }
            break;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, vs.image);
}

void setSpriteBlendMode(sw command) {
    ub *baseAddr = (ub *)&command;
     
    ub r = vs.texshade[baseAddr[0]];
    ub g = vs.texshade[baseAddr[1]];
    ub b = vs.texshade[baseAddr[2]];

    if (baseAddr[3] & 2) {
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.transparent = TRUE;
       
        if (baseAddr[3] & 1) {
            glColor4ub(255, 255, 255, vs.gAlpha);
        }
        else {
            glColor4ub(r, g, b, vs.gAlpha);
        }
    }
    else {
        glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
        vs.transparent = FALSE;
        
        if (baseAddr[3] & 1) {
            glColor3ub(255, 255, 255);
        }
        else {
            glColor3ub(r, g, b);
        }
    }
}

void primLineF2(ub *baseAddr) {
    sh *gpuPoint = (sh *)baseAddr;
    
    sh x1 = (gpuPoint[2] << 21) >> 21;
    sh y1 = (gpuPoint[3] << 21) >> 21;
    sh x2 = (gpuPoint[4] << 21) >> 21;
    sh y2 = (gpuPoint[5] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], vs.gAlpha);
    }
    else {
        if (vs.transparent) {
            glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
            vs.transparent = FALSE;
        }
        glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
    }
    
    glBegin(GL_LINES);
        glVertex2s(x1,y1);
        glVertex2s(x2,y2);
    glEnd();
}

void primLineG2(ub *baseAddr) {
    sh *gpuPoint = (sh *)baseAddr;
    ub alpha;

    sh x1 = (gpuPoint[2] << 21) >> 21;
    sh y1 = (gpuPoint[3] << 21) >> 21;
    sh x2 = (gpuPoint[6] << 21) >> 21;
    sh y2 = (gpuPoint[7] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        alpha = vs.gAlpha;
    }
    else {
        if (vs.transparent) {
            glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
            vs.transparent = FALSE;
        }
        alpha = 255;
    }
    
    glBegin(GL_LINES);
        glColor4ub(baseAddr[0], baseAddr[1], baseAddr[ 2], alpha);
        glVertex2s(x1,y1);
        glColor4ub(baseAddr[8], baseAddr[9], baseAddr[10], alpha);
        glVertex2s(x2,y2);
    glEnd();
}

void primPolyF3(ub *baseAddr) {
    sh *gpuPoint = (sh *)baseAddr;
    
    sh x1 = (gpuPoint[2] << 21) >> 21;
    sh y1 = (gpuPoint[3] << 21) >> 21;
    sh x2 = (gpuPoint[4] << 21) >> 21;
    sh y2 = (gpuPoint[5] << 21) >> 21;
    sh x3 = (gpuPoint[6] << 21) >> 21;
    sh y3 = (gpuPoint[7] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], vs.gAlpha);
    }
    else {
        if (vs.transparent) {
            glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
            vs.transparent = FALSE;
        }
        glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
    }
    
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2s(x1,y1);
        glVertex2s(x2,y2);
        glVertex2s(x3,y3);
    glEnd();
}

void primPolyF4(ub *baseAddr) {
    sh *gpuPoint = (sh *)baseAddr;
    
    sh x1 = (gpuPoint[2] << 21) >> 21;
    sh y1 = (gpuPoint[3] << 21) >> 21;
    sh x2 = (gpuPoint[4] << 21) >> 21;
    sh y2 = (gpuPoint[5] << 21) >> 21;
    sh x3 = (gpuPoint[6] << 21) >> 21;
    sh y3 = (gpuPoint[7] << 21) >> 21;
    sh x4 = (gpuPoint[8] << 21) >> 21;
    sh y4 = (gpuPoint[9] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;
    x4 += vs.psxDraw.offsetX;
    y4 += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], vs.gAlpha);
    }
    else {
        if (vs.transparent) {
            glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
            vs.transparent = FALSE;
        }
        glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
    }
    
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2s(x1,y1);
        glVertex2s(x2,y2);
        glVertex2s(x3,y3);
        glVertex2s(x4,y4);
    glEnd();
}

void primPolyG3(ub *baseAddr) {
    sh *gpuPoint = (sh *)baseAddr;
    ub alpha;
    
    sh x1 = (gpuPoint[ 2] << 21) >> 21;
    sh y1 = (gpuPoint[ 3] << 21) >> 21;
    sh x2 = (gpuPoint[ 6] << 21) >> 21;
    sh y2 = (gpuPoint[ 7] << 21) >> 21;
    sh x3 = (gpuPoint[10] << 21) >> 21;
    sh y3 = (gpuPoint[11] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        alpha = vs.TransRate[vs.texinfo.abr].alpha;
    }
    else {
        if (vs.transparent) {
            glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
            vs.transparent = FALSE;
        }
        alpha = 255;
    }
    
    glBegin(GL_TRIANGLE_STRIP);
        glColor4ub(baseAddr[ 0], baseAddr[ 1], baseAddr[ 2], alpha);
        glVertex2s(x1,y1);
        glColor4ub(baseAddr[ 8], baseAddr[ 9], baseAddr[10], alpha);
        glVertex2s(x2,y2);
        glColor4ub(baseAddr[16], baseAddr[17], baseAddr[18], alpha);
        glVertex2s(x3,y3);
    glEnd();
}

void primPolyG4(ub *baseAddr) {
    sh *gpuPoint = (sh *)baseAddr;
    ub alpha;

    sh x1 = (gpuPoint[ 2] << 21) >> 21;
    sh y1 = (gpuPoint[ 3] << 21) >> 21;
    sh x2 = (gpuPoint[ 6] << 21) >> 21;
    sh y2 = (gpuPoint[ 7] << 21) >> 21;
    sh x3 = (gpuPoint[10] << 21) >> 21;
    sh y3 = (gpuPoint[11] << 21) >> 21;
    sh x4 = (gpuPoint[14] << 21) >> 21;
    sh y4 = (gpuPoint[15] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;
    x4 += vs.psxDraw.offsetX;
    y4 += vs.psxDraw.offsetY;

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        alpha = vs.TransRate[vs.texinfo.abr].alpha;
    }
    else {
        if (vs.transparent) {
            glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
            vs.transparent = FALSE;
        }
        alpha = 255;
    }
    
    glBegin(GL_TRIANGLE_STRIP);
        glColor4ub(baseAddr[ 0], baseAddr[ 1], baseAddr[ 2], alpha);
        glVertex2s(x1,y1);
        glColor4ub(baseAddr[ 8], baseAddr[ 9], baseAddr[10], alpha);
        glVertex2s(x2,y2);
        glColor4ub(baseAddr[16], baseAddr[17], baseAddr[18], alpha);
        glVertex2s(x3,y3);
        glColor4ub(baseAddr[24], baseAddr[25], baseAddr[26], alpha);
        glVertex2s(x4,y4);
    glEnd();
}

void primPolyFT3(ub *baseAddr) {
    uw *gpuData  = (uw *)baseAddr;
    sh *gpuPoint = (sh *)baseAddr;

    sh x1 = (gpuPoint[ 2] << 21) >> 21;
    sh y1 = (gpuPoint[ 3] << 21) >> 21;
    sh x2 = (gpuPoint[ 6] << 21) >> 21;
    sh y2 = (gpuPoint[ 7] << 21) >> 21;
    sh x3 = (gpuPoint[10] << 21) >> 21;
    sh y3 = (gpuPoint[11] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;

    sh tx0 = (short)(gpuData[2] & 0xff);
    sh ty0 = (short)((gpuData[2]>>8) & 0xff);
    sh tx1 = (short)(gpuData[4] & 0xff);
    sh ty1 = (short)((gpuData[4]>>8) & 0xff);
    sh tx2 = (short)(gpuData[6] & 0xff);
    sh ty2 = (short)((gpuData[6]>>8) & 0xff);
    
    uh gpuDataX = (uh)(gpuData[4]>>16);

    sw tempABR = (gpuDataX >> 5) & 0x3;
    if (vs.texinfo.abr != tempABR) {
        vs.texinfo.abr = tempABR;
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        vs.transparent = TRUE;
    }
    UpdateTextureInfo(gpuDataX);

    sw clutP = (gpuData[2]>>12) & 0x7fff0;
    setupTexture(clutP);
    
    baseAddr[0] = vs.texshade[baseAddr[0]];
    baseAddr[1] = vs.texshade[baseAddr[1]];
    baseAddr[2] = vs.texshade[baseAddr[2]];

    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        if (baseAddr[3] & 1) {
            glColor4ub(255, 255, 255, vs.gAlpha);
            
        }
        else {
            glColor4ub(baseAddr[0], baseAddr[1], baseAddr[2], vs.gAlpha);
        }
    }
    else {
        glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
        vs.transparent = FALSE;
        
        if (baseAddr[3] & 1) {
            glColor3ub(255, 255, 255);
        }
        else {
            glColor3ub(baseAddr[0], baseAddr[1], baseAddr[2]);
        }
    }

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    
    sh xtest = (x1 - x2) * (tx0 - tx1);
    if (xtest == 0) {
        xtest = (x1 - x3) * (tx0 - tx2);
    }
    if (xtest < 0) {
        glTranslatef(1.0f, 0, 0);
    }

    sh ytest = (y1 - y2) * (ty0 - ty1);
    if (ytest == 0) {
        ytest = (y1 - y3) * (ty0 - ty2);
    }
    if (ytest < 0) {
        glTranslatef(0, 1.0f, 0);
    }

    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2s(tx0,ty0);
        glVertex2s(x1,y1);
        glTexCoord2s(tx1,ty1);
        glVertex2s(x2,y2);
        glTexCoord2s(tx2,ty2);
        glVertex2s(x3,y3);
    glEnd();

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D,vs.nullid);
}

void primPolyFT4(ub *baseAddr) {
    uw *gpuData  = (uw *)baseAddr;
    sh *gpuPoint = (sh *)baseAddr;

    sh x1 = (gpuPoint[2]<<21)>>21;
    sh y1 = (gpuPoint[3]<<21)>>21;
    sh x2 = (gpuPoint[6]<<21)>>21;
    sh y2 = (gpuPoint[7]<<21)>>21;
    sh x3 = (gpuPoint[10]<<21)>>21;
    sh y3 = (gpuPoint[11]<<21)>>21;
    sh x4 = (gpuPoint[14]<<21)>>21;
    sh y4 = (gpuPoint[15]<<21)>>21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;
    x4 += vs.psxDraw.offsetX;
    y4 += vs.psxDraw.offsetY;

    sh tx0 = (sh)(gpuData[2] & 0xff);
    sh ty0 = (sh)((gpuData[2]>>8) & 0xff);
    sh tx1 = (sh)(gpuData[4] & 0xff);
    sh ty1 = (sh)((gpuData[4]>>8) & 0xff);
    sh tx2 = (sh)(gpuData[6] & 0xff);
    sh ty2 = (sh)((gpuData[6]>>8) & 0xff);
    sh tx3 = (sh)(gpuData[8] & 0xff);
    sh ty3 = (sh)((gpuData[8]>>8) & 0xff);
    
    uh gpuDataX = (uh)(gpuData[4]>>16);

    sw tempABR = (gpuDataX >> 5) & 0x3;
    if (vs.texinfo.abr != tempABR) {
        vs.texinfo.abr = tempABR;
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        vs.transparent = TRUE;
    }
    UpdateTextureInfo(gpuDataX);

    sw clutP = (gpuData[2]>>12) & 0x7fff0;
    setupTexture(clutP);
    
    if (baseAddr[3] & 2) {
        if (!vs.transparent) {
            glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
            vs.transparent = TRUE;
        }
        if(baseAddr[3] & 1) {
            glColor4ub(255, 255, 255, vs.gAlpha);
            
        }
        else {
            glColor4ub(vs.texshade[baseAddr[0]], vs.texshade[baseAddr[1]], vs.texshade[baseAddr[2]], vs.gAlpha);
        }
    }
    else {
        glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
        vs.transparent = FALSE;
        
        if (baseAddr[3] & 1) {
            glColor3ub(255, 255, 255);
        }
        else {
            glColor3ub(vs.texshade[baseAddr[0]], vs.texshade[baseAddr[1]], vs.texshade[baseAddr[2]]);
        }
    }

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    
    sh xtest = (x1 - x2) ^ (tx0 - tx1);
    if (xtest == 0) {
        xtest = (x1 - x3) ^ (tx0 - tx2);
    }
    if (xtest < 0) {
        glTranslatef(1.0f, 0, 0);
    }

    sh ytest = (y1 - y2) ^ (ty0 - ty1);
    if (ytest == 0) {
        ytest = (y1 - y3) ^ (ty0 - ty2);
    }
    if (ytest < 0) {
        glTranslatef(0, 1.0f, 0);
    }

    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2s(tx0,ty0);
        glVertex2s(x1,y1);
        glTexCoord2s(tx1,ty1);
        glVertex2s(x2,y2);
        glTexCoord2s(tx2,ty2);
        glVertex2s(x3,y3);
        glTexCoord2s(tx3,ty3);
        glVertex2s(x4,y4);
    glEnd();

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D,vs.nullid);
}

void primPolyGT3(ub *baseAddr) {
    uw *gpuData  = (uw *)baseAddr;
    sh *gpuPoint = (sh *)baseAddr;
    ub alpha;

    sh x1 = (gpuPoint[ 2] << 21) >> 21;
    sh y1 = (gpuPoint[ 3] << 21) >> 21;
    sh x2 = (gpuPoint[ 8] << 21) >> 21;
    sh y2 = (gpuPoint[ 9] << 21) >> 21;
    sh x3 = (gpuPoint[14] << 21) >> 21;
    sh y3 = (gpuPoint[15] << 21) >> 21;

    x1 += vs.psxDraw.offsetX;
    y1 += vs.psxDraw.offsetY;
    x2 += vs.psxDraw.offsetX;
    y2 += vs.psxDraw.offsetY;
    x3 += vs.psxDraw.offsetX;
    y3 += vs.psxDraw.offsetY;

    sh tx0 = (sh)((gpuData[2] & 0xff));
    sh ty0 = (sh)((gpuData[2] >> 8) & 0xff);
    sh tx1 = (sh)((gpuData[5] & 0xff));
    sh ty1 = (sh)((gpuData[5] >> 8) & 0xff);
    sh tx2 = (sh)((gpuData[8] & 0xff));
    sh ty2 = (sh)((gpuData[8] >> 8) & 0xff);
    
    uh gpuDataX = (uh)(gpuData[5] >> 16);
    sw tempABR = (gpuDataX >> 5) & 0x3;
    
    if (vs.texinfo.abr != tempABR) {
        vs.texinfo.abr = tempABR;
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        vs.transparent = TRUE;
    }
    
    UpdateTextureInfo(gpuDataX);
    sw clutP = (gpuData[2] >> 12) & 0x7fff0;
    setupTexture(clutP);
    
    if (baseAddr[3] & 2) {
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.transparent = TRUE;
        alpha = vs.gAlpha;
    }
    else {
        glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
        vs.transparent = FALSE;
        alpha = 255;
    }

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    
    sh xtest = (x1 - x2) * (tx0 - tx1);
    if (xtest == 0) {
        xtest = (x1 - x3) * (tx0 - tx2);
    }
    if (xtest < 0) {
        glTranslatef(1.0f, 0, 0);
    }

    sh ytest = (y1 - y2) * (ty0 - ty1);
    if (ytest == 0) {
        ytest = (y1 - y3) * (ty0 - ty2);
    }
    if (ytest < 0) {
        glTranslatef(0, 1.0f, 0);
    }

    glBegin(GL_TRIANGLE_STRIP);
        glColor4ub(vs.texshade[baseAddr[ 0]], vs.texshade[baseAddr[ 1]], vs.texshade[baseAddr[ 2]],alpha);
        glTexCoord2s(tx0, ty0);
        glVertex2s(x1, y1);
        glColor4ub(vs.texshade[baseAddr[12]], vs.texshade[baseAddr[13]], vs.texshade[baseAddr[14]],alpha);
        glTexCoord2s(tx1, ty1);
        glVertex2s(x2, y2);
        glColor4ub(vs.texshade[baseAddr[24]], vs.texshade[baseAddr[25]], vs.texshade[baseAddr[26]],alpha);
        glTexCoord2s(tx2, ty2);
        glVertex2s(x3, y3);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D,vs.nullid);
}

void primPolyGT4(ub *baseAddr) {
    GT4 *hi = (GT4 *)baseAddr;
    sh *gpuPoint = (sh *)baseAddr;
    
    sh x1 = ((hi->v[0].w << 21) >> 21) + vs.psxDraw.offsetX; //(gpuPoint[ 2] << 21) >> 21;
    sh y1 = ((hi->v[0].h << 21) >> 21) + vs.psxDraw.offsetY; //(gpuPoint[ 3] << 21) >> 21;
    sh x2 = ((hi->v[1].w << 21) >> 21) + vs.psxDraw.offsetX; //(gpuPoint[ 8] << 21) >> 21;
    sh y2 = ((hi->v[1].h << 21) >> 21) + vs.psxDraw.offsetY; //(gpuPoint[ 9] << 21) >> 21;
    sh x3 = ((hi->v[2].w << 21) >> 21) + vs.psxDraw.offsetX; //(gpuPoint[14] << 21) >> 21;
    sh y3 = ((hi->v[2].h << 21) >> 21) + vs.psxDraw.offsetY; //(gpuPoint[15] << 21) >> 21;
    sh x4 = ((hi->v[3].w << 21) >> 21) + vs.psxDraw.offsetX; //(gpuPoint[20] << 21) >> 21;
    sh y4 = ((hi->v[3].h << 21) >> 21) + vs.psxDraw.offsetY; //(gpuPoint[21] << 21) >> 21;
    
    sh tx0 = hi->v[0].u; //(sh)((gpuData[2] & 0xff));
    sh ty0 = hi->v[0].v; //(sh)((gpuData[2] >> 8) & 0xff);
    sh tx1 = hi->v[1].u; //(sh)((gpuData[5] & 0xff));
    sh ty1 = hi->v[1].v; //(sh)((gpuData[5] >> 8) & 0xff);
    sh tx2 = hi->v[2].u; //(sh)((gpuData[8] & 0xff));
    sh ty2 = hi->v[2].v; //(sh)((gpuData[8] >> 8) & 0xff);
    sh tx3 = hi->v[3].u; //(sh)((gpuData[11] & 0xff));
    sh ty3 = hi->v[3].v; //(sh)((gpuData[11] >> 8) & 0xff);

    uw *gpuData  = (uw *)baseAddr;
    uh gpuDataX = (uh)(gpuData[5] >> 16);
    sw tempABR = (gpuDataX >> 5) & 0x3;
    
    if (vs.texinfo.abr != tempABR) {
        vs.texinfo.abr = tempABR;
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.gAlpha = vs.TransRate[vs.texinfo.abr].alpha;
        vs.transparent = TRUE;
    }
    
    UpdateTextureInfo(gpuDataX);
    sw clutP = (gpuData[2] >> 12) & 0x7fff0;
    setupTexture(clutP);
    
    ub alpha;
    if (baseAddr[3] & 2) {
        glBlendFunc(vs.TransRate[vs.texinfo.abr].src, vs.TransRate[vs.texinfo.abr].dst);
        vs.transparent = TRUE;
        alpha = vs.gAlpha;
    }
    else {
        glBlendFunc(vs.TransRate[0].src, vs.TransRate[0].dst);
        vs.transparent = FALSE;
        alpha = 255;
    }

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();

    sh xtest = (x1 - x2) * (tx0 - tx1);
    if (xtest == 0) {
        xtest = (x1 - x3) * (tx0 - tx2);
    }
    if (xtest < 0) {
        glTranslatef(1.0f, 0, 0);
    }

    sh ytest = (y1 - y2) * (ty0 - ty1);
    if (ytest == 0) {
        ytest = (y1 - y3) * (ty0 - ty2);
    }
    if (ytest < 0) {
        glTranslatef(0, 1.0f, 0);
    }

    glBegin(GL_TRIANGLE_STRIP);
        glColor4ub(vs.texshade[baseAddr[ 0]], vs.texshade[baseAddr[ 1]], vs.texshade[baseAddr[ 2]],alpha);
        glTexCoord2s(tx0, ty0);
        glVertex2s(x1, y1);
        glColor4ub(vs.texshade[baseAddr[12]], vs.texshade[baseAddr[13]], vs.texshade[baseAddr[14]],alpha);
        glTexCoord2s(tx1, ty1);
        glVertex2s(x2, y2);
        glColor4ub(vs.texshade[baseAddr[24]], vs.texshade[baseAddr[25]], vs.texshade[baseAddr[26]],alpha);
        glTexCoord2s(tx2, ty2);
        glVertex2s(x3, y3);
        glColor4ub(vs.texshade[baseAddr[36]], vs.texshade[baseAddr[37]], vs.texshade[baseAddr[38]],alpha);
        glTexCoord2s(tx3, ty3);
        glVertex2s(x4, y4);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, vs.nullid);
}

/*
    Sprites
*/

void sprite(ub * data, int size) {
    SPRT *hi = (SPRT *)data;
    
    sh x = hi->v[0].w + vs.psxDraw.offsetX;
    sh y = hi->v[0].h + vs.psxDraw.offsetX;
    
    sh tx = hi->v[0].u;
    sh ty = hi->v[0].v;
    
    sh tsz_w = size;
    sh tsz_h = size;
    
    sh vsz_w = size;
    sh vsz_h = size;
    
    if (size > 16) {
        tx += vs.psxDraw.texwinX1;
        ty += vs.psxDraw.texwinY1;
        
        vsz_w = hi->w;
        vsz_h = hi->h;
        
        tsz_w = MIN(vs.psxDraw.texwinX2, vsz_w);
        tsz_h = MIN(vs.psxDraw.texwinY2, vsz_h);
    }
    
    uw *p32 = (uw *)data;
    setupTexture((p32[2] >> 12) & 0x7fff0);
    setSpriteBlendMode(*p32);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLTexCoord2s(tx,         ty);         GLVertex2s(x,         y);
        GLTexCoord2s(tx + tsz_w, ty);         GLVertex2s(x + vsz_w, y);
        GLTexCoord2s(tx,         ty + tsz_h); GLVertex2s(x,         y + vsz_h);
        GLTexCoord2s(tx + tsz_w, ty + tsz_h); GLVertex2s(x + vsz_w, y + vsz_h);
    GLEnd();
    
    if (data[3] & 2) {
        GLEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_EQUAL, 1);
        glColor3ub(255, 255, 255);
        GLDisable(GL_BLEND);
        GLStart(GL_TRIANGLE_STRIP);
            GLTexCoord2s(tx,         ty);         GLVertex2s(x,         y);
            GLTexCoord2s(tx + tsz_w, ty);         GLVertex2s(x + vsz_w, y);
            GLTexCoord2s(tx,         ty + tsz_h); GLVertex2s(x,         y + vsz_h);
            GLTexCoord2s(tx + tsz_w, ty + tsz_h); GLVertex2s(x + vsz_w, y + vsz_h);
        GLEnd();
        GLEnable(GL_BLEND);
        GLDisable(GL_ALPHA_TEST);
    }
    
    GLBindTexture(GL_TEXTURE_2D, vs.nullid);
}

void primSprt8(ub * data) {
    sprite(data, 8);
}

void primSprt16(ub *data) {
    sprite(data, 16);
}

void primSprtS(ub *data) {
    sprite(data, 32);
}

void primNI(ub *bA) {
}

ub primTableC[256] = {
    0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    4,4,4,4,7,7,7,7,5,5,5,5,9,9,9,9,
    6,6,6,6,9,9,9,9,8,8,8,8,12,12,12,12,
    3,3,3,3,0,0,0,0,5,5,5,5,6,6,6,6,
    4,4,4,4,0,0,0,0,7,7,7,7,9,9,9,9,
    3,3,3,3,4,4,4,4,2,2,2,2,0,0,0,0,
    2,2,2,2,3,3,3,3,2,2,2,2,3,3,3,3,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

void (*primTableJ[256])(ub *) = {
    primNI,primNI,primBlkFill,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primPolyF3,primPolyF3,primPolyF3,primPolyF3,primPolyFT3,primPolyFT3,primPolyFT3,primPolyFT3,
    primPolyF4,primPolyF4,primPolyF4,primPolyF4,primPolyFT4,primPolyFT4,primPolyFT4,primPolyFT4,
    primPolyG3,primPolyG3,primPolyG3,primPolyG3,primPolyGT3,primPolyGT3,primPolyGT3,primPolyGT3,
    primPolyG4,primPolyG4,primPolyG4,primPolyG4,primPolyGT4,primPolyGT4,primPolyGT4,primPolyGT4,
    primLineF2,primLineF2,primLineF2,primLineF2,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primLineG2,primLineG2,primLineG2,primLineG2,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primTileS,primTileS,primTileS,primTileS,primSprtS,primSprtS,primSprtS,primSprtS,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primSprt8,primSprt8,primSprt8,primSprt8,
    primNI,primNI,primNI,primNI,primSprt16,primSprt16,primSprt16,primSprt16,
    primMoveImage,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primLoadImage,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primStoreImage,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,cmdTexturePage,cmdTextureWindow,cmdDrawAreaStart,cmdDrawAreaEnd,cmdDrawOffset,cmdSTP,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI,
    primNI,primNI,primNI,primNI,primNI,primNI,primNI,primNI
};
