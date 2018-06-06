#import "Global.h"


// 32-bit accessor
#define oooo(base, index) \
    base[(index)]

// 16-bit accessor
#define __oo(base, index, offset) \
    base[(index<<1)+offset]

// 08-bit accessor
#define ___o(base, index, offset) \
    base[(index<<2)+offset]

// Cop2c
#define R11R12 oooo(cop2c.isw,  0)    /* Rotation matrix */
#define R11    __oo(cop2c.ish,  0, 0) /* Rotation matrix */
#define R12    __oo(cop2c.ish,  0, 1) /* Rotation matrix */
#define R13    __oo(cop2c.ish,  1, 0) /* Rotation matrix */
#define R21    __oo(cop2c.ish,  1, 1) /* Rotation matrix */
#define R22R23 oooo(cop2c.isw,  2)    /* Rotation matrix */
#define R22    __oo(cop2c.ish,  2, 0) /* Rotation matrix */
#define R23    __oo(cop2c.ish,  2, 1) /* Rotation matrix */
#define R31    __oo(cop2c.ish,  3, 0) /* Rotation matrix */
#define R32    __oo(cop2c.ish,  3, 1) /* Rotation matrix */
#define R33    __oo(cop2c.ish,  4, 0) /* Rotation matrix */
#define TRX    oooo(cop2c.isw,  5)    /* Translation vector (X) */
#define TRY    oooo(cop2c.isw,  6)    /* Translation vector (Y) */
#define TRZ    oooo(cop2c.isw,  7)    /* Translation vector (Z) */
#define L11    __oo(cop2c.ish,  8, 0) /* Lite source direction vector X 3 */
#define L12    __oo(cop2c.ish,  8, 1) /* Lite source direction vector X 3 */
#define L13    __oo(cop2c.ish,  9, 0) /* Lite source direction vector X 3 */
#define L21    __oo(cop2c.ish,  9, 1) /* Lite source direction vector X 3 */
#define L22    __oo(cop2c.ish, 10, 0) /* Lite source direction vector X 3 */
#define L23    __oo(cop2c.ish, 10, 1) /* Lite source direction vector X 3 */
#define L31    __oo(cop2c.ish, 11, 0) /* Lite source direction vector X 3 */
#define L32    __oo(cop2c.ish, 11, 1) /* Lite source direction vector X 3 */
#define L33    __oo(cop2c.ish, 12, 0) /* Lite source direction vector X 3 */
#define RBK    oooo(cop2c.isw, 13)    /* Peripheral color (R) */
#define GBK    oooo(cop2c.isw, 14)    /* Peripheral color (G) */
#define BBK    oooo(cop2c.isw, 15)    /* Peripheral color (B) */
#define LR1    __oo(cop2c.ish, 16, 0) /* Lite source color X 3 */
#define LR2    __oo(cop2c.ish, 16, 1) /* Lite source color X 3 */
#define LR3    __oo(cop2c.ish, 17, 0) /* Lite source color X 3 */
#define LG1    __oo(cop2c.ish, 17, 1) /* Lite source color X 3 */
#define LG2    __oo(cop2c.ish, 18, 0) /* Lite source color X 3 */
#define LG3    __oo(cop2c.ish, 18, 1) /* Lite source color X 3 */
#define LB1    __oo(cop2c.ish, 19, 0) /* Lite source color X 3 */
#define LB2    __oo(cop2c.ish, 19, 1) /* Lite source color X 3 */
#define LB3    __oo(cop2c.ish, 20, 0) /* Lite source color X 3 */
#define RFC    oooo(cop2c.isw, 21)    /* Far color (R) */
#define GFC    oooo(cop2c.isw, 22)    /* Far color (G) */
#define BFC    oooo(cop2c.isw, 23)    /* Far color (B) */
#define OFX    oooo(cop2c.isw, 24)    /* Screen offset (X) */
#define OFY    oooo(cop2c.isw, 25)    /* Screen offset (Y) */
#define H      __oo(cop2c.ish, 26, 0) /* Screen position */
#define DQA    __oo(cop2c.ish, 27, 0) /* Depth parameter A (coefficient) */
#define DQB    oooo(cop2c.isw, 28)    /* Depth parameter B (offset) */
#define ZSF3   __oo(cop2c.ish, 29, 0) /* Z scale factor */
#define ZSF4   __oo(cop2c.ish, 30, 0) /* Z scale factor */
#define FLAG   oooo(cop2c.iuw, 31)    /* ! */

// Cop2d
#define VXY0   oooo(cop2d.iuw,  0)    /* Vector #0 (X/Y) */
#define VX0    __oo(cop2d.ish,  0, 0) /* Vector #0 (X) */
#define VY0    __oo(cop2d.ish,  0, 1) /* Vector #0 (Y) */
#define VZ0    __oo(cop2d.ish,  1, 0) /* Vector #0 (Z) */
#define VXY1   oooo(cop2d.iuw,  2)    /* Vector #1 (X/Y) */
#define VX1    __oo(cop2d.ish,  2, 0) /* Vector #1 (X) */
#define VY1    __oo(cop2d.ish,  2, 1) /* Vector #1 (Y) */
#define VZ1    __oo(cop2d.ish,  3, 0) /* Vector #1 (Z) */
#define VXY2   oooo(cop2d.iuw,  4)    /* Vector #2 (X/Y) */
#define VX2    __oo(cop2d.ish,  4, 0) /* Vector #2 (X) */
#define VY2    __oo(cop2d.ish,  4, 1) /* Vector #2 (Y) */
#define VZ2    __oo(cop2d.ish,  5, 0) /* Vector #2 (Z) */
#define RGB    oooo(cop2d.iuw,  6)    /* Color data + GTE instruction */
#define R      ___o(cop2d.iub,  6, 0) /* Color (R) */
#define G      ___o(cop2d.iub,  6, 1) /* Color (G) */
#define B      ___o(cop2d.iub,  6, 2) /* Color (B) */
#define CODE   ___o(cop2d.iub,  6, 3) /* GTE instruction */
#define OTZ    __oo(cop2d.iuh,  7, 0) /* Z-component value */
#define IR0    __oo(cop2d.ish,  8, 0) /* Intermediate value #0 */
#define IR1    __oo(cop2d.ish,  9, 0) /* Intermediate value #1 */
#define IR2    __oo(cop2d.ish, 10, 0) /* Intermediate value #2 */
#define IR3    __oo(cop2d.ish, 11, 0) /* Intermediate value #3 */
#define SXY0   oooo(cop2d.iuw, 12)    /* Calc. result record #0 (X/Y) */
#define SX0    __oo(cop2d.ish, 12, 0) /* Calc. result record #0 (X) */
#define SY0    __oo(cop2d.ish, 12, 1) /* Calc. result record #0 (Y) */
#define SXY1   oooo(cop2d.iuw, 13)    /* Calc. result record #1 (X/Y) */
#define SX1    __oo(cop2d.ish, 13, 0) /* Calc. result record #1 (X) */
#define SY1    __oo(cop2d.ish, 13, 1) /* Calc. result record #1 (Y) */
#define SXY2   oooo(cop2d.iuw, 14)    /* Calc. result record #2 (X/Y) */
#define SX2    __oo(cop2d.ish, 14, 0) /* Calc. result record #2 (X) */
#define SY2    __oo(cop2d.ish, 14, 1) /* Calc. result record #2 (Y) */
#define SXYP   oooo(cop2d.iuw, 15)    /* Calc. result set (X/Y) */
#define SXP    __oo(cop2d.ish, 15, 0) /* Calc. result set (X) */
#define SYP    __oo(cop2d.ish, 15, 1) /* Calc. result set (Y) */
#define SZ0    __oo(cop2d.iuh, 16, 0) /* Calc. result record #0 (Z) */
#define SZ1    __oo(cop2d.iuh, 17, 0) /* Calc. result record #1 (Z) */
#define SZ2    __oo(cop2d.iuh, 18, 0) /* Calc. result record #2 (Z) */
#define SZ3    __oo(cop2d.iuh, 19, 0) /* Calc. result record #3 (Z) */
#define RGB0   oooo(cop2d.iuw, 20)    /* Calc. result record #0 (R/G/B) */
#define R0     ___o(cop2d.iub, 20, 0) /* Calc. result record #0 (R) */
#define G0     ___o(cop2d.iub, 20, 1) /* Calc. result record #0 (G) */
#define B0     ___o(cop2d.iub, 20, 2) /* Calc. result record #0 (B) */
#define CODE0  ___o(cop2d.iub, 20, 3) /* Calc. result record #0 (?) */
#define RGB1   oooo(cop2d.iuw, 21)    /* Calc. result record #1 (R/G/B) */
#define R1     ___o(cop2d.iub, 21, 0) /* Calc. result record #1 (R) */
#define G1     ___o(cop2d.iub, 21, 1) /* Calc. result record #1 (G) */
#define B1     ___o(cop2d.iub, 21, 2) /* Calc. result record #1 (B) */
#define CODE1  ___o(cop2d.iub, 21, 3) /* Calc. result record #1 (?) */
#define RGB2   oooo(cop2d.iuw, 22)    /* Calc. result record #2 (R/G/B) */
#define R2     ___o(cop2d.iub, 22, 0) /* Calc. result record #2 (R) */
#define G2     ___o(cop2d.iub, 22, 1) /* Calc. result record #2 (G) */
#define B2     ___o(cop2d.iub, 22, 2) /* Calc. result record #2 (B) */
#define CODE2  ___o(cop2d.iub, 22, 3) /* Calc. result record #2 (?) */
#define RES1   oooo(cop2d.iuw, 23)    /* Reserved (access prohibited) */
#define MAC0   oooo(cop2d.isw, 24)    /* Sum of products #0 */
#define MAC1   oooo(cop2d.isw, 25)    /* Sum of products #1 */
#define MAC2   oooo(cop2d.isw, 26)    /* Sum of products #2 */
#define MAC3   oooo(cop2d.isw, 27)    /* Sum of products #3 */
#define IRGB   oooo(cop2d.iuw, 28)    /* Color data input */
#define ORGB   oooo(cop2d.iuw, 29)    /* Color data output */
#define LZCS   oooo(cop2d.iuw, 30)    /* Lead zero/one count source data */
#define LZCR   oooo(cop2d.iuw, 31)    /* Lead zero/one count process result */

#define VX(n)  __oo(cop2d.ish, ((n<<1)+0), 0)
#define VY(n)  __oo(cop2d.ish, ((n<<1)+0), 1)
#define VZ(n)  __oo(cop2d.ish, ((n<<1)+1), 0)

#define SX(n)  __oo(cop2d.ish, (n+12), 0)
#define SY(n)  __oo(cop2d.ish, (n+12), 1)
#define SZ(n)  __oo(cop2d.iuh, (n+17), 0)

// General
#define FIX(a) \
    ((a) / 4096.0)

#define LIM(a, min, max, bit) \
    (((a) < min) ? (FLAG |= (1<<bit), min) : \
    (((a) > max) ? (FLAG |= (1<<bit), max) : ((a))))

#define LIM_A1S(n) LIM(n, -32768.0, 32767.0, 24)
#define LIM_A2S(n) LIM(n, -32768.0, 32767.0, 23)
#define LIM_A3S(n) LIM(n, -32768.0, 32767.0, 22)
#define LIM_A1U(n) LIM(n,      0.0, 32767.0, 24)
#define LIM_A2U(n) LIM(n,      0.0, 32767.0, 23)
#define LIM_A3U(n) LIM(n,      0.0, 32767.0, 22)
#define LIM_B1( n) LIM(n,      0.0,   255.0, 21)
#define LIM_B2( n) LIM(n,      0.0,   255.0, 20)
#define LIM_B3( n) LIM(n,      0.0,   255.0, 19)
#define LIM_C(  n) LIM(n,      0.0, 65535.0, 18)
#define LIM_D1( n) LIM(n,  -1024.0,  1023.0, 14)
#define LIM_D2( n) LIM(n,  -1024.0,  1023.0, 13)
#define LIM_E(  n) LIM(n,      0.0,  4095.0, 12)

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
            return;
    }
    
    printx("/// PSeudo Unknown Cop2 write: %d", addr);
}

void CstrMips::readCop2(uw addr) {
    switch(addr) {
        /* unused */
        case  7:
        case  9:
        case 10:
        case 11:
        case 19:
        case 25:
        case 26:
        case 27:
        case 31:
            return;
    }
    
    printx("/// PSeudo Unknown Cop2 read: %d", addr);
}

void CstrMips::executeCop2(uw code) {
    switch(code & 63) {
        case 0: // Basic
            printx("/// PSeudo Unknown cop2 basic $%08x | %d", code, (rs & 7));
            return;
            
        case 1: // RTPS
            {
                FLAG = 0;
                
                MAC1 = FIX(R11 * VX0 + R12 * VY0 + R13 * VZ0) + TRX;
                MAC2 = FIX(R21 * VX0 + R22 * VY0 + R23 * VZ0) + TRY;
                MAC3 = FIX(R31 * VX0 + R32 * VY0 + R33 * VZ0) + TRZ;
                
                double quotient = H / LIM_C(MAC3); if (quotient > 2147483647.0) { quotient = 2.0; FLAG |= (1<<17); }
                
                SZ0 = SZ1;
                SZ1 = SZ2;
                SZ2 = SZ3;
                SZ3 = LIM_C(MAC3);
                
                SXY0 = SXY1;
                SXY1 = SXY2;
                
                SX2 = LIM_D1(OFX / 65536.0 + LIM_A1S(MAC1) * quotient);
                SY2 = LIM_D2(OFY / 65536.0 + LIM_A2S(MAC2) * quotient);
                
                MAC2IR0();
                
                MAC0 =       (DQB / 16777216.0 + DQA / 256.0 * quotient) * 16777216.0;
                IR0  = LIM_E((DQB / 16777216.0 + DQA / 256.0 * quotient) * 4096.0);
            }
            return;
    
        case 48: // RTPT
            {
                double quotient = 0;
                
                FLAG = 0;
                SZ0  = SZ3;
                
                for (int v = 0; v < 3; v++) {
                    MAC1 = FIX(R11 * VX(v) + R12 * VY(v) + R13 * VZ(v)) + TRX;
                    MAC2 = FIX(R21 * VX(v) + R22 * VY(v) + R23 * VZ(v)) + TRY;
                    MAC3 = FIX(R31 * VX(v) + R32 * VY(v) + R33 * VZ(v)) + TRZ;
                    
                    quotient = H / LIM_C(MAC3); if (quotient > 2147483647.0) { quotient = 2.0; FLAG |= (1<<17); }
                    
                    SZ(v) = LIM_C(MAC3);
                    
                    SX(v) = LIM_D1(OFX / 65536.0 + LIM_A1S(MAC1) * quotient);
                    SY(v) = LIM_D2(OFY / 65536.0 + LIM_A2S(MAC2) * quotient);
                }
                
                MAC2IR0();

                MAC0 =       (DQB / 16777216.0 + DQA / 256.0 * quotient) * 16777216.0;
                IR0  = LIM_E((DQB / 16777216.0 + DQA / 256.0 * quotient) * 4096.0);
            }
            return;
            
        case 45: // AVSZ3
            FLAG = 0;
            
            MAC0 = (SZ1 + SZ2 + SZ3) * (ZSF3 / 4096.0);
            OTZ  = LIM_C(MAC0);
            return;
            
        case 46: // AVSZ4
            FLAG = 0;
            
            MAC0 = (SZ0 + SZ1 + SZ2 + SZ3) * (ZSF4 / 4096.0);
            OTZ  = LIM_C(MAC0);
            return;
            
        case 6: // NCLIP
            FLAG = 0;
            
            MAC0 = SX0 * (SY1 - SY2) + SX1 * (SY2 - SY0) + SX2 * (SY0 - SY1);
            return;
            
        case 18: // MVMVA
            FLAG = 0;
            
            switch(code & 0xf8000) {
                case 0x18000:
                    MAC1 = ((sh)IR1 * R11 + (sh)IR2 * R12 + (sh)IR3 * R13);
                    MAC2 = ((sh)IR1 * R21 + (sh)IR2 * R22 + (sh)IR3 * R23);
                    MAC3 = ((sh)IR1 * R31 + (sh)IR2 * R32 + (sh)IR3 * R33);
                    break;
                    
                case 0x98000:
                    MAC1 = FIX((sh)IR1 * R11 + (sh)IR2 * R12 + (sh)IR3 * R13);
                    MAC2 = FIX((sh)IR1 * R21 + (sh)IR2 * R22 + (sh)IR3 * R23);
                    MAC3 = FIX((sh)IR1 * R31 + (sh)IR2 * R32 + (sh)IR3 * R33);
                    break;
                    
                case 0x80000:
                    MAC1 = FIX(VX0 * R11 + VY0 * R12 + VZ0 * R13);
                    MAC2 = FIX(VX0 * R21 + VY0 * R22 + VZ0 * R23);
                    MAC3 = FIX(VX0 * R31 + VY0 * R32 + VZ0 * R33);
                    break;
                    
                default:
                    printf("/// PSeudo Unknown cop2 mvmva (code & 0xf8000) $%08x\n", (code & 0xf8000));
                    break;
            }
            
            switch(code & 0x6000) {
                case 0x6000:
                    break;
                    
                default:
                    printf("/// PSeudo Unknown cop2 mvmva (code & 0x6000) $%08x\n", (code & 0x6000));
                    break;
            }
            
            if (code & 0x400) {
                MAC2IR1();
            }
            else {
                MAC2IR0();
            }
            return;
    }
    
    printx("/// PSeudo Unknown cop2 opcode $%08x | %d", code, (code & 63));
}
