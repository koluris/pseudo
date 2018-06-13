#import "Global.h"


// Accessors
#define oooo(base, index) \
    base[(index)]

#define __oo(base, index, offset) \
    base[(index << 1) + offset]

#define ___o(base, index, offset) \
    base[(index << 2) + offset]

// Cop2d
// --------------------------------
//  0 | VY0         | VX0         | Vector X, Y, Z (1.15.0) or (1.3.12)
//  1 |             | VZ0         |
//  2 | VY1         | VX1         |
//  3 |             | VZ1         |
//  4 | VY2         | VX2         |
//  5 |             | VZ2         |
// --------------------------------
//  6 | CODE | B    | G    | R    | Color data + GTE instruction (0.8.0)
// --------------------------------
//  7 |             | OTZ         | Z-component value (0.15.0)
// --------------------------------
//  8 | IR0                       | Intermediate value (1.3.12)
//  9 | IR1                       |
// 10 | IR2                       |
// 11 | IR3                       |
// --------------------------------
// 12 | SY0         | SX0         | Calculation result record X, Y, P (1.15.0)
// 13 | SY1         | SX1         |
// 14 | SY2         | SX2         |
// 15 | SYP         | SXP         |
// --------------------------------
// 16 |             | SZ0         | Calculation result record Z (0.16.0)
// 17 |             | SZ1         |
// 18 |             | SZ2         |
// 19 |             | SZ3         |
// --------------------------------
// 20 | CD0  | B0   | G0   | R0   | Color data (0.8.0)
// 21 | CD1  | B1   | G1   | R1   |
// 22 | CD2  | B2   | G2   | R2   |
// --------------------------------
// 23 | RES1                      |
// --------------------------------
// 24 | MAC0                      | Sum of products (1.31.0)
// 25 | MAC1                      |
// 26 | MAC2                      |
// 27 | MAC3                      |
// --------------------------------
// 28 | IRGB                      | Color data input/output (?)
// 29 | ORGB                      |
//---------------------------------
// 30 | LZCS                      | Lead zero/one count source data (1.31.0)
// --------------------------------
// 31 |                    | LZCR | Lead zero/one count process result (0.6.0)
// --------------------------------

#define VX0   __oo(cop2d.ish,  0, 0)
#define VY0   __oo(cop2d.ish,  0, 1)
#define VZ0   __oo(cop2d.ish,  1, 0)
#define VX1   __oo(cop2d.ish,  2, 0)
#define VY1   __oo(cop2d.ish,  2, 1)
#define VZ1   __oo(cop2d.ish,  3, 0)
#define VX2   __oo(cop2d.ish,  4, 0)
#define VY2   __oo(cop2d.ish,  4, 1)
#define VZ2   __oo(cop2d.ish,  5, 0)
#define RGB   oooo(cop2d.iuw,  6)
#define R     ___o(cop2d.iub,  6, 0)
#define G     ___o(cop2d.iub,  6, 1)
#define B     ___o(cop2d.iub,  6, 2)
#define CODE  ___o(cop2d.iub,  6, 3)
#define OTZ   __oo(cop2d.iuh,  7, 0)
#define IR0   __oo(cop2d.ish,  8, 0)
#define IR1   __oo(cop2d.ish,  9, 0)
#define IR2   __oo(cop2d.ish, 10, 0)
#define IR3   __oo(cop2d.ish, 11, 0)
#define SXY0  oooo(cop2d.iuw, 12)
#define SX0   __oo(cop2d.ish, 12, 0)
#define SY0   __oo(cop2d.ish, 12, 1)
#define SXY1  oooo(cop2d.iuw, 13)
#define SX1   __oo(cop2d.ish, 13, 0)
#define SY1   __oo(cop2d.ish, 13, 1)
#define SXY2  oooo(cop2d.iuw, 14)
#define SX2   __oo(cop2d.ish, 14, 0)
#define SY2   __oo(cop2d.ish, 14, 1)
#define SXYP  oooo(cop2d.iuw, 15)
#define SXP   __oo(cop2d.ish, 15, 0)
#define SYP   __oo(cop2d.ish, 15, 1)
#define SZ0   __oo(cop2d.iuh, 16, 0)
#define SZ1   __oo(cop2d.iuh, 17, 0)
#define SZ2   __oo(cop2d.iuh, 18, 0)
#define SZ3   __oo(cop2d.iuh, 19, 0)
#define RGB0  oooo(cop2d.iuw, 20)
#define R0    ___o(cop2d.iub, 20, 0)
#define G0    ___o(cop2d.iub, 20, 1)
#define B0    ___o(cop2d.iub, 20, 2)
#define CD0   ___o(cop2d.iub, 20, 3)
#define RGB1  oooo(cop2d.iuw, 21)
#define R1    ___o(cop2d.iub, 21, 0)
#define G1    ___o(cop2d.iub, 21, 1)
#define B1    ___o(cop2d.iub, 21, 2)
#define CD1   ___o(cop2d.iub, 21, 3)
#define RGB2  oooo(cop2d.iuw, 22)
#define R2    ___o(cop2d.iub, 22, 0)
#define G2    ___o(cop2d.iub, 22, 1)
#define B2    ___o(cop2d.iub, 22, 2)
#define CD2   ___o(cop2d.iub, 22, 3)
#define RES1  oooo(cop2d.iuw, 23)
#define MAC0  oooo(cop2d.isw, 24)
#define MAC1  oooo(cop2d.isw, 25)
#define MAC2  oooo(cop2d.isw, 26)
#define MAC3  oooo(cop2d.isw, 27)
#define IRGB  oooo(cop2d.iuw, 28)
#define ORGB  oooo(cop2d.iuw, 29)
#define LZCS  oooo(cop2d.isw, 30)
#define LZCR  oooo(cop2d.iuw, 31)

#define VX(n) __oo(cop2d.ish, ((n << 1) + 0), 0)
#define VY(n) __oo(cop2d.ish, ((n << 1) + 0), 1)
#define VZ(n) __oo(cop2d.ish, ((n << 1) + 1), 0)

#define RR(n) ___o(cop2d.iub, (n + 20), 0)
#define GG(n) ___o(cop2d.iub, (n + 20), 1)
#define BB(n) ___o(cop2d.iub, (n + 20), 2)
#define CD(n) ___o(cop2d.iub, (n + 20), 3)

//#define SX(n) __oo(cop2d.ish, (n + 12), 0)
//#define SY(n) __oo(cop2d.ish, (n + 12), 1)
//#define SZ(n) __oo(cop2d.iuh, (n + 17), 0)

// Cop2c
// --------------------------------
//  0 | R12         | R11         | Rotation matrix (1.3.12)
//  1 | R21         | R13         |
//  2 | R23         | R22         |
//  3 | R32         | R31         |
//  4 |             | R33         |
// --------------------------------
//  5 | TRX                       | Translation vector X, Y, X (1.31.0)
//  6 | TRY                       |
//  7 | TRZ                       |
// --------------------------------
//  8 | L12         | L11         | Lite source direction (1.3.12)
//  9 | L21         | L13         |
// 10 | L23         | L22         |
// 11 | L32         | L31         |
// 12 |             | L33         |
// --------------------------------
// 13 | RBK                       | Peripheral color R, G, B (1.19.12)
// 14 | GBK                       |
// 15 | BBK                       |
// --------------------------------
// 16 | LR2         | LR1         | Lite source color (1.3.12)
// 17 | LG1         | LR3         |
// 18 | LG3         | LG2         |
// 19 | LB2         | LB1         |
// 20 |             | LB3         |
// --------------------------------
// 21 | RFC                       | Far color R, G, B (1.27.4)
// 22 | BFC                       |
// 23 | GFC                       |
// --------------------------------
// 24 | OFX                       | Screen offset X, Y (1.15.16)
// 25 | OFY                       |
// --------------------------------
// 26 |             | H           | Screen position (0.16.0)
// --------------------------------
// 27 |             | DQA         | Depth parameter A Coefficient (1.7.8)
// --------------------------------
// 28 | DQB                       | Depth parameter B Offset (1.7.24)
// --------------------------------
// 29 |             | ZSF3        | Z scale factor (1.3.12)
// 30 |             | ZSF4        |
// --------------------------------
// 31 | FLAG                      | !
// --------------------------------

#define R11   __oo(cop2c.ish,  0, 0)
#define R12   __oo(cop2c.ish,  0, 1)
#define R13   __oo(cop2c.ish,  1, 0)
#define R21   __oo(cop2c.ish,  1, 1)
#define R22   __oo(cop2c.ish,  2, 0)
#define R23   __oo(cop2c.ish,  2, 1)
#define R31   __oo(cop2c.ish,  3, 0)
#define R32   __oo(cop2c.ish,  3, 1)
#define R33   __oo(cop2c.ish,  4, 0)
#define TRX   oooo(cop2c.isw,  5)
#define TRY   oooo(cop2c.isw,  6)
#define TRZ   oooo(cop2c.isw,  7)
#define L11   __oo(cop2c.ish,  8, 0)
#define L12   __oo(cop2c.ish,  8, 1)
#define L13   __oo(cop2c.ish,  9, 0)
#define L21   __oo(cop2c.ish,  9, 1)
#define L22   __oo(cop2c.ish, 10, 0)
#define L23   __oo(cop2c.ish, 10, 1)
#define L31   __oo(cop2c.ish, 11, 0)
#define L32   __oo(cop2c.ish, 11, 1)
#define L33   __oo(cop2c.ish, 12, 0)
#define RBK   oooo(cop2c.isw, 13)
#define GBK   oooo(cop2c.isw, 14)
#define BBK   oooo(cop2c.isw, 15)
#define LR1   __oo(cop2c.ish, 16, 0)
#define LR2   __oo(cop2c.ish, 16, 1)
#define LR3   __oo(cop2c.ish, 17, 0)
#define LG1   __oo(cop2c.ish, 17, 1)
#define LG2   __oo(cop2c.ish, 18, 0)
#define LG3   __oo(cop2c.ish, 18, 1)
#define LB1   __oo(cop2c.ish, 19, 0)
#define LB2   __oo(cop2c.ish, 19, 1)
#define LB3   __oo(cop2c.ish, 20, 0)
#define RFC   oooo(cop2c.isw, 21)
#define BFC   oooo(cop2c.isw, 22)
#define GFC   oooo(cop2c.isw, 23)
#define OFX   oooo(cop2c.isw, 24)
#define OFY   oooo(cop2c.isw, 25)
#define H     __oo(cop2c.iuh, 26, 0)
#define DQA   __oo(cop2c.ish, 27, 0)
#define DQB   oooo(cop2c.isw, 28)
#define ZSF3  __oo(cop2c.ish, 29, 0)
#define ZSF4  __oo(cop2c.ish, 30, 0)
#define FLAG  oooo(cop2c.iuw, 31)

// Fixed-point arithmetic
#define FPN_24(n) \
    ((n) / 16777216.0)

#define FPN_16(n) \
    ((n) /    65536.0)

#define FPN_12(n) \
    ((n) /     4096.0)

#define FPN_08(n) \
    ((n) /      256.0)

// Limit definition
#define LIM(n, min, max, bit) \
    ( (n) < min ? FLAG |= (1<<bit), min : \
      (n) > max ? FLAG |= (1<<bit), max : (n) )

// Limits
#define LIM_A1S(n) \
    LIM(n, -32768.0, 32767.0, 24)

#define LIM_A2S(n) \
    LIM(n, -32768.0, 32767.0, 23)

#define LIM_A3S(n) \
    LIM(n, -32768.0, 32767.0, 22)

#define LIM_A1U(n) \
    LIM(n,      0.0, 32767.0, 24)

#define LIM_A2U(n) \
    LIM(n,      0.0, 32767.0, 23)

#define LIM_A3U(n) \
    LIM(n,      0.0, 32767.0, 22)

#define LIM_B1(n) \
    LIM(n,      0.0,   255.0, 21)

#define LIM_B2(n) \
    LIM(n,      0.0,   255.0, 20)

#define LIM_B3(n) \
    LIM(n,      0.0,   255.0, 19)

#define LIM_C(  n) \
    LIM(n,      0.0, 65535.0, 18)

#define LIM_D1( n) \
    LIM(n,  -1024.0,  1023.0, 14)

#define LIM_D2( n) \
    LIM(n,  -1024.0,  1023.0, 13)

#define LIM_E(  n) \
    LIM(n,      0.0,  4095.0, 12)

// Division overflow
#define DIV_OVERFLOW(n) \
    ( (n) > 2.0 ? FLAG |= (1<<17), 2.0 : (n) )

// Common
#define MAC2IR0() { \
    IR1 = LIM_A1S(MAC1); \
    IR2 = LIM_A2S(MAC2); \
    IR3 = LIM_A3S(MAC3); \
}

#define MAC2IR1() { \
    IR1 = LIM_A1U(MAC1); \
    IR2 = LIM_A2U(MAC2); \
    IR3 = LIM_A3U(MAC3); \
}


void CstrMips::writeCop2(uw addr) {
    switch(addr) {
        case  8:
        case  9:
        case 10:
        case 11:
            oooo(cop2d.iuw, addr) = __oo(cop2d.ish, addr, 0);
            return;
            
        case 17:
        case 18:
        case 19:
            oooo(cop2d.iuw, addr) = __oo(cop2d.iuh, addr, 0);
            return;
            
        case 30:
            {
                LZCR = 0;
                uw sbit = (LZCS & 0x80000000) ? LZCS : ~LZCS;
                
                for (; sbit & 0x80000000; sbit <<= 1) {
                    LZCR++;
                }
            }
            return;
            
        /* unused */
        case  0:
        case  1:
        case  2:
        case  3:
        case  4:
        case  5:
        case  6:
        case 12:
        case 13:
        case 14:
        case 20:
            return;
    }
    
    printx("/// PSeudo Unknown Cop2 write: %d", addr);
}

void CstrMips::readCop2(uw addr) {
    switch(addr) {
        /* unused */
        case  7:
        case  8:
        case  9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 24:
        case 25:
        case 26:
        case 27:
        case 31:
            return;
    }
    
    printx("/// PSeudo Unknown Cop2 read: %d", addr);
}

void CstrMips::opcodeRTPS(ub vectors) {
    double sx, su, quotient = 0.0;
    
    for (int i = 0; i < vectors; i++) {
        MAC1 = TRX + FPN_12(R11*VX(i) + R12*VY(i) + R13*VZ(i));
        MAC2 = TRY + FPN_12(R21*VX(i) + R22*VY(i) + R23*VZ(i));
        MAC3 = TRZ + FPN_12(R31*VX(i) + R32*VY(i) + R33*VZ(i));
        
        MAC2IR0();
        
        SZ0 = SZ1;
        SZ1 = SZ2;
        SZ2 = SZ3;
        SZ3 = LIM_C(MAC3);
        
        quotient = DIV_OVERFLOW(H / (double)SZ3);
        
        sx = FPN_16(OFX) + IR1 * quotient;
        su = FPN_16(OFY) + IR2 * quotient;
        
        SXY0 = SXY1;
        SXY1 = SXY2;
        SX2  = LIM_D1(sx);
        SY2  = LIM_D2(su);
    }
    
    MAC0 = FPN_24(DQB) + FPN_08(DQA) * quotient;
    IR0  = LIM_E(MAC0);
}

void CstrMips::opcodeNCCS(ub vectors) {
    double L1, L2, L3, RLT, GLT, BLT;
    
    for (int n = 0; n < vectors; n++) {
        L1  = LIM_A1U(FPN_24(L11*VX(n) + L12*VY(n) + L13*VZ(n)));
        L2  = LIM_A2U(FPN_24(L21*VX(n) + L22*VY(n) + L23*VZ(n)));
        L3  = LIM_A3U(FPN_24(L31*VX(n) + L32*VY(n) + L33*VZ(n)));
        
        RLT = FPN_12(RBK + LR1*L1 + LR2*L2 + LR3*L3);
        GLT = FPN_12(GBK + LG1*L1 + LG2*L2 + LG3*L3);
        BLT = FPN_12(BBK + LB1*L1 + LB2*L2 + LB3*L3);
        
        MAC1 = R * LIM_A1U(RLT);
        MAC2 = G * LIM_A2U(GLT);
        MAC3 = B * LIM_A3U(BLT);
        
        RGB0 = RGB1;
        RGB1 = RGB2;
        
        R2  = LIM_B1(MAC1);
        G2  = LIM_B2(MAC2);
        B2  = LIM_B3(MAC3);
        CD2 = CODE;
    }
    
    MAC2IR1();
}

void CstrMips::opcodeNCS(ub vectors) {
    double L1, L2, L3;
    
    for (int n = 0; n < vectors; n++) {
        L1 = LIM_A1U(FPN_12(L11*VX(n) + L12*VY(n) + L13*VZ(n)));
        L2 = LIM_A2U(FPN_12(L21*VX(n) + L22*VY(n) + L23*VZ(n)));
        L3 = LIM_A3U(FPN_12(L31*VX(n) + L32*VY(n) + L33*VZ(n)));
    
        MAC1 = FPN_12(RBK + LR1*L1 + LR2*L2 + LR3*L3);
        MAC2 = FPN_12(GBK + LG1*L1 + LG2*L2 + LG3*L3);
        MAC3 = FPN_12(BBK + LB1*L1 + LB2*L2 + LB3*L3);
    
        RGB0 = RGB1;
        RGB1 = RGB2;
    
        R2  = LIM_B1(MAC1 / 16.0);
        G2  = LIM_B2(MAC2 / 16.0);
        B2  = LIM_B3(MAC3 / 16.0);
        CD2 = CODE;
    }
    
    MAC2IR1();
}

void CstrMips::executeCop2(uw code) {
    // Reset
    FLAG = 0;
    
    switch(code & 63) {
        case 1: // RTPS
            opcodeRTPS(1);
            return;
            
        case 48: // RTPT
            opcodeRTPS(3);
            return;
            
        case 6: // NCLIP
            {
                MAC0 = SX0 * (SY1 - SY2) + SX1 * (SY2 - SY0) + SX2 * (SY0 - SY1);
            }
            return;
            
        case 18: // MVMVA
            {
                switch(code & 0xf8000) {
                    case 0x18000:
                        MAC1 = ((sh)IR1*R11 + (sh)IR2*R12 + (sh)IR3*R13);
                        MAC2 = ((sh)IR1*R21 + (sh)IR2*R22 + (sh)IR3*R23);
                        MAC3 = ((sh)IR1*R31 + (sh)IR2*R32 + (sh)IR3*R33);
                        break;

                    case 0x80000:
                        MAC1 = FPN_12(VX0*R11 + VY0*R12 + VZ0*R13);
                        MAC2 = FPN_12(VX0*R21 + VY0*R22 + VZ0*R23);
                        MAC3 = FPN_12(VX0*R31 + VY0*R32 + VZ0*R33);
                        break;

                    case 0x98000:
                        MAC1 = FPN_12((sh)IR1*R11 + (sh)IR2*R12 + (sh)IR3*R13);
                        MAC2 = FPN_12((sh)IR1*R21 + (sh)IR2*R22 + (sh)IR3*R23);
                        MAC3 = FPN_12((sh)IR1*R31 + (sh)IR2*R32 + (sh)IR3*R33);
                        break;

                    case 0xa0000:
                        MAC1 = FPN_12(VX0*L11 + VY0*L12 + VZ0*L13);
                        MAC2 = FPN_12(VX0*L21 + VY0*L22 + VZ0*L23);
                        MAC3 = FPN_12(VX0*L31 + VY0*L32 + VZ0*L33);
                        break;

                    default:
                        printx("/// PSeudo Unknown cop2 mvmva (code & 0xf8000) $%08x\n", (code & 0xf8000));
                        break;
                }

                switch(code & 0x6000) {
                    case 0x0000:
                        MAC1 += TRX;
                        MAC2 += TRY;
                        MAC3 += TRZ;
                        break;

                    case 0x6000:
                        break;

                    default:
                        printx("/// PSeudo Unknown cop2 mvmva (code & 0x6000) $%08x\n", (code & 0x6000));
                        break;
                }

                if (code & 0x400) {
                    MAC2IR1();
                }
                else {
                    MAC2IR0();
                }
                
//#define C11 LR1
//#define C12 LR2
//#define C13 LR3
//#define C21 LG1
//#define C22 LG2
//#define C23 LG3
//#define C31 LB1
//#define C32 LB2
//#define C33 LB3
//
//#define MVMVA_FUNC(v0, v1, v2, mx) { \
//    SSX = v0 * mx##11 + v1 * mx##12 + v2 * mx##13; \
//    SSY = v0 * mx##21 + v1 * mx##22 + v2 * mx##23; \
//    SSZ = v0 * mx##31 + v1 * mx##32 + v2 * mx##33; \
//}
//
//                double SSX, SSY, SSZ;
//
//                switch(code & 0x78000) {
//                    case 0x00000:
//                        MVMVA_FUNC(VX0, VY0, VZ0, R);
//                        break;
//
//                    case 0x08000:
//                        MVMVA_FUNC(VX1, VY1, VZ1, R);
//                        break;
//
//                    case 0x10000:
//                        MVMVA_FUNC(VX2, VY2, VZ2, R);
//                        break;
//
//                    case 0x18000:
//                        MVMVA_FUNC((sh)IR1, (sh)IR2, (sh)IR3, R);
//                        break;
//
//                    case 0x20000:
//                        MVMVA_FUNC(VX0, VY0, VZ0, L);
//                        break;
//
//                    case 0x28000:
//                        MVMVA_FUNC(VX1, VY1, VZ1, L);
//                        break;
//
//                    case 0x30000:
//                        MVMVA_FUNC(VX2, VY2, VZ2, L);
//                        break;
//
//                    case 0x38000:
//                        MVMVA_FUNC((sh)IR1, (sh)IR2, (sh)IR3, L);
//                        break;
//
//                    case 0x40000:
//                        MVMVA_FUNC(VX0, VY0, VZ0, C);
//                        break;
//
//                    case 0x48000:
//                        MVMVA_FUNC(VX1, VY1, VZ1, C);
//                        break;
//
//                    case 0x50000:
//                        MVMVA_FUNC(VX2, VY2, VZ2, C);
//                        break;
//
//                    case 0x58000:
//                        MVMVA_FUNC((sh)IR1, (sh)IR2, (sh)IR3, C);
//                        break;
//
//                    default:
//                        SSX = SSY = SSZ = 0;
//                        break;
//                }
//
//                if (code & 0x80000) {
//                    MAC1 = FPN_12(SSX);
//                    MAC2 = FPN_12(SSY);
//                    MAC3 = FPN_12(SSZ);
//                }
//
//                switch(code & 0x6000) {
//                    case 0x0000:
//                        MAC1 += TRX;
//                        MAC2 += TRY;
//                        MAC3 += TRZ;
//                        break;
//
//                    case 0x2000:
//                        MAC1 += RBK;
//                        MAC2 += GBK;
//                        MAC3 += BBK;
//                        break;
//
//                    case 0x4000:
//                        MAC1 += RFC;
//                        MAC2 += GFC;
//                        MAC3 += BFC;
//                        break;
//                }
//
//                if (code & 0x400) {
//                    MAC2IR1();
//                }
//                else {
//                    MAC2IR0();
//                }
            }
            return;
            
        case 19: // NCDS
            {
                double L1, L2, L3, RLT, GLT, BLT;
                
                L1  = LIM_A1U(FPN_24(L11*VX0 + L12*VY0 + L13*VZ0));
                L2  = LIM_A1U(FPN_24(L21*VX0 + L22*VY0 + L23*VZ0));
                L3  = LIM_A1U(FPN_24(L31*VX0 + L32*VY0 + L33*VZ0));
                
                RLT = LIM_A1U(FPN_12(RBK + LR1*L1 + LR2*L2 + LR3*L3));
                GLT = LIM_A2U(FPN_12(GBK + LG1*L1 + LG2*L2 + LG3*L3));
                BLT = LIM_A3U(FPN_12(BBK + LB1*L1 + LB2*L2 + LB3*L3));
                
                MAC1 = R*RLT + IR0*LIM_A1S(RFC - R*RLT);
                MAC2 = G*GLT + IR0*LIM_A2S(GFC - G*GLT);
                MAC3 = B*BLT + IR0*LIM_A3S(BFC - B*BLT);
                
                RGB0 = RGB1;
                RGB1 = RGB2;
                
                R2  = LIM_B1(MAC1);
                G2  = LIM_B2(MAC2);
                B2  = LIM_B3(MAC3);
                CD2 = CODE;
                
                MAC2IR1();
            }
            return;
            
        case 27: // NCCS
            opcodeNCCS(1);
            return;
            
        case 63: // NCCT
            opcodeNCCS(3);
            return;
            
        case 30: // NCS
            opcodeNCS(1);
            return;
            
        case 32: // NCT
            opcodeNCS(3);
            return;
            
        case 41: // DPCL
            {
                MAC1 = R*IR1 + IR0*LIM_A1S(RFC - R*IR1);
                MAC2 = G*IR2 + IR0*LIM_A2S(GFC - G*IR2);
                MAC3 = B*IR3 + IR0*LIM_A3S(BFC - B*IR3);
                
                RGB0 = RGB1;
                RGB1 = RGB2;
                
                R2  = LIM_B1(MAC1);
                G2  = LIM_B2(MAC2);
                B2  = LIM_B3(MAC3);
                CD2 = CODE;
                
                MAC2IR0();
            }
            return;
            
        case 42: // DPCT
            {
//                ub vectors = 3;
//                ub Rx, Gx, Bx;
//
//                for (int n = 0; n < vectors; n++) {
////                    if (vectors == 1) {
////                        Rx = R;
////                        Gx = G;
////                        Bx = B;
////                    }
////                    else {
////                        Rx = RR(n);
////                        Gx = GG(n);
////                        Bx = BB(n);
////                    }
//
//                    MAC1 = RR(n)*16.0 + FPN_12(IR0*LIM_A1S(RFC - R*16.0));
//                    MAC2 = GG(n)*16.0 + FPN_12(IR0*LIM_A2S(GFC - G*16.0));
//                    MAC3 = BB(n)*16.0 + FPN_12(IR0*LIM_A3S(BFC - B*16.0));
//
//                    //RGB0 = RGB1;
//                    //RGB1 = RGB2;
//
//                    RR(n) = LIM_B1(MAC1 / 16.0);
//                    GG(n) = LIM_B2(MAC2 / 16.0);
//                    BB(n) = LIM_B3(MAC3 / 16.0);
//                    CD(n) = CODE;
//                }
//
//                MAC2IR0();
            }
            return;
            
        case 45: // AVSZ3
            {
                MAC0 = (SZ1 + SZ2 + SZ3) * FPN_12(ZSF3);
                OTZ  = LIM_C(MAC0);
            }
            return;
            
        case 46: // AVSZ4
            {
                MAC0 = (SZ0 + SZ1 + SZ2 + SZ3) * FPN_12(ZSF4);
                OTZ  = LIM_C(MAC0);
            }
            return;
            
        case 61: // GPF
            {
                if (code & 0x80000) {
                    MAC1 = FPN_12(IR0 * IR1);
                    MAC2 = FPN_12(IR0 * IR2);
                    MAC3 = FPN_12(IR0 * IR3);
                    
                    IR1 = LIM_A1S(MAC1);
                    IR2 = LIM_A1S(MAC2);
                    IR3 = LIM_A1S(MAC3);
                }
                else {
                    printx("GPF %d", 2);
                }
                
                RGB0 = RGB1;
                RGB1 = RGB2;
                
                R2  = LIM_B1(MAC1);
                G2  = LIM_B2(MAC2);
                B2  = LIM_B3(MAC3);
                CD2 = CODE;
            }
            return;
    }
    
    printx("/// PSeudo Unknown Cop2 opcode: %d", (code & 63));
}

//MAC0 =       (DQB / 16777216.0 + DQA / 256.0 * quotient) * 16777216.0;
//IR0  = LIM_E((DQB / 16777216.0 + DQA / 256.0 * quotient) * 4096.0);
