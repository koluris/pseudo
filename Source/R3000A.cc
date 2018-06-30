#import "Global.h"


/*  6-bit */
#define opcode \
    ((code >> 26) & 63)

/* 16-bit */
#define imm \
    ((sh)code)

#define immu \
    (code & 0xffff)

/* 32-bit */
#define ob \
    (base[rs] + imm)

#define baddr \
    (pc + (imm << 2))

#define saddr \
    ((code & 0x3ffffff) << 2) | (pc & 0xf0000000)

#define opcodeSWx(o, d) \
    mem.write32(ob & ~3, (base[rt] o shift[d][ob & 3]) | (mem.read32(ob & ~3) & mask[d][ob & 3]))

#define opcodeLWx(o, d) \
    base[rt] = (base[rt] & mask[d][ob & 3]) | (mem.read32(ob & ~3) o shift[d][ob & 3])


CstrMips cpu;

// SLLV
//
// 32 | 16 |  8 |  4 |  2 |  1 |
// ---|----|----|----|----|----| -> 4
//  0 |  0 |  0 |  1 |  0 |  0 |

void CstrMips::reset() {
    memset(&base, 0, sizeof(base));
    memset(&copr, 0, sizeof(copr));
    cop2c = { 0 };
    cop2d = { 0 };
    
    copr[12] = 0x10900000;
    copr[15] = 0x2; // Co-processor Revision
    
    setpc(0xbfc00000);
    res.s64 = opcodeCount = 0;
}

void CstrMips::setpc(uw addr) {
    pc = addr;
    
    if ((pc >> 20) == 0xbfc) {
        instCache = (uw *)&mem.rom.ptr[addr & 0x000fffff];
    }
    else {
        instCache = (uw *)&mem.ram.ptr[addr & 0x00ffffff];
    }
}

void CstrMips::bootstrap() {
    while(pc != 0x80030000) {
        step(false);
    }
}

void CstrMips::run() {
    // Go!
    while(!psx.suspended) {
        step(false);
    }
}

void CstrMips::step(bool branched) {
    uw code = *instCache++; pc += 4;
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
                    branch(base[rs]);
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
                    
                case 13: // BREAK
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
                    
                case 24: // MULT
                    res.s64 = (sd)(sw)base[rs] * (sw)base[rt];
                    return;
                    
                case 25: // MULTU
                    res.s64 = (sd)base[rs] * base[rt];
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
                    
                case 34: // SUB
                    base[rd] = base[rs] - base[rt];
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
                    
                case 38: // XOR
                    base[rd] = base[rs] ^ base[rt];
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
            
            printx("/// PSeudo $%08x | Unknown special opcode $%08x | %d", pc, code, (code & 63));
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
                    
                case 17: // BGEZAL
                    base[31] = pc + 4;
                    
                    if ((sw)base[rs] >= 0) {
                        branch(baddr);
                    }
                    return;
            }
            
            printx("/// PSeudo $%08x | Unknown bcond opcode $%08x | %d", pc, code, rt);
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
            
        case 14: // XORI
            base[rt] = base[rs] ^ immu;
            return;
            
        case 15: // LUI
            base[rt] = code << 16;
            return;
            
        case 16: // COP0
            switch(rs) {
                case MFC:
                    base[rt] = copr[rd];
                    return;
                    
                case MTC:
                    copr[rd] = base[rt];
                    return;
                    
                case RFE: // Return from exception
                    copr[12] = (copr[12] & 0xfffffff0) | ((copr[12] >> 2) & 0xf);
                    return;
            }
            
            printx("/// PSeudo $%08x | Unknown cop0 opcode $%08x | %d", pc, code, rs);
            return;
            
        case 18: // COP2
            switch(rs) {
                case MFC:
                    readCop2(rd);
                    base[rt] = cop2d.iuw[rd];
                    return;
                    
                case CFC:
                    base[rt] = cop2c.iuw[rd];
                    return;
                    
                case MTC:
                    cop2d.iuw[rd] = base[rt];
                    writeCop2(rd);
                    return;
                    
                case CTC:
                    cop2c.iuw[rd] = base[rt];
                    return;
            }
            
            // Execute GTE opcode
            executeCop2(code);
            return;
            
        case 32: // LB
            base[rt] = (sb)mem.read08(ob);
            return;
            
        case 33: // LH
            base[rt] = (sh)mem.read16(ob);
            return;
            
        case 34: // LWL
            opcodeLWx(<<, 0);
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
            
        case 38: // LWR
            opcodeLWx(>>, 1);
            return;
            
        case 40: // SB
            mem.write08(ob, base[rt]);
            return;
            
        case 41: // SH
            mem.write16(ob, base[rt]);
            return;
            
        case 42: // SWL
            opcodeSWx(>>, 2);
            return;
            
        case 43: // SW
            mem.write32(ob, base[rt]);
            return;
            
        case 46: // SWR
            opcodeSWx(<<, 3);
            return;
            
        case 50: // LWC2
            cop2d.iuw[rt] = mem.read32(ob);
            writeCop2(rt);
            return;
            
        case 58: // SWC2
            readCop2(rt);
            mem.write32(ob, cop2d.iuw[rt]);
            return;
    }
    
    printx("/// PSeudo $%08x | Unknown basic opcode $%08x | %d", pc, code, opcode);
}

#define LAZY_CLOCK \
    64

void CstrMips::branch(uw addr) {
    // Execute instruction in slot
    step (true);
    setpc(addr);
    
    if (opcodeCount >= LAZY_CLOCK) {
        // Rootcounters, interrupts
        rootc.update();
        bus.interruptsUpdate();
        
        // Exceptions
        if (data32 & mask32) {
            if ((copr[12] & 0x401) == 0x401) {
                exception(0x400, false);
            }
        }
        opcodeCount %= LAZY_CLOCK;
    }
}

void CstrMips::exception(uw code, bool branched) {
    if (branched) {
        printx("/// PSeudo Exception %s", "branched");
    }
    
    copr[12] = (copr[12] & 0xffffffc0) | ((copr[12] << 2) & 0x3f);
    copr[13] = code;
    copr[14] = pc;
    
    setpc(0x80);
}
