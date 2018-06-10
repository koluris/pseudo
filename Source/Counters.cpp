#import "Global.h"


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
    for (int i = 0; i < 3; i++) {
        bounds[i] = RTC_BOUND;
    }
    
    vbk = 0;
    hbk = 0;
}

void CstrCounters::update() {
    count(0) += mode(0) & 0x100 ? PSX_CYCLE : PSX_CYCLE / 8;

    if (count(0) >= bound(0)) {
        printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 0, 0);
    }

    if (!(mode(1) & 0x100)) {
        count(1) += PSX_CYCLE;

        if (count(1) >= bound(1)) {
            printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 1, 1);
        }
    }
    else if ((hbk += PSX_CYCLE) >= PSX_HSYNC) { hbk = 0;
        if (++count(1) >= bound(1)) {
            count(1) = 0;
            if (mode(1) & 0x50) {
                bus.interruptSet(CstrBus::INT_RTC1);
            }
        }
    }

    if (!(mode(2) & 1)) {
        count(2) += mode(2) & 0x200 ? PSX_CYCLE / 8 : PSX_CYCLE;

        if (count(2) >= bound(2)) {
            count(2) = 0;
            if (mode(2) & 0x50) {
                bus.interruptSet(CstrBus::INT_RTC2);
            }
        }
    }
    
    // VBlank
    if ((vbk += PSX_CYCLE) >= PSX_VSYNC) { vbk = 0;
        draw.refresh();
        bus.interruptSet(CstrBus::INT_VSYNC);
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
            mode( p) = data;
            bound(p) = ((mode(p) & 8) && dst(p)) ? dst(p) : RTC_BOUND;
            return;
            
        case RTC_TARGET:
            dst(  p) = data & 0xffff;
            bound(p) = ((mode(p) & 8) && dst(p)) ? dst(p) : RTC_BOUND;
            return;
    }
    
    printx("/// PSeudo RTC Write(%lu): $%x", sizeof(T), (addr & 0xf));
}

template void CstrCounters::write<uw>(uw, uw);
template void CstrCounters::write<uh>(uw, uh);

template <class T>
T CstrCounters::read(uw addr) {
    ub p = RTC_PORT(addr);
    
    switch(addr & 0xf) {
        case RTC_COUNT:
            return count(p);
            
        case RTC_MODE:
            return mode( p);
            
        case RTC_TARGET:
            return dst(  p);
    }
    
    printx("/// PSeudo RTC Read(%lu): $%x", sizeof(T), (addr & 0xf));
    return 0;
}

template uw CstrCounters::read<uw>(uw);
template uh CstrCounters::read<uh>(uw);
