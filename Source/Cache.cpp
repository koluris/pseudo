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
        GLTexPhoto2D   (GL_TEXTURE_2D, 0, GL_RGBA, TEX_SIZE, TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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
    
    uh *tex  = vs.vram.ptr + (tp & 15) * 64 + (tp & 16) * 16 * FRAME_W;
    uh *ctbl = vs.vram.ptr + (clut & 0x7fff) * 16;
    
    TEXEL bTexture[TEX_SIZE*TEX_SIZE], *data = bTexture;
    TEXEL clut2[TEX_SIZE];
    
    switch((tp >> 7) & 3) {
        case 0: // 04-bit
            pixel2texel(clut2, ctbl, TEX_SIZE / 16);
            for (int v = 0; v < TEX_SIZE; v++) {
                for (int h = 0; h < (TEX_SIZE / 4); h++) {
                    *(data++) = clut2[(tex[h]    )&0xf];
                    *(data++) = clut2[(tex[h]>> 4)&0xf];
                    *(data++) = clut2[(tex[h]>> 8)&0xf];
                    *(data++) = clut2[(tex[h]>>12)&0xf];
                }
                tex += FRAME_W;
            }
            break;
            
        case 1: // 08-bit
            pixel2texel(clut2, ctbl, TEX_SIZE);
            for (int v = 0; v < TEX_SIZE; v++) {
                for (int h = 0; h < (TEX_SIZE / 2); h++) {
                    *(data++) = clut2[(tex[h]   )&255];
                    *(data++) = clut2[(tex[h]>>8)&255];
                }
                tex += FRAME_W;
            }
            break;
            
        case 2: // 15-bit direct
            for (int v = 0; v < TEX_SIZE; v++) {
                pixel2texel(data, tex, TEX_SIZE);
                data += TEX_SIZE;
                tex  += FRAME_W;
            }
            break;
    }
    
    GLBindTexture(GL_TEXTURE_2D, cache[index].tex);
    GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, bTexture);
    
    cache[index].uid = uid;
    index = (index+1)&(TCACHE_MAX-1);
}
