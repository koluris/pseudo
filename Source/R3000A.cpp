#import "Global.h"


CstrMips cpu;

// SLLV
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 4
//  0 |  0 |  0 |  1 |  0 |  0 |

void CstrMips::reset() {
    memset(base, 0, sizeof(base));
    memset(copr, 0, sizeof(copr));
    
    copr[12] = 0x10900000;
    copr[15] = 0x2; // Co-processor Revision
    
    pc = 0xbfc00000;
    res.u64 = opcodeCount = 0;
    
    // Bootstrap
    while(pc != 0x80030000) {
        step(false);
    }
}

//uw vbk = 0;

void CstrMips::branch(uw addr) {
    step(true);
    pc = addr;
    
    if (opcodeCount >= PSX_CYCLE) { // TODO: Rootcounters, interrupts
//        if ((vbk += PSX_CYCLE) >= PSX_VSYNC) { vbk = 0;
//            data16 |= 1 << 0;
//            vs.redraw();
//        }
        
        // Exceptions
//        if (data32 & mask32) {
//            if ((copr[12] & 0x401) == 0x401) {
//                exception(0x400, false);
//            }
//        }
        opcodeCount %= PSX_CYCLE;
    }
}

void CstrMips::step(bool branched) {
    uw code = mem.read32(pc); pc += 4;
    base[0] = 0;
    opcodeCount++;
    
    switch(opcode) {
        case 0: // SPECIAL
            switch(code & 63) {
                case 0: // SLL
                    if (code) { // No operation?
                        base[rd] = base[rt] << sa;
                    }
                    return;
                    
                case 2: // SRL
                    base[rd] = base[rt] >> sa;
                    return;
                    
                case 3: // SRA
                    base[rd] = (sw)base[rt] >> sa;
                    return;
                    
                case 4: // SLLV
                    base[rd] = base[rt] << (base[rs] & 31);
                    return;
                    
                case 6: // SRLV
                    base[rd] = base[rt] >> (base[rs] & 31);
                    return;
                    
                case 7: // SRAV
                    base[rd] = (sw)base[rt] >> (base[rs] & 31);
                    return;
                    
                case 8: // JR
                    branch(base[rs]); // TODO: Print the output
                    psx.console(base, pc);
                    return;
                    
                case 9: // JALR
                    base[rd] = pc + 4;
                    branch(base[rs]);
                    return;
                    
                case 12: // SYSCALL
                    pc -= 4;
                    exception(0x20, branched);
                    return;
                    
                case 16: // MFHI
                    base[rd] = res.u32[1];
                    return;
                    
                case 17: // MTHI
                    res.u32[1] = base[rs];
                    return;
                    
                case 18: // MFLO
                    base[rd] = res.u32[0];
                    return;
                    
                case 19: // MTLO
                    res.u32[0] = base[rs];
                    return;
                    
                case 25: // MULTU
                    {
                        // Example: 0xdeadc0decafebabe
                        //
                        // LO <- t[31.. 0] | LO order (LSW) -> 0xcafebabe
                        // HI <- t[63..32] | HI order (MSW) -> 0xdeadc0de
                        
                        res.u64 = base[rs] * base[rt];
                        //sd haha = (sd)base[rs] * base[rt];
                        //res.u32[0] = haha&0xffffffff;
                        //res.u32[1] = haha>>32;
                    }
                    return;
                    
                case 26: // DIV
                    if (base[rt]) {
                        res.u32[0] = (sw)base[rs] / (sw)base[rt];
                        res.u32[1] = (sw)base[rs] % (sw)base[rt];
                    }
                    return;
                    
                case 27: // DIVU
                    if (base[rt]) {
                        res.u32[0] = base[rs] / base[rt];
                        res.u32[1] = base[rs] % base[rt];
                    }
                    return;
                    
                case 32: // ADD
                    base[rd] = base[rs] + base[rt];
                    return;
                    
                case 33: // ADDU
                    base[rd] = base[rs] + base[rt];
                    return;
                    
                case 35: // SUBU
                    base[rd] = base[rs] - base[rt];
                    return;
                    
                case 36: // AND
                    base[rd] = base[rs] & base[rt];
                    return;
                    
                case 37: // OR
                    base[rd] = base[rs] | base[rt];
                    return;
                    
                case 39: // NOR
                    base[rd] = ~(base[rs] | base[rt]);
                    return;
                    
                case 42: // SLT
                    base[rd] = (sw)base[rs] < (sw)base[rt];
                    return;
                    
                case 43: // SLTU
                    base[rd] = base[rs] < base[rt];
                    return;
            }
            printx("$%08x | Unknown special opcode $%08x | %d", pc, code, (code & 63));
            return;
            
        case 1: // REGIMM
            switch(rt) {
                case 0: // BLTZ
                    if ((sw)base[rs] < 0) {
                        branch(baddr);
                    }
                    return;
                    
                case 1: // BGEZ
                    if ((sw)base[rs] >= 0) {
                        branch(baddr);
                    }
                    return;
            }
            printx("$%08x | Unknown bcond opcode $%08x | %d", pc, code, rt);
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
            
        case 6: // BLEZ
            if ((sw)base[rs] <= 0) {
                branch(baddr);
            }
            return;
            
        case 7: // BGTZ
            if ((sw)base[rs] > 0) {
                branch(baddr);
            }
            return;
            
        case 8: // ADDI
            base[rt] = base[rs] + imm;
            return;
            
        case 9: // ADDIU
            base[rt] = base[rs] + imm;
            return;
            
        case 10: // SLTI
            base[rt] = (sw)base[rs] < imm;
            return;
            
        case 11: // SLTIU
            base[rt] = base[rs] < immu;
            return;
            
        case 12: // ANDI
            base[rt] = base[rs] & immu;
            return;
            
        case 13: // ORI
            base[rt] = base[rs] | immu;
            return;
            
        case 15: // LUI
            base[rt] = code << 16;
            return;
            
        case 16: // COP0
            switch(rs) {
                case 0: // MFC0
                    base[rt] = copr[rd];
                    return;
                    
                case 4: // MTC0
                    copr[rd] = base[rt];
                    return;
                    
                case 16: // RFE (Return From Exception)
                    // SR ← SR[31..4] || SR[5..2] /// 31..4 means 32-4 = 28-bits, also 5..2 means 6-2 = 4-bits, etc
                    copr[12] = (copr[12] & 0xfffffff0) | ((copr[12] >> 2) & 0xf);
                    return;
            }
            printx("$%08x | Unknown cop0 opcode $%08x | %d", pc, code, rs);
            return;
            
        case 32: // LB
            base[rt] = (sb)mem.read08(ob);
            return;
            
        case 33: // LH
            base[rt] = (sh)mem.read16(ob);
            return;
            
        case 35: // LW
            base[rt] = mem.read32(ob);
            return;
            
        case 36: // LBU
            base[rt] = mem.read08(ob);
            return;
            
        case 37: // LHU
            base[rt] = mem.read16(ob);
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
    printx("$%08x | Unknown basic opcode $%08x | %d", pc, code, opcode);
}

void CstrMips::exception(uw code, bool branched) {
    copr[12] = (copr[12] & 0xffffffc0) | ((copr[12] << 2) & 0x3f);
    copr[13] = code;
    copr[14] = pc;
    
    pc = 0x80;
}

void CstrMips::run() {
    // Reset state
    stopped = false;
    
    // Go!
    while(!stopped) {
        step(false);
    }
}

void CstrMips::stop() {
    stopped = true;
}
