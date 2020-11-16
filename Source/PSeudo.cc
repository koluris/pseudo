/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


CstrPSeudo psx;

void CstrPSeudo::init(const char *path) {
    FILE *fp = fopen(path, "rb");
    
    // Available
    if (fp) {
        if (fileSize(fp) == mem.rom.size) {
            fread(mem.rom.ptr, 1, mem.rom.size, fp);
            reset();
        }
        else {
            printx("/// PSeudo BIOS incorrect file size: %d", fileSize(fp));
        }
        
        fclose(fp);
    }
    else {
        printx("/// PSeudo BIOS %s", "file not found");
    }
}

void CstrPSeudo::reset() {
    suspended = false;
    
    audio.reset();
      bus.reset();
    cache.reset();
       cd.reset();
     cop2.reset();
      cpu.reset();
     disc.reset();
     mdec.reset();
      mem.reset();
    rootc.reset();
      sio.reset();
       vs.reset();
    
    // BIOS version, ex: "ROM Version 4.1 12/16/97 E"
    ub version[0x1b];
    memcp(version, &mem.rom.ptr[mem.rom.size - 0xc7], sizeof(version));
    
#ifdef APPLE_MACOS
    [app consoleClear];
    [app consolePrint:[NSChars charsWithFormat:@"%s\n\nPSeudo™ : Alpha 0.84\n-> reset complete\n", version]];
#elif  APPLE_IOS
    // TODO
#endif
}

void CstrPSeudo::iso(const char *path) {
    if (disc.open(path)) {
        // Prerequisite boot
        cpu.bootstrap();
        
        if (0) { // Enable to skip BIOS boot
            cpu.setpc(cpu.base[31]);
        }
    }
}

void CstrPSeudo::executable(const char *path) {
    FILE *fp = fopen(path, "rb");
    
    // Available
    if (fp) {
        if (fileSize(fp)) {
            // Prerequisite boot
            cpu.bootstrap();
            
            // EXE file
            fread(&header, 1, sizeof(header), fp);
            fseek(fp, 0x800, SEEK_SET);
            fread(&mem.ram.ptr[header.t_addr & (mem.ram.size - 1)], 1, header.t_size, fp);
            
            cpu.setpc(header.pc0);
            
            // GP0 and SAddr
            cpu.base[28] = header.cp0;
            cpu.base[29] = header.s_addr;
        }
        else {
            printx("/// PSeudo EXE incorrect file size: %d", fileSize(fp));
        }
        
        fclose(fp);
    }
    else {
        printx("/// PSeudo EXE file %s", "not found");
    }
}

void CstrPSeudo::console(uw *base, uw addr) {
    if (addr == 0xb0) {
        if (base[9] == 59 || base[9] == 61) {
            char put = toupper(base[4] & 0xff);
#ifdef APPLE_MACOS
            [app consolePrint:[NSChars charsWithFormat:@"%c", put]];
#elif  APPLE_IOS
            // TODO
#endif
        }
    }
}

uw CstrPSeudo::fileSize(FILE *fp) {
    uw size;
    
    fseek(fp, 0, SEEK_END);
    size = (uw)ftell(fp);
    rewind(fp);
    
    return size;
}
