#include "Global.h"


CstrDisc disc;

void CstrDisc::reset() {
    if (file) {
        fclose(file); file = 0;
    }
}

bool CstrDisc::open(const char *path) {
    file = fopen(path, "rb");
    
    if (!file) {
        return false;
    }
    
    return true;
}
