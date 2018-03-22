#include "Global.h"


CstrMips cpu;

void CstrMips::reset() {
    memset(base, 0, sizeof(base));
    
    pc = 0xbfc00000;
}
