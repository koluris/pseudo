class CstrGraphics {
    struct {
        uw data, status;
    } ret;
    
public:
    void reset();
    
    // Store
    void dataMemWrite(uw *, sw);
    void dataWrite(uw);
    void statusWrite(uw);
    
    // Load
    uw statusRead();
};

extern CstrGraphics vs;
