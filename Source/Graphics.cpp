#import "Global.h"


CstrGraphics vs;

#define GPU_COMMAND(x)\
    (x >> 24) & 0xff

void CstrGraphics::reset() {
    ret.data   = 0x400;
    ret.status = 0x14802000;
}

void CstrGraphics::dataMemWrite(uw *ptr, sw size) {
    ret.data = *ptr;
}

void CstrGraphics::dataWrite(uw data) {
    dataMemWrite(&data, 1);
}

void CstrGraphics::statusWrite(uw data) {
    switch(GPU_COMMAND(data)) {
    }
    printx("GPU Status Write -> 0x%x", (GPU_COMMAND(data)));
}

uw CstrGraphics::statusRead() {
    return ret.status;
}
