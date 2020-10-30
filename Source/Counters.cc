#include "Global.h"

CstrCounters rootc;

void CstrCounters::reset() {
}

void CstrCounters::step(int i, int cycles) {
    timer[i].cnt += cycles;
    uw tval = timer[i].current;
    if (i == 0) {
        auto clock = (ClockSource0)(timer[i].mode.clockSource & 1);
        
        if (clock == ClockSource0::dotClock) {
            tval += timer[i].cnt / 6;
            timer[i].cnt %= 6;
        } else {
            tval += (int)(timer[i].cnt / 1.5f);
            timer[i].cnt %= (int)1.5f;
        }
    } else if (i == 1) {
        auto clock = (ClockSource1)(timer[i].mode.clockSource & 1);
        
        if (clock == ClockSource1::hblank) {
            tval += timer[i].cnt / 3413;
            timer[i].cnt %= 3413;
        } else {
            tval += (int)(timer[i].cnt / 1.5f);
            timer[i].cnt %= (int)1.5f;
        }
    } else if (i == 2) {
        auto clock = (ClockSource2)((timer[i].mode.clockSource >> 1) & 1);
        
        if (clock == ClockSource2::systemClock_8) {
            tval += (int)(timer[i].cnt / (8 * 1.5f));
            timer[i].cnt %= (int)(8 * 1.5f);
        } else {
            tval += (int)(timer[i].cnt * 1.5f);
            timer[i].cnt %= (int)1.5f;
        }
    }
    
    bool possibleIrq = false;
    
    if (tval >= timer[i].target) {
        timer[i].mode.reachedTarget = true;
        if (timer[i].mode.resetToZero == ResetToZero::whenTarget) tval = 0;
        if (timer[i].mode.irqWhenTarget) possibleIrq = true;
    }
    
    if (tval >= 0xffff) {
        timer[i].mode.reachedFFFF = true;
        if (timer[i].mode.resetToZero == ResetToZero::whenFFFF) tval = 0;
        if (timer[i].mode.irqWhenFFFF) possibleIrq = true;
    }
    
    if (possibleIrq) {
        if (timer[i].mode.irqPulseMode == IrqPulseMode::toggle) {
            timer[i].mode.interruptRequest = !timer[i].mode.interruptRequest;
        } else {
            timer[i].mode.interruptRequest = false;
        }
        
        if (timer[i].mode.irqRepeatMode == IrqRepeatMode::oneShot && oneShotIrqOccured) {
            return;
        }
        
        if (timer[i].mode.interruptRequest == false) {
            bus.interruptSet(4 + i);
            oneShotIrqOccured = true;
        }
        timer[i].mode.interruptRequest = true;
    }
    
    timer[i].current = (uh)tval;
}

uh CstrCounters::read(uw addr) {
    ub i = (addr >> 4) & 3;
    
    switch(addr & 0xf) {
        case 0:
            return timer[i].current;
            
        case 4:
            return timer[i].mode._reg;
            
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
            timer[i].mode._reg = data;
            return;
            
        case 8:
            timer[i].target = data;
            return;
    }
}
