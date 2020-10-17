#include "Global.h"


#define MSF2SECT(m, s, f) \
    (((m) * 60 + (s) - 2) * 75 + (f))


CstrDisc disc;

void CstrDisc::reset() {
    memset(&bfr, 0, UDF_DATASIZE);
    
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

void CstrDisc::fetchTN(ub *b) {
    b[0] = 1;
    b[1] = 1;
}

void CstrDisc::fetchTD(ub *b) {
    memset(b + 1, 0, 3);
    
    b[0] = 0;
    b[1] = 2;
    b[2] = 0;
}

bool CstrDisc::trackRead(ub *t) {
    if (!file) {
        memset(&bfr, 0, UDF_DATASIZE);
        return false;
    }
    
    fseek(file, MSF2SECT(BCD2INT(t[0]), BCD2INT(t[1]), BCD2INT(t[2])) * UDF_FRAMESIZERAW + 12, SEEK_SET);
    fread(bfr, 1, UDF_DATASIZE, file);
    
    return true;
}
