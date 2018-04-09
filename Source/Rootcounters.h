#define PSX_CLK\
    33868800

// Assume NTSC for now
#define PSX_VSYNC\
    (PSX_CLK/60)

// This is uttermost experimental, it's the Achilles' heel
#define PSX_CYCLE\
    64


class CstrCounters {
    uw vbk;
    
public:
    void reset();
    void update();
};

extern CstrCounters rootc;
