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
    
    GLViewport(0, 0, 320, 240);
    GLMatrixMode(GL_MODELVIEW);
    GLID();
    GLClearColor(0, 0, 0, 0);
    GLClear(GL_COLOR_BUFFER_BIT);
    resize(320, 240);
    GLFlush();
}

void CstrGraphics::resize(uh resX, uh resY) {
    if (resX && resY) {
        GLMatrixMode(GL_PROJECTION);
        GLID();
        GLOrtho(0.0, resX, resY, 0.0, 1.0, -1.0);
    }
}

void CstrGraphics::redraw() {
    ret.status ^= GPU_ODDLINES;
    GLFlush();
}

template <class T>
void drawF(uw *f, ub size, GLenum mode) {
    T *k = (T *)f;
    
    GLColor4ub(k->co.c, k->co.m, k->co.k, 255);
    
    GLStart(mode);
    for (sw i=0; i<size; i++) {
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

template <class T>
void drawG(uw *f, ub size, GLenum mode) {
    T *k = (T *)f;
    
    GLStart(mode);
    for (sw i=0; i<size; i++) {
        GLColor4ub(k->v[i].co.c, k->v[i].co.m, k->v[i].co.k, 255);
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

#define COLOR_MAX   255
#define COLOR_HALF  COLOR_MAX>>1

template <class T>
void drawFT(uw *f, ub size) {
    T *k = (T *)f;
    
    if (k->co.n&1) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, 255);
    }
    else {
        GLColor4ub(k->co.c, k->co.m, k->co.k, 255);
    }
    
    GLStart(GL_TRIANGLE_STRIP);
    for (sw i=0; i<size; i++) {
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

template <class T>
void drawGT(uw *f, ub size) {
    T *k = (T *)f;
    
    GLStart(GL_TRIANGLE_STRIP);
    for (sw i=0; i<size; i++) {
        GLColor4ub(k->v[i].co.c, k->v[i].co.m, k->v[i].co.k, 255);
        GLVertex2s(k->v[i].w, k->v[i].h);
    }
    GLEnd();
}

void CstrGraphics::draw(uw addr, uw *data) {
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
            drawF<F3>(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x24: // TODO: Vertex FT3
        case 0x25:
        case 0x26:
        case 0x27:
            drawFT<FT3>(data, 3);
            return;
            
        case 0x28: // TODO: Vertex F4
        case 0x29:
        case 0x2a:
            drawF<F4>(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x2c: // TODO: Vertex FT4
        case 0x2d:
        case 0x2e:
            drawFT<FT4>(data, 4);
            return;
            
        case 0x30: // TODO: Vertex G3
        case 0x31:
        case 0x32:
        case 0x33:
            drawG<G3>(data, 3, GL_TRIANGLE_STRIP);
            return;
            
        case 0x34: // TODO: Vertex GT3
        case 0x35:
        case 0x36:
        case 0x37:
            drawGT<GT3>(data, 3);
            return;
            
        case 0x38: // TODO: Vertex G4
        case 0x39:
        case 0x3a:
            drawG<G4>(data, 4, GL_TRIANGLE_STRIP);
            return;
            
        case 0x3c: // TODO: Vertex GT4
        case 0x3d:
        case 0x3e:
            drawGT<GT4>(data, 4);
            return;
            
        case 0x40: // TODO: Line F2
        case 0x41:
        case 0x42:
        case 0x43:
            drawF<F2>(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x49: // TODO: Line F3
        case 0x4a:
            drawF<F3>(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x4c: // TODO: Line F4
        case 0x4e:
            drawF<F4>(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x50: // TODO: Line G2
        case 0x51:
        case 0x52:
        case 0x53:
            drawG<G2>(data, 2, GL_LINE_STRIP);
            return;
            
        case 0x59: // TODO: Line G3
        case 0x5a:
            drawG<G3>(data, 3, GL_LINE_STRIP);
            return;
            
        case 0x5c: // TODO: Line G4
        case 0x5e:
        case 0x5f:
            drawG<G4>(data, 4, GL_LINE_STRIP);
            return;
            
        case 0x60: // TODO: Tile
        case 0x61:
        case 0x62:
        case 0x63:
            return;
            
        case 0x64: // TODO: Sprite
        case 0x65:
        case 0x66:
        case 0x67:
            return;
            
        case 0x69: // TODO: Tile 1
        case 0x6b:
            return;
            
        case 0x71: // TODO: Tile 8
        case 0x72:
        case 0x73:
            return;
            
        case 0x74: // TODO: Sprite 8
        case 0x75:
        case 0x76:
        case 0x77:
            return;
            
        case 0x79: // TODO: Tile 16
        case 0x7b:
            return;
            
        case 0x7d: // TODO: Sprite 16
        case 0x7f:
            return;
            
        case 0x80: // TODO: Move photo
            return;
            
        case 0xa0: // TODO: Load photo
            return;
            
        case 0xc0: // TODO: Store photo
            return;
            
        case 0xe1: // TODO: Texture P.
            return;
            
        case 0xe2: // TODO: Texture Window
            return;
            
        case 0xe3: // TODO: Draw Area Start
            return;
            
        case 0xe4: // TODO: Draw Area End
            return;
            
        case 0xe5: // TODO: Draw Offset
            return;
            
        case 0xe6: // TODO: STP
            ret.status = (ret.status & (~(3 << 11))) | ((data[0] & 3) << 11);
            return;
    }
    printx("PSeudo /// GPU Draw -> $%x", addr);
}

void CstrGraphics::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case GPU_REG_DATA:
            dataWrite(&data, 1);
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
    sw i = 0;
    
    while (i < size) {
        ret.data = *ptr;
        ptr++;
        i++;
        
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
}

void CstrGraphics::executeDMA(CstrBus::castDMA *dma) {
    uw *p = (uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)], size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    
    switch(dma->chcr) {
        case 0x00000401: // Disable DMA?
            return;
            
        case 0x01000201:
            dataWrite(p, size);
            return;
            
        case 0x01000401:
            do {
                uw hdr = *(uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
                p = (uw *)&mem.ram.ptr[(dma->madr + 4) & 0x1ffffc];
                dataWrite(p, hdr >> 24);
                dma->madr = hdr & 0xffffff;
            }
            while (dma->madr != 0xffffff);
            return;
    }
    printx("PSeudo /// GPU DMA: $%x", dma->chcr);
}
