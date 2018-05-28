#import "Global.h"


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

void CstrCache::pixel2texel(TEXEL *t, uh *p, sw n) {
    do {
        sw c = *p++;
        t->k = (c>>10)<<3;
        t->m = (c>>5)<<3;
        t->c = c<<3;
        if (c) t->a = 255;
        else t->a = 0;
        t++;
    } while(--n);
}

void CstrCache::fetchTexture(uw tp, uw clut) {
    GLuint uid = (clut << 16) | tp;
    
    for (int i = 0; i < TCACHE_MAX; i++) {
        if (cache[i].uid == uid) {
            GLBindTexture(GL_TEXTURE_2D, cache[i].tex);
            return;
        }
    }
    
    uh *tex  = vs.vram.ptr + (tp & 15) * 64 + (tp & 16) * (FRAME_W * 256/16);
    uh *ctbl = vs.vram.ptr + (clut & 0x7fff) * 16;
    
    TEXEL bTexture[TEX_SIZE*TEX_SIZE], *t = bTexture;
    TEXEL ctbl2[TEX_SIZE];
    
    switch((tp >> 7) & 3) {
        case 0: // 04-bit
            pixel2texel(ctbl2, ctbl, 16);
            for (int v = 0; v < 256; v++) {
                for (int h = 0; h < 256; h += 4) {
                    int c = *tex++;
                    t[0] = ctbl2[(c >> 0x0) & 15];
                    t[1] = ctbl2[(c >> 0x4) & 15];
                    t[2] = ctbl2[(c >> 0x8) & 15];
                    t[3] = ctbl2[(c >> 0xc) & 15];
                    t += 4;
                }
                tex += FRAME_W - 256 / 4;
            }
            break;
            
        case 1: // 08-bit
            pixel2texel(ctbl2, ctbl, 256);
            for (int v = 0; v < 256; v++) {
                for (int h = 0; h < 256; h += 2) {
                    int c = *tex++;
                    t[0] = ctbl2[(c >> 0x0) & 255];
                    t[1] = ctbl2[(c >> 0x8) & 255];
                    t += 2;
                }
                tex += FRAME_W - 256 / 2;
            }
            break;
            
        case 2: // 15-bit direct
            for (int v = 0; v < 256; v++) {
                pixel2texel(t, tex, 256);
                tex += FRAME_W;
                t += 256;
            }
            break;
            
        default:
            printx("/// PSeudo Texture TP unknown: %d", ((tp >> 7) & 3));
            break;
    }
    
    GLBindTexture(GL_TEXTURE_2D, cache[index].tex);
    GLTexPhoto2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, bTexture);
    
    cache[index].uid = uid;
    index = (index+1)&(TCACHE_MAX-1);
}
