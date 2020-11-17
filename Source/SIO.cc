/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


// Check for pushed button
#define btnCheck(btn) \
    if (pushed) { \
        btnState &= ( (0xffff ^ (1 << btn))); \
    } \
    else { \
        btnState |= (~(0xffff ^ (1 << btn))); \
    }


CstrSerial sio;

void CstrSerial::reset() {
    rx.enabled = false;
    rx.data    = 0;
    
    btnState = 0xffff;
    index    = 0;

    bfr[0] = 0xff;
    bfr[1] = 0x41;
    bfr[2] = 0x5a;
    bfr[3] = 0xff;
    bfr[4] = 0xff;
}

void CstrSerial::pollController(ub data) {
    if ((index == 0 && data != 0x01) || (index == 1 && data != 0x42)) {
        return;
    }
    
    rx.data = bfr[index];
    
    if (++index == sizeof(bfr)) {
        index = 0;
    }
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
    accessMem(mem.hwr, uh) = data;
}

void CstrSerial::write08(uw addr, ub data) {
    switch(LOW_BITS(addr)) {
        case 0x1040:
            rx.enabled = true;
            pollController(data);
            bus.interruptSet(CstrBus::INT_SIO0);
            return;
    }

    accessMem(mem.hwr, ub) = data;
}

uh CstrSerial::read16(uw addr) {
    switch(LOW_BITS(addr)) {
        case 0x1044:
            return 0b101 | (rx.enabled << 1);
    }
    
    return accessMem(mem.hwr, uh);
}

ub CstrSerial::read08(uw addr) {
    switch(LOW_BITS(addr)) {
        case 0x1040:
            if (rx.enabled) {
                rx.enabled = false;
                return rx.data;
            }
            return 0xff;
    }
    
    return accessMem(mem.hwr, ub);
}
