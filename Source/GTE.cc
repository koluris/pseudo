#include "Global.h"


// 32-bit accessor
#define oooo(base, index) \
    base[(index)]

// 16-bit accessor
#define __oo(base, index, offset) \
    base[(index << 1) + offset]

// 08-bit accessor
#define ___o(base, index, offset) \
    base[(index << 2) + offset]

// Cop2c
#define R11R12 oooo(cpu.cop2c.sw__,  0)    /* Rotation matrix */
#define R11    __oo(cpu.cop2c.sh__,  0, 0) /* Rotation matrix */
#define R12    __oo(cpu.cop2c.sh__,  0, 1) /* Rotation matrix */
#define R13    __oo(cpu.cop2c.sh__,  1, 0) /* Rotation matrix */
#define R21    __oo(cpu.cop2c.sh__,  1, 1) /* Rotation matrix */
#define R22R23 oooo(cpu.cop2c.sw__,  2)    /* Rotation matrix */
#define R22    __oo(cpu.cop2c.sh__,  2, 0) /* Rotation matrix */
#define R23    __oo(cpu.cop2c.sh__,  2, 1) /* Rotation matrix */
#define R31    __oo(cpu.cop2c.sh__,  3, 0) /* Rotation matrix */
#define R32    __oo(cpu.cop2c.sh__,  3, 1) /* Rotation matrix */
#define R33    __oo(cpu.cop2c.sh__,  4, 0) /* Rotation matrix */
#define TRX    oooo(cpu.cop2c.sw__,  5)    /* Translation vector (X) */
#define TRY    oooo(cpu.cop2c.sw__,  6)    /* Translation vector (Y) */
#define TRZ    oooo(cpu.cop2c.sw__,  7)    /* Translation vector (Z) */
#define L11    __oo(cpu.cop2c.sh__,  8, 0) /* Lite source direction vector X 3 */
#define L12    __oo(cpu.cop2c.sh__,  8, 1) /* Lite source direction vector X 3 */
#define L13    __oo(cpu.cop2c.sh__,  9, 0) /* Lite source direction vector X 3 */
#define L21    __oo(cpu.cop2c.sh__,  9, 1) /* Lite source direction vector X 3 */
#define L22    __oo(cpu.cop2c.sh__, 10, 0) /* Lite source direction vector X 3 */
#define L23    __oo(cpu.cop2c.sh__, 10, 1) /* Lite source direction vector X 3 */
#define L31    __oo(cpu.cop2c.sh__, 11, 0) /* Lite source direction vector X 3 */
#define L32    __oo(cpu.cop2c.sh__, 11, 1) /* Lite source direction vector X 3 */
#define L33    __oo(cpu.cop2c.sh__, 12, 0) /* Lite source direction vector X 3 */
#define RBK    oooo(cpu.cop2c.sw__, 13)    /* Peripheral color (R) */
#define GBK    oooo(cpu.cop2c.sw__, 14)    /* Peripheral color (G) */
#define BBK    oooo(cpu.cop2c.sw__, 15)    /* Peripheral color (B) */
#define LR1    __oo(cpu.cop2c.sh__, 16, 0) /* Lite source color X 3 */
#define LR2    __oo(cpu.cop2c.sh__, 16, 1) /* Lite source color X 3 */
#define LR3    __oo(cpu.cop2c.sh__, 17, 0) /* Lite source color X 3 */
#define LG1    __oo(cpu.cop2c.sh__, 17, 1) /* Lite source color X 3 */
#define LG2    __oo(cpu.cop2c.sh__, 18, 0) /* Lite source color X 3 */
#define LG3    __oo(cpu.cop2c.sh__, 18, 1) /* Lite source color X 3 */
#define LB1    __oo(cpu.cop2c.sh__, 19, 0) /* Lite source color X 3 */
#define LB2    __oo(cpu.cop2c.sh__, 19, 1) /* Lite source color X 3 */
#define LB3    __oo(cpu.cop2c.sh__, 20, 0) /* Lite source color X 3 */
#define RFC    oooo(cpu.cop2c.sw__, 21)    /* Far color (R) */
#define GFC    oooo(cpu.cop2c.sw__, 22)    /* Far color (G) */
#define BFC    oooo(cpu.cop2c.sw__, 23)    /* Far color (B) */
#define OFX    oooo(cpu.cop2c.sw__, 24)    /* Screen offset (X) */
#define OFY    oooo(cpu.cop2c.sw__, 25)    /* Screen offset (Y) */
#define H      __oo(cpu.cop2c.sh__, 26, 0) /* Screen position */
#define DQA    __oo(cpu.cop2c.sh__, 27, 0) /* Depth parameter A (coefficient) */
#define DQB    oooo(cpu.cop2c.sw__, 28)    /* Depth parameter B (offset) */
#define ZSF3   __oo(cpu.cop2c.sh__, 29, 0) /* Z scale factor */
#define ZSF4   __oo(cpu.cop2c.sh__, 30, 0) /* Z scale factor */
#define FLAG   oooo(cpu.cop2c.uw__, 31)    /* ! */

// Cop2d
#define VXY0   oooo(cpu.cop2d.uw__,  0)    /* Vector #0 (X/Y) */
#define VX0    __oo(cpu.cop2d.sh__,  0, 0) /* Vector #0 (X) */
#define VY0    __oo(cpu.cop2d.sh__,  0, 1) /* Vector #0 (Y) */
#define VZ0    __oo(cpu.cop2d.sh__,  1, 0) /* Vector #0 (Z) */
#define VXY1   oooo(cpu.cop2d.uw__,  2)    /* Vector #1 (X/Y) */
#define VX1    __oo(cpu.cop2d.sh__,  2, 0) /* Vector #1 (X) */
#define VY1    __oo(cpu.cop2d.sh__,  2, 1) /* Vector #1 (Y) */
#define VZ1    __oo(cpu.cop2d.sh__,  3, 0) /* Vector #1 (Z) */
#define VXY2   oooo(cpu.cop2d.uw__,  4)    /* Vector #2 (X/Y) */
#define VX2    __oo(cpu.cop2d.sh__,  4, 0) /* Vector #2 (X) */
#define VY2    __oo(cpu.cop2d.sh__,  4, 1) /* Vector #2 (Y) */
#define VZ2    __oo(cpu.cop2d.sh__,  5, 0) /* Vector #2 (Z) */
#define RGB    oooo(cpu.cop2d.uw__,  6)    /* Color data +  instruction */
#define R      ___o(cpu.cop2d.ub__,  6, 0) /* Color (R) */
#define G      ___o(cpu.cop2d.ub__,  6, 1) /* Color (G) */
#define B      ___o(cpu.cop2d.ub__,  6, 2) /* Color (B) */
#define CODE   ___o(cpu.cop2d.ub__,  6, 3) /*  instruction */
#define OTZ    __oo(cpu.cop2d.uh__,  7, 0) /* Z-component value */
#define IR0    __oo(cpu.cop2d.sh__,  8, 0) /* Intermediate value #0 */
#define IR1    __oo(cpu.cop2d.sh__,  9, 0) /* Intermediate value #1 */
#define IR2    __oo(cpu.cop2d.sh__, 10, 0) /* Intermediate value #2 */
#define IR3    __oo(cpu.cop2d.sh__, 11, 0) /* Intermediate value #3 */
#define SXY0   oooo(cpu.cop2d.uw__, 12)    /* Calc. result record #0 (X/Y) */
#define SX0    __oo(cpu.cop2d.sh__, 12, 0) /* Calc. result record #0 (X) */
#define SY0    __oo(cpu.cop2d.sh__, 12, 1) /* Calc. result record #0 (Y) */
#define SXY1   oooo(cpu.cop2d.uw__, 13)    /* Calc. result record #1 (X/Y) */
#define SX1    __oo(cpu.cop2d.sh__, 13, 0) /* Calc. result record #1 (X) */
#define SY1    __oo(cpu.cop2d.sh__, 13, 1) /* Calc. result record #1 (Y) */
#define SXY2   oooo(cpu.cop2d.uw__, 14)    /* Calc. result record #2 (X/Y) */
#define SX2    __oo(cpu.cop2d.sh__, 14, 0) /* Calc. result record #2 (X) */
#define SY2    __oo(cpu.cop2d.sh__, 14, 1) /* Calc. result record #2 (Y) */
#define SXYP   oooo(cpu.cop2d.uw__, 15)    /* Calc. result set (X/Y) */
#define SXP    __oo(cpu.cop2d.sh__, 15, 0) /* Calc. result set (X) */
#define SYP    __oo(cpu.cop2d.sh__, 15, 1) /* Calc. result set (Y) */
#define SZ0    __oo(cpu.cop2d.uh__, 16, 0) /* Calc. result record #0 (Z) */
#define SZ1    __oo(cpu.cop2d.uh__, 17, 0) /* Calc. result record #1 (Z) */
#define SZ2    __oo(cpu.cop2d.uh__, 18, 0) /* Calc. result record #2 (Z) */
#define SZ3    __oo(cpu.cop2d.uh__, 19, 0) /* Calc. result record #3 (Z) */
#define RGB0   oooo(cpu.cop2d.uw__, 20)    /* Calc. result record #0 (R/G/B) */
#define R0     ___o(cpu.cop2d.ub__, 20, 0) /* Calc. result record #0 (R) */
#define G0     ___o(cpu.cop2d.ub__, 20, 1) /* Calc. result record #0 (G) */
#define B0     ___o(cpu.cop2d.ub__, 20, 2) /* Calc. result record #0 (B) */
#define CODE0  ___o(cpu.cop2d.ub__, 20, 3) /* Calc. result record #0 (?) */
#define RGB1   oooo(cpu.cop2d.uw__, 21)    /* Calc. result record #1 (R/G/B) */
#define R1     ___o(cpu.cop2d.ub__, 21, 0) /* Calc. result record #1 (R) */
#define G1     ___o(cpu.cop2d.ub__, 21, 1) /* Calc. result record #1 (G) */
#define B1     ___o(cpu.cop2d.ub__, 21, 2) /* Calc. result record #1 (B) */
#define CODE1  ___o(cpu.cop2d.ub__, 21, 3) /* Calc. result record #1 (?) */
#define RGB2   oooo(cpu.cop2d.uw__, 22)    /* Calc. result record #2 (R/G/B) */
#define R2     ___o(cpu.cop2d.ub__, 22, 0) /* Calc. result record #2 (R) */
#define G2     ___o(cpu.cop2d.ub__, 22, 1) /* Calc. result record #2 (G) */
#define B2     ___o(cpu.cop2d.ub__, 22, 2) /* Calc. result record #2 (B) */
#define CODE2  ___o(cpu.cop2d.ub__, 22, 3) /* Calc. result record #2 (?) */
#define RES1   oooo(cpu.cop2d.uw__, 23)    /* Reserved (access prohibited) */
#define MAC0   oooo(cpu.cop2d.sw__, 24)    /* Sum of products #0 */
#define MAC1   oooo(cpu.cop2d.sw__, 25)    /* Sum of products #1 */
#define MAC2   oooo(cpu.cop2d.sw__, 26)    /* Sum of products #2 */
#define MAC3   oooo(cpu.cop2d.sw__, 27)    /* Sum of products #3 */
#define IRGB   oooo(cpu.cop2d.uw__, 28)    /* Color data input */
#define ORGB   oooo(cpu.cop2d.uw__, 29)    /* Color data output */
#define LZCS   oooo(cpu.cop2d.uw__, 30)    /* Lead zero/one count source data */
#define LZCR   oooo(cpu.cop2d.uw__, 31)    /* Lead zero/one count process result */

#define MX11(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 0), 0) : 0)
#define MX12(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 0), 1) : 0)
#define MX13(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 1), 0) : 0)
#define MX21(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 1), 1) : 0)
#define MX22(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 2), 0) : 0)
#define MX23(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 2), 1) : 0)
#define MX31(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 3), 0) : 0)
#define MX32(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 3), 1) : 0)
#define MX33(n) (n < 3 ? __oo(cpu.cop2c.sh__, ((n << 3) + 4), 0) : 0)

#define CV1( n) (n < 3 ? oooo(cpu.cop2c.sw__, ((n << 3) + 5)) : 0)
#define CV2( n) (n < 3 ? oooo(cpu.cop2c.sw__, ((n << 3) + 6)) : 0)
#define CV3( n) (n < 3 ? oooo(cpu.cop2c.sw__, ((n << 3) + 7)) : 0)

#define VX(  n) (n < 3 ? __oo(cpu.cop2d.sh__, ((n << 1) + 0), 0) : __oo(cpu.cop2d.sh__,  9, 0))
#define VY(  n) (n < 3 ? __oo(cpu.cop2d.sh__, ((n << 1) + 0), 1) : __oo(cpu.cop2d.sh__, 10, 0))
#define VZ(  n) (n < 3 ? __oo(cpu.cop2d.sh__, ((n << 1) + 1), 0) : __oo(cpu.cop2d.sh__, 11, 0))

#define SX(n) __oo(cpu.cop2d.sh__, (n + 12), 0)
#define SY(n) __oo(cpu.cop2d.sh__, (n + 12), 1)
#define SZ(n) __oo(cpu.cop2d.uh__, (n + 17), 0)

#define LIM(a, min, max, bit) \
    (((sw)(a) < min) ? (FLAG |= (1 << bit), min) : \
    (((sw)(a) > max) ? (FLAG |= (1 << bit), max) : ((sw)(a))))

#define limB1(a, l) LIM((a), !l * -32768, 32767, 24)
#define limB2(a, l) LIM((a), !l * -32768, 32767, 23)
#define limB3(a, l) LIM((a), !l * -32768, 32767, 22)
#define limC1(a) LIM((a),       0,    255, 21)
#define limC2(a) LIM((a),       0,    255, 20)
#define limC3(a) LIM((a),       0,    255, 19)
#define limD( a) LIM((a),       0,  65535, 18)
#define limE( a) LIM((a), -131072, 131071, 17)
#define limG1(a) LIM((a),   -1024,   1023, 14)
#define limG2(a) LIM((a),   -1024,   1023, 13)
#define limH( a) LIM((a),       0,   4096, 12)

#define _SF(op) ((op >> 19) & 1)
#define _MX(op) ((op >> 17) & 3)
#define _V( op) ((op >> 15) & 3)
#define _CV(op) ((op >> 13) & 3)
#define _LM(op) ((op >> 10) & 1)

#define MAC2IR(n) \
    IR1 = limB1(MAC1, n); \
    IR2 = limB2(MAC2, n); \
    IR3 = limB3(MAC3, n)

#define MAC2RGB4() \
    RGB0  = RGB1; \
    RGB1  = RGB2; \
    CODE2 = CODE; \
    \
    R2 = limC1(MAC1 >> 4); \
    G2 = limC2(MAC2 >> 4); \
    B2 = limC3(MAC3 >> 4)

#define op (code & 0x1ffffff)

//sd BOUNDS(sd n_value, sd n_max, int n_maxflag, sd n_min, int n_minflag) {
//	if (n_value > n_max) {
//		FLAG |= n_maxflag;
//	} else if (n_value < n_min) {
//		FLAG |= n_minflag;
//	}
//	return n_value;
//}

#define A1(a) (a)//BOUNDS((a), 0x7fffffff, (1 << 30), -(sd)0x80000000, (1 << 31) | (1 << 27))
#define A2(a) (a)//BOUNDS((a), 0x7fffffff, (1 << 29), -(sd)0x80000000, (1 << 31) | (1 << 26))
#define A3(a) (a)//BOUNDS((a), 0x7fffffff, (1 << 28), -(sd)0x80000000, (1 << 31) | (1 << 25))

#define F(a) (a)//BOUNDS((a), 0x7fffffff, (1 << 31) | (1 << 16), -(sd)0x80000000, (1 << 31) | (1 << 15))

uw MFC2(int addr) {
	switch (addr) {
		case 1:
		case 3:
		case 5:
		case 8:
		case 9:
		case 10:
		case 11:
            oooo(cpu.cop2d.sw__, addr) = __oo(cpu.cop2d.sh__, addr, 0);
			break;

		case 7:
		case 16:
		case 17:
		case 18:
		case 19:
			oooo(cpu.cop2d.uw__, addr) = __oo(cpu.cop2d.uh__, addr, 0);
			break;

		case 15:
            oooo(cpu.cop2d.uw__, addr) = SXY2;
			break;

		case 28:
		case 29:
            oooo(cpu.cop2d.uw__, addr) = LIM(IR1 >> 7, 0, 0x1f, 0) |
									(LIM(IR2 >> 7, 0, 0x1f, 0) << 5) |
									(LIM(IR3 >> 7, 0, 0x1f, 0) << 10);
			break;
	}
    return oooo(cpu.cop2d.uw__, addr);
}

void MTC2(int addr, uw data) {
	switch (addr) {
		case 15:
			SXY0 = SXY1;
			SXY1 = SXY2;
			SXY2 = data;
			SXYP = data;
			break;

		case 28:
			IRGB = data;
			IR1 = (data & 0x1f) << 7;
			IR2 = (data & 0x3e0) << 2;
			IR3 = (data & 0x7c00) >> 3;
			break;

		case 30:
			{
				int a;
				LZCS = data;

				a = LZCS;
				if (a > 0) {
					int i;
					for (i = 31; (a & (1 << i)) == 0 && i >= 0; i--);
					LZCR = 31 - i;
				} else if (a < 0) {
					int i;
					a ^= 0xffffffff;
					for (i = 31; (a & (1 << i)) == 0 && i >= 0; i--);
					LZCR = 31 - i;
				} else {
					LZCR = 32;
				}
			}
			break;

		case 31:
			return;

		default:
            oooo(cpu.cop2d.uw__, addr) = data;
	}
}

void CTC2(int addr, uw data) {
	switch (addr) {
		case 4:
		case 12:
		case 20:
		case 26:
		case 27:
		case 29:
		case 30:
            data = (sw)(sh)data;
			break;

		case 31:
            data = data & 0x7ffff000;
			if (data & 0x7f87e000) data |= 0x80000000;
			break;
	}

    oooo(cpu.cop2c.uw__, addr) = data;
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

void RTPS(uw code) {
	int quotient;

	FLAG = 0;

	MAC1 = A1((((sd)TRX << 12) + (R11 * VX0) + (R12 * VY0) + (R13 * VZ0)) >> 12);
	MAC2 = A2((((sd)TRY << 12) + (R21 * VX0) + (R22 * VY0) + (R23 * VZ0)) >> 12);
	MAC3 = A3((((sd)TRZ << 12) + (R31 * VX0) + (R32 * VY0) + (R33 * VZ0)) >> 12);
	IR1 = limB1(MAC1, 0);
	IR2 = limB2(MAC2, 0);
	IR3 = limB3(MAC3, 0);
	SZ0 = SZ1;
	SZ1 = SZ2;
	SZ2 = SZ3;
	SZ3 = limD(MAC3);
	quotient = limE(DIVIDE(H, SZ3));
	SXY0 = SXY1;
	SXY1 = SXY2;
	SX2 = limG1(F((sd)OFX + ((sd)IR1 * quotient)) >> 16);
	SY2 = limG2(F((sd)OFY + ((sd)IR2 * quotient)) >> 16);

	MAC0 = F((sd)DQB + ((sd)DQA * quotient));
	IR0 = limH(MAC0 >> 12);
}

void RTPT(uw code) {
	int quotient;
	int v;
	sw vx, vy, vz;

	FLAG = 0;

	SZ0 = SZ3;
	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		MAC1 = A1((((sd)TRX << 12) + (R11 * vx) + (R12 * vy) + (R13 * vz)) >> 12);
		MAC2 = A2((((sd)TRY << 12) + (R21 * vx) + (R22 * vy) + (R23 * vz)) >> 12);
		MAC3 = A3((((sd)TRZ << 12) + (R31 * vx) + (R32 * vy) + (R33 * vz)) >> 12);
		IR1 = limB1(MAC1, 0);
		IR2 = limB2(MAC2, 0);
		IR3 = limB3(MAC3, 0);
		SZ(v) = limD(MAC3);
		quotient = limE(DIVIDE(H, SZ(v)));
		SX(v) = limG1(F((sd)OFX + ((sd)IR1 * quotient)) >> 16);
		SY(v) = limG2(F((sd)OFY + ((sd)IR2 * quotient)) >> 16);
	}
	MAC0 = F((sd)DQB + ((sd)DQA * quotient));
	IR0 = limH(MAC0 >> 12);
}

void MVMVA(uw code) {
	int shift = 12 * _SF(op);
	int mx = _MX(op);
	int v = _V(op);
	int cv = _CV(op);
	int lm = _LM(op);
	sw vx = VX(v);
	sw vy = VY(v);
	sw vz = VZ(v);

	FLAG = 0;

	MAC1 = A1((((sd)CV1(cv) << 12) + (MX11(mx) * vx) + (MX12(mx) * vy) + (MX13(mx) * vz)) >> shift);
	MAC2 = A2((((sd)CV2(cv) << 12) + (MX21(mx) * vx) + (MX22(mx) * vy) + (MX23(mx) * vz)) >> shift);
	MAC3 = A3((((sd)CV3(cv) << 12) + (MX31(mx) * vx) + (MX32(mx) * vy) + (MX33(mx) * vz)) >> shift);

	IR1 = limB1(MAC1, lm);
	IR2 = limB2(MAC2, lm);
	IR3 = limB3(MAC3, lm);
}

void NCLIP(uw code) {
	FLAG = 0;

	MAC0 = F((sd)SX0 * (SY1 - SY2) +
				SX1 * (SY2 - SY0) +
				SX2 * (SY0 - SY1));
}

void AVSZ3(uw code) {
	FLAG = 0;

	MAC0 = F((sd)(ZSF3 * SZ1) + (ZSF3 * SZ2) + (ZSF3 * SZ3));
	OTZ = limD(MAC0 >> 12);
}

void AVSZ4(uw code) {
	FLAG = 0;

	MAC0 = F((sd)(ZSF4 * (SZ0 + SZ1 + SZ2 + SZ3)));
	OTZ = limD(MAC0 >> 12);
}

void SQR(uw code) {
	int shift = 12 * _SF(op);
	int lm = _LM(op);

	FLAG = 0;

	MAC1 = A1((IR1 * IR1) >> shift);
	MAC2 = A2((IR2 * IR2) >> shift);
	MAC3 = A3((IR3 * IR3) >> shift);
	IR1 = limB1(MAC1, lm);
	IR2 = limB2(MAC2, lm);
	IR3 = limB3(MAC3, lm);
}

void NCCS(uw code) {
	FLAG = 0;

	MAC1 = A1((((sd)L11 * VX0) + (L12 * VY0) + (L13 * VZ0)) >> 12);
	MAC2 = A2((((sd)L21 * VX0) + (L22 * VY0) + (L23 * VZ0)) >> 12);
	MAC3 = A3((((sd)L31 * VX0) + (L32 * VY0) + (L33 * VZ0)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
	MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
	MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1(((sd)R * IR1) >> 8);
	MAC2 = A2(((sd)G * IR2) >> 8);
	MAC3 = A3(((sd)B * IR3) >> 8);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void NCCT(uw code) {
	int v;
	sw vx, vy, vz;

	FLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		MAC1 = A1((((sd)L11 * vx) + (L12 * vy) + (L13 * vz)) >> 12);
		MAC2 = A2((((sd)L21 * vx) + (L22 * vy) + (L23 * vz)) >> 12);
		MAC3 = A3((((sd)L31 * vx) + (L32 * vy) + (L33 * vz)) >> 12);
		IR1 = limB1(MAC1, 1);
		IR2 = limB2(MAC2, 1);
		IR3 = limB3(MAC3, 1);
		MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
		MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
		MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
		IR1 = limB1(MAC1, 1);
		IR2 = limB2(MAC2, 1);
		IR3 = limB3(MAC3, 1);
		MAC1 = A1(((sd)R * IR1) >> 8);
		MAC2 = A2(((sd)G * IR2) >> 8);
		MAC3 = A3(((sd)B * IR3) >> 8);

		RGB0 = RGB1;
		RGB1 = RGB2;
		CODE2 = CODE;
		R2 = limC1(MAC1 >> 4);
		G2 = limC2(MAC2 >> 4);
		B2 = limC3(MAC3 >> 4);
	}
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
}

void NCDS(uw code) {
	FLAG = 0;

	MAC1 = A1((((sd)L11 * VX0) + (L12 * VY0) + (L13 * VZ0)) >> 12);
	MAC2 = A2((((sd)L21 * VX0) + (L22 * VY0) + (L23 * VZ0)) >> 12);
	MAC3 = A3((((sd)L31 * VX0) + (L32 * VY0) + (L33 * VZ0)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
	MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
	MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1(((((sd)R << 4) * IR1) + (IR0 * limB1(RFC - ((R * IR1) >> 8), 0))) >> 12);
	MAC2 = A2(((((sd)G << 4) * IR2) + (IR0 * limB2(GFC - ((G * IR2) >> 8), 0))) >> 12);
	MAC3 = A3(((((sd)B << 4) * IR3) + (IR0 * limB3(BFC - ((B * IR3) >> 8), 0))) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void NCDT(uw code) {
	int v;
	sw vx, vy, vz;

	FLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		MAC1 = A1((((sd)L11 * vx) + (L12 * vy) + (L13 * vz)) >> 12);
		MAC2 = A2((((sd)L21 * vx) + (L22 * vy) + (L23 * vz)) >> 12);
		MAC3 = A3((((sd)L31 * vx) + (L32 * vy) + (L33 * vz)) >> 12);
		IR1 = limB1(MAC1, 1);
		IR2 = limB2(MAC2, 1);
		IR3 = limB3(MAC3, 1);
		MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
		MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
		MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
		IR1 = limB1(MAC1, 1);
		IR2 = limB2(MAC2, 1);
		IR3 = limB3(MAC3, 1);
		MAC1 = A1(((((sd)R << 4) * IR1) + (IR0 * limB1(RFC - ((R * IR1) >> 8), 0))) >> 12);
		MAC2 = A2(((((sd)G << 4) * IR2) + (IR0 * limB2(GFC - ((G * IR2) >> 8), 0))) >> 12);
		MAC3 = A3(((((sd)B << 4) * IR3) + (IR0 * limB3(BFC - ((B * IR3) >> 8), 0))) >> 12);

		RGB0 = RGB1;
		RGB1 = RGB2;
		CODE2 = CODE;
		R2 = limC1(MAC1 >> 4);
		G2 = limC2(MAC2 >> 4);
		B2 = limC3(MAC3 >> 4);
	}
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
}

void OP(uw code) {
	int shift = 12 * _SF(op);
	int lm = _LM(op);

	FLAG = 0;

	MAC1 = A1(((sd)(R22 * IR3) - (R33 * IR2)) >> shift);
	MAC2 = A2(((sd)(R33 * IR1) - (R11 * IR3)) >> shift);
	MAC3 = A3(((sd)(R11 * IR2) - (R22 * IR1)) >> shift);
	IR1 = limB1(MAC1, lm);
	IR2 = limB2(MAC2, lm);
	IR3 = limB3(MAC3, lm);
}

void DCPL(uw code) {
	int lm = _LM(op);

	sd RIR1 = ((sd)R * IR1) >> 8;
	sd GIR2 = ((sd)G * IR2) >> 8;
	sd BIR3 = ((sd)B * IR3) >> 8;

	FLAG = 0;

	MAC1 = A1(RIR1 + ((IR0 * limB1(RFC - RIR1, 0)) >> 12));
	MAC2 = A2(GIR2 + ((IR0 * limB1(GFC - GIR2, 0)) >> 12));
	MAC3 = A3(BIR3 + ((IR0 * limB1(BFC - BIR3, 0)) >> 12));

	IR1 = limB1(MAC1, lm);
	IR2 = limB2(MAC2, lm);
	IR3 = limB3(MAC3, lm);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void GPF(uw code) {
	int shift = 12 * _SF(op);

	FLAG = 0;

	MAC1 = A1(((sd)IR0 * IR1) >> shift);
	MAC2 = A2(((sd)IR0 * IR2) >> shift);
	MAC3 = A3(((sd)IR0 * IR3) >> shift);
	IR1 = limB1(MAC1, 0);
	IR2 = limB2(MAC2, 0);
	IR3 = limB3(MAC3, 0);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void GPL(uw code) {
	int shift = 12 * _SF(op);

	FLAG = 0;

	MAC1 = A1((((sd)MAC1 << shift) + (IR0 * IR1)) >> shift);
	MAC2 = A2((((sd)MAC2 << shift) + (IR0 * IR2)) >> shift);
	MAC3 = A3((((sd)MAC3 << shift) + (IR0 * IR3)) >> shift);
	IR1 = limB1(MAC1, 0);
	IR2 = limB2(MAC2, 0);
	IR3 = limB3(MAC3, 0);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void DPCS(uw code) {
	int shift = 12 * _SF(op);

	FLAG = 0;

	MAC1 = A1(((R << 16) + (IR0 * limB1(A1((sd)RFC - (R << 4)) << (12 - shift), 0))) >> 12);
	MAC2 = A2(((G << 16) + (IR0 * limB2(A2((sd)GFC - (G << 4)) << (12 - shift), 0))) >> 12);
	MAC3 = A3(((B << 16) + (IR0 * limB3(A3((sd)BFC - (B << 4)) << (12 - shift), 0))) >> 12);

	IR1 = limB1(MAC1, 0);
	IR2 = limB2(MAC2, 0);
	IR3 = limB3(MAC3, 0);
	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void DPCT(uw code) {
	int v;

	FLAG = 0;

	for (v = 0; v < 3; v++) {
		MAC1 = A1((((sd)R0 << 16) + ((sd)IR0 * (limB1(RFC - (R0 << 4), 0)))) >> 12);
		MAC2 = A2((((sd)G0 << 16) + ((sd)IR0 * (limB1(GFC - (G0 << 4), 0)))) >> 12);
		MAC3 = A3((((sd)B0 << 16) + ((sd)IR0 * (limB1(BFC - (B0 << 4), 0)))) >> 12);

		RGB0 = RGB1;
		RGB1 = RGB2;
		CODE2 = CODE;
		R2 = limC1(MAC1 >> 4);
		G2 = limC2(MAC2 >> 4);
		B2 = limC3(MAC3 >> 4);
	}
	IR1 = limB1(MAC1, 0);
	IR2 = limB2(MAC2, 0);
	IR3 = limB3(MAC3, 0);
}

void NCS(uw code) {
	FLAG = 0;

	MAC1 = A1((((sd)L11 * VX0) + (L12 * VY0) + (L13 * VZ0)) >> 12);
	MAC2 = A2((((sd)L21 * VX0) + (L22 * VY0) + (L23 * VZ0)) >> 12);
	MAC3 = A3((((sd)L31 * VX0) + (L32 * VY0) + (L33 * VZ0)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
	MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
	MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void NCT(uw code) {
	int v;
	sw vx, vy, vz;

	FLAG = 0;

	for (v = 0; v < 3; v++) {
		vx = VX(v);
		vy = VY(v);
		vz = VZ(v);
		MAC1 = A1((((sd)L11 * vx) + (L12 * vy) + (L13 * vz)) >> 12);
		MAC2 = A2((((sd)L21 * vx) + (L22 * vy) + (L23 * vz)) >> 12);
		MAC3 = A3((((sd)L31 * vx) + (L32 * vy) + (L33 * vz)) >> 12);
		IR1 = limB1(MAC1, 1);
		IR2 = limB2(MAC2, 1);
		IR3 = limB3(MAC3, 1);
		MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
		MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
		MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
		RGB0 = RGB1;
		RGB1 = RGB2;
		CODE2 = CODE;
		R2 = limC1(MAC1 >> 4);
		G2 = limC2(MAC2 >> 4);
		B2 = limC3(MAC3 >> 4);
	}
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
}

void CC(uw code) {
	FLAG = 0;

	MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
	MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
	MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1(((sd)R * IR1) >> 8);
	MAC2 = A2(((sd)G * IR2) >> 8);
	MAC3 = A3(((sd)B * IR3) >> 8);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void INTPL(uw code) {
	int shift = 12 * _SF(op);
	int lm = _LM(op);

	FLAG = 0;

	MAC1 = A1(((IR1 << 12) + (IR0 * limB1(((sd)RFC - IR1), 0))) >> shift);
	MAC2 = A2(((IR2 << 12) + (IR0 * limB2(((sd)GFC - IR2), 0))) >> shift);
	MAC3 = A3(((IR3 << 12) + (IR0 * limB3(((sd)BFC - IR3), 0))) >> shift);
	IR1 = limB1(MAC1, lm);
	IR2 = limB2(MAC2, lm);
	IR3 = limB3(MAC3, lm);
	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void CDP(uw code) {
	FLAG = 0;

	MAC1 = A1((((sd)RBK << 12) + (LR1 * IR1) + (LR2 * IR2) + (LR3 * IR3)) >> 12);
	MAC2 = A2((((sd)GBK << 12) + (LG1 * IR1) + (LG2 * IR2) + (LG3 * IR3)) >> 12);
	MAC3 = A3((((sd)BBK << 12) + (LB1 * IR1) + (LB2 * IR2) + (LB3 * IR3)) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);
	MAC1 = A1(((((sd)R << 4) * IR1) + (IR0 * limB1(RFC - ((R * IR1) >> 8), 0))) >> 12);
	MAC2 = A2(((((sd)G << 4) * IR2) + (IR0 * limB2(GFC - ((G * IR2) >> 8), 0))) >> 12);
	MAC3 = A3(((((sd)B << 4) * IR3) + (IR0 * limB3(BFC - ((B * IR3) >> 8), 0))) >> 12);
	IR1 = limB1(MAC1, 1);
	IR2 = limB2(MAC2, 1);
	IR3 = limB3(MAC3, 1);

	RGB0 = RGB1;
	RGB1 = RGB2;
	CODE2 = CODE;
	R2 = limC1(MAC1 >> 4);
	G2 = limC2(MAC2 >> 4);
	B2 = limC3(MAC3 >> 4);
}

void (*psxCP2[64])(uw) = {
    psxNULL, RTPS , psxNULL , psxNULL, psxNULL, psxNULL , NCLIP, psxNULL, // 00
    psxNULL , psxNULL , psxNULL , psxNULL, OP  , psxNULL , psxNULL , psxNULL, // 08
    DPCS , INTPL, MVMVA, NCDS, CDP , psxNULL , NCDT , psxNULL, // 10
    psxNULL , psxNULL , psxNULL , NCCS, CC  , psxNULL , NCS  , psxNULL, // 18
    NCT  , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 20
    SQR  , DCPL , DPCT , psxNULL, psxNULL, AVSZ3, AVSZ4, psxNULL, // 28
    RTPT , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 30
    psxNULL , psxNULL , psxNULL , psxNULL, psxNULL, GPF  , GPL  , NCCT  // 38
};
