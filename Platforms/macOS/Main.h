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
@prop (nonatomic, retain) NSOperationQueue *queue;

// Exposed Functions
- (BOOL)emulatorEnabled;
- (void)openPSXfile:(NSURL *)path;
- (void)consoleClear;
- (void)consolePrint:(NSChars *)text;
- (void)windowX:(int)w andY:(int)h;

@end

extern Main *app;
