#import "Global.h"


#define cdrAcc(addr) \
    mem.hwr.ptr[0x1800 | addr]


CstrCD cd;

void CstrCD::reset() {
    res   = { 0 };
    param = { 0 };
    
    stat     = 0;
    order    = 0;
    occupied = false;
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            if (!data) {
                param.p = 0;
                param.c = 0;
                res.ok  = false;
            }
            else if (data & 1) {
                res.ok = true;
                
                if (data == 1) {
                    order = 1;
                }
            }
            return;
            
        case 1:
            stat = 2;
            
            switch(data) {
                case 1: // nop
                    res.ok      = true;
                    res.p       = 0;
                    res.c       = 1;
                    res.data[0] = 0x02;
                    stat        = 2;
                    occupied    = true;
                    break;
                    
                case 10: // init
                    //mode      = 0;
                    res.ok      = true;
                    res.p       = 0;
                    res.c       = 1;
                    res.data[0] = 0x22;
                    stat        = 2;
                    occupied    = false;
                    break;
                    
                case 12: // demute
                    res.ok      = true;
                    res.p       = 0;
                    res.c       = 1;
                    res.data[0] = 0x02;
                    stat        = 2;
                    occupied    = false;
                    break;
                    
                case 128: // ?
                    stat = 2;
                    break;
                    
                default:
                    printx("/// PSeudo CD write 1: %d", data);
                    break;
            }
            
            bus.interruptSet(CstrBus::INT_CD);
            return;

        case 2: // 0x80
            if (data == 7 && order == 2) {
                res.ok  = false;
                param.p = 0;
                param.c = 0;
                order   = 0;
                stat    = 0;
                mem.hwr.ptr[0x1800] = 0;
                return;
            }
            
            order = 0;
            param.data[param.p++] = data;
            param.c++;
            
            if (param.p > 7) {
                printx("/// PSeudo CD write 2 param.p > %d", 7);
            }
            return;

        case 3: // 0x00
            if (data == 7 && order == 1) {
                order = 2;
            }
            else {
                order = 0;
            }
            
            if (data == 7 && res.ok) {
                res.c = 0;
                stat  = 0;
            }
            return;
    }
    
    printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
}

ub CstrCD::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            if (occupied) {
                cdrAcc(0) |= 0x40;
            }
            else {
                cdrAcc(0) &= 0xbf;
            }
            return cdrAcc(0) | 0x18;
            
        case 3:
            return cdrAcc(3) = stat;
    }
    
    printx("/// PSeudo CD read: %d", (addr & 0xf));
    return 0;
}
