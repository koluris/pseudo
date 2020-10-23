/* Base structure taken from FPSE open source emulator, and improved upon (Credits: BERO, LDChen) */

#include "Global.h"


CstrCache cache;

void CstrCache::reset() {
    for (auto &tc : cache) {
        GLDeleteTextures(1, &tc.tex);
        tc = { 0 };
        
        createTexture(&tc.tex, 256, 256);
    }
    
    index = 0;
}

#define COLOR_32BIT(a, b, c, r) \
    (((ub)(a))<<24) | (((ub)(b))<<16) | (((ub)(c))<<8) | ((ub)(r))

uw CstrCache::pixel2texel(uh p) {
    return COLOR_32BIT(p ? 255 : 0, (p >> 10) << 3, (p >> 5) << 3, p << 3);
}

void CstrCache::createTexture(GLuint *tex, int w, int h) {
    GLGenTextures(1, tex);
    GLBindTexture  (GL_TEXTURE_2D, *tex);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLTexPhoto2D   (GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void CstrCache::fetchTexture(uw tp, uw clut) {
    uw uid = (clut << 16) | tp;
    
    for (auto &tc : cache) {
        if (tc.uid == uid) { // Found cached texture
            GLBindTexture(GL_TEXTURE_2D, tc.tex);
            return;
        }
    }
    
    // Basic info
    auto &tc  = cache[index];
    tc.pos.w  = (tp & 15) * 64;
    tc.pos.h  = ((tp >> 4) & 1) * 256;
    tc.pos.cc = (clut & 0x7fff) * 16;
    
    // Reset
    tex = { 0 };
    
    switch((tp >> 7) & 3) {
        case TEX_04BIT: // 16 color palette
            for (int i = 0; i < 16; i++) {
                tex.cc[i] = pixel2texel(vs.vram.ptr[tc.pos.cc]);
                tc.pos.cc++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < (256 / 4); w++) {
                    const uh p = vs.vram.ptr[(tc.pos.h + h) * FRAME_W + tc.pos.w + w];
                    tex.bfr[h][w*4 + 0] = tex.cc[(p >> 0x0) & 15];
                    tex.bfr[h][w*4 + 1] = tex.cc[(p >> 0x4) & 15];
                    tex.bfr[h][w*4 + 2] = tex.cc[(p >> 0x8) & 15];
                    tex.bfr[h][w*4 + 3] = tex.cc[(p >> 0xc) & 15];
                }
            }
            break;
            
        case TEX_08BIT: // 256 color palette
            for (int i = 0; i < 256; i++) {
                tex.cc[i] = pixel2texel(vs.vram.ptr[tc.pos.cc]);
                tc.pos.cc++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < (256 / 2); w++) {
                    const uh p = vs.vram.ptr[(tc.pos.h + h) * FRAME_W + tc.pos.w + w];
                    tex.bfr[h][w*2 + 0] = tex.cc[(p >> 0) & 255];
                    tex.bfr[h][w*2 + 1] = tex.cc[(p >> 8) & 255];
                }
            }
            break;
            
        case TEX_15BIT: // No color palette
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w++) {
                    const uh p = vs.vram.ptr[(tc.pos.h + h) * FRAME_W + tc.pos.w + w];
                    tex.bfr[h][w] = pixel2texel(p);
                }
            }
            break;
            
        default:
            printx("/// PSeudo Texture Cache: Unknown %d", ((tp >> 7) & 3));
            break;
    }
    
    // Attach texture
    GLBindTexture  (GL_TEXTURE_2D, tc.tex);
    GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, tex.bfr);
    
    // Advance cache counter
    tc.uid = uid;
    
    if ((index + 1) >= TCACHE_MAX) {
        printf("Cache cow\n");
    }
    index  = (index + 1) & (TCACHE_MAX - 1);
}

void CstrCache::invalidate(sh X, sh Y, sh W, sh H) {
    for (auto &tc : cache) {
        //if (((tc.pos.w + 255) >= X) && ((tc.pos.h + 255) >= Y) && ((tc.pos.w) <= W) && ((tc.pos.h) <= H)) {
#if 0
            printf("VRAM: %4d %4d %4d %4d\n", X, Y, W, H);
            printf("TEXT: %4d %4d %4d %4d\n", tc.pos.w, tc.pos.h, (tc.pos.w + 255), (tc.pos.h + 255));
            printf("-\n");
#endif
            tc.uid = 0;
            //continue;
        //}
    }
}
