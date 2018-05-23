#import "Global.h"


#define MAX_CHANNELS\
    24

#define MAX_VOLUME\
    0x3fff

#define spuAcc(addr)\
    *(uh *)&mem.hwr.ptr[addr]


CstrAudio audio;

void CstrAudio::reset() {
    // Variables
    spuAddr = ~0;
    spuVolumeL = MAX_VOLUME;
    spuVolumeR = MAX_VOLUME;
    
    // Channels
    for (int n = 0; n < MAX_CHANNELS; n++) {
        memset(&spuVoices[n].buffer, 0, USHRT_MAX * 2);
        spuVoices[n].count    = 0;
        spuVoices[n].freq     = 0;
        spuVoices[n].on       = false;
        spuVoices[n].pos      = 0;
        spuVoices[n].raddr    = 0;
        spuVoices[n].saddr    = 0;
        spuVoices[n].size     = 0;
        spuVoices[n].volume.l = 0;
        spuVoices[n].volume.r = 0;
    }
}

void CstrAudio::voiceOn(uh data) {
    for (int n = 0; n < MAX_CHANNELS; n++) {
        if (data & (1 << n)) {
            spuVoices[n].on    = true;
            spuVoices[n].count = 0;
            spuVoices[n].pos   = 0;
            spuVoices[n].raddr = 0;
            spuVoices[n].size  = 0;
            
            //depackVAG(spuVoices[n]);
        }
    }
}

void CstrAudio::voiceOff(uh data) {
    for (int n = 0; n < MAX_CHANNELS; n++) {
        if (data & (1 << n)) {
            spuVoices[n].on = false;
        }
    }
}

void CstrAudio::write(uw addr, uh data) {
    // Switch to low order bits
    addr = lob(addr);
    
    spuAcc(addr) = data;
    
    // HW
    switch(addr) {
        case 0x1d80: // Volume L
            spuVolumeL = data & MAX_VOLUME;
            return;
        
        case 0x1d82: // Volume R
            spuVolumeR = data & MAX_VOLUME;
            return;
        
        case 0x1d88: // Sound On 1
            voiceOn(data);
            return;
        
        case 0x1d8a: // Sound On 2
            voiceOn(data << 16);
            return;
        
        case 0x1d8c: // Sound Off 1
            voiceOff(data);
            return;
        
        case 0x1d8e: // Sound Off 2
            voiceOff(data << 16);
            return;
        
        case 0x1da6: // Transfer Address
            spuAddr = data << 3;
            return;
        
        case 0x1daa: // Control
            return;
        
        case 0x1d84: // Reverb Volume L
        case 0x1d86: // Reverb Volume R
        case 0x1d90: // FM Mode On 1
        case 0x1d92: // FM Mode On 2
        case 0x1d94: // Noise Mode On 1
        case 0x1d96: // Noise Mode On 2
        case 0x1d98: // Reverb Mode On 1
        case 0x1d9a: // Reverb Mode On 2
        case 0x1dac:
        case 0x1db0: // CD Volume L
        case 0x1db2: // CD Volume R
        case 0x1db4:
        case 0x1db6:
            return;
    }
    
    printx("PSeudo /// SPU write: $%x <- $%x", addr, data);
}

uh CstrAudio::read(uw addr) {
    printx("PSeudo /// SPU read: $%x", addr);
    return 0;
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    printx("PSeudo /// SPU DMA: $%x", dma->chcr);
}
