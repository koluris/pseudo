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

#define startRead(kind) \
    reads = kind; \
    readed = 0xff; \
    interruptQueue(READ_ACK)

#define stopRead() \
    if (reads) { \
        reads = 0; \
    }


CstrCD cd;

void CstrCD::reset() {
    ret      = { 0 };
    param    = { 0 };
    result   = { 0 };
    sector   = { 0 };
    transfer = { 0 };
    
    irq = 0;
    interruptSet = 0;
    interruptReadSet = 0;
    reads  = 0;
    readed = 0;
    
    occupied = false;
    seeked   = false;
}

void CstrCD::update() {
    if (interruptSet) {
        if (interruptSet++ >= 16) {
            interruptSet = 0;
            interrupt();
        }
    }
    
    if (interruptReadSet) {
        if (interruptReadSet++ >= 1024) {
            interruptReadSet = 0;
            interruptRead();
        }
    }
}

void CstrCD::interruptQueue(ub code) {
    irq = code;
    
    if (!ret.status) {
        interruptSet = 1;
    }
}

void CstrCD::trackRead() {
    sector.prev[0] = INT2BCD(sector.data[0]);
    sector.prev[1] = INT2BCD(sector.data[1]);
    sector.prev[2] = INT2BCD(sector.data[2]);
    
    disc.trackRead(sector.prev);
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
            
        case CdlSetloc:
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x2;
            result.data[0] = ret.statp;
            break;
            
        case CdlPause:
            setResultSize(1);
            result.data[0] = ret.statp;
            ret.status = CD_STAT_ACKNOWLEDGE;
            interruptQueue(CdlPause + 0x20);
            ret.control |= 0x80;
            break;
            
        case CdlPause + 0x20:
            setResultSize(1);
            ret.statp &= (~(0x20));
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            ret.status = CD_STAT_COMPLETE;
            break;
            
        case CdlInit:
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp = 0x02;
            result.data[0] = ret.statp;
            interruptQueue(CdlInit + 0x20);
            break;
            
        case CdlInit + 0x20:
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            result.data[0] = ret.statp;
            break;
            
        case CdlDemute:
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case CdlSetmode:
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case CdlSeekL:
            setResultSize(1);
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            ret.statp |= 0x40;
            ret.status = CD_STAT_ACKNOWLEDGE;
            seeked = true;
            interruptQueue(CdlSeekL + 0x20);
            break;
            
        case CdlSeekL + 0x20:
            setResultSize(1);
            ret.statp |= 0x02;
            ret.statp &= (~(0x40));
            result.data[0] = ret.statp;
            ret.status = CD_STAT_COMPLETE;
            break;
            
        case READ_ACK:
            if (!reads) {
                return;
            }

            setResultSize(1);
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            
            if (seeked == false) {
                seeked = true;
                ret.statp |= 0x40;
            }
            
            ret.statp |= 0x20;
            ret.status = CD_STAT_ACKNOWLEDGE;

            trackRead(); // ?
            interruptReadSet = 1;
            break;
            
        default:
            printx("/// PSeudo CD irqCache: %d", irqCache);
            break;
    }
    
    if (ret.status != CD_STAT_NO_INTR && ret.re2 != 0x18) {
        bus.interruptSet(CstrBus::INT_CD);
    }
}

void CstrCD::interruptRead() {
    if (!reads) {
        return;
    }
    
    if (ret.status) {
        interruptReadSet = 1;
        return;
    }
    
    occupied = true;
    setResultSize(1);
    ret.statp |= 0x22;
    ret.statp &= (~(0x40));
    result.data[0] = ret.statp;
    
    trackRead();
    memcp(transfer.data, disc.bfr, CstrDisc::UDF_DATASIZE);
    ret.status = CD_STAT_DATA_READY;
    
    sector.data[2]++;
    if (sector.data[2] == 75) {
        sector.data[2] = 0;
      
        sector.data[1]++;
        if (sector.data[1] == 60) {
            sector.data[1] = 0;
            sector.data[0]++;
        }
    }
    readed = 0;
    
    if ((transfer.data[4 + 2] & 0x80) && (ret.mode & 0x02)) {
        interruptQueue(CdlPause);
    }
    else {
        interruptReadSet = 1;
    }
    
    bus.interruptSet(CstrBus::INT_CD);
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            ret.control = data | (ret.control & (~(0x03)));
            
            if (!data) {
                param.p = 0;
                param.c = 0;
                result.done = false;
            }
            return;
            
        case 1:
            occupied = false;
            
            if (ret.control & 0x01) {
                return;
            }
            
            switch(data) {
                case CdlNop: // TODO: More
                    defaultCtrlAndStat();
                    break;
                    
                case CdlSetloc:
                    stopRead();
                    seeked = false;
                    for (int i = 0; i < 3; i++) {
                        sector.data[i] = BCD2INT(param.data[i]);
                    }
                    sector.data[3] = 0;
                    defaultCtrlAndStat();
                    break;
                    
                case CdlReadN:
                    irq = 0;
                    stopRead();
                    ret.control |= 0x80;
                    ret.status = CD_STAT_NO_INTR;
                    startRead(1);
                    break;
                    
                case CdlPause:
                    stopRead();
                    defaultCtrlAndStat();
                    break;
                    
                case CdlInit:
                    stopRead();
                    defaultCtrlAndStat();
                    break;
                    
                case CdlDemute:
                    defaultCtrlAndStat();
                    break;
                    
                case CdlSetmode:
                    ret.mode = param.data[0];
                    defaultCtrlAndStat();
                    break;
                    
                case CdlSeekL:
                    defaultCtrlAndStat();
                    break;
                    
                default:
                    printx("/// PSeudo CD write: %d <- %d", (addr & 0xf), data);
                    break;
            }
            
            if (ret.status != CD_STAT_NO_INTR) {
                bus.interruptSet(CstrBus::INT_CD);
            }
            return;
            
        case 2:
            if (ret.control & 0x01) {
                switch (data) {
                    case 7:
                        ret.control &= (~(0x03));
                        param.p = 0;
                        param.c = 0;
                        result.done = true;
                        return;
                        
                    default:
                        ret.re2 = data;
                        return;
                }
            }
            else if (!(ret.control & 0x01) && param.p < 8) {
                param.data[param.p++] = data;
                param.c++;
            }
            return;
            
        case 3:
            if (data == 0x07 && ((ret.control & 0x01) == true)) {
                ret.status = 0;
                
                if (irq == 0xff) {
                    irq = 0;
                    return;
                }
                
                if (irq) {
                    interruptSet = 1;
                }
                
                if (reads && !result.done) {
                    interruptReadSet = 1;
                }
                return;
            }
            
            if (data == 0x80 && ((ret.control & 0x01) == false) && !readed) {
                readed = 1;
                transfer.p = 0;

                switch (ret.mode & 0x30) {
                    case 0x00:
                        transfer.p += 12;
                        return;
                        
                    case 0x20:
                        return;
                        
                    default:
                        printx("/// PSeudo CD write: %d switch 0x%x", (addr & 0xf), (ret.mode & 0x30));
                        return;
                }
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
            CD_REG(1) = 0;
            
            if (result.done) {
                CD_REG(1) = result.data[result.p++];
                
                if (result.p == result.c) {
                    result.done = false;
                }
            }
            
            return CD_REG(1);
            
        case 3:
            CD_REG(3) = 0;
            
            if (ret.status) {
                if (ret.control & 0x01) {
                    CD_REG(3) = ret.status | 0xe0;
                }
                else {
                    printx("/// PSeudo CD read: haha %d", 2);
                    CD_REG(3) = 0xff;
                }
            }
            
            return CD_REG(3);
    }
    
    printx("/// PSeudo CD read: %d", (addr & 0xf));
    return 0;
}

void CstrCD::executeDMA(CstrBus::castDMA *dma) {
    uw size = (dma->bcr & 0xffff) * 4;
    
    switch(dma->chcr) {
        case 0x11000000:
            if (!readed) {
                break;
            }
            
            for (int i = 0; i < size; i++) {
                mem.ram.ptr[(dma->madr + i) & (mem.ram.size - 1)] = transfer.data[transfer.p + i];
            }
            
            transfer.p += size;
            return;
        
        default:
            printx("/// PSeudo CD DMA: 0x%08x\n", dma->chcr);
            return;
    }
}
