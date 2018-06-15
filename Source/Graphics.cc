#import "Global.h"


#define GPU_COMMAND(x) \
    (x >> 24) & 0xff


CstrGraphics vs;

void CstrGraphics::reset() {
    memset(vram.ptr, 0, vram.size);
    memset(&   vrop, 0, sizeof(vrop));
    memset(&    ret, 0, sizeof(ret));
    memset(&   pipe, 0, sizeof(pipe));
    
    ret.disabled = true;
    ret.data     = 0; //0x400;
    ret.status   = GPU_READYFORCOMMANDS | GPU_IDLE | GPU_DISPLAYDISABLED | 0x2000; // 0x14802000;
    modeDMA      = GPU_DMA_NONE;
    vpos         = 0;
    vdiff        = 0;
    isVideoPAL   = false;
    
    // Reset canvas
    draw.reset();
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
                    return;
                    
                case 0x01:
                    memset(&pipe, 0, sizeof(pipe));
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
                    isVideoPAL = data & 8;
                    
                    {
                        // Basic info
                        uh w = resMode[(data & 3) | ((data & 0x40) >> 4)];
                        uh h = (data & 4) ? 480 : 240;
                        
                        if ((data >> 5) & 1) { // No distinction for interlaced
                            draw.resize(w, h);
                        }
                        else { // Normal modes
                            if (h == vdiff) {
                                draw.resize(w, h);
                            }
                            else {
                                draw.resize(w, vpos ? vpos : vdiff);
                            }
                        }
                    }
                    return;
                    
                case 0x10: // TODO: Information
                    switch(data & 0xffffff) {
                        case 7:
                            ret.data = 2;
                            return;
                    }
                    //printx("/// PSeudo GPU Write Status Information: $%x", (data & 0xffffff));
                    return;
                    
                /* unused */
                case 0x02:
                case 0x06:
                    return;
            }
            printx("/// PSeudo GPU Write Status: $%x", (GPU_COMMAND(data)));
            return;
    }
    printx("/// PSeudo GPU Write: $%x <- $%x", (addr & 0xf), data);
}

uw CstrGraphics::read(uw addr) {
    switch(addr & 0xf) {
        case GPU_REG_DATA:
            return ret.data;
            
        case GPU_REG_STATUS:
            //return (ret.status | (GPU_READYFORCOMMANDS | GPU_READYFORVRAM | GPU_IDLE)) & ~(GPU_INTERLACED | GPU_DOUBLEHEIGHT);
            return ret.status | GPU_READYFORVRAM;
    }
    printx("/// PSeudo GPU Read: $%x", (addr & 0xf));
    
    return 0;
}

int CstrGraphics::fetchMem(uh *ptr, sw size) {
    if (!vrop.enabled) {
        modeDMA = GPU_DMA_NONE;
        return 0;
    }
    
    int count = 0;
    size <<= 1;
    
    while (vrop.v.p < vrop.v.end) {
        while (vrop.h.p < vrop.h.end) {
            vs.vram.ptr[(vrop.v.p << 10) + vrop.h.p] = *ptr;
            
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
        vrop.enabled = false;
        
        if (count%2 == 1) {
            count++;
        }
        
        modeDMA = GPU_DMA_NONE;
    }
    
    return count >> 1;
}

void CstrGraphics::dataWrite(uw *ptr, sw size) {
    int i = 0;
    
    while (i < size) {
        if (modeDMA == GPU_DMA_MEM2VRAM) {
            if ((i += fetchMem((uh *)ptr, size-i)) >= size) {
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
            
            draw.primitive(pipe.prim, pipe.data);
        }
    }
}

//void CstrGraphics::dataRead(uw *ptr, sw size) {
//    if (modeDMA == GPU_DMA_VRAM2MEM) {
//        ret.status &= ~0x14000000;
//
//        do {
//            uw data = (uw)vrop.pvram[vrop.h.p];
//
//            if (++vrop.h.p >= vrop.h.end) {
//                vrop.h.p = vrop.h.start;
//                vrop.pvram += FRAME_W;
//            }
//
//            data |= (uw)vrop.pvram[vrop.h.p] << 16;
//            *ptr++ = data;
//
//            if (++vrop.h.p >= vrop.h.end) {
//                vrop.h.p = vrop.h.start;
//                vrop.pvram += FRAME_W;
//
//                if (++vrop.v.p >= vrop.v.end) {
//                    modeDMA = GPU_DMA_NONE;
//                    ret.status &= ~GPU_READYFORVRAM;
//                    break;
//                }
//            }
//        }
//        while(--size);
//
//        ret.status = (ret.status | 0x14000000) & (~GPU_DMABITS);
//    }
//}

void CstrGraphics::photoRead(uw *data) {
    uh *k = (uh *)data;
    
    vrop.enabled = true;
    vrop.h.p     = vrop.h.start = k[2];
    vrop.v.p     = vrop.v.start = k[3];
    vrop.h.end   = vrop.h.start + k[4];
    vrop.v.end   = vrop.v.start + k[5];
    
    modeDMA = GPU_DMA_MEM2VRAM;
}

void CstrGraphics::executeDMA(CstrBus::castDMA *dma) {
    uw *p = (uw *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)], size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    
    switch(dma->chcr) {
        case 0x01000200:
            //dataRead(p, size);
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
    
    printx("/// PSeudo GPU DMA: $%x", dma->chcr);
}
