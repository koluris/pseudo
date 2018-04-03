/*  5-bit */
#define sa\
    ((code >>  6) & 31)

#define rd\
    ((code >> 11) & 31)

#define rt\
    ((code >> 16) & 31)

#define rs\
    ((code >> 21) & 31)

/*  6-bit */
#define opcode\
    ((code >> 26) & 63)

/* 16-bit */
#define imm\
    ((sh)code)

#define immu\
    (code & 0xffff)

/* 32-bit */
#define ob\
    (base[rs] + imm)

#define baddr\
    (pc + (imm << 2))

#define saddr\
    ((code & 0x3ffffff) << 2) | (pc & 0xf0000000)


class CstrMips {
    union { // Product & quotient (lo, hi)
        ud u64; uw u32[2];
    } res;
    
    uw opcodeCount;
    bool stopped;
    
    void branch(uw);
    void step(bool);
    void exception(uw, bool);
    
public:
    uw base[32], copr[16], pc;
    
    void reset();
    void run();
    void stop();
};

extern CstrMips cpu;
