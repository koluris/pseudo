#import "Global.h"


#define COLOR_MAX   255
#define COLOR_HALF  COLOR_MAX >> 1


CstrDraw draw;

void CstrDraw::reset() {
    blend = 0;
    
    GLViewport(0, 0, 320*2, 240*2);
    GLMatrixMode(GL_MODELVIEW);
    GLID();
    GLEnable(GL_BLEND);
    GLClearColor(0, 0, 0, 0);
    GLClear(GL_COLOR_BUFFER_BIT);
    resize(320, 240);
    GLFlush();
}

void CstrDraw::resize(uh resX, uh resY) {
    if (resX && resY) {
        GLMatrixMode(GL_PROJECTION);
        GLID();
        GLOrtho(0.0, resX, resY, 0.0, 1.0, -1.0);
    }
}

void CstrDraw::refresh() {
    vs.ret.status ^= GPU_ODDLINES;
    GLFlush();
}

void CstrDraw::blockFill(uw *data) {
    BLK *k = (BLK *)data;
    
    GLColor4ub(k->co.r, k->co.c, k->co.b, COLOR_MAX);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(k->v[0].w,      k->v[0].h);
        GLVertex2s(k->v[0].w+k->w, k->v[0].h);
        GLVertex2s(k->v[0].w,      k->v[0].h+k->h);
        GLVertex2s(k->v[0].w+k->w, k->v[0].h+k->h);
    GLEnd();
}

template <class T>
void CstrDraw::drawF(uw *data, ub size, GLenum mode) {
    T *k = (T *)data;
    
    ub b[] = {
        k->co.a & 2 ? blend : (ub)0,
        k->co.a & 2 ? bit[blend].opaque : (ub)COLOR_MAX
    };
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    GLColor4ub(k->co.r, k->co.c, k->co.b, b[1]);
    
    GLStart(mode);
    for (sw i = 0; i < size; i++) {
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

template <class T>
void CstrDraw::drawG(uw *data, ub size, GLenum mode) {
    T *k = (T *)data;
    
    ub b[] = {
        k->v[0].co.a & 2 ? blend : (ub)0,
        k->v[0].co.a & 2 ? bit[blend].opaque : (ub)COLOR_MAX
    };
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    GLStart(mode);
    for (sw i = 0; i < size; i++) {
        GLColor4ub(k->v[i].co.r, k->v[i].co.c, k->v[i].co.b, b[1]);
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

template <class T>
void CstrDraw::drawFT(uw *data, ub size) {
    T *k = (T *)data;
    
    blend = (k->v[1].clut >> 5) & 3;
    
    ub b[] = {
        k->co.a & 2 ? blend : (ub)0,
        k->co.a & 2 ? bit[blend].opaque : (ub)COLOR_MAX
    };
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    if (k->co.a & 1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
    }
    else {
        GLColor4ub(k->co.r, k->co.c, k->co.b, b[1]);
    }
    
    GLStart(GL_TRIANGLE_STRIP);
    for (sw i = 0; i < size; i++) {
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

template <class T>
void CstrDraw::drawGT(uw *data, ub size) {
    T *k = (T *)data;
    
    blend = (k->v[1].clut >> 5) & 3;
    
    ub b[] = {
        k->v[0].co.a & 2 ? blend : (ub)0,
        k->v[0].co.a & 2 ? bit[blend].opaque : (ub)COLOR_MAX
    };
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    GLStart(GL_TRIANGLE_STRIP);
    for (sw i = 0; i < size; i++) {
        GLColor4ub(k->v[i].co.r, k->v[i].co.c, k->v[i].co.b, b[1]);
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

void CstrDraw::drawTile(uw *data, sh size) {
    BLK *k = (BLK *)data;
    
    if (size) {
        k->w = size;
        k->h = size;
    }
    
    ub b[] = {
        k->co.a & 2 ? blend : (ub)0,
        k->co.a & 2 ? bit[blend].opaque : (ub)COLOR_MAX
    };
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    GLColor4ub(k->co.r, k->co.c, k->co.b, b[1]);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(k->v[0].w,      k->v[0].h);
        GLVertex2s(k->v[0].w+k->w, k->v[0].h);
        GLVertex2s(k->v[0].w,      k->v[0].h+k->h);
        GLVertex2s(k->v[0].w+k->w, k->v[0].h+k->h);
    GLEnd();
}

void CstrDraw::drawSprite(uw *data, sh size) {
    SPRT *k = (SPRT *)data;
    
    if (size) {
        k->w = size;
        k->h = size;
    }
    
    ub b[] = {
        k->co.a & 2 ? blend : (ub)0,
        k->co.a & 2 ? bit[blend].opaque : (ub)COLOR_MAX
    };
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    if (k->co.a & 1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
    }
    else {
        GLColor4ub(k->co.r, k->co.c, k->co.b, b[1]);
    }
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(k->v[0].w,      k->v[0].h);
        GLVertex2s(k->v[0].w+k->w, k->v[0].h);
        GLVertex2s(k->v[0].w,      k->v[0].h+k->h);
        GLVertex2s(k->v[0].w+k->w, k->v[0].h+k->h);
    GLEnd();
}

void CstrDraw::primitive(uw addr, uw *data) {
    // Primitives
    switch(addr & 0xfc) {
        case 0x20: // Vertex F3
            drawF<F3>(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x24: // Vertex FT3
            drawFT<FT3>(data, 3);
            return;
            
        case 0x28: // Vertex F4
            drawF<F4>(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x2c: // Vertex FT4
            drawFT<FT4>(data, 4);
            return;
            
        case 0x30: // Vertex G3
            drawG<G3>(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x34: // Vertex GT3
            drawGT<GT3>(data, 3);
            return;
            
        case 0x38: // Vertex G4
            drawG<G4>(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x3c: // Vertex GT4
            drawGT<GT4>(data, 4);
            return;
            
        case 0x40: // Line F2
            drawF<F2>(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x48: // Line F3
            drawF<F3>(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x4c: // Line F4
            drawF<F4>(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x50: // Line G2
            drawG<G2>(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x58: // Line G3
            drawG<G3>(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x5c: // Line G4
            drawG<G4>(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x60: // Tile
            drawTile(data, 0);
            return;
            
        case 0x64: // Sprite
            drawSprite(data, 0);
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
            blockFill(data);
            return;
            
        case 0x80: // TODO: Move photo
            return;
            
        case 0xa0: // TODO: Load photo
            return;
            
        case 0xc0: // TODO: Store photo
            return;
            
        case 0xe1: // Texture P.
            blend  = (data[0] >> 5) & 0x3;
            vs.ret.status = vs.ret.status & (~0x7ff);
            GLBlendFunc(bit[blend].src, bit[blend].dst);
            return;
            
        case 0xe2: // TODO: Texture Window
            return;
            
        case 0xe3: // TODO: Draw Area Start
            return;
            
        case 0xe4: // TODO: Draw Area End
            return;
            
        case 0xe5: // TODO: Draw Offset
            return;
            
        case 0xe6: // STP
            vs.ret.status = (vs.ret.status & (~(3 << 11))) | ((data[0] & 3) << 11);
            return;
    }
    
    printx("/// PSeudo primitive: $%x", addr);
}
