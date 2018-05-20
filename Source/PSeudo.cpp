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
    [app consolePrint:@"PSeudo reset\n"];
#endif
    
      cpu.reset();
      mem.reset();
    rootc.reset();
       vs.reset();
}

void CstrPSeudo::executable(const char *path) {
    cpu.bootstrap();
    
    struct {
        ub id[8]; uw v[17];
    } header;
    
    FILE *fp = fopen(path, "rb");
    fread(&header, 1, sizeof(header), fp);
    fseek(fp, 0x800, SEEK_SET);
    fread(&mem.ram.ptr[header.v[4] & (mem.ram.size - 1)], 1, header.v[5], fp);
    fclose(fp);
    
    cpu.pc = header.v[2];
    cpu.base[28] = header.v[3];
    cpu.base[29] = header.v[10];
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
