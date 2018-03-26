#import "Global.h"


CstrMips cpu;

/*  6-bit */
#define op\
    ((code >> 26) & 63)

/*  5-bit */
#define rt\
    ((code >> 16) & 31)

/*  5-bit */
#define rs\
    ((code >> 21) & 31)

/* 16-bit */
#define imm\
    (code & 0xffff)

// ORI
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 13
//  0 |  0 |  1 |  1 |  0 |  1 |

// LUI
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 15
//  0 |  0 |  1 |  1 |  1 |  1 |

// SW
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 43
//  1 |  0 |  1 |  0 |  1 |  1 |

void CstrMips::reset() {
    memset(base, 0, sizeof(base));
    
    pc = 0xbfc00000;
    lo = 0;
    hi = 0;
    nopCounter = 0;
    
    while(1) {
        step();
    }
}

void CstrMips::step() {
    uw code = mem.read32(pc);
    
    printf("PC: 0x%08x, Code: 0x%08x, OP: 0x%08x\n", pc, code, op);
    
    pc += 4;
    base[0] = 0;
    
    // No operation counter
    if (code == 0) {
        if (++nopCounter == 10) {
            printx("%d unknown operations, abort.\n", nopCounter);
        };
        return;
    }
    
    switch(op) {
        case 13: // ORI
            base[rt] = base[rs] | imm;
            return;
            
        case 15: // LUI
            base[rt] = imm << 16;
            return;
            
        case 43: // SW
            mem.write32(base[rs] + (sh)code, base[rt]);
            return;
    }
}
