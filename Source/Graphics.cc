/* Base structure taken from PCSX-df open source emulator, and improved upon (Credits: Stephen Chao) */

#include "Global.h"


#define GPU_COMMAND(x) \
    (x >> 24) & 0xff


CstrGraphics vs;

void CstrGraphics::reset() {
    memset(vram.ptr, 0, vram.size);
    vrop = { 0 };
    ret  = { 0 };
    pipe = { 0 };
    
    ret.disabled = true;
    ret.data     = 0x400;
    ret.status   = GPU_STAT_READYFORCOMMANDS | GPU_STAT_IDLE | GPU_STAT_DISPLAYDISABLED | 0x2000; // 0x14802000;
    modeDMA      = GPU_DMA_NONE;
    vpos         = 0;
    vdiff        = 0;
    isVideoPAL   = false;
    isVideo24Bit = false;
}

#define NTSC \
    (CLOCKS_PER_SEC / 59.94)

#define PAL \
    (CLOCKS_PER_SEC / 50.00)

// Function "mach_absolute_time()" returns Nanoseconds

// 1,000,000,000 Nano
// 1,000,000     Micro
// 1,000         Milli
// 1             Base unit, 1 second

double then = 1.0;

void CstrGraphics::refresh() {
    ret.status ^= GPU_STAT_ODDLINES;
    
    if (ret.disabled) {
        GLClear(GL_COLOR_BUFFER_BIT);
        draw.swapBuffers();
    }
    
    // FPS throttle
#if 0
    double now = mach_absolute_time() / 1000.0;
    then = now > (then + CLOCKS_PER_SEC) ? now : then + (isVideoPAL ? PAL : NTSC);
    
    if (then > now) {
        usleep(then - now);
    }
#endif
    
    // Draw
    draw.swapBuffers();
}

void CstrGraphics::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case GPU_REG_DATA:
            dataWrite(&data, 1);
            return;
            
        case GPU_REG_STATUS:
            switch(GPU_COMMAND(data)) {
                case 0x00:
                    ret.status   = 0x14802000;
                    ret.disabled = true;
                    isVideoPAL   = false;
                    isVideo24Bit = false;
                    return;
                    
                case 0x01:
                    pipe = { 0 };
                    return;
                    
                case 0x03:
                    ret.disabled = data & 1;
                    return;
                    
                case 0x04:
                    modeDMA = data & 3;
                    return;
                    
                case 0x05:
                    vpos = MAX(vpos, (data >> 10) & 0x1ff);
                    return;
                    
                case 0x07:
                    vdiff = ((data >> 10) & 0x3ff) - (data & 0x3ff);
                    return;
                    
                case 0x08:
                    isVideoPAL   = (data) & 8;
                    isVideo24Bit = (data >> 4) & 1;
                    
                    {
                        // Basic info
                        uh w = resMode[(data & 3) | ((data & 0x40) >> 4)];
                        uh h = (data & 4) ? 480 : 240;
                        
                        if ((data >> 5) & 1) { // No distinction for interlaced
                            draw.resize(w, h);
                            //printf("1 %d / %d\n", w, h);
                        }
                        else { // Normal modes
                            if (h == vdiff) {
                                draw.resize(w, h);
                                //printf("2 %d / %d\n", w, h);
                            }
                            else {
                                vdiff = vdiff == 226 ? 240 : vdiff; // paradox-059
                                draw.resize(w, vpos ? vpos : vdiff);
                                //printf("3 %d / %d\n", w, (vpos ? vpos : vdiff));
                            }
                        }
                    }
                    return;
                    
                case 0x10: // TODO: Information
                    switch(data & 0xffffff) {
                        case 7:
                            ret.data = 2;
                            return;
                            
                        default:
                            //printf("Requested GPU info: %d\n", (data & 0xffffff));
                            return;
                    }
                    return;
                    
                /* unused */
                case 0x02:
                case 0x06:
                    return;
            }
            printx("/// PSeudo GPU Write Status: 0x%x", (GPU_COMMAND(data)));
            return;
    }
    printx("/// PSeudo GPU Write: 0x%x <- 0x%x", (addr & 0xf), data);
}

uw CstrGraphics::read(uw addr) {
    switch(addr & 0xf) {
        case GPU_REG_DATA:
            dataRead(&ret.data, 1);
            return ret.data;
            
        case GPU_REG_STATUS:
            return ret.status | GPU_STAT_READYFORVRAM;
    }
    printx("/// PSeudo GPU Read: 0x%x", (addr & 0xf));
    
    return 0;
}

void CstrGraphics::dataWrite(uw *ptr, sw size) {
    int i = 0;
    
    while (i < size) {
        if (modeDMA == GPU_DMA_MEM2VRAM) {
            if ((i += fetchMem((uh *)ptr, size - i)) >= size) {
                continue;
            }
            ptr += i;
        }
        
        ret.data = *ptr++;
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
                vrop.raw[count] = cache.pixel2texel(*ptr);
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
            uw data = (uw)vrop.pvram[vrop.h.p];
            
            if (++vrop.h.p >= vrop.h.end) {
                vrop.h.p = vrop.h.start;
                vrop.pvram += FRAME_W;
            }
            
            data |= (uw)vrop.pvram[vrop.h.p] << 16;
            *ptr++ = data;
            
            if (++vrop.h.p >= vrop.h.end) {
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

void CstrGraphics::photoMove(uw *packets) {
    uh h_0 = (packets[1]    )&0x03ff,
       v_0 = (packets[1]>>16)&0x01ff,
       h_1 = (packets[2]    )&0x03ff,
       v_1 = (packets[2]>>16)&0x01ff,
       h_t = (packets[3]    )&0xffff,
       v_t = (packets[3]>>16)&0xffff;
    
    if ((h_0+h_t) > FRAME_W) h_t = FRAME_W - h_0;
    if ((h_1+h_t) > FRAME_W) h_t = FRAME_W - h_1;
    if ((v_0+v_t) > FRAME_H) v_t = FRAME_H - v_0;
    if ((v_1+v_t) > FRAME_H) v_t = FRAME_H - v_1;
    
    for (sw v=0; v<v_t; v++) {
        for (sw h=0; h<h_t; h++) {
            vram.ptr[(FRAME_W*(v_1+v))+(h_1+h)] = vram.ptr[(FRAME_W*(v_0+v))+(h_0+h)];
        }
    }
}

void CstrGraphics::photoWrite(uw *packets) {
    uh *p = (uh *)packets;
    
    vrop.h.start = vrop.h.p = p[2];
    vrop.v.start = vrop.v.p = p[3];
    vrop.h.end   = vrop.h.p + p[4];
    vrop.v.end   = vrop.v.p + p[5];
    vrop.pvram   = &vram.ptr[vrop.v.p * FRAME_W];
    
    modeDMA = GPU_DMA_VRAM2MEM;
    
    ret.status |= GPU_STAT_READYFORVRAM;
}

void CstrGraphics::photoRead(uw *packets) {
    uh *p = (uh *)packets;
    
    vrop.enabled = true;
    vrop.raw     = new uw[p[4] * p[5]];
    
    vrop.h.start = vrop.h.p = p[2];
    vrop.v.start = vrop.v.p = p[3];
    vrop.h.end   = vrop.h.p + p[4];
    vrop.v.end   = vrop.v.p + p[5];
    vrop.pvram   = &vram.ptr[vrop.v.p * FRAME_W];
    
    modeDMA = GPU_DMA_MEM2VRAM;
    
    // Cache invalidation
    cache.invalidate(vrop.h.start, vrop.v.start, vrop.h.end, vrop.v.end);
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
                p = (uw *)&mem.ram.ptr[(dma->madr + 4) & 0x1ffffc];
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
