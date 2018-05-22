class CstrDraw {
    // Basic
    TYPEDEF struct { ub c, m, k, n; } COLOR;
    TYPEDEF struct { sh w, h; } POINTF;
    TYPEDEF struct { sh w, h; ub u, v; uh clut; } POINTFT;
    TYPEDEF struct { COLOR co; sh w, h; } POINTG;
    TYPEDEF struct { COLOR co; sh w, h; ub u, v; uh clut; } POINTGT;
    
    // VertexF & LineF
    TYPEDEF struct { COLOR co; POINTF v[2]; } F2;
    TYPEDEF struct { COLOR co; POINTF v[3]; } F3;
    TYPEDEF struct { COLOR co; POINTF v[4]; } F4;
    
    // VertexFT
    TYPEDEF struct { COLOR co; POINTFT v[3]; } FT3;
    TYPEDEF struct { COLOR co; POINTFT v[4]; } FT4;
    
    // VertexG & LineG
    TYPEDEF struct { POINTG v[2]; } G2;
    TYPEDEF struct { POINTG v[3]; } G3;
    TYPEDEF struct { POINTG v[4]; } G4;
    
    // VertexGT
    TYPEDEF struct { POINTGT v[3]; } GT3;
    TYPEDEF struct { POINTGT v[4]; } GT4;
    
    // BlockFill & Sprites
    TYPEDEF struct { COLOR co; POINTF  v[1]; sh w, h; } BLK;
    TYPEDEF struct { COLOR co; POINTFT v[1]; sh w, h; } SPRT;
    
    uw blend;
    
    // Setup blend values
    const struct {
        sw src, dst; ub opaque;
    } bit[4] = {
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128 },
        { GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0 },
        { GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0 },
        { GL_SRC_ALPHA, GL_ONE,                  64 },
    };
    
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
