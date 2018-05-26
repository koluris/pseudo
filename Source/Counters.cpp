#import "Global.h"


CstrCounters rootc;

void CstrCounters::reset() {
    memset(&timer, 0, sizeof(timer));
    
    for (ub i = 0; i < 3; i++) {
        timer[i].bound = RTC_LIMIT;
    }
    
    vbk = 0;
    hbk = 0;
}

template <class T>
void CstrCounters::write(uw addr, T data) {
    ub chn = (addr & 0x30) >> 4;
    
    switch(addr & 0xf) {
        case RTC_COUNT:
            timer[chn].count = data & 0xffff;
            return;
            
        case RTC_MODE:
            timer[chn].mode  = data;
            timer[chn].bound = (timer[chn].mode & 8) ? timer[chn].des : RTC_LIMIT;
            return;
            
        case RTC_TARGET:
            timer[chn].des   = data & 0xffff;
            timer[chn].bound = (timer[chn].mode & 8) ? timer[chn].des : RTC_LIMIT;
            return;
    }
    
    printx("/// PSeudo RTC write(%lu) 0x%x\n", sizeof(T), (addr & 0xf));
}

template void CstrCounters::write<uw>(uw, uw);
template void CstrCounters::write<uh>(uw, uh);

template <class T>
T CstrCounters::read(uw addr) {
    ub chn = (addr & 0x30) >> 4;
    
    switch(addr & 0xf) {
        case RTC_COUNT:
            return timer[chn].count;
            
        case RTC_MODE:
            return timer[chn].mode;
            
        case RTC_TARGET:
            return timer[chn].des;
    }
    
    printx("/// PSeudo RTC read(%lu) 0x%x\n", sizeof(T), (addr & 0xf));
    return 0;
}

template uw CstrCounters::read<uw>(uw);
template uh CstrCounters::read<uh>(uw);

void CstrCounters::update() {
    timer[0].count = timer[0].count + (timer[0].mode & 0x100 ? PSX_CYCLE : PSX_CYCLE / 8);
    
    if (timer[0].count >= timer[0].bound) {
        printx("/// PSeudo RTC timer[0].count >= timer[0].bound\n", 0);
    }
    
    if (!(timer[1].mode & 0x100)) {
        timer[1].count = timer[1].count + PSX_CYCLE;
        
        if (timer[1].count >= timer[1].bound) {
            printx("/// PSeudo RTC timer[1].count >= timer[1].bound\n", 0);
        }
    }
    else if ((hbk += PSX_CYCLE) >= PSX_HSYNC) { hbk = 0;
        if (++timer[1].count >= timer[1].bound) { timer[1].count = 0;
            if (timer[1].mode & 0x50) {
                data16 |= 0x20;
            }
        }
    }
    
    if (!(timer[2].mode & 1)) {
        timer[2].count = timer[2].count + (timer[2].mode & 0x200 ? PSX_CYCLE / 8 : PSX_CYCLE);
        
        if (timer[2].count >= timer[2].bound) { timer[2].count = 0;
            if (timer[2].mode & 0x50) {
                data16 |= 0x40;
            }
        }
    }
    
    // VBlank
    if ((vbk += PSX_CYCLE) >= PSX_VSYNC) { vbk = 0;
        data16 |= 1;
        draw.refresh();
    }
}
