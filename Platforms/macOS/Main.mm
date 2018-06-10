#import "Global.h"
#import "../../Source/Global.h"


#define TITLEBAR_HEIGHT \
    22


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
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
    [self setWindowResolution];
    
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
            [self openPSXfile:[op URL]];
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
    
    // Pad init
    [NSEvent addLocalMonitorForEventsMask:NSDownMask handler:^NSEvent* (NSEvent* event) {
        sio.padListener([event kCode], true);
        return nil;
    }];
    
    [NSEvent addLocalMonitorForEventsMask:NSUpMask handler:^NSEvent* (NSEvent* event) {
        sio.padListener([event kCode], false);
        return nil;
    }];
    
    // Startup
    psx.init([path UTF8Chars]);
}

- (void)emulationStart {
    // Reset state
    psx.suspended = false;
    
    // CPU & Graphics
    [self.queue addOperation:[NSBlockOperation blockOperationWithBlock:^{
        [[self.openGLView openGLContext] makeCurrentContext];
        
        cpu.run();
    }]];
    
    // Audio
    [self.queue addOperation:[NSBlockOperation blockOperationWithBlock:^{
        audio.decodeStream();
    }]];
}

- (void)emulationStop {
    psx.suspended = true;
    
    // Wait for NSOperationQueue to exit
    [self.queue waitUntilAllOperationsAreFinished];
    psx.reset();
}

- (void)openPSXfile:(NSURL *)path {
    // Stop current emulation process & reset
    [self emulationStop];
    
    // Load executable
    [self setWindowCaption:[path lastPathComponent]];
    psx.executable([[path path] UTF8Chars]);
    
    // Start new emulation process
    [self emulationStart];
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
    
    // Close sheet
    [self.window endSheet:self.options];
    
    // Resize window
    [self setWindowResolution];
    
    // Continue
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

- (void)setWindowCaption:(NSChars *)text {
    self.window.title = [NSChars charsWithFormat:@"PSeudo™ : Alpha (%@)", text];
}

- (void)setWindowResolution {
    NSChars *code = [self.options readTextFrom:@"windowResolution"];
    
    if (![code isEqualToChars:@""]) {
        int multiplier = [code intValue] + 1;
        int w = 320 * multiplier;
        int h = 240 * multiplier;
        
        // Resize window
        NSRect frame = self.window.frame;
        frame.size.width = w;
        frame.size.hei   = h + TITLEBAR_HEIGHT;
        [self.window setFrame:frame disp:YES];
        [self.window center];
        
        // Set emulator dimensions
        draw.setWindowResolution(w, h);
    }
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
