#import "Global.h"


CstrMips cpu;

/*  5-bit */
#define sa\
    ((code >>  6) & 31)

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
#define ob\
    (base[rs] + imm)

#define baddr\
    (pc + (imm << 2))

#define saddr\
    ((code & 0x3ffffff) << 2) | (pc & 0xf0000000)

// ADD
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 32
//  1 |  0 |  0 |  0 |  0 |  0 |

void CstrMips::reset() {
    memset(base, 0, sizeof(base));
    memset(copr, 0, sizeof(copr));
    
    pc = 0xbfc00000;
    lo = 0;
    hi = 0;
    nopCounter = 0;
    
    while(pc != 0x80030000) {
        step(false);
    }
}

void CstrMips::branch(uw addr) {
    step(true);
    pc = addr;
}

void CstrMips::step(bool inslot) {
    uw code = mem.read32(pc);
    pc += 4;
    base[0] = 0;
    
    // No operation counter
    if (code == 0) {
        if (++nopCounter == 200) {
            //printx("%d unknown operations, abort\n", nopCounter);
        };
        return;
    }
    
    switch(op) {
        case 0: // SPECIAL
            switch(code & 63) {
                case 0: // SLL
                    base[rd] = base[rt] << sa;
                    return;
                    
                case 8: // JR
                    branch(base[rs]); // Remember to print the output
                    return;
                    
                case 32: // ADD
                    base[rd] = base[rs] + base[rt];
                    return;
                    
                case 33: // ADDU
                    base[rd] = base[rs] + base[rt];
                    return;
                    
                case 36: // AND
                    base[rd] = base[rs] & base[rt];
                    return;
                    
                case 37: // OR
                    base[rd] = base[rs] | base[rt];
                    return;
                    
                case 43: // SLTU
                    base[rd] = base[rs] < base[rt];
                    return;
            }
            printx("$%08x | Unknown special opcode $%08x | %d\n", pc, code, (code & 63));
            return;
            
        case 2: // J
            branch(saddr);
            return;
            
        case 3: // JAL
            base[31] = pc + 4;
            branch(saddr);
            return;
            
        case 4: // BEQ
            if (base[rs] == base[rt]) {
                branch(baddr);
            }
            return;
            
        case 5: // BNE
            if (base[rs] != base[rt]) {
                branch(baddr);
            }
            return;
            
        case 8: // ADDI
            base[rt] = base[rs] + imm;
            return;
            
        case 9: // ADDIU
            base[rt] = base[rs] + imm;
            return;
            
        case 12: // ANDI
            base[rt] = base[rs] & immu;
            return;
            
        case 13: // ORI
            base[rt] = base[rs] | immu;
            return;
            
        case 15: // LUI
            base[rt] = immu << 16;
            return;
            
        case 16: // COP0
            switch(rs) {
                case 0: // MFC0
                    base[rt] = copr[rd];
                    return;
                    
                case 4: // MTC0
                    copr[rd] = base[rt];
                    return;
            }
            printx("$%08x | Unknown cop0 opcode $%08x | %d\n", pc, code, rs);
            return;
            
        case 32: // LB
            base[rt] = mem.read08(ob);
            return;
            
        case 35: // LW
            base[rt] = mem.read32(ob);
            return;
            
        case 40: // SB
            mem.write08(ob, base[rt]);
            return;
            
        case 41: // SH
            mem.write16(ob, base[rt]);
            return;
            
        case 43: // SW
            mem.write32(ob, base[rt]);
            return;
    }
    printx("$%08x | Unknown basic opcode $%08x | %d\n", pc, code, op);
}
