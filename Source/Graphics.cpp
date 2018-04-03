#import "Global.h"


#define GPU_DITHER           0x00000200
#define GPU_DRAWINGALLOWED   0x00000400
#define GPU_MASKDRAWN        0x00000800
#define GPU_MASKENABLED      0x00001000
#define GPU_WIDTHBITS        0x00070000
#define GPU_DOUBLEHEIGHT     0x00080000
#define GPU_PAL              0x00100000
#define GPU_RGB24            0x00200000
#define GPU_INTERLACED       0x00400000
#define GPU_DISPLAYDISABLED  0x00800000
#define GPU_IDLE             0x04000000
#define GPU_READYFORVRAM     0x08000000
#define GPU_READYFORCOMMANDS 0x10000000
#define GPU_DMABITS          0x60000000
#define GPU_ODDLINES         0x80000000

#define GPU_COMMAND(x)\
    (x >> 24) & 0xff


CstrGraphics vs;

void CstrGraphics::reset() {
    memset(& ret, 0, sizeof(ret));
    memset(&pipe, 0, sizeof(pipe));
    
    ret.data   = 0x400;
    ret.status = 0x14802000;
}

void CstrGraphics::redraw() {
    ret.status ^= GPU_ODDLINES;
}

void draw(uw addr, uw *data) {
    // Operations
    switch(addr) {
        case 0xe1: // TODO: TEXTURE PAGE
            return;
    }
    printx("GPU Data Write -> 0x%x", addr);
}

void CstrGraphics::dataMemWrite(uw *ptr, sw size) {
    ret.data = *ptr;
    ptr++;
    
    if (!pipe.size) {
        ub prim  = GPU_COMMAND(ret.data);
        ub count = pSize[prim];
        
        if (count) {
            pipe.data[0] = ret.data;
            pipe.prim = prim;
            pipe.size = count;
            pipe.row  = 1;
        }
        else {
            return;
        }
    }
    else {
        pipe.data[pipe.row] = ret.data;
        pipe.row++;
    }
    
    if (pipe.size == pipe.row) {
        pipe.size = 0;
        pipe.row  = 0;
        
        draw(pipe.prim, pipe.data);
    }
}

void CstrGraphics::dataWrite(uw data) {
    dataMemWrite(&data, 1); // TODO: Sizes > 1
}

void CstrGraphics::statusWrite(uw data) {
    switch(GPU_COMMAND(data)) {
    }
    printx("GPU Status Write -> 0x%x", (GPU_COMMAND(data)));
}

uw CstrGraphics::statusRead() {
    ret.status |=  GPU_READYFORVRAM;
    ret.status &= ~GPU_DOUBLEHEIGHT;
    
    return ret.status;
}
