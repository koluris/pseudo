@interface Options : NSPanel

// Textfield
@prop (weak) IBOutlet NSTextField *biosFile;

// Combobox
@prop (weak) IBOutlet NSPopUpButton *cpuMode;
@prop (weak) IBOutlet NSPopUpButton *windowResolution;
@prop (weak) IBOutlet NSPopUpButton *fpsLimit;

// Checkbox
@prop (weak) IBOutlet NSButton *skipBootScreen;
@prop (weak) IBOutlet NSButton *smoothTextures;
@prop (weak) IBOutlet NSButton *audioStereo;
@prop (weak) IBOutlet NSButton *audioMono;
@prop (weak) IBOutlet NSButton *audioOff;

// Members
@prop NSUserDefaults *defaults;

@end
