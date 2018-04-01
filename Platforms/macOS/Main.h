@interface Main : NSOb <NSApplicationListener>

@prop (weak) IBOutlet NSOpenGLView *openGLView;
@prop (unsafe_unretained) IBOutlet NSTextView *consoleView;

// Console
- (void)consoleClear;
- (void)consolePrint:(NSChars *)text;

@end

extern Main *app;
