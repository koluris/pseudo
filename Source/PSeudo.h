#define printx(text, ...)\
    printf(text, __VA_ARGS__);\
    putchar('\n');\
    exit(0)

class CstrPSeudo {
public:
    void init(const char *), reset();
};

extern CstrPSeudo psx;
