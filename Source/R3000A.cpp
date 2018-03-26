#import "Global.h"


CstrMips cpu;

/*  5-bit */
#define rd\
    ((code >> 11) & 31)

#define rt\
    ((code >> 16) & 31)

#define rs\
    ((code >> 21) & 31)

/*  6-bit */
#define op\
    ((code >> 26) & 63)

/* 16-bit */
#define imm\
    ((sh)code)

#define immu\
    (code & 0xffff)

/* 32-bit */
#define saddr\
    ((code & 0x3ffffff) << 2) | (pc & 0xf0000000)

// J
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 2
//  0 |  0 |  0 |  0 |  1 |  0 |

// ADDIU
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 9
//  0 |  0 |  1 |  0 |  0 |  1 |

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
        step(false);
    }
}

void CstrMips::branch(uw addr) {
    step(true);
    pc = addr;
}

void CstrMips::step(bool inslot) {
    uw code = mem.read32(pc);
    
    printf("$%08x -> $%08x | %d\n", pc, code, op);
    
    pc += 4;
    base[0] = 0;
    
    // No operation counter
    if (code == 0) {
        if (++nopCounter == 30) {
            printx("%d unknown operations, abort.\n", nopCounter);
        };
        return;
    }
    
    switch(op) {
        case 0: // SPECIAL
            switch(code & 63) {
                case 37: // OR
                    base[rd] = base[rs] | base[rt];
                    return;
            }
            printx("Unknown Special opcode -> %d\n", (code & 63));
            return;
            
        case 2: // J
            branch(saddr);
            return;
            
        case 9: // ADDIU
            base[rt] = base[rs] + imm;
            return;
            
        case 13: // ORI
            base[rt] = base[rs] | immu;
            return;
            
        case 15: // LUI
            base[rt] = immu << 16;
            return;
            
        case 43: // SW
            mem.write32(base[rs] + (sh)code, base[rt]);
            return;
    }
    exit(0);
}
