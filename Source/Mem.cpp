#import "Global.h"


CstrMem mem;

void CstrMem::reset() {
    // Do not reset ROM, it contains the BIOS
    memset(ram.ptr, 0, ram.size);
    memset(hwr.ptr, 0, hwr.size);
}

uw CstrMem::read32(uw addr) {
    switch(addr) {
        // ROM
        case 0xbfc00000 ... 0xbfc80000-1:
            return *(uw *)&rom.ptr[addr&(rom.size-1)];
    }
    printf("Unknown Mem Read 32 -> 0x%08x\n", addr);
    exit(0);
    
    return 0;
}
