#import "Global.h"


CstrHardware io;

void CstrHardware::write32(uw addr, uw data) {
    switch(lob(addr)) {
        case 0x1070: // iStatus
            data32 &= data & mask32;
            return;
            
        case 0x10a0 ... 0x10e8: // DMA
            if (addr & 8) {
                bus.checkDMA(addr, data);
                return;
            }
            accessMem(mem.hwr, uw) = data;
            return;
            
        case 0x10f4: // DICR, thanks Calb, Galtor :)
            icr = (icr & (~((data & 0xff000000) | 0xffffff))) | (data & 0xffffff);
            return;
            
        case 0x1810 ... 0x1814: // Graphics
            vs.write(addr, data);
            return;
            
        /* unused */
        case 0x1000: // ?
        case 0x1004: // ?
        case 0x1008: // ?
        case 0x100c: // ?
        case 0x1010: // ?
        case 0x1014: // SPU
        case 0x1018: // DV5
        case 0x101c: // ?
        case 0x1020: // COM
        case 0x1060: // RAM Size
        case 0x1074: // iMask
        case 0x10f0: // DPCR
        case 0x1114 ... 0x1118: // Rootcounters
            accessMem(mem.hwr, uw) = data;
            return;
    }
    printx("PSeudo /// Hardware Write 32: $%x <- $%x", addr, data);
}

void CstrHardware::write16(uw addr, uh data) {
    switch(lob(addr)) {
        case 0x1070: // iStatus
            data16 &= data & mask16;
            return;
            
        /* unused */
        case 0x1014: // ?
        case 0x1048: // SIO
        case 0x104a:
        case 0x104e:
        case 0x1074: // iMask
        case 0x1100 ... 0x1128: // Rootcounters
        case 0x1c00 ... 0x1dfe: // Audio
            accessMem(mem.hwr, uh) = data;
            return;
    }
    printx("PSeudo /// Hardware Write 16: $%x <- $%x", addr, data);
}

void CstrHardware::write08(uw addr, ub data) {
    switch(lob(addr)) {
        /* unused */
        case 0x1040: // SIO
        case 0x1800 ... 0x1803: // CD-ROM
        case 0x2041:
            accessMem(mem.hwr, ub) = data;
            return;
    }
    printx("PSeudo /// Hardware Write 08: $%x <- $%x", addr, data);
}

uw CstrHardware::read32(uw addr) {
    switch(lob(addr)) {
        case 0x1810 ... 0x1814: // Graphics
            return vs.read(addr);
            
        /* unused */
        case 0x1014: // ?
        case 0x1070: // iStatus
        case 0x1074: // iMask
        case 0x10a8: // DMA
        case 0x10c8:
        case 0x10e8:
        case 0x10f0: // DPCR
        case 0x10f4: // DICR
        case 0x1110: // Rootcounters
            return accessMem(mem.hwr, uw);
    }
    printx("PSeudo /// Hardware Read 32: $%x", addr);
    
    return 0;
}

uh CstrHardware::read16(uw addr) {
    switch(lob(addr)) {
        case 0x1044: // SIO
            return 0xffff; // Nice :)
            
        /* unused */
        case 0x104a: // SIO
        case 0x1014: // ?
        case 0x1070: // iStatus
        case 0x1074: // iMask
        case 0x1110: // Rootcounters
        case 0x1c08 ... 0x1dae: // Audio
            return accessMem(mem.hwr, uh);
    }
    printx("PSeudo /// Hardware Read 16: $%x", addr);
    
    return 0;
}

ub CstrHardware::read08(uw addr) {
    switch(lob(addr)) {
        /* unused */
        case 0x1040: // SIO
        case 0x1800 ... 0x1803: // CD-ROM
            return accessMem(mem.hwr, ub);
    }
    printx("PSeudo /// Hardware Read 08: $%x", addr);
    
    return 0;
}
