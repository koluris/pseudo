#import "Global.h"


@implementation OpenGLView

- (id)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    
    if (self) {
        [self registerForDraggedTypes:[NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
    }
    return self;
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
    return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    NSURL *fileURL = [NSURL URLFromPasteboard: [sender draggingPasteboard]];
    [app openPSXfile:fileURL];
    
    return YES;
}

@end
