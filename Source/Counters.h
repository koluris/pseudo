class CstrCounters {
    // Constant enumerations
    enum {
        PSX_CLOCK      = 33868800,
        PSX_VSYNC_NTSC = PSX_CLOCK / 60,
        PSX_VSYNC_PAL  = PSX_CLOCK / 50,
        PSX_HSYNC      = PSX_CLOCK / 15734
    };
    
    enum {
        RTC_COUNT  = 0,
        RTC_MODE   = 4,
        RTC_TARGET = 8,
        RTC_BOUND  = 0xffff
    };
    
    uh bounds[3];
    sw vbk, hbk;
    
public:
    void reset();
    void update(int);
    
    // Store
    template <class T>
    void write(uw, T);
};

extern CstrCounters rootc;
