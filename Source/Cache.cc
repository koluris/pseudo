#import "Global.h"


#define COLOR_32BIT(a, b, c, r) \
    (((ub)(a))<<24) | (((ub)(b))<<16) | (((ub)(c))<<8) | ((ub)(r))


CstrCache cache;

void CstrCache::reset() {
    for (int i = 0; i < TCACHE_MAX; i++) {
        GLDeleteTextures(1, &cache[i].tex);
        cache[i].uid = 0;
        cache[i].tex = 0;
        
        GLGenTextures(1, &cache[i].tex);
        GLBindTexture  (GL_TEXTURE_2D, cache[i].tex);
        GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    index = 0;
}

uw CstrCache::pixel2texel(uh p) {
    return COLOR_32BIT(p ? 255 : 0, (p >> 10) << 3, (p >> 5) << 3, p << 3);
}

void CstrCache::fetchTexture(uw tp, uw clut) {
    GLuint uid = (clut << 16) | tp; // Generate a unique texture ID
    
    for (int i = 0; i < TCACHE_MAX; i++) {
        if (cache[i].uid == uid) { // Found cached texture
            GLBindTexture(GL_TEXTURE_2D, cache[i].tex);
            return;
        }
    }
    
    // Reset
    memset(&tex, 0, sizeof(tex));
    
    // Basic info
    tex.pos.w  = (tp & 15) * 64;
    tex.pos.h  = ((tp >> 4) & 1) * 256;
    tex.pos.cc = (clut & 0x7fff) * 16;
    
    switch((tp >> 7) & 3) {
        case TEX_04BIT: // 16 color palette
            for (int i = 0; i < 16; i++) {
                tex.cc[i] = pixel2texel(vs.vram.ptr[tex.pos.cc]);
                tex.pos.cc++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < (256 / 4); w++) {
                    const uh p = vs.vram.ptr[(tex.pos.h + h) * FRAME_W + tex.pos.w + w];
                    tex.bfr[h][w*4 + 0] = tex.cc[(p >> 0x0) & 15];
                    tex.bfr[h][w*4 + 1] = tex.cc[(p >> 0x4) & 15];
                    tex.bfr[h][w*4 + 2] = tex.cc[(p >> 0x8) & 15];
                    tex.bfr[h][w*4 + 3] = tex.cc[(p >> 0xc) & 15];
                }
            }
            break;
            
        case TEX_08BIT: // 256 color palette
            for (int i = 0; i < 256; i++) {
                tex.cc[i] = pixel2texel(vs.vram.ptr[tex.pos.cc]);
                tex.pos.cc++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < (256 / 2); w++) {
                    const uh p = vs.vram.ptr[(tex.pos.h + h) * FRAME_W + tex.pos.w + w];
                    tex.bfr[h][w*2 + 0] = tex.cc[(p >> 0) & 255];
                    tex.bfr[h][w*2 + 1] = tex.cc[(p >> 8) & 255];
                }
            }
            break;
            
        case TEX_15BIT: // No color palette
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w++) {
                    const uh p = vs.vram.ptr[(tex.pos.h + h) * FRAME_W + tex.pos.w + w];
                    tex.bfr[h][w] = pixel2texel(p);
                }
            }
            break;
    }
    
    // Attach texture
    GLBindTexture(GL_TEXTURE_2D, cache[index].tex);
    GLTexPhoto2D (GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bfr);
    
    // Advance cache counter
    cache[index].uid = uid;
    index = (index + 1) & (TCACHE_MAX - 1);
}
