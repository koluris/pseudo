/* Base structure taken from FPSE open source emulator, and improved upon (Credits: BERO, LDChen) */

//#include "Global.h"
//
//
//#define count(n) \
//    *(uh *)&mem.hwr.ptr[0x1100 + (n << 4)]
//
//#define mode( n) \
//    *(uw *)&mem.hwr.ptr[0x1104 + (n << 4)]
//
//#define dst(  n) \
//    *(uh *)&mem.hwr.ptr[0x1108 + (n << 4)]
//
//#define bound(n) \
//    bounds[n]
//
//#define RTC_PORT(addr) \
//    (addr >> 4) & 3
//
//
//CstrCounters rootc;
//
//void CstrCounters::reset() {
//    for (auto &item : bounds) {
//        item = RTC_BOUND;
//    }
//
//    vbk = 0;
//    hbk = PSX_HSYNC;
//}
//
//void CstrCounters::update(int threshold) {
//    int temp;
//
//    temp = count(0) + ((mode(0) & 0x100) ? threshold : threshold / 8);
//
//    if (temp >= bound(0) && count(0) < bound(0)) { temp = 0;
//        if (mode(0) & 0x50) {
//            bus.interruptSet(CstrBus::INT_RTC0);
//        }
//    }
//    count(0) = temp;
//
//    if (!(mode(1) & 0x100)) {
//        temp = count(1) + threshold;
//
//        if (temp >= bound(1) && count(1) < bound(1)) { temp = 0;
//            if (mode(1) & 0x50) {
//                printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 1, 1);
//            }
//        }
//        count(1) = temp;
//    }
//    else {
//        if ((hbk -= threshold) <= 0) {
//            if (++count(1) == bound(1)) { count(1) = 0;
//
//                if (mode(1) & 0x50) {
//                    bus.interruptSet(CstrBus::INT_RTC1);
//                }
//            }
//            hbk = PSX_HSYNC;
//        }
//    }
//
//    if (!(mode(2) & 1)) {
//        temp = count(2) + ((mode(2) & 0x200) ? threshold / 8 : threshold);
//
//        if (temp >= bound(2) && count(2) < bound(2)) { temp = 0;
//            if (mode(2) & 0x50) {
//                bus.interruptSet(CstrBus::INT_RTC2);
//            }
//        }
//        count(2) = temp;
//    }
//
//    // VBlank
//    vbk += threshold;
//
//    if (vbk >= (vs.isVideoPAL ? PSX_VSYNC_PAL : PSX_VSYNC_NTSC)) { vbk = 0;
//        bus.interruptSet(CstrBus::INT_VSYNC);
//        vs.refresh();
//    }
//}
//
//template <class T>
//void CstrCounters::write(uw addr, T data) {
//    ub p = RTC_PORT(addr);
//    uh cond = 1;
//
//    switch(addr & 0xf) {
//        case RTC_COUNT:
//            count(p) = data & 0xffff;
//            return;
//
//        case RTC_MODE:
//            mode(p) = data;
//
//            if (sizeof(T) == 2) {
//                cond = dst(p);
//            }
//            bound(p) = (cond && (data & 0x08)) ? dst(p) : RTC_BOUND;
//            return;
//
//        case RTC_TARGET:
//            dst(p) = data & 0xffff;
//
//            if (sizeof(T) == 2) {
//                cond = dst(p);
//            }
//            bound(p) = (cond && (mode(p) & 0x08)) ? dst(p) : RTC_BOUND;
//            return;
//    }
//
//    printx("/// PSeudo RTC Write(%u): 0x%x", (uw)sizeof(T), (addr & 0xf));
//}
//
//template void CstrCounters::write<uw>(uw, uw);
//template void CstrCounters::write<uh>(uw, uh);

#include "Global.h"

CstrCounters rootc;

void CstrCounters::reset()
{
    for (int i = 0; i < 3; i++) {
        timers[i].current.raw = 0;
        timers[i].mode.raw = 0;
        timers[i].target.raw = 0;
        
        timers[i].paused = false;
        timers[i].already_fired_irq = false;
        
        timers[i].count = 0;
    }
}

template <class T>
T CstrCounters::read(uw addr)
{
    ub p = (addr >> 4) & 3;
    
    switch (addr & 0xf) {
    case 0x0: /* Write to Counter value. */
        return timers[p].current.value;
    case 0x4: /* Write to Counter control. */
        return timers[p].mode.raw;
    case 0x8: /* Write to Counter target. */
        return timers[p].target.target;
    default:
            return 0;
    }
}

template uw CstrCounters::read<uw>(uw);
template uh CstrCounters::read<uh>(uw);

template <class T>
void CstrCounters::write(uw addr, T data)
{
    ub p = (addr >> 4) & 3;
    
    switch (addr & 0xf) {
    case 0x0: /* Write to Counter value. */
        timers[p].current.raw = data;
        break;
    case 0x4: { /* Write to Counter control. */
        timers[p].current.raw = 0;
        timers[p].mode.raw = data;

        timers[p].already_fired_irq = false;
        timers[p].mode.irq_request = true;

        if (timers[p].mode.sync_enable) {
            SyncMode sync = get_sync_mode(p);

            if (p == 0 || p == 1) {
                if (sync == SyncMode::PauseFreeRun)
                    timers[p].paused = true;
            }
            if (p == 2) {
                if (sync == SyncMode::Stop)
                    timers[p].paused = true;
            }
        }
        break;
    }
    case 0x8: /* Write to Counter target. */
        timers[p].target.raw = data;
        break;
    }
}

template void CstrCounters::write<uw>(uw, uw);
template void CstrCounters::write<uh>(uw, uh);

void CstrCounters::fire_irq(int i)
{
    /* Toggle the interrupt request bit or disable it. */
    if (timers[i].mode.irq_pulse_mode == IRQToggle::Toggle)
        timers[i].mode.irq_request = !timers[i].mode.irq_request;
    else
        timers[i].mode.irq_request = false;

    bool trigger = !timers[i].mode.irq_request;
    if (timers[i].mode.irq_repeat_mode == IRQRepeat::OneShot) {
        if (!timers[i].already_fired_irq && trigger)
            timers[i].already_fired_irq = true;
        else
            return;
    }
    
    timers[i].mode.irq_request = true;
    irq_type(i);
}

void CstrCounters::tick(uw cycles, int i)
{
    /* Get the timer's clock source. */
    ClockSrc clock_src = get_clock_source(i);

    /* Get the timer's sync mode. */
    SyncMode sync = get_sync_mode(i);

    /* Add the cycles to the counter. */
    timers[i].count += cycles;

    /* Increment timer according to the clock source and id. */
    if (i == 0) {
        if (timers[i].mode.sync_enable) {
            /* Timer is paused. */
            if (sync == SyncMode::Pause && in_hblank) {
                return;
            }
            /* Timer must be reset. */
            else if (sync == SyncMode::Reset && in_hblank) {
                timers[i].current.raw = 0;
            }
            /* Timer must be reset and paused outside of hblank. */
            else if (sync == SyncMode::ResetPause) {
                if (in_hblank) timers[i].current.raw = 0;
                else return;
            }
            /* Timer is paused until hblank occurs once. */
            else {
                if (!prev_hblank && in_hblank) timers[i].mode.sync_enable = false;
                else return;
            }
        }

        if (clock_src == ClockSrc::Dotclock) {
            timers[i].current.raw += (uh)(timers[i].count * 11 / 7 / dot_div);
            timers[i].count = 0;
        }
        else {
            timers[i].current.raw += (uh)timers[i].count;
            timers[i].count = 0;
        }
    }
    else if (i == 1) {
        if (timers[i].mode.sync_enable) {
            /* Timer is paused. */
            if (sync == SyncMode::Pause && in_vblank) {
                return;
            }
            /* Timer must be reset. */
            else if (sync == SyncMode::Reset && in_vblank) {
                timers[i].current.raw = 0;
            }
            /* Timer must be reset and paused outside of vblank. */
            else if (sync == SyncMode::ResetPause) {
                if (in_vblank) timers[i].current.raw = 0;
                else return;
            }
            /* Timer is paused until vblank occurs once. */
            else {
                if (!prev_vblank && in_vblank) timers[i].mode.sync_enable = false;
                else return;
            }
        }

        if (clock_src == ClockSrc::Hblank) {
            timers[i].current.raw += (uh)(timers[i].count / 2160);
            timers[i].count %= 2160;
        }
        else {
            timers[i].current.raw += (uh)timers[i].count;
            timers[i].count = 0;
        }
    }
    else if (i == 2) {
        if (timers[i].mode.sync_enable && sync == SyncMode::Stop)
            return;

        if (clock_src == ClockSrc::SystemDiv8) {
            timers[i].current.raw += (uh)(timers[i].count / 8);
            timers[i].count %= 8;
        }
        else {
            timers[i].current.raw += (uh)timers[i].count;
            timers[i].count = 0;
        }
    }

    bool should_irq = false;
    /* If we reached or exceeded the target value. */
    if (timers[i].current.raw >= timers[i].target.target) {
        timers[i].mode.reached_target = true;

        if (timers[i].mode.reset == ResetWhen::Target)
            timers[i].current.raw = 0;

        if (timers[i].mode.irq_when_target)
            should_irq = true;
    }

    /* If there was an overflow. */
    if (timers[i].current.raw >= 0xffff) {
        timers[i].mode.reached_overflow = true;

        //if (mode.reset == ResetWhen::Overflow)
        //    current.raw = 0;

        if (timers[i].mode.irq_when_overflow)
            should_irq = true;
    }

    /* Fire interrupt if necessary. */
    if (should_irq) {
        fire_irq(i);
    }
}

void CstrCounters::gpu_sync(GPUSync sync) {
    prev_hblank = in_hblank;
    prev_vblank = in_vblank;
    
    in_hblank = sync.hblank;
    in_vblank = sync.vblank;
    
    dot_div = sync.dotDiv;
}

void CstrCounters::irq_type(int i)
{
    /* Select interrupt time. */
    switch (i) {
        case 0:
            bus.interruptSet(CstrBus::INT_RTC0);
            break;
        case 1:
            bus.interruptSet(CstrBus::INT_RTC1);
            break;
        case 2:
            bus.interruptSet(CstrBus::INT_RTC2);
            break;
        default:
            break;
    }
}

ClockSrc CstrCounters::get_clock_source(int i)
{
    uw clock_src = timers[i].mode.clock_source;

    /* Timer0 can have either dotclock or sysclock. */
    if (i == 0) {
        /* Values 1, 3 -> dotclock. Values 0, 2 -> sysclock */
        if (clock_src == 1 || clock_src == 3)
            return ClockSrc::Dotclock;
        else
            return ClockSrc::System;
    } /* Timer1 can have either hblank or sysclock. */
    else if (i == 1) {
        /* Values 1, 3 -> hblank. Values 0, 2 -> sysclock */
        if (clock_src == 1 || clock_src == 3)
            return ClockSrc::Hblank;
        else
            return ClockSrc::System;
    } /* Timer2 can have either syslock / 8 or sysclock. */
    else {
        /* Values 0, 1 -> sysclock. Values 2, 3 -> sysclock / 8. */
        if (clock_src == 0 || clock_src == 1)
            return ClockSrc::System;
        else
            return ClockSrc::SystemDiv8;
    }
}

SyncMode CstrCounters::get_sync_mode(int i)
{
    uint sync = timers[i].mode.sync_mode;

    if (i == 0 || i == 1) {
        return (SyncMode)sync;
    }
    else {
        if (sync == 0 || sync == 3)
            return SyncMode::Stop;
        else
            return SyncMode::FreeRun;
    }
}
