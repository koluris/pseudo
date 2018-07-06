#import "Global.h"


CstrCD cd;

void CstrCD::reset() {
    res   = { 0 };
    param = { 0 };
    
    ctrl   = 0;
    status = 0;
    order  = 0;
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            if (!data) {
                res.ok    = false;
                param.ptr = 0;
            }
            
            if (data & 1) {
                res.ok = true;
                
                if (data == 1) {
                    order = 1;
                }
            }
            return;
            
        case 1:
            status = 2;
            
            switch(data) {
                case 1: // nop
                    status      = 3;
                    res.data[0] = status;
                    res.size    = 1;
                    break;
            }
            
            res.ok  = true;
            res.ptr = 0;
            bus.interruptSet(CstrBus::INT_CD);
            return;
            
        case 2:
            if (data == 7) {
                if (order == 2) {
                    res.ok    = true;
                    param.ptr = 0;
                    order     = 0;
                    status    = 0;
                    ctrl      = 0;
                    return;
                }
            }
            order = 0;
            
            if (param.ptr < 8) {
                param.data[param.ptr++] = data;
            }
            return;
            
        case 3:
            if (data == 7) {
                order = (order == 1) ? 2 : 0;
                
                if (res.ok) {
                    res.ptr  = 0;
                    res.size = 0;
                    status   = 0;
                }
            }
            return;
    }
    
    printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
}

ub CstrCD::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            {
                ub ret = ctrl;
                
                if (res.ok && res.size) {
                    ret |=  (0x20);
                }
                else {
                    ret &= ~(0x20);
                }
                
                return ret | 0x18;
            }
            
        case 1:
            {
                ub ret = 0;
                
                if (res.size) {
                    ret = res.data[res.ptr++];
                    
                    if (res.ptr >= res.size) {
                        res.ok   = false;
                        res.ptr  = 0;
                        res.size = 0;
                    }
                }
                
                return ret;
            }
            
        case 3:
            return status;
    }
    
    printx("/// PSeudo CD read: %d", (addr & 0xf));
    return 0;
}
