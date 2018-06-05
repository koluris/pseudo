#import "Global.h"


CstrCop2 cop2;

void CstrCop2::reset() {
    memset(&base, 0, sizeof(base));
}

#define FIX(a)\
    ((a) >> 12)

#define LIMIT(a, min, max)\
    if (a < min) a = min; else if (a > max) a = max

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
#define SX0  ((short *)&base[12].d)[0]
#define SY0  ((short *)&base[12].d)[1]
#define SX1  ((short *)&base[13].d)[0]
#define SY1  ((short *)&base[13].d)[1]
#define SX2  ((short *)&base[14].d)[0]
#define SY2  ((short *)&base[14].d)[1]
#define SZ0  base[16].d
#define SZ1  base[17].d
#define SZ2  base[18].d
#define SZ3  base[19].d
#define SZx  base[19].d
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






#define lim(X, MIN, MAX, BIT) ((X < MIN) ? (FLAG |= (1 << BIT), MIN) : ((X > MAX) ? (FLAG |= (1 << BIT), MAX) : X))

#define limA1S(sz) lim(sz, -32768.0, 32767.0, 24)
#define limA2S(sz) lim(sz, -32768.0, 32767.0, 23)
#define limA3S(sz) lim(sz, -32768.0, 32767.0, 22)
#define limA1U(sz) lim(sz,      0.0, 32767.0, 24)
#define limA2U(sz) lim(sz,      0.0, 32767.0, 23)
#define limA3U(sz) lim(sz,      0.0, 32767.0, 22)
#define limB1( sz) lim(sz,      0.0,   255.0, 21)
#define limB2( sz) lim(sz,      0.0,   255.0, 20)
#define limB3( sz) lim(sz,      0.0,   255.0, 19)
#define limC(  sz) lim(sz,      0.0, 65535.0, 18)
#define limD1( sz) lim(sz,  -1024.0,  1023.0, 14)
#define limD2( sz) lim(sz,  -1024.0,  1023.0, 13)
#define limE(  sz) lim(sz,      0.0,  4095.0, 12)

#define MAC2IR0() IR1 = limA1S(MAC1); IR2 = limA2S(MAC2); IR3 = limA3S(MAC3);
#define MAC2IR1() IR1 = limA1U(MAC1); IR2 = limA2U(MAC2); IR3 = limA3U(MAC3);

double RZ;

void CstrCop2::subroutine(uw code, uw rss) {
    switch(code & 0x1ffffff) {
        case 0x0180001: // RTPS
        {
            FLAG = 0;
            
            MAC1 = (R11*VX0 + R12*VY0 + R13*VZ0) / 4096.0 + TRX;
            MAC2 = (R21*VX0 + R22*VY0 + R23*VZ0) / 4096.0 + TRY;
            MAC3 = (R31*VX0 + R32*VY0 + R33*VZ0) / 4096.0 + TRZ;
            
            RZ = H / limC(MAC3); if (RZ > 2147483647.0) { RZ = 2.0; FLAG |= (1<<17); }
            
            SZ0 = SZ1;
            SZ1 = SZ2;
            SZ2 = SZ3; SZ3 = limC(MAC3);
            
            base[12].d = base[13].d;
            base[13].d = base[14].d;
            
            SX2 = limD1(OFX / 65536.0 + limA1S(MAC1) * RZ);
            SY2 = limD2(OFY / 65536.0 + limA2S(MAC2) * RZ);
            
            MAC2IR0();
            
            MAC0 =      (DQB / 16777216.0 + DQA / 256.0 * RZ) * 16777216.0;
            IR0  = limE((DQB / 16777216.0 + DQA / 256.0 * RZ) * 4096.0);
            
//            int vx, vy, vz, rz;
//
//            SZx = SZ0;
//            SZ0 = SZ1;
//            SZ1 = SZ2;
//
//            vx = FIX(R11 * VX0 + R12 * VY0 + R13 * VZ0) + TRX;
//            vy = FIX(R21 * VX0 + R22 * VY0 + R23 * VZ0) + TRY;
//            vz = FIX(R31 * VX0 + R32 * VY0 + R33 * VZ0) + TRZ;
//
//            LIMIT(vz, 0, 65535);
//            rz = H * 4096 / vz;
//
//            SX2 = FIX(vx * rz) + OFX;
//            SY2 = FIX(vy * rz) + OFY;
//            SZ2 = vz;
        }
            return;
            
        case 0x0280030: // RTPT
        {
            FLAG = 0;
            
            SZ0 = SZ3;
            
            MAC1 = (R11*VX0 + R12*VY0 + R13*VZ0) / 4096.0 + TRX;
            MAC2 = (R21*VX0 + R22*VY0 + R23*VZ0) / 4096.0 + TRY;
            MAC3 = (R31*VX0 + R32*VY0 + R33*VZ0) / 4096.0 + TRZ;
            
            RZ = H / limC(MAC3); if (RZ > 2147483647.0) { RZ = 2.0; FLAG |= (1<<17); }
            
            SZ1 = limC(MAC3);
            
            SX0 = limD1(OFX / 65536.0 + limA1S(MAC1) * RZ);
            SY0 = limD2(OFY / 65536.0 + limA2S(MAC2) * RZ);
            
            MAC1 = (R11*VX1 + R12*VY1 + R13*VZ1) / 4096.0 + TRX;
            MAC2 = (R21*VX1 + R22*VY1 + R23*VZ1) / 4096.0 + TRY;
            MAC3 = (R31*VX1 + R32*VY1 + R33*VZ1) / 4096.0 + TRZ;
            
            RZ = H / limC(MAC3); if (RZ > 2147483647.0) { RZ = 2.0; FLAG |= (1<<17); }
            
            SZ2 = limC(MAC3);
            
            SX1 = limD1(OFX / 65536.0 + limA1S(MAC1) * RZ);
            SY1 = limD2(OFY / 65536.0 + limA2S(MAC2) * RZ);
            
            MAC1 = (R11*VX2 + R12*VY2 + R13*VZ2) / 4096.0 + TRX;
            MAC2 = (R21*VX2 + R22*VY2 + R23*VZ2) / 4096.0 + TRY;
            MAC3 = (R31*VX2 + R32*VY2 + R33*VZ2) / 4096.0 + TRZ;
            
            RZ = H / limC(MAC3); if (RZ > 2147483647.0) { RZ = 2.0; FLAG |= (1<<17); }
            
            SZ3 = limC(MAC3);
            
            SX2 = limD1(OFX / 65536.0 + limA1S(MAC1) * RZ);
            SY2 = limD2(OFY / 65536.0 + limA2S(MAC2) * RZ);
            
            MAC2IR0();
            
            MAC0 =      (DQB / 16777216.0 + DQA / 256.0 * RZ) * 16777216.0;
            IR0  = limE((DQB / 16777216.0 + DQA / 256.0 * RZ) * 4096.0);
            
//            int vx, vy, vz, rz;
//
//            SZx = SZ2;
//
//            vx = FIX(R11 * VX0 + R12 * VY0 + R13 * VZ0) + TRX;
//            vy = FIX(R21 * VX0 + R22 * VY0 + R23 * VZ0) + TRY;
//            vz = FIX(R31 * VX0 + R32 * VY0 + R33 * VZ0) + TRZ;
//
//            LIMIT(vz, 0, 65535);
//
//            rz = H * 4096 / vz;
//
//            SX0 = FIX(vx * rz) + OFX;
//            SY0 = FIX(vy * rz) + OFY;
//
//            SZ0 = vz;
//
//            vx = FIX(R11 * VX1 + R12 * VY1 + R13 * VZ1) + TRX;
//            vy = FIX(R21 * VX1 + R22 * VY1 + R23 * VZ1) + TRY;
//            vz = FIX(R31 * VX1 + R32 * VY1 + R33 * VZ1) + TRZ;
//
//            LIMIT(vz, 0, 65535);
//            rz = H * 4096 / vz;
//
//            SX1 = FIX(vx * rz) + OFX;
//            SY1 = FIX(vy * rz) + OFY;
//
//            SZ1 = vz;
//
//            vx = FIX(R11 * VX2 + R12 * VY2 + R13 * VZ2) + TRX;
//            vy = FIX(R21 * VX2 + R22 * VY2 + R23 * VZ2) + TRY;
//            vz = FIX(R31 * VX2 + R32 * VY2 + R33 * VZ2) + TRZ;
//
//            LIMIT(vz, 0, 65535);
//            rz = H * 4096/vz;
//
//            SX2 = FIX(vx * rz) + OFX;
//            SY2 = FIX(vy * rz) + OFY;
//
//            SZ2 = vz;
        }
            return;
    }
    
    printx("/// PSeudo Unknown cop2 opcode $%08x | %d | $%x", code, rss, (code & 0x1ffffff));
}
