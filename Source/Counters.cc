#include "Global.h"

CstrCounters rootc;

void CstrCounters::reset()
{
    for (int i = 0; i < 3; i++) {
        timer[i].current.raw = 0;
        timer[i].mode.raw = 0;
        timer[i].destination.raw = 0;
        
        timer[i].paused = false;
        timer[i].already_fired_irq = false;
        
        timer[i].count = 0;
    }
}

void CstrCounters::tick(uw cycles, int i) {
    ClockSrc clock_src = get_clock_source(i);
    SyncMode sync = get_sync_mode(i);
    timer[i].count += cycles;
    
    if (i == 0) {
        if (timer[i].mode.sync_enable) {
            if (sync == SyncMode::Pause && in_hblank) {
                return;
            }
            else if (sync == SyncMode::Reset && in_hblank) {
                timer[i].current.raw = 0;
            }
            else if (sync == SyncMode::ResetPause) {
                if (in_hblank) {
                    timer[i].current.raw = 0;
                }
                else {
                    return;
                }
            }
            else {
                if (!prev_hblank && in_hblank) {
                    timer[i].mode.sync_enable = false;
                }
                else {
                    return;
                }
            }
        }
        
        if (clock_src == ClockSrc::Dotclock) {
            timer[i].current.raw += (uh)(timer[i].count * 11 / 7 / dot_div);
            timer[i].count = 0;
        }
        else {
            timer[i].current.raw += (uh)timer[i].count;
            timer[i].count = 0;
        }
    }
    else if (i == 1) {
        if (timer[i].mode.sync_enable) {
            if (sync == SyncMode::Pause && in_vblank) {
                return;
            }
            else if (sync == SyncMode::Reset && in_vblank) {
                timer[i].current.raw = 0;
            }
            else if (sync == SyncMode::ResetPause) {
                if (in_vblank) {
                    timer[i].current.raw = 0;
                }
                else {
                    return;
                }
            }
            else {
                if (!prev_vblank && in_vblank) {
                    timer[i].mode.sync_enable = false;
                }
                else {
                    return;
                }
            }
        }
        
        if (clock_src == ClockSrc::Hblank) {
            timer[i].current.raw += (uh)(timer[i].count / 2160);
            timer[i].count %= 2160;
        }
        else {
            timer[i].current.raw += (uh)timer[i].count;
            timer[i].count = 0;
        }
    }
    else if (i == 2) {
        if (timer[i].mode.sync_enable && sync == SyncMode::Stop) {
            return;
        }
        
        if (clock_src == ClockSrc::SystemDiv8) {
            timer[i].current.raw += (uh)(timer[i].count / 8);
            timer[i].count %= 8;
        }
        else {
            timer[i].current.raw += (uh)timer[i].count;
            timer[i].count = 0;
        }
    }
    
    bool should_irq = false;
    
    if (timer[i].current.raw >= timer[i].destination.destination) {
        timer[i].mode.reached_destination = true;
        
        if (timer[i].mode.reset == ResetWhen::Destination) {
            timer[i].current.raw = 0;
        }
        
        if (timer[i].mode.irq_when_target) {
            should_irq = true;
        }
    }
    
    if (timer[i].current.raw >= 0xffff) {
        timer[i].mode.reached_overflow = true;
        
        if (timer[i].mode.irq_when_overflow) {
            should_irq = true;
        }
    }
    
    if (should_irq) {
        if (timer[i].mode.irq_pulse_mode == IRQAlternate::Alternate) {
            timer[i].mode.irq_request = !timer[i].mode.irq_request;
        }
        else {
            timer[i].mode.irq_request = false;
        }
        
        if (timer[i].mode.irq_repeat_mode == IRQRepeat::OneShot) {
            if (!timer[i].already_fired_irq && !timer[i].mode.irq_request) {
                timer[i].already_fired_irq = true;
            }
            else {
                return;
            }
        }
        
        timer[i].mode.irq_request = true;
        bus.interruptSet(CstrBus::INT_RTC0 + i);
    }
}

template <class T>
void CstrCounters::write(uw addr, T data) {
    ub p = (addr >> 4) & 3;
    
    switch(addr & 0xf) {
        case 0:
            timer[p].current.raw = data;
            return;
            
        case 4:
            timer[p].current.raw = 0;
            timer[p].mode.raw = data;
            timer[p].already_fired_irq = false;
            timer[p].mode.irq_request = true;
            
            if (timer[p].mode.sync_enable) {
                SyncMode sync = get_sync_mode(p);
                
                if (p == 0 || p == 1) {
                    if (sync == SyncMode::PauseFreeRun) {
                        timer[p].paused = true;
                    }
                }
                if (p == 2) {
                    if (sync == SyncMode::Stop) {
                        timer[p].paused = true;
                    }
                }
            }
            return;
            
        case 8:
            timer[p].destination.raw = data;
            return;
    }
}

template void CstrCounters::write<uw>(uw, uw);
template void CstrCounters::write<uh>(uw, uh);

template <class T>
T CstrCounters::read(uw addr) {
    ub p = (addr >> 4) & 3;
    
    switch(addr & 0xf) {
        case 0:
            return timer[p].current.value;
            
        case 4:
            return timer[p].mode.raw;
            
        case 8:
            return timer[p].destination.destination;
    }
    
    return 0;
}

template uw CstrCounters::read<uw>(uw);
template uh CstrCounters::read<uh>(uw);

void CstrCounters::gpu_sync(GPUSync sync) {
    prev_hblank = in_hblank;
    prev_vblank = in_vblank;
    
    in_hblank = sync.hblank;
    in_vblank = sync.vblank;
    
    dot_div = sync.dotDiv;
}

CstrCounters::ClockSrc CstrCounters::get_clock_source(int i) {
    uw clock_src = timer[i].mode.clock_source;
    
    if (i == 0) {
        if (clock_src == 1 || clock_src == 3) {
            return ClockSrc::Dotclock;
        }
        else {
            return ClockSrc::System;
        }
    }
    else if (i == 1) {
        if (clock_src == 1 || clock_src == 3) {
            return ClockSrc::Hblank;
        }
        else {
            return ClockSrc::System;
        }
    }
    else {
        if (clock_src == 0 || clock_src == 1) {
            return ClockSrc::System;
        }
        else {
            return ClockSrc::SystemDiv8;
        }
    }
}

CstrCounters::SyncMode CstrCounters::get_sync_mode(int i) {
    uw sync = timer[i].mode.sync_mode;
    
    if (i == 0 || i == 1) {
        return (SyncMode)sync;
    }
    else {
        if (sync == 0 || sync == 3) {
            return SyncMode::Stop;
        }
        else {
            return SyncMode::FreeRun;
        }
    }
}
