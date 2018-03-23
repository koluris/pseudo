#import "Global.h"


CstrMips cpu;

#define op\
    ((code >> 26) & 63)

// LUI
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ----------------------------- = 15
//  0 |  0 |  1 |  1 |  1 |  1 |

void CstrMips::reset() {
    memset(base, 0, sizeof(base));
    pc = 0xbfc00000;
    
    step();
}

void CstrMips::step() {
    uw code = mem.read32(pc);
    printf("PC: 0x%08x, Code: 0x%08x\n", pc, code);
    printf("%d\n", op);
    pc += 4;
    base[0] = 0;
    
    switch(op) {
        case 15: // LUI
            return;
    }
    
    exit(0);
}
