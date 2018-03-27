#import "Global.h"
#import "../../Source/PSeudo.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    psx.init([@"/Users/dk/Downloads/scph1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}
