/*  5-bit */
#define sa \
    ((code >>  6) & 31)

#define rd \
    ((code >> 11) & 31)

#define rt \
    ((code >> 16) & 31)

#define rs \
    ((code >> 21) & 31)


class CstrMips {
    // Co-processor specifics
    enum {
        MFC = 0x00,
        CFC = 0x02,
        MTC = 0x04,
        CTC = 0x06,
        RFE = 0x10,
    };
    
    const uw mask[4][4] = {
        { 0x00ffffff, 0x0000ffff, 0x000000ff, 0x00000000 },
        { 0x00000000, 0xff000000, 0xffff0000, 0xffffff00 },
        { 0xffffff00, 0xffff0000, 0xff000000, 0x00000000 },
        { 0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff },
    };
    
    const ub shift[4][4] = {
        { 0x18, 0x10, 0x08, 0x00 },
        { 0x00, 0x08, 0x10, 0x18 },
        { 0x18, 0x10, 0x08, 0x00 },
        { 0x00, 0x08, 0x10, 0x18 },
    };
    
    union { // Product & quotient (lo, hi)
        sd s64; uw u32[2];
    } res;
    
    uw *instCache, opcodeCount;
    
    void step(bool);
    void branch(uw);
    void exception(uw, bool);
    
    // Co-processor 2 (GTE)
//    union pair {
//        uw iuw[32]; uh iuh[32*2]; ub iub[32*4];
//        sw isw[32]; sh ish[32*2]; sb isb[32*4];
//    } cop2c, cop2d;
    
//    void executeCop2(uw);
//    void   writeCop2(uw);
//    void    readCop2(uw);
//
//    void opcodeRTPS(ub);
//    void opcodeNCCS(ub);
//    void opcodeNCS (ub);
    
    typedef union {
        struct { ub l, h, h2, h3; } iub;
        struct { uh l, h; } iuw;
        struct { sb l, h, h2, h3; } isb;
        struct { sh l, h; } isw;
    } PAIR;
    
    typedef struct {
        short x, y;
    } SVector2D;

    typedef struct {
        short z, pad;
    } SVector2Dz;

    typedef struct {
        short x, y, z, pad;
    } SVector3D;

    typedef struct {
        short x, y, z, pad;
    } LVector3D;

    typedef struct {
        unsigned char r, g, b, c;
    } CBGR;

    typedef struct {
        short m11, m12, m13, m21, m22, m23, m31, m32, m33, pad;
    } SMatrix3D;

    typedef union {
        struct {
            SVector3D     v0, v1, v2;
            CBGR          rgb;
            sw          otz;
            sw          ir0, ir1, ir2, ir3;
            SVector2D     sxy0, sxy1, sxy2, sxyp;
            SVector2Dz    sz0, sz1, sz2, sz3;
            CBGR          rgb0, rgb1, rgb2;
            sw          reserved;
            sw          mac0, mac1, mac2, mac3;
            uw irgb, orgb;
            sw          lzcs, lzcr;
        } n;
        uw r[32];
        PAIR p[32];
    } psxCP2Data;
    
    typedef union {
        struct {
            SMatrix3D rMatrix;
            sw      trX, trY, trZ;
            SMatrix3D lMatrix;
            sw      rbk, gbk, bbk;
            SMatrix3D cMatrix;
            sw      rfc, gfc, bfc;
            sw      ofx, ofy;
            sw      h;
            sw      dqa, dqb;
            sw      zsf3, zsf4;
            sw      flag;
        } n;
        uw r[32];
        PAIR p[32];
    } psxCP2Ctrl;
    
public:
    uw base[32], copr[16], pc;
    psxCP2Data CP2D;
    psxCP2Ctrl CP2C;
    
    void reset();
    void setpc(uw);
    void bootstrap();
    void run();
};

extern CstrMips cpu;
