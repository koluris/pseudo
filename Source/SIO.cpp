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


CstrSerial sio;

void CstrSerial::reset() {
    cnt = 0;
    fireInterrupt = false;
    
    // Pad Buffer
    bfr[0] = 0x00;
    bfr[1] = 0x41;
    bfr[2] = 0x5a;
    bfr[3] = 0xff;
    bfr[4] = 0xff;
    
    // Hardwire (X) cross button
    const uh btn = 0x0040; // ~(0xffbf)
    bfr[3] = (ub)(btn);
    bfr[4] = (ub)(btn >> 8);
}

uh CstrSerial::read16() {
    if (fireInterrupt) {
        bus.interruptSet(CstrBus::IRQ_SIO0);
    }
    return 0xd02b; // 0xffff
}

ub CstrSerial::read08() {
    if ((control & 0xffef) == 0x1003) { // SIO0
        fireInterrupt = true;
        
        if (data == 0x42 /*|| data == 0x43 || data == 0x45*/) {
            cnt = 1;
        }

        if (cnt) {
            ub ret = bfr[cnt];
            
            if (++cnt == sizeof(bfr)) {
                cnt = 0;
                fireInterrupt = false;
            }
            return ret;
        }
    }
    return 0;
}
