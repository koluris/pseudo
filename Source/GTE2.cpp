#import "Global.h"


#define oooo(base, index) \
    base[(index)]

#define __oo(base, index, offset) \
    base[(index << 1) + offset]

// Rotation matrix
//
// [ 0] R12 (1.3.12) | R11 (1.3.12)
// [ 1] R21 (1.3.12) | R13 (1.3.12)
// [ 2] R23 (1.3.12) | R22 (1.3.12)
// [ 3] R32 (1.3.12) | R31 (1.3.12)
// [ 4]              | R33 (1.3.12)

#define R11 __oo(cop2c.ish, 0, 0)
#define R12 __oo(cop2c.ish, 0, 1)
#define R13 __oo(cop2c.ish, 1, 0)
#define R21 __oo(cop2c.ish, 1, 1)
#define R22 __oo(cop2c.ish, 2, 0)
#define R23 __oo(cop2c.ish, 2, 1)
#define R31 __oo(cop2c.ish, 3, 0)
#define R32 __oo(cop2c.ish, 3, 1)
#define R33 __oo(cop2c.ish, 4, 0)

// Translation vector
//
// [ 5] TRX (1.31.0)
// [ 6] TRY (1.31.0)
// [ 7] TRZ (1.31.0)

#define TRX oooo(cop2c.isw, 5, 0)
#define TRY oooo(cop2c.isw, 6, 0)
#define TRZ oooo(cop2c.isw, 7, 0)

// Lite source direction
//
// [ 8] L12 (1.3.12) | L11 (1.3.12)
// [ 9] L21 (1.3.12) | L13 (1.3.12)
// [10] L23 (1.3.12) | L22 (1.3.12)
// [11] L32 (1.3.12) | L31 (1.3.12)
// [12]              | L33 (1.3.12)

#define L11 __oo(cop2c.ish,  8, 0)
#define L12 __oo(cop2c.ish,  8, 1)
#define L13 __oo(cop2c.ish,  9, 0)
#define L21 __oo(cop2c.ish,  9, 1)
#define L22 __oo(cop2c.ish, 10, 0)
#define L23 __oo(cop2c.ish, 10, 1)
#define L31 __oo(cop2c.ish, 11, 0)
#define L32 __oo(cop2c.ish, 11, 1)
#define L33 __oo(cop2c.ish, 12, 0)


void CstrMips::executeCop2(uw code) {
    switch(code & 63) {
    }
    
    printx("/// PSeudo Unknown Cop2 opcode: %d", (code & 63));
}
