#import "Global.h"
#import "../../Source/PSeudo.h"


@implementation Main

- (void)applicationDidFinishLaunch:(NSNotification *)aNotification {
    app = (Main *)[[NSApplication sharedApplication] delegate];
    
    //NSURL *uri = [[NSBundle mainBundle] bundleURL];
    
    // NSTextView
    self.consoleView.textContainerInset = NSMakeSize(8.0f, 8.0f);
    self.consoleView.string = @"psx";
    
    psx.setConsoleView(self.consoleView);
    psx.init([@"/Users/dk/Downloads/scph1001.bin" UTF8Chars]);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (void)hi {
    printf("hi\n");
}

@end

// Startup
int main() {
    const char *b[] = {};
    return NSApplicationMain(0, b);
}

Main *app;

//#define appendAttributedChars   appendAttributedString
//
//void str_pseudo::console(uw *r, uw addr) {
//    if (addr == 0xb0) {
//        if (r[9] == 59 || r[9] == 61) {
//            char put = toupper(r[4]&0xff);
//
//#ifdef MAC_OS_X
//            NSChars *text = [NSChars charsWithFormat:@"%c", put];
//            dispatch_ASYNC(dispatch_main_queue(), ^{
//                NSAttributedChars *attr = [[NSAttributedChars alloc] initWithChars:text];
//                [[output textStore] appendAttributedChars:attr];
//            });
//#endif
//        }
//    }
//}

