#import "Global.h"


CstrMem mem;

#define accessMem(mem, dt)\
    *(dt *)&mem.ptr[addr&(mem.size-1)]

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
            return;
    }
    printf("Unknown Mem Write 32: 0x%x <- 0x%x\n", addr, data);
    exit(0);
}

uw CstrMem::read32(uw addr) {
    switch(addr) {
        case 0xbfc00000 ... 0xbfc80000-1: // ROM
            return accessMem(rom, uw);
    }
    printf("Unknown Mem Read 32: 0x%x\n", addr);
    exit(0);
    
    return 0;
}
