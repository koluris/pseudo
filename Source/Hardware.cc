/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


CstrHardware io;

template <class T>
void CstrHardware::write(uw addr, T data) {
    switch(sizeof(T)) {
        case HWR_ACCESS_32:
            switch(LOW_BITS(addr)) {
                case 0x1070: // iStatus
                    data32 &= data & mask32;
                    return;
                    
                case 0x1080 ... 0x10e8: // DMA
                    if (addr & 8) {
                        bus.checkDMA(addr, data);
                        return;
                    }
                    accessMem(mem.hwr, uw) = data;
                    return;
                    
                case 0x10f4: // DICR, thanks Calb, Galtor :)
                    dicr = (dicr & (~((data & 0xff000000) | 0xffffff))) | (data & 0xffffff);
                    return;
                    
                case 0x1104 ... 0x1124: // Rootcounters
                    rootc.write(addr, data);
                    return;
                    
                case 0x1810 ... 0x1814: // Graphics
                    vs.write(addr, data);
                    return;
                    
                case 0x1820 ... 0x1824: // MDEC
                    mdec.write(addr, data);
                    return;
                    
                /* unused */
                case 0x1000: // ?
                case 0x1004: // ?
                case 0x1008: // ?
                case 0x100c: // ?
                case 0x1010: // ?
                case 0x1014: // SPU
                case 0x1018: // DV5
                case 0x101c: // ?
                case 0x1020: // COM
                case 0x1060: // RAM Size
                case 0x1074: // iMask
                case 0x10f0: // DPCR
                case 0x1c00 ... 0x1c40: // V8 2nd Offence (?)
                    accessMem(mem.hwr, uw) = data;
                    return;
            }
            break;
            
        case HWR_ACCESS_16:
            switch(LOW_BITS(addr)) {
                case 0x1040 ... 0x104e: // SIO 0
                    sio.write16(addr, data);
                    return;
                    
                case 0x1070: // iStatus
                    data16 &= data & mask16;
                    return;
                    
                case 0x1100 ... 0x1128: // Rootcounters
                    rootc.write(addr, data);
                    return;
                    
                case 0x1c00 ... 0x1dfe: // Audio
                    audio.write(addr, data);
                    return;
                    
                /* unused */
                case 0x1014: // ?
                case 0x1058: // SIO 1 ?
                case 0x105a: // SIO 1 Control
                case 0x105e: // SIO 1 Baud
                case 0x1074: // iMask
                    accessMem(mem.hwr, uh) = data;
                    return;
            }
            break;
            
        case HWR_ACCESS_08:
            switch(LOW_BITS(addr)) {
                case 0x1040 ... 0x104e: // SIO 0
                    sio.write08(addr, data);
                    return;
                    
                case 0x1800 ... 0x1803: // CD-ROM
                    cd.write(addr, data);
                    return;
                    
                /* unused */
                case 0x10f6:
                case 0x2041: // DIP Switch?
                    accessMem(mem.hwr, ub) = data;
                    return;
            }
            break;
    }
    
    printx("/// PSeudo HW Write(%u): 0x%x <- 0x%x", (uw)sizeof(T), addr, data);
}

template void CstrHardware::write<uw>(uw, uw);
template void CstrHardware::write<uh>(uw, uh);
template void CstrHardware::write<ub>(uw, ub);

template <class T>
T CstrHardware::read(uw addr) {
    switch(sizeof(T)) {
        case HWR_ACCESS_32:
            switch(LOW_BITS(addr)) {
                case 0x1100 ... 0x1110: // Rootcounters
                    return rootc.read(addr);
                    
                case 0x1810 ... 0x1814: // Graphics
                    return vs.read(addr);
                    
                case 0x1820 ... 0x1824: // MDEC
                    return mdec.read(addr);
                    
                /* unused */
                case 0x1014: // ?
                case 0x1060: // RAM Size
                case 0x1070: // iStatus
                case 0x1074: // iMask
                case 0x1088 ... 0x10e8: // DMA
                case 0x10f0: // DPCR
                case 0x10f4: // DICR
                    return accessMem(mem.hwr, uw);
            }
            break;
            
        case HWR_ACCESS_16:
            switch(LOW_BITS(addr)) {
                case 0x1040 ... 0x104e: // SIO 0
                    return sio.read16(addr);
                    
                case 0x1100 ... 0x1128: // Rootcounters
                    return rootc.read(addr);
                    
                case 0x1c00 ... 0x1e3e: // Audio
                    return audio.read(addr);
                    
                /* unused */
                case 0x1014: // ?
                case 0x1054: // SIO 1 Status
                case 0x105a: // SIO 1 Control
                case 0x105e: // SIO 1 Baud
                case 0x1070: // iStatus
                case 0x1074: // iMask
                case 0x1130: // ?
                    return accessMem(mem.hwr, uh);
            }
            break;
            
        case HWR_ACCESS_08:
            switch(LOW_BITS(addr)) {
                case 0x1040 ... 0x104e: // SIO 0
                    return sio.read08(addr);
                    
                case 0x1800 ... 0x1803: // CD-ROM
                    return cd.read(addr);
                    
                /* unused */
                case 0x10f6: // ?
                case 0x1c08 ... 0x1d78: // XenoGears (?)
                    return accessMem(mem.hwr, ub);
            }
            break;
    }
    
    printx("/// PSeudo HW Read(%u): 0x%x", (uw)sizeof(T), addr);
    return 0;
}

template uw CstrHardware::read<uw>(uw);
template uh CstrHardware::read<uh>(uw);
template ub CstrHardware::read<ub>(uw);
