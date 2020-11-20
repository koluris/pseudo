/* Base structure taken from FPSE open source emulator, and improved upon (Credits: BERO, LDChen) */

#include "Global.h"


#define COLOR_32BIT(a, b, c, r) \
    (((ub)(a))<<24) | (((ub)(b))<<16) | (((ub)(c))<<8) | ((ub)(r))


CstrCache tcache;

void CstrCache::reset() {
    for (auto &tc : cache) {
        GLDeleteTextures(1, &tc.tex);
        tc = { 0 };
        
        createTexture(&tc.tex, 256, 256);
        
        tc.clut = 1024 * 512 + 1;
        tc.update = false;
    }
    
    textureState = { 0 };
    index = 0;
}

void CstrCache::updateTextureState(uw data) {
    textureState.x = (data << 6) & 0x3c0;
    textureState.y = (data << 4) & 0x100;
    textureState.colormode = (data >> 7) & 0x3;
    
    if (textureState.colormode == 3) {
        textureState.colormode = 2;
    }
}

uw CstrCache::pixel2texel(uh p) {
    return COLOR_32BIT(p ? 255 : 0, (p >> 10) << 3, (p >> 5) << 3, p << 3);
}

void CstrCache::createTexture(GLuint *tex, int w, int h) {
    GLGenTextures(1, tex);
    GLBindTexture  (GL_TEXTURE_2D, *tex);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLTexPhoto2D   (GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void CstrCache::fetchTexture(uw tp, uw clut) {
    //uw uid = (clut << 16) | tp;
    //printf("0x%x 0x%x\n", clutP, uid);
    
//    for (auto &tc : cache) {
//        //if (tc.uid == uid) { // Found cached texture
//            if (tc.x == textureState.x && tc.y == textureState.y && tc.clut == clutP) {
//                if (tc.update == true || tc.tp != textureState.colormode) {
//                    //ctext = i;
//                    break;
//                }
//                GLBindTexture(GL_TEXTURE_2D, tc.tex);
//                return;
//            }
//        //}
//    }
    
    if ((textureState.colormode & 2) == 2) {
        clut = 1024 * 512 + 1;
    }
    
    sw ctext = index;
    for (int i = 0; i < TCACHE_MAX; i++) {
        if (cache[i].pos.w == textureState.x && cache[i].pos.h == textureState.y && cache[i].clut == clut) {
            if (cache[i].update == true || cache[i].tp != textureState.colormode) {
                ctext = i;
                break;
            }
            GLBindTexture(GL_TEXTURE_2D, cache[i].tex);
            return;
        }
    }
    
    // Basic info
    auto &tc  = cache[ctext];
    tc.pos.w  = textureState.x; //(tp & 15) * 64;
    tc.pos.h  = textureState.y; //((tp >> 4) & 1) * 256;
    tc.pos.cc = (clut & 0x7fff) * 16;
    tc.tp = textureState.colormode;
    tc.clut = clut;
    tc.update = false;
    
    if (ctext == index) {
        if ((index + 1) >= TCACHE_MAX) {
            printx("/// PSeudo Texture Cache Full: %d", (index + 1));
        }
        index = (index + 1) & (TCACHE_MAX - 1);
    }
    
    // Reset
    tex = { 0 };
    
    switch(textureState.colormode) {
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
            
        case TEX_15BIT:   // No color palette
        case TEX_15BIT_2: // Seen on some rare cases
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w++) {
                    const uh p = vs.vram.ptr[(tc.pos.h + h) * FRAME_W + tc.pos.w + w];
                    tex.bfr[h][w] = pixel2texel(p);
                }
            }
            break;
            
        default:
            printx("/// PSeudo Texture Cache: %d", ((tp >> 7) & 3));
            break;
    }
    
    // Attach texture
    GLBindTexture  (GL_TEXTURE_2D, tc.tex);
    GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, tex.bfr);
}

void CstrCache::invalidate(sh x, sh y, sh w, sh h) {
    for (auto &tc : cache) {
        if (((tc.pos.w + 255) >= x) && ((tc.pos.h + 255) >= y) && (tc.pos.w <= (x + w)) && (tc.pos.h <= (y + h))) {
            tc.update = true;
        }
    }
}
