// Standard C libraries
#import <stdlib.h>
#import <stdio.h>
#import <string.h>
#import <limits.h>
#import <mach/mach_time.h>

#ifdef APPLE_MACOS
    #import <OpenGL/gl.h>
    #import <OpenAL/al.h>
    #import <OpenAL/alc.h>

    #import "../Platforms/macOS/Global.h"
#elif  APPLE_IOS
    #import <OpenGLES/ES1/gl.h>
    #import <OpenAL/al.h>
    #import <OpenAL/alc.h>

    #import "../Platforms/iOS/Global.h"
#endif

//#import <AL/al.h>
//#import <AL/alc.h>

// Data
typedef uint64_t ud; // uns doubleword
typedef uint32_t uw; // uns word
typedef uint16_t uh; // uns halfword
typedef uint8_t  ub; // uns bite

typedef int64_t sd; // doubleword
typedef int32_t sw; // word
typedef int16_t sh; // halfword
typedef int8_t  sb; // bite

// Basic
#define memcp               memcpy
#define redirect            goto

// OpenGL 1.1
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

// OpenGLES 1.0
#define GLClipPlanef        glClipPlanef
#define GLOrthof            glOrthof

// OpenAL
#define alSourceStream      alSourcePlay

// User imports
#import "Bus.h"
#import "Cache.h"
#import "Counters.h"
#import "Draw.h"
#import "Dude.h"
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
