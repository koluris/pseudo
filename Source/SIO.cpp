#import "Global.h"


#define data\
    *(ub *)&mem.hwr.ptr[0x1040]

#define status\
    *(uw *)&mem.hwr.ptr[0x1044]

#define mode\
    *(uh *)&mem.hwr.ptr[0x1048]

#define control\
    *(uh *)&mem.hwr.ptr[0x104a]

#define baud\
    *(uh *)&mem.hwr.ptr[0x104e]

// Check for pushed button
#define btnCheck(btn)\
    if (pushed) {\
        btnState &=  (0xffff ^ (1 << btn));\
    }\
    else {\
        btnState |= ~(0xffff ^ (1 << btn));\
    }


CstrSerial sio;

void CstrSerial::reset() {
    cnt = 0;
    btnState = 0xffff;
    
    // Default pad buffer
    bfr[0] = 0x00;
    bfr[1] = 0x41;
    bfr[2] = 0x5a;
    bfr[3] = 0xff;
    bfr[4] = 0xff;
}

void CstrSerial::padListener(int code, bool pushed) {
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
        btnCheck(PAD_BTN_CIRCLE);
    }
    
    if (code ==   6) { // Z
        btnCheck(PAD_BTN_CROSS);
    }
    
    bfr[3] = (ub)(btnState);
    bfr[4] = (ub)(btnState >> 8);
}

uh CstrSerial::read16() {
    bus.interruptSet(CstrBus::INT_SIO0);
    return 0xd02b; // 0xffff
}

ub CstrSerial::read08() {
    if ((control & 0xffef) == 0x1003) { // SIO0
        if (data == 0x42 /*|| data == 0x43 || data == 0x45*/) {
            cnt = 1;
        }
        
        if (cnt) {
            ub ret = bfr[cnt];
            
            if (++cnt == sizeof(bfr)) {
                cnt = 0;
            }
            return ret;
        }
    }
    return 0;
}
