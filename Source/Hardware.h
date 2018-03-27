class CstrHardware {
public:
    void write32(uw, uw);
    void write16(uw, uh);
    void write08(uw, ub);
};

extern CstrHardware io;
