/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


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
    mem.write<uw>(ob & (~(3)), (base[rt] o shift[d][ob & 3]) | (mem.read<uw>(ob & (~(3))) & mask[d][ob & 3]))

#define opcodeLWx(o, d) \
    base[rt] = (base[rt] & mask[d][ob & 3]) | (mem.read<uw>(ob & (~(3))) o shift[d][ob & 3])


CstrMips cpu;

void CstrMips::reset() {
    memset(&base, 0, sizeof(base));
    memset(&copr, 0, sizeof(copr));
    
    copr[12] = 0x10900000;
    copr[15] = 0x2; // Co-processor Revision
    
    setpc(0xbfc00000);
    res.s64 = 0;
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
    const int threshold = 100;
    
    while(!psx.suspended) {
        for (int i = 0; i < threshold; i++) {
            step(false);
        }
        
           cd.update();
        rootc.update(threshold * 3);
           vs.update(threshold * 3);
          bus.update();
        
        if (data32 & mask32) {
            if ((copr[12] & 0x401) == 0x401) {
                exception(0x400, false);
            }
        }
    }
}

void CstrMips::step(bool branched) {
    const uw code = *instCache++;
    pc += 4;
    base[0] = 0;
    
    switch(opcode) {
        case 0: // SPECIAL
            switch(code & 63) {
                case 0: // SLL
                    if (code) { // No operation
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
                    
                case 9: // JALR
                    base[rd] = pc + 4;
                    
                case 8: // JR
                    branch(base[rs]);
                    psx.console(base, pc);
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
                    res.u64 = (ud)base[rs] * base[rt];
                    return;
                    
                case 26: // DIV
                    if (base[rt]) {
                        res.s32[0] = (sw)base[rs] / (sw)base[rt];
                        res.s32[1] = (sw)base[rs] % (sw)base[rt];
                    }
                    else {
                        res.u32[0] = (sw)base[rs] >= 0 ? 0xffffffff : 1;
                        res.u32[1] = (sw)base[rs];
                    }
                    return;
                    
                case 27: // DIVU
                    if (base[rt]) {
                        res.u32[0] = base[rs] / base[rt];
                        res.u32[1] = base[rs] % base[rt];
                    }
                    else {
                        res.u32[0] = 0xffffffff;
                        res.u32[1] = base[rs];
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
                    base[rd] = (~(base[rs] | base[rt]));
                    return;
                    
                case 42: // SLT
                    base[rd] = (sw)base[rs] < (sw)base[rt];
                    return;
                    
                case 43: // SLTU
                    base[rd] = base[rs] < base[rt];
                    return;
            }
            
            printx("/// PSeudo 0x%08x | Special opcode 0x%08x | %d", pc, code, (code & 63));
            return;
            
        case 1: // REGIMM
            switch(rt) {
                case 16: // BLTZAL
                    base[31] = pc + 4;
                    
                case 0: // BLTZ
                    if ((sw)base[rs] < 0) {
                        branch(baddr);
                    }
                    return;
                    
                case 17: // BGEZAL
                    base[31] = pc + 4;
                    
                case 1: // BGEZ
                    if ((sw)base[rs] >= 0) {
                        branch(baddr);
                    }
                    return;
            }
            
            printx("/// PSeudo 0x%08x | Bcond opcode 0x%08x | %d", pc, code, rt);
            return;
            
        case 3: // JAL
            base[31] = pc + 4;
            
        case 2: // J
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
                    copr[12] = (copr[12] & (~(0xf))) | ((copr[12] >> 2) & 0xf);
                    return;
            }
            
            printx("/// PSeudo 0x%08x | Cop0 opcode 0x%08x | %d", pc, code, rs);
            return;
            
        case 18: // COP2
            cop2.execute(code);
            return;
            
        case 32: // LB
            base[rt] = (sb)mem.read<ub>(ob);
            return;
            
        case 33: // LH
            base[rt] = (sh)mem.read<uh>(ob);
            return;
            
        case 34: // LWL
            opcodeLWx(<<, 0);
            return;
            
        case 35: // LW
            base[rt] = mem.read<uw>(ob);
            return;
            
        case 36: // LBU
            base[rt] = mem.read<ub>(ob);
            return;
            
        case 37: // LHU
            base[rt] = mem.read<uh>(ob);
            return;
            
        case 38: // LWR
            opcodeLWx(>>, 1);
            return;
            
        case 40: // SB
            mem.write<ub>(ob, base[rt]);
            return;
            
        case 41: // SH
            mem.write<uh>(ob, base[rt]);
            return;
            
        case 42: // SWL
            opcodeSWx(>>, 2);
            return;
            
        case 43: // SW
            mem.write<uw>(ob, base[rt]);
            return;
            
        case 46: // SWR
            opcodeSWx(<<, 3);
            return;
            
        case 50: // LWC2
            cop2.MTC2(rt, mem.read<uw>(ob));
            return;
            
        case 58: // SWC2
            mem.write<uw>(ob, cop2.MFC2(rt));
            return;
    }
    
    printx("/// PSeudo 0x%08x | Basic opcode 0x%08x | %d", pc, code, opcode);
}

void CstrMips::branch(uw addr) {
    // Execute instruction in slot
     step(true);
    setpc(addr);
}

void CstrMips::exception(uw code, bool branched) {
    if (branched) {
        printx("/// PSeudo Exception is %s", "branched");
    }
    
    copr[12] = (copr[12] & (~(0x3f))) | ((copr[12] << 2) & 0x3f);
    copr[13] = code;
    copr[14] = pc;
    
    setpc(0x80);
}
