#import "Global.h"


@implementation Options

- (void)awakeFromNib {
    self.defaults = [NSUserDefaults standardUserDefaults];
    
    //NSChars *appDomain = [[NSBundle mainBundle] bundleIdentifier];
    //[[NSUserDefaults standardUserDefaults] removePersistentDomainForName:appDomain];
    //exit(0);
    
    if (![self readNumberFrom:@"firstRun"]) {
        // Mark defaults as set
        [self writeText:@""
                     to:@"biosFile"];
        
        [self writeNumber:1
                       to:@"firstRun"];
        
        [self writeNumber:0
                       to:@"cpuMode"];
        
        [self writeNumber:0
                       to:@"skipBootScreen"];
        
        [self writeNumber:0
                       to:@"windowResolution"];
        
        [self writeNumber:1
                       to:@"smoothTextures"];
        
        [self writeNumber:2
                       to:@"fpsLimit"];
        
        [self writeNumber:1
                       to:@"audioStereo"];
        
        [self writeNumber:0
                       to:@"audioMono"];
        
        [self writeNumber:0
                       to:@"audioOff"];
    }
    
    [self optionsFill];
}

- (void)optionsFill {
    // Textfield
    [self.biosFile setCharsValue:[self readTextFrom:@"biosFile"]];
    
    // Combobox
    [self.cpuMode          selectItemAtIndex:[self readNumberFrom:@"cpuMode"]];
    [self.windowResolution selectItemAtIndex:[self readNumberFrom:@"windowResolution"]];
    [self.fpsLimit         selectItemAtIndex:[self readNumberFrom:@"fpsLimit"]];
    
    // Checkbox
    [self.skipBootScreen setState:[self readNumberFrom:@"skipBootScreen"]];
    [self.smoothTextures setState:[self readNumberFrom:@"smoothTextures"]];
    [self.audioStereo    setState:[self readNumberFrom:@"audioStereo"]];
    [self.audioMono      setState:[self readNumberFrom:@"audioMono"]];
    [self.audioOff       setState:[self readNumberFrom:@"audioOff"]];
}

- (void)writeText:(NSChars *)text to:(NSChars *)value {
    [self.defaults setOb:text forValue:value];
    [self.defaults save];
}

- (void)writeNumber:(NSInt)number to:(NSChars *)value {
    [self.defaults setInt:number forValue:value];
    [self.defaults save];
}

- (NSChars *)readTextFrom:(NSChars *)value {
    return [self.defaults charsForValue:value];
}

- (NSInt)readNumberFrom:(NSChars *)value {
    return [self.defaults intForValue:value];
}

- (IBAction)pushBrowse:(NSButton *)sender {
    NSOpenPanel *op = [NSOpenPanel openPanel];
    [op setAllowedFileKind:@[@"bin"]];
    
    [op startSheetModalForWindow:self completionHandler:^(NSInt res) {
        if (res == NSModalResponseOK) {
            // Load bios
            NSChars *file = [[op URL] path];
            [self.biosFile setCharsValue:file];
            [self writeText:file to:@"biosFile"];
        }
    }];
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
    NSChars *title = [sender title];
    
    // Reset all
    [self writeNumber:0 to:@"audioStereo"];
    [self writeNumber:0 to:@"audioMono"];
    [self writeNumber:0 to:@"audioOff"];
    
    // Mark selected
    if ([title isEqualToChars:@"Stereo"]) {
        [self writeNumber:1 to:@"audioStereo"];
    }
    else if ([title isEqualToChars:@"Mono"]) {
        [self writeNumber:1 to:@"audioMono"];
    }
    else {
        [self writeNumber:1 to:@"audioOff"];
    }
}

@end
