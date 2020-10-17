#include "Global.h"


CstrMotionDecoder mdec;

void CstrMotionDecoder::reset() {
    cmd    = 0;
    status = 0;
}

void CstrMotionDecoder::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case 0:
            cmd = data;
            return;
            
        case 4:
            if (data & 0x80000000) {
                reset();
            }
            return;
    }
    
    printx("/// PSeudo MDEC write: 0x%08x <- 0x%08x\n", addr, data);
}

uw CstrMotionDecoder::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            return cmd;
            
        case 4:
            return status;
    }
    
    printx("/// PSeudo MDEC read: 0x%08x\n", addr);
    return 0;
}
