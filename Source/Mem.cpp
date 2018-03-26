#import "Global.h"


CstrMem mem;

void CstrMem::reset() {
    // Leave ROM intact, it contains BIOS
    memset(ram.ptr, 0, ram.size);
    memset(hwr.ptr, 0, hwr.size);
}

void CstrMem::write32(uw addr, uw data) {
    switch(addr) {
       // case 0x00000000 ... 0x00200000-1: // RAM
            //accessMem(ram, uw) = data;
            //return;
            
        case 0x1f801000 ... 0x1f803000-1: // Hardware
            io.write32(addr, data);
            return;
    }
    printx("Unknown Mem Write 32: $%x <- $%x\n", addr, data);
}

uw CstrMem::read32(uw addr) {
    switch(addr) {
        case 0xbfc00000 ... 0xbfc80000-1: // ROM
            return accessMem(rom, uw);
    }
    printx("Unknown Mem Read 32: $%x\n", addr);
    
    return 0;
}
