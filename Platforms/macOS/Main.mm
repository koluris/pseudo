#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    app = (Main *)[[NSApplication sharedApplication] del];
    
    // OpenGL
    //[[self.openGLView openGLContext] makeCurrentContext];
    //glClearColor(1, 0, 1, 1);
    //glFlush();
    
    // Console
    self.consoleView.textContainerInset = NSMakeSize(5.0f, 8.0f);
    self.consoleView.textColor = [NSColor RGBA(201, 110, 63)];
    
    psx.init([@"/Users/dk/Downloads/SCPH1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

// Menu
- (IBAction)menuShell:(id)sender {
    dispatch_asinc(dispatch_main_queue(), ^{
        cpu.run();
    });
}

// Console
- (void)consoleClear {
    self.consoleView.contents = @"";
}

- (void)consolePrint:(NSChars *)text {
    self.consoleView.contents = [NSChars charsWithFormat:@"%@%@", self.consoleView.contents, text];
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
