/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


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
    
    if (window.multiplier > 1) { // Crap
        GLLineWidth((GLfloat)window.multiplier);
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
    swapBuffers();
    
    // 16-bit texture
    cache.createTexture(&fb16tex, FRAME_W, FRAME_H);
}

void CstrDraw::swapBuffers() {
#ifdef APPLE_MACOS
    [[app.openGLView openGLContext] flushBuffer];
#elif  APPLE_IOS
    // TODO
#endif
}

void CstrDraw::resize(sh w, sh h) {
#if 0
    w = FRAME_W;
    h = FRAME_H;
#endif
    
    // Not current
    if (res.h != w || res.v != h) {
#if 0
        GLViewport((window.h - w) / 2, (window.v - h) / 2, w, h); // Keep PSX aspect ratio
#endif
        GLMatrixMode(GL_PROJECTION);
        GLID();
        
#ifdef APPLE_MACOS
        GLOrtho (0, w, h, 0, 1, -1);
#elif  APPLE_IOS
        GLOrthof(0, w, h, 0, 1, -1);
#endif
        
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

ub CstrDraw::opaqueFunc(ub a) {
    ub b1 = a ? opaque : 0;
    ub b2 = a ? bit[opaque].trans : COLOR_MAX;
    
    GLBlendFunc(bit[b1].src, bit[b1].dst);
    
    return b2;
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
    
#ifdef APPLE_MACOS
    GLClipPlane (GL_CLIP_PLANE0 + (plane + 0), e1);
    GLClipPlane (GL_CLIP_PLANE0 + (plane + 1), e2);
#elif  APPLE_IOS
    GLClipPlanef(GL_CLIP_PLANE0 + (plane + 0), (float *)e1);
    GLClipPlanef(GL_CLIP_PLANE0 + (plane + 1), (float *)e2);
#endif
}

void CstrDraw::outputVRAM(uw *raw, sh X, sh Y, sh W, sh H) {
    // Disable state
    opaqueClipState(false);
    
    GLMatrixMode(GL_TEXTURE);
    GLPushMatrix();
    GLID();
    GLScalef(1.0 / FRAME_W, 1.0 / FRAME_H, 1.0);
    
    GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, COLOR_MAX);
    
    GLEnable(GL_TEXTURE_2D);
    GLBindTexture  (GL_TEXTURE_2D, fb16tex);
    GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGBA, GL_UNSIGNED_BYTE, raw);
    
#ifdef APPLE_MACOS
    GLStart(GL_TRIANGLE_STRIP);
        GLTexCoord2s(0, 0); GLVertex2s(X,   Y);
        GLTexCoord2s(W, 0); GLVertex2s(X+W, Y);
        GLTexCoord2s(0, H); GLVertex2s(X,   Y+H);
        GLTexCoord2s(W, H); GLVertex2s(X+W, Y+H);
    GLEnd();
#elif  APPLE_IOS
    // TODO
#endif
    
    GLDisable(GL_TEXTURE_2D);
    GLPopMatrix();
    
    // Enable state
    opaqueClipState(true);
}

template <class T>
void parse(T *components, uw *packets, int points, int step) {
    for (uw i = 0, *p = packets; i < points; i++, p += step) {
        components[i] = *(T *)&p;
    }
}

void CstrDraw::primitive(uw addr, uw *packets) {
    switch((addr >> 5) & 7) {
        case GPU_TYPE_CMD:
            switch(addr) {
                case 0x01: // Reset
                    vs.write(0x1f801814, 0x01000000);
                    return;
                    
                case 0x02: // Rect
                    {
                        // Basic packet components
                        Chromatic *hue[1];
                        Coords    *vx [1];
                        Coords    *sz [1];
                        
                        parse(hue, &packets[0], 1, 0);
                        parse( vx, &packets[1], 1, 0);
                        parse( sz, &packets[2], 1, 0);
                        
                        opaqueClipState(false);
                        GLColor4ub(hue[0]->r, hue[0]->c, hue[0]->b, COLOR_MAX);
                        
#ifdef APPLE_MACOS
                        GLRecti(vx[0]->w,
                                vx[0]->h,
                                vx[0]->w + sz[0]->w,
                                vx[0]->h + sz[0]->h);
#elif  APPLE_IOS
                        // TODO
#endif
                        
                        opaqueClipState(true);
                    }
                    return;
                    
                default:
                    printx("/// PSeudo GPU_TYPE_CMD unknown: 0x%x", addr);
                    return;
            }
            return;
            
        case GPU_TYPE_POLYGON:
            {
                POLY *setup = (POLY *)&addr;
                
                // Options
                int step   = setup->texture  ? 2 : 1; // The offset to fetch specific data from packets
                int points = setup->vertices ? 4 : 3;
                
                // Basic packet components
                Chromatic *hue[points];
                Coords    *vx [points];
                Texture   *tex[points];
                
                if (setup->shade) {
                    // Gouraud
                    parse(hue, &packets[0], points, step + 1);
                    parse( vx, &packets[1], points, step + 1);
                    parse(tex, &packets[2], points, 3);
                }
                else {
                    // Flat
                    parse(hue, &packets[0], points, 0);
                    parse( vx, &packets[1], points, step);
                    parse(tex, &packets[2], points, step);
                }
                
                if (setup->texture) {
                    GLEnable(GL_TEXTURE_2D);
                    cache.fetchTexture(tex[1]->tp, tex[0]->tp);
                    
                    opaque = (tex[1]->tp >> 5) & 3;
                }
                
                const ub b = opaqueFunc(setup->transparent);
                
#ifdef APPLE_MACOS
                GLStart(GL_TRIANGLE_STRIP);
                for (int i = 0; i < points; i++) {
                    if (setup->texture && setup->exposure) {
                        hue[i]->r = COLOR_HALF;
                        hue[i]->c = COLOR_HALF;
                        hue[i]->b = COLOR_HALF;
                    }
                    
                    // Cast offset
                    vx[i]->w += offset.h;
                    vx[i]->h += offset.v;
                    
                    GLColor4ub  (hue[i]->r, hue[i]->c, hue[i]->b, b);
                    GLTexCoord2s(tex[i]->u, tex[i]->v);
                    GLVertex2s  (vx [i]->w, vx [i]->h);
                }
                GLEnd();
#elif  APPLE_IOS
                // TODO
#endif
                GLDisable(GL_TEXTURE_2D);
            }
            return;
            
        case GPU_TYPE_LINE:
            {
                LINE *setup = (LINE *)&addr;
                
                // Options
                int points = setup->multiline ? 256 : 2; // eurasia-001, fuzzion, mups-016, paradox-030, paradox-074, pop-n-pop
                
                // Basic packet components
                Chromatic *hue[points];
                Coords    *vx [points];
                
                if (setup->shade) {
                    // Gouraud
                    parse(hue, &packets[0], points, 2);
                    parse( vx, &packets[1], points, 2);
                }
                else {
                    // Flat
                    parse(hue, &packets[0], points, 0);
                    parse( vx, &packets[1], points, 1);
                }
                
                const ub b = opaqueFunc(setup->transparent);
                
#ifdef APPLE_MACOS
                GLStart(GL_LINE_STRIP);
                for (int i = 0; i < points; i++) {
                    if (setup->multiline) { // Special case
                        if (*(uw *)vx[i] == LINE_TERM_CODE || *(uw *)hue[i] == LINE_TERM_CODE) {
                            break;
                        }
                    }
                    
                    // Cast offset
                    vx[i]->w += offset.h;
                    vx[i]->h += offset.v;
                    
                    GLColor4ub(hue[i]->r, hue[i]->c, hue[i]->b, b);
                    GLVertex2s(vx [i]->w, vx [i]->h);
                }
                GLEnd();
#elif  APPLE_IOS
                // TODO
#endif
            }
            return;
            
        case GPU_TYPE_SPRITE:
            {
                SPRT *setup = (SPRT *)&addr;
                
                // Basic packet components
                Chromatic *hue[1];
                Coords    *vx [1];
                Texture   *tex[1];
                Coords    *sz [1];
                
                parse(hue, &packets[0], 1, 0);
                parse( vx, &packets[1], 1, 0);
                parse(tex, &packets[2], 1, 0);
                parse( sz, &packets[setup->texture ? 3 : 2], 1, 0);
                
                // Square size
                int size = spriteSize[setup->size];
                
                if (size) {
                    sz[0]->w = size;
                    sz[0]->h = size;
                }
                
                if (setup->texture) {
                    GLEnable(GL_TEXTURE_2D);
                    cache.fetchTexture(spriteTP, tex[0]->tp);
                    
                    if (setup->exposure) { // This is not in specs?
                        hue[0]->r = COLOR_HALF;
                        hue[0]->c = COLOR_HALF;
                        hue[0]->b = COLOR_HALF;
                    }
                }
                
                const ub b = opaqueFunc(setup->transparent);
                GLColor4ub(hue[0]->r, hue[0]->c, hue[0]->b, b);
                
#ifdef APPLE_MACOS
                GLStart(GL_TRIANGLE_STRIP);
                    // Cast offset
                    vx[0]->w += offset.h;
                    vx[0]->h += offset.v;
                    
                    GLTexCoord2s(tex[0]->u,            tex[0]->v);
                    GLVertex2s  (vx [0]->w,            vx [0]->h);
                    GLTexCoord2s(tex[0]->u + sz[0]->w, tex[0]->v);
                    GLVertex2s  (vx [0]->w + sz[0]->w, vx [0]->h);
                    GLTexCoord2s(tex[0]->u,            tex[0]->v + sz[0]->h);
                    GLVertex2s  (vx [0]->w,            vx [0]->h + sz[0]->h);
                    GLTexCoord2s(tex[0]->u + sz[0]->w, tex[0]->v + sz[0]->h);
                    GLVertex2s  (vx [0]->w + sz[0]->w, vx [0]->h + sz[0]->h);
                GLEnd();
#elif  APPLE_IOS
                // TODO
#endif
                GLDisable(GL_TEXTURE_2D);
            }
            return;
            
        case GPU_TYPE_IMG_MOVE:
            vs.photoMove(packets);
            return;
            
        case GPU_TYPE_IMG_SEND:
            vs.photoRead(packets);
            return;
            
        case GPU_TYPE_IMG_COPY:
            return;
            
        case GPU_TYPE_ENV:
            switch(addr) {
                case 0xe1: // Texture P.
                    spriteTP = (packets[0]) & 0xffffff;
                    opaque   = (packets[0] >> 5) & 3;
                    GLBlendFunc(bit[opaque].src, bit[opaque].dst);
                    return;
                    
                case 0xe2: // Texture Window
                    //TWX = (((packets[0] >> 10) & 0x1f) << 3);
                    //TWY = (((packets[0] >> 15) & 0x1f) << 3);
                    //TWW = 255 - (((packets[0] >> 0) & 0x1f) << 3);
                    //TWH = 255 - (((packets[0] >> 5) & 0x1f) << 3);
                    return;
                    
                case 0xe3: // Draw Area Start
                    setDrawArea(0, packets[0]);
                    return;
                    
                case 0xe4: // Draw Area End
                    setDrawArea(2, packets[0]);
                    return;
                    
                case 0xe5: // Draw Offset
                    offset.h = ((sw)packets[0] << 21) >> 21;
                    offset.v = ((sw)packets[0] << 10) >> 21;
                    return;
                    
                case 0xe6: // TODO: STP
                    return;
                    
                default:
                    printx("/// PSeudo GPU_TYPE_ENV unknown: 0x%x", addr);
                    return;
            }
            return;
    }
    
    printx("/// PSeudo GPU command unknown: 0x%08x / %d", packets[0], ((addr >> 5) & 7));
}
