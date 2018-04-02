#define data32\
    *(uw *)&mem.hwr.ptr[0x1070]

#define mask32\
    *(uw *)&mem.hwr.ptr[0x1074]

#define data16\
    *(uh *)&mem.hwr.ptr[0x1070]


class CstrHardware {
public:
    // Store
    void write32(uw, uw);
    void write16(uw, uh);
    void write08(uw, ub);
    
    // Load
    uw read32(uw);
    uh read16(uw);
};

extern CstrHardware io;
