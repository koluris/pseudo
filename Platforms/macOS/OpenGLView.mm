#import "Global.h"


@implementation OpenGLView

- (id)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    
    if (self) {
        NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute[]) {
            /*NSOpenGLPFADoubleBuffer,*/ 76, 0 /* Preserve draw buffer */
        }];
        
        self = [super initWithFrame:self.frame pixelFormat:pixelFormat];
        
        [self registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    }
    return self;
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
    return [app emulatorEnabled];
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    NSURL *fileURL = [NSURL URLFromPasteboard:[sender draggingPasteboard]];
    [app openPSXfile:fileURL];
    
    return YES;
}

@end
