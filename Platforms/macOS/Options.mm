#import "Global.h"


@implementation Options

- (void)awakeFromNib {
    //NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    //[defaults setInteger:9001 forKey:@"HighScore"];
    //[defaults synchronize];
    
    printf("awakeFromNib\n");
    
    [self.cpuMode selectItemWithTitle:@"Interpreter"];
    [self.skipBootScreen setState:NSOffState];
    [self.windowResolution selectItemWithTitle:@"Native"];
    [self.smoothTextures setState:NSOnState];
    [self.fpsLimit selectItemWithTitle:@"60 (PAL)"];
    [self.audioStereo setState:NSOnState];
    [self.audioMono setState:NSOffState];
    [self.audioOff setState:NSOffState];
}

@end
