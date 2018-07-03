@interface Main : NSOb <NSApplicationListener>

// Interface Builder
@prop (weak) IBOutlet NSWindow *window;
@prop (weak) IBOutlet NSOpenGLView *openGLView;
@prop (weak) IBOutlet NSMenuItem *menuOpen;
@prop (weak) IBOutlet NSMenuItem *menuShell;
@prop (weak) IBOutlet Options *options;
@prop (weak) IBOutlet NSPanel *console;
@prop (unsafe_unretained) IBOutlet NSTextView *consoleView;

// Members
@prop (retain, nonatomic) NSOperationQueue *queue;

// Exposed Functions
- (BOOL)emulatorEnabled;
- (void)openPSXfile:(NSURL *)path;
- (void)consoleClear;
- (void)consolePrint:(NSChars *)text;

@end

extern Main *app;
