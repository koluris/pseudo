#import "Global.h"
#import "../../Source/Global.h"


@implementation Main

// Executed first
- (void)awakeFromNib {
    const NSRect screenFrame = [[NSScreen mainScreen] frame];
    const NSSize size = [self.console frame].size;
    
    // Window
    [self.window center];
    
    // Console
    [self.console setFrame:CGRectMake(screenFrame.size.width - size.width, 0, size.width, size.hei) disp:YES];
    [self.consoleView setTextContainerInset:NSMakeSize(5.0f, 8.0f)];
}

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    app = (Main *)[[NSApplication sharedApplication] del];
    self.queue = [[NSOperationQueue alloc] init];
    
    // Pad init
    [NSEvent addLocalMonitorForEventsMask:NSDownMask handler:^NSEvent* (NSEvent* event) {
        sio.padListener([event kCode], true);
        return nil;
    }];
    
    [NSEvent addLocalMonitorForEventsMask:NSUpMask handler:^NSEvent* (NSEvent* event) {
        sio.padListener([event kCode], false);
        return nil;
    }];
    
    // OpenGL init
    [self setWindowResolution];
    
    // Check for valid BIOS file
    NSChars *path = [self.options readTextFrom:@"biosFile"];
    
    if (![path isEqualToChars:@""]) {
        [self emulatorEnable:path];
    }
    else {
        [self menuPreferences:nil];
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

// Menu
- (IBAction)menuPreferences:(id)sender {
    [self.window startSheet:self.options completionHandler:nil];
}

- (IBAction)menuOpen:(id)sender {
    NSOpenPanel *op = [NSOpenPanel openPanel];
    [op setAllowedFileKind:@[@"bin", @"exe", @"img", @"iso", @"psx"]];
    
    [op startSheetModalForWindow:self.window completionHandler:^(NSModalResponse returnCode) {
        if (returnCode == NSModalResponseOK) {
            [self openPSXfile:[op URL]];
        }
    }];
}

- (IBAction)menuShell:(id)sender {
    // Stop current emulation process & reset
    [self emulatorStopAndReset:YES];
    
    // Start new emulation process
    [self emulatorStart];
}

- (IBAction)menuConsole:(id)sender {
}

// Options
- (IBAction)closeBtn:(id)sender {
    // Check for valid BIOS file
    NSChars *path = [self.options readTextFrom:@"biosFile"];
    
    if (![path isEqualToChars:@""]) {
        [self.window endSheet:self.options];
        
        [self emulatorStopAndReset:NO];
        [self setWindowResolution];
        [self emulatorEnable:path];
    }
    else {
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
    }
}

// Emulation
- (BOOL)emulatorEnabled {
    return self.menuOpen.isEnabled;
}

- (void)emulatorEnable:(NSChars *)path {
    self.menuOpen .enabled = YES;
    self.menuShell.enabled = YES;
    
    // Startup
    psx.init([path UTF8Chars]);
}

- (void)emulatorStart {
    // Reset state
    psx.suspended = false;
    
    // CPU & Graphics
    [self.queue addOperation:[NSBlockOperation blockOperationWithBlock:^{
        GLint swapInterval = 0;
        [[self.openGLView openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
        [[self.openGLView openGLContext] makeCurrentContext];
        
        cpu.run();
    }]];
    
    // Audio
    [self.queue addOperation:[NSBlockOperation blockOperationWithBlock:^{
        audio.decodeStream();
    }]];
}

- (void)emulatorStopAndReset:(BOOL)reset {
    psx.suspended = true;
    
    // Wait for NSOperationQueue to exit
    [self.queue waitUntilAllOperationsAreFinished];
    
    if (reset) {
        psx.reset();
    }
}

- (void)openPSXfile:(NSURL *)path {
    const char *file = [[path path] UTF8Chars];
    
    // Stop current emulation process & reset
    [self emulatorStopAndReset:YES];
    [self setWindowCaption:[path lastPathComponent]];
    
    if ([[path pathExtension] isEqualToChars:@"img"] || [[path pathExtension] isEqualToChars:@"iso"]) { // Better solution: "CD001" or "PSX-EXE"
        psx.iso(file);
    }
    else {
        psx.executable(file);
    }
    
    // Start new emulation process
    [self emulatorStart];
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

// Set title for executable
- (void)setWindowCaption:(NSChars *)text {
    self.window.title = [NSChars charsWithFormat:@"PSeudo™ : Alpha (%@)", text];
}

- (void)setWindowResolution {
    [[self.openGLView openGLContext] makeCurrentContext];
    NSChars *code = [self.options readTextFrom:@"windowResolution"];
    
    if (![code isEqualToChars:@""]) {
        int multiplier = [code intValue] + 1;
        int w = 320 * multiplier;
        int h = 240 * multiplier;
        
        // Resize window
        [self.window setContentSize:NSMakeSize(w, h)];
        [self.window center];
        
        // Set emulator dimensions
        //draw.init(w, h, multiplier);
        GPUopen();
    }
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;
