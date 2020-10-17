class CstrDisc {
    FILE *file;
    
public:
    enum {
        UDF_FRAMESIZERAW = 2352,
        UDF_DATASIZE = UDF_FRAMESIZERAW - 12
    };
    
    ub bfr[UDF_FRAMESIZERAW];
    
    void reset();
    bool open(const char *);
    void fetchTN(ub *);
    void fetchTD(ub *);
    bool trackRead(ub *);
};

extern CstrDisc disc;
