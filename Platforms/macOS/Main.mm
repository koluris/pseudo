#import "Global.h"
#import "../../Source/PSeudo.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    psx.reset();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end

// Startup
int main() {
    return NSApplicationMain(0, nil);
}
