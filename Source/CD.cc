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
    interruptQueue(6)

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
        if (interruptReadSet++ >= 512) {
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
        case  1: // CdlNop
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            break;
            
        case  2: // CdlSetloc
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case  3: // CdlAudio
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            ret.statp |= 0x80;
            break;
            
        case  6: // CdlReadN
            if (!reads) {
                return;
            }
            
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            ret.statp |= 0x20;
            
            if (seeked == false) {
                seeked = true;
                ret.statp |= 0x40;
            }
            interruptReadSet = 1;
            break;
            
        case  7: // CdlIdle
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case  8: // CdlStop
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            ret.statp &= (~(0x2));
            result.data[0] = ret.statp;
            break;
            
        case  9: // CdlPause
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.control |= 0x80;
            result.data[0] = ret.statp;
            interruptQueue(irqCache + 0x20);
            break;
            
        case  9 + 0x20:
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            ret.statp |= 0x02;
            ret.statp &= (~(0x20));
            result.data[0] = ret.statp;
            break;
            
        case 10: // CdlInit
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp = 0x02;
            result.data[0] = ret.statp;
            interruptQueue(irqCache + 0x20);
            break;
            
        case 10 + 0x20:
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            result.data[0] = ret.statp;
            break;
            
        case 11: // CdlMute
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case 12: // CdlDemute
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case 13: // CdlSetfilter
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case 14: // CdlSetmode
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            break;
            
        case 15: // CdlGetmode
            setResultSize(6);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            result.data[1] = ret.mode;
            result.data[2] = ret.file;
            result.data[3] = ret.channel;
            result.data[4] = 0;
            result.data[5] = 0;
            break;
            
        case 16: // CdlGetlocL
            setResultSize(8);
            ret.status = CD_STAT_ACKNOWLEDGE;
            for (int i = 0; i < 8; i++) {
                result.data[i] = transfer.data[i];
            }
            break;
            
        case 17: // CdlGetlocP
            setResultSize(8);
            ret.status = CD_STAT_ACKNOWLEDGE;
            result.data[0] = 1;
            result.data[1] = 1;
            result.data[2] = sector.prev[0];
            result.data[3] = INT2BCD((BCD2INT(sector.prev[1])) - 2);
            result.data[4] = sector.prev[2];
            memcp(result.data + 5, sector.prev, 3);
            break;
            
        case 19: // CdlGetTN
            setResultSize(3);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            disc.fetchTN(result.tn);
            result.data[1] = INT2BCD(result.tn[0]);
            result.data[2] = INT2BCD(result.tn[1]);
            break;
            
        case 20: // CdlGetTD
            setResultSize(4);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            disc.fetchTD(result.td);
            result.data[1] = INT2BCD(result.td[2]);
            result.data[2] = INT2BCD(result.td[1]);
            result.data[3] = INT2BCD(result.td[0]);
            break;
            
        case 21: // CdlSeekL
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            ret.statp |= 0x40;
            interruptQueue(irqCache + 0x20);
            seeked = true;
            break;
            
        case 21 + 0x20:
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            ret.statp |= 0x02;
            ret.statp &= (~(0x40));
            result.data[0] = ret.statp;
            break;
            
        case 22: // CdlSeekP
            setResultSize(1);
            ret.status = CD_STAT_ACKNOWLEDGE;
            ret.statp |= 0x02;
            result.data[0] = ret.statp;
            ret.statp |= 0x40;
            interruptQueue(irqCache + 0x20);
            break;
            
        case 22 + 0x20:
            setResultSize(1);
            ret.status = CD_STAT_COMPLETE;
            ret.statp |= 0x02;
            ret.statp &= (~(0x40));
            result.data[0] = ret.statp;
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
        interruptQueue(9); // CdlPause
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
                case  7: // CdlIdle
                case  8: // CdlStop
                case  9: // CdlPause
                case 10: // CdlInit
                    stopRead();
                    
                case  1: // CdlNop
                case  3: // CdlAudio
                case 11: // CdlMute
                case 12: // CdlDemute
                case 15: // CdlGetmode
                case 16: // CdlGetlocL
                case 17: // CdlGetlocP
                case 19: // CdlGetTN
                case 20: // CdlGetTD
                case 21: // CdlSeekL
                case 22: // CdlSeekP
                    defaultCtrlAndStat();
                    break;
                    
                case  2:  // CdlSetloc
                    stopRead();
                    defaultCtrlAndStat();
                    seeked = false;
                    for (int i = 0; i < 3; i++) {
                        sector.data[i] = BCD2INT(param.data[i]);
                    }
                    sector.data[3] = 0;
                    break;
                    
                case  6: // CdlReadN
                case 27: // CdlReadS
                    stopRead();
                    irq = 0;
                    ret.status = CD_STAT_NO_INTR;
                    ret.control |= 0x80;
                    startRead(1);
                    break;
                    
                case 13: // CdlSetfilter
                    ret.file    = param.data[0];
                    ret.channel = param.data[1];
                    defaultCtrlAndStat();
                    break;
                    
                case 14: // CdlSetmode
                    ret.mode = param.data[0];
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
            
        case 2:
            if (!readed) {
                printx("/// PSeudo CD read: %d", 2);
                return 0;
            }
            return transfer.data[transfer.p++];
            
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
        case 0x11400100: // ?
            if (!readed) {
                return;
            }
            
            for (int i = 0; i < size; i++) {
                mem.ram.ptr[(dma->madr + i) & (mem.ram.size - 1)] = transfer.data[transfer.p + i];
            }
            
            transfer.p += size;
            return;
            
        case 0x00000000: // ?
            return;
            
        default:
            printx("/// PSeudo CD DMA: 0x%08x\n", dma->chcr);
            return;
    }
}
