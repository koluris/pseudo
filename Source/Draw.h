class CstrDraw {
    enum {
        COLOR_MAX  = 255,
        COLOR_HALF = COLOR_MAX >> 1,
        
        LINE_TERM_CODE = 0x55555555
    };
    
    // Basic
    struct RGBC  { ub a, b, c, n; };
    struct PF    { sh w, h; };
    struct PFT   { sh w, h; ub u, v; uh clut; };
    struct PG    { RGBC c; sh w, h; };
    struct PGT   { RGBC c; sh w, h; ub u, v; uh clut; };
    
    // Vertex & Line (F, FT)
    struct PFx   { RGBC c; PF  vx[256]; };
    struct PFTx  { RGBC c; PFT vx[4]; };
    
    // Vertex & Line (G, GT)
    struct PGx   { PG  vx[256]; };
    struct PGTx  { PGT vx[4]; };
    
    // Tile & Sprite
    struct TILEx { RGBC c; PF  vx; sh w, h; };
    struct SPRTx { RGBC c; PFT vx; sh w, h; };
    
    // Window
    struct {
        sh h, v; int multiplier;
    } window;
    
    // Offset
    struct {
        sh h, v;
    } res, offset;
    
    // Setup opaque values
    const struct {
        sw src, dst; ub trans;
    } bit[4] = {
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128 },
        { GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0 },
        { GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0 },
        { GL_SRC_ALPHA, GL_ONE,                  64 },
    };
    
    ub opaque;
    uw spriteTP;
    
    // Primitives
    void drawRect  (uw *);
    void drawF     (uw *, int, GLenum);
    void drawG     (uw *, int, GLenum);
    void drawFT    (uw *, int);
    void drawGT    (uw *, int);
    void drawTile  (uw *, int);
    void drawSprite(uw *, int);
    
    ub *opaqueFunc(ub);
    void setDrawArea(int, uw);
    
public:
    void init(sh, sh, int);
    void reset();
    void resize(sh, sh);
    void opaqueClipState(bool);
    void primitive(uw, uw *);
};

extern CstrDraw draw;
