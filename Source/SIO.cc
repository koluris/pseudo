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
    status   = SIO_STAT_TX_READY | SIO_STAT_TX_EMPTY;
    index    = 0;
    padst    = 0;
    btnState = 0xffff;
    
    // Default pad buffer
    memset(&bfr, 0, sizeof(bfr));
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
            control = data & (~(SIO_CTRL_RESET_ERROR));
            
            if (control & SIO_CTRL_RESET || !control) {
                status = SIO_STAT_TX_READY | SIO_STAT_TX_EMPTY;
                index  = 0;
                padst  = 0;
            }
            return;
    }
    
    accessMem(mem.hwr, uh) = data;
}

void CstrSerial::write08(uw addr, ub data) {
    switch(LOW_BITS(addr)) {
        case 0x1040:
            {
                switch(padst) {
                    case 1:
                        if (data & 0x40) {
                            index = 1;
                            padst = 2;
                            
                            switch(data) {
                                case 0x42:
                                    bfr[1] = 0x41;
                                    break;
                                    
                                case 0x43:
                                    bfr[1] = 0x43;
                                    break;
                            }
                        }
                        else {
                            padst = 0;
                        }
                        
                        bus.interruptSet(CstrBus::INT_SIO0);
                        return;
                        
                    case 2:
                        if (++index == 5) {
                            padst = 0;
                            return;
                        }
                        
                        bus.interruptSet(CstrBus::INT_SIO0);
                        return;
                }
                
                if (data == 1) {
                    status &=!SIO_STAT_TX_EMPTY;
                    status |= SIO_STAT_RX_READY;
                    index = 0;
                    padst = 1;
                    
                    if (control == 0x1003) {
                        bus.interruptSet(CstrBus::INT_SIO0);
                    }
                }
            }
            return;
    }
    
    accessMem(mem.hwr, ub) = data;
}

uh CstrSerial::read16(uw addr) {
    return accessMem(mem.hwr, uh);
}

ub CstrSerial::read08(uw addr) {
    switch(LOW_BITS(addr)) {
        case 0x1040:
            {
                if (!(status & SIO_STAT_RX_READY) || control == 0x3003) {
                    return 0;
                }
                
                ub data = bfr[index];
                
                if (index == 5) {
                    status &= (~(SIO_STAT_RX_READY));
                    status |= SIO_STAT_TX_EMPTY;
                }
                
                return data;
            }
    }
    
    return accessMem(mem.hwr, ub);
}
