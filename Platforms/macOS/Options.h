@interface Options : NSPanel

@prop (weak) IBOutlet NSPopUpButton *cpuMode;
@prop (weak) IBOutlet NSButton *skipBootScreen;
@prop (weak) IBOutlet NSPopUpButton *windowResolution;
@prop (weak) IBOutlet NSButton *smoothTextures;
@prop (weak) IBOutlet NSPopUpButton *fpsLimit;
@prop (weak) IBOutlet NSButton *audioStereo;
@prop (weak) IBOutlet NSButton *audioMono;
@prop (weak) IBOutlet NSButton *audioOff;

@prop NSUserDefaults *defaults;

@end
