// Standard C libraries
#import <ctype.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>

// Apple macOS
#ifdef MAC_OS_X
    #import "../Platforms/macOS/Global.h"
#endif

// Data
typedef unsigned int   uw;
typedef unsigned short uh;
typedef unsigned char  ub;

typedef int   sw;
typedef short sh;
typedef char  sb;

// User imports
#import "Hardware.h"
#import "Mem.h"
#import "PSeudo.h"
#import "R3000A.h"
