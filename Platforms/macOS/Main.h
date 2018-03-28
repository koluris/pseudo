@interface Main : NSOb <NSApplicationListener>

@property (unsafe_unretained) IBOutlet NSTextView *consoleView;

@end

extern Main *app;
