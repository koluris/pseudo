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
            // TODO: Incorrect file size
        }
        
        fclose(fp);
    }
    else {
        // TODO: File not found
    }
}

void CstrPSeudo::reset() {
    suspended = false;
    
    audio.reset();
      bus.reset();
    cache.reset();
       cd.reset();
      cpu.reset();
      mem.reset();
    rootc.reset();
      sio.reset();
       vs.reset();
    
    // BIOS version, ex: "ROM Version 4.1 12/16/97 E"
    ub version[0x1b];
    memcp(version, &mem.rom.ptr[mem.rom.size - 0xc7], sizeof(version));
    
#ifdef APPLE_MACOS
    [app consoleClear];
    [app consolePrint:[NSChars charsWithFormat:@"%s\n\nPSeudo™ : Alpha 0.81\n-> reset complete\n", version]];
#elif  APPLE_IOS
    // TODO
#endif
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
            // TODO: Incorrect file size
        }
        
        fclose(fp);
    }
    else {
        // TODO: File not found
    }
}

void CstrPSeudo::console(uw *r, uw addr) {
    if (addr == 0xb0) {
        if (r[9] == 59 || r[9] == 61) {
            char put = toupper(r[4] & 0xff);
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
