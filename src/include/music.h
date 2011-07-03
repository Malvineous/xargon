//		MUSIC.H Generalized sound driver - header
//		Copyright 1991 Epic MegaGames, Written by Tim Sweeney

#define num_samps 50
#define num_text  40
#define num_macs  128
#define maxsndlen 4096

extern long vocposn [num_samps];
extern int voclen [num_samps];
extern int vocrate [num_samps];
extern char *soundmac [num_macs];
extern long textposn [num_text];
extern int textlen [num_text];

extern int *freq;
extern int *dur;

extern int vocflag;
extern int musicflag;
extern char *textmsg;
extern int textmsglen;

extern unsigned int xmusicrate;
extern unsigned int xclockrate;
extern unsigned int xclockval;

extern char *xvocptr;
extern int xvoclen;

extern int soundf;
extern int *myclock;
void snd_init (char *newvoclib);
void snd_play (int priority,int num);
void snd_do (void);
void snd_exit (void);
void text_get (int n);

void music_init (void);
void sb_playtune (char *fname);
void sb_shutup (void);
void sb_update (void);
int sb_playing (void);