class CstrDraw {
    enum {
        COLOR_MAX  = 255,
        COLOR_HALF = 128,
        
        LINE_TERM_CODE = 0x55555555
    };
    
    // Basic packet components
    enum {
        GPU_TYPE_CMD,
        GPU_TYPE_POLYGON,
        GPU_TYPE_LINE,
        GPU_TYPE_SPRITE,
        GPU_TYPE_IMG_MOVE,
        GPU_TYPE_IMG_SEND,
        GPU_TYPE_IMG_COPY,
        GPU_TYPE_ENV
    };
    
    struct POLY {
        ub exposure    : 1;
        ub transparent : 1;
        ub texture     : 1;
        ub vertices    : 1;
        ub shade       : 1;
        ub             : 3;
    };
    
    struct LINE {
        ub             : 1;
        ub transparent : 1;
        ub             : 1;
        ub multiline   : 1;
        ub shade       : 1;
        ub             : 3;
    };
    
    struct SPRT {
        ub exposure    : 1;
        ub transparent : 1;
        ub texture     : 1;
        ub size        : 2;
        ub             : 3;
    };
    
    struct Chromatic {
        ub r, c, b, a;
    };
    
    struct Coords {
        sh w, h;
    };
    
    struct Texture {
        ub u, v; uh tp;
    };
    
    const ub spriteSize[4] = {
        0, 1, 8, 16
    };
    
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
    
    GLuint fb24tex = 0;
    GLuint fb16tex = 0;
    ub opaque;
    uw spriteTP;
    
    void reset();
    void keepAspectRatio(sh, sh, int);
    void opaqueClipState(bool);
    ub opaqueFunc(ub);
    void setDrawArea(int, uw);
    
public:
    void init(sh, sh, int);
    void swapBuffers();
    void resize(sh, sh);
    void outputVRAM(uw *, sh, sh, sh, sh);
    void primitive(uw, uw *);
};

extern CstrDraw draw;
