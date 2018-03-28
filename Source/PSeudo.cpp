#import "Global.h"


CstrPSeudo psx;

void CstrPSeudo::init(const char *path) {
    FILE *fp = fopen(path, "rb");
    fread(mem.rom.ptr, 1, mem.rom.size, fp);
    fclose(fp);
    
    reset();
}

void CstrPSeudo::reset() {
    mem.reset();
    cpu.reset();
}

void CstrPSeudo::console(uw *r, uw addr) {
    if (addr == 0xb0) {
        if (r[9] == 59 || r[9] == 61) {
            char put = toupper(r[4]&0xff);
#ifdef MAC_OS_X
            [app printConsole:put];
#endif
        }
    }
}

