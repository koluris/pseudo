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
                    
                case 0x10f4: // DICR, thanks Calb, Galtor :)
                    dicr = (dicr & (~((data & 0xff000000) | 0xffffff))) | (data & 0xffffff);
                    return;
                      
				case 0x1810: // Graphics
				case 0x1814:
                    vs.write(addr, data);
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
                case 0x1820: // MDEC 0
                case 0x1824: // MDEC 1
                    accessMem(mem.hwr, uw) = data;
                    return;

				default:
					if (addr >= 0x1f801080 && addr <= 0x1f8010e8) { // DMA
						if (addr & 8) {
							bus.checkDMA(addr, data);
							return;
						}
						accessMem(mem.hwr, uw) = data;
						return;
					}
					
					if (addr >= 0x1f801104 && addr <= 0x1f801124) { // Rootcounters
						rootc.write<uw>(addr, data);
						return;
					}
            }
            break;
            
        case HWR_ACCESS_16:
            switch(LOW_BITS(addr)) {
                case 0x1070: // iStatus
                    data16 &= data & mask16;
                    return;
                    
                /* unused */
                case 0x1014: // ?
				case 0x1048: // SIO Mode
				case 0x104e: // SIO Baud
                case 0x1074: // iMask
                    accessMem(mem.hwr, uh) = data;
                    return;

				default:
					if (addr >= 0x1f801104 && addr <= 0x1f801124) { // Rootcounters
						rootc.write<uw>(addr, data);
						return;
					}

					if (addr >= 0x1f801c00 && addr <= 0x1f801dfe) { // Audio
						audio.write(addr, data);
						return;
					}
            }
            break;
            
        case HWR_ACCESS_08:
            switch(LOW_BITS(addr)) {   
                /* unused */
                case 0x1040: // SIO Data
                case 0x2041:
                    accessMem(mem.hwr, ub) = data;
                    return;

				default:
					if (addr >= 0x1f801800 && addr <= 0x1f801803) { // CD-ROM
						cd.write(addr, data);
						return;
					}
            }
            break;
    }
    
    printx("/// PSeudo HW Write(%lu): 0x%x <- 0x%x", sizeof(T), addr, data);
}

template void CstrHardware::write<uw>(uw, uw);
template void CstrHardware::write<uh>(uw, uh);
template void CstrHardware::write<ub>(uw, ub);

template <class T>
T CstrHardware::read(uw addr) {
    switch(sizeof(T)) {
        case HWR_ACCESS_32:
            switch(LOW_BITS(addr)) {
				case 0x1810: // Graphics
				case 0x1814:
                    return vs.read(addr);
                    
                /* unused */
                case 0x1014: // ?
                case 0x1060: // RAM Size
                case 0x1070: // iStatus
                case 0x1074: // iMask
                case 0x1098: // DMA
                case 0x10a8:
                case 0x10c8:
                case 0x10e8:
                case 0x10f0: // DPCR
                case 0x10f4: // DICR
                case 0x1824: // MDEC 1
                    return accessMem(mem.hwr, uw);

				default:
					if (addr >= 0x1f801100 && addr <= 0x1f801110) { // Rootcounters
						return accessMem(mem.hwr, uw);
					}
            }
            break;
            
        case HWR_ACCESS_16:
            switch(LOW_BITS(addr)) {
                case 0x1044: // SIO Status
                    return sio.read16();
                    
                /* unused */
                case 0x1014: // ?
                case 0x104a: // SIO Control
                case 0x104e: // SIO Baud
                case 0x1054: // SIO Status
                case 0x1070: // iStatus
                case 0x1074: // iMask
					return accessMem(mem.hwr, uh);

				default:
					if (addr >= 0x1f801100 && addr <= 0x1f801128) { // Rootcounters
						return accessMem(mem.hwr, uh);
					}

					if (addr >= 0x1f801c00 && addr <= 0x1f801e0e) { // Audio
						return audio.read(addr);
					}
            }
            break;
            
        case HWR_ACCESS_08:
            switch(LOW_BITS(addr)) {
                case 0x1040: // SIO Data
                    return sio.read08();
                    
				default:
					if (addr >= 0x1f801800 && addr <= 0x1f801803) { // CD-ROM
						return cd.read(addr);
					}
            }
            break;
    }
    
    printx("/// PSeudo HW Read(%lu): 0x%x", sizeof(T), addr);
    return 0;
}

template uw CstrHardware::read<uw>(uw);
template uh CstrHardware::read<uh>(uw);
template ub CstrHardware::read<ub>(uw);
