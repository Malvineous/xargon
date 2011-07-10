// MUSIC.C Unified Sound Unit - Sound Blaster + PC Speaker

#include <stdlib.h>
//#include <conio.h>
//#include <dos.h>
//#include <alloc.h>
#include <string.h>
#include <fcntl.h>
//#include <io.h>
//#include <mem.h>
#include "include/config.h"
#include "include/music.h"
#include "include/worx.h"

void soundadd (int priority,char *s);
void sampadd (int instr,int len,int durfactor,int note);

void timerset (int numero,int moodi,unsigned int arvo);

#define memvocs 5
#define maxvoclen 6144
char *memvoc;						// Size = memvocs*maxvoclen

int soundoff=1;					// = 1 until set on
int soundf=1;
int makesound=0;
//int *myclock=(int*)((long)0x0040006cL);
int notepriority,samppriority,oldpri;
int soundcount;
int SetDSP=0;
int SetWORX=0;
int oldfreq;
int soundptr,soundlen;

int32_t vocposn [num_samps];
int16_t voclen [num_samps];
int16_t vocrate [num_samps];
int8_t vocnum [num_samps];

int32_t textposn [num_text];
int16_t textlen [num_text];
uint16_t vocused [num_samps];
uint16_t vocuse=0;
char *soundmac [num_macs];
char *textmsg;
int16_t textmsglen;

unsigned int clockrate,clockcount;

int *freq=NULL;
int *dur=NULL;

const int headersize=sizeof(vocposn)+sizeof(voclen)+sizeof(vocrate)+
	sizeof(textposn)+sizeof(textlen);

int vocflag=1;
int musicflag=1;
int vocfilehandle=-1;
int vocpri;
char *song = NULL;
// extern char mirrortab[num_samps];

void interrupt (*oldint8)(void)=NULL;
void interrupt (*worxint8)(void)=NULL;

void interrupt spkr_intr (void);

const char vochdr[0x20]={
	0x43,0x72,0x65,0x61,0x74,0x69,0x76,0x65,
	0x20,0x56,0x6f,0x69,0x63,0x65,0x20,0x46,
	0x69,0x6c,0x65,0x1a,0x1a,0x00,0x0a,0x01,
	0x29,0x11,0x01,0x5d,0x2d,0x00,0xaa,0x00
	};

#define numsamples 40
#define samplen 128

int *SOUNDS;

#define samples(x) (SOUNDS+x*samplen)
#define sampledur(x) (max(1,*(SOUNDS+numsamples*samplen+x)))

const int notetable[144]= {
	64   ,67   ,71   ,76   ,80   ,85   ,90   ,95   ,101  ,107  ,114  ,121   ,0    ,0    ,0    ,0,
	128  ,135  ,143  ,152  ,161  ,170  ,181  ,191  ,203  ,215  ,228  ,242   ,0    ,0    ,0    ,0,
	256  ,271  ,287  ,304  ,322  ,341  ,362  ,383  ,406  ,430  ,456  ,483   ,0    ,0    ,0    ,0,
	512  ,542  ,574  ,608  ,645  ,683  ,724  ,767  ,812  ,861  ,912  ,967   ,0    ,0    ,0    ,0,
	1024 ,1084 ,1149 ,1217 ,1290 ,1366 ,1448 ,1534 ,1625 ,1722 ,1825 ,1933  ,0    ,0    ,0    ,0,
	2048 ,2169 ,2298 ,2435 ,2580 ,2733 ,2896 ,3068 ,3250 ,3444 ,3649 ,3866  ,0    ,0    ,0    ,0,
	4096 ,4339 ,4597 ,4870 ,5160 ,5467 ,5792 ,6137 ,6501 ,6888 ,7298 ,7732  ,0    ,0    ,0    ,0,
	8192 ,8679 ,9195 ,9741 ,10321,10935,11585,12274,13003,13777,14596,15646 ,0    ,0    ,0    ,0,
	16384,17358,18390,19483,20642,21870,23170,24548,26007,27554,29192,30928 ,0    ,0    ,0    ,0
	};

extern void rexit (int);

extern void interrupt WorxBugInt8 (void);

void interrupt testintr (void) {
	spkr_intr();
	oldint8();
	}

void getvoc (int c) {
	int n,a,lasta;
	unsigned int lastused;
	nosound();
	if ((voclen[c]!=0) && (vocnum[c]==-1)) {
		// Free an old voc if necessary
		n=0;
		lasta=-1; lastused=65535;
		for (a=0; a<num_samps; a++) if (vocnum[a]!=-1) {
			n++;
			if (vocused[a]<lastused) {
				lastused=vocused[a]; lasta=a;
				};
			}
		if (n>=(memvocs-1)) {
			vocnum[c]=vocnum[lasta];
			vocnum[lasta]=-1;
			}
		else {
			vocnum[c]=n;
			};
		// Load this voc
		n=vocnum[c];
		memcpy (memvoc+n*maxvoclen,vochdr,0x20);
		*(memvoc+n*maxvoclen+0x1b)=(char)(voclen[c]);
		*(memvoc+n*maxvoclen+0x1c)=(char)((voclen[c])>>8);
		*(memvoc+n*maxvoclen+0x1e)=96;			// Speed = 256-1,000,000/rate
		lseek (vocfilehandle,vocposn[c],SEEK_SET);
		read (vocfilehandle,memvoc+n*maxvoclen+0x20,voclen[c]);
		};
	};

void snd_init (char *voclib) {
	int i;
	clockrate=0; clockcount=0;
	textmsg=NULL;

	for (i=0; i<num_samps; i++) {
		vocposn [i]=-1;
		voclen [i]=0;
		vocrate [i]=0;
		vocnum [i]=-1;
		vocused [i]=0;
		};
	for (i=0; i<num_macs; i++) soundmac [i]=NULL;
	StartWorx();
	worxint8=getvect(8);
	setvect (8,WorxBugInt8);
	if (musicflag) musicflag=AdlibDetect();
	if (!musicflag) vocflag=0;
	if (voclib[0]==0) {vocflag=0; return;}
	vocfilehandle=_open (voclib,O_BINARY|O_RDONLY);
	if (vocfilehandle==-1) {vocflag=0; return;}
	read (vocfilehandle,&vocposn,sizeof (vocposn));
	read (vocfilehandle,&voclen, sizeof (voclen ));
	read (vocfilehandle,&vocrate,sizeof (vocrate));
	read (vocfilehandle,&textposn,sizeof(textposn));
	read (vocfilehandle,&textlen,sizeof (textlen));
	};

void snd_play (int priority, int num) {
	if (vocflag&&soundf) {
		if ((!VOCPlaying())||(priority>=oldpri)) {
//	if (mirrortab[num]) num=mirrortab[num];
			getvoc (num);
			if (vocnum[num]!=-1) {
				PlayVOCBlock (memvoc+vocnum[num]*maxvoclen,127);
				vocused[num]=vocuse; vocuse++;
				};
			oldpri=priority;
			}
		}
		else if ((num<num_macs)&&(soundmac[num]!=NULL)&&
			(freq!=NULL)&&(dur!=NULL)) soundadd (priority,soundmac[num]);
	};

void snd_do (void) {
	uint16_t j;
	int c;
	int soundhandle;
	nosound();

	if (nosnd||musicflag||vocflag) clockrate=0;
	else if (!vocflag) clockrate=64;

	if (musicflag) {
		SetFMVolume (0xf,0xf);
		};
	if (vocflag) {
		SetDSP=(DSPReset()!=0);
		vocflag=SetDSP;
		if (vocflag==0) soundoff=1;
		else {
			SetMasterVolume (0xf,0xf);
			};
		};
	if (vocflag) memvoc=malloc (maxvoclen*memvocs);
	else {
		memvoc=NULL;
		freq=malloc (maxsndlen*2+128);
		dur=malloc (maxsndlen*2+128);
		lseek (vocfilehandle,headersize,SEEK_SET);
		for (c=0; c<num_macs; c++) {
			read (vocfilehandle,&j,2);
			if (j!=0) {
				soundmac[c]=malloc(j);
				if (!soundmac[c]) rexit (154);
				read (vocfilehandle,soundmac[c],j);
				}
			else soundmac[c]=NULL;
			};
		SOUNDS=malloc (10480);
		soundhandle=_open ("audio.epc",O_BINARY|O_RDONLY);
		if (soundhandle==-1) rexit (155);
		_read (soundhandle,SOUNDS,10400);
		close (soundhandle);
		};

	if (clockrate==0) {
		clockrate=1;
		soundoff=1;
		}
	else if (clockrate>1) {
		soundoff=0;
		oldint8 = getvect (8);
		setvect (8,spkr_intr);
		timerset (0,2,65536L/clockrate);
		};
	};

void text_get (int n) {
	textmsg=NULL;

	if (textlen[n]!=0) {
		textmsglen=textlen[n];
		textmsg=malloc(textmsglen);
		if (textmsg!=NULL) {
			lseek (vocfilehandle,textposn[n],SEEK_SET);
			if (read (vocfilehandle,textmsg,textmsglen)==-1) textmsg=NULL;
			};
		};
	};

void snd_exit (void) {
	int c;
	timerset (0,2,0);
	nosound();

	if (freq!=NULL) free (freq);
	if (dur!=NULL) free (dur);

	for (c=0; c<num_macs; c++) if (soundmac[c]!=NULL) free (soundmac[c]);
	if (memvocs!=NULL) free (memvoc);
	if (vocfilehandle>=0) close (vocfilehandle);
	if (oldint8!=NULL) setvect (8,oldint8);
	if (SetDSP) DSPClose();
	setvect (8,worxint8);
	CloseWorx();
	};

// void sb_update (void) {};			// WORX handles looping. Not needed.

int sb_playing (void) {
	return (1);
	};

void sb_shutup (void) {
	if (musicflag) {
		StopSequence();
//		free(song); // PORT
		song=NULL;
		};
	};

void sb_playtune (char *filename) {
	if (!musicflag) return;
	sb_shutup();
	song=GetSequence(filename);
	if (song!=NULL) {
		SetLoopMode (1);
		PlayCMFBlock(song);
		};
	};

void sampadd1 (int instr, int len, int durfactor, int note) {
	int c;
	long tempfreq,sampfreq,playfreq;
	int *s;

	// printf (" %i-%i",instr,sampledur(instr));
	s=samples(instr);
	if (soundoff) return;
	c=0;
	playfreq=notetable[note+16];
	makesound=1;
	do {
		sampfreq=s[c++];
		if (sampfreq==-1L) freq[soundlen]=-1;
		else {
			tempfreq=(sampfreq*playfreq)>>10;		// Notetable[48]=2>>10
			freq[soundlen]=tempfreq;
			}
		dur [soundlen++]=durfactor;
		} while ((c<len)&&(soundlen<maxsndlen));
	};

void soundadd1 (int priority, char *s) {
	int c;
	int note;
	int instr=-1;
	int notedur;
	int restdur;

	if (soundoff) {return;};
	if ((!makesound)||((priority>=notepriority)&&(notepriority!=-1))||
		(priority==-1)) {
		if ((priority>=0)||(!makesound)) {
			makesound=0;
			soundptr=0;
			soundlen=0;
			soundcount=0;
			};
		notepriority=priority;
		c=0;
		do {
			if (s[c]=='\xf0') {
				c++;
				instr=s[c];
				c++;
				};
			note=s[c++];
			notedur=s[c++];

			if (instr==-1) {
				freq [soundlen]=notetable [note];
				dur [soundlen++]=notedur*clockrate;
				makesound=1;
				}
			else {
				restdur=(notedur*clockrate) - (samplen*sampledur(instr));
				if (restdur>0) {
					sampadd (instr,samplen,sampledur(instr),note);
					freq[soundlen ]=-1;
					dur [soundlen++]=restdur;
					}
				else sampadd (instr,(notedur*clockrate)/(sampledur(instr)),
					(sampledur(instr)),note);
				};
			} while ((s[c]!=0)&&(soundlen<maxsndlen));
		};
	};

void soundstop (void) {
	makesound=0;
	nosound();
	};

void timerset (int numero, int moodi, unsigned int arvo) {
	outportb (0x43,64*numero+48+2*moodi+0);
	outportb (0x40+numero, arvo);
	outportb (0x40+numero, arvo>>8);
	};

void sampadd (int instr, int len, int durfactor, int note) {
	int *s;
	int c;
	long tempfreq,sampfreq,playfreq;

	s=samples(instr);
	if (soundoff) {return;};
	c=0;
	playfreq=notetable[note+16];
	makesound=1;
	do {
		sampfreq=s[c++];
		if (sampfreq==-1L) freq[soundlen]=-1;
		else {
			tempfreq=(sampfreq*playfreq)>>10;			// Notetable[48]=2>>10
			freq[soundlen]=tempfreq;
			}
		dur [soundlen++]=durfactor;
		} while ((c<len)&&(soundlen<maxsndlen));
	};

void soundadd2 (int priority, char *s) {
	int c;
	int note;
	int instr=-1;
	int notedur;
	int restdur;

	if (soundoff) {return;};
	if ((!makesound)||((priority>=notepriority)&&(notepriority!=-1))||
		(priority==-1)) {
		if ((priority>=0)||(!makesound)) {
			makesound=0;
			soundptr=0;
			soundlen=0;
			soundcount=0;
			};
		notepriority=priority;
		c=0;
		do {
			if (s[c]=='\xf0') {
				c++;
				instr=s[c++];
				};
			note=s[c++];
			notedur=s[c++];

			if (instr==-1) {
				freq [soundlen]=notetable [note];
				dur [soundlen++]=notedur*clockrate;
				makesound=1;
				}
			else {
				restdur=notedur*clockrate - samplen*sampledur(instr);
				if (restdur>0) {
					sampadd (instr,samplen,sampledur(instr),note);
					freq[soundlen ]=-1;
					dur [soundlen++]=restdur;
					}
				else sampadd (instr,(notedur*clockrate)/sampledur(instr),
					sampledur(instr),note);
				};
			} while ((s[c]!=0)&&(soundlen<maxsndlen));
		};
	};

void soundadd (int priority, char *s) {
	int c;
	int note;
	int instr=-1;
	int notedur;
	int restdur;

	if (soundoff) {return;};
	if ((!makesound)||((priority>=notepriority)&&(notepriority!=-1))||
		(priority==-1)) {
		if ((priority>=0)||(!makesound)) {
			makesound=0;
			soundptr=0;
			soundlen=0;
			soundcount=0;
			};
		notepriority=priority;
		c=0;
		do {
			if (s[c]=='\xf0') {
				c++;
				instr=s[c++];
				};
			note=s[c++];
			notedur=s[c++];

			if (instr==-1) {
				freq [soundlen]=notetable [note];
				dur [soundlen++]=notedur*clockrate;
				makesound=1;
				}
			else {						// There's a bug here...
				restdur=notedur*clockrate - samplen*sampledur(instr);
				if (restdur>0) {
					sampadd (instr, samplen,sampledur(instr),note);
					freq[soundlen ]=-1;
					dur [soundlen++]=restdur;
					}
				else sampadd (instr,restdur/sampledur(instr),
					sampledur(instr),note);
				};
			} while ((s[c]!=0)&&(soundlen<maxsndlen));
		};
	};
