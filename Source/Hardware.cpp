#import "Global.h"


CstrHardware io;

void CstrHardware::write32(uw addr, uw data) {
    switch(addr & 0xffff) {
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
            accessMem(mem.hwr, uw) = data;
            return;
    }
    printx("Unknown Hardware Write 32: $%x <- $%x\n", addr, data);
}

void CstrHardware::write16(uw addr, uh data) {
    switch(addr & 0xffff) {
        /* unused */
        case 0x1d80:
        case 0x1d82:
        case 0x1d84:
        case 0x1d86:
            accessMem(mem.hwr, uh) = data;
            return;
    }
    printx("Unknown Hardware Write 16: $%x <- $%x\n", addr, data);
}

void CstrHardware::write08(uw addr, ub data) {
    switch(addr & 0xffff) {
        /* unused */
        case 0x2041:
            accessMem(mem.hwr, ub) = data;
            return;
    }
    printx("Unknown Hardware Write 08: $%x <- $%x\n", addr, data);
}
