#import "Global.h"


#define oooo(base, index) \
    base[(index)]

#define __oo(base, index, offset) \
    base[(index << 1) + offset]

// ------------------
//  0 | R12  | R11  | Rotation matrix (1.3.12)
//  1 | R21  | R13  |
//  2 | R23  | R22  |
//  3 | R32  | R31  |
//  4 |      | R33  |
// ------------------
//  5 |        TRX  | Translation vector (1.31.0)
//  6 |        TRY  |
//  7 |        TRZ  |
// ------------------
//  8 | L12  | L11  | Lite source direction (1.3.12)
//  9 | L21  | L13  |
// 10 | L23  | L22  |
// 11 | L32  | L31  |
// 12 |      | L33  |
// ------------------
// 13 |        RBK  | Peripheral color (1.19.12)
// 14 |        GBK  |
// 15 |        BBK  |
// ------------------
// 16 | LR2  | LR1  | Lite source color (1.3.12)
// 17 | LG1  | LR3  |
// 18 | LG3  | LG2  |
// 19 | LB2  | LB1  |
// 20 |      | LB3  |
// ------------------
// 21 |        RFC  | Far color (1.27.4)
// 22 |        BFC  |
// 23 |        GFC  |
// ------------------
// 24 |        OFX  | Screen offset (1.15.16)
// 25 |        OFY  |
// ------------------
// 26 |      | H    | Screen position (0.16.0)
// ------------------
// 27 |      | DQA  | Depth parameter A (1.7.8)
// ------------------
// 28 |        DQB  | Depth parameter B (1.7.24)
// ------------------
// 29 |      | ZSF3 | Z scale factor (1.3.12)
// 30 |      | ZSF4 |
// ------------------
// 31 |        FLAG | !
// ------------------

#define R11  __oo(cop2c.ish,  0, 0)
#define R12  __oo(cop2c.ish,  0, 1)
#define R13  __oo(cop2c.ish,  1, 0)
#define R21  __oo(cop2c.ish,  1, 1)
#define R22  __oo(cop2c.ish,  2, 0)
#define R23  __oo(cop2c.ish,  2, 1)
#define R31  __oo(cop2c.ish,  3, 0)
#define R32  __oo(cop2c.ish,  3, 1)
#define R33  __oo(cop2c.ish,  4, 0)
#define TRX  oooo(cop2c.isw,  5, 0)
#define TRY  oooo(cop2c.isw,  6, 0)
#define TRZ  oooo(cop2c.isw,  7, 0)
#define L11  __oo(cop2c.ish,  8, 0)
#define L12  __oo(cop2c.ish,  8, 1)
#define L13  __oo(cop2c.ish,  9, 0)
#define L21  __oo(cop2c.ish,  9, 1)
#define L22  __oo(cop2c.ish, 10, 0)
#define L23  __oo(cop2c.ish, 10, 1)
#define L31  __oo(cop2c.ish, 11, 0)
#define L32  __oo(cop2c.ish, 11, 1)
#define L33  __oo(cop2c.ish, 12, 0)
#define RBK  oooo(cop2c.isw, 13, 0)
#define GBK  oooo(cop2c.isw, 14, 0)
#define BBK  oooo(cop2c.isw, 15, 0)
#define LR1  __oo(cop2c.ish, 16, 0)
#define LR2  __oo(cop2c.ish, 16, 1)
#define LR3  __oo(cop2c.ish, 17, 0)
#define LG1  __oo(cop2c.ish, 17, 1)
#define LG2  __oo(cop2c.ish, 18, 0)
#define LG3  __oo(cop2c.ish, 18, 1)
#define LB1  __oo(cop2c.ish, 19, 0)
#define LB2  __oo(cop2c.ish, 19, 1)
#define LB3  __oo(cop2c.ish, 20, 0)
#define RFC  oooo(cop2c.isw, 21, 0)
#define BFC  oooo(cop2c.isw, 22, 0)
#define GFC  oooo(cop2c.isw, 23, 0)
#define OFX  oooo(cop2c.isw, 24, 0)
#define OFY  oooo(cop2c.isw, 25, 0)
#define H    __oo(cop2c.iuh, 26, 0)
#define DQA  __oo(cop2c.ish, 27, 0)
#define DQB  oooo(cop2c.isw, 28, 0)
#define ZSF3 __oo(cop2c.ish, 29, 0)
#define ZSF4 __oo(cop2c.ish, 30, 0)
#define FLAG oooo(cop2c.iuw, 31, 0)

void CstrMips::executeCop2(uw code) {
    switch(code & 63) {
    }
    
    printx("/// PSeudo Unknown Cop2 opcode: %d", (code & 63));
}
