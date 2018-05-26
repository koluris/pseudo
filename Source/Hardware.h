// Low order bits
#define LO_BITS(addr)\
    (addr & 0xffff)

#define data32\
    *(uw *)&mem.hwr.ptr[0x1070]

#define mask32\
    *(uw *)&mem.hwr.ptr[0x1074]

#define data16\
    *(uh *)&mem.hwr.ptr[0x1070]

#define mask16\
    *(uh *)&mem.hwr.ptr[0x1074]


class CstrHardware {
public:
    // Store
    void write32(uw, uw);
    void write16(uw, uh);
    void write08(uw, ub);
    
    // Load
    uw read32(uw);
    uh read16(uw);
    ub read08(uw);
};

extern CstrHardware io;
