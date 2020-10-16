/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


CstrBus bus;

void CstrBus::reset() {
    for (auto &item : interrupts) {
        item.queued = INT_DISABLED;
    }
}

void CstrBus::interruptSet(ub code) {
    interrupts[code].queued = INT_ENABLED;
}

void CstrBus::update() { // A method to schedule when IRQs should fire
    for (auto &item : interrupts) {
        if (item.queued) {
            if (item.queued++ == item.dest) {
                item.queued = INT_DISABLED;
                data16 |= 1 << item.code;
            }
        }
    }
}

void CstrBus::checkDMA(uw addr, uw data) {
    ub chan = ((addr >> 4) & 0xf) - 8;
    
    if (dpcr & (8 << (chan * 4))) { // GPU does not execute sometimes
        castDMA *dma = (castDMA *)&mem.hwr.ptr[addr & 0xfff0];
        dma->chcr = data;
        
        switch(chan) {
            case DMA_MDEC_IN: // TODO
            case DMA_MDEC_OUT:
                break;
                
            case DMA_GPU:
                vs.executeDMA(dma);
                break;
                
            case DMA_CD:
                cd.executeDMA(dma);
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
            bus.interruptSet(CstrBus::INT_DMA);
        }
    }
}
