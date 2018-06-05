#import "Global.h"


CstrCop2 cop2;

void CstrCop2::reset() {
    memset(&base, 0, sizeof(base));
}

#define VX0  ((short *)&base[ 0].d)[0]
#define VY0  ((short *)&base[ 0].d)[1]
#define VZ0  ((short *)&base[ 1].d)[0]
#define VX1  ((short *)&base[ 2].d)[0]
#define VY1  ((short *)&base[ 2].d)[1]
#define VZ1  ((short *)&base[ 3].d)[0]
#define VX2  ((short *)&base[ 4].d)[0]
#define VY2  ((short *)&base[ 4].d)[1]
#define VZ2  ((short *)&base[ 5].d)[0]
#define IR0  ((int   *)&base[ 8].d)[0]
#define IR1  ((int   *)&base[ 9].d)[0]
#define IR2  ((int   *)&base[10].d)[0]
#define IR3  ((int   *)&base[11].d)[0]
#define SXY0 base[12].d
#define SX0  ((short *)&base[12].d)[0]
#define SY0  ((short *)&base[12].d)[1]
#define SXY1 base[13].d
#define SX1  ((short *)&base[13].d)[0]
#define SY1  ((short *)&base[13].d)[1]
#define SXY2 base[14].d
#define SX2  ((short *)&base[14].d)[0]
#define SY2  ((short *)&base[14].d)[1]
#define SZ0  base[16].d
#define SZ1  base[17].d
#define SZ2  base[18].d
#define SZ3  base[19].d
#define MAC0 ((int   *)&base[24].d)[0]
#define MAC1 ((int   *)&base[25].d)[0]
#define MAC2 ((int   *)&base[26].d)[0]
#define MAC3 ((int   *)&base[27].d)[0]

#define R11  ((short *)&base[ 0].c)[0]
#define R12  ((short *)&base[ 0].c)[1]
#define R13  ((short *)&base[ 1].c)[0]
#define R21  ((short *)&base[ 1].c)[1]
#define R22  ((short *)&base[ 2].c)[0]
#define R23  ((short *)&base[ 2].c)[1]
#define R31  ((short *)&base[ 3].c)[0]
#define R32  ((short *)&base[ 3].c)[1]
#define R33  ((short *)&base[ 4].c)[0]
#define TRX  ((int   *)&base[ 5].c)[0]
#define TRY  ((int   *)&base[ 6].c)[0]
#define TRZ  ((int   *)&base[ 7].c)[0]
#define OFX  ((int   *)&base[24].c)[0]
#define OFY  ((int   *)&base[25].c)[0]
#define H    ((uh    *)&base[26].c)[0]
#define DQA  ((short *)&base[27].c)[0]
#define DQB  ((int   *)&base[28].c)[0]
#define FLAG ((int   *)&base[31].c)[0]

#define FIX(a) \
    ((a)>>12)

#define LIM(a, min, max, bit) \
    (((a) < min) ? (FLAG |= (1<<bit), min) : \
    (((a) > max) ? (FLAG |= (1<<bit), max) : ((a))))

#define limB1(a) LIM(a, -32768.0, 32767.0, 24)
#define limB2(a) LIM(a, -32768.0, 32767.0, 23)
#define limB3(a) LIM(a, -32768.0, 32767.0, 22)
#define limD(a)  LIM(a,      0.0, 65535.0, 18)
#define limG1(a) LIM(a,  -1024.0,  1023.0, 14)
#define limG2(a) LIM(a,  -1024.0,  1023.0, 13)
#define limH(a)  LIM(a,      0.0,  4096.0, 12)

#define MAC2IR()\
    IR1 = limB1(MAC1);\
    IR2 = limB2(MAC2);\
    IR3 = limB3(MAC3)

void CstrCop2::subroutine(uw code, uw rss) {
    switch(code & 0x1ffffff) {
        case 0x0180001: // RTPS
        {
            FLAG = 0;
            
            MAC1 = FIX(R11 * VX0 + R12 * VY0 + R13 * VZ0) + TRX;
            MAC2 = FIX(R21 * VX0 + R22 * VY0 + R23 * VZ0) + TRY;
            MAC3 = FIX(R31 * VX0 + R32 * VY0 + R33 * VZ0) + TRZ;
            
            MAC2IR();
            
            SZ0  = SZ1;
            SZ1  = SZ2;
            SZ2  = SZ3;
            SZ3  = limD(MAC3);
            
            sw quotient = H * 4096.0 / SZ3;
            
            SXY0 = SXY1;
            SXY1 = SXY2;
            
            SX2  = limG1(FIX(IR1 * quotient) + OFX);
            SY2  = limG2(FIX(IR2 * quotient) + OFY);
            
            MAC0 = FIX(DQA * quotient) + DQB;
            IR0  = limH(MAC0);
        }
            return;
            
        case 0x0280030: // RTPT
        {
        }
            return;
    }
    
    printx("/// PSeudo Unknown cop2 opcode $%08x | %d | $%x", code, rss, (code & 0x1ffffff));
}
