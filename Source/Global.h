// Standard C libraries
#import <ctype.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import <mach/mach_time.h>
#import <OpenGL/gl.h>

// Apple macOS
#ifdef MAC_OS_X
    #import <OpenAL/al.h>
    #import <OpenAL/alc.h>

    #import "../Platforms/macOS/Global.h"
#else
    #import <AL/al.h>
    #import <AL/alc.h>
#endif

// Data
typedef uint64_t ud; // uns doubleword
typedef uint32_t uw; // uns word
typedef uint16_t uh; // uns halfword
typedef uint8_t  ub; // uns bite

typedef int64_t sd; // doubleword
typedef int32_t sw; // word
typedef int16_t sh; // halfword
typedef int8_t  sb; // bite

// Defines
#define TYPEDEF     typedef
#define memcp       memcpy
#define redirect    goto

// OpenGL
#define GLBindTexture       glBindTexture
#define GLBlendFunc         glBlendFunc
#define GLClear             glClear
#define GLClearColor        glClearColor
#define GLClipPlane         glClipPlane
#define GLColor4ub          glColor4ub
#define GLDeleteTextures    glDeleteTextures
#define GLDisable           glDisable
#define GLEnable            glEnable
#define GLEnd               glEnd
#define GLFlush             glFlush
#define GLGenTextures       glGenTextures
#define GLID                glLoadIdentity
#define GLLineWidth         glLineWidth
#define GLMatrixMode        glMatrixMode
#define GLOrtho             glOrtho
#define GLPopMatrix         glPopMatrix
#define GLPushMatrix        glPushMatrix
#define GLRecti             glRecti
#define GLScalef            glScalef
#define GLStart             glBegin
#define GLTexCoord2s        glTexCoord2s
#define GLTexEnvi           glTexEnvi
#define GLTexParameteri     glTexParameteri
#define GLTexPhoto2D        glTexImage2D
#define GLTexSubPhoto2D     glTexSubImage2D
#define GLVertex2s          glVertex2s
#define GLViewport          glViewport

// OpenAL
#define alSourceStream  alSourcePlay

// User imports
#import "Bus.h"
#import "Cache.h"
#import "Counters.h"
#import "Draw.h"
#import "Graphics.h"
#import "Hardware.h"
#import "Mem.h"
#import "PSeudo.h"
#import "R3000A.h"
#import "SIO.h"
#import "Sound.h"

//union {
//    struct {
//        uw tw         : 4;
//        uw th         : 1;
//        uw abr        : 2;
//        uw tp         : 2;
//        uw dtd        : 1;
//        uw dfe        : 1;
//        uw md         : 1;
//        uw me         : 1;
//        uw reserved   : 1;
//        uw rev        : 1;
//        uw texdisable : 1;
//        uw w          : 3;
//        uw h          : 1;
//        uw video      : 1;
//        uw is24       : 1;
//        uw isinter    : 1;
//        uw den        : 1;
//        uw irq1       : 1;
//        uw dmareq     : 1;
//        uw occupied   : 1;
//        uw photo      : 1;
//        uw com        : 1;
//        uw dmadir     : 2;
//        uw lcf        : 1;
//    };
//
//    uw raw;
//} stat;

//enum {
//    GPU_STAT_TX0,
//    GPU_STAT_TX1,
//    GPU_STAT_TX2,
//    GPU_STAT_TX3,
//    GPU_STAT_TY,
//    GPU_STAT_ABR0,
//    GPU_STAT_ABR1,
//    GPU_STAT_CLUT_8,
//    GPU_STAT_NO_CLUT,
//    GPU_STAT_DITHER,
//    GPU_STAT_DRAW_ENABLED,
//    GPU_STAT_MASK_DRAWN,
//    GPU_STAT_MASK_ENABLED,
//    GPU_STAT_MIRROR_X,
//    GPU_STAT_MIRROR_Y,
//    GPU_STAT_UNK2,
//    GPU_STAT_WIDTH0,
//    GPU_STAT_WIDTH1,
//    GPU_STAT_WIDTH2,
//    GPU_STAT_HEIGHT,
//    GPU_STAT_PAL,
//    GPU_STAT_RGB24,
//    GPU_STAT_INTERLACED,
//    GPU_STAT_DISPLAY_DISABLED,
//    GPU_STAT_UNK3,
//    GPU_STAT_UNK4,
//    GPU_STAT_IDLE,
//    GPU_STAT_READY_FOR_IMG,
//    GPU_STAT_READY_FOR_CMD,
//    GPU_STAT_DMA_DIRECTION,
//    GPU_STAT_DMA_ENABLED,
//    GPU_STAT_ODD_LINE
//};

//if (k->c.n & 2) {
//    GLEnable(GL_ALPHA_TEST);
//    GLAlphaFunc(GL_GEQUAL, 1);
//    GLColor4ub(255, 255, 255, 255);
//    GLDisable(GL_BLEND);
//    GLStart(GL_TRIANGLE_STRIP);
//    GLTexCoord2s(k->vx.u,      k->vx.v);      GLVertex2s(k->vx.w+offset.h,      k->vx.h+offset.v);
//    GLTexCoord2s(k->vx.u+k->w, k->vx.v);      GLVertex2s(k->vx.w+offset.h+k->w, k->vx.h+offset.v);
//    GLTexCoord2s(k->vx.u,      k->vx.v+k->h); GLVertex2s(k->vx.w+offset.h,      k->vx.h+offset.v+k->h);
//    GLTexCoord2s(k->vx.u+k->w, k->vx.v+k->h); GLVertex2s(k->vx.w+offset.h+k->w, k->vx.h+offset.v+k->h);
//    GLEnd();
//    GLEnable(GL_BLEND);
//    GLDisable(GL_ALPHA_TEST);
//}

//void CstrDraw::drawF(uw *packet, int size, GLenum mode) {
//    PFx *p = (PFx *)packet;
//
//    // Special case
//    if (mode == GL_LINE_STRIP) { // eur-001.psx, fuzzion.psx
//        if (size > 2) {
//            size = 256;
//        }
//    }
//
//    ub *b = opaqueFunc(p->hue.a);
//    GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
//
//    GLStart(mode);
//    for (int i = 0; i < size; i++) {
//        if (size == 256) { // Special case
//            if (*(uw *)&p->v[i] == LINE_TERM_CODE) {
//                break;
//            }
//        }
//        GLVertex2s(p->v[i].coords.w, p->v[i].coords.h);
//    }
//    GLEnd();
//}

//void CstrDraw::drawG(uw *packet, int size, GLenum mode) {
//    PGx *p = (PGx *)packet;
//
//    // Special case
//    if (mode == GL_LINE_STRIP) { // mups-016.psx, pdx-030.psx, pdx-074.psx, pop-n-pop.psx
//        if (size > 3) {
//            size = 256;
//        }
//    }
//
//    ub *b = opaqueFunc(p->v[0].hue.a);
//
//    GLStart(mode);
//    for (int i = 0; i < size; i++) {
//        if (size == 256) { // Special case
//            if (*(uw *)&p->v[i] == LINE_TERM_CODE) {
//                break;
//            }
//        }
//        GLColor4ub(p->v[i].   hue.r, p->v[i].   hue.c, p->v[i].hue.b, b[1]);
//        GLVertex2s(p->v[i].coords.w, p->v[i].coords.h);
//    }
//    GLEnd();
//}

//void CstrDraw::drawFT(uw *packet, int size) {
//    PFTx *p = (PFTx *)packet;
//
//    opaque = (p->v[1].tex.clut >> 5) & 3;
//    ub *b = opaqueFunc(p->hue.a);
//
//    if (p->hue.a & 1) {
//        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
//    }
//    else {
//        GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
//    }
//
//    GLEnable(GL_TEXTURE_2D);
//    cache.fetchTexture(p->v[1].tex.clut, p->v[0].tex.clut);
//
//    GLStart(GL_TRIANGLE_STRIP);
//    for (int i = 0; i < size; i++) {
//        GLTexCoord2s(p->v[i].   tex.u, p->v[i].   tex.v);
//        GLVertex2s  (p->v[i].coords.w, p->v[i].coords.h);
//    }
//    GLEnd();
//
//    GLDisable(GL_TEXTURE_2D);
//}

//void CstrDraw::drawGT(uw *packet, int size) {
//    PGTx *p = (PGTx *)packet;
//
//    opaque = (p->v[1].tex.clut >> 5) & 3;
//    ub *b = opaqueFunc(p->v[0].hue.a);
//
//    GLEnable(GL_TEXTURE_2D);
//    cache.fetchTexture(p->v[1].tex.clut, p->v[0].tex.clut);
//
//    GLStart(GL_TRIANGLE_STRIP);
//    for (int i = 0; i < size; i++) {
//        GLColor4ub  (p->v[i].   hue.r, p->v[i].   hue.c, p->v[i].hue.b, b[1]);
//        GLTexCoord2s(p->v[i].   tex.u, p->v[i].   tex.v);
//        GLVertex2s  (p->v[i].coords.w, p->v[i].coords.h);
//    }
//    GLEnd();
//
//    GLDisable(GL_TEXTURE_2D);
//}

//void CstrDraw::drawTile(uw *packet, int size) {
//    TILEx *p = (TILEx *)packet;
//
//    if (size) {
//        p->size.w = size;
//        p->size.h = size;
//    }
//
//    ub *b = opaqueFunc(p->hue.a);
//    GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
//
//    GLStart(GL_TRIANGLE_STRIP);
//        GLVertex2s(p->v.coords.w,             p->v.coords.h);
//        GLVertex2s(p->v.coords.w + p->size.w, p->v.coords.h);
//        GLVertex2s(p->v.coords.w,             p->v.coords.h + p->size.h);
//        GLVertex2s(p->v.coords.w + p->size.w, p->v.coords.h + p->size.h);
//    GLEnd();
//}

//void CstrDraw::drawSprite(uw *packet, int size) {
//    SPRTx *p = (SPRTx *)packet;
//
//    if (size) {
//        p->size.w = size;
//        p->size.h = size;
//    }
//
//    ub *b = opaqueFunc(p->hue.a);
//
//    if (p->hue.a & 1) {
//        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, b[1]);
//    }
//    else {
//        GLColor4ub(p->hue.r, p->hue.c, p->hue.b, b[1]);
//    }
//
//    GLEnable(GL_TEXTURE_2D);
//    cache.fetchTexture(spriteTP, p->v.tex.clut);
//
//    GLStart(GL_TRIANGLE_STRIP);
//        GLTexCoord2s(p->v.   tex.u,             p->v.   tex.v);
//        GLVertex2s  (p->v.coords.w,             p->v.coords.h);
//        GLTexCoord2s(p->v.   tex.u + p->size.w, p->v.   tex.v);
//        GLVertex2s  (p->v.coords.w + p->size.w, p->v.coords.h);
//        GLTexCoord2s(p->v.   tex.u,             p->v.   tex.v + p->size.h);
//        GLVertex2s  (p->v.coords.w,             p->v.coords.h + p->size.h);
//        GLTexCoord2s(p->v.   tex.u + p->size.w, p->v.   tex.v + p->size.h);
//        GLVertex2s  (p->v.coords.w + p->size.w, p->v.coords.h + p->size.h);
//    GLEnd();
//
//    GLDisable(GL_TEXTURE_2D);
//}
