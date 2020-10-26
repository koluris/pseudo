#include "Global.h"

//#define MAKERGB15(R, G, B) ((((R)>>3)<<10)|(((G)>>3)<<5)|((B)>>3))
//#define RGB15CL(N) IMAGE[N] = MAKERGB15(ROUND(GETY + R), ROUND(GETY + G), ROUND(GETY + B));

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
    tex[a + 0] = ROUND(getY + B); \
    tex[a + 1] = ROUND(getY + G); \
    tex[a + 2] = ROUND(getY + R); \


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

void CstrMotionDecoder::macroBlock(sw *block, sw kh, sw sh) {
    sw index = 0;
    for (sw k=0; k<8; k++, (sh) ? block+=8 : block++) {
        if((block[index + kh*1]|
            block[index + kh*2]|
            block[index + kh*3]|
            block[index + kh*4]|
            block[index + kh*5]|
            block[index + kh*6]|
            block[index + kh*7]) == 0) {
            block[index + kh*0]=
            block[index + kh*1]=
            block[index + kh*2]=
            block[index + kh*3]=
            block[index + kh*4]=
            block[index + kh*5]=
            block[index + kh*6]=
            block[index + kh*7]=
            block[index + kh*0]>>sh;
            
            continue;
        }
        sw z10 = block[index + kh*0]+block[index + kh*4];
        sw z11 = block[index + kh*0]-block[index + kh*4];
        sw z13 = block[index + kh*2]+block[index + kh*6];
        sw z12 = block[index + kh*2]-block[index + kh*6]; z12 = ((z12*362)>>8)-z13;
        
        sw tmp0 = z10+z13;
        sw tmp3 = z10-z13;
        sw tmp1 = z11+z12;
        sw tmp2 = z11-z12;
        
        z13 = block[index + kh*3]+block[index + kh*5];
        z10 = block[index + kh*3]-block[index + kh*5];
        z11 = block[index + kh*1]+block[index + kh*7];
        z12 = block[index + kh*1]-block[index + kh*7]; sw z5 = (((z12-z10)*473)>>8);
        
        sw tmp7 = z11+z13;
        sw tmp6 = (((z10)*669)>>8)+z5 -tmp7;
        sw tmp5 = (((z11-z13)*362)>>8)-tmp6;
        sw tmp4 = (((z12)*277)>>8)-z5 +tmp5;
        
        block[index + kh*0] = (tmp0+tmp7)>>sh;
        block[index + kh*7] = (tmp0-tmp7)>>sh;
        block[index + kh*1] = (tmp1+tmp6)>>sh;
        block[index + kh*6] = (tmp1-tmp6)>>sh;
        block[index + kh*2] = (tmp2+tmp5)>>sh;
        block[index + kh*5] = (tmp2-tmp5)>>sh;
        block[index + kh*4] = (tmp3+tmp4)>>sh;
        block[index + kh*3] = (tmp3-tmp4)>>sh;
    }
}

void CstrMotionDecoder::idct(sw *block, sw k) {
    if (k == 0) {
        sw val = block[0] >> 5;

        for (int i = 0; i < 64; i++) {
            block[i] = val;
        }
        return;
    }
    
    macroBlock(block, 8, 0);
    macroBlock(block, 1, 5);
}

void CstrMotionDecoder::uv15(sw *Block, uh *IMAGE) {
//    sw GETY;
//    sw CB,CR,R,G,B;
//
//    sw *YYBLK = Block + 64 * 2;
//    sw *CBBLK = Block;
//    sw *CRBLK = Block + 64;
//
//    for (sw Y=0; Y<16; Y+=2, CRBLK+=4, CBBLK+=4, YYBLK+=8, IMAGE+=24) {
//        if (Y == 8) {
//            YYBLK = YYBLK + 64;
//        }
//
//        for (sw X=0; X<4; X++, IMAGE+=2, CRBLK++, CBBLK++, YYBLK+=2) {
//            CR = *CRBLK;
//            CB = *CBBLK;
//
//            R = MULR(CR);
//            G = MULG(CB) + MULF(CR);
//            B = MULB(CB);
//
//            GETY = YYBLK[0]; RGB15CL(0x00);
//            GETY = YYBLK[1]; RGB15CL(0x01);
//            GETY = YYBLK[8]; RGB15CL(0x10);
//            GETY = YYBLK[9]; RGB15CL(0x11);
//
//            CR = *(CRBLK + 4);
//            CB = *(CBBLK + 4);
//
//            R = MULR(CR);
//            G = MULG(CB) + MULF(CR);
//            B = MULB(CB);
//
//            GETY = YYBLK[64 + 0]; RGB15CL(0x08);
//            GETY = YYBLK[64 + 1]; RGB15CL(0x09);
//            GETY = YYBLK[64 + 8]; RGB15CL(0x18);
//            GETY = YYBLK[64 + 9]; RGB15CL(0x19);
//        }
//    }
}

void CstrMotionDecoder::uv24(sw *blk, ub *tex) {
    sw *CBBLK = blk;
    sw *CRBLK = blk + 64;
    sw *YYBLK = blk + 64 * 2;
    
    for (sw h = 0; h < 16; h += 2, CBBLK += 4, CRBLK += 4, YYBLK += 8, tex += 24 * 3) {
        if (h == 8) {
            YYBLK = YYBLK + 64;
        }
        
        for (sw w = 0; w < 4; w++, CBBLK++, CRBLK++, YYBLK += 2, tex += 2 * 3) {
            sw CB = *CBBLK;
            sw CR = *CRBLK;
            
            sw B = MULB(CB);
            sw G = MULG(CB) + MULF(CR);
            sw R = MULR(CR);
            
            sw getY;
            getY = YYBLK[0]; RGB24CL(0x00 * 3);
            getY = YYBLK[1]; RGB24CL(0x01 * 3);
            getY = YYBLK[8]; RGB24CL(0x10 * 3);
            getY = YYBLK[9]; RGB24CL(0x11 * 3);
            
            CB = *(CBBLK + 4);
            CR = *(CRBLK + 4);
            
            B = MULB(CB);
            G = MULG(CB) + MULF(CR);
            R = MULR(CR);
            
            getY = YYBLK[64 + 0]; RGB24CL(0x08 * 3);
            getY = YYBLK[64 + 1]; RGB24CL(0x09 * 3);
            getY = YYBLK[64 + 8]; RGB24CL(0x18 * 3);
            getY = YYBLK[64 + 9]; RGB24CL(0x19 * 3);
        }
    }
}

void CstrMotionDecoder::executeDMA(CstrBus::castDMA *dma) {
    //ub *p = &mem.ram.ptr[dma->madr&(mem.ram.size-1)];
    sw size = (dma->bcr >> 16) * (dma->bcr & 0xffff);
    static uw maddr = 0;
    
    switch (dma->chcr & 0xfff) {
        case 0x200:
        {
            if (cmd & 0x8000000) { // YUV15
            }
            else { // YUV24
                sw blk[384];
                uw im = dma->madr;
                
                for (; size > 0; size -= 384 / 2, im += 384 * 2) {
                    memset(&blk, 0, sizeof(blk));
                    sw *iqtab = iq_uv;
                    
                    for (sw i = 0, blkindex = 0; i < 6; i++, blkindex += 64) {
                        if (i > 1) {
                            iqtab = iq_y;
                        }
                        
                        sw rl = *(uh *)&mem.ram.ptr[maddr & (mem.ram.size - 1)];
                        maddr += 2;
                        
                        sw q_scale = rl >> 10;
                        blk[blkindex + 0] = iqtab[0] * VALOF(rl);
                        
                        sw k = 0;
                        for(;;) {
                            rl = *(uh *)&mem.ram.ptr[maddr & (mem.ram.size - 1)];
                            maddr += 2;
                            
                            if (rl == 0xfe00) {
                                break;
                            }
                            k += (rl >> 10) + 1;
                            if (k > 63) {
                                break;
                            }
                            blk[blkindex + zscan[k]] = (iqtab[k] * q_scale * VALOF(rl)) >> 3;
                        }
                        idct(&blk[blkindex], k + 1);
                    }
                    
                    uv24(blk, (ub *)&mem.ram.ptr[im & (mem.ram.size - 1)]);
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
