#define TCACHE_MAX\
    2048


class CstrCache {
    enum {
        TCACHE_04BIT,
        TCACHE_08BIT,
        TCACHE_15BIT,
    };
    
    struct {
        GLuint uid, tex;
    } cache[TCACHE_MAX];
    
    uh index;
    uw pixel2texel(uh);
    
public:
    ~CstrCache() {
        for (int i = 0; i < TCACHE_MAX; i++) {
            GLDeleteTextures(1, &cache[i].tex);
        }
    }
    
    void reset();
    void fetchTexture(uw, uw);
};

extern CstrCache cache;
