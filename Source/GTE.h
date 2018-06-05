class CstrCop2 {
    union pair {
        uw iuw[32]; uh iuh[32*2]; ub iub[32*4];
        sw isw[32]; sh ish[32*2]; sb isb[32*4];
    };
    
public:
    pair cop2c, cop2d;
    
    void reset();
    void subroutine(uw, uw);
};

extern CstrCop2 cop2;
