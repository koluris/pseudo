class CstrCop2 {
    union pair {
        uw uw__[32 * 1];
        uh uh__[32 * 2];
        ub ub__[32 * 4];
        
        sw sw__[32 * 1];
        sh sh__[32 * 2];
        sb sb__[32 * 4];
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
