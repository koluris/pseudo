#import "Global.h"


CstrCop2 cop2;

void CstrCop2::reset() {
    memset(&base, 0, sizeof(base));
}

#define FIX(a)\
    ((a) >> 12)

#define LIMIT(a, min, max)\
    if (a < min) a = min; else if (a > max) a = max

#define VX0 ((short *)&base[0].d)[0]
#define VY0 ((short *)&base[0].d)[1]
#define VZ0 ((short *)&base[1].d)[1]
#define VX1 ((short *)&base[2].d)[0]
#define VY1 ((short *)&base[2].d)[1]
#define VZ1 ((short *)&base[3].d)[0]
#define VX2 ((short *)&base[4].d)[0]
#define VY2 ((short *)&base[4].d)[1]
#define VZ2 ((short *)&base[5].d)[0]

#define SX0 ((short *)&base[12].d)[0]
#define SY0 ((short *)&base[12].d)[1]
#define SX1 ((short *)&base[13].d)[0]
#define SY1 ((short *)&base[13].d)[1]
#define SX2 ((short *)&base[14].d)[0]
#define SY2 ((short *)&base[14].d)[1]
#define SZ0 base[16].d
#define SZ1 base[17].d
#define SZ2 base[18].d
#define SZx base[19].d

#define R11 ((short *)&base[0].c)[0]
#define R12 ((short *)&base[0].c)[1]
#define R13 ((short *)&base[1].c)[0]
#define R21 ((short *)&base[1].c)[1]
#define R22 ((short *)&base[2].c)[0]
#define R23 ((short *)&base[2].c)[1]
#define R31 ((short *)&base[3].c)[0]
#define R32 ((short *)&base[3].c)[1]
#define R33 ((short *)&base[4].c)[0]
#define TRX base[5].c
#define TRY base[6].c
#define TRZ base[7].c
#define OFX (base[24].c >> 16)
#define OFY (base[25].c >> 16)
#define H   ((short *)&base[26].c)[0]

void CstrCop2::subroutine(uw code, uw rss) {
    switch(code & 0x1ffffff) {
        case 0x0180001: // RTPS
        {
            int vx, vy, vz, rz;
            
            SZx = SZ0;
            SZ0 = SZ1;
            SZ1 = SZ2;
            
            vx = FIX(R11 * VX0 + R12 * VY0 + R13 * VZ0) + TRX;
            vy = FIX(R21 * VX0 + R22 * VY0 + R23 * VZ0) + TRY;
            vz = FIX(R31 * VX0 + R32 * VY0 + R33 * VZ0) + TRZ;
            
            LIMIT(vz, 0, 65535);
            rz = H * 4096 / vz;
            
            SX2 = FIX(vx * rz) + OFX;
            SY2 = FIX(vy * rz) + OFY;
            SZ2 = vz;
        }
            return;
            
        case 0x0280030: // RTPT
        {
            int vx, vy, vz, rz;
            
            SZx = SZ2;
            
            vx = FIX(R11 * VX0 + R12 * VY0 + R13 * VZ0) + TRX;
            vy = FIX(R21 * VX0 + R22 * VY0 + R23 * VZ0) + TRY;
            vz = FIX(R31 * VX0 + R32 * VY0 + R33 * VZ0) + TRZ;
            
            LIMIT(vz, 0, 65535);
            
            rz = H * 4096 / vz;
            
            SX0 = FIX(vx * rz) + OFX;
            SY0 = FIX(vy * rz) + OFY;
            
            SZ0 = vz;
            
            vx = FIX(R11 * VX1 + R12 * VY1 + R13 * VZ1) + TRX;
            vy = FIX(R21 * VX1 + R22 * VY1 + R23 * VZ1) + TRY;
            vz = FIX(R31 * VX1 + R32 * VY1 + R33 * VZ1) + TRZ;
            
            LIMIT(vz, 0, 65535);
            rz = H * 4096 / vz;
            
            SX1 = FIX(vx * rz) + OFX;
            SY1 = FIX(vy * rz) + OFY;
            
            SZ1 = vz;
            
            vx = FIX(R11 * VX2 + R12 * VY2 + R13 * VZ2) + TRX;
            vy = FIX(R21 * VX2 + R22 * VY2 + R23 * VZ2) + TRY;
            vz = FIX(R31 * VX2 + R32 * VY2 + R33 * VZ2) + TRZ;
            
            LIMIT(vz, 0, 65535);
            rz = H * 4096/vz;
            
            SX2 = FIX(vx * rz) + OFX;
            SY2 = FIX(vy * rz) + OFY;
            
            SZ2 = vz;
        }
            return;
    }
    
    printx("/// PSeudo Unknown cop2 opcode $%08x | %d | $%x", code, rss, (code & 0x1ffffff));
}
