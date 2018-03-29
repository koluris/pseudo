#import "Global.h"


CstrMem mem;

void CstrMem::reset() {
    // Leave ROM intact, it contains BIOS
    memset(ram.ptr, 0, ram.size);
    memset(hwr.ptr, 0, hwr.size);
}

void CstrMem::write32(uw addr, uw data) {
    switch(addr) {
        case 0x00000000 ... 0x00200000-1: // RAM
        case 0x80000000 ... 0x80200000-1:
        case 0xa0000000 ... 0xa0200000-1:
            // A shorter alternative to allow mem write
            if (!(cpu.copr[12] & 0x10000)) {
                accessMem(ram, uw) = data;
            }
            return;
            
        case 0x1f801000 ... 0x1f803000-1: // Hardware
            io.write32(addr, data);
            return;
            
        case 0xfffe0130: // Possible values: $804, $800, $1e988
            printf("$fffe0130 <- $%x\n", data);
            return;
    }
    printx("Unknown Mem Write 32: $%x <- $%x\n", addr, data);
}

void CstrMem::write16(uw addr, uh data) {
    switch(addr) {
        case 0x80000000 ... 0x80200000-1: // RAM
            accessMem(ram, uh) = data;
            return;
            
        case 0x1f801000 ... 0x1f803000-1: // Hardware
            io.write16(addr, data);
            return;
    }
    printx("Unknown Mem Write 16: $%x <- $%x\n", addr, data);
}

void CstrMem::write08(uw addr, ub data) {
    switch(addr) {
        case 0x00000000 ... 0x00200000-1: // RAM
        case 0x80000000 ... 0x80200000-1:
        case 0xa0000000 ... 0xa0200000-1:
            accessMem(ram, ub) = data;
            return;
            
        case 0x1f801000 ... 0x1f803000-1: // Hardware
            io.write08(addr, data);
            return;
    }
    printx("Unknown Mem Write 08: $%x <- $%x\n", addr, data);
}

uw CstrMem::read32(uw addr) {
    switch(addr) {
        case 0x00000000 ... 0x00200000-1: // RAM
        case 0x80000000 ... 0x80200000-1:
        case 0xa0000000 ... 0xa0200000-1:
            return accessMem(ram, uw);
            
        case 0xbfc00000 ... 0xbfc80000-1: // ROM
            return accessMem(rom, uw);
            
        case 0x1f801000 ... 0x1f803000-1: // Hardware
            return io.read32(addr);
    }
    printx("Unknown Mem Read 32: $%x\n", addr);
    
    return 0;
}

ub CstrMem::read08(uw addr) {
    switch(addr) {
        case 0x00000000 ... 0x00200000-1: // RAM
        case 0x80000000 ... 0x80200000-1:
            return accessMem(ram, ub);
            
        case 0xbfc00000 ... 0xbfc80000-1: // ROM
            return accessMem(rom, ub);
            
        case 0x1f000084: // Serial?
            return 0;
    }
    printx("Unknown Mem Read 08: $%x\n", addr);
    
    return 0;
}
