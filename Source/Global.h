// Standard C libraries
#import <ctype.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>
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
#define TYPEDEF         typedef

// OpenGL
#define GLBindTexture       glBindTexture
#define GLBlendFunc         glBlendFunc
#define GLClear             glClear
#define GLClearColor        glClearColor
#define GLColor4ub          glColor4ub
#define GLDeleteTextures    glDeleteTextures
#define GLDisable           glDisable
#define GLEnable            glEnable
#define GLEnd               glEnd
#define GLFlush             glFlush
#define GLGenTextures       glGenTextures
#define GLID                glLoadIdentity
#define GLMatrixMode        glMatrixMode
#define GLOrtho             glOrtho
#define GLScalef            glScalef
#define GLStart             glBegin
#define GLTexCoord2s        glTexCoord2s
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
#import "Sound.h"
