#import "Global.h"


CstrCounters rootc;

void CstrCounters::reset() {
    vbk = 0;
}

void CstrCounters::update() {
    // VBlank
    if ((vbk += PSX_CYCLE) >= PSX_VSYNC) { vbk = 0;
        data16 |= 1;
        draw.refresh();
    }
}
