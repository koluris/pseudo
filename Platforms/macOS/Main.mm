#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    app = (Main *)[[NSApplication sharedApplication] del];
    self.screenFrame = [[NSScreen mainScreen] frame];
    self.queue = [[NSOperationQueue alloc] init];
    
    // Window
    [self.window center];
    
    // Console
    NSSize size = [self.console frame].size;
    [self.console setFrame:CGRectMake(self.screenFrame.size.width - size.width, 0, size.width, size.hei) disp:YES];
    self.consoleView.textContainerInset = NSMakeSize(5.0f, 8.0f);
    
    // OpenGL init
    draw.reset();
    
    // Check for valid BIOS file
    NSChars *path = [self.options readTextFrom:@"biosFile"];
    
    if ([path isEqualToChars:@""]) {
        [self menuPreferences:nil];
        return;
    }
    
    [self enableEmulator:path];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

// Menu
- (IBAction)menuPreferences:(id)sender {
    [self.window startSheet:self.options completionHandler:nil];
}

- (IBAction)menuOpen:(id)sender {
    NSOpenPanel *op = [NSOpenPanel openPanel];
    [op setAllowedFileKind:@[@"bin", @"exe", @"psx"]];
    
    [op startSheetModalForWindow:self.window completionHandler:^(NSModalResponse returnCode) {
        if (returnCode == NSModalResponseOK) {
            // Stop current emulation process & reset
            [self emulationStop];

            // Load executable
            NSChars *file = [[op URL] path];
            psx.executable([file UTF8Chars]);

            // Start new emulation process
            [self emulationStart];
        }
    }];
}

- (IBAction)menuShell:(id)sender {
    // Stop current emulation process & reset
    [self emulationStop];
    
    // Start new emulation process
    [self emulationStart];
}

- (IBAction)menuConsole:(id)sender {
}

// Emulation
- (void)enableEmulator:(NSChars *)path {
    self.menuOpen .enabled = YES;
    self.menuShell.enabled = YES;
    
    // Startup
    psx.init([path UTF8Chars]);
}

- (void)emulationStart {
    [self.queue addOperation:[NSBlockOperation blockOperationWithBlock:^{
        [[self.openGLView openGLContext] makeCurrentContext];
        
        cpu.run();
    }]];
    
//    dispatch_asinc(dispatch_main_queue(), ^{
//        while(1) {
//            audio.decodeStream();
//            //sleep(1);
//        }
//    });
}

- (void)emulationStop {
    cpu.suspend();
    
    // Wait for NSOperationQueue to exit
    [self.queue waitUntilAllOperationsAreFinished];
    psx.reset();
}

// Options
- (IBAction)closeBtn:(id)sender {
    // Check for valid BIOS file
    NSChars *path = [self.options readTextFrom:@"biosFile"];
    
    if ([path isEqualToChars:@""]) {
        NSAlert *alert = [[NSAlert alloc] init];
        
        // Information
        [alert setMesText:@"BIOS dump"];
        [alert setInformativeText:@"Please browse for a valid BIOS file in order to operate the emulator."];
        [alert addButtonWithTitle:@"OK"];
        [alert addButtonWithTitle:@"Exit"];
        
        // Size
        NSRect frame = alert.window.frame;
        frame.size.width = self.options.frame.size.width - 26.0f;
        [alert.window setFrame:frame disp:YES];
        
        [alert startSheetModalForWindow:self.options completionHandler:^(NSModalResponse returnCode) {
            if (returnCode == NSAlertSecondButtonReturn) {
                [self.window endSheet:self.options];
            }
        }];
        return;
    }
    
    // Startup
    [self.window endSheet:self.options];
    path = [self.options readTextFrom:@"biosFile"];
    [self enableEmulator:path];
}

// Console
- (void)consoleClear {
    self.consoleView.contents = @"";
}

- (void)consolePrint:(NSChars *)text {
    dispatch_asinc(dispatch_main_queue(), ^{
        NSAttributedChars *attr = [[NSAttributedChars alloc] initWithChars:text attributes:@{ NSFontAttributeName: [NSFont fontWithName:@"Menlo" size:10], NSForeColorAttributeName: [NSColor RGBA(225, 170, 0)] }];
        [[self.consoleView textStore] appendAttributedChars:attr];

    });
}


@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
