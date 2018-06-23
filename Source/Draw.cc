#import "Global.h"


CstrDraw draw;

void CstrDraw::init(sh w, sh h, int multiplier) {
    window.h = w;
    window.v = h;
    window.multiplier = multiplier;
    
    reset();
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
    
    opaqueClipState(true);
    
    // Textures
    GLMatrixMode(GL_TEXTURE);
    GLID();
    GLScalef(1.0 / 256, 1.0 / 256, 1.0);
    GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    GLTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
    
    // Redraw
    resize(window.h, window.v);
    GLClearColor(0, 0, 0, 0);
    GLClear(GL_COLOR_BUFFER_BIT);
    GLFlush();
    
    // 16-bit texture
    cache.createTexture(&fb16tex, FRAME_W, FRAME_H);
}

#define SHOW_VRAM \
    0

#define KEEP_ASPECT_RATIO \
    0

void CstrDraw::resize(sh w, sh h) {
#if SHOW_VRAM
    w = FRAME_W;
    h = FRAME_H;
#endif
    
    // Not current
    if (res.h != w || res.v != h) {
#if KEEP_ASPECT_RATIO
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

void CstrDraw::opaqueClipState(bool enable) {
    if (enable) {
        GLEnable(GL_BLEND);
        GLEnable(GL_CLIP_PLANE0);
        GLEnable(GL_CLIP_PLANE1);
        GLEnable(GL_CLIP_PLANE2);
        GLEnable(GL_CLIP_PLANE3);
    }
    else {
        GLDisable(GL_BLEND);
        GLDisable(GL_CLIP_PLANE0);
        GLDisable(GL_CLIP_PLANE1);
        GLDisable(GL_CLIP_PLANE2);
        GLDisable(GL_CLIP_PLANE3);
    }
}

void CstrDraw::outputVRAM(uw *raw, sh X, sh Y, sh W, sh H) {
    // Disable state
    opaqueClipState(false);
    
    GLMatrixMode(GL_TEXTURE);
    GLPushMatrix();
    GLID();
    GLScalef(1.0 / FRAME_W, 1.0 / FRAME_H, 1.0);
    
    GLColor4ub(127, 127, 127, 255);
    
    GLEnable(GL_TEXTURE_2D);
    GLBindTexture  (GL_TEXTURE_2D, fb16tex);
    GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGBA, GL_UNSIGNED_BYTE, raw);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLTexCoord2s(0, 0); GLVertex2s(X,   Y);
        GLTexCoord2s(W, 0); GLVertex2s(X+W, Y);
        GLTexCoord2s(0, H); GLVertex2s(X,   Y+H);
        GLTexCoord2s(W, H); GLVertex2s(X+W, Y+H);
    GLEnd();
    
    GLDisable(GL_TEXTURE_2D);
    GLPopMatrix();
    
    // Enable state
    opaqueClipState(true);
}

void CstrDraw::drawRect(uw *packet) {
    TILEx *p = (TILEx *)packet;
    
    opaqueClipState(false);
    GLColor4ub(p->hue.r, p->hue.c, p->hue.b, COLOR_MAX);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(p->v.coords.w,             p->v.coords.h);
        GLVertex2s(p->v.coords.w + p->size.w, p->v.coords.h);
        GLVertex2s(p->v.coords.w,             p->v.coords.h + p->size.h);
        GLVertex2s(p->v.coords.w + p->size.w, p->v.coords.h + p->size.h);
    GLEnd();
    
    opaqueClipState(true);
}

void CstrDraw::drawF(uw *packet, int size, GLenum mode) {
    PFx *p = (PFx *)packet;
    
    // Special case
    if (mode == GL_LINE_STRIP) { // eur-001.psx, fuzzion.psx
        if (size > 2) {
            size = 256;
        }
    }
    
    ub *b = opaqueFunc(p->hue.a);
    GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
    
    GLStart(mode);
    for (int i = 0; i < size; i++) {
        if (size == 256) { // Special case
            if (*(uw *)&p->v[i] == LINE_TERM_CODE) {
                break;
            }
        }
        GLVertex2s(p->v[i].coords.w, p->v[i].coords.h);
    }
    GLEnd();
}

void CstrDraw::drawG(uw *packet, int size, GLenum mode) {
    PGx *p = (PGx *)packet;
    
    // Special case
    if (mode == GL_LINE_STRIP) { // mups-016.psx, pdx-030.psx, pdx-074.psx, pop-n-pop.psx
        if (size > 3) {
            size = 256;
        }
    }
    
    ub *b = opaqueFunc(p->v[0].hue.a);
    
    GLStart(mode);
    for (int i = 0; i < size; i++) {
        if (size == 256) { // Special case
            if (*(uw *)&p->v[i] == LINE_TERM_CODE) {
                break;
            }
        }
        GLColor4ub(p->v[i].   hue.r, p->v[i].   hue.c, p->v[i].hue.b, b[1]);
        GLVertex2s(p->v[i].coords.w, p->v[i].coords.h);
    }
    GLEnd();
}

void CstrDraw::drawFT(uw *packet, int size) {
    PFTx *p = (PFTx *)packet;
    
    opaque = (p->v[1].tex.clut >> 5) & 3;
    ub *b = opaqueFunc(p->hue.a);
    
    if (p->hue.a & 1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
    }
    else {
        GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
    }
    
    GLEnable(GL_TEXTURE_2D);
    cache.fetchTexture(p->v[1].tex.clut, p->v[0].tex.clut);
    
    GLStart(GL_TRIANGLE_STRIP);
    for (int i = 0; i < size; i++) {
        GLTexCoord2s(p->v[i].   tex.u, p->v[i].   tex.v);
        GLVertex2s  (p->v[i].coords.w, p->v[i].coords.h);
    }
    GLEnd();
    
    GLDisable(GL_TEXTURE_2D);
}

void CstrDraw::drawGT(uw *packet, int size) {
    PGTx *p = (PGTx *)packet;
    
    opaque = (p->v[1].tex.clut >> 5) & 3;
    ub *b = opaqueFunc(p->v[0].hue.a);
    
    GLEnable(GL_TEXTURE_2D);
    cache.fetchTexture(p->v[1].tex.clut, p->v[0].tex.clut);
    
    GLStart(GL_TRIANGLE_STRIP);
    for (int i = 0; i < size; i++) {
        GLColor4ub  (p->v[i].   hue.r, p->v[i].   hue.c, p->v[i].hue.b, b[1]);
        GLTexCoord2s(p->v[i].   tex.u, p->v[i].   tex.v);
        GLVertex2s  (p->v[i].coords.w, p->v[i].coords.h);
    }
    GLEnd();
    
    GLDisable(GL_TEXTURE_2D);
}

void CstrDraw::drawTile(uw *packet, int size) {
    TILEx *p = (TILEx *)packet;
    
    if (size) {
        p->size.w = size;
        p->size.h = size;
    }
    
    ub *b = opaqueFunc(p->hue.a);
    GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLVertex2s(p->v.coords.w,             p->v.coords.h);
        GLVertex2s(p->v.coords.w + p->size.w, p->v.coords.h);
        GLVertex2s(p->v.coords.w,             p->v.coords.h + p->size.h);
        GLVertex2s(p->v.coords.w + p->size.w, p->v.coords.h + p->size.h);
    GLEnd();
}

void CstrDraw::drawSprite(uw *packet, int size) {
    SPRTx *p = (SPRTx *)packet;
    
    if (size) {
        p->size.w = size;
        p->size.h = size;
    }
    
    ub *b = opaqueFunc(p->hue.a);
    
    if (p->hue.a & 1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
    }
    else {
        GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
    }
    
    GLEnable(GL_TEXTURE_2D);
    cache.fetchTexture(spriteTP, p->v.tex.clut);
    
    GLStart(GL_TRIANGLE_STRIP);
        GLTexCoord2s(p->v.   tex.u,             p->v.   tex.v);
        GLVertex2s  (p->v.coords.w,             p->v.coords.h);
        GLTexCoord2s(p->v.   tex.u + p->size.w, p->v.   tex.v);
        GLVertex2s  (p->v.coords.w + p->size.w, p->v.coords.h);
        GLTexCoord2s(p->v.   tex.u,             p->v.   tex.v + p->size.h);
        GLVertex2s  (p->v.coords.w,             p->v.coords.h + p->size.h);
        GLTexCoord2s(p->v.   tex.u + p->size.w, p->v.   tex.v + p->size.h);
        GLVertex2s  (p->v.coords.w + p->size.w, p->v.coords.h + p->size.h);
    GLEnd();
    
    GLDisable(GL_TEXTURE_2D);
}

ub *CstrDraw::opaqueFunc(ub a) {
    ub *b = new ub[2];
    b[0] = a & 2 ? opaque : 0;
    b[1] = a & 2 ? bit[opaque].trans : COLOR_MAX;
    
    GLBlendFunc(bit[b[0]].src, bit[b[0]].dst);
    
    return b;
}

void CstrDraw::setDrawArea(int plane, uw data) {
    double e1[] = { 1, 0, 0, (data) & 0x3ff };
    double e2[] = { 0, 1, 0, (data >> 10) & 0x1ff };
    
    if (plane) {
        e1[0] = -e1[0]; e1[3]++; // ?
        e2[1] = -e2[1]; e2[3]++; // ?
    }
    else {
        e1[3] = -e1[3];
        e2[3] = -e2[3];
    }
    
    GLClipPlane(GL_CLIP_PLANE0 + (plane + 0), e1);
    GLClipPlane(GL_CLIP_PLANE0 + (plane + 1), e2);
}

enum {
    GPU_TYPE_CMD,
    GPU_TYPE_POLYGON,
    GPU_TYPE_LINE,
    GPU_TYPE_SPRITE,
    GPU_TYPE_IMG_MOVE,
    GPU_TYPE_IMG_SEND,
    GPU_TYPE_IMG_COPY,
    GPU_TYPE_ENV
};

struct POLY {
    ub exposure    : 1;
    ub transparent : 1;
    ub texture     : 1;
    ub vertices    : 1;
    ub shade       : 1;
    ub             : 3;
};

struct RGBC {
    ub r, c, b, a;
};

struct TEX {
    ub u, v; uh tp;
};

struct POINT {
    sh w, h;
};

template <class T>
void parse(T *components, uw *packets, int start, int step) {
    for (uw i = 0, *p = &packets[start]; i < 4; i++, p += step) {
        components[i] = *(T  *)&p;
    }
}

void CstrDraw::primitive(uw addr, uw *packets) {
    switch((addr >> 5) & 7) {
        case GPU_TYPE_CMD:
            switch(addr) {
                case 0x01: // Flush
                    vs.write(0x1f801814, 0x01000000);
                    return;
                    
                default:
                    printx("/// PSeudo GPU_TYPE_CMD unknown: 0x%x", addr);
                    return;
            }
            return;
            
        case GPU_TYPE_POLYGON:
            {
                POLY *b = (POLY *)&addr;
                
                // Basic components
                RGBC  *hue[4];
                POINT * vx[4];
                TEX   *tex[4];
                
                if (b->shade) {
                    // Gouraud
                    if (b->texture) {
                        parse(hue, packets, 0, 3);
                        parse( vx, packets, 1, 3);
                        parse(tex, packets, 2, 3);
                    }
                    else {
                        parse(hue, packets, 0, 2);
                        parse( vx, packets, 1, 2);
                    }
                }
                else {
                    // Flat
                    hue[0] = (RGBC *)&packets[0];
                    GLColor4ub(hue[0]->r, hue[0]->c, hue[0]->b, 255);
                    
                    if (b->texture) {
                        parse( vx, packets, 1, 2);
                        parse(tex, packets, 2, 2);
                    }
                    else {
                        parse( vx, packets, 1, 1);
                    }
                }
                
                if (b->texture) {
                    GLEnable(GL_TEXTURE_2D);
                    cache.fetchTexture(tex[1]->tp, tex[0]->tp);
                }
                
                GLStart(GL_TRIANGLE_STRIP);
                for (int i = 0; i < (b->vertices ? 4 : 3); i++) {
                    if (b->shade  ) GLColor4ub  (hue[i]->r, hue[i]->c, hue[i]->b, 255);
                    if (b->texture) GLTexCoord2s(tex[i]->u, tex[i]->v);
                    GLVertex2s(vx[i]->w, vx[i]->h);
                }
                GLEnd();
                GLDisable(GL_TEXTURE_2D);
            }
            return;
            
        case GPU_TYPE_LINE:
            return;
            
        case GPU_TYPE_SPRITE:
            return;
            
        case GPU_TYPE_IMG_MOVE:
            vs.photoMove(data);
            return;
            
        case GPU_TYPE_IMG_SEND:
            vs.photoRead(data);
            return;
            
        case GPU_TYPE_IMG_COPY:
            return;
            
        case GPU_TYPE_ENV:
            switch(addr) {
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
                    
                default:
                    printx("/// PSeudo GPU_TYPE_ENV unknown: 0x%x", addr);
                    return;
            }
            return;
    }
    
    printx("/// PSeudo GPU command unknown: 0x%08x / %d", data[0], ((addr >> 5) & 7));
    
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
        case 0x01: // Flush
            vs.write(0x1f801814, 0x01000000);
            return;
            
        case 0x02: // Block Fill
            drawRect(data);
            return;
            
        case 0x80: // TODO: Move photo
            vs.photoMove(data);
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
