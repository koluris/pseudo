#include "Global.h"


#define    MAKERGB15(R, G, B) ((((R)>>3)<<10)|(((G)>>3)<<5)|((B)>>3))
#define    ROUND(c) rtbl[(c) + 128 + 256]

#define RGB15CL(N) IMAGE[N] = MAKERGB15(ROUND(GETY + R), ROUND(GETY + G), ROUND(GETY + B));
#define RGB24CL(N) IMAGE[N+ 2] = ROUND(GETY + R); IMAGE[N+ 1] = ROUND(GETY + G); IMAGE[N+ 0] = ROUND(GETY + B);

#define    MULR(A) (((sw)0x0000059B * (A)) >> 10)
#define    MULG(A) (((sw)0xFFFFFEA1 * (A)) >> 10)
#define    MULB(A) (((sw)0x00000716 * (A)) >> 10)
#define    MULF(A) (((sw)0xFFFFFD25 * (A)) >> 10)

#define    RUNOF(a) ((a)>>10)
#define    VALOF(a) ((sw)(((a)<<22)>>22))


CstrMotionDecoder mdec;

void CstrMotionDecoder::reset() {
    rl = (uh *)&mem.ram.ptr[0x100000];
    status = cmd = 0;
    
    for (sw k=0; k<256; k++) {
        rtbl[k+0x000] = 0;
        rtbl[k+0x100] = k;
        rtbl[k+0x200] = 255;
    }
}

void CstrMotionDecoder::write(uw addr, uw data) {
    switch(addr & 0xf) {
        case 0:
            cmd = data;
            
            if ((data&0xf5ff0000) == 0x30000000) {
                len = data&0xffff;
            }
            return;
            
        case 4:
            if (data & 0x80000000) {
                reset();
            }
            return;
    }
    
    printx("/// PSeudo MDEC write: 0x%08x <- 0x%08x", addr, data);
}

uw CstrMotionDecoder::read(uw addr) {
    switch(addr & 0xf) {
        case 0:
            return cmd;
            
        case 4:
            return status;
    }
    
    printx("/// PSeudo MDEC read: 0x%08x", addr);
    return 0;
}

sw iq_y[64], iq_uv[64];

void CstrMotionDecoder::MacroBlock(sw *block, sw kh, sw sh) {
    for (sw k=0; k<8; k++, (sh) ? block+=8 : block++) {
        if((block[kh*1]|
            block[kh*2]|
            block[kh*3]|
            block[kh*4]|
            block[kh*5]|
            block[kh*6]|
            block[kh*7]) == 0) {
            block[kh*0]=
            block[kh*1]=
            block[kh*2]=
            block[kh*3]=
            block[kh*4]=
            block[kh*5]=
            block[kh*6]=
            block[kh*7]=
            block[kh*0]>>sh;
            
            continue;
        }
        sw z10 = block[kh*0]+block[kh*4];
        sw z11 = block[kh*0]-block[kh*4];
        sw z13 = block[kh*2]+block[kh*6];
        sw z12 = block[kh*2]-block[kh*6]; z12 = ((z12*362)>>8)-z13;
        
        sw tmp0 = z10+z13;
        sw tmp3 = z10-z13;
        sw tmp1 = z11+z12;
        sw tmp2 = z11-z12;
        
        z13 = block[kh*3]+block[kh*5];
        z10 = block[kh*3]-block[kh*5];
        z11 = block[kh*1]+block[kh*7];
        z12 = block[kh*1]-block[kh*7]; sw z5 = (((z12-z10)*473)>>8);
        
        sw tmp7 = z11+z13;
        sw tmp6 = (((z10)*669)>>8)+z5 -tmp7;
        sw tmp5 = (((z11-z13)*362)>>8)-tmp6;
        sw tmp4 = (((z12)*277)>>8)-z5 +tmp5;
        
        block[kh*0] = (tmp0+tmp7)>>sh;
        block[kh*7] = (tmp0-tmp7)>>sh;
        block[kh*1] = (tmp1+tmp6)>>sh;
        block[kh*6] = (tmp1-tmp6)>>sh;
        block[kh*2] = (tmp2+tmp5)>>sh;
        block[kh*5] = (tmp2-tmp5)>>sh;
        block[kh*4] = (tmp3+tmp4)>>sh;
        block[kh*3] = (tmp3-tmp4)>>sh;
    }
}

void CstrMotionDecoder::idct(sw *block, sw k) {
    if (k == 0) {
        sw val = block[0]>>5;
        
        for (sw i=0; i<64; i++) {
            block[i] = val;
        }
        return;
    }
    MacroBlock(block, 8, 0);
    MacroBlock(block, 1, 5);
}

void CstrMotionDecoder::TabInit(sw *iqtab, ub *iq_y) {
    for (sw i=0; i<64; i++) {
        iqtab[i] = iq_y[i]*aanscales[zscan[i]]>>12;
    }
}

uh *CstrMotionDecoder::rl2blk(sw *blk, uh *mdec_rl) {
    sw k,q_scale,rl;
    sw *iqtab;
    
    memset(blk, 0, 6*64*4);
    iqtab = iq_uv;
    
    for (sw i=0; i<6; i++) {
        
        if (i>1) iqtab = iq_y;
        
        rl = *mdec_rl++;
        q_scale = rl>>10;
        blk[0] = iqtab[0]*VALOF(rl);
        k = 0;
        
        for(;;) {
            rl = *mdec_rl++; if (rl==0xfe00) break;
            k += (rl>>10)+1;if (k >  63) break;
            blk[zscan[k]] = (iqtab[k] * q_scale * VALOF(rl)) >> 3;
        }
        idct(blk, k+1);
        
        blk+=64;
    }
    return mdec_rl;
}

void CstrMotionDecoder::Yuv15(sw *Block, uh *IMAGE) {
    sw GETY;
    sw CB,CR,R,G,B;
    
    sw *YYBLK = Block + 64 * 2;
    sw *CBBLK = Block;
    sw *CRBLK = Block + 64;
    
    for (sw Y=0; Y<16; Y+=2, CRBLK+=4, CBBLK+=4, YYBLK+=8, IMAGE+=24) {
        if (Y == 8) {
            YYBLK = YYBLK + 64;
        }
        
        for (sw X=0; X<4; X++, IMAGE+=2, CRBLK++, CBBLK++, YYBLK+=2) {
            CR = *CRBLK;
            CB = *CBBLK;
            
            R = MULR(CR);
            G = MULG(CB) + MULF(CR);
            B = MULB(CB);
            
            GETY = YYBLK[0]; RGB15CL(0x00);
            GETY = YYBLK[1]; RGB15CL(0x01);
            GETY = YYBLK[8]; RGB15CL(0x10);
            GETY = YYBLK[9]; RGB15CL(0x11);
            
            CR = *(CRBLK + 4);
            CB = *(CBBLK + 4);
            
            R = MULR(CR);
            G = MULG(CB) + MULF(CR);
            B = MULB(CB);
            
            GETY = YYBLK[64 + 0]; RGB15CL(0x08);
            GETY = YYBLK[64 + 1]; RGB15CL(0x09);
            GETY = YYBLK[64 + 8]; RGB15CL(0x18);
            GETY = YYBLK[64 + 9]; RGB15CL(0x19);
        }
    }
}

void CstrMotionDecoder::Yuv24(sw *Block, ub *IMAGE) {
    sw GETY;
    sw CB, CR, R, G, B;
    
    sw *YYBLK = Block + 64 * 2;
    sw *CBBLK = Block;
    sw *CRBLK = Block + 64;
    
    for (sw Y=0; Y<16; Y+=2, CRBLK+=4, CBBLK+=4, YYBLK+=8, IMAGE+=24*3) {
        if (Y == 8) {
            YYBLK = YYBLK + 64;
        }
        
        for (sw X=0; X<4; X++, IMAGE+=2*3, CRBLK++, CBBLK++, YYBLK+=2) {
            CR = *CRBLK;
            CB = *CBBLK;
            
            R = MULR(CR);
            G = MULG(CB) + MULF(CR);
            B = MULB(CB);
            
            GETY = YYBLK[0]; RGB24CL(0x00 * 3);
            GETY = YYBLK[1]; RGB24CL(0x01 * 3);
            GETY = YYBLK[8]; RGB24CL(0x10 * 3);
            GETY = YYBLK[9]; RGB24CL(0x11 * 3);
            
            CR = *(CRBLK + 4);
            CB = *(CBBLK + 4);
            
            R = MULR(CR);
            G = MULG(CB) + MULF(CR);
            B = MULB(CB);
            
            GETY = YYBLK[64 + 0]; RGB24CL(0x08 * 3);
            GETY = YYBLK[64 + 1]; RGB24CL(0x09 * 3);
            GETY = YYBLK[64 + 8]; RGB24CL(0x18 * 3);
            GETY = YYBLK[64 + 9]; RGB24CL(0x19 * 3);
        }
    }
}

void CstrMotionDecoder::executeDMA(CstrBus::castDMA *dma) {
    ub *p = &mem.ram.ptr[dma->madr&(mem.ram.size-1)];
    sw z = (dma->bcr>>16)*(dma->bcr&0xffff);
    
    switch (dma->chcr&0xfff) {
        case 0x200:
        {
            sw blocksize, blk[384];
            uh *im = (uh *)p;
            
            if (cmd&0x08000000) {
                blocksize = 256;
            }
            else {
                blocksize = 384;
            }
            
            for (; z>0; z-=blocksize/2, im+=blocksize) {
                rl = rl2blk(blk, rl);
                
                if (cmd&0x8000000) {
                    Yuv15(blk, im);
                }
                else {
                    Yuv24(blk, (ub *)im);
                }
            }
            break;
        }
            
        case 0x201:
            if (cmd == 0x40000001) {
                TabInit(iq_y, p);
                TabInit(iq_uv, p+64);
            }
            if ((cmd&0xf5ff0000) == 0x30000000) {
                rl = (uh *)p;
            }
            break;
    }
}
