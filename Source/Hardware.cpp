#import "Global.h"


CstrHardware io;

#define hob(addr)\
    (addr & 0xffff)

void CstrHardware::write32(uw addr, uw data) {
    switch(hob(addr)) {
        case 0x1070:
            data32 &= data & mask32;
            return;
            
        /* unused */
        case 0x1000:
        case 0x1004:
        case 0x1008:
        case 0x100c:
        case 0x1010:
        case 0x1014: // SPU
        case 0x1018: // DV5
        case 0x101c:
        case 0x1020: // COM
        case 0x1060: // RAM Size
        case 0x1074:
        case 0x10f0:
            accessMem(mem.hwr, uw) = data;
            return;
    }
    printx("Unknown Hardware Write 32: $%x <- $%x", addr, data);
}

void CstrHardware::write16(uw addr, uh data) {
    switch(hob(addr)) {
        case 0x1100: // Rootcounters
        case 0x1104:
        case 0x1108:
        case 0x1110:
        case 0x1114:
        case 0x1118:
        case 0x1120:
        case 0x1124:
        case 0x1128:
            accessMem(mem.hwr, uh) = data;
            return;
            
        /* unused */
        case 0x1c00 ... 0x1dfe: // Audio
            accessMem(mem.hwr, uh) = data;
            return;
    }
    printx("Unknown Hardware Write 16: $%x <- $%x", addr, data);
}

void CstrHardware::write08(uw addr, ub data) {
    switch(hob(addr)) {
        /* unused */
        case 0x2041:
            accessMem(mem.hwr, ub) = data;
            return;
    }
    printx("Unknown Hardware Write 08: $%x <- $%x", addr, data);
}

uw CstrHardware::read32(uw addr) {
    switch(hob(addr)) {
        /* unused */
        case 0x1074:
        case 0x10f0:
            return accessMem(mem.hwr, uw);
    }
    printx("Unknown Hardware Read 32: $%x", addr);
    
    return 0;
}

uh CstrHardware::read16(uw addr) {
    switch(hob(addr)) {
        /* unused */
        case 0x1d88 ... 0x1dae: // Audio
            return accessMem(mem.hwr, uh);
    }
    printx("Unknown Hardware Read 16: $%x", addr);
    
    return 0;
}
