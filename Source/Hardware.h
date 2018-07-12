#define data32 \
    *(uw *)&mem.hwr.ptr[0x1070]

#define mask32 \
    *(uw *)&mem.hwr.ptr[0x1074]

#define data16 \
    *(uh *)&mem.hwr.ptr[0x1070]

#define mask16 \
    *(uh *)&mem.hwr.ptr[0x1074]


class CstrHardware {
    enum {
        HWR_ACCESS_32 = 4,
        HWR_ACCESS_16 = 2,
        HWR_ACCESS_08 = 1,
    };
    
public:
    // Store
    template <class T>
    void write(uw, T);
    
    // Load
    template <class T>
    T read(uw);
};

extern CstrHardware io;
