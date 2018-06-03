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
        if (c) {
            printf("old: 0x%x\n", c);
            printf("%d %d %d\n", t->c, t->m, t->k);
        }
        if (c) t->a = 255;
        else t->a = 0;
        t++;
    } while(--n);
}

#if 0
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
            //printf("%d\n", ((tp & 15) * 64 + (tp & 16) * (FRAME_W * 256/16)));
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
#else
//transform a 32-bit GL word into a 16-bit VRAM short
//uh tthi(uw cc) {
//    uw r,g,b,a;
//    uh d;
//    r=(cc>>3)&0x1f;
//    g=(cc>>11)&0x1f;
//    b=(cc>>19)&0x1f;
//    a=(cc>>24)&1;
//    d=r+0x20*g+0x400*b+0x8000*a;
//    return(d);
//}

#define tthi(c)     ((((c)&0xf8)>>3)\
    +(((c)&0xf800)>>6)\
    +(((c)&0x1f80000)>>9))

ub vram4(uw vpos) {
    if (vpos > 0x200000) return 0;
    uh c = tthi(*(uh *)&vs.vram.ptr[vpos/4]);
    ub d = (c >> ((vpos & 3) * 4)) & 0x0f;
    return d;
}

ub vram8(uw vpos) {
    if (vpos > 0x100000) return 0;
    uh c = tthi(*(uw *)&vs.vram.ptr[vpos/2]);
    ub d = (c >> ((vpos & 1) * 8)) & 0xff;
    return d;
}

uw mimi(uh c) {
    ub tb = (c >> 0xa) << 3;
    ub tg = (c >> 0x5) << 3;
    ub tr = (c >> 0x0) << 3;
    ub ta = c ? 255 : 0;
    
    return (uw)((ta << 24) | (tb << 16) | (tg << 8)  | (tr << 0));
}

uw txt[65536];

void CstrCache::fetchTexture(uw tp, uw clut) {
    GLuint uid = (clut << 16) | tp;

    for (int i = 0; i < TCACHE_MAX; i++) {
        if (cache[i].uid == uid) {
            GLBindTexture(GL_TEXTURE_2D, cache[i].tex);
            return;
        }
    }

    memset(&txt, 0, sizeof(txt));

    uw cc[256];
    memset(&cc, 0, sizeof(cc));

    uh texx = (tp & 0xf) * 64;
    uh texy = ((tp >> 4) & 1) * 256;
    
//    printf("0x%x 0x%x\n", tp, clut);
//    uh clux = (clut >> 12) & 0x3f0;
//    uh cluy = clut >> 22;
    
    uh clux = clut & 0xff;
    uh cluy = clut >> 8;

    switch((tp >> 7) & 3) {
        case 0: /* 16 colors palette */
            for (int i = 0; i < 16; i++) {
                cc[i] = mimi(vs.vram.ptr[cluy * 1024 + clux + i]);
            }

            for (int j = 0; j < 256; j++) {
                int k = j * 256;

                for (int i = 0; i < 256; i++) {
                    txt[k] = cc[vram4((texy + j) * 4096 + texx * 4 + i)];
                    k++;
                }
            }
            break;

        case 1: /* 256 colors palette */
            for (int i = 0; i < 256; i++) {
                cc[i] = mimi(vs.vram.ptr[cluy * 1024 + clux + i]);
            }

            for (int j = 0; j < 256; j++) {
                int k = j * 256;

                for (int i = 0; i < 256; i++) {
                    txt[k] = cc[vram8((texy + j) * 2048 + texx * 2 + i)];
                    k++;
                }
            }
            break;

        case 2: /* No color palette */
            for (int j = 0; j < 256; j++) {
                int k = j * 256;

                for (int i = 0; i < 256; i++) {
                    txt[k] = mimi(vs.vram.ptr[(texy + j) * 1024 + texx + i]);
                    k++;
                }
            }
            break;
    }

    GLBindTexture(GL_TEXTURE_2D, cache[index].tex);
    GLTexPhoto2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, txt);

    cache[index].uid = uid;
    index = (index+1)&(TCACHE_MAX-1);
}
#endif
