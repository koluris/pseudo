#include "Global.h"

#define VX(n) (n < 3 ? cpu.CP2D.p[n << 1].isw.l : cpu.CP2D.p[9].isw.l)
#define VY(n) (n < 3 ? cpu.CP2D.p[n << 1].isw.h : cpu.CP2D.p[10].isw.l)
#define VZ(n) (n < 3 ? cpu.CP2D.p[(n << 1) + 1].isw.l : cpu.CP2D.p[11].isw.l)
#define MX11(n) (n < 3 ? cpu.CP2C.p[(n << 3)].isw.l : 0)
#define MX12(n) (n < 3 ? cpu.CP2C.p[(n << 3)].isw.h : 0)
#define MX13(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 1].isw.l : 0)
#define MX21(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 1].isw.h : 0)
#define MX22(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 2].isw.l : 0)
#define MX23(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 2].isw.h : 0)
#define MX31(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 3].isw.l : 0)
#define MX32(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 3].isw.h : 0)
#define MX33(n) (n < 3 ? cpu.CP2C.p[(n << 3) + 4].isw.l : 0)
#define CV1(n) (n < 3 ? (sw)cpu.CP2C.r[(n << 3) + 5] : 0)
#define CV2(n) (n < 3 ? (sw)cpu.CP2C.r[(n << 3) + 6] : 0)
#define CV3(n) (n < 3 ? (sw)cpu.CP2C.r[(n << 3) + 7] : 0)

#define fSX(n) ((cpu.CP2D.p)[((n) + 12)].isw.l)
#define fSY(n) ((cpu.CP2D.p)[((n) + 12)].isw.h)
#define fSZ(n) ((cpu.CP2D.p)[((n) + 17)].iuw.l) /* (n == 0) => SZ1; */

#define gteVXY0 (cpu.CP2D.r[0])
#define gteVX0  (cpu.CP2D.p[0].isw.l)
#define gteVY0  (cpu.CP2D.p[0].isw.h)
#define gteVZ0  (cpu.CP2D.p[1].isw.l)
#define gteVXY1 (cpu.CP2D.r[2])
#define gteVX1  (cpu.CP2D.p[2].isw.l)
#define gteVY1  (cpu.CP2D.p[2].isw.h)
#define gteVZ1  (cpu.CP2D.p[3].isw.l)
#define gteVXY2 (cpu.CP2D.r[4])
#define gteVX2  (cpu.CP2D.p[4].isw.l)
#define gteVY2  (cpu.CP2D.p[4].isw.h)
#define gteVZ2  (cpu.CP2D.p[5].isw.l)
#define gteRGB  (cpu.CP2D.r[6])
#define gteR    (cpu.CP2D.p[6].iub.l)
#define gteG    (cpu.CP2D.p[6].iub.h)
#define gteB    (cpu.CP2D.p[6].iub.h2)
#define gteCODE (cpu.CP2D.p[6].iub.h3)
#define gteOTZ  (cpu.CP2D.p[7].iuw.l)
#define gteIR0  (cpu.CP2D.p[8].isw.l)
#define gteIR1  (cpu.CP2D.p[9].isw.l)
#define gteIR2  (cpu.CP2D.p[10].isw.l)
#define gteIR3  (cpu.CP2D.p[11].isw.l)
#define gteSXY0 (cpu.CP2D.r[12])
#define gteSX0  (cpu.CP2D.p[12].isw.l)
#define gteSY0  (cpu.CP2D.p[12].isw.h)
#define gteSXY1 (cpu.CP2D.r[13])
#define gteSX1  (cpu.CP2D.p[13].isw.l)
#define gteSY1  (cpu.CP2D.p[13].isw.h)
#define gteSXY2 (cpu.CP2D.r[14])
#define gteSX2  (cpu.CP2D.p[14].isw.l)
#define gteSY2  (cpu.CP2D.p[14].isw.h)
#define gteSXYP (cpu.CP2D.r[15])
#define gteSXP  (cpu.CP2D.p[15].isw.l)
#define gteSYP  (cpu.CP2D.p[15].isw.h)
#define gteSZ0  (cpu.CP2D.p[16].iuw.l)
#define gteSZ1  (cpu.CP2D.p[17].iuw.l)
#define gteSZ2  (cpu.CP2D.p[18].iuw.l)
#define gteSZ3  (cpu.CP2D.p[19].iuw.l)
#define gteRGB0  (cpu.CP2D.r[20])
#define gteR0    (cpu.CP2D.p[20].iub.l)
#define gteG0    (cpu.CP2D.p[20].iub.h)
#define gteB0    (cpu.CP2D.p[20].iub.h2)
#define gteCODE0 (cpu.CP2D.p[20].iub.h3)
#define gteRGB1  (cpu.CP2D.r[21])
#define gteR1    (cpu.CP2D.p[21].iub.l)
#define gteG1    (cpu.CP2D.p[21].iub.h)
#define gteB1    (cpu.CP2D.p[21].iub.h2)
#define gteCODE1 (cpu.CP2D.p[21].iub.h3)
#define gteRGB2  (cpu.CP2D.r[22])
#define gteR2    (cpu.CP2D.p[22].iub.l)
#define gteG2    (cpu.CP2D.p[22].iub.h)
#define gteB2    (cpu.CP2D.p[22].iub.h2)
#define gteCODE2 (cpu.CP2D.p[22].iub.h3)
#define gteRES1  (cpu.CP2D.r[23])
#define gteMAC0  (((sw *)cpu.CP2D.r)[24])
#define gteMAC1  (((sw *)cpu.CP2D.r)[25])
#define gteMAC2  (((sw *)cpu.CP2D.r)[26])
#define gteMAC3  (((sw *)cpu.CP2D.r)[27])
#define gteIRGB  (cpu.CP2D.r[28])
#define gteORGB  (cpu.CP2D.r[29])
#define gteLZCS  (cpu.CP2D.r[30])
#define gteLZCR  (cpu.CP2D.r[31])

#define gteR11R12 (((sw *)cpu.CP2C.r)[0])
#define gteR22R23 (((sw *)cpu.CP2C.r)[2])
#define gteR11 (cpu.CP2C.p[0].isw.l)
#define gteR12 (cpu.CP2C.p[0].isw.h)
#define gteR13 (cpu.CP2C.p[1].isw.l)
#define gteR21 (cpu.CP2C.p[1].isw.h)
#define gteR22 (cpu.CP2C.p[2].isw.l)
#define gteR23 (cpu.CP2C.p[2].isw.h)
#define gteR31 (cpu.CP2C.p[3].isw.l)
#define gteR32 (cpu.CP2C.p[3].isw.h)
#define gteR33 (cpu.CP2C.p[4].isw.l)
#define gteTRX (((sw *)cpu.CP2C.r)[5])
#define gteTRY (((sw *)cpu.CP2C.r)[6])
#define gteTRZ (((sw *)cpu.CP2C.r)[7])
#define gteL11 (cpu.CP2C.p[8].isw.l)
#define gteL12 (cpu.CP2C.p[8].isw.h)
#define gteL13 (cpu.CP2C.p[9].isw.l)
#define gteL21 (cpu.CP2C.p[9].isw.h)
#define gteL22 (cpu.CP2C.p[10].isw.l)
#define gteL23 (cpu.CP2C.p[10].isw.h)
#define gteL31 (cpu.CP2C.p[11].isw.l)
#define gteL32 (cpu.CP2C.p[11].isw.h)
#define gteL33 (cpu.CP2C.p[12].isw.l)
#define gteRBK (((sw *)cpu.CP2C.r)[13])
#define gteGBK (((sw *)cpu.CP2C.r)[14])
#define gteBBK (((sw *)cpu.CP2C.r)[15])
#define gteLR1 (cpu.CP2C.p[16].isw.l)
#define gteLR2 (cpu.CP2C.p[16].isw.h)
#define gteLR3 (cpu.CP2C.p[17].isw.l)
#define gteLG1 (cpu.CP2C.p[17].isw.h)
#define gteLG2 (cpu.CP2C.p[18].isw.l)
#define gteLG3 (cpu.CP2C.p[18].isw.h)
#define gteLB1 (cpu.CP2C.p[19].isw.l)
#define gteLB2 (cpu.CP2C.p[19].isw.h)
#define gteLB3 (cpu.CP2C.p[20].isw.l)
#define gteRFC (((sw *)cpu.CP2C.r)[21])
#define gteGFC (((sw *)cpu.CP2C.r)[22])
#define gteBFC (((sw *)cpu.CP2C.r)[23])
#define gteOFX (((sw *)cpu.CP2C.r)[24])
#define gteOFY (((sw *)cpu.CP2C.r)[25])
#define gteH   (cpu.CP2C.p[26].isw.l)
#define gteDQA (cpu.CP2C.p[27].isw.l)
#define gteDQB (((sw *)cpu.CP2C.r)[28])
#define gteZSF3 (cpu.CP2C.p[29].isw.l)
#define gteZSF4 (cpu.CP2C.p[30].isw.l)
#define gteFLAG (cpu.CP2C.r[31])

#define GTE_OP(op) ((op >> 20) & 31)
#define GTE_SF(op) ((op >> 19) & 1)
#define GTE_MX(op) ((op >> 17) & 3)
#define GTE_V(op) ((op >> 15) & 3)
#define GTE_CV(op) ((op >> 13) & 3)
#define GTE_CD(op) ((op >> 11) & 3) /* not used */
#define GTE_LM(op) ((op >> 10) & 1)
#define GTE_CT(op) ((op >> 6) & 15) /* not used */
#define GTE_FUNCT(op) (op & 63)

#define gteop (code & 0x1ffffff)

sd BOUNDS(sd n_value, sd n_max, int n_maxflag, sd n_min, int n_minflag) {
	if (n_value > n_max) {
		gteFLAG |= n_maxflag;
	} else if (n_value < n_min) {
		gteFLAG |= n_minflag;
	}
	return n_value;
}

sw LIM(sw value, sw max, sw min, uw flag) {
	sw ret = value;
	if (value > max) {
		gteFLAG |= flag;
		ret = max;
	} else if (value < min) {
		gteFLAG |= flag;
		ret = min;
	}
	return ret;
}

#define A1(a) BOUNDS((a), 0x7fffffff, (1 << 30), -(sd)0x80000000, (1 << 31) | (1 << 27))
#define A2(a) BOUNDS((a), 0x7fffffff, (1 << 29), -(sd)0x80000000, (1 << 31) | (1 << 26))
#define A3(a) BOUNDS((a), 0x7fffffff, (1 << 28), -(sd)0x80000000, (1 << 31) | (1 << 25))
#define limB1(a, l) LIM((a), 0x7fff, -0x8000 * !l, (1 << 31) | (1 << 24))
#define limB2(a, l) LIM((a), 0x7fff, -0x8000 * !l, (1 << 31) | (1 << 23))
#define limB3(a, l) LIM((a), 0x7fff, -0x8000 * !l, (1 << 22))
#define limC1(a) LIM((a), 0x00ff, 0x0000, (1 << 21))
#define limC2(a) LIM((a), 0x00ff, 0x0000, (1 << 20))
#define limC3(a) LIM((a), 0x00ff, 0x0000, (1 << 19))
#define limD(a) LIM((a), 0xffff, 0x0000, (1 << 31) | (1 << 18))

uw limE(uw result) {
	if (result > 0x1ffff) {
		gteFLAG |= (1 << 31) | (1 << 17);
		return 0x1ffff;
	}
	return result;
}

#define F(a) BOUNDS((a), 0x7fffffff, (1 << 31) | (1 << 16), -(sd)0x80000000, (1 << 31) | (1 << 15))
#define limG1(a) LIM((a), 0x3ff, -0x400, (1 << 31) | (1 << 14))
#define limG2(a) LIM((a), 0x3ff, -0x400, (1 << 31) | (1 << 13))
#define limH(a) LIM((a), 0x1000, 0x0000, (1 << 12))

uw MFC2(int reg) {
	switch (reg) {
		case 1:
		case 3:
		case 5:
		case 8:
		case 9:
		case 10:
		case 11:
			cpu.CP2D.r[reg] = (sw)cpu.CP2D.p[reg].isw.l;
			break;

		case 7:
		case 16:
		case 17:
		case 18:
		case 19:
			cpu.CP2D.r[reg] = (uw)cpu.CP2D.p[reg].iuw.l;
			break;

		case 15:
			cpu.CP2D.r[reg] = gteSXY2;
			break;

		case 28:
		case 29:
			cpu.CP2D.r[reg] = LIM(gteIR1 >> 7, 0x1f, 0, 0) |
									(LIM(gteIR2 >> 7, 0x1f, 0, 0) << 5) |
									(LIM(gteIR3 >> 7, 0x1f, 0, 0) << 10);
			break;
	}
	return cpu.CP2D.r[reg];
}

void MTC2(uw value, int reg) {
	switch (reg) {
		case 15:
			gteSXY0 = gteSXY1;
			gteSXY1 = gteSXY2;
			gteSXY2 = value;
			gteSXYP = value;
			break;

		case 28:
			gteIRGB = value;
			gteIR1 = (value & 0x1f) << 7;
			gteIR2 = (value & 0x3e0) << 2;
			gteIR3 = (value & 0x7c00) >> 3;
			break;

		case 30:
			{
				int a;
				gteLZCS = value;

				a = gteLZCS;
				if (a > 0) {
					int i;
					for (i = 31; (a & (1 << i)) == 0 && i >= 0; i--);
					gteLZCR = 31 - i;
				} else if (a < 0) {
					int i;
					a ^= 0xffffffff;
					for (i = 31; (a & (1 << i)) == 0 && i >= 0; i--);
					gteLZCR = 31 - i;
				} else {
					gteLZCR = 32;
				}
			}
			break;

		case 31:
			return;

		default:
			cpu.CP2D.r[reg] = value;
	}
}

void CTC2(uw value, int reg) {
	switch (reg) {
		case 4:
		case 12:
		case 20:
		case 26:
		case 27:
		case 29:
		case 30:
			value = (sw)(sh)value;
			break;

		case 31:
			value = value & 0x7ffff000;
			if (value & 0x7f87e000) value |= 0x80000000;
			break;
	}

	cpu.CP2C.r[reg] = value;
}

uw DIVIDE(sh n, uh d) {
	if (n >= 0 && n < d * 2) {
		sw n_ = n;
		return ((n_ << 16) + d / 2) / d;
	}
	return 0xffffffff;
}

void psxNULL(uw code) {
}

void gteRTPS(uw code) {
	int quotient;

	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteTRX << 12) + (gteR11 * gteVX0) + (gteR12 * gteVY0) + (gteR13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((sd)gteTRY << 12) + (gteR21 * gteVX0) + (gteR22 * gteVY0) + (gteR23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((sd)gteTRZ << 12) + (gteR31 * gteVX0) + (gteR32 * gteVY0) + (gteR33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 0);
	gteIR2 = limB2(gteMAC2, 0);
	gteIR3 = limB3(gteMAC3, 0);
	gteSZ0 = gteSZ1;
	gteSZ1 = gteSZ2;
	gteSZ2 = gteSZ3;
	gteSZ3 = limD(gteMAC3);
	quotient = limE(DIVIDE(gteH, gteSZ3));
	gteSXY0 = gteSXY1;
	gteSXY1 = gteSXY2;
	gteSX2 = limG1(F((sd)gteOFX + ((sd)gteIR1 * quotient)) >> 16);
	gteSY2 = limG2(F((sd)gteOFY + ((sd)gteIR2 * quotient)) >> 16);

	gteMAC0 = F((sd)gteDQB + ((sd)gteDQA * quotient));
	gteIR0 = limH(gteMAC0 >> 12);
}

void gteRTPT(uw code) {
	int quotient;
	int v;
	sw vx, vy, vz;

	gteFLAG = 0;

	gteSZ0 = gteSZ3;
	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((sd)gteTRX << 12) + (gteR11 * vx) + (gteR12 * vy) + (gteR13 * vz)) >> 12);
		gteMAC2 = A2((((sd)gteTRY << 12) + (gteR21 * vx) + (gteR22 * vy) + (gteR23 * vz)) >> 12);
		gteMAC3 = A3((((sd)gteTRZ << 12) + (gteR31 * vx) + (gteR32 * vy) + (gteR33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 0);
		gteIR2 = limB2(gteMAC2, 0);
		gteIR3 = limB3(gteMAC3, 0);
		fSZ(v) = limD(gteMAC3);
		quotient = limE(DIVIDE(gteH, fSZ(v)));
		fSX(v) = limG1(F((sd)gteOFX + ((sd)gteIR1 * quotient)) >> 16);
		fSY(v) = limG2(F((sd)gteOFY + ((sd)gteIR2 * quotient)) >> 16);
	}
	gteMAC0 = F((sd)gteDQB + ((sd)gteDQA * quotient));
	gteIR0 = limH(gteMAC0 >> 12);
}

void gteMVMVA(uw code) {
	int shift = 12 * GTE_SF(gteop);
	int mx = GTE_MX(gteop);
	int v = GTE_V(gteop);
	int cv = GTE_CV(gteop);
	int lm = GTE_LM(gteop);
	sw vx = VX(v);
	sw vy = VY(v);
	sw vz = VZ(v);

	gteFLAG = 0;

	gteMAC1 = A1((((sd)CV1(cv) << 12) + (MX11(mx) * vx) + (MX12(mx) * vy) + (MX13(mx) * vz)) >> shift);
	gteMAC2 = A2((((sd)CV2(cv) << 12) + (MX21(mx) * vx) + (MX22(mx) * vy) + (MX23(mx) * vz)) >> shift);
	gteMAC3 = A3((((sd)CV3(cv) << 12) + (MX31(mx) * vx) + (MX32(mx) * vy) + (MX33(mx) * vz)) >> shift);

	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);
}

void gteNCLIP(uw code) {
	gteFLAG = 0;

	gteMAC0 = F((sd)gteSX0 * (gteSY1 - gteSY2) +
				gteSX1 * (gteSY2 - gteSY0) +
				gteSX2 * (gteSY0 - gteSY1));
}

void gteAVSZ3(uw code) {
	gteFLAG = 0;

	gteMAC0 = F((sd)(gteZSF3 * gteSZ1) + (gteZSF3 * gteSZ2) + (gteZSF3 * gteSZ3));
	gteOTZ = limD(gteMAC0 >> 12);
}

void gteAVSZ4(uw code) {
	gteFLAG = 0;

	gteMAC0 = F((sd)(gteZSF4 * (gteSZ0 + gteSZ1 + gteSZ2 + gteSZ3)));
	gteOTZ = limD(gteMAC0 >> 12);
}

void gteSQR(uw code) {
	int shift = 12 * GTE_SF(gteop);
	int lm = GTE_LM(gteop);

	gteFLAG = 0;

	gteMAC1 = A1((gteIR1 * gteIR1) >> shift);
	gteMAC2 = A2((gteIR2 * gteIR2) >> shift);
	gteMAC3 = A3((gteIR3 * gteIR3) >> shift);
	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);
}

void gteNCCS(uw code) {
	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteL11 * gteVX0) + (gteL12 * gteVY0) + (gteL13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((sd)gteL21 * gteVX0) + (gteL22 * gteVY0) + (gteL23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((sd)gteL31 * gteVX0) + (gteL32 * gteVY0) + (gteL33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1(((sd)gteR * gteIR1) >> 8);
	gteMAC2 = A2(((sd)gteG * gteIR2) >> 8);
	gteMAC3 = A3(((sd)gteB * gteIR3) >> 8);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteNCCT(uw code) {
	int v;
	sw vx, vy, vz;

	gteFLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((sd)gteL11 * vx) + (gteL12 * vy) + (gteL13 * vz)) >> 12);
		gteMAC2 = A2((((sd)gteL21 * vx) + (gteL22 * vy) + (gteL23 * vz)) >> 12);
		gteMAC3 = A3((((sd)gteL31 * vx) + (gteL32 * vy) + (gteL33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
		gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
		gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1(((sd)gteR * gteIR1) >> 8);
		gteMAC2 = A2(((sd)gteG * gteIR2) >> 8);
		gteMAC3 = A3(((sd)gteB * gteIR3) >> 8);

		gteRGB0 = gteRGB1;
		gteRGB1 = gteRGB2;
		gteCODE2 = gteCODE;
		gteR2 = limC1(gteMAC1 >> 4);
		gteG2 = limC2(gteMAC2 >> 4);
		gteB2 = limC3(gteMAC3 >> 4);
	}
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
}

void gteNCDS(uw code) {
	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteL11 * gteVX0) + (gteL12 * gteVY0) + (gteL13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((sd)gteL21 * gteVX0) + (gteL22 * gteVY0) + (gteL23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((sd)gteL31 * gteVX0) + (gteL32 * gteVY0) + (gteL33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1(((((sd)gteR << 4) * gteIR1) + (gteIR0 * limB1(gteRFC - ((gteR * gteIR1) >> 8), 0))) >> 12);
	gteMAC2 = A2(((((sd)gteG << 4) * gteIR2) + (gteIR0 * limB2(gteGFC - ((gteG * gteIR2) >> 8), 0))) >> 12);
	gteMAC3 = A3(((((sd)gteB << 4) * gteIR3) + (gteIR0 * limB3(gteBFC - ((gteB * gteIR3) >> 8), 0))) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteNCDT(uw code) {
	int v;
	sw vx, vy, vz;

	gteFLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((sd)gteL11 * vx) + (gteL12 * vy) + (gteL13 * vz)) >> 12);
		gteMAC2 = A2((((sd)gteL21 * vx) + (gteL22 * vy) + (gteL23 * vz)) >> 12);
		gteMAC3 = A3((((sd)gteL31 * vx) + (gteL32 * vy) + (gteL33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
		gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
		gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1(((((sd)gteR << 4) * gteIR1) + (gteIR0 * limB1(gteRFC - ((gteR * gteIR1) >> 8), 0))) >> 12);
		gteMAC2 = A2(((((sd)gteG << 4) * gteIR2) + (gteIR0 * limB2(gteGFC - ((gteG * gteIR2) >> 8), 0))) >> 12);
		gteMAC3 = A3(((((sd)gteB << 4) * gteIR3) + (gteIR0 * limB3(gteBFC - ((gteB * gteIR3) >> 8), 0))) >> 12);

		gteRGB0 = gteRGB1;
		gteRGB1 = gteRGB2;
		gteCODE2 = gteCODE;
		gteR2 = limC1(gteMAC1 >> 4);
		gteG2 = limC2(gteMAC2 >> 4);
		gteB2 = limC3(gteMAC3 >> 4);
	}
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
}

void gteOP(uw code) {
	int shift = 12 * GTE_SF(gteop);
	int lm = GTE_LM(gteop);

	gteFLAG = 0;

	gteMAC1 = A1(((sd)(gteR22 * gteIR3) - (gteR33 * gteIR2)) >> shift);
	gteMAC2 = A2(((sd)(gteR33 * gteIR1) - (gteR11 * gteIR3)) >> shift);
	gteMAC3 = A3(((sd)(gteR11 * gteIR2) - (gteR22 * gteIR1)) >> shift);
	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);
}

void gteDCPL(uw code) {
	int lm = GTE_LM(gteop);

	sd RIR1 = ((sd)gteR * gteIR1) >> 8;
	sd GIR2 = ((sd)gteG * gteIR2) >> 8;
	sd BIR3 = ((sd)gteB * gteIR3) >> 8;

	gteFLAG = 0;

	gteMAC1 = A1(RIR1 + ((gteIR0 * limB1(gteRFC - RIR1, 0)) >> 12));
	gteMAC2 = A2(GIR2 + ((gteIR0 * limB1(gteGFC - GIR2, 0)) >> 12));
	gteMAC3 = A3(BIR3 + ((gteIR0 * limB1(gteBFC - BIR3, 0)) >> 12));

	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteGPF(uw code) {
	int shift = 12 * GTE_SF(gteop);

	gteFLAG = 0;

	gteMAC1 = A1(((sd)gteIR0 * gteIR1) >> shift);
	gteMAC2 = A2(((sd)gteIR0 * gteIR2) >> shift);
	gteMAC3 = A3(((sd)gteIR0 * gteIR3) >> shift);
	gteIR1 = limB1(gteMAC1, 0);
	gteIR2 = limB2(gteMAC2, 0);
	gteIR3 = limB3(gteMAC3, 0);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteGPL(uw code) {
	int shift = 12 * GTE_SF(gteop);

	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteMAC1 << shift) + (gteIR0 * gteIR1)) >> shift);
	gteMAC2 = A2((((sd)gteMAC2 << shift) + (gteIR0 * gteIR2)) >> shift);
	gteMAC3 = A3((((sd)gteMAC3 << shift) + (gteIR0 * gteIR3)) >> shift);
	gteIR1 = limB1(gteMAC1, 0);
	gteIR2 = limB2(gteMAC2, 0);
	gteIR3 = limB3(gteMAC3, 0);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteDPCS(uw code) {
	int shift = 12 * GTE_SF(gteop);

	gteFLAG = 0;

	gteMAC1 = A1(((gteR << 16) + (gteIR0 * limB1(A1((sd)gteRFC - (gteR << 4)) << (12 - shift), 0))) >> 12);
	gteMAC2 = A2(((gteG << 16) + (gteIR0 * limB2(A2((sd)gteGFC - (gteG << 4)) << (12 - shift), 0))) >> 12);
	gteMAC3 = A3(((gteB << 16) + (gteIR0 * limB3(A3((sd)gteBFC - (gteB << 4)) << (12 - shift), 0))) >> 12);

	gteIR1 = limB1(gteMAC1, 0);
	gteIR2 = limB2(gteMAC2, 0);
	gteIR3 = limB3(gteMAC3, 0);
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteDPCT(uw code) {
	int v;

	gteFLAG = 0;

	for (v = 0; v < 3; v++) {
		gteMAC1 = A1((((sd)gteR0 << 16) + ((sd)gteIR0 * (limB1(gteRFC - (gteR0 << 4), 0)))) >> 12);
		gteMAC2 = A2((((sd)gteG0 << 16) + ((sd)gteIR0 * (limB1(gteGFC - (gteG0 << 4), 0)))) >> 12);
		gteMAC3 = A3((((sd)gteB0 << 16) + ((sd)gteIR0 * (limB1(gteBFC - (gteB0 << 4), 0)))) >> 12);

		gteRGB0 = gteRGB1;
		gteRGB1 = gteRGB2;
		gteCODE2 = gteCODE;
		gteR2 = limC1(gteMAC1 >> 4);
		gteG2 = limC2(gteMAC2 >> 4);
		gteB2 = limC3(gteMAC3 >> 4);
	}
	gteIR1 = limB1(gteMAC1, 0);
	gteIR2 = limB2(gteMAC2, 0);
	gteIR3 = limB3(gteMAC3, 0);
}

void gteNCS(uw code) {
	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteL11 * gteVX0) + (gteL12 * gteVY0) + (gteL13 * gteVZ0)) >> 12);
	gteMAC2 = A2((((sd)gteL21 * gteVX0) + (gteL22 * gteVY0) + (gteL23 * gteVZ0)) >> 12);
	gteMAC3 = A3((((sd)gteL31 * gteVX0) + (gteL32 * gteVY0) + (gteL33 * gteVZ0)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteNCT(uw code) {
	int v;
	sw vx, vy, vz;

	gteFLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		gteMAC1 = A1((((sd)gteL11 * vx) + (gteL12 * vy) + (gteL13 * vz)) >> 12);
		gteMAC2 = A2((((sd)gteL21 * vx) + (gteL22 * vy) + (gteL23 * vz)) >> 12);
		gteMAC3 = A3((((sd)gteL31 * vx) + (gteL32 * vy) + (gteL33 * vz)) >> 12);
		gteIR1 = limB1(gteMAC1, 1);
		gteIR2 = limB2(gteMAC2, 1);
		gteIR3 = limB3(gteMAC3, 1);
		gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
		gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
		gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
		gteRGB0 = gteRGB1;
		gteRGB1 = gteRGB2;
		gteCODE2 = gteCODE;
		gteR2 = limC1(gteMAC1 >> 4);
		gteG2 = limC2(gteMAC2 >> 4);
		gteB2 = limC3(gteMAC3 >> 4);
	}
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
}

void gteCC(uw code) {
	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1(((sd)gteR * gteIR1) >> 8);
	gteMAC2 = A2(((sd)gteG * gteIR2) >> 8);
	gteMAC3 = A3(((sd)gteB * gteIR3) >> 8);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteINTPL(uw code) {
	int shift = 12 * GTE_SF(gteop);
	int lm = GTE_LM(gteop);

	gteFLAG = 0;

	gteMAC1 = A1(((gteIR1 << 12) + (gteIR0 * limB1(((sd)gteRFC - gteIR1), 0))) >> shift);
	gteMAC2 = A2(((gteIR2 << 12) + (gteIR0 * limB2(((sd)gteGFC - gteIR2), 0))) >> shift);
	gteMAC3 = A3(((gteIR3 << 12) + (gteIR0 * limB3(((sd)gteBFC - gteIR3), 0))) >> shift);
	gteIR1 = limB1(gteMAC1, lm);
	gteIR2 = limB2(gteMAC2, lm);
	gteIR3 = limB3(gteMAC3, lm);
	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void gteCDP(uw code) {
	gteFLAG = 0;

	gteMAC1 = A1((((sd)gteRBK << 12) + (gteLR1 * gteIR1) + (gteLR2 * gteIR2) + (gteLR3 * gteIR3)) >> 12);
	gteMAC2 = A2((((sd)gteGBK << 12) + (gteLG1 * gteIR1) + (gteLG2 * gteIR2) + (gteLG3 * gteIR3)) >> 12);
	gteMAC3 = A3((((sd)gteBBK << 12) + (gteLB1 * gteIR1) + (gteLB2 * gteIR2) + (gteLB3 * gteIR3)) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);
	gteMAC1 = A1(((((sd)gteR << 4) * gteIR1) + (gteIR0 * limB1(gteRFC - ((gteR * gteIR1) >> 8), 0))) >> 12);
	gteMAC2 = A2(((((sd)gteG << 4) * gteIR2) + (gteIR0 * limB2(gteGFC - ((gteG * gteIR2) >> 8), 0))) >> 12);
	gteMAC3 = A3(((((sd)gteB << 4) * gteIR3) + (gteIR0 * limB3(gteBFC - ((gteB * gteIR3) >> 8), 0))) >> 12);
	gteIR1 = limB1(gteMAC1, 1);
	gteIR2 = limB2(gteMAC2, 1);
	gteIR3 = limB3(gteMAC3, 1);

	gteRGB0 = gteRGB1;
	gteRGB1 = gteRGB2;
	gteCODE2 = gteCODE;
	gteR2 = limC1(gteMAC1 >> 4);
	gteG2 = limC2(gteMAC2 >> 4);
	gteB2 = limC3(gteMAC3 >> 4);
}

void (*psxCP2[64])(uw) = {
    psxNULL, gteRTPS , psxNULL , psxNULL, psxNULL, psxNULL , gteNCLIP, psxNULL, // 00
    psxNULL , psxNULL , psxNULL , psxNULL, gteOP  , psxNULL , psxNULL , psxNULL, // 08
    gteDPCS , gteINTPL, gteMVMVA, gteNCDS, gteCDP , psxNULL , gteNCDT , psxNULL, // 10
    psxNULL , psxNULL , psxNULL , gteNCCS, gteCC  , psxNULL , gteNCS  , psxNULL, // 18
    gteNCT  , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 20
    gteSQR  , gteDCPL , gteDPCT , psxNULL, psxNULL, gteAVSZ3, gteAVSZ4, psxNULL, // 28
    gteRTPT , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 30
    psxNULL , psxNULL , psxNULL , psxNULL, psxNULL, gteGPF  , gteGPL  , gteNCCT  // 38
};
