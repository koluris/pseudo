class CstrCache {
    enum {
        // Texture modes
        TEX_04BIT,
        TEX_08BIT,
        TEX_15BIT,
        
        // Maximum texture cache
        TCACHE_MAX = 2048
    };
    
    struct {
        struct { // Mem position of texture and color lookup table
            uw w, h, cc;
        } pos;
        
        // Texture and color lookup table buffer
        uw bfr[256][256], cc[256];
    } tex;
    
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
