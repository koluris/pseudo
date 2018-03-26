#import "Global.h"


CstrHardware io;

void CstrHardware::write32(uw addr, uw data) {
    switch(addr & 0xffff) {
        case 0x1010:
            accessMem(mem.hwr, uw) = data;
            return;
    }
    printx("Unknown Hardware Write 32: 0x%x <- 0x%x\n", addr, data);
}
