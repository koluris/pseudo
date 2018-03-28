@interface Main : NSOb <NSApplicationListener>

@prop (unsafe_unretained) IBOutlet NSTextView *consoleView;

- (void)hi;

@end

extern Main *app;
