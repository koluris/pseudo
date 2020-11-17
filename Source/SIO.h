class CstrSerial {
    enum {
        PAD_BTN_SELECT,
        PAD_BTN_START = 3,
        PAD_BTN_UP,
        PAD_BTN_RIGHT,
        PAD_BTN_DOWN,
        PAD_BTN_LEFT,
        PAD_BTN_L2,
        PAD_BTN_R2,
        PAD_BTN_L1,
        PAD_BTN_R1,
        PAD_BTN_TRIANGLE,
        PAD_BTN_CIRCLE,
        PAD_BTN_CROSS,
        PAD_BTN_SQUARE
    };
    
    struct {
        bool enabled;
        ub data;
    } rx;
    
    uh btnState;
    ub bfr[5], index;
    
    void pollController(ub);
    
public:
    void reset();
    void padListener(int, bool);
    
    void write16(uw, uh);
    void write08(uw, ub);
    
    uh read16(uw);
    ub read08(uw);
};

extern CstrSerial sio;
