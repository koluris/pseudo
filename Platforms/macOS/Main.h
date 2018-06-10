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
@prop NSRect screenFrame;
@prop (nonatomic, retain) NSOperationQueue *queue;

// Exposed Functions
- (void)openPSXfile:(NSURL *)path;
- (void)consoleClear;
- (void)consolePrint:(NSChars *)text;

@end

extern Main *app;
