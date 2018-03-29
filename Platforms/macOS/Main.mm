#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    app = (Main *)[[NSApplication sharedApplication] del];
    
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    
    // NSTextView
    self.consoleView.textContainerInset = NSMakeSize(5.0f, 8.0f);
    //self.consoleView.textColor = [NSColor whiteColor];
    
    psx.init([@"/Users/dk/Downloads/scph1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (void)printConsole:(char)text {
    static bool once = true;
    
    if (once) {
        self.consoleView.contents = @"";
        once = false;
        return;
    }
    
    self.consoleView.contents = [NSChars charsWithFormat:@"%@%c", self.consoleView.contents, text];
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
