#import "Global.h"


#define COLOR_32BIT(a, b, c, r)\
    ( (((ub)(a))<<24) | (((ub)(b))<<16) | (((ub)(c))<<8) | ((ub)(r)) )


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
        GLTexPhoto2D   (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    
    index = 0;
}

uw CstrCache::pixel2texel(uh p) {
    return COLOR_32BIT(p ? 255 : 0, (p >> 10) << 3, (p >> 5) << 3, p << 3);
}

void CstrCache::fetchTexture(uw tp, uw clut) {
    GLuint uid = (clut << 16) | tp;
    
    for (int i = 0; i < TCACHE_MAX; i++) {
        if (cache[i].uid == uid) {
            GLBindTexture(GL_TEXTURE_2D, cache[i].tex);
            return;
        }
    }
    
    uw tex[256][256], cc[256];
    memset(&tex, 0, sizeof(tex));
    memset(& cc, 0, sizeof(cc));
    
    struct {
        struct {
            uh w, h;
        } tex;
        
        uw cc;
    } pos;
    
    pos.tex.w = (tp & 15) * 64;
    pos.tex.h = ((tp >> 4) & 1) * 256;
    pos.cc    = (clut & 0x7fff) * 16;
    
    //printf("0x%x | 0x%x 0x%x\n", ((tp & 15) * 64 + (tp & 16) * 16 * FRAME_W), pos.tex.w, pos.tex.h);
    
    switch((tp >> 7) & 3) {
        case TCACHE_04BIT:
            for (int i = 0; i < 16; i++) { // Color lookup table
                cc[i] = pixel2texel(vs.vram.ptr[pos.cc]);
                pos.cc++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w+=4) {
                    const uh p = vs.vram.ptr[((pos.tex.h + h) * 4096 + pos.tex.w * 4 + w) / 4];
                    tex[h][w + 0] = cc[(p >> 0x0) & 15];
                    tex[h][w + 1] = cc[(p >> 0x4) & 15];
                    tex[h][w + 2] = cc[(p >> 0x8) & 15];
                    tex[h][w + 3] = cc[(p >> 0xc) & 15];
                }
            }
            break;
            
        case TCACHE_08BIT:
            for (int i = 0; i < 256; i++) { // Color lookup table
                cc[i] = pixel2texel(vs.vram.ptr[pos.cc]);
                pos.cc++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w+=2) {
                    const uh p = vs.vram.ptr[((pos.tex.h + h) * 2048 + pos.tex.w * 2 + w) / 2];
                    tex[h][w + 0] = cc[(p >> 0) & 255];
                    tex[h][w + 1] = cc[(p >> 8) & 255];
                }
            }
            break;
            
        case TCACHE_15BIT:
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w++) {
                    tex[h][w] = pixel2texel(vs.vram.ptr[(pos.tex.h + h) * FRAME_W + pos.tex.w + w]);
                }
            }
            break;
    }
    
    GLBindTexture(GL_TEXTURE_2D, cache[index].tex);
    GLTexPhoto2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
    
    cache[index].uid = uid;
    index = (index+1)&(TCACHE_MAX-1);
}
