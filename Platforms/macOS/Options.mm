#import "Global.h"


@implementation Options

- (void)awakeFromNib {
    printf("awakeFromNib\n");
    
    self.defaults = [NSUserDefaults standardUserDefaults];
    
    if (![self readNumberFrom:@"firstRun"]) {
        [self writeNumber:1 to:@"firstRun"];
        [self writeNumber:0 to:@"cpuMode"];
        [self writeNumber:0 to:@"skipBootScreen"];
        [self writeNumber:0 to:@"windowResolution"];
        [self writeNumber:1 to:@"smoothTextures"];
        [self writeNumber:2 to:@"fpsLimit"];
        [self writeNumber:1 to:@"audioStereo"];
        [self writeNumber:0 to:@"audioMono"];
        [self writeNumber:0 to:@"audioOff"];
    }
    
    [self optionsFill];
}

- (void)optionsFill {
    [self.cpuMode selectItemAtIndex:[self readNumberFrom:@"cpuMode"]];
    [self.skipBootScreen setState:[self readNumberFrom:@"skipBootScreen"]];
    [self.windowResolution selectItemAtIndex:[self readNumberFrom:@"windowResolution"]];
    [self.smoothTextures setState:[self readNumberFrom:@"smoothTextures"]];
    [self.fpsLimit selectItemAtIndex:[self readNumberFrom:@"fpsLimit"]];
    [self.audioStereo setState:[self readNumberFrom:@"audioStereo"]];
    [self.audioMono setState:[self readNumberFrom:@"audioMono"]];
    [self.audioOff setState:[self readNumberFrom:@"audioOff"]];
}

- (void)writeNumber:(NSInt)number to:(NSChars *)text {
    [self.defaults setInteger:number forKey:text];
    [self.defaults synchronize];
}

- (NSInt)readNumberFrom:(NSChars *)text {
    return [self.defaults integerForKey:text];
}

- (IBAction)pushCpuMode:(NSPopUpButton *)sender {
    [self writeNumber:[sender indexOfSelectedItem] to:@"cpuMode"];
}

- (IBAction)pushSkipBootScreen:(NSButton *)sender {
    [self writeNumber:[sender state] to:@"skipBootScreen"];
}

- (IBAction)pushWindowResolution:(NSPopUpButton *)sender {
    [self writeNumber:[sender indexOfSelectedItem] to:@"windowResolution"];
}

- (IBAction)pushSmoothTextures:(NSButton *)sender {
    [self writeNumber:[sender state] to:@"smoothTextures"];
}

- (IBAction)pushFpsLimit:(NSPopUpButton *)sender {
    [self writeNumber:[sender indexOfSelectedItem] to:@"fpsLimit"];
}

- (IBAction)pushAudioChannel:(NSButton *)sender {
}

@end
