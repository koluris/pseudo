#define PSX_CLK\
    33868800

// Assume NTSC for now
#define PSX_VSYNC\
    (PSX_CLK / 60)

#define PSX_HSYNC\
    (PSX_CLK / 15734)

// This is uttermost experimental, it's the Achilles' heel
#define PSX_CYCLE\
    64


class CstrCounters {
    enum {
        RTC_COUNT  = 0x0,
        RTC_MODE   = 0x4,
        RTC_TARGET = 0x8,
        RTC_LIMIT  = 0xffff
    };
    
    struct {
        uw mode; uh count, des, bound;
    } timer[3];
    
    uw vbk, hbk;
    
public:
    void reset();
    void update();
    
    template <class T>
    void write(uw, T);
    
    template <class T>
    T read(uw);
};

extern CstrCounters rootc;
