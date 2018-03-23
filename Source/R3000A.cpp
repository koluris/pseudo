#import "Global.h"


CstrMips cpu;

void CstrMips::reset() {
    memset(base, 0, sizeof(base));
    pc = 0xbfc00000;
    step();
}

void CstrMips::step() {
    uw code = mem.read32(pc);
    
    printf("Code -> 0x%08x\n", code);
    exit(0);
}
