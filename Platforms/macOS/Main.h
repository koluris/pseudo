@interface Main : NSOb <NSApplicationListener>

@prop (weak) IBOutlet NSWindow *window;
@prop (weak) IBOutlet NSOpenGLView *openGLView;
@prop (weak) IBOutlet NSPanel *console;
@prop (unsafe_unretained) IBOutlet NSTextView *consoleView;
@prop (nonatomic, retain) NSOperationQueue *queue;

// Console
- (void)consoleClear;
- (void)consolePrint:(NSChars *)text;

@end

extern Main *app;
