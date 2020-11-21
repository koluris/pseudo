/* Base structure taken from PCSX-df open source emulator, and improved upon (Credits: Stephen Chao) */

#include "Global.h"


#define GPU_COMMAND(x) \
    (x >> 24) & 0xff


CstrGraphics vs;

void CstrGraphics::reset() {
    memset(vram.ptr, 0, vram.size);
    memset(info, 0, sizeof(info));
    vrop = { 0 };
    ret  = { 0 };
    pipe = { 0 };
    
    info[GPU_INFO_VERSION] = 0x2;
    ret.data     = 0x400;
    ret.status   = GPU_STAT_READYFORCOMMANDS | GPU_STAT_IDLE | GPU_STAT_DISPLAYDISABLED | 0x2000; // 0x14802000;
    modeDMA      = GPU_DMA_NONE;
    clock        = 0;
    scanline     = 0;
    stall        = 0;
    vpos         = 0;
    vdiff        = 0;
    isDisabled   = true;
    isVideo24Bit = false;
    isVideoPAL   = false;
}

void CstrGraphics::update(uw frames) {
    uw lines = (clock += frames) / 3413;
    clock %= 3413;
    
    if ((scanline += lines) >= 262) { scanline = 0;
#if 0
        // FPS throttle
        static double then = 1.0;
        double now = mach_absolute_time() / 1000.0;
        then = now > (then + CLOCKS_PER_SEC) ? now : then + (isVideoPAL ? PAL : NTSC);
        
        if (then > now) {
            usleep(then - now);
        }
#endif
        if ((++stall % 2)) {
            draw.swapBuffers(isDisabled);
        }
        bus.interruptSet(CstrBus::INT_VSYNC);
    }
}

void CstrGraphics::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case 0: // Data
            dataWrite(&data, 1);
            return;
            
        case 4: // Status
            switch(GPU_COMMAND(data)) {
                case 0x00:
                    ret.status   = 0x14802000;
                    isDisabled   = true;
                    isVideo24Bit = false;
                    isVideoPAL   = false;
                    return;
                    
                case 0x01:
                    pipe = { 0 };
                    return;
                    
                case 0x03:
                    isDisabled = data & 1;
                    return;
                    
                case 0x04:
                    modeDMA = data & 3;
                    return;
                    
                case 0x05:
                    {
                        const uh temp = (data >> 10) & 0x1ff;
                          vpos = temp >= 200 ? temp : vpos;
                    }
                    return;
                    
                case 0x07:
                    vdiff = ((data >> 10) & 0x3ff) - (data & 0x3ff);
                    return;
                    
                case 0x08:
                    {
                        isInterlaced = data & 0x20;
                        isVideo24Bit = data & 0x10;
                        isVideoPAL   = data & 0x08;
                        
                        // Basic info
                        const uh w = resMode[(data & 3) | ((data & 0x40) >> 4)];
                        const uh h = (data & 4) ? 480 : 240;
                        
                        if (isInterlaced || h == vpos || h == vdiff) { // No distinction for interlaced or normal mode
                            draw.resize(w, h); //printf("1 %d / %d\n", w, h);
                        }
                        else { // Special case
                            if (vdiff < 240) { // pdx-059, wurst2k
                                vdiff = 240;
                            }
                            draw.resize(w, vpos ? vpos : vdiff); //printf("2 %d / %d\n", w, (vpos ? vpos : vdiff));
                        }
                    }
                    return;
                    
                case 0x10: // TODO: Information
                    switch(data & 0xffffff) {
                        case 0:
                        case 1:
                        case 6:
                        case 8:
                            printx("/// PSeudo GPU info: %d\n", (data & 0xffffff));
                            return;
                    }
                    
                    ret.data = info[data & 0xffffff];
                    return;
                    
                /* unused */
                case 0x02:
                case 0x06:
                    return;
            }
            
            printx("/// PSeudo GPU Write Status: 0x%x", (GPU_COMMAND(data)));
            return;
    }
    
    printx("/// PSeudo GPU Write: %d <- 0x%x", (addr & 0xf), data);
}

uw CstrGraphics::read(uw addr) {
    switch(addr & 0xf) {
        case 0: // Data
            dataRead(&ret.data, 1);
            return ret.data;
            
        case 4: // Status
            return (ret.status ^= GPU_STAT_ODDLINES) | GPU_STAT_READYFORVRAM;
    }
    
    printx("/// PSeudo GPU Read: %d", (addr & 0xf));
    return 0;
}

void CstrGraphics::dataWrite(uw *ptr, sw size) {
    for (int i = 0; i < size; i++) {
        if (modeDMA == GPU_DMA_MEM2VRAM) {
            if ((i += fetchMem((uh *)ptr, size - i)) >= size) {
                continue;
            }
            ptr += i;
        }
        ret.data = *ptr++;
        
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
                continue;
            }
        }
        else {
            pipe.data[pipe.row] = ret.data;
            
            if (pipe.size > 128) { // Lines with termination code
                if ((pipe.size == 254 && pipe.row >= 3) || (pipe.size == 255 && pipe.row >= 4 && !(pipe.row & 1))) {
                    if ((pipe.data[pipe.row] & 0xf000f000) == 0x50005000) {
                        pipe.row = pipe.size - 1;
                    }
                }
            }
            pipe.row++;
        }
        
        if (pipe.size == pipe.row) {
            pipe.size = 0;
            pipe.row  = 0;
            
            draw.primitive(pipe.prim, pipe.data);
        }
    }
}

int CstrGraphics::fetchMem(uh *ptr, sw size) {
    if (!vrop.enabled) {
        modeDMA = GPU_DMA_NONE;
        return 0;
    }
    
    int count = 0;
    size <<= 1;
    
    uh *st = (uh *)vrop.raw;
    
    while (vrop.v.p < vrop.v.end) {
        while (vrop.h.p < vrop.h.end) {
            if (isVideo24Bit) {
                *st++ = *ptr;
            }
            else {
                vrop.raw[count] = tcache.pixel2texel(*ptr);
            }
            
            vram.ptr[(vrop.v.p << 10) + vrop.h.p] = *ptr;
            vrop.h.p++;
            ptr++;
            
            if (++count == size) {
                if (vrop.h.p == vrop.h.end) {
                    vrop.h.p  = vrop.h.start;
                    vrop.v.p++;
                }
                
                redirect VRAM_END;
            }
        }
        
        vrop.h.p = vrop.h.start;
        vrop.v.p++;
    }
    
VRAM_END:
    if (vrop.v.p >= vrop.v.end) {
        draw.outputVRAM(vrop.raw, vrop.h.start, vrop.v.start, vrop.h.end - vrop.h.start, vrop.v.end - vrop.v.start);
        
        delete[] vrop.raw;
        vrop.enabled = false;
        
        modeDMA = GPU_DMA_NONE;
    }
    
    if (count % 2) {
        count++;
    }
    return count >> 1;
}

void CstrGraphics::dataRead(uw *ptr, sw size) {
    if (modeDMA == GPU_DMA_VRAM2MEM) {
        ret.status &= (~(0x14000000));
        
        do {
            *ptr++ = *(uw *)&vram.ptr[vrop.pvram + vrop.h.p]; // Bad Access V8 2nd Offence
            
            if ((vrop.h.p += 2) >= vrop.h.end) {
                vrop.h.p = vrop.h.start;
                vrop.pvram += FRAME_W;
                
                if (++vrop.v.p >= vrop.v.end) {
                    modeDMA = GPU_DMA_NONE;
                    ret.status &= (~(GPU_STAT_READYFORVRAM));
                    break;
                }
            }
        } while (--size);
        
        ret.status = (ret.status | 0x14000000) & (~(GPU_STAT_DMABITS));
    }
}

void CstrGraphics::photoMoveWithin(uw *packets) {
    // Source
    uh srcX = (packets[1] >>  0) & 0x03ff;
    uh srcY = (packets[1] >> 16) & 0x01ff;
    
    // Destination
    uh dstX = (packets[2] >>  0) & 0x03ff;
    uh dstY = (packets[2] >> 16) & 0x01ff;
    
    // W + H of transfer
    uh   iW = (packets[3] >>  0) & 0xffff;
    uh   iH = (packets[3] >> 16) & 0xffff;
    
    if ((srcX + iW) > FRAME_W) iW = FRAME_W - srcX;
    if ((dstX + iW) > FRAME_W) iW = FRAME_W - dstX;
    if ((srcY + iH) > FRAME_H) iH = FRAME_H - srcY;
    if ((dstY + iH) > FRAME_H) iH = FRAME_H - dstY;
    
    for (int v = 0; v < iH; v++) {
        for (int h = 0; h < iW; h++) {
            vram.ptr[((dstY + v) << 10) + dstX + h] = vram.ptr[((srcY + v) << 10) + srcX + h];
        }
    }
}

void CstrGraphics::photoSendTo(uw *packets) {
    uh *p = (uh *)packets;
    
    vrop.h.start = vrop.h.p = p[2];
    vrop.v.start = vrop.v.p = p[3];
    vrop.h.end   = vrop.h.p + p[4];
    vrop.v.end   = vrop.v.p + p[5];
    
    vrop.enabled = true;
    vrop.raw = new uw[p[4] * p[5]];
    modeDMA = GPU_DMA_MEM2VRAM;
    
    // Cache invalidation
    tcache.invalidate(vrop.h.start, vrop.v.start, vrop.h.end, vrop.v.end);
}

void CstrGraphics::photoReadFrom(uw *packets) {
    uh *p = (uh *)packets;
    
    vrop.h.start = vrop.h.p = p[2];
    vrop.v.start = vrop.v.p = p[3];
    vrop.h.end   = vrop.h.p + p[4];
    vrop.v.end   = vrop.v.p + p[5];
    
    vrop.pvram = p[3] * FRAME_W;
    modeDMA = GPU_DMA_VRAM2MEM;
    
    ret.status |= GPU_STAT_READYFORVRAM;
}

void CstrGraphics::executeDMA(CstrBus::castDMA *dma) {
    uw *p   = (uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    
    switch(dma->chcr) {
        case 0x01000200:
            dataRead(p, size);
            return;
            
        case 0x01000201:
            dataWrite(p, size);
            return;
            
        case 0x01000401:
            do {
                uw hdr = *(uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
                p = (uw *)&mem.ram.ptr[(dma->madr + 4) & (mem.ram.size - 1)];
                dataWrite(p, hdr >> 24);
                dma->madr = hdr & 0xffffff;
            }
            while(dma->madr != 0xffffff);
            return;
            
        /* unused */
        case 0x00000401: // Disable DMA?
            return;
    }
    
    printx("/// PSeudo GPU DMA: 0x%x", dma->chcr);
}
