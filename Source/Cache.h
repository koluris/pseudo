class CstrCache {
    enum {
        // Texture modes
        TEX_04BIT,
        TEX_08BIT,
        TEX_15BIT,
        
        // Maximum texture cache
        TCACHE_MAX = 64
    };
    
    struct {
        struct { // Mem position of texture and color lookup table
            uw w, h, cc;
        } pos;
        
        GLuint uid, tex;
    } cache[TCACHE_MAX];
    
    struct {
        // Texture and color lookup table buffer
        uw bfr[256][256], cc[256];
    } tex;
    
    uh index;
    uw pixel2texel(uh);
    
public:
    ~CstrCache() {
        for (auto &tc : cache) {
            GLDeleteTextures(1, &tc.tex);
        }
    }
    
    void reset();
    void fetchTexture(uw, uw);
    void invalidate(sh, sh, sh, sh);
};

extern CstrCache cache;
