#if 0

// Cop2d
#define VXY0   oooo(cop2d.iuw,  0)    /* Vector #0 (X/Y) */
#define VX0    __oo(cop2d.ish,  0, 0) /* Vector #0 (X) */
#define VY0    __oo(cop2d.ish,  0, 1) /* Vector #0 (Y) */
#define VZ0    __oo(cop2d.ish,  1, 0) /* Vector #0 (Z) */
#define VXY1   oooo(cop2d.iuw,  2)    /* Vector #1 (X/Y) */
#define VX1    __oo(cop2d.ish,  2, 0) /* Vector #1 (X) */
#define VY1    __oo(cop2d.ish,  2, 1) /* Vector #1 (Y) */
#define VZ1    __oo(cop2d.ish,  3, 0) /* Vector #1 (Z) */
#define VXY2   oooo(cop2d.iuw,  4)    /* Vector #2 (X/Y) */
#define VX2    __oo(cop2d.ish,  4, 0) /* Vector #2 (X) */
#define VY2    __oo(cop2d.ish,  4, 1) /* Vector #2 (Y) */
#define VZ2    __oo(cop2d.ish,  5, 0) /* Vector #2 (Z) */
#define RGB    oooo(cop2d.iuw,  6)    /* Color data + GTE instruction */
#define R      ___o(cop2d.iub,  6, 0) /* Color (R) */
#define G      ___o(cop2d.iub,  6, 1) /* Color (G) */
#define B      ___o(cop2d.iub,  6, 2) /* Color (B) */
#define CODE   ___o(cop2d.iub,  6, 3) /* GTE instruction */
#define OTZ    __oo(cop2d.iuh,  7, 0) /* Z-component value */
#define IR0    __oo(cop2d.ish,  8, 0) /* Intermediate value #0 */
#define IR1    __oo(cop2d.ish,  9, 0) /* Intermediate value #1 */
#define IR2    __oo(cop2d.ish, 10, 0) /* Intermediate value #2 */
#define IR3    __oo(cop2d.ish, 11, 0) /* Intermediate value #3 */
#define SXY0   oooo(cop2d.iuw, 12)    /* Calc. result record #0 (X/Y) */
#define SX0    __oo(cop2d.ish, 12, 0) /* Calc. result record #0 (X) */
#define SY0    __oo(cop2d.ish, 12, 1) /* Calc. result record #0 (Y) */
#define SXY1   oooo(cop2d.iuw, 13)    /* Calc. result record #1 (X/Y) */
#define SX1    __oo(cop2d.ish, 13, 0) /* Calc. result record #1 (X) */
#define SY1    __oo(cop2d.ish, 13, 1) /* Calc. result record #1 (Y) */
#define SXY2   oooo(cop2d.iuw, 14)    /* Calc. result record #2 (X/Y) */
#define SX2    __oo(cop2d.ish, 14, 0) /* Calc. result record #2 (X) */
#define SY2    __oo(cop2d.ish, 14, 1) /* Calc. result record #2 (Y) */
#define SXYP   oooo(cop2d.iuw, 15)    /* Calc. result set (X/Y) */
#define SXP    __oo(cop2d.ish, 15, 0) /* Calc. result set (X) */
#define SYP    __oo(cop2d.ish, 15, 1) /* Calc. result set (Y) */
#define SZ0    __oo(cop2d.iuh, 16, 0) /* Calc. result record #0 (Z) */
#define SZ1    __oo(cop2d.iuh, 17, 0) /* Calc. result record #1 (Z) */
#define SZ2    __oo(cop2d.iuh, 18, 0) /* Calc. result record #2 (Z) */
#define SZ3    __oo(cop2d.iuh, 19, 0) /* Calc. result record #3 (Z) */
#define RGB0   oooo(cop2d.iuw, 20)    /* Calc. result record #0 (R/G/B) */
#define R0     ___o(cop2d.iub, 20, 0) /* Calc. result record #0 (R) */
#define G0     ___o(cop2d.iub, 20, 1) /* Calc. result record #0 (G) */
#define B0     ___o(cop2d.iub, 20, 2) /* Calc. result record #0 (B) */
#define CODE0  ___o(cop2d.iub, 20, 3) /* Calc. result record #0 (?) */
#define RGB1   oooo(cop2d.iuw, 21)    /* Calc. result record #1 (R/G/B) */
#define R1     ___o(cop2d.iub, 21, 0) /* Calc. result record #1 (R) */
#define G1     ___o(cop2d.iub, 21, 1) /* Calc. result record #1 (G) */
#define B1     ___o(cop2d.iub, 21, 2) /* Calc. result record #1 (B) */
#define CODE1  ___o(cop2d.iub, 21, 3) /* Calc. result record #1 (?) */
#define RGB2   oooo(cop2d.iuw, 22)    /* Calc. result record #2 (R/G/B) */
#define R2     ___o(cop2d.iub, 22, 0) /* Calc. result record #2 (R) */
#define G2     ___o(cop2d.iub, 22, 1) /* Calc. result record #2 (G) */
#define B2     ___o(cop2d.iub, 22, 2) /* Calc. result record #2 (B) */
#define CODE2  ___o(cop2d.iub, 22, 3) /* Calc. result record #2 (?) */
#define RES1   oooo(cop2d.iuw, 23)    /* Reserved (access prohibited) */
#define MAC0   oooo(cop2d.isw, 24)    /* Sum of products #0 */
#define MAC1   oooo(cop2d.isw, 25)    /* Sum of products #1 */
#define MAC2   oooo(cop2d.isw, 26)    /* Sum of products #2 */
#define MAC3   oooo(cop2d.isw, 27)    /* Sum of products #3 */
#define IRGB   oooo(cop2d.iuw, 28)    /* Color data input */
#define ORGB   oooo(cop2d.iuw, 29)    /* Color data output */
#define LZCS   oooo(cop2d.iuw, 30)    /* Lead zero/one count source data */
#define LZCR   oooo(cop2d.iuw, 31)    /* Lead zero/one count process result */

// Cop2c
#define R11R12 oooo(cop2c.isw,  0)    /* Rotation matrix */
#define R11    __oo(cop2c.ish,  0, 0) /* Rotation matrix */
#define R12    __oo(cop2c.ish,  0, 1) /* Rotation matrix */
#define R13    __oo(cop2c.ish,  1, 0) /* Rotation matrix */
#define R21    __oo(cop2c.ish,  1, 1) /* Rotation matrix */
#define R22R23 oooo(cop2c.isw,  2)    /* Rotation matrix */
#define R22    __oo(cop2c.ish,  2, 0) /* Rotation matrix */
#define R23    __oo(cop2c.ish,  2, 1) /* Rotation matrix */
#define R31    __oo(cop2c.ish,  3, 0) /* Rotation matrix */
#define R32    __oo(cop2c.ish,  3, 1) /* Rotation matrix */
#define R33    __oo(cop2c.ish,  4, 0) /* Rotation matrix */
#define TRX    oooo(cop2c.isw,  5)    /* Translation vector (X) */
#define TRY    oooo(cop2c.isw,  6)    /* Translation vector (Y) */
#define TRZ    oooo(cop2c.isw,  7)    /* Translation vector (Z) */
#define L11    __oo(cop2c.ish,  8, 0) /* Lite source direction vector X 3 */
#define L12    __oo(cop2c.ish,  8, 1) /* Lite source direction vector X 3 */
#define L13    __oo(cop2c.ish,  9, 0) /* Lite source direction vector X 3 */
#define L21    __oo(cop2c.ish,  9, 1) /* Lite source direction vector X 3 */
#define L22    __oo(cop2c.ish, 10, 0) /* Lite source direction vector X 3 */
#define L23    __oo(cop2c.ish, 10, 1) /* Lite source direction vector X 3 */
#define L31    __oo(cop2c.ish, 11, 0) /* Lite source direction vector X 3 */
#define L32    __oo(cop2c.ish, 11, 1) /* Lite source direction vector X 3 */
#define L33    __oo(cop2c.ish, 12, 0) /* Lite source direction vector X 3 */
#define RBK    oooo(cop2c.isw, 13)    /* Peripheral color (R) */
#define GBK    oooo(cop2c.isw, 14)    /* Peripheral color (G) */
#define BBK    oooo(cop2c.isw, 15)    /* Peripheral color (B) */
#define LR1    __oo(cop2c.ish, 16, 0) /* Lite source color X 3 */
#define LR2    __oo(cop2c.ish, 16, 1) /* Lite source color X 3 */
#define LR3    __oo(cop2c.ish, 17, 0) /* Lite source color X 3 */
#define LG1    __oo(cop2c.ish, 17, 1) /* Lite source color X 3 */
#define LG2    __oo(cop2c.ish, 18, 0) /* Lite source color X 3 */
#define LG3    __oo(cop2c.ish, 18, 1) /* Lite source color X 3 */
#define LB1    __oo(cop2c.ish, 19, 0) /* Lite source color X 3 */
#define LB2    __oo(cop2c.ish, 19, 1) /* Lite source color X 3 */
#define LB3    __oo(cop2c.ish, 20, 0) /* Lite source color X 3 */
#define RFC    oooo(cop2c.isw, 21)    /* Far color (R) */
#define GFC    oooo(cop2c.isw, 22)    /* Far color (G) */
#define BFC    oooo(cop2c.isw, 23)    /* Far color (B) */
#define OFX    oooo(cop2c.isw, 24)    /* Screen offset (X) */
#define OFY    oooo(cop2c.isw, 25)    /* Screen offset (Y) */
#define H      __oo(cop2c.ish, 26, 0) /* Screen position */
#define DQA    __oo(cop2c.ish, 27, 0) /* Depth parameter A (coefficient) */
#define DQB    oooo(cop2c.isw, 28)    /* Depth parameter B (offset) */
#define ZSF3   __oo(cop2c.ish, 29, 0) /* Z scale factor */
#define ZSF4   __oo(cop2c.ish, 30, 0) /* Z scale factor */
#define FLAG   oooo(cop2c.iuw, 31)    /* ! */

#endif
