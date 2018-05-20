@interface Main : NSOb <NSApplicationListener>

// Interface Builder
@prop (weak) IBOutlet NSWindow *window;
@prop (weak) IBOutlet NSOpenGLView *openGLView;
@prop (weak) IBOutlet Options *options;
@prop (weak) IBOutlet NSPanel *console;
@prop (unsafe_unretained) IBOutlet NSTextView *consoleView;

// Members
@prop NSRect screenFrame;
@prop (nonatomic, retain) NSOperationQueue *queue;

// Exposed Functions
- (void)consoleClear;
- (void)consolePrint:(NSChars *)text;

@end

extern Main *app;
