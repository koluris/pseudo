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
#import "CD.h"
#import "Counters.h"
#import "Draw.h"
#import "Graphics.h"
#import "Hardware.h"
#import "Mem.h"
#import "PSeudo.h"
#import "R3000A.h"
#import "SIO.h"
#import "Sound.h"
