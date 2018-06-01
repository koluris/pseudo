#import "Global.h"


CstrCop2 cop2;

void CstrCop2::subroutine(uw code) {
    switch(code & 63) {
        case 0: // Basic
            switch(rs & 7) {
                case 0: // MFC2
                    cpu.base[rt] = 0; // TODO
                    return;
            }
            return;
    }
    return;
}
