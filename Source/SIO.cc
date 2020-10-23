/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


#define status \
    *(uw *)&mem.hwr.ptr[0x1044]

#define mode \
    *(uh *)&mem.hwr.ptr[0x1048]

#define control \
    *(uh *)&mem.hwr.ptr[0x104a]

#define baud \
    *(uh *)&mem.hwr.ptr[0x104e]

// Check for pushed button
#define btnCheck(btn) \
    if (pushed) { \
        btnState &=  (0xffff ^ (1 << btn)); \
    } \
    else { \
        btnState |= ~(0xffff ^ (1 << btn)); \
    }


CstrSerial sio;

void CstrSerial::reset() {
    memset(&bfr, 0, sizeof(bfr));
    btnState = 0xffff;
    status   = SIO_STAT_TX_READY | SIO_STAT_TX_EMPTY;
    padst    = 0;
    parp     = 0;
    
    // Default pad buffer
    bfr[0] = 0x00;
    bfr[1] = 0x41;
    bfr[2] = 0x5a;
    bfr[3] = 0xff;
    bfr[4] = 0xff;
}

void CstrSerial::padListener(int code, bool pushed) {
#ifdef APPLE_MACOS
    if (code ==  19) { // Select
        btnCheck(PAD_BTN_SELECT);
    }
    
    if (code ==  18) { // Start
        btnCheck(PAD_BTN_START);
    }
    
    if (code == 126) { // Up
        btnCheck(PAD_BTN_UP);
    }
    
    if (code == 124) { // R
        btnCheck(PAD_BTN_RIGHT);
    }
    
    if (code == 125) { // Down
        btnCheck(PAD_BTN_DOWN);
    }
    
    if (code == 123) { // Left
        btnCheck(PAD_BTN_LEFT);
    }
    
    if (code ==   7) { // X
        btnCheck(PAD_BTN_CROSS);
    }
    
    if (code ==   6) { // Z
        btnCheck(PAD_BTN_CIRCLE);
    }
#endif
    
    bfr[3] = (ub)(btnState);
    bfr[4] = (ub)(btnState >> 8);
}

void CstrSerial::write16(uw addr, uh data) {
    switch(LOW_BITS(addr)) {
        case 0x104a:
            control = data;
            
            if (control & SIO_CTRL_RESET_ERROR) {
                status  &= (~(SIO_STAT_IRQ));
                control &= (~(SIO_CTRL_RESET_ERROR));
            }
            
            if (control & SIO_CTRL_RESET || !control) {
                status = SIO_STAT_TX_READY | SIO_STAT_TX_EMPTY;
                padst  = 0;
                parp   = 0;
            }
            return;
            
        default:
            accessMem(mem.hwr, uh) = data;
            return;
    }
    
    printx("/// PSeudo SIO write16 0x%x <- 0x%x", LOW_BITS(addr), data);
}

void CstrSerial::write08(ub data) {
    switch(padst) {
        case 1:
            if (data & 0x40) {
                padst = 2;
                parp  = 1;
                
                switch(data) {
                    case 0x42:
                        bfr[1] = 0x41;
                        break;
                        
                    case 0x43:
                        bfr[1] = 0x43;
                        break;
                        
                    default:
                        printx("/// PSeudo SIO write08 data == 0x%0x", data);
                        break;
                }
            }
            else {
                padst = 0;
            }
            
            bus.interruptSet(CstrBus::INT_SIO0);
            return;
            
        case 2:
            if (++parp != 5) {
                bus.interruptSet(CstrBus::INT_SIO0);
            }
            else {
                padst = 0;
            }
            return;
    }
    
    if (data == 1) {
        status &=!SIO_STAT_TX_EMPTY;
        status |= SIO_STAT_RX_READY;
        padst = 1;
        parp  = 0;
        
        if (control & SIO_CTRL_DTR) {
            switch (control) {
                case 0x1003:
                    bus.interruptSet(CstrBus::INT_SIO0);
                    break;
                    
                default:
//#ifdef DEBUG
//                    printf("/// PSeudo : [-] SIO control -> 0x%08x\n", control);
//                    exit(0);
//#endif
                    break;
            }
        }
    }
}

uh CstrSerial::read16(uw addr) {
    return accessMem(mem.hwr, uh);
}

ub CstrSerial::read08() {
    if (!(status & SIO_STAT_RX_READY)) {
        return 0;
    }
    
    ub data = bfr[parp];
    
    if (parp == 5) {
        status &= (~(SIO_STAT_RX_READY));
        status |= SIO_STAT_TX_EMPTY;
        
        if (padst == 2) {
            printx("/// PSeudo SIO read08 (padst == %d)", 2);
        }
    }
    
    return data;
}
