/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


CstrMem mem;

void CstrMem::reset() {
    // Leave ROM intact, it contains BIOS
    memset(ram.ptr, 0, ram.size);
    memset(hwr.ptr, 0, hwr.size);
    
    //bus.interruptSet(CstrBus::INT_SPU);
}

template <class T>
void CstrMem::write(uw addr, T data) {
    if ((addr & MEM_MASK) < MEM_BOUNDS_RAM) { // RAM
        // A shorter method to allow mem write
        if (!(cpu.copr[12] & 0x10000)) {
            accessMem(ram, T) = data;
        }
        return;
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_SCR) { // Scratchpad
        accessMem(hwr, T) = data;
        return;
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_HWR) { // Hardware
        io.write<T>(addr, data);
        return;
    }
    
    if ((addr) == 0xfffe0130) { // Known: 0x804, 0x800, 0x1e988
        return;
    }
    
    printx("/// PSeudo Mem Write(%u): 0x%x <- 0x%x", (uw)sizeof(T), addr, data);
}

template void CstrMem::write<uw>(uw, uw);
template void CstrMem::write<uh>(uw, uh);
template void CstrMem::write<ub>(uw, ub);

template <class T>
T CstrMem::read(uw addr) {
    if ((addr & MEM_MASK) < MEM_BOUNDS_RAM) { // RAM
        return accessMem(ram, T);
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_SCR) { // Scratchpad
        return accessMem(hwr, T);
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_HWR) { // Hardware
        return io.read<T>(addr);
    }
    
    if ((addr & MEM_MASK) < MEM_BOUNDS_ROM) { // ROM
        return accessMem(rom, T);
    }
    
    if ((addr) == 0xfffe0130) { // Known: 0x804, 0x800, 0x1e988
        return 0;
    }
    
    printx("/// PSeudo Mem Read(%u): 0x%x", (uw)sizeof(T), addr);
    return 0;
}

template uw CstrMem::read<uw>(uw);
template uh CstrMem::read<uh>(uw);
template ub CstrMem::read<ub>(uw);

// OTC
void CstrMem::executeDMA(CstrBus::castDMA *dma) {
    uw *p = (uw *)&ram.ptr[dma->madr & (ram.size - 1)];
    
    if (dma->chcr == 0x11000002) {
        while(dma->bcr--) {
            *p-- = (dma->madr - 4) & 0xffffff;
            dma->madr -= 4;
        }
        p++; *p = 0xffffff;
    }
}
