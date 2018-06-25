#import "Global.h"


#ifndef MYSPU

PsxSpuFran psxSpuFran;

#define MAXCHAN 24

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

typedef struct
{
 int               bNew;
 int               iSBPos;
 int               spos;
 int               sinc;
 int               SB[32];
 int               sval;
 unsigned char *   pStart;
 unsigned char *   pCurr;
 unsigned char *   pLoop;
 int               bOn;
 int               bStop;
 int               iActFreq;
 int               iUsedFreq;
 int               iLeftVolume;
 int               bIgnoreLoop;
 int               iRightVolume;
 int               iRawPitch;
 int               s_1;
 int               s_2;
 int               bNoise;
 int               bFMod;
 int               iOldNoise;
 ADSRInfo          ADSR;
 ADSRInfoEx        ADSRX;
} SPUCHAN;

typedef struct
{
 int StartAddr;
 int CurrAddr;
 int VolLeft;
 int VolRight;
 int iLastRVBLeft;
 int iLastRVBRight;
 int iRVBLeft;
 int iRVBRight;
 int FB_SRC_A;
 int FB_SRC_B;
 int IIR_ALPHA;
 int ACC_COEF_A;
 int ACC_COEF_B;
 int ACC_COEF_C;
 int ACC_COEF_D;
 int IIR_COEF;
 int FB_ALPHA;
 int FB_X;
 int IIR_DEST_A0;
 int IIR_DEST_A1;
 int ACC_SRC_A0;
 int ACC_SRC_A1;
 int ACC_SRC_B0;
 int ACC_SRC_B1;
 int IIR_SRC_A0;
 int IIR_SRC_A1;
 int IIR_DEST_B0;
 int IIR_DEST_B1;
 int ACC_SRC_C0;
 int ACC_SRC_C1;
 int ACC_SRC_D0;
 int ACC_SRC_D1;
 int IIR_SRC_B1;
 int IIR_SRC_B0;
 int MIX_DEST_A0;
 int MIX_DEST_A1;
 int MIX_DEST_B0;
 int MIX_DEST_B1;
 int IN_COEF_L;
 int IN_COEF_R;
} REVERBInfo;

static uh regArea[10000];
static uh spuMem[256*1024];
static ub *spuMemC;
static ub *pMixIrq=0;
static SPUCHAN         s_chan[MAXCHAN+1];
static REVERBInfo      rvb;

static uh spuCtrl = 0;
static uh spuStat = 0;
static uh spuIrq = 0;
static uw spuAddr = 0xFFFFFFFF;

int iFMod[44100];

static unsigned long int RateTable[160];

void PsxSpuFran::init() {
    spuMemC = (unsigned char *)spuMem;
    spuMemC = (unsigned char *)spuMem;
}

void PsxSpuFran::reset() {
    memset((void *)s_chan,0,MAXCHAN*sizeof(SPUCHAN));
    memset((void *)&rvb,0,sizeof(REVERBInfo));
    
    spuIrq=0;
    spuAddr=0xFFFFFFFF;
    pMixIrq=0;
    memset((void *)s_chan,0,(MAXCHAN+1)*sizeof(SPUCHAN));
    
    for(int i=0;i<MAXCHAN;i++)
    {
        s_chan[i].ADSRX.SustainLevel = 0xf<<27;
        s_chan[i].pLoop=spuMemC;
        s_chan[i].pStart=spuMemC;
        s_chan[i].pCurr=spuMemC;
    }
    memset(iFMod, 0, sizeof(iFMod));
    
//    {
//        unsigned long r=3,rs2=1,rd2=0;
//        int i;
//        memset(RateTable,0,sizeof(unsigned long)*160);
//        
//        for(i=32;i<160;i++)
//        {
//            if(r<0x3FFFFFFF)
//            {
//                r+=rs2;
//                rd2++;
//                if(rd2==5) {
//                    rd2=1;
//                    rs2*=2;
//                }
//            }
//            if(r>0x3FFFFFFF)
//                r=0x3FFFFFFF;
//            RateTable[i]=r;
//        }
//    }
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

void spuFranWriteRegister(uw reg, uh data) {
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
//            s_chan[ch].ADSRX.AttackModeExp=(data&0x8000) >> 15;
//            s_chan[ch].ADSRX.AttackRate = ((data>>8) & 0x007F)^0x7F;
//            s_chan[ch].ADSRX.DecayRate = 4*(((data>>4) & 0x000F)^0x1F);
//            s_chan[ch].ADSRX.SustainLevel = (data & 0x000F) << 27;
			break;
		case 10:
//            s_chan[ch].ADSRX.SustainModeExp = (data&0x8000) >> 15;
//            s_chan[ch].ADSRX.SustainIncrease= (data&0x4000) >> 14;
//            s_chan[ch].ADSRX.SustainRate = ((data>>6) & 0x007F)^0x7F;
//            s_chan[ch].ADSRX.ReleaseModeExp = (data&0x0020) >> 5;
//            s_chan[ch].ADSRX.ReleaseRate = 4*((data & 0x001F)^0x1F);
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
	case H_SPUctrl    : spuCtrl=data; 		break;
	case H_SPUstat    : spuStat=data & 0xF800; 	break;
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
	case H_SPUirqAddr : spuIrq = data;		break;
	case H_SPUrvolL   : rvb.VolLeft=data; 		break;
	case H_SPUrvolR   : rvb.VolRight=data; 		break;
	case H_SPUon1     : SoundOn(0,16,data); 		break;
	case H_SPUon2     : SoundOn(16,24,data); 	break;
	case H_SPUoff1    : SoundOff(0,16,data); 	break;
	case H_SPUoff2    : SoundOff(16,24,data); 	break;
	case H_CDLeft     : break;
	case H_CDRight    : break;
	case H_FMod1      : FModOn(0,16,data); 		break;
	case H_FMod2      : FModOn(16,24,data); 		break;
	case H_Noise1     : NoiseOn(0,16,data); 		break;
	case H_Noise2     : NoiseOn(16,24,data); 	break;
	case H_RVBon1	  : break;
	case H_RVBon2	  : break;
	case H_Reverb+0   : rvb.FB_SRC_A=data;	  break;
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

uh spuFranReadRegister(uw reg) {
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

//static const unsigned long int TableDisp[] = {
// -0x18+0+32,-0x18+4+32,-0x18+6+32,-0x18+8+32,
// -0x18+9+32,-0x18+10+32,-0x18+11+32,-0x18+12+32,
//
// -0x1B+0+32,-0x1B+4+32,-0x1B+6+32,-0x1B+8+32,
// -0x1B+9+32,-0x1B+10+32,-0x1B+11+32,-0x1B+12+32,
//};
//
//static inline int MixADSR(SPUCHAN *ch) {
//    uw disp;
//    sw EnvelopeVol = ch->ADSRX.EnvelopeVol;
//
//    if (ch->bStop) {
//        if(ch->ADSRX.ReleaseModeExp)
//                disp = TableDisp[(EnvelopeVol>>28)&0x7];
//        else
//                disp=-0x0C+32;
//
//        EnvelopeVol-=RateTable[ch->ADSRX.ReleaseRate + disp];
//
//        if(EnvelopeVol<0)
//            {
//                EnvelopeVol=0;
//                ch->bOn=0;
//            }
//
//        ch->ADSRX.EnvelopeVol=EnvelopeVol;
//        ch->ADSRX.lVolume=(EnvelopeVol>>=21);
//        return EnvelopeVol;
//    }
//    else
//    {
//        if(ch->ADSRX.State==0)
//            {
//                disp = -0x10+32;
//                if(ch->ADSRX.AttackModeExp)
//                {
//                    if(EnvelopeVol>=0x60000000)
//                        disp = -0x18+32;
//                }
//                EnvelopeVol+=RateTable[ch->ADSRX.AttackRate+disp];
//
//                if(EnvelopeVol<0)
//                {
//                    EnvelopeVol=0x7FFFFFFF;
//                    ch->ADSRX.State=1;
//                }
//
//                ch->ADSRX.EnvelopeVol=EnvelopeVol;
//                ch->ADSRX.lVolume=(EnvelopeVol>>=21);
//                return EnvelopeVol;
//            }
//
//        if(ch->ADSRX.State==1)
//            {
//                disp = TableDisp[(EnvelopeVol>>28)&0x7];
//                EnvelopeVol-=RateTable[ch->ADSRX.DecayRate+disp];
//
//                if(EnvelopeVol<0)
//                    EnvelopeVol=0;
//                if(EnvelopeVol <= ch->ADSRX.SustainLevel)
//                    ch->ADSRX.State=2;
//
//                ch->ADSRX.EnvelopeVol=EnvelopeVol;
//                ch->ADSRX.lVolume=(EnvelopeVol>>=21);
//                return EnvelopeVol;
//            }
//
//        if(ch->ADSRX.State==2)
//            {
//                if(ch->ADSRX.SustainIncrease)
//                {
//                    disp = -0x10+32;
//                    if(ch->ADSRX.SustainModeExp)
//                    {
//                        if(EnvelopeVol>=0x60000000)
//                            disp = -0x18+32;
//                    }
//                    EnvelopeVol+=RateTable[ch->ADSRX.SustainRate+disp];
//
//                    if(EnvelopeVol<0)
//                        EnvelopeVol=0x7FFFFFFF;
//                }
//                else
//                {
//                    if(ch->ADSRX.SustainModeExp)
//                        disp = TableDisp[((EnvelopeVol>>28)&0x7)+8];
//                    else
//                        disp=-0x0F+32;
//
//                    EnvelopeVol-=RateTable[ch->ADSRX.SustainRate+disp];
//
//                    if(EnvelopeVol<0)
//                        EnvelopeVol=0;
//                }
//
//                ch->ADSRX.EnvelopeVol=EnvelopeVol;
//                ch->ADSRX.lVolume=(EnvelopeVol>>=21);
//                return EnvelopeVol;
//            }
//    }
//    return 0;
//}

static inline void StartSound(SPUCHAN * pChannel) {
	pChannel->ADSRX.lVolume=1;
	pChannel->ADSRX.State=0;
	pChannel->ADSRX.EnvelopeVol=0;
	pChannel->pCurr=pChannel->pStart;
	pChannel->s_1=0;
	pChannel->s_2=0;
	pChannel->iSBPos=28;
	pChannel->bNew=0;
	pChannel->bStop=0;
	pChannel->bOn=1;
	pChannel->SB[29]=0;
	pChannel->SB[30]=0;
	pChannel->spos=0x10000L;
	pChannel->SB[31]=0;
}

static inline void VoiceChangeFrequency(SPUCHAN * pChannel) {
	pChannel->iUsedFreq=pChannel->iActFreq;
	pChannel->sinc=pChannel->iRawPitch<<4;
	if(!pChannel->sinc) pChannel->sinc=1;
}

//inline void FModChangeFrequency(SPUCHAN * pChannel,int ns)
//{
//    int NP=pChannel->iRawPitch;
//    NP=((32768L+iFMod[ns])*NP)/32768L;
//    if(NP>0x3FFF) NP=0x3FFF;
//    if(NP<0x1)    NP=0x1;
//    NP=(44100L*NP)/(4096L);
//    pChannel->iActFreq=NP;
//    pChannel->iUsedFreq=NP;
//    pChannel->sinc=(((NP/10)<<16)/4410);
//    if(!pChannel->sinc) pChannel->sinc=1;
//    iFMod[ns]=0;
//}

inline void StoreInterpolationVal(SPUCHAN * pChannel,int fa)
{
	if(pChannel->bFMod==2)
		pChannel->SB[29]=fa;
	else
	{
		if((spuCtrl&0x4000)==0)
			fa=0;
		else
			{
				if(fa>32767L)  fa=32767L;
				if(fa<-32767L) fa=-32767L;
			}
		pChannel->SB[29]=fa;
	}
}

void PsxSpuFran::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    
    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void PsxSpuFran::fillBuffer() {
    //int size = SBUF_SIZE;
	//int sampleCount = !vs.isVideoPAL ? 44100/60 : 44100/50;
	//sampleCount = MIN(size/4, sampleCount);
	//memset(stream, 0, sampleCount*4);
    
    static const int f[5][2] = {
        {   0,   0 },
        {  60,   0 },
        { 115, -52 },
        {  98, -55 },
        { 122, -60 },
    };
    
    int s_1, s_2, fa;
    int predict_nr, shift_factor, flags, s;
    
    while(!psx.suspended) {
        for (SPUCHAN *ch = s_chan; ch != s_chan + MAXCHAN; ch++) {
            if (ch->bNew)
                StartSound(ch);
            
            if (!ch->bOn)
                continue;
            
            if (ch->iActFreq != ch->iUsedFreq)
                VoiceChangeFrequency(ch);
            
            for (int ns = 0; ns < SBUF_SIZE; ns++) {
//                if (ch->bFMod == 1 && iFMod[ns])
//                    FModChangeFrequency(ch, ns);
                
                while(ch->spos >= 0x10000) {
                    if (ch->iSBPos == 28) {
                        if (ch->pCurr == (ub *)-1) {
                            ch->bOn = 0;
                            ch->ADSRX.lVolume = 0;
                            ch->ADSRX.EnvelopeVol = 0;
                            goto ENDX;
                        }
                        
                        ch->iSBPos = 0;
                        s_1 = ch->s_1;
                        s_2 = ch->s_2;
                        predict_nr = (int)*ch->pCurr;
                        ch->pCurr++;
                        shift_factor = predict_nr & 0xf;
                        predict_nr >>= 4;
                        flags = (int)*ch->pCurr;
                        ch->pCurr++;

                        for (int i = 0; i < 28; ch->pCurr++) {
                            s = ((((int)*ch->pCurr) & 0xf) << 12);
                            if (s & 0x8000) s |= 0xFFFF0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            s = ((((int)*ch->pCurr) & 0xf0) << 8);
                            ch->SB[i++] = fa;
                            if (s & 0x8000) s |= 0xFFFF0000;
                            fa = (s >> shift_factor);
                            fa = fa + ((s_1 * f[predict_nr][0]) >> 6) + ((s_2 * f[predict_nr][1]) >> 6);
                            s_2 = s_1;
                            s_1 = fa;
                            ch->SB[i++] = fa;
                        }
                        
                        if ((flags & 4) && (!ch->bIgnoreLoop))
                            ch->pLoop = ch->pCurr - 16;
                        
                        if (flags & 1) {
                            if (flags != 3 || ch->pLoop == NULL)
                                ch->pCurr = (ub *)-1;
                            else
                                ch->pCurr = ch->pLoop;
                        }
                        ch->s_1 = s_1;
                        ch->s_2 = s_2;
                    }
                    fa = ch->SB[ch->iSBPos++];
                    StoreInterpolationVal(ch, fa);
                    ch->spos -= 0x10000;
                }

                if (ch->bNoise)
                    fa = 0;
                else
                    fa = ch->SB[29];
                
                ch->sval = fa; //(MixADSR(ch)*fa) >> 10;
                
                if (ch->bFMod == 2) {
                    iFMod[ns] = ch->sval;
                }
                else {
                    sbuf.temp[ns*2+0] += (+ch->sval * ch->iLeftVolume ) / MAX_VOLUME;
                    sbuf.temp[ns*2+1] += (-ch->sval * ch->iRightVolume) / MAX_VOLUME;
                }
                ch->spos += ch->sinc;
            }
            ENDX: ;
        }
        
        for (int i = 0; i < SBUF_SIZE; i++) {
            sbuf.fin[i*2+0] = (+(sbuf.temp[i*2+0] / 4) * 16383) / MAX_VOLUME;
            sbuf.fin[i*2+1] = (-(sbuf.temp[i*2+1] / 4) * 16383) / MAX_VOLUME;
        }
        
        // OpenAL
        ALint processed;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        
        if (processed >= ALC_BUF_AMOUNT) {
            // We have to free buffers
            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
        }
        
        while(--processed < 0) {
            stream();
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        }
        
        ALuint buffer;
        alSourceUnqueueBuffers(source, 1, &buffer);
        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf.fin, SBUF_SIZE*2*2, SAMPLE_RATE);
        alSourceQueueBuffers(source, 1, &buffer);
        stream();
        
        // Clear
        sbuf = { 0 };
    }
}

void spuFranReadDMAMem(uh *pusPSXMem, int size) {
    if (spuAddr + (size<<1) >= 0x80000) {
        memcpy(pusPSXMem, &spuMem[spuAddr>>1], 0x7FFFF-spuAddr+1);
        memcpy(pusPSXMem+(0x7FFFF-spuAddr+1), spuMem, (size<<1)-(0x7FFFF-spuAddr+1));
        spuAddr = (size<<1) - (0x7FFFF-spuAddr+1);
    } else {
        memcpy(pusPSXMem,&spuMem[spuAddr>>1], (size<<1));
        spuAddr += (size<<1);
    }
}

void spuFranWriteDMAMem(uh *pusPSXMem, int size) {
    if (spuAddr+(size<<1)>0x7FFFF) {
        memcpy(&spuMem[spuAddr>>1], pusPSXMem, 0x7FFFF-spuAddr+1);
        memcpy(spuMem, pusPSXMem+(0x7FFFF-spuAddr+1), (size<<1)-(0x7FFFF-spuAddr+1));
        spuAddr = (size<<1)-(0x7FFFF-spuAddr+1);
    } else {
        memcpy(&spuMem[spuAddr>>1], pusPSXMem, (size<<1));
        spuAddr += (size<<1);
    }
}

void executeDMA(CstrBus::castDMA *dma) {
    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
    
    switch(dma->chcr) {
        case 0x01000201:
            spuFranWriteDMAMem(p, size);
            return;
            
        case 0x01000200:
            spuFranReadDMAMem(p, size);
            return;
    }
    
    printx("/// PSeudo SPU DMA: $%x", dma->chcr);
}

#endif
