class CstrDisc {
    FILE *file;
    
public:
    void reset();
    bool open(const char *);
};

extern CstrDisc disc;
