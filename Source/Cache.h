class CstrCache {
    enum {
        // Texture modes
        TEX_04BIT,
        TEX_08BIT,
        TEX_15BIT,
        TEX_15BIT_2,
        
        // Maximum texture cache
        TCACHE_MAX = 384
    };
    
    struct {
        uw w, h, color, abr;
    } info;
    
    struct {
        // Texture and color lookup table buffer
        uw bfr[256][256], cc[256];
    } tex;
    
    uh index;
    
public:
    ~CstrCache() {
        for (auto &tc : cache) {
            GLDeleteTextures(1, &tc.tex);
        }
    }
    
    struct {
        struct { // Mem position of texture and color lookup table
            uw w, h, cc;
        } pos;
        
        uw tp;
        bool update;
        GLuint uid, tex;
    } cache[TCACHE_MAX];
    
    void reset();
    uw pixel2texel(uh);
    void createTexture(GLuint *, int, int);
    void fetchTexture(uw, uw);
    void invalidate(sh, sh, sh, sh);
    void updateTextureState(uw);
};

extern CstrCache tcache;
