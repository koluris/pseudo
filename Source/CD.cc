#import "Global.h"


CstrCD cd;

void CstrCD::reset() {
    res   = { 0 };
    param = { 0 };
    
    check  = 0;
    mode   = 0;
    status = 0;
    state  = 0;
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
                case  1: // Nop
                    res.data[0] = state;
                    res.size    = 1;
                    status      = 3;
                    break;
                    
                case  2: // TODO
                    break;
                    
                case 10: // Init
                    res.data[0] = state = (state | CD_STAT_STANDBY) & ~(CD_STAT_PLAY);
                    res.size    = 1;
                    status      = 2;
                    mode        = 0;
                    break;
                    
                case 12: // Demute
                    res.data[0] = state;
                    res.size    = 1;
                    status      = 2;
                    break;
                    
                case 14: // TODO
                    break;
                    
                case 25: // Test
                    state |= CD_STAT_STANDBY;
                    
                    switch(param.data[0]) {
                        case 0x20:
                            // Timestamp?
                            break;
                            
                        default:
                            printx("/// PSeudo CD CMD Test: 0x%x", param.data[0]);
                            break;
                    }
                    
                    res.size = 8;
                    status   = 3;
                    break;
                    
                case 26: // Check ID
                    res.data[0] = 0x00; // 0x08 insert PSX CD-ROM
                    res.data[1] = 0x80; // 0x80 into BIOS
                    res.data[2] = 0x00;
                    res.data[3] = 0x00;
                    
                    //*(uw *)(res.data + 4) = SCExID;
                    
                    res.size = 8;
                    status   = 2;
                    break;
                    
                default:
                    if (data != 128) {
                        printx("/// PSeudo CD CMD: %d", data);
                    }
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
                    check     = 0;
                    status    = 0;
                    order     = 0;
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
    ub ret = 0;
    
    switch(addr & 0xf) {
        case 0:
            ret = (check | 0x18) & ~(0x20);
            
            if (res.ok && res.size) {
                ret |= 0x20;
            }
            return ret;
            
        case 1:
            if (res.size) {
                ret = res.data[res.ptr++];
                
                if (res.ptr >= res.size) {
                    res.ok   = false;
                    res.ptr  = 0;
                    res.size = 0;
                }
            }
            return ret;
            
        case 3:
            return status;
    }
    
    printx("/// PSeudo CD read: %d", (addr & 0xf));
    return 0;
}

/*
CstrCD cd;

void CstrCD::reset() {
    ebus = { 0 };
    interrupt = { 0 };
    interrupt.status = 0xe0;
    
    check  = CD_CTRL_PH | CD_CTRL_NP;
    status = CD_STAT_STANDBY;
    kind   = CD_CMD_TYPE_BLOCKING;
}

void CstrCD::exec(ub data) {
    switch(data) {
        case 1: // nop
            ebus.res[0] = &status;
            kind = CD_CMD_TYPE_BLOCKING;
            break;
            
        default:
            printx("/// PSeudo CD exec: %d", data);
            break;
    }
    
    bus.interruptSet(CstrBus::INT_CD); // ?
    // SET_MAIN(cdrom);
    // blockEnd = 0;
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            switch(data) {
                case 0:
                    check &= ~(CD_CTRL_MODE_1 | CD_CTRL_MODE_0);
                    return;
                    
                case 1:
                    check &= ~(CD_CTRL_MODE_1);
                    check |=  (CD_CTRL_MODE_0);
                    return;
            }
            
            printx("/// PSeudo CD write 0: %d", data);
            return;
            
        case 1:
            switch(check & 0x03) {
                case 0:
                    if (!interrupt.queued) {
                        if ((interrupt.status & 0x7) || (check & CD_CTRL_BUSY)) {
                            printx("/// PSeudo CD write %d (interrupt.status & 0x7) || (check & CD_CTRL_BUSY)", 1);
                        }
                    }
                    else {
                        printx("/// PSeudo CD write %d !interrupt.queued case 2", 1);
                    }
                    
                    exec(data);
                    return;
            }
            
            printx("/// PSeudo CD write 1: %d", (check & 0x03));
            return;
            
        case 3:
            switch(check & 0x03) {
                case 0:
                    switch(data) {
                        case 0:
                            return;
                    }
                    
                    printx("/// PSeudo CD write 3, 0: 0x%x", data);
                    return;
            }
            
            printx("/// PSeudo CD write 3: %d", (check & 0x03));
            return;
    }
    
    printx("/// PSeudo CD write: %d <- 0x%x", (addr & 0xf), data);
}

ub CstrCD::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            return check;
            
        case 3:
            switch(check & 0x03) {
                case 0:
                    return 0xff;
                    
                case 1:
                    return interrupt.status;
            }
            
            printx("/// PSeudo CD read 3: %d", (check & 0x03));
            return 0;
    }
    
    printx("/// PSeudo CD read: %d", (addr & 0xf));
    return 0;
}*/
