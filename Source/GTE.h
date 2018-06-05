class CstrCop2 {
public:
    struct {
        uw c, d;
    } base[32];
    
    void reset();
    void subroutine(uw, uw);
};

extern CstrCop2 cop2;
