#import "Global.h"


#define GPU_COMMAND(x)\
    (x >> 24) & 0xff


CstrGraphics vs;

void CstrGraphics::reset() {
    ret.data   = 0x400;
    ret.status = 0x14802000;
    
    // Command buffer
    memset(pipe.data, 0, sizeof(pipe.data));
    pipe.prim = 0;
    pipe.size = 0;
    pipe.row  = 0;
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
    }
    else {
        pipe.data[pipe.row] = ret.data;
        pipe.row++;
    }
    
    if (pipe.size == pipe.row) {
        pipe.size = 0;
        pipe.row  = 0;
        
        printx("GPU Data Write -> 0x%x", (GPU_COMMAND(ret.data)));
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
