#import "Global.h"


//#define GPU_DITHER           0x00000200
//#define GPU_DRAWINGALLOWED   0x00000400
//#define GPU_MASKDRAWN        0x00000800
//#define GPU_MASKENABLED      0x00001000
//#define GPU_WIDTHBITS        0x00070000
#define GPU_DOUBLEHEIGHT     0x00080000
//#define GPU_PAL              0x00100000
//#define GPU_RGB24            0x00200000
//#define GPU_INTERLACED       0x00400000
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
    ret.status = GPU_READYFORCOMMANDS | GPU_IDLE | GPU_DISPLAYDISABLED | 0x2000; // 0x14802000;
    modeDMA    = GPU_DMA_NONE;
}

void CstrGraphics::redraw() {
    ret.status ^= GPU_ODDLINES;
}

void resize(uh resX, uh resY) {
    if (resX && resY) {
        GLMatrixMode(GL_PROJECTION);
        GLID();
        GLOrtho(0.0, resX, resY, 0.0, 1.0, -1.0);
    }
}

void draw(uw addr, uw *data) {
    // Operations
    switch(addr) {
        case 0x01: // TODO: Flush
            return;
            
        case 0x02: // Block Fill
            return;
            
        case 0x20: // TODO: Vertex F3
        case 0x21:
        case 0x22:
        case 0x23:
            return;
            
        case 0x24: // TODO: Vertex FT3
        case 0x25:
        case 0x27:
            return;
            
        case 0x28: // TODO: Vertex F4
        case 0x29:
            return;
            
        case 0x2d: // TODO: Vertex FT4
            return;
            
        case 0x30: // TODO: Vertex G3
        case 0x31:
        case 0x32:
        case 0x33:
            return;
            
        case 0x34: // TODO: Vertex GT3
        case 0x35:
        case 0x36:
        case 0x37:
            return;
            
        case 0x39: // TODO: Vertex G4
            return;
            
        case 0x3d: // TODO: Vertex GT4
            return;
            
        case 0x40: // TODO: Line F2
        case 0x41:
        case 0x42:
        case 0x43:
            return;
            
        case 0x4a: // TODO: Line F3
            return;
            
        case 0x4e: // TODO: Line F4
            return;
            
        case 0x52: // TODO: Line G2
        case 0x53:
            return;
            
        case 0x64: // TODO: Sprite
        case 0x65:
        case 0x66:
        case 0x67:
            return;
            
        case 0x74: // TODO: Sprite 8
        case 0x75:
        case 0x76:
        case 0x77:
            return;
            
        case 0x7f: // TODO: Sprite 16
            return;
            
        case 0xa0: // TODO: LOAD IMAGE
            return;
            
        case 0xc0: // TODO: STORE IMAGE
            return;
            
        case 0xe1: // TODO: TEXTURE PAGE
            return;
            
        case 0xe3: // TODO: Draw Area Start
            return;
            
        case 0xe4: // TODO: Draw Area End
            return;
            
        case 0xe5: // TODO: Draw Offset
            return;
    }
    printx("PSeudo /// GPU Draw -> $%x", addr);
}

void CstrGraphics::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case GPU_REG_DATA:
            dataWrite(&data, 1); // TODO: Sizes > 1
            return;
            
        case GPU_REG_STATUS:
            switch(GPU_COMMAND(data)) {
                case 0x00:
                    ret.status = 0x14802000;
                    return;
                    
                case 0x01:
                    memset(&pipe, 0, sizeof(pipe));
                    return;
                    
                case 0x04:
                    modeDMA = data & 3;
                    
                    ret.status |= data << 29;
                    ret.status &= ~GPU_DMABITS;
                    return;
                    
                case 0x08:
                    resize(resMode[(data & 3) | ((data & 0x40) >> 4)], (data & 4) ? 480 : 240);
                    return;
                    
                /* unused */
                case 0x02:
                case 0x03:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x10: // TODO: Information
                    return;
            }
            printx("PSeudo /// GPU Write Status: $%x", (GPU_COMMAND(data)));
            return;
    }
    printx("PSeudo /// GPU Write: $%x <- $%x", (addr & 0xf), data);
}

uw CstrGraphics::read(uw addr) {
    switch(addr & 0xf) {
        case GPU_REG_DATA:
            return ret.data;
            
        case GPU_REG_STATUS:
            ret.status |=  GPU_READYFORVRAM;
            ret.status &= ~GPU_DOUBLEHEIGHT;
            
            return ret.status;
    }
    printx("PSeudo /// GPU Read: $%x", (addr & 0xf));
    
    return 0;
}

void CstrGraphics::dataWrite(uw *ptr, sw size) {
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

void CstrGraphics::executeDMA(CstrBus::castDMA *dma) {
    switch(dma->chcr) {
        case 0x00000401: // Disable DMA?
        case 0x01000201:
        case 0x01000401:
            return;
    }
    printx("PSeudo /// GPU DMA: $%x", dma->chcr);
}
