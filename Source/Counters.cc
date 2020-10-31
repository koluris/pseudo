#include "Global.h"


#define RTC_FIRE_IRQ(limit, on, when) \
    if (tval >= tmr->limit) { \
        if (tmr->mode.resetZero == ResetToZero::on) tval = 0; \
        if (tmr->mode.when) bus.interruptSet(CstrBus::INT_RTC0 + p); \
    }

#define RTC_PORT \
    (addr >> 4) & 3


CstrCounters rootc;

void CstrCounters::reset() {
    for (auto &tmr : timer) {
        tmr.current = 0;
        tmr.bounds  = 0xffff;
        tmr.dest    = 0;
        tmr.temp    = 0;
    }
}

void CstrCounters::update(uw frames) {
    for (int p = 0; p < 3; p++) {
        auto tmr = &timer[p];
        
        uw tval   = tmr->current;
        uw source = tmr->mode.clockSource >> ((p == 2) ? 1 : 0);
        
        float rate = 1.5f;
        if ((source & 1) == 1) {
            rate = table[p];
        }
        
        tval += (tmr->temp += frames) / rate;
        tmr->temp %= (uw)rate;
        
        RTC_FIRE_IRQ(  dest, onDest  , irqWhenDest);
        RTC_FIRE_IRQ(bounds, onBounds, irqWhenBounds);
        
        tmr->current = (uh)tval;
    }
}

void CstrCounters::write(uw addr, uh data) {
    auto tmr = &timer[RTC_PORT];
    
    switch(addr & 0xf) {
        case 0: tmr->current   = data; return;
        case 4: tmr->mode.data = data; return;
        case 8: tmr->dest      = data; return;
    }
    
#ifdef DEBUG
    printx("/// PSeudo RTC Write: %d <- 0x%x", (addr & 0xf), data);
#endif
}

uh CstrCounters::read(uw addr) {
    auto tmr = &timer[RTC_PORT];
    
    switch(addr & 0xf) {
        case 0: return tmr->current;
        case 4: return tmr->mode.data;
        case 8: return tmr->dest;
    }
    
#ifdef DEBUG
    printx("/// PSeudo RTC Read: %d", (addr & 0xf));
#endif
    return 0;
}
