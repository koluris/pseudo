class CstrDraw {
    enum {
        COLOR_MAX  = 255,
        COLOR_HALF = COLOR_MAX >> 1,
        
        LINE_TERM_CODE = 0x55555555
    };
    
    // Basic packet components
    struct Chromatic {
        ub r, c, b, a;
    };
    
    struct Coords {
        sh w, h;
    };
    
    struct Tex {
        ub u, v; uh clut;
    };
    
    // Primitive Packets
    struct PF    {                Coords coords; };
    struct PFT   {                Coords coords; Tex tex; };
    struct PG    { Chromatic hue; Coords coords; };
    struct PGT   { Chromatic hue; Coords coords; Tex tex; };
    
    // Vertex & Line (F, FT)
    struct PFx   { Chromatic hue; PF  v[256]; }; // Line with termination code
    struct PFTx  { Chromatic hue; PFT v[  4]; };
    
    // Vertex & Line (G, GT)
    struct PGx   { PG  v[256]; }; // Line with termination code
    struct PGTx  { PGT v[  4]; };
    
    // Tile & Sprite
    struct TILEx { Chromatic hue; PF  v; Coords size; };
    struct SPRTx { Chromatic hue; PFT v; Coords size; };
    
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
