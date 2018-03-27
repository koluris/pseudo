#define data32\
    *(ub *)&mem.hwr.ptr[0x1070]

#define mask32\
    *(ub *)&mem.hwr.ptr[0x1074]

class CstrHardware {
public:
    void write32(uw, uw);
    void write16(uw, uh);
    void write08(uw, ub);
    
    uw read32(uw);
};

extern CstrHardware io;
