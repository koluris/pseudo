#include "Global.h"

#define VALOF(a) \
    ((sw)(((a) << 22) >> 22))

#define MULR(a) \
    (((sw)0x0000059b * (a)) >> 10)

#define MULG(a) \
    (((sw)0xfffffea1 * (a)) >> 10)

#define MULB(a) \
    (((sw)0x00000716 * (a)) >> 10)

#define MULF(a) \
    (((sw)0xfffffd25 * (a)) >> 10)

#define ROUND(a) \
    rtbl[(a) + 128 + 256]

#define RGB24CL(a) \
    tex[a + 0] = ROUND(data + iB); \
    tex[a + 1] = ROUND(data + iG); \
    tex[a + 2] = ROUND(data + iR); \

CstrMotionDecoder mdec;

void CstrMotionDecoder::reset() {
    cmd    = 0;
    status = 0;
    
    for (int k = 0; k < 256; k++) {
        rtbl[k + 0x000] = 0;
        rtbl[k + 0x100] = k;
        rtbl[k + 0x200] = 255;
    }
}

void CstrMotionDecoder::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case 0:
            cmd = data;
            return;
            
        case 4:
            if (data & 0x80000000) {
                reset();
            }
            return;
    }
    
    printx("/// PSeudo MDEC write: 0x%08x <- 0x%08x\n", addr, data);
}

uw CstrMotionDecoder::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            return cmd;
            
        case 4:
            return status;
    }
    
    printx("/// PSeudo MDEC read: 0x%08x\n", addr);
    return 0;
}

#define macroBlock(block, idx, kh, sh) { \
    uw index = idx; \
    for (int k = 0; k < 8; k++, index += (sh) ? 8 : 1) { \
        if((block[index + kh * 1] | \
            block[index + kh * 2] | \
            block[index + kh * 3] | \
            block[index + kh * 4] | \
            block[index + kh * 5] | \
            block[index + kh * 6] | \
            block[index + kh * 7]) == 0) { \
                block[index + kh * 0] = \
                block[index + kh * 1] = \
                block[index + kh * 2] = \
                block[index + kh * 3] = \
                block[index + kh * 4] = \
                block[index + kh * 5] = \
                block[index + kh * 6] = \
                block[index + kh * 7] = \
                block[index + kh * 0] >> sh; \
                \
                continue; \
        } \
        sw z10 = block[index + kh * 0] + block[index + kh * 4]; \
        sw z11 = block[index + kh * 0] - block[index + kh * 4]; \
        sw z13 = block[index + kh * 2] + block[index + kh * 6]; \
        sw z12 = block[index + kh * 2] - block[index + kh * 6]; \
        z12 = ((z12 * 362) >> 8) - z13; \
        \
        sw tmp0 = z10 + z13; \
        sw tmp3 = z10 - z13; \
        sw tmp1 = z11 + z12; \
        sw tmp2 = z11 - z12; \
        \
        z13 = block[index + kh * 3] + block[index + kh * 5]; \
        z10 = block[index + kh * 3] - block[index + kh * 5]; \
        z11 = block[index + kh * 1] + block[index + kh * 7]; \
        z12 = block[index + kh * 1] - block[index + kh * 7]; \
        \
        sw z5 = ((z12 - z10) * 473) >> 8; \
        \
        sw tmp7 = z11 + z13; \
        sw tmp6 = ((z10 * 669) >> 8) + z5 - tmp7; \
        sw tmp5 = (((z11 - z13) * 362) >> 8) - tmp6; \
        sw tmp4 = ((z12 * 277) >> 8) - z5 + tmp5; \
        \
        block[index + kh * 0] = (tmp0 + tmp7) >> sh; \
        block[index + kh * 7] = (tmp0 - tmp7) >> sh; \
        block[index + kh * 1] = (tmp1 + tmp6) >> sh; \
        block[index + kh * 6] = (tmp1 - tmp6) >> sh; \
        block[index + kh * 2] = (tmp2 + tmp5) >> sh; \
        block[index + kh * 5] = (tmp2 - tmp5) >> sh; \
        block[index + kh * 4] = (tmp3 + tmp4) >> sh; \
        block[index + kh * 3] = (tmp3 - tmp4) >> sh; \
    } \
}

void CstrMotionDecoder::executeDMA(CstrBus::castDMA *dma) {
    sw size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    static uw maddr = 0; // Needs to be class variable
    
    switch (dma->chcr & 0xfff) {
        case 0x200:
        {
            if (cmd & 0x8000000) { // YUV15
                printf("Yesh\n");
            }
            else { // YUV24
                sw blk[384]; // Needs to be class variable
                uw im = dma->madr;
                
                for (; size > 0; size -= 384 / 2, im += 384 * 2) {
                    memset(&blk, 0, sizeof(blk));
                    sw *iqtab = iq_uv;
                    sw blkindex = 0;
                    
                    for (int i = 0; i < 6; i++) {
                        if (i > 1) {
                            iqtab = iq_y;
                        }
                        
                        sw rl = *(uh *)&mem.ram.ptr[maddr & (mem.ram.size - 1)];
                        maddr += 2;
                        
                        sw q_scale = rl >> 10;
                        blk[blkindex] = iqtab[0] * VALOF(rl);
                        
                        sw k = 0;
                        for(;;) {
                            rl = *(uh *)&mem.ram.ptr[maddr & (mem.ram.size - 1)];
                            maddr += 2;
                            
                            if (rl == 0xfe00) {
                                break;
                            }
                            
                            if ((k += (rl >> 10) + 1) > 63) {
                                break;
                            }
                            blk[blkindex + zscan[k]] = (iqtab[k] * q_scale * VALOF(rl)) >> 3;
                        }
                        
                        if ((k + 1) == 0) {
                            for (int i = 0; i < 64; i++) {
                                blk[blkindex + i] = blk[blkindex] >> 5;
                            }
                            continue;
                        }
                        
                        // Macro blocks
                        macroBlock(blk, blkindex, 8, 0);
                        macroBlock(blk, blkindex, 1, 5);
                        
                        blkindex += 64;
                    }
                    
                    // YUV24
                    uw immmm = im;
                    
                    sw indexCb = 0;
                    sw indexCr = 64;
                    sw indexY  = 64 * 2;
                    
                    for (int h = 0; h < 16; h += 2) {
                        if (h == 8) {
                            indexY += 64;
                        }
                        
                        for (int w = 0; w < 4; w++) {
                            ub *tex = (ub *)&mem.ram.ptr[immmm & (mem.ram.size - 1)];
                            sw data;
                            
                            sw CB = blk[indexCb];
                            sw CR = blk[indexCr];
                            
                            sw iB = MULB(CB);
                            sw iG = MULG(CB) + MULF(CR);
                            sw iR = MULR(CR);
                            
                            data = blk[indexY + 0]; RGB24CL(0x00 * 3);
                            data = blk[indexY + 1]; RGB24CL(0x01 * 3);
                            data = blk[indexY + 8]; RGB24CL(0x10 * 3);
                            data = blk[indexY + 9]; RGB24CL(0x11 * 3);
                            
                            CB = blk[indexCb + 4];
                            CR = blk[indexCr + 4];
                            
                            iB = MULB(CB);
                            iG = MULG(CB) + MULF(CR);
                            iR = MULR(CR);
                            
                            data = blk[indexY + 64 + 0]; RGB24CL(0x08 * 3);
                            data = blk[indexY + 64 + 1]; RGB24CL(0x09 * 3);
                            data = blk[indexY + 64 + 8]; RGB24CL(0x18 * 3);
                            data = blk[indexY + 64 + 9]; RGB24CL(0x19 * 3);
                            
                            indexCb += 1;
                            indexCr += 1;
                            indexY  += 2;
                            immmm   += 2 * 3;
                        }
                        
                        indexCb += 4;
                        indexCr += 4;
                        indexY  += 8;
                        immmm   += 24 * 3;
                    }
                }
            }
            return;
        }
            
        case 0x201:
            if (cmd == 0x40000001) {
                ub *ramp = (ub *)&mem.ram.ptr[(dma->madr) & (mem.ram.size - 1)];
                for (int i = 0; i < 64; i++) {
                    iq_y[i] = (ramp[i] * aanscales[zscan[i]]) >> 12;
                }
                
                ramp = (ub *)&mem.ram.ptr[(dma->madr + 64) & (mem.ram.size - 1)];
                for (int i = 0; i < 64; i++) {
                    iq_uv[i] = (ramp[i] * aanscales[zscan[i]]) >> 12;
                }
            }
            
            if ((cmd & 0xf5ff0000) == 0x30000000) {
                maddr = dma->madr;
            }
            return;
    }
}
