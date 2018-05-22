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
    // Operations
    switch(addr) {
        case 0x01: // TODO: Flush
            return;
            
        case 0x02: // Block Fill
            blockFill(data);
            return;
            
        case 0x20: // TODO: Vertex F3
        case 0x21:
        case 0x22:
        case 0x23:
            drawF<F3>(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x24: // TODO: Vertex FT3
        case 0x25:
        case 0x26:
        case 0x27:
            drawFT<FT3>(data, 3);
            return;
            
        case 0x28: // TODO: Vertex F4
        case 0x29:
        case 0x2a:
            drawF<F4>(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x2c: // TODO: Vertex FT4
        case 0x2d:
        case 0x2e:
        case 0x2f:
            drawFT<FT4>(data, 4);
            return;
            
        case 0x30: // TODO: Vertex G3
        case 0x31:
        case 0x32:
        case 0x33:
            drawG<G3>(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x34: // TODO: Vertex GT3
        case 0x35:
        case 0x36:
        case 0x37:
            drawGT<GT3>(data, 3);
            return;
            
        case 0x38: // TODO: Vertex G4
        case 0x39:
        case 0x3a:
            drawG<G4>(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x3c: // TODO: Vertex GT4
        case 0x3d:
        case 0x3e:
            drawGT<GT4>(data, 4);
            return;
            
        case 0x40: // TODO: Line F2
        case 0x41:
        case 0x42:
        case 0x43:
            drawF<F2>(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x49: // TODO: Line F3
        case 0x4a:
            drawF<F3>(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x4c: // TODO: Line F4
        case 0x4d:
        case 0x4e:
        case 0x4f:
            drawF<F4>(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x50: // TODO: Line G2
        case 0x51:
        case 0x52:
        case 0x53:
            drawG<G2>(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x58: // TODO: Line G3
        case 0x59:
        case 0x5a:
        case 0x5b:
            drawG<G3>(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x5c: // TODO: Line G4
        case 0x5e:
        case 0x5f:
            drawG<G4>(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x60: // TODO: Tile
        case 0x61:
        case 0x62:
        case 0x63:
            drawTile(data, 0);
            return;
            
        case 0x64: // TODO: Sprite
        case 0x65:
        case 0x66:
        case 0x67:
            drawSprite(data, 0);
            return;
            
        case 0x68: // TODO: Tile 1
        case 0x69:
        case 0x6a:
        case 0x6b:
            drawTile(data, 1);
            return;
            
        case 0x70: // TODO: Tile 8
        case 0x71:
        case 0x72:
        case 0x73:
            drawTile(data, 8);
            return;
            
        case 0x74: // TODO: Sprite 8
        case 0x75:
        case 0x76:
        case 0x77:
            drawSprite(data, 8);
            return;
            
        case 0x79: // TODO: Tile 16
        case 0x7a:
        case 0x7b:
            drawTile(data, 16);
            return;
            
        case 0x7c: // TODO: Sprite 16
        case 0x7d:
        case 0x7f:
            drawSprite(data, 16);
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
    printx("PSeudo /// GPU Draw -> $%x", addr);
}
