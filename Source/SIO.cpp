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

#define hi(btn)\
    0xffff ^ (1 << btn)


CstrSerial sio;

void CstrSerial::reset() {
    cnt = 0;
    btnState = 0xffff;
    
    // Pad Buffer
    bfr[0] = 0x00;
    bfr[1] = 0x41;
    bfr[2] = 0x5a;
    bfr[3] = 0xff;
    bfr[4] = 0xff;
}

void CstrSerial::padListener(int code, bool pushed) {
    switch(code) {
        case 7: // Cross
            if (pushed)
                btnState &=  (hi(PAD_BTN_CROSS));
            else
                btnState |= ~(hi(PAD_BTN_CROSS));
            break;
            
        case 125: // Down
            if (pushed)
                btnState &=  (hi(PAD_BTN_DOWN));
            else
                btnState |= ~(hi(PAD_BTN_DOWN));
            break;
            
        case 126: // Up
            if (pushed)
                btnState &=  (hi(PAD_BTN_UP));
            else
                btnState |= ~(hi(PAD_BTN_UP));
            break;
    }
    
    bfr[3] = (ub)(btnState);
    bfr[4] = (ub)(btnState >> 8);
}

uh CstrSerial::read16() {
    bus.interruptSet(CstrBus::IRQ_SIO0);
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
