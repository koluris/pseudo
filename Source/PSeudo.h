#define printx(text, ...)\
    printf(text, __VA_ARGS__);\
    putchar('\n');\
    exit(0)

class CstrPSeudo {
    void reset();
    
public:
    void init(const char *);
};

extern CstrPSeudo psx;
