#import "Global.h"


// Low order bits
#define lob(addr)\
    (addr & 0xffff)


CstrHardware io;

void CstrHardware::write32(uw addr, uw data) {
    switch(lob(addr)) {
        case 0x1070:
            data32 &= data & mask32;
            return;
            
        case 0x10a8 ... 0x10e8: // DMA
            if (addr & 8) {
                bus.checkDMA(addr, data);
                return;
            }
            accessMem(mem.hwr, uw) = data;
            return;
            
        case 0x10f4: // Thanks Calb, Galtor :)
            icr = (icr & (~((data & 0xff000000) | 0xffffff))) | (data & 0xffffff);
            return;
            
        case 0x1810:
            vs.dataWrite(data);
            return;
            
        case 0x1814:
            vs.statusWrite(data);
            return;
            
        /* unused */
        case 0x1114 ... 0x1118: // Rootcounters
            
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
    switch(lob(addr)) {
        case 0x1070:
            data16 &= data & mask16;
            return;
            
        /* unused */
        case 0x1100 ... 0x1128: // Rootcounters
        case 0x1c00 ... 0x1dfe: // Audio
        case 0x1074:
            accessMem(mem.hwr, uh) = data;
            return;
    }
    printx("Unknown Hardware Write 16: $%x <- $%x", addr, data);
}

void CstrHardware::write08(uw addr, ub data) {
    switch(lob(addr)) {
        /* unused */
        case 0x2041:
            accessMem(mem.hwr, ub) = data;
            return;
    }
    printx("Unknown Hardware Write 08: $%x <- $%x", addr, data);
}

uw CstrHardware::read32(uw addr) {
    switch(lob(addr)) {
        case 0x1810:
            return vs.dataRead();
            
        case 0x1814:
            return vs.statusRead();
            
        /* unused */
        case 0x1070:
        case 0x1074:
        case 0x10e8:
        case 0x10f0:
        case 0x10f4:
            return accessMem(mem.hwr, uw);
    }
    printx("Unknown Hardware Read 32: $%x", addr);
    
    return 0;
}

uh CstrHardware::read16(uw addr) {
    switch(lob(addr)) {
        /* unused */
        case 0x1c0c ... 0x1dae: // Audio
        case 0x1070:
        case 0x1074:
            return accessMem(mem.hwr, uh);
    }
    printx("Unknown Hardware Read 16: $%x", addr);
    
    return 0;
}
