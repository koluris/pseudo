#import "Global.h"


CstrBus bus;

void CstrBus::checkDMA(uw addr, uw data) {
    ub chan = ((addr >> 4) & 0xf) - 8;
    
    if (pcr & (8 << (chan * 4))) { // GPU does not execute sometimes
        chcr = data;
        
        switch(chan) {
            case 6:
                mem.executeDMA(addr);
                break;
                
            default:
                printx("Unknown DMA channel: %d", chan);
                break;
        }
        chcr = data & ~0x01000000;
        
        if (icr & (1 << (16 + chan))) {
            printx("DMA ICR %d", 0);
        }
    }
}
