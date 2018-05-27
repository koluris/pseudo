class CstrDraw {
    // Basic
    struct RGBC  { ub a, b, c, n; };
    struct PF    { sh w, h; };
    struct PFT   { sh w, h; ub u, v; uh tp; };
    struct PG    { RGBC c; sh w, h; };
    struct PGT   { RGBC c; sh w, h; ub u, v; uh tp; };
    
    // Vertex & Line (F, FT)
    struct PFx   { RGBC c; PF  vx[4]; };
    struct PFTx  { RGBC c; PFT vx[4]; };
    
    // Vertex & Line (G, GT)
    struct PGx   { PG  vx[4]; };
    struct PGTx  { PGT vx[4]; };
    
    // BlockFill & Sprites
    struct TILEx { RGBC c; PF  vx; sh w, h; };
    struct SPRTx { RGBC c; PFT vx; sh w, h; };
    
    // Setup blend values
    const struct {
        sw src, dst; ub opaque;
    } bit[4] = {
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128 },
        { GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0 },
        { GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0 },
        { GL_SRC_ALPHA, GL_ONE,                  64 },
    };
    
    ub blend;
    
    // Primitives
    void blockFill(uw *f);
    
    template <class T>
    void drawF(uw *, ub, GLenum);
    
    template <class T>
    void drawG(uw *, ub, GLenum);
    
    template <class T>
    void drawFT(uw *, ub);
    
    template <class T>
    void drawGT(uw *, ub);
    
    void drawTile(uw *, sh);
    
    void drawSprite(uw *, sh);
    
public:
    void reset();
    void resize(uh, uh);
    void refresh();
    void primitive(uw, uw *);
};

extern CstrDraw draw;
