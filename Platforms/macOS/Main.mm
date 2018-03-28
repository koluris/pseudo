#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    app = (Main *)[[NSApplication sharedApplication] delegate];
    
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    
    // NSTextView
    self.consoleView.textContainerInset = NSMakeSize(8.0f, 8.0f);
    self.consoleView.string = @"psx";
    
    //psx.setConsoleView(self.consoleView);
    psx.init([@"/Users/dk/Downloads/scph1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (void)hi {
    printf("hi\n");
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
