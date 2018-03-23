#import "Global.h"


CstrMem mem;

void CstrMem::init() {
    ram = (ub *)malloc(0x200000);
    rom = (ub *)malloc(0x80000);
    hwr = (ub *)malloc(0x4000);
}

void CstrMem::reset() {
    memset(&ram, 0, sizeof(ram));
    memset(&rom, 0, sizeof(rom));
    memset(&hwr, 0, sizeof(hwr));
}
