#include "Global.h"


#define CD_REG(r) \
    *(ub *)&mem.hwr.ptr[0x1800 | r]

#define setResultSize(size) \
    result.p = 0; \
    result.c = size; \
    result.done = true

#define defaultCtrlAndStat() \
    ret.control |= 0x80; \
    ret.status = CD_STAT_NO_INTR; \
    interruptQueue(data)

#define stopRead() \
    if (reads) { \
        reads = 0; \
    }


CstrCD cd;

void CstrCD::reset() {
    ret    = { 0 };
    param  = { 0 };
    result = { 0 };
    
    occupied = false;
    reads    = false;
    readed   = false;
    
    irq = 0;
    interruptSet = 0;
}

void CstrCD::interruptQueue(ub code) {
    irq = code;
    
    if (ret.status) {
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
    
    if (ret.status) {
        interruptSet = 1;
        return;
    }

    irq = 0xff;
    ret.control &= (~(0x80));
    
    switch(irqCache) {
        case CdlNop:
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            break;
            
        case CdlInit:
            setResultSize(1);
            ret.statp = 0x2;
            result.data[0] = ret.statp;
            ret.status = CD_STAT_ACKNOWLEDGE;
            interruptQueue(CdlInit + 0x20);
            break;
            
        case CdlInit + 0x20:
            setResultSize(1);
            result.data[0] = ret.statp;
            ret.status = CD_STAT_COMPLETE;
            break;
            
        default:
            printx("/// PSeudo CD irqCache: %d", irqCache);
            break;
    }
    
    if (ret.status != CD_STAT_NO_INTR && ret.re2 != 0x18) {
        bus.interruptSet(CstrBus::INT_CD);
    }
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            ret.control = data | (ret.control & (~(0x3)));
            
            if (!data) {
                param.p = 0;
                param.c = 0;
                result.done = false;
            }
            return;
            
        case 1:
            occupied = false;
            
            if (ret.control & 0x1) {
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
            
            if (ret.status != CD_STAT_NO_INTR) {
                bus.interruptSet(CstrBus::INT_CD);
            }
            return;
            
        case 2:
            if (ret.control & 0x1) {
                switch (data) {
                    case 7:
                        param.p = 0;
                        param.c = 0;
                        result.done = true;
                        ret.control &= (~(3));
                        return;
                        
                    default:
                        printx("/// PSeudo CD write: %d (control & 0x1) <- %d", (addr & 0xf), data);
                        return;
                }
            }
            else if (!(ret.control & 0x1) && param.p < 8) {
                printx("/// PSeudo CD write: %d (!(control & 0x1) && param.p < 8)", (addr & 0xf));
            }
            return;
            
        case 3:
            if (data == 0x07 && ((ret.control & 0x1) == true)) {
                ret.status = 0;
                
                if (irq == 0xff) {
                    irq = 0;
                    return;
                }
                
                if (irq) {
                    interruptSet = 1;
                }
                
                if (reads && !result.done) {
                    printx("/// PSeudo CD write: %d (reads && !res.ok)", (addr & 0xf));
                }
                return;
            }
            
            if (data == 0x80 && ((ret.control & 0x1) == false) && readed == 0) {
                printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
            }
            return;
    }
    
    printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
}

ub CstrCD::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            if (result.done) {
                ret.control |= 0x20;
            }
            else {
                ret.control &= (~(0x20));
            }
            
            if (occupied) {
                ret.control |= 0x40;
            }
            
            ret.control |= 0x18;
            
            return CD_REG(0) = ret.control;
            
        case 1:
            if (result.done) {
                    CD_REG(1) = result.data[result.p++];
                    
                if (result.p == result.c) {
                    result.done = false;
                }
            }
            else {
                CD_REG(1) = 0;
            }
            return CD_REG(1);
            
        case 3:
            if (ret.status) {
                if (ret.control & 0x1) {
                    CD_REG(3) = ret.status | 0xe0;
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
