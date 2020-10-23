class CstrCop2 {
    union pair {
        uw uw[32 * 1];
        uh uh[32 * 2];
        ub ub[32 * 4];
        
        sw sw[32 * 1];
        sh sh[32 * 2];
        sb sb[32 * 4];
    } cop2c, cop2d;
    
    uw divide(sh, uh);
    
public:
    void reset();
    void execute(uw);
    uw MFC2(uw);
    void MTC2(uw, uw);
    void CTC2(uw, uw);
};

extern CstrCop2 cop2;
