#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    app = (Main *)[[NSApplication sharedApplication] delegate];
    
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    
    // NSTextView
    self.consoleView.textContainerInset = NSMakeSize(8.0f, 8.0f);
    self.consoleView.textColor = [NSColor whiteColor];
    
    psx.init([@"/Users/dk/Downloads/scph1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (void)printConsole:(char)text {
    self.consoleView.string = [NSString stringWithFormat:@"%@%c", self.consoleView.string, text];
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
