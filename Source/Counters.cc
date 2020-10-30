#include "Global.h"

#define BIAS \
    1.5f

#define TMR_FIRE_IRQ(limit, on, when) \
    if (tval >= tmr->limit) { \
        if (tmr->mode.resetZero == ResetToZero::on) tval = 0; \
        if (tmr->mode.when) bus.interruptSet(CstrBus::INT_RTC0 + p); \
    }

CstrCounters rootc;

void CstrCounters::reset() {
    for (auto &tmr : timer) {
        tmr.current = 0;
        tmr.destination = 0;
        tmr.temp = 0;
        tmr.bounds = 0xffff;
    }
}

void CstrCounters::update(uw frames) {
    for (int p = 0; p < 3; p++) {
        auto tmr = &timer[p];
        
        uw tval   = tmr->current;
        uw source = tmr->mode.clockSource >> ((p == 2) ? 1 : 0);
        float divisor = BIAS;
        
        if ((source & 1) == 1) {
            switch(p) {
                case 0: divisor = 6; break;
                case 1: divisor = 3413; break;
                case 2: divisor = 8 * BIAS; break;
            }
        }
        
        tval += (tmr->temp += frames) / divisor;
        tmr->temp %= (uw)divisor;
        
        TMR_FIRE_IRQ(destination, onDest  , irqWhenDest);
        TMR_FIRE_IRQ(     bounds, onBounds, irqWhenBounds);
        
        tmr->current = (uh)tval;
    }
}

uh CstrCounters::read(uw addr) {
    auto tmr = &timer[(addr >> 4) & 3];
    
    switch(addr & 0xf) {
        case 0: return tmr->current;
        case 4: return tmr->mode.data;
        case 8: return tmr->destination;
    }
    
    return 0;
}

void CstrCounters::write(uw addr, uh data) {
    auto tmr = &timer[(addr >> 4) & 3];
    
    switch(addr & 0xf) {
        case 0: tmr->current     = data; return;
        case 4: tmr->mode.data   = data; return;
        case 8: tmr->destination = data; return;
    }
}
