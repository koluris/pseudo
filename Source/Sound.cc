/* Base structure taken from SOPE open source emulator, and improved upon (Credits: SaD, Linuzappz) */

#include "Global.h"


CstrAudio audio;

// num of channels
#define MAXCHAN     24

///////////////////////////////////////////////////////////
// struct defines
///////////////////////////////////////////////////////////

// ADSR INFOS PER CHANNEL
typedef struct
{
 int            AttackModeExp;
 long           AttackTime;
 long           DecayTime;
 long           SustainLevel;
 int            SustainModeExp;
 long           SustainModeDec;
 long           SustainTime;
 int            ReleaseModeExp;
 unsigned long  ReleaseVal;
 long           ReleaseTime;
 long           ReleaseStartTime;
 long           ReleaseVol;
 long           lTime;
 long           lVolume;
} ADSRInfo;

typedef struct
{
 int            State;
 int            AttackModeExp;
 int            AttackRate;
 int            DecayRate;
 int            SustainLevel;
 int            SustainModeExp;
 int            SustainIncrease;
 int            SustainRate;
 int            ReleaseModeExp;
 int            ReleaseRate;
 int            EnvelopeVol;
 long           lVolume;
 long           lDummy1;
 long           lDummy2;
} ADSRInfoEx;

///////////////////////////////////////////////////////////

// MAIN CHANNEL STRUCT
typedef struct
{
 int               bNew;                               // start flag

 int               iSBPos;                             // mixing stuff
 int               spos;
 int               sinc;
 int               SB[32];                             // Pete added another 32 dwords in 1.6 ... prevents overflow issues with gaussian/cubic interpolation (thanx xodnizel!), and can be used for even better interpolations, eh? :)
 int               sval;

 unsigned char *   pStart;                             // start ptr into sound mem
 unsigned char *   pCurr;                              // current pos in sound mem
 unsigned char *   pLoop;                              // loop ptr in sound mem

 int               bOn;                                // is channel active (sample playing?)
 int               bStop;                              // is channel stopped (sample _can_ still be playing, ADSR Release phase)
 int               iActFreq;                           // current psx pitch
 int               iUsedFreq;                          // current pc pitch
 int               iLeftVolume;                        // left volume
 int               bIgnoreLoop;                        // ignore loop bit, if an external loop address is used
 int               iRightVolume;                       // right volume
 int               iRawPitch;                          // raw pitch (0...3fff)
 int               s_1;                                // last decoding infos
 int               s_2;
 int               bNoise;                             // noise active flag
 int               bFMod;                              // freq mod (0=off, 1=sound channel, 2=freq channel)
 int               iOldNoise;                          // old noise val for this channel
 ADSRInfo          ADSR;                               // active ADSR settings
 ADSRInfoEx        ADSRX;                              // next ADSR settings (will be moved to active on sample start)

} SPUCHAN;

///////////////////////////////////////////////////////////

typedef struct
{
 int StartAddr;      // reverb area start addr in samples
 int CurrAddr;       // reverb area curr addr in samples

 int VolLeft;
 int VolRight;
 int iLastRVBLeft;
 int iLastRVBRight;
 int iRVBLeft;
 int iRVBRight;


 int FB_SRC_A;       // (offset)
 int FB_SRC_B;       // (offset)
 int IIR_ALPHA;      // (coef.)
 int ACC_COEF_A;     // (coef.)
 int ACC_COEF_B;     // (coef.)
 int ACC_COEF_C;     // (coef.)
 int ACC_COEF_D;     // (coef.)
 int IIR_COEF;       // (coef.)
 int FB_ALPHA;       // (coef.)
 int FB_X;           // (coef.)
 int IIR_DEST_A0;    // (offset)
 int IIR_DEST_A1;    // (offset)
 int ACC_SRC_A0;     // (offset)
 int ACC_SRC_A1;     // (offset)
 int ACC_SRC_B0;     // (offset)
 int ACC_SRC_B1;     // (offset)
 int IIR_SRC_A0;     // (offset)
 int IIR_SRC_A1;     // (offset)
 int IIR_DEST_B0;    // (offset)
 int IIR_DEST_B1;    // (offset)
 int ACC_SRC_C0;     // (offset)
 int ACC_SRC_C1;     // (offset)
 int ACC_SRC_D0;     // (offset)
 int ACC_SRC_D1;     // (offset)
 int IIR_SRC_B1;     // (offset)
 int IIR_SRC_B0;     // (offset)
 int MIX_DEST_A0;    // (offset)
 int MIX_DEST_A1;    // (offset)
 int MIX_DEST_B0;    // (offset)
 int MIX_DEST_B1;    // (offset)
 int IN_COEF_L;      // (coef.)
 int IN_COEF_R;      // (coef.)
} REVERBInfo;

static uh regArea[10000];
static uh spuMem[256*1024];
static ub *spuMemC;
static ub *pSpuBuffer;
static ub *pMixIrq=0;

// infos struct for each channel
static SPUCHAN         s_chan[MAXCHAN+1];                     // channel + 1 infos (1 is security for fmod handling)
static REVERBInfo      rvb;

static uh spuCtrl = 0;                             // some vars to store psx reg infos
static uh spuStat = 0;
static uh spuIrq = 0;
static uw spuAddr = 0xFFFFFFFF;                    // address into spu mem

// TODO
int iFMod[44100];

unsigned long * XAFeed  = NULL;
unsigned long * XAPlay  = NULL;
unsigned long * XAStart = NULL;
unsigned long * XAEnd   = NULL;
unsigned long   XARepeat  = 0;

int             iLeftXAVol  = 32767;
int             iRightXAVol = 32767;

//static uh spuFranReadDMA() {
//    uh data = spuMem[spuAddr >> 1];
//    spuAddr = (spuAddr + 2) & 0x7FFFF;
//    return data;
//}
//
//static void spuFranWriteDMA(uh data) {
//    spuMem[spuAddr>>1] = data;
//    spuAddr = (spuAddr + 2) & 0x7FFFF;
//}

static void spuFranReadDMAMem(uh *pusPSXMem, int size) {
    if (spuAddr + (size<<1) >= 0x80000) {
        memcpy(pusPSXMem, &spuMem[spuAddr>>1], 0x7FFFF-spuAddr+1);
        memcpy(pusPSXMem+(0x7FFFF-spuAddr+1), spuMem, (size<<1)-(0x7FFFF-spuAddr+1));
        spuAddr = (size<<1) - (0x7FFFF-spuAddr+1);
    } else {
        memcpy(pusPSXMem,&spuMem[spuAddr>>1], (size<<1));
        spuAddr += (size<<1);
    }
}

static void spuFranWriteDMAMem(uh *pusPSXMem, int size) {
    if (spuAddr+(size<<1)>0x7FFFF) {
        memcpy(&spuMem[spuAddr>>1], pusPSXMem, 0x7FFFF-spuAddr+1);
        memcpy(spuMem, pusPSXMem+(0x7FFFF-spuAddr+1), (size<<1)-(0x7FFFF-spuAddr+1));
        spuAddr = (size<<1)-(0x7FFFF-spuAddr+1);
    } else {
        memcpy(&spuMem[spuAddr>>1], pusPSXMem, (size<<1));
        spuAddr += (size<<1);
    }
}

static inline void SoundOn(int start, int end, uh data) {
    for (int ch = start; ch < end; ch++,data>>=1) {
        if ((data&1) && s_chan[ch].pStart) {
            s_chan[ch].bIgnoreLoop=0;
            s_chan[ch].bNew=1;
        }
    }
}

static inline void SoundOff(int start,int end, uh data) {
    for (int ch = start; ch < end; ch++,data>>=1)
        s_chan[ch].bStop |= (data & 1);
}

static inline void FModOn(int start,int end, uh data) {
    for (int ch = start; ch < end; ch++,data>>=1) {
        if (data & 1) {
            if (ch > 0) {
                s_chan[ch].bFMod = 1;
                s_chan[ch-1].bFMod=2;
            }
        } else {
            s_chan[ch].bFMod = 0;
        }
    }
}

static inline void NoiseOn(int start,int end, uh data) {
    for (int ch=start;ch<end;ch++,data>>=1)
        s_chan[ch].bNoise = data & 1;
}

static inline int calcVolume(sh vol) {
    if (vol & 0x8000) {
        int sInc=1;
        if (vol & 0x2000)
            sInc = -1;
        if (vol & 0x1000)
            vol ^= 0xFFFF;
        vol = ((vol&0x7F)+1)/2;
        vol += vol/(2*sInc);
        vol *= 128;
    } else {
        if (vol & 0x4000)
            vol = 0x3FFF - (vol&0x3FFF);
    }
    vol &= 0x3FFF;
    return vol;
}

static inline void setPitch(int ch, int val) {
    val = MIN(val, 0x3FFF);
    s_chan[ch].iRawPitch = val;
    val = (44100*val) / 4096;
    if (val < 1)
        val = 1;
    s_chan[ch].iActFreq = val;
}

void CstrAudio::write(uw reg, uh data) {
    reg &= 0xFFF;
    regArea[(reg-0xC00)>>1] = data;
    if (reg>=0x0C00 && reg<0x0D80) {
        int ch = (reg>>4) - 0xC0;
        switch (reg&0x0F) {
        case 0:
            s_chan[ch].iLeftVolume = calcVolume(data);
            break;
        case 2:
            s_chan[ch].iRightVolume = calcVolume(data);
            break;
        case 4:
            setPitch(ch, data);
            break;
        case 6:
            s_chan[ch].pStart = spuMemC + (data<<3);
            break;
        case 8:
            s_chan[ch].ADSRX.AttackModeExp=(data&0x8000) >> 15;
            s_chan[ch].ADSRX.AttackRate = ((data>>8) & 0x007F)^0x7F;
            s_chan[ch].ADSRX.DecayRate = 4*(((data>>4) & 0x000F)^0x1F);
            s_chan[ch].ADSRX.SustainLevel = (data & 0x000F) << 27;
            break;
        case 10:
            s_chan[ch].ADSRX.SustainModeExp = (data&0x8000) >> 15;
            s_chan[ch].ADSRX.SustainIncrease= (data&0x4000) >> 14;
            s_chan[ch].ADSRX.SustainRate = ((data>>6) & 0x007F)^0x7F;
            s_chan[ch].ADSRX.ReleaseModeExp = (data&0x0020) >> 5;
            s_chan[ch].ADSRX.ReleaseRate = 4*((data & 0x001F)^0x1F);
            break;
        case 12:
            break;
        case 14:
            s_chan[ch].pLoop = spuMemC + (data<<3);
            s_chan[ch].bIgnoreLoop = 1;
            break;
        }
        return;
    }
    switch (reg) {
    case H_SPUaddr:
        spuAddr = data << 3;
        break;
    case H_SPUdata:
        spuMem[spuAddr>>1] = data;
        spuAddr += 2;
        spuAddr &= 0x7FFFF;
        break;
    case H_SPUctrl    : spuCtrl=data;         break;
    case H_SPUstat    : spuStat=data & 0xF800;     break;
    case H_SPUReverbAddr:
        if (data==0xFFFF || data<=0x200) {
            rvb.StartAddr=rvb.CurrAddr=0;
        } else {
            if (rvb.StartAddr != (data<<2)) {
                rvb.StartAddr = data<<2;
                rvb.CurrAddr=rvb.StartAddr;
            }
        }
        break;
    case H_SPUirqAddr : spuIrq = data;        break;
    case H_SPUrvolL   : rvb.VolLeft=data;         break;
    case H_SPUrvolR   : rvb.VolRight=data;         break;
    case H_SPUon1     : SoundOn(0,16,data);         break;
    case H_SPUon2     : SoundOn(16,24,data);     break;
    case H_SPUoff1    : SoundOff(0,16,data);     break;
    case H_SPUoff2    : SoundOff(16,24,data);     break;
    case H_CDLeft     : iLeftXAVol=data  & 0x7FFF;     break;
    case H_CDRight    : iRightXAVol=data & 0x7FFF;     break;
    case H_FMod1      : FModOn(0,16,data);         break;
    case H_FMod2      : FModOn(16,24,data);         break;
    case H_Noise1     : NoiseOn(0,16,data);         break;
    case H_Noise2     : NoiseOn(16,24,data);     break;
    case H_RVBon1      : break;
    case H_RVBon2      : break;
    case H_Reverb+0   : rvb.FB_SRC_A=data;      break;
    case H_Reverb+2   : rvb.FB_SRC_B=data;    break;
    case H_Reverb+4   : rvb.IIR_ALPHA=data;   break;
    case H_Reverb+6   : rvb.ACC_COEF_A=data;  break;
    case H_Reverb+8   : rvb.ACC_COEF_B=data;  break;
    case H_Reverb+10  : rvb.ACC_COEF_C=data;  break;
    case H_Reverb+12  : rvb.ACC_COEF_D=data;  break;
    case H_Reverb+14  : rvb.IIR_COEF=data;    break;
    case H_Reverb+16  : rvb.FB_ALPHA=data;    break;
    case H_Reverb+18  : rvb.FB_X=data;        break;
    case H_Reverb+20  : rvb.IIR_DEST_A0=data; break;
    case H_Reverb+22  : rvb.IIR_DEST_A1=data; break;
    case H_Reverb+24  : rvb.ACC_SRC_A0=data;  break;
    case H_Reverb+26  : rvb.ACC_SRC_A1=data;  break;
    case H_Reverb+28  : rvb.ACC_SRC_B0=data;  break;
    case H_Reverb+30  : rvb.ACC_SRC_B1=data;  break;
    case H_Reverb+32  : rvb.IIR_SRC_A0=data;  break;
    case H_Reverb+34  : rvb.IIR_SRC_A1=data;  break;
    case H_Reverb+36  : rvb.IIR_DEST_B0=data; break;
    case H_Reverb+38  : rvb.IIR_DEST_B1=data; break;
    case H_Reverb+40  : rvb.ACC_SRC_C0=data;  break;
    case H_Reverb+42  : rvb.ACC_SRC_C1=data;  break;
    case H_Reverb+44  : rvb.ACC_SRC_D0=data;  break;
    case H_Reverb+46  : rvb.ACC_SRC_D1=data;  break;
    case H_Reverb+48  : rvb.IIR_SRC_B1=data;  break;
    case H_Reverb+50  : rvb.IIR_SRC_B0=data;  break;
    case H_Reverb+52  : rvb.MIX_DEST_A0=data; break;
    case H_Reverb+54  : rvb.MIX_DEST_A1=data; break;
    case H_Reverb+56  : rvb.MIX_DEST_B0=data; break;
    case H_Reverb+58  : rvb.MIX_DEST_B1=data; break;
    case H_Reverb+60  : rvb.IN_COEF_L=data;   break;
    case H_Reverb+62  : rvb.IN_COEF_R=data;   break;
    }
}

uh CstrAudio::read(uw reg) {
    reg &= 0xFFF;
    if (reg>=0x0C00 && reg<0x0D80) {
        int ch=(reg>>4)-0xc0;
        switch (reg&0x0F) {
        case 12: {
            if (s_chan[ch].bNew)
                return 1;
            if (s_chan[ch].ADSRX.lVolume && !s_chan[ch].ADSRX.EnvelopeVol)
                return 1;
            return s_chan[ch].ADSRX.EnvelopeVol >> 16;
        case 14:
            if (!s_chan[ch].pLoop)
                return 0;
            return (s_chan[ch].pLoop-spuMemC) >> 3;
        }
        }
    }

    switch(reg) {
    case H_SPUctrl: return spuCtrl;
    case H_SPUstat: return spuStat;
    case H_SPUaddr: return spuAddr>>3;
    case H_SPUdata: {
        uh s = spuMem[spuAddr>>1];
        spuAddr += 2;
        spuAddr &= 0x7FFFF;
        return s;
    }
    case H_SPUirqAddr: return spuIrq;
    }
    return regArea[(reg-0xc00)>>1];
}

// MIX XA
//static inline void MixXA(uh *dst, int nSamples) {
//    int i;
//    unsigned long XALastVal;
//    int leftvol =iLeftXAVol;
//    int rightvol=iRightXAVol;
//
//    for(i=0;i<nSamples && XAPlay!=XAFeed;i++)
//    {
//        XALastVal=*XAPlay++;
//        if(XAPlay==XAEnd) XAPlay=XAStart;
//        (*dst++)+=(((short)(XALastVal&0xffff))       * leftvol)>>15;
//        (*dst++)+=(((short)((XALastVal>>16)&0xffff)) * rightvol)>>15;
//    }
//
//    if(XAPlay==XAFeed && XARepeat)
//    {
//        XARepeat--;
//        for(;i<nSamples;i++)
//            {
//                (*dst++)+=(((short)(XALastVal&0xffff))       * leftvol)>>15;
//                (*dst++)+=(((short)((XALastVal>>16)&0xffff)) * rightvol)>>15;
//            }
//    }
//}

static unsigned long int RateTable[160];

/* INIT ADSR */
void InitADSR(void)
{
    unsigned long r=3,xrs=1,xrd=0;
    int i;
    memset(RateTable,0,sizeof(unsigned long)*160);        // build the rate table according to Neill's rules (see at bottom of file)

    for(i=32;i<160;i++)                                   // we start at pos 32 with the real values... everything before is 0
    {
        if(r<0x3FFFFFFF)
            {
                r+=xrs;
                xrd++;
                if(xrd==5) {
                    xrd=1;
                    xrs*=2;
                }
            }
        if(r>0x3FFFFFFF)
            r=0x3FFFFFFF;
        RateTable[i]=r;
    }
}

static const uw TableDisp[] = {
 -0x18+0+32,-0x18+4+32,-0x18+6+32,-0x18+8+32,       // release/decay
 -0x18+9+32,-0x18+10+32,-0x18+11+32,-0x18+12+32,

 -0x1B+0+32,-0x1B+4+32,-0x1B+6+32,-0x1B+8+32,       // sustain
 -0x1B+9+32,-0x1B+10+32,-0x1B+11+32,-0x1B+12+32,
};


/* MIX ADSR */
static inline int MixADSR(SPUCHAN *ch) {
    uw xdisp;
    sw EnvelopeVol = ch->ADSRX.EnvelopeVol;

    if (ch->bStop) {
        if(ch->ADSRX.ReleaseModeExp)
                xdisp = TableDisp[(EnvelopeVol>>28)&0x7];
        else
                xdisp=-0x0C+32;

        EnvelopeVol-=RateTable[ch->ADSRX.ReleaseRate + xdisp];

        if(EnvelopeVol<0)
            {
                EnvelopeVol=0;
                ch->bOn=0;
            }

        ch->ADSRX.EnvelopeVol=EnvelopeVol;
        ch->ADSRX.lVolume=(EnvelopeVol>>=21);
        return EnvelopeVol;
    }
    else                                                  // not stopped yet?
    {
        if(ch->ADSRX.State==0)                       // -> attack
            {
                xdisp = -0x10+32;
                if(ch->ADSRX.AttackModeExp)
                {
                    if(EnvelopeVol>=0x60000000)
                        xdisp = -0x18+32;
                }
                EnvelopeVol+=RateTable[ch->ADSRX.AttackRate+xdisp];

                if(EnvelopeVol<0)
                {
                    EnvelopeVol=0x7FFFFFFF;
                    ch->ADSRX.State=1;
                }

                ch->ADSRX.EnvelopeVol=EnvelopeVol;
                ch->ADSRX.lVolume=(EnvelopeVol>>=21);
                return EnvelopeVol;
            }

        if(ch->ADSRX.State==1)                       // -> decay
            {
                xdisp = TableDisp[(EnvelopeVol>>28)&0x7];
                EnvelopeVol-=RateTable[ch->ADSRX.DecayRate+xdisp];

                if(EnvelopeVol<0)
                    EnvelopeVol=0;
                if(EnvelopeVol <= ch->ADSRX.SustainLevel)
                    ch->ADSRX.State=2;

                ch->ADSRX.EnvelopeVol=EnvelopeVol;
                ch->ADSRX.lVolume=(EnvelopeVol>>=21);
                return EnvelopeVol;
            }

        if(ch->ADSRX.State==2)                       // -> sustain
            {
                if(ch->ADSRX.SustainIncrease)
                {
                    xdisp = -0x10+32;
                    if(ch->ADSRX.SustainModeExp)
                    {
                        if(EnvelopeVol>=0x60000000)
                            xdisp = -0x18+32;
                    }
                    EnvelopeVol+=RateTable[ch->ADSRX.SustainRate+xdisp];

                    if(EnvelopeVol<0)
                        EnvelopeVol=0x7FFFFFFF;
                }
                else
                {
                    if(ch->ADSRX.SustainModeExp)
                        xdisp = TableDisp[((EnvelopeVol>>28)&0x7)+8];
                    else
                        xdisp=-0x0F+32;

                    EnvelopeVol-=RateTable[ch->ADSRX.SustainRate+xdisp];

                    if(EnvelopeVol<0)
                        EnvelopeVol=0;
                }

                ch->ADSRX.EnvelopeVol=EnvelopeVol;
                ch->ADSRX.lVolume=(EnvelopeVol>>=21);
                return EnvelopeVol;
            }
    }
    return 0;
}

// FEED XA
//inline void FeedXA(xa_decode_t *xap)
//{
//    int sinc,spos,i,iSize;
//    XARepeat  = 100;                                      // set up repeat
//
//    iSize=((44100*xap->nsamples)/xap->freq);              // get size
//    if(!iSize) return;                                    // none? bye
//
//    if(XAFeed<XAPlay) {
//        if ((XAPlay-XAFeed)==0) return;               // how much space in my buf?
//    } else {
//        if (((XAEnd-XAFeed) + (XAPlay-XAStart))==0) return;
//    }
//
//    spos=0x10000L;
//    sinc = (xap->nsamples << 16) / iSize;                 // calc freq by num / size
//
//    if(xap->stereo)
//    {
//        unsigned long * pS=(unsigned long *)xap->pcm;
//        unsigned long l=0;
//
//        for(i=0;i<iSize;i++)
//            {
//                while(spos>=0x10000L)
//                {
//                    l = *pS++;
//                    spos -= 0x10000L;
//                }
//
//                *XAFeed++=l;
//
//                if(XAFeed==XAEnd)
//                    XAFeed=XAStart;
//                if(XAFeed==XAPlay)
//                {
//                    if(XAPlay!=XAStart)
//                        XAFeed=XAPlay-1;
//                    break;
//                }
//
//                spos += sinc;
//            }
//    }
//    else
//    {
//        unsigned short * pS=(unsigned short *)xap->pcm;
//        unsigned long l;short s=0;
//
//        for(i=0;i<iSize;i++)
//            {
//                while(spos>=0x10000L)
//                {
//                    s = *pS++;
//                    spos -= 0x10000L;
//                }
//                l=s;
//
//                *XAFeed++=(l|(l<<16));
//
//                if(XAFeed==XAEnd)
//                    XAFeed=XAStart;
//                if(XAFeed==XAPlay)
//                {
//                    if(XAPlay!=XAStart)
//                        XAFeed=XAPlay-1;
//                    break;
//                }
//
//                spos += sinc;
//            }
//    }
//}

static inline void StartSound(SPUCHAN * pChannel) {
    pChannel->ADSRX.lVolume=1;                            // Start ADSR
    pChannel->ADSRX.State=0;
    pChannel->ADSRX.EnvelopeVol=0;
    pChannel->pCurr=pChannel->pStart;                     // set sample start
    pChannel->s_1=0;                                      // init mixing vars
    pChannel->s_2=0;
    pChannel->iSBPos=28;
    pChannel->bNew=0;                                     // init channel flags
    pChannel->bStop=0;
    pChannel->bOn=1;
    pChannel->SB[29]=0;                                   // init our interpolation helpers
    pChannel->SB[30]=0;
    pChannel->spos=0x10000L;
    pChannel->SB[31]=0;                    // -> no/simple interpolation starts with one 44100 decoding
}

static inline void VoiceChangeFrequency(SPUCHAN * pChannel) {
    pChannel->iUsedFreq=pChannel->iActFreq;               // -> take it and calc steps
    pChannel->sinc=pChannel->iRawPitch<<4;
    if(!pChannel->sinc) pChannel->sinc=1;
}

inline void FModChangeFrequency(SPUCHAN * pChannel,int ns)
{
    int NP=pChannel->iRawPitch;
    NP=((32768L+iFMod[ns])*NP)/32768L;
    if(NP>0x3FFF) NP=0x3FFF;
    if(NP<0x1)    NP=0x1;
    NP=(44100L*NP)/(4096L);                               // calc frequency
    pChannel->iActFreq=NP;
    pChannel->iUsedFreq=NP;
    pChannel->sinc=(((NP/10)<<16)/4410);
    if(!pChannel->sinc) pChannel->sinc=1;
    iFMod[ns]=0;
}

inline void StoreInterpolationVal(SPUCHAN * pChannel,int fa)
{
    if(pChannel->bFMod==2)                                    // fmod freq channel
        pChannel->SB[29]=fa;
    else
    {
        if((spuCtrl&0x4000)==0)
            fa=0;                               // muted?
        else                                            // else adjust
            {
                if(fa>32767L)  fa=32767L;
                if(fa<-32767L) fa=-32767L;
            }
        pChannel->SB[29]=fa;                               // no interpolation
    }
}

//static void spuFranPlayADPCMchannel(xa_decode_t *xap) {
//    Q_ASSERT(xap != 0);
//    if (xap->freq)
//        FeedXA(xap); // call main XA feeder
//}

//static void spuFranPlayCDDAchannel(sh *, int) {
//}

bool CstrAudio::init() {
//    SPU_writeRegister        = spuFranWriteRegister;
//    SPU_readRegister        = spuFranReadRegister;
//    SPU_writeDMA            = spuFranWriteDMA;
//    SPU_readDMA                = spuFranReadDMA;
//    SPU_writeDMAMem            = spuFranWriteDMAMem;
//    SPU_readDMAMem            = spuFranReadDMAMem;
//    SPU_playADPCMchannel    = spuFranPlayADPCMchannel;
//    SPU_playCDDAchannel        = spuFranPlayCDDAchannel;

    spuMemC=(unsigned char *)spuMem;                      // just small setup
    memset((void *)s_chan,0,MAXCHAN*sizeof(SPUCHAN));
    memset((void *)&rvb,0,sizeof(REVERBInfo));
    InitADSR();

    spuIrq=0;
    spuAddr=0xFFFFFFFF;
    spuMemC=(unsigned char *)spuMem;
    pMixIrq=0;
    memset((void *)s_chan,0,(MAXCHAN+1)*sizeof(SPUCHAN));

    //Setup streams
    pSpuBuffer=(unsigned char *)malloc(32768);            // alloc mixing buffer
    XAStart = (unsigned long *)malloc(44100*4);           // alloc xa buffer
    XAPlay  = XAStart;
    XAFeed  = XAStart;
    XAEnd   = XAStart + 44100;
    for(int i=0;i<MAXCHAN;i++)                                // loop sound channels
    {
        s_chan[i].ADSRX.SustainLevel = 0xf<<27;       // -> init sustain
        s_chan[i].pLoop=spuMemC;
        s_chan[i].pStart=spuMemC;
        s_chan[i].pCurr=spuMemC;
    }
    memset(iFMod, 0, sizeof(iFMod));
    return true;
}

void CstrAudio::reset() {
}

void CstrAudio::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void CstrAudio::decodeStream() {
    while(!psx.suspended) {
        int sampleCount = 256 / 4;
        memset(&sbuf, 0, sampleCount * 4);
        
        int s_1,s_2,fa;
        int predict_nr,shift_factor,flags,s;
        static const int f[5][2] = {{0,0},{60,0},{115,-52},{98,-55},{122,-60}};

        //uh *dst = (uh *)sbuf;
        for (SPUCHAN *ch = s_chan; ch != s_chan + MAXCHAN; ch++) {
            if (ch->bNew)
                StartSound(ch);
            if (!ch->bOn)
                continue;
            if (ch->iActFreq != ch->iUsedFreq)
                VoiceChangeFrequency(ch);
            for (int ns = 0; ns < sampleCount; ns++) {
                if (ch->bFMod==1 && iFMod[ns])     // fmod freq channel
                    FModChangeFrequency(ch, ns);
                while (ch->spos >= 0x10000L) {
                    if (ch->iSBPos == 28) {
                        if (ch->pCurr == (ub *)-1) // special "stop" sign
                        {
                            ch->bOn=0; // -> turn everything off
                            ch->ADSRX.lVolume=0;
                            ch->ADSRX.EnvelopeVol=0;
                            goto ENDX;       // -> and done for this channel
                        }
                        ch->iSBPos=0;
                        s_1=ch->s_1;
                        s_2=ch->s_2;
                        predict_nr=(int)*ch->pCurr;
                        ch->pCurr++;
                        shift_factor=predict_nr&0xf;
                        predict_nr >>= 4;
                        flags=(int)*ch->pCurr;
                        ch->pCurr++;

                        for (int i=0;i<28;ch->pCurr++) {
                            s=((((int)*ch->pCurr)&0xf)<<12);
                            if(s&0x8000) s|=0xFFFF0000;
                            fa=(s >> shift_factor);
                            fa=fa + ((s_1 * f[predict_nr][0])>>6) + ((s_2 * f[predict_nr][1])>>6);
                            s_2=s_1;s_1=fa;
                            s=((((int)*ch->pCurr) & 0xf0) << 8);
                            ch->SB[i++]=fa;
                            if(s&0x8000) s|=0xFFFF0000;
                            fa=(s>>shift_factor);
                            fa=fa + ((s_1 * f[predict_nr][0])>>6) + ((s_2 * f[predict_nr][1])>>6);
                            s_2=s_1;s_1=fa;
                            ch->SB[i++]=fa;
                        }

                        // flag handler
                        if ((flags&4) && (!ch->bIgnoreLoop))
                            ch->pLoop=ch->pCurr-16;                // loop address
                        if (flags&1)                                   // 1: stop/loop
                        {
                            // We play this block out first...
                            if(flags!=3 || ch->pLoop==NULL)   // PETE: if we don't check exactly for 3, loop hang ups will happen (DQ4, for example)
                                ch->pCurr = (ub*)-1;    // and checking if pLoop is set avoids crashes, yeah
                            else
                                ch->pCurr = ch->pLoop;
                        }
                        ch->s_1=s_1;
                        ch->s_2=s_2;
                    }
                    fa=ch->SB[ch->iSBPos++];        // get sample data
                    StoreInterpolationVal(ch,fa);         // store val for later interpolation
                    ch->spos -= 0x10000L;
                }

                if (ch->bNoise)
                    fa = 0;
                else
                    fa = ch->SB[29];               // get interpolation val
                ch->sval = fa >> 2;   // mix adsr
                //ch->sval = (MixADSR(ch)*fa)>>10;   // mix adsr
                if (ch->bFMod==2) {                       // fmod freq channel
                    iFMod[ns]=ch->sval;             // -> store 1T sample data, use that to do fmod on next channel
                } else {
                    sbuf[ns*2+0]+=(ch->sval*ch->iLeftVolume)>>14;
                    sbuf[ns*2+1]+=(ch->sval*ch->iRightVolume)>>14;
                }
                ch->spos += ch->sinc;
            }
            ENDX: ;
        }
        
        // OpenAL
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        if (processed >= SPU_ALC_BUF_AMOUNT) {
            // We have to free buffers
            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
        }

        while(--processed < 0) {
            stream();
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        }

        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, sampleCount * 4, SPU_SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
        stream();

        //if (XAPlay!=XAFeed || XARepeat)
            //MixXA((uh *)stream, sampleCount);
    }
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;

    switch(dma->chcr) {
        case 0x01000201: // Write
            spuFranWriteDMAMem(p, size);
            return;

        case 0x01000200: // Read
            spuFranReadDMAMem(p, size);
            return;
    }

    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
}


//void CstrAudio::reset() {
//    // Mem
//    memset(&spuMem, 0, sizeof(spuMem));
//    spuAddr = 0xffffffff;
//
//    // Channels reset
//    for (auto &item : spuVoices) {
//        item = { 0 };
//    }
//}
//
//sh CstrAudio::setVolume(sh data) { // Sweep mode
//    return ((data & 0x7fff) ^ 0x4000) - 0x4000;
//}
//
//#define audioSet(a, b) \
//    rest = (*p & a) << b; \
//    if (rest & 0x8000) rest |= 0xffff0000; \
//    rest = (rest >> shift) + ((s[0] * f[predict][0] + s[1] * f[predict][1] + 32) >> 6); \
//    s[1] = s[0]; \
//    s[0] = MIN(MAX(rest, SHRT_MIN), SHRT_MAX); \
//    chn.bfr[chn.size++] = s[0]
//
//void CstrAudio::voiceOn(uw data) {
//    for (int n = 0; n < SPU_CHANNELS; n++) {
//        if (data & (1 << n)) {
//            auto &chn = spuVoices[n];
//            chn.count = 0;
//            chn.pos   = 0;
//            chn.raddr = 0;
//            chn.size  = 0;
//
//            { // Depack VAG
//                ub *p = (ub *)&spuMem[chn.saddr >> 1];
//
//                static sw s[2] = {
//                    0,
//                    0,
//                };
//
//                for(; chn.size < SPU_CHANNEL_BUF_SIZE - 28;) {
//                    ub shift   = *p & 0xf;
//                    ub predict = *p++ >> 4;
//                    ub op      = *p++;
//
//                    for (int i = chn.size, rest; chn.size < i + 28; p++) {
//                        audioSet(0x0f, 0xc);
//                        audioSet(0xf0, 0x8);
//                    }
//
//                    switch(op) {
//                        case 3: // End
//                        case 7:
//                            redirect NEXT_CHANNEL;
//
//                        case 6: // Repeat
//                            chn.raddr = chn.size;
//                    }
//                }
//
//                printf("/// PSeudo SPU Channel size overflow\n");
//            }
//        }
//        NEXT_CHANNEL: ;
//    }
//}
//
//void CstrAudio::voiceOff(uw data) {
//    for (int n = 0; n < SPU_CHANNELS; n++) {
//        if (data & (1 << n)) {
//        }
//    }
//}
//
//void CstrAudio::stream() {
//    ALint state;
//    alGetSourcei(source, AL_SOURCE_STATE, &state);
//
//    if (state != AL_PLAYING) {
//        alSourceStream(source);
//    }
//}
//
//void CstrAudio::decodeStream() {
//    while(!psx.suspended) {
//        sw temp[SPU_SAMPLE_SIZE] = { 0 };
//
//        // Clear
//        memset(&sbuf, 0, sizeof(sbuf));
//
//        for (auto &chn : spuVoices) {
//            // Channel on?
//            if (chn.size < 1) {
//                continue;
//            }
//
//            for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
//                chn.count += chn.freq;
//
//                if (chn.count >= SPU_SAMPLE_RATE) {
//                    chn.pos   += chn.count / SPU_SAMPLE_RATE;
//                    chn.count %= SPU_SAMPLE_RATE;
//                }
//
//                // Mix Channel Samples
//                temp[i + 0] += (+chn.bfr[chn.pos] * chn.volumeL) / SPU_MAX_VOLUME;
//                temp[i + 1] += (-chn.bfr[chn.pos] * chn.volumeR) / SPU_MAX_VOLUME;
//
//                // End of Sample
//                if (chn.pos >= chn.size) {
//                    if (chn.raddr > 0) { // Repeat?
//                        chn.pos   = chn.raddr;
//                        chn.count = 0;
//                        continue;
//                    }
//
//                    chn.size = 0;
//                    break;
//                }
//            }
//        }
//
//        // Volume Mix
//        for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
//            sbuf[i + 0] = MIN(MAX(+(temp[i + 0] / 2), SHRT_MIN), SHRT_MAX);
//            sbuf[i + 1] = MIN(MAX(-(temp[i + 1] / 2), SHRT_MIN), SHRT_MAX);
//        }
//
//        // OpenAL
//        ALint processed;
//        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
//
//        if (processed >= SPU_ALC_BUF_AMOUNT) {
//            // We have to free buffers
//            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
//        }
//
//        while(--processed < 0) {
//            stream();
//            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
//        }
//
//        ALuint buffer;
//        alSourceUnqueueBuffers(source, 1, &buffer);
//        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
//        alSourceQueueBuffers(source, 1, &buffer);
//        stream();
//    }
//}
//
//#define spuChannel(addr) \
//    (addr >> 4) & 0x1f
//
//void CstrAudio::write(uw addr, uh data) {
//    switch(LOW_BITS(addr)) {
//        case 0x1c00 ... 0x1d7e: // Channels
//            {
//                ub n = spuChannel(addr);
//
//                switch(addr & 0xf) {
//                    case 0x0: // Volume L
//                        spuVoices[n].volumeL = setVolume(data);
//                        return;
//
//                    case 0x2: // Volume R
//                        spuVoices[n].volumeR = setVolume(data);
//                        return;
//
//                    case 0x4: // Pitch
//                        spuVoices[n].freq = MAX((data * SPU_SAMPLE_RATE) / 4096, 1);
//                        return;
//
//                    case 0x6: // Sound Address
//                        spuVoices[n].saddr = data << 3;
//                        return;
//
//                    case 0xe: // Return Address
//                        spuVoices[n].raddr = data << 3;
//                        return;
//
//                    /* unused */
//                    case 0x8:
//                    case 0xa:
//                    case 0xc:
//                        accessMem(mem.hwr, uh) = data;
//                        return;
//                }
//
//                printx("/// PSeudo SPU write phase: 0x%x <- 0x%x", (addr & 0xf), data);
//                return;
//            }
//
//        case 0x1d88: // Sound On 1
//            voiceOn(data);
//            return;
//
//        case 0x1d8a: // Sound On 2
//            voiceOn(data << 16);
//            return;
//
//        case 0x1d8c: // Sound Off 1
//            voiceOff(data);
//            return;
//
//        case 0x1d8e: // Sound Off 2
//            voiceOff(data << 16);
//            return;
//
//        case 0x1da6: // Transfer Address
//            spuAddr = data << 3;
//            return;
//
//        case 0x1da8: // Data
//            spuMem[spuAddr >> 1] = data;
//            spuAddr += 2;
//            spuAddr &= 0x7ffff;
//            return;
//
//        /* unused */
//        case 0x1d80: // Volume L
//        case 0x1d82: // Volume R
//        case 0x1d84: // Reverb Volume L
//        case 0x1d86: // Reverb Volume R
//        case 0x1d90: // FM Mode On 1
//        case 0x1d92: // FM Mode On 2
//        case 0x1d94: // Noise Mode On 1
//        case 0x1d96: // Noise Mode On 2
//        case 0x1d98: // Reverb Mode On 1
//        case 0x1d9a: // Reverb Mode On 2
//        case 0x1d9c: // Mute 1
//        case 0x1d9e: // Mute 2
//        case 0x1daa: // Control
//        case 0x1da2: // Reverb Address
//        case 0x1dac: // ?
//        case 0x1db0: // CD Volume L
//        case 0x1db2: // CD Volume R
//        case 0x1db4:
//        case 0x1db6:
//        case 0x1dc0 ... 0x1dfe: // Reverb
//            accessMem(mem.hwr, uh) = data;
//            return;
//    }
//
//    printx("/// PSeudo SPU write: 0x%x <- 0x%x", addr, data);
//}
//
//uh CstrAudio::read(uw addr) {
//    switch(LOW_BITS(addr)) {
//        case 0x1c00 ... 0x1d7e: // Channels
//            {
//                ub n = spuChannel(addr);
//
//                switch(addr & 0xf) {
//                    case 0xc: // Hack
//                        if (spuVoices[n].size) {
//                            return 1;
//                        }
//                        return 0;
//
//                    /* unused */
//                    case 0x0:
//                    case 0x2:
//                    case 0x4:
//                    case 0x6:
//                    case 0x8:
//                    case 0xa:
//                    case 0xe: // madman
//                        return accessMem(mem.hwr, uh);
//                }
//
//                printx("/// PSeudo SPU read phase: 0x%x", (addr & 0xf));
//                return 0;
//            }
//
//        case 0x1da6: // Transfer Address
//            return spuAddr >> 3;
//
//        /* unused */
//        case 0x1d88: // Sound On 1
//        case 0x1d8a: // Sound On 2
//        case 0x1d8c: // Sound Off 1
//        case 0x1d8e: // Sound Off 2
//        case 0x1d94: // Noise Mode On 1
//        case 0x1d96: // Noise Mode On 2
//        case 0x1d98: // Reverb Mode On 1
//        case 0x1d9a: // Reverb Mode On 2
//        case 0x1d9c: // Voice Status 0 - 15
//        case 0x1daa: // Control
//        case 0x1dac: // ?
//        case 0x1dae: // Status
//        case 0x1db8:
//        case 0x1dba:
//        case 0x1e00:
//        case 0x1e02:
//        case 0x1e04:
//        case 0x1e06:
//        case 0x1e08:
//        case 0x1e0a:
//        case 0x1e0c:
//        case 0x1e0e:
//            return accessMem(mem.hwr, uh);
//    }
//
//    printx("/// PSeudo SPU read: 0x%x", addr);
//    return 0;
//}
//
//void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
//    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
//    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
//
//    switch(dma->chcr) {
//        case 0x01000201: // Write
//            for (uw i = 0; i < size; i++) {
//                spuMem[spuAddr >> 1] = *p++;
//                spuAddr += 2;
//                spuAddr &= 0x7ffff;
//            }
//            return;
//
//        case 0x01000200: // Read
//            for (uw i = 0; i < size; i++) {
//                *p++ = spuMem[spuAddr >> 1];
//                spuAddr += 2;
//                spuAddr &= 0x7ffff;
//            }
//            return;
//    }
//
//    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
//}
