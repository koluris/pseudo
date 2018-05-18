#define printx(text, ...)\
    printf(text, __VA_ARGS__);\
    putchar('\n');\
    exit(0)

class CstrPSeudo {
public:
    void init(const char *);
    void reset();
    void executable(const char *);
    void console(uw *, uw);
};

extern CstrPSeudo psx;
