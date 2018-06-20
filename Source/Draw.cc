#import "Global.h"


CstrDraw draw;

void CstrDraw::init(sh w, sh h, int multiplier) {
    window = { 0 };
    
    window.h = w;
    window.v = h;
    window.multiplier = multiplier;
    
    draw.reset();
}

void CstrDraw::reset() {
    res    = { 0 };
    offset = { 0 };
    
    opaque   = 0;
    spriteTP = 0;
    
    // OpenGL
    GLViewport(0, 0, window.h, window.v);
    
    if (window.multiplier > 1) {
        // Crap
        GLLineWidth(window.multiplier);
    }
    
    GLEnable(GL_BLEND);
    GLEnable(GL_CLIP_PLANE0);
    GLEnable(GL_CLIP_PLANE1);
    GLEnable(GL_CLIP_PLANE2);
    GLEnable(GL_CLIP_PLANE3);
    
    // Textures
    GLMatrixMode(GL_TEXTURE);
    GLID();
    GLScalef(1 / 256.0, 1 / 256.0, 1);
    GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    GLTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
    //GLTexEnvi(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 2);
    
    // Redraw
    resize(window.h, window.v);
    GLClearColor(0, 0, 0, 0);
    GLClear(GL_COLOR_BUFFER_BIT);
    GLFlush();
}

void CstrDraw::resize(sh w, sh h) {
    // Not current
    if (res.h != w || res.v != h) {
#if 0
        GLViewport((window.h - w) / 2, (window.v - h) / 2, w, h); // Keep PSX aspect ratio
#endif
        GLMatrixMode(GL_PROJECTION);
        GLID();
        GLOrtho(0, w, h, 0, 1, -1);
        
        // Make current
        res.h = w;
        res.v = h;
    }
}

void CstrDraw::drawRect(uw *data) {
    TILEx *k = (TILEx *)data;
    
    GLDisable(GL_CLIP_PLANE0);
    GLDisable(GL_CLIP_PLANE1);
    GLDisable(GL_CLIP_PLANE2);
    GLDisable(GL_CLIP_PLANE3);
    
    GLColor4ub(k->c.a, k->c.b, k->c.c, COLOR_MAX);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(k->vx.w,        k->vx.h);
        GLVertex2s(k->vx.w + k->w, k->vx.h);
        GLVertex2s(k->vx.w,        k->vx.h + k->h);
        GLVertex2s(k->vx.w + k->w, k->vx.h + k->h);
    GLEnd();
    
    //GLRecti(k->vx.w, k->vx.h, k->vx.w + k->w, k->vx.h + k->h);
    
    GLEnable(GL_CLIP_PLANE0);
    GLEnable(GL_CLIP_PLANE1);
    GLEnable(GL_CLIP_PLANE2);
    GLEnable(GL_CLIP_PLANE3);
}

void CstrDraw::drawF(uw *data, int size, GLenum mode) {
    PFx *k = (PFx *)data;
    
    // Special case
    if (mode == GL_LINE_STRIP) {
        if (size > 2) {
            size = 256;
        }
    }
    
    const ub *b = opaqueFunc(k->c.n);
    GLColor4ub(k->c.a, k->c.b, k->c.c, b[1]);
    
    GLStart(mode);
    for (int i = 0; i < size; i++) {
        if (size == 256) { // Special case
            if (*(uw *)&k->vx[i] == LINE_TERM_CODE) {
                break;
            }
        }
        GLVertex2s(k->vx[i].w + offset.h, k->vx[i].h + offset.v);
    }
    GLEnd();
}

void CstrDraw::drawG(uw *data, int size, GLenum mode) {
    PGx *k = (PGx *)data;
    
    // Special case
    if (mode == GL_LINE_STRIP) {
        if (size > 3) {
            size = 256;
        }
    }
    
    const ub *b = opaqueFunc(k->vx[0].c.n);
    
    GLStart(mode);
    for (int i = 0; i < size; i++) {
        if (size == 256) { // Special case
            if (*(uw *)&k->vx[i] == LINE_TERM_CODE) {
                break;
            }
        }
        GLColor4ub(k->vx[i].c.a, k->vx[i].c.b, k->vx[i].c.c, b[1]);
        GLVertex2s(k->vx[i].w + offset.h, k->vx[i].h + offset.v);
    }
    GLEnd();
}

void CstrDraw::drawFT(uw *data, int size) {
    PFTx *k = (PFTx *)data;
    
    opaque = (k->vx[1].clut >> 5) & 3;
    
    const ub *b = opaqueFunc(k->c.n);
    
    if (k->c.n & 1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
    }
    else {
        GLColor4ub(k->c.a, k->c.b, k->c.c, b[1]);
    }
    
    GLEnable(GL_TEXTURE_2D);
    cache.fetchTexture(k->vx[1].clut, k->vx[0].clut);
    
    GLStart(GL_TRIANGLE_STRIP);
    for (int i = 0; i < size; i++) {
        GLTexCoord2s(k->vx[i].u, k->vx[i].v);
        GLVertex2s  (k->vx[i].w + offset.h, k->vx[i].h + offset.v);
    }
    GLEnd();
    
    GLDisable(GL_TEXTURE_2D);
}

void CstrDraw::drawGT(uw *data, int size) {
    PGTx *k = (PGTx *)data;
    
    opaque = (k->vx[1].clut >> 5) & 3;
    
    const ub *b = opaqueFunc(k->vx[0].c.n);
    
    GLEnable(GL_TEXTURE_2D);
    cache.fetchTexture(k->vx[1].clut, k->vx[0].clut);
    
    GLStart(GL_TRIANGLE_STRIP);
    for (int i = 0; i < size; i++) {
        GLColor4ub  (k->vx[i].c.a, k->vx[i].c.b, k->vx[i].c.c, b[1]);
        GLTexCoord2s(k->vx[i].u, k->vx[i].v);
        GLVertex2s  (k->vx[i].w + offset.h, k->vx[i].h + offset.v);
    }
    GLEnd();
    
    GLDisable(GL_TEXTURE_2D);
}

void CstrDraw::drawTile(uw *data, int size) {
    TILEx *k = (TILEx *)data;
    
    if (size) {
        k->w = size;
        k->h = size;
    }
    
    const ub *b = opaqueFunc(k->c.n);
    GLColor4ub(k->c.a, k->c.b, k->c.c, b[1]);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(k->vx.w + offset.h,        k->vx.h + offset.v);
        GLVertex2s(k->vx.w + offset.h + k->w, k->vx.h + offset.v);
        GLVertex2s(k->vx.w + offset.h,        k->vx.h + offset.v + k->h);
        GLVertex2s(k->vx.w + offset.h + k->w, k->vx.h + offset.v + k->h);
    GLEnd();
}

void CstrDraw::drawSprite(uw *data, int size) {
    SPRTx *k = (SPRTx *)data;
    
    if (size) {
        k->w = size;
        k->h = size;
    }
    
    const ub *b = opaqueFunc(k->c.n);
    
    if (k->c.n & 1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
    }
    else {
        GLColor4ub(k->c.a, k->c.b, k->c.c, b[1]);
    }
    
    GLEnable(GL_TEXTURE_2D);
    cache.fetchTexture(spriteTP, k->vx.clut);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLTexCoord2s(k->vx.u,      k->vx.v);      GLVertex2s(k->vx.w+offset.h,      k->vx.h+offset.v);
        GLTexCoord2s(k->vx.u+k->w, k->vx.v);      GLVertex2s(k->vx.w+offset.h+k->w, k->vx.h+offset.v);
        GLTexCoord2s(k->vx.u,      k->vx.v+k->h); GLVertex2s(k->vx.w+offset.h,      k->vx.h+offset.v+k->h);
        GLTexCoord2s(k->vx.u+k->w, k->vx.v+k->h); GLVertex2s(k->vx.w+offset.h+k->w, k->vx.h+offset.v+k->h);
    GLEnd();
    
//    if (k->c.n & 2) {
//        GLEnable(GL_ALPHA_TEST);
//        glAlphaFunc(GL_GEQUAL, 1);
//        GLColor4ub(255, 255, 255, 255);
//        GLDisable(GL_BLEND);
//        GLStart(GL_TRIANGLE_STRIP);
//        GLTexCoord2s(k->vx.u,      k->vx.v);      GLVertex2s(k->vx.w+offset.h,      k->vx.h+offset.v);
//        GLTexCoord2s(k->vx.u+k->w, k->vx.v);      GLVertex2s(k->vx.w+offset.h+k->w, k->vx.h+offset.v);
//        GLTexCoord2s(k->vx.u,      k->vx.v+k->h); GLVertex2s(k->vx.w+offset.h,      k->vx.h+offset.v+k->h);
//        GLTexCoord2s(k->vx.u+k->w, k->vx.v+k->h); GLVertex2s(k->vx.w+offset.h+k->w, k->vx.h+offset.v+k->h);
//        GLEnd();
//        GLEnable(GL_BLEND);
//        GLDisable(GL_ALPHA_TEST);
//    }
    
    GLDisable(GL_TEXTURE_2D);
}

ub *CstrDraw::opaqueFunc(ub n) {
    ub *b = new ub[2];
    b[0] = n & 2 ? opaque : 0;
    b[1] = n & 2 ? bit[opaque].trans : COLOR_MAX;
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    return b;
}

void CstrDraw::setDrawArea(int plane, uw data) {
    double e1[] = { 1, 0, 0, (data) & 0x3ff };
    double e2[] = { 0, 1, 0, (data >> 10) & 0x1ff };
    
    if (plane) {
        e1[0] = -e1[0];
        e2[1] = -e2[1];
        
        e1[3] += 1;
        e2[3] += 1;
    }
    else {
        e1[3] = -e1[3];
        e2[3] = -e2[3];
    }
    
    GLClipPlane(GL_CLIP_PLANE0 + (plane + 0), e1);
    GLClipPlane(GL_CLIP_PLANE0 + (plane + 1), e2);
}

void CstrDraw::primitive(uw addr, uw *data) {
    // Primitives
    switch(addr & 0xfc) {
        case 0x20: // Vertex F3
            drawF(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x24: // Vertex FT3
            drawFT(data, 3);
            return;
            
        case 0x28: // Vertex F4
            drawF(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x2c: // Vertex FT4
            drawFT(data, 4);
            return;
            
        case 0x30: // Vertex G3
            drawG(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x34: // Vertex GT3
            drawGT(data, 3);
            return;
            
        case 0x38: // Vertex G4
            drawG(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x3c: // Vertex GT4
            drawGT(data, 4);
            return;
            
        case 0x40: // Line F2
            drawF(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x48: // Line F3
            drawF(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x4c: // Line F4
            drawF(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x50: // Line G2
            drawG(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x58: // Line G3
            drawG(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x5c: // Line G4
            drawG(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x60: // Tile
            drawTile(data, NULL);
            return;
            
        case 0x64: // Sprite
            drawSprite(data, NULL);
            return;
            
        case 0x68: // Tile 1
            drawTile(data, 1);
            return;
            
        case 0x70: // Tile 8
            drawTile(data, 8);
            return;
            
        case 0x74: // Sprite 8
            drawSprite(data, 8);
            return;
            
        case 0x78: // Tile 16
            drawTile(data, 16);
            return;
            
        case 0x7c: // Sprite 16
            drawSprite(data, 16);
            return;
    }
    
    // Operations
    switch(addr) {
        case 0x01: // TODO: Flush
            return;
            
        case 0x02: // Block Fill
            drawRect(data);
            return;
            
        case 0x80: // TODO: Move photo
            return;
            
        case 0xa0: // Load photo
            vs.photoRead(data);
            return;
            
        case 0xc0: // TODO: Store photo
            return;
            
        case 0xe1: // Texture P.
            spriteTP = (data[0]) & 0x7ff;
            opaque   = (data[0] >> 5) & 3;
            GLBlendFunc(bit[opaque].src, bit[opaque].dst);
            return;
            
        case 0xe2: // TODO: Texture Window
            return;
            
        case 0xe3: // Draw Area Start
            setDrawArea(0, data[0]);
            return;
            
        case 0xe4: // Draw Area End
            setDrawArea(2, data[0]);
            return;
            
        case 0xe5: // Draw Offset
            offset.h = ((sw)data[0] << 21) >> 21;
            offset.v = ((sw)data[0] << 10) >> 21;
            return;
            
        case 0xe6: // TODO: STP
            return;
    }
    
    printx("/// PSeudo primitive: $%x", addr);
}
