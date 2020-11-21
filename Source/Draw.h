#define NORMALIZE_PT(a) \
    (((a) << 21) >> 21)

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
        ub raw_tex    : 1;
        ub semi_trans : 1;
        ub textured   : 1;
        ub four_point : 1;
        ub shaded     : 1;
        ub            : 3;
    };
    
    struct LINE {
        ub            : 1;
        ub semi_trans : 1;
        ub            : 1;
        ub multiline  : 1;
        ub shaded     : 1;
        ub            : 3;
    };
    
    struct SPRT {
        ub raw_tex    : 1;
        ub semi_trans : 1;
        ub textured   : 1;
        ub size       : 2;
        ub            : 3;
    };
    
    struct Color {
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
    
    // Texture Window
    struct {
        sh startX, endX;
        sh startY, endY;
    } texWindow;
    
    // Setup opaque values
    const struct {
        sw src, dst; ub trans;
    } bit[4] = {
        { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 128 },
        { GL_ONE,       GL_ONE_MINUS_SRC_ALPHA,   0 },
        { GL_ZERO,      GL_ONE_MINUS_SRC_COLOR,   0 },
        { GL_SRC_ALPHA, GL_ONE,                  64 },
    };
    
    GLuint fb24tex;
    GLuint fb16tex;
    
    void keepAspectRatio(sh, sh, int);
    void opaqueClipState(bool);
    ub opaqueFunc(ub);
    void setDrawArea(int, uw);
    void updateTextureState(uw);
    
public:
    struct TextureState {
        uw tp, w, h, abr, color;
    } texState;
    
    void init(sh, sh, int);
    void reset();
    void swapBuffers(bool);
    void resize(sh, sh);
    void primitive(uw, uw *);
    void outputVRAM(uw *, sh, sh, sh, sh, bool);
};

extern CstrDraw draw;
