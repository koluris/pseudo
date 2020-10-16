#include "Global.h"


#define CD_REG(r) \
    *(ub *)&mem.hwr.ptr[0x1800 | r]

#define setResultSize(size) \
    res.p  = 0; \
    res.c  = size; \
    res.ok = 1

#define defaultCtrlAndStat() \
    control |= 0x80; \
    status = CD_STAT_NO_INTR; \
    interruptQueue(data)

#define stopRead() \
    if (reads) { \
        reads = 0; \
    }


CstrCD cd;

void CstrCD::reset() {
    param = { 0 };
    res   = { 0 };
    
    control = 0;
    status  = 0;
    statP   = 0;
    
    occupied = false;
    reads    = false;
    readed   = false;
    
    interruptSet = 0;
    irq = 0;
    re2 = 0;
}

void CstrCD::interruptQueue(ub code) {
    irq = code;
    
    if (status) {
        printf(":(\n");
    }
    else {
        interruptSet = 1;
    }
}

void CstrCD::update() {
    if (interruptSet) {
        if (interruptSet++ >= 16) {
            interruptSet = 0;
            interrupt();
        }
    }
}

void CstrCD::interrupt() {
    ub irqCache = irq;
    
    if (status) {
        interruptSet = 1;
        return;
    }

    irq = 0xff;
    control &= (~(0x80));
    
    switch(irqCache) {
        case CdlNop:
            setResultSize(1);
            status = CD_STAT_ACKNOWLEDGE;
            break;
            
        case CdlInit:
            setResultSize(1);
            statP = 0x2;
            res.data[0] = statP;
            status = CD_STAT_ACKNOWLEDGE;
            interruptQueue(CdlInit + 0x20);
            break;
            
        default:
            printx("/// PSeudo CD irqCache: %d", irqCache);
            break;
    }
    
    if (status != CD_STAT_NO_INTR && re2 != 0x18) {
        bus.interruptSet(CstrBus::INT_CD);
    }
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            control = data | (control & (~(0x3)));
            
            if (!data) {
                param.p = 0;
                param.c = 0;
                res.ok  = 0;
            }
            return;
            
        case 1:
            occupied = false;
            
            if (control & 0x1) {
                return;
            }
            
            switch(data) {
                case CdlNop: // TODO: More
                    defaultCtrlAndStat();
                    break;
                    
                case CdlInit:
                  stopRead();
                  defaultCtrlAndStat();
                  break;
                    
                default:
                    printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
                    break;
            }
            
            if (status != CD_STAT_NO_INTR) {
                bus.interruptSet(CstrBus::INT_CD);
            }
            return;
            
        case 2:
            if (control & 0x1) {
                switch (data) {
                    case 7:
                        param.p = 0;
                        param.c = 0;
                        res.ok  = 1;
                        control &= (~(3));
                        return;
                        
                    default:
                        printx("/// PSeudo CD write: %d (control & 0x1) <- %d", (addr & 0xf), data);
                        return;
                }
            }
            else if (!(control & 0x1) && param.p < 8) {
                printx("/// PSeudo CD write: %d (!(control & 0x1) && param.p < 8)", (addr & 0xf));
            }
            return;
            
        case 3:
            if (data == 0x07 && ((control & 0x1) == true)) {
                status = 0;
                
                if (irq == 0xff) {
                    irq = 0;
                    return;
                }
                
                if (irq) {
                    interruptSet = 1;
                }
                
                if (reads && !res.ok) {
                    printx("/// PSeudo CD write: %d (reads && !res.ok)", (addr & 0xf));
                }
                return;
            }
            
            if (data == 0x80 && ((control & 0x1) == false) && readed == 0) {
                printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
            }
            return;
    }
    
    printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
}

ub CstrCD::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            if (res.ok) {
                control |= 0x20;
            }
            else {
                control &= (~(0x20));
            }
            
            if (occupied) {
                control |= 0x40;
            }
            
            control |= 0x18;
            
            return CD_REG(0) = control;
            
        case 1:
            if (res.ok) {
                    CD_REG(1) = res.data[res.p++];
                    
                if (res.p == res.c) {
                    res.ok = 0;
                }
            }
            else {
                CD_REG(1) = 0;
            }
            return CD_REG(1);
            
        case 3:
            if (status) {
                if (control & 0x1) {
                    CD_REG(3) = status | 0xe0;
                }
                else {
                    printx("/// PSeudo CD read: haha 2", 0);
                    CD_REG(3) = 0xff;
                }
            }
            else {
                CD_REG(3) = 0;
            }
            return CD_REG(3);
    }
    
    printx("/// PSeudo CD read: %d", (addr & 0xf));
    return 0;
}
