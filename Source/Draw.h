class CstrDraw {
    // Basic
    struct COLOR   { ub r, c, b, a; };
    struct POINTF  { sh w, h; };
    struct POINTFT { sh w, h; ub u, v; uh clut; };
    struct POINTG  { COLOR co; sh w, h; };
    struct POINTGT { COLOR co; sh w, h; ub u, v; uh clut; };
    
    // VertexF & LineF
    struct F2 { COLOR co; POINTF v[2]; };
    struct F3 { COLOR co; POINTF v[3]; };
    struct F4 { COLOR co; POINTF v[4]; };
    
    // VertexFT
    struct FT3 { COLOR co; POINTFT v[3]; };
    struct FT4 { COLOR co; POINTFT v[4]; };
    
    // VertexG & LineG
    struct G2 { POINTG v[2]; };
    struct G3 { POINTG v[3]; };
    struct G4 { POINTG v[4]; };
    
    // VertexGT
    struct GT3 { POINTGT v[3]; };
    struct GT4 { POINTGT v[4]; };
    
    // BlockFill & Sprites
    struct BLK  { COLOR co; POINTF  v[1]; sh w, h; };
    struct SPRT { COLOR co; POINTFT v[1]; sh w, h; };
    
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
