#import "Global.h"


CstrMem mem;

void CstrMem::reset() {
    // Leave ROM intact, it contains BIOS
    memset(ram.ptr, 0, ram.size);
    memset(hwr.ptr, 0, hwr.size);
}

void CstrMem::write32(uw addr, uw data) {
    switch(addr) {
        case 0x00000000 ... (0x00800000-1): // RAM
        case 0x80000000 ... (0x80800000-1):
        case 0xa0000000 ... (0xa0200000-1):
            // A shorter alternative to allow mem write
            if (!(cpu.copr[12] & 0x10000)) {
                accessMem(ram, uw) = data;
            }
            return;
            
        case 0x1f800000 ... (0x1f800400-1): // Scratchpad
            accessMem(hwr, uw) = data;
            return;
            
        case 0x1f801000 ... (0x1f804000-1): // Hardware
            io.write32(addr, data);
            return;
            
        case 0xfffe0130: // Possible values: $804, $800, $1e988
            //printf("$fffe0130 <- $%x\n", data);
            return;
    }
    printx("/// PSeudo Mem Write 32: $%x <- $%x", addr, data);
}

void CstrMem::write16(uw addr, uh data) {
    switch(addr) {
        case 0x00000000 ... (0x00800000-1): // RAM
        case 0x80000000 ... (0x80800000-1):
        case 0xa0000000 ... (0xa0200000-1):
            accessMem(ram, uh) = data;
            return;
            
        case 0x1f800000 ... (0x1f800400-1): // Scratchpad
            accessMem(hwr, uh) = data;
            return;
            
        case 0x1f801000 ... (0x1f804000-1): // Hardware
            io.write16(addr, data);
            return;
    }
    printx("/// PSeudo Mem Write 16: $%x <- $%x", addr, data);
}

void CstrMem::write08(uw addr, ub data) {
    switch(addr) {
        case 0x00000000 ... (0x00800000-1): // RAM
        case 0x80000000 ... (0x80800000-1):
        case 0xa0000000 ... (0xa0200000-1):
            accessMem(ram, ub) = data;
            return;
            
        case 0x1f800000 ... (0x1f800400-1): // Scratchpad
            accessMem(hwr, ub) = data;
            return;
            
        case 0x1f801000 ... (0x1f804000-1): // Hardware
            io.write08(addr, data);
            return;
    }
    printx("/// PSeudo Mem Write 08: $%x <- $%x", addr, data);
}

uw CstrMem::read32(uw addr) {
    switch(addr) {
        case 0x00000000 ... (0x00800000-1): // RAM
        case 0x80000000 ... (0x80800000-1):
        case 0xa0000000 ... (0xa0200000-1):
            return accessMem(ram, uw);
            
        case 0xbfc00000 ... (0xbfc80000-1): // ROM
            return accessMem(rom, uw);
            
        case 0x1f800000 ... (0x1f800400-1): // Scratchpad
            return accessMem(hwr, uw);
            
        case 0x1f801000 ... (0x1f804000-1): // Hardware
            return io.read32(addr);
    }
    printx("/// PSeudo Mem Read 32: $%x", addr);
    
    return 0;
}

uh CstrMem::read16(uw addr) {
    switch(addr) {
        case 0x00000000 ... (0x00800000-1): // RAM
        case 0x80000000 ... (0x80800000-1):
            return accessMem(ram, uh);
            
        case 0xbfc00000 ... (0xbfc80000-1): // ROM
            return accessMem(rom, uh);
            
        case 0x1f800000 ... (0x1f800400-1): // Scratchpad
            return accessMem(hwr, uh);
            
        case 0x1f801000 ... (0x1f804000-1): // Hardware
            return io.read16(addr);
    }
    printx("/// PSeudo Mem Read 16: $%x", addr);
    
    return 0;
}

ub CstrMem::read08(uw addr) {
    switch(addr) {
        case 0x00000000 ... (0x00800000-1): // RAM
        case 0x80000000 ... (0x80800000-1):
            return accessMem(ram, ub);
            
        case 0xbfc00000 ... (0xbfc80000-1): // ROM
            return accessMem(rom, ub);
            
        case 0x1f000084: // Serial?
            return 0;
            
        case 0x1f800000 ... (0x1f800400-1): // Scratchpad
            return accessMem(hwr, ub);
            
        case 0x1f801000 ... (0x1f804000-1): // Hardware
            return io.read08(addr);
    }
    printx("/// PSeudo Mem Read 08: $%x", addr);
    
    return 0;
}

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
