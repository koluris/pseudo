struct CstrMem {
    ub *ram, *rom, *hwr;
    
    void init(), reset();
};
