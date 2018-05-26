#import "Global.h"


CstrBus bus;

void CstrBus::reset() {
    ub size = sizeof(interrupts) / sizeof(interrupts[0]);
    
    for (ub i = 0; i < size; i++) {
        interrupts[i].queued = IRQ_QUEUED_NO;
    }
}

void CstrBus::interruptsUpdate() { // A method to schedule when IRQs should fire
    ub size = sizeof(interrupts) / sizeof(interrupts[0]);
    
    for (ub i = 0; i < size; i++) {
        interrupt *item = &interrupts[i];
        
        if (item->queued) {
            if (item->queued++ == item->dest) {
                item->queued = IRQ_QUEUED_NO;
                data16 |= (1 << item->code);
                break;
            }
        }
    }
}

void CstrBus::interruptSet(ub code) {
    interrupts[code].queued = IRQ_QUEUED_YES;
}

void CstrBus::checkDMA(uw addr, uw data) {
    ub chan = ((addr >> 4) & 0xf) - 8;
    
    if (dpcr & (8 << (chan * 4))) { // GPU does not execute sometimes
        castDMA *dma = (castDMA *)&mem.hwr.ptr[addr & 0xfff0];
        dma->chcr = data;
        
        switch(chan) {
            case DMA_GPU:
                vs.executeDMA(dma);
                break;
                
            case DMA_SPU:
                audio.executeDMA(dma);
                break;
                
            case DMA_CLEAR_OT:
                mem.executeDMA(dma);
                break;
                
            default:
                printx("/// PSeudo DMA Channel: %d", chan);
                break;
        }
        dma->chcr = data & ~(0x01000000);
        
        if (dicr & (1 << (16 + chan))) {
            dicr |= 1 << (24 + chan);
            bus.interruptSet(CstrBus::IRQ_DMA);
        }
    }
}
