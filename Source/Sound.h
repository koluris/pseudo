//class CstrAudio {
//    enum {
//        SPU_ALC_BUF_AMOUNT   = 16,
//        SPU_CHANNELS         = 24,
//        SPU_CHANNEL_BUF_SIZE = 11 * (USHRT_MAX + 1), // 2 would cover it, but... hs-001
//        SPU_MAX_VOLUME       = 0x3fff,
//        SPU_SAMPLE_RATE      = 44100,
//        SPU_SAMPLE_SIZE      = 512,
//    };
//
//    const sh f[5][2] = {
//        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
//    };
//
//    struct voice {
//        sh bfr[SPU_CHANNEL_BUF_SIZE];
//        sw count, freq, pos, raddr, saddr, size;
//        sh volumeL, volumeR;
//    } spuVoices[SPU_CHANNELS];
//
//    uh spuMem[1024 * 256];
//    uw spuAddr;
//    sh sbuf[SPU_SAMPLE_SIZE];
//
//    sh setVolume(sh);
//    void voiceOn (uw);
//    void voiceOff(uw);
//    void stream();
//
//    // OpenAL
//    ALCdevice *device;
//    ALCcontext *ctx;
//    ALuint source;
//    ALuint bfr[SPU_ALC_BUF_AMOUNT];
//
//public:
//    CstrAudio() {
//        // OpenAL
//        device = alcOpenDevice(NULL);
//
//        if (!device) {
//            printf("ALC Device error\n");
//        }
//
//        ctx = alcCreateContext(device, NULL);
//        alcMakeContextCurrent(ctx);
//
//        if (!ctx) {
//            printf("ALC Context error\n");
//        }
//
//        alGenSources(1, &source);
//        alGenBuffers(SPU_ALC_BUF_AMOUNT, bfr);
//
//        for (auto &item : bfr) {
//            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
//        }
//
//        alSourceQueueBuffers(source, SPU_ALC_BUF_AMOUNT, bfr);
//    }
//
//    ~CstrAudio() {
//        alDeleteSources(1, &source);
//        alDeleteBuffers(SPU_ALC_BUF_AMOUNT, bfr);
//        ALCdevice *device = alcGetContextsDevice(ctx);
//        alcMakeContextCurrent(NULL);
//        alcCloseDevice(device);
//    }
//
//    void reset();
//    void decodeStream();
//
//    // Store
//    void write(uw, uh);
//
//    // Load
//    uh read(uw);
//
//    // DMA
//    void executeDMA(CstrBus::castDMA *);
//};

#define MAXCHAN 24

class CstrAudio {
    enum {
        SPU_ALC_BUF_AMOUNT = 16,
        SPU_SAMPLE_RATE    = 44100,
        SPU_SAMPLE_SIZE    = 256,
        SPU_SAMPLE_COUNT   = SPU_SAMPLE_SIZE / 4
    };
    
    const int f[5][2] = {
        { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 }
    };
    
    uh sbuf[SPU_SAMPLE_SIZE];
    uh spuMem[256 * 1024];
    ub *spuMemC;
    uw spuAddr;
        
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[SPU_ALC_BUF_AMOUNT];
    
    struct voices {
        int bNew;
        int iSBPos;
        int spos;
        int sinc;
        int SB[32];
        int sval;
        ub *pStart;
        ub *pCurr;
        ub *pLoop;
        int bOn;
        int bStop;
        int iActFreq;
        int iUsedFreq;
        int volumeL;
        int volumeR;
        int bIgnoreLoop;
        int iRawPitch;
        int s_1;
        int s_2;
    } spuVoices[MAXCHAN + 1];
    
    void SoundOn(int, int, uh);
    void SoundOff(int, int, uh);
    void setPitch(int, int);
    void StartSound(int);
    void VoiceChangeFrequency(int);
    int calcVolume(sh);
    void stream();
    
public:
    CstrAudio() {
        // OpenAL
        device = alcOpenDevice(NULL);

        if (!device) {
            printf("ALC Device error\n");
        }

        ctx = alcCreateContext(device, NULL);
        alcMakeContextCurrent(ctx);

        if (!ctx) {
            printf("ALC Context error\n");
        }

        alGenSources(1, &source);
        alGenBuffers(SPU_ALC_BUF_AMOUNT, bfr);

        for (auto &item : bfr) {
            alBufferData(item, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE, SPU_SAMPLE_RATE);
        }

        alSourceQueueBuffers(source, SPU_ALC_BUF_AMOUNT, bfr);
    }
    
    ~CstrAudio() {
        alDeleteSources(1, &source);
        alDeleteBuffers(SPU_ALC_BUF_AMOUNT, bfr);
        ALCdevice *device = alcGetContextsDevice(ctx);
        alcMakeContextCurrent(NULL);
        alcCloseDevice(device);
    }
    
    void init();
    void reset();
    void write(uw, uh);
    uh read(uw);
    void executeDMA(CstrBus::castDMA *);
    void decodeStream();
};

extern CstrAudio audio;

#define H_SPUirqAddr     0x0da4
#define H_SPUaddr        0x0da6
#define H_SPUdata        0x0da8
#define H_SPUctrl        0x0daa
#define H_SPUstat        0x0dae
#define H_SPUmvolL       0x0d80
#define H_SPUmvolR       0x0d82
#define H_SPUrvolL       0x0d84
#define H_SPUrvolR       0x0d86
#define H_SPUon1         0x0d88
#define H_SPUon2         0x0d8a
#define H_SPUoff1        0x0d8c
#define H_SPUoff2        0x0d8e
#define H_FMod1          0x0d90
#define H_FMod2          0x0d92
#define H_Noise1         0x0d94
#define H_Noise2         0x0d96
#define H_SPUMute1       0x0d9c
#define H_SPUMute2       0x0d9e
#define H_ExtLeft        0x0db4
#define H_ExtRight       0x0db6
#define H_SPUPitch0      0x0c04
#define H_SPUPitch1      0x0c14
#define H_SPUPitch2      0x0c24
#define H_SPUPitch3      0x0c34
#define H_SPUPitch4      0x0c44
#define H_SPUPitch5      0x0c54
#define H_SPUPitch6      0x0c64
#define H_SPUPitch7      0x0c74
#define H_SPUPitch8      0x0c84
#define H_SPUPitch9      0x0c94
#define H_SPUPitch10     0x0ca4
#define H_SPUPitch11     0x0cb4
#define H_SPUPitch12     0x0cc4
#define H_SPUPitch13     0x0cd4
#define H_SPUPitch14     0x0ce4
#define H_SPUPitch15     0x0cf4
#define H_SPUPitch16     0x0d04
#define H_SPUPitch17     0x0d14
#define H_SPUPitch18     0x0d24
#define H_SPUPitch19     0x0d34
#define H_SPUPitch20     0x0d44
#define H_SPUPitch21     0x0d54
#define H_SPUPitch22     0x0d64
#define H_SPUPitch23     0x0d74

#define H_SPUStartAdr0   0x0c06
#define H_SPUStartAdr1   0x0c16
#define H_SPUStartAdr2   0x0c26
#define H_SPUStartAdr3   0x0c36
#define H_SPUStartAdr4   0x0c46
#define H_SPUStartAdr5   0x0c56
#define H_SPUStartAdr6   0x0c66
#define H_SPUStartAdr7   0x0c76
#define H_SPUStartAdr8   0x0c86
#define H_SPUStartAdr9   0x0c96
#define H_SPUStartAdr10  0x0ca6
#define H_SPUStartAdr11  0x0cb6
#define H_SPUStartAdr12  0x0cc6
#define H_SPUStartAdr13  0x0cd6
#define H_SPUStartAdr14  0x0ce6
#define H_SPUStartAdr15  0x0cf6
#define H_SPUStartAdr16  0x0d06
#define H_SPUStartAdr17  0x0d16
#define H_SPUStartAdr18  0x0d26
#define H_SPUStartAdr19  0x0d36
#define H_SPUStartAdr20  0x0d46
#define H_SPUStartAdr21  0x0d56
#define H_SPUStartAdr22  0x0d66
#define H_SPUStartAdr23  0x0d76

#define H_SPULoopAdr0   0x0c0e
#define H_SPULoopAdr1   0x0c1e
#define H_SPULoopAdr2   0x0c2e
#define H_SPULoopAdr3   0x0c3e
#define H_SPULoopAdr4   0x0c4e
#define H_SPULoopAdr5   0x0c5e
#define H_SPULoopAdr6   0x0c6e
#define H_SPULoopAdr7   0x0c7e
#define H_SPULoopAdr8   0x0c8e
#define H_SPULoopAdr9   0x0c9e
#define H_SPULoopAdr10  0x0cae
#define H_SPULoopAdr11  0x0cbe
#define H_SPULoopAdr12  0x0cce
#define H_SPULoopAdr13  0x0cde
#define H_SPULoopAdr14  0x0cee
#define H_SPULoopAdr15  0x0cfe
#define H_SPULoopAdr16  0x0d0e
#define H_SPULoopAdr17  0x0d1e
#define H_SPULoopAdr18  0x0d2e
#define H_SPULoopAdr19  0x0d3e
#define H_SPULoopAdr20  0x0d4e
#define H_SPULoopAdr21  0x0d5e
#define H_SPULoopAdr22  0x0d6e
#define H_SPULoopAdr23  0x0d7e

#define CTRL_IRQ                0x40
#define CTRL_REVERB             0x80
#define CTRL_NOISE              0x3f00
#define CTRL_MUTE               0x4000
#define CTRL_ON                 0x8000

#define STAT_IRQ                0x40
