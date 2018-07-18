/* Base structure taken from FPSE open source emulator, and improved upon (Credits: BERO, LDChen) */

#include "Global.h"


#define count(n) \
    *(uh *)&mem.hwr.ptr[0x1100 + (n << 4)]

#define mode( n) \
    *(uw *)&mem.hwr.ptr[0x1104 + (n << 4)]

#define dst(  n) \
    *(uh *)&mem.hwr.ptr[0x1108 + (n << 4)]

#define bound(n) \
    bounds[n]

#define RTC_PORT(addr) \
    (addr >> 4) & 3


CstrCounters rootc;

void CstrCounters::reset() {
    for (auto &item : bounds) {
        item = RTC_BOUND;
    }
    
    vbk = 0;
    hbk = PSX_HSYNC;
}

void CstrCounters::update() {
    int temp;
    
    temp = count(0) + ((mode(0) & 0x100) ? PSX_BIAS : PSX_BIAS / 8);
    
    if (temp >= bound(0) && count(0) < bound(0)) { temp = 0;
        if (mode(0) & 0x50) {
            printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 0, 0);
        }
    }
    count(0) = temp;
    
    if (!(mode(1) & 0x100)) {
        temp = count(1) + PSX_BIAS;
        
        if (temp >= bound(1) && count(1) < bound(1)) { temp = 0;
            if (mode(1) & 0x50) {
                printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 1, 1);
            }
        }
        count(1) = temp;
    }
    else {
        if ((hbk -= PSX_BIAS) <= 0) {
            if (++count(1) == bound(1)) { count(1) = 0;
                
                if (mode(1) & 0x50) {
                    bus.interruptSet(CstrBus::INT_RTC1);
                }
            }
            hbk = PSX_HSYNC;
        }
    }
    
    if (!(mode(2) & 1)) {
        temp = count(2) + ((mode(2) & 0x200) ? PSX_BIAS / 8 : PSX_BIAS);
        
        if (temp >= bound(2) && count(2) < bound(2)) { temp = 0;
            if (mode(2) & 0x50) {
                bus.interruptSet(CstrBus::INT_RTC2);
            }
        }
        count(2) = temp;
    }
    
    // VBlank
    vbk += PSX_BIAS;
    
    if (vbk >= (vs.isVideoPAL ? PSX_VSYNC_PAL : PSX_VSYNC_NTSC)) { vbk = 0;
        vs.refresh();
    }
}

template <class T>
void CstrCounters::write(uw addr, T data) {
    ub p = RTC_PORT(addr);
    
    switch(addr & 0xf) {
        case RTC_COUNT:
            count(p) = data & 0xffff;
            return;
            
        case RTC_MODE:
            mode (p) = data;
            bound(p) = ((mode(p) & 8) && dst(p)) ? dst(p) : RTC_BOUND;
            return;
            
        case RTC_TARGET:
            dst  (p) = data & 0xffff;
            bound(p) = ((mode(p) & 8) && dst(p)) ? dst(p) : RTC_BOUND;
            return;
    }
    
    printx("/// PSeudo RTC Write(%lu): 0x%x", sizeof(T), (addr & 0xf));
}

template void CstrCounters::write<uw>(uw, uw);
template void CstrCounters::write<uh>(uw, uh);
