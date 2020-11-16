#undef s_addr

#ifdef DEBUG

#define printx(text, ...) \
    printf(text, __VA_ARGS__); \
    putchar('\n'); \
    exit(0)

#else

#define printx(text, ...)

#endif

// Low order bits
#define LOW_BITS(addr) \
    (addr & 0xffff)


class CstrPSeudo {
    struct {
        ub id[8];  /* SCE EXE || PS-X EXE */
        uw text;   /* SCE */
        uw data;   /* SCE */
        uw pc0;
        uw cp0;    /* SCE */
        uw t_addr;
        uw t_size;
        uw d_addr; /* SCE */
        uw d_size; /* SCE */
        uw b_addr; /* SCE */
        uw b_size; /* SCE */
        uw s_addr;
        uw s_size;
        uw SavedSP;
        uw SavedFP;
        uw SavedGP;
        uw SavedRA;
        uw SavedS0;
    } header;
    
    uw fileSize(FILE *);
    
public:
    // Variables
    bool suspended;
    
    // Functions
    void init(const char *);
    void reset();
    void iso(const char *);
    void executable(const char *);
    void console(uw *, uw);
};

extern CstrPSeudo psx;
