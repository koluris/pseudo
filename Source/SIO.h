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
    
    uh btnState;
    ub bfr[5], cnt;
    
public:
    void reset();
    void padListener(int, bool);
    
    // Load
    uh read16();
    ub read08();
};

extern CstrSerial sio;
