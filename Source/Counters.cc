/* Base structure taken from FPSE open source emulator, and improved upon (Credits: BERO, LDChen) */

#import "Global.h"


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
//    vbk = hbk = 0;
//}
//
//void CstrCounters::update() {
//    count(0) += mode(0) & 0x100 ? PSX_BIAS : PSX_BIAS / 8;
//
//    if (count(0) >= bound(0)) {
//        printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 0, 0);
//    }
//
//    if (!(mode(1) & 0x100)) {
//        count(1) += PSX_BIAS;
//
//        if (count(1) >= bound(1)) {
//            printx("/// PSeudo RTC timer[%d].count >= timer[%d].bound", 1, 1);
//        }
//    }
//    else if ((hbk += PSX_BIAS) >= PSX_HSYNC) { hbk = 0;
//        if (++count(1) >= bound(1)) {
//            count(1) = 0;
//            if (mode(1) & 0x50) {
//                bus.interruptSet(CstrBus::INT_RTC1);
//            }
//        }
//    }
//
//    if (!(mode(2) & 1)) {
//        count(2) += mode(2) & 0x200 ? PSX_BIAS / 8 : PSX_BIAS;
//
//        if (count(2) >= bound(2)) {
//            count(2) = 0;
//            if (mode(2) & 0x50) {
//                bus.interruptSet(CstrBus::INT_RTC2);
//            }
//        }
//    }
//
//    // VBlank
//    if ((vbk += PSX_BIAS) >= (vs.isVideoPAL ? PSX_VSYNC_PAL : PSX_VSYNC_NTSC)) { vbk = 0;
//        vs.refresh();
//    }
//}
//
//template <class T>
//void CstrCounters::write(uw addr, T data) {
//    ub p = RTC_PORT(addr);
//
//    switch(addr & 0xf) {
//        case RTC_COUNT:
//            count(p) = data & 0xffff;
//            return;
//
//        case RTC_MODE:
//            mode (p) = data;
//            bound(p) = ((mode(p) & 8) && dst(p)) ? dst(p) : RTC_BOUND;
//            return;
//
//        case RTC_TARGET:
//            dst  (p) = data & 0xffff;
//            bound(p) = ((mode(p) & 8) && dst(p)) ? dst(p) : RTC_BOUND;
//            return;
//    }
//
//    printx("/// PSeudo RTC Write(%lu): 0x%x", sizeof(T), (addr & 0xf));
//}
//
//template void CstrCounters::write<uw>(uw, uw);
//template void CstrCounters::write<uh>(uw, uh);
//
//template <class T>
//T CstrCounters::read(uw addr) {
//    ub p = RTC_PORT(addr);
//
//    switch(addr & 0xf) {
//        case RTC_COUNT:
//            return count(p);
//
//        case RTC_MODE:
//            return mode (p);
//
//        case RTC_TARGET:
//            return dst  (p);
//    }
//
//    printx("/// PSeudo RTC Read(%lu): 0x%x", sizeof(T), (addr & 0xf));
//    return 0;
//}
//
//template uw CstrCounters::read<uw>(uw);
//template uh CstrCounters::read<uh>(uw);

#define BIAS    2

static int cnts = 4;
psxCounter psxCounters[5];

static void psxRcntUpd(unsigned long index) {
    psxCounters[index].sCycle = cpu.opcodeCount;
    
    if (((!(psxCounters[index].mode & 1)) || (index!=2)) &&
        psxCounters[index].mode & 0x30) {
        if (psxCounters[index].mode & 0x10) {
            psxCounters[index].Cycle = ((psxCounters[index].target - psxCounters[index].count) * psxCounters[index].rate) / BIAS;
        } else {
            psxCounters[index].Cycle = ((0xffff - psxCounters[index].count) * psxCounters[index].rate) / BIAS;
        }
    } else psxCounters[index].Cycle = 0xffffffff;
}

static void psxRcntReset(unsigned long index) {
    psxCounters[index].count = 0;
    psxRcntUpd(index);
    bus.interruptSet(psxCounters[index].interrupt);
    
    if (!(psxCounters[index].mode & 0x40)) {
        psxCounters[index].Cycle = 0xffffffff;
    }
}

uw psxNextCounter, psxNextsCounter;

static void psxRcntSet() {
    psxNextCounter = 0x7fffffff;
    psxNextsCounter = cpu.opcodeCount;
    
    for (int i=0; i<cnts; i++) {
        sw count;
        if (psxCounters[i].Cycle == 0xffffffff) continue;
        count = psxCounters[i].Cycle - (cpu.opcodeCount - psxCounters[i].sCycle);
        
        if (count < 0) {
            psxNextCounter = 0; break;
        }
        
        if (count < (sw)psxNextCounter) {
            psxNextCounter = count;
        }
    }
}

void psxRcntInit() {
    memset(psxCounters, 0, sizeof(psxCounters));
    
    psxCounters[0].rate = 1; psxCounters[0].interrupt = CstrBus::INT_RTC0;
    psxCounters[1].rate = 1; psxCounters[1].interrupt = CstrBus::INT_RTC1;
    psxCounters[2].rate = 1; psxCounters[2].interrupt = CstrBus::INT_RTC2;
    
    psxCounters[3].interrupt = CstrBus::INT_VSYNC;
    psxCounters[3].mode = 0x58;
    psxCounters[3].target = 1;
    psxUpdateVSyncRate();
    
    cnts = 4;
    
    psxRcntUpd(0);
    psxRcntUpd(1);
    psxRcntUpd(2);
    psxRcntUpd(3);
    psxRcntSet();
}

#define PSXCLK    33868800

void psxUpdateVSyncRate() {
    psxCounters[3].rate = (PSXCLK / 60);// / BIAS;
    psxCounters[3].rate-= (psxCounters[3].rate / 262) * 22;
    //if (Config.VSyncWA) psxCounters[3].rate/= 2;
}

void psxUpdateVSyncRateEnd() {
    psxCounters[3].rate = (PSXCLK / 60);// / BIAS;
    psxCounters[3].rate = (psxCounters[3].rate / 262) * 22;
    //if (Config.VSyncWA) psxCounters[3].rate/= 2;
}

void psxRcntUpdate() {
    if ((cpu.opcodeCount - psxCounters[3].sCycle) >= psxCounters[3].Cycle) {
        if (psxCounters[3].mode & 0x10000) {
            psxCounters[3].mode&=~0x10000;
            psxUpdateVSyncRate();
            psxRcntUpd(3);
            vs.refresh();
        } else {
            psxCounters[3].mode|= 0x10000;
            psxUpdateVSyncRateEnd();
            psxRcntUpd(3);
            
            bus.interruptSet(CstrBus::INT_VSYNC);
        }
    }
    
    if ((cpu.opcodeCount - psxCounters[0].sCycle) >= psxCounters[0].Cycle) {
        psxRcntReset(0);
    }
    
    if ((cpu.opcodeCount - psxCounters[1].sCycle) >= psxCounters[1].Cycle) {
        psxRcntReset(1);
    }
    
    if ((cpu.opcodeCount - psxCounters[2].sCycle) >= psxCounters[2].Cycle) {
        psxRcntReset(2);
    }
    
    if (cnts >= 5) {
        if ((cpu.opcodeCount - psxCounters[4].sCycle) >= psxCounters[4].Cycle) {
//            SPU_async((psxRegs.cycle - psxCounters[4].sCycle) * BIAS);
            psxRcntReset(4);
        }
    }
    
    psxRcntSet();
}

void psxRcntWcount(uw index, uw value) {
    //    SysPrintf("writeCcount[%d] = %x\n", index, value);
    //    PSXCPU_LOG("writeCcount[%d] = %x\n", index, value);
    psxCounters[index].count = value;
    psxRcntUpd(index);
    psxRcntSet();
}

void psxRcntWmode(uw index, uw value)  {
    psxCounters[index].mode = value;
    psxCounters[index].count = 0;
    if(index == 0) {
        switch (value & 0x300) {
            case 0x100:
                psxCounters[index].rate = ((psxCounters[3].rate /** BIAS*/) / 386) / 262; // seems ok
                break;
            default:
                psxCounters[index].rate = 1;
        }
    }
    else if(index == 1) {
        switch (value & 0x300) {
            case 0x100:
                psxCounters[index].rate = (psxCounters[3].rate /** BIAS*/) / 262; // seems ok
                break;
            default:
                psxCounters[index].rate = 1;
        }
    }
    else if(index == 2) {
        switch (value & 0x300) {
            case 0x200:
                psxCounters[index].rate = 8; // 1/8 speed
                break;
            default:
                psxCounters[index].rate = 1; // normal speed
        }
    }
    
    // Need to set a rate and target
    psxRcntUpd(index);
    psxRcntSet();
}

void psxRcntWtarget(uw index, uw value) {
    psxCounters[index].target = value;
    psxRcntUpd(index);
    psxRcntSet();
}

uw psxRcntRcount(uw index) {
    uw ret;
    
    if (psxCounters[index].mode & 0x08) {
//        if (Config.RCntFix) {
//            ret = (psxCounters[index].count + /*BIAS **/ ((cpu.opcodeCount - psxCounters[index].sCycle) / psxCounters[index].rate)) & 0xffff;
//        }
//        else {
            ret = (psxCounters[index].count + BIAS * ((cpu.opcodeCount - psxCounters[index].sCycle) / psxCounters[index].rate)) & 0xffff;
//        }
    }
    else {
        ret = (psxCounters[index].count + BIAS * (cpu.opcodeCount / psxCounters[index].rate)) & 0xffff;
//        if (Config.RCntFix) {
//            ret/= 16;
//        }
    }
    
    return ret;
}
