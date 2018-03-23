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
