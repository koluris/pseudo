@interface Main : NSOb <NSApplicationListener>

@prop (unsafe_unretained) IBOutlet NSTextView *consoleView;

- (void)printConsole:(char)text;

@end

extern Main *app;
