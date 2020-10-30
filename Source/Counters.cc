#include "Global.h"

CstrCounters rootc;

void CstrCounters::reset() {
    oneShotIrqOccured = false;
}

void CstrCounters::step(ub p, uw c) {
    auto tmr = &timer[p];
    tmr->cnt += c;
    uw tval = tmr->current;
    
    switch(p) {
        case 0:
            if ((ClockSource0)(tmr->mode.clockSource & 1) == ClockSource0::dotClock) {
                tval += tmr->cnt / 6;
                tmr->cnt %= 6;
            } else {
                tval += (int)(tmr->cnt / 1.5f);
                tmr->cnt %= (int)1.5f;
            }
            break;
            
        case 1:
            if ((ClockSource1)(tmr->mode.clockSource & 1) == ClockSource1::hblank) {
                tval += tmr->cnt / 3413;
                tmr->cnt %= 3413;
            } else {
                tval += (int)(tmr->cnt / 1.5f);
                tmr->cnt %= (int)1.5f;
            }
            break;
            
        case 2:
            if ((ClockSource2)((tmr->mode.clockSource >> 1) & 1) == ClockSource2::systemClock_8) {
                tval += (int)(tmr->cnt / (8 * 1.5f));
                tmr->cnt %= (int)(8 * 1.5f);
            } else {
                tval += (int)(tmr->cnt * 1.5f);
                tmr->cnt %= (int)1.5f;
            }
            break;
    }
    
    bool possibleIrq = false;
    
    if (tval >= tmr->target) {
        tmr->mode.reachedTarget = true;
        if (tmr->mode.resetToZero == ResetToZero::whenTarget) tval = 0;
        if (tmr->mode.irqWhenTarget) possibleIrq = true;
    }
    
    if (tval >= 0xffff) {
        tmr->mode.reachedFFFF = true;
        if (tmr->mode.resetToZero == ResetToZero::whenFFFF) tval = 0;
        if (tmr->mode.irqWhenFFFF) possibleIrq = true;
    }
    
    if (possibleIrq) {
        if (tmr->mode.irqPulseMode == IrqPulseMode::toggle) {
            tmr->mode.interruptRequest = !tmr->mode.interruptRequest;
        } else {
            tmr->mode.interruptRequest = false;
        }
        
        if (tmr->mode.irqRepeatMode == IrqRepeatMode::oneShot && oneShotIrqOccured) {
            return;
        }
        
        if (tmr->mode.interruptRequest == false) {
            bus.interruptSet(CstrBus::INT_RTC0 + p);
            oneShotIrqOccured = true;
        }
        tmr->mode.interruptRequest = true;
    }
    
    tmr->current = (uh)tval;
}

uh CstrCounters::read(uw addr) {
    ub i = (addr >> 4) & 3;
    
    switch(addr & 0xf) {
        case 0:
            return timer[i].current;
            
        case 4:
            return timer[i].mode.data;
            
        case 8:
            return timer[i].target;
    }
    
    return 0;
}

void CstrCounters::write(uw addr, uh data) {
    ub i = (addr >> 4) & 3;
    
    switch(addr & 0xf) {
        case 0:
            timer[i].current = data;
            return;
            
        case 4:
            timer[i].current = 0;
            timer[i].mode.data = data;
            return;
            
        case 8:
            timer[i].target = data;
            return;
    }
}
