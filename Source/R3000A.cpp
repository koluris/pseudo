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
    
    step();
    step();
    step();
    step();
    step();
}

void CstrMips::step() {
    uw code = mem.read32(pc);
    
    printf("PC: 0x%x, Code: 0x%x\n", pc, code);
    printf("%d\n", op);
    
    pc += 4;
    base[0] = 0;
    
    switch(op) {
        case 13: // ORI
            base[rt] = base[rs] | imm;
            return;
            
        case 15: // LUI
            base[rt] = imm >> 16;
            return;
            
        case 43: // SW
            mem.write32(base[rs] + (sh)code, base[rt]);
            return;
    }
    
    exit(0);
}
