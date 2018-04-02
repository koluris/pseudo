#import "Global.h"


#define GPU_COMMAND(x)\
    (x >> 24) & 0xff


CstrGraphics vs;

void CstrGraphics::reset() {
    memset(& ret, 0, sizeof(ret));
    memset(&pipe, 0, sizeof(pipe));
    
    ret.data   = 0x400;
    ret.status = 0x14802000;
}

void draw(uw addr, uw *data) {
    // Operations
    switch(addr) {
        case 0xe1: // TEXTURE PAGE
            return;
    }
    printx("GPU Data Write -> 0x%x", (GPU_COMMAND(data[0])));
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
