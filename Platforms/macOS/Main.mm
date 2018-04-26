#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    app = (Main *)[[NSApplication sharedApplication] del];
    self.queue = [[NSOperationQueue alloc] init];
    
    // Console
    self.consoleView.textContainerInset = NSMakeSize(5.0f, 8.0f);
    self.consoleView.textColor = [NSColor RGBA(201, 110, 63)];
    
    psx.init([@"/Users/dk/Downloads/SCPH1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

// Menu
- (IBAction)menuOpen:(id)sender {
    NSOpenPanel *op = [NSOpenPanel openPanel];
    
    [op setAllowedFileKind:[[NSBunch alloc] initWithObs:@"bin", @"exe", @"psx", nil]];
    [op startWithCompletionHandler:^(NSInteger res) {
        if (res == NSModalResponseOK) {
            NSChars *file = [[op URL] path];
            psx.executable([file UTF8Chars]);
            
            [self emulationStart];
        }
    }];
}

- (IBAction)menuShell:(id)sender {
    [self emulationStart];
}

// OpenGL
- (void)emulationStart {
    [self.queue addOperation:[NSBlockOperation blockOperationWithBlock:^{
        [[self.openGLView openGLContext] makeCurrentContext];
        
        // OpenGL
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        
        cpu.run();
    }]];
}

// Console
- (void)consoleClear {
    self.consoleView.contents = @"";
}

- (void)consolePrint:(NSChars *)text {
    dispatch_asinc(dispatch_main_queue(), ^{
        self.consoleView.contents = [NSChars charsWithFormat:@"%@%@", self.consoleView.contents, text];
    });
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
