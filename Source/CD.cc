#import "Global.h"


CstrCD cd;

void CstrCD::reset() {
}

void CstrCD::write(uw addr, ub data) {
    switch(addr & 0xf) {
        case 0:
            // 1 & 2
            return;
            
        case 2:
            // 0x80
            return;
    }
}

// PSeudo Hardware Write 08: $1f801800 <- $1
// PSeudo Hardware Write 08: $1f801800 <- $2
// PSeudo Hardware Write 08: $1f801802 <- $80
// PSeudo Hardware Read  08: $1f801803
