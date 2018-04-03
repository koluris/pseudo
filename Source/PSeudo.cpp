#import "Global.h"


CstrPSeudo psx;

void CstrPSeudo::init(const char *path) {
    FILE *fp = fopen(path, "rb");
    fread(mem.rom.ptr, 1, mem.rom.size, fp);
    fclose(fp);
    
    reset();
}

void CstrPSeudo::reset() {
#ifdef MAC_OS_X
    [app consoleClear];
    [app consolePrint:@"PSeudo startup\n"];
#endif
    
    cpu.reset();
    mem.reset();
    vs .reset();
}

void CstrPSeudo::console(uw *r, uw addr) {
    if (addr == 0xb0) {
        if (r[9] == 59 || r[9] == 61) {
            char put = toupper(r[4]&0xff);
#ifdef MAC_OS_X
            [app consolePrint:[NSChars charsWithFormat:@"%c", put]];
#endif
        }
    }
}
