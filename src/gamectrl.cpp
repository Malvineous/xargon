// GAMECTRL.C

#include "port.h"
//#include <dos.h>;
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
//#include <mem.h>;
//#include <conio.h>;
#include <stdlib.h>
//#include <io.h>;
#include "include/keyboard.h"

extern int gamecount;
extern int debug,swrite;

long systime=0;
//extern char *myclock;
extern long longclock;

int dx1, dy1, fire1, fire2, fire1off, fire2off;
int joyflag;
int key;
int dx1hold, dy1hold, flow1;
int dx1old, dy1old;
int joyxsense, joyysense;

int macplay, macrecord, macabort, macaborted, mactime;
char *macptr=NULL;
char macfname[32];
unsigned int macofs,maclen;

void recmac (void);
void getmac (void);

// local

int joyxl, joyxc, joyxr, joyyu, joyyc, joyyd;
char keybuf [256];

int buttona1 (void) {
	return ((inportb(0x201) & 0x10)==0);
	};

int buttona2 (void) {
	return ((inportb(0x201) & 0x20)==0);
	};

void readspeed (void) {
/*
	int oldclock;
	systime=0;
	oldclock=*myclock;
	do {} while (*myclock==oldclock);
	do {systime++;} while ((*myclock-oldclock)<5);
// Now 5 intervals have passed
	systime/=4L;
*/
	systime = 4096; // dummy
	};

void readjoy (int *x, int *y) {				// may want to delay...
	int n;

	disable();
	*x=0; *y=0; n=0;
	outportb (0x201,0);
	do {
		*x+=(inportb(0x201)&1); *y+=(inportb(0x201)&2); n++;
		} while (!(((inportb(0x201)&3)==0)|(n<0)));
	enable();
	*y/=2;
	if (n<0) {
		*x=-1; *y=-1;
		};
	};

int caldir (char *s, int *jx, int *jy) {
	int result=0;
	int key=0;
	cputs (s);
	do {
		readjoy (jx,jy);
		if (k_pressed()) key=k_read();
		} while ((key!=escape)&&(!buttona1()));
	delay (25);
	if (key!=escape) {
		result=1;
		do {
			if (k_pressed()) key=k_read();
		} while ((buttona1())&&(key!=escape));
	};
	delay (25);
	cputs ("\r\n");
	return (result);
	};

int joypresent (void) {
	int x,y;
	readjoy (&x,&y);
	if ((x>0)&&(y>0)) {
		joyxsense=x; joyysense=y;
		return (1);
		}; return (0);
	};

int calibratejoy (void) {
	int key;
	redo:
	joyflag=0;
	cputs ("\r\nJoystick calibration:  Press ESCAPE to abort.\r\n");
	if (caldir("  Center joystick and press button: ",&joyxc,&joyyc)&&
		caldir ("  Move joystick to UPPER LEFT corner and press button: ",
			&joyxl,&joyyu)&&
		caldir ("  Move joystick to LOWER RIGHT corner and press button: ",
			&joyxr,&joyyd)) {
		joyxl-=joyxc; joyxr-=joyxc; joyyu-=joyyc; joyyd-=joyyc;
		if ((joyxl<-1)&&(joyxr>1)&&(joyyu<-1)&&(joyyd>1)) return (1);
		else {
			cputs ("  Calibration failed - try again (y/N)? ");
			do {} while (!k_pressed());
			cputs ("\r\n");
			key=k_read();
			if (toupper (key)=='Y') goto redo;
			};
		}; return (0);
	};

void checkctrl (int pollflag) {
	int x1, y1, xs, ys;

	if (macplay) {
		getmac(); return;
		};

	SDL_Event event;

	dx1=0; dy1=0;
	fire1=0; flow1=0;
	reloop:
	key=0;
	//if (k_pressed()) {
	if (SDL_PollEvent(&event)) {
		//key=k_read();
		//if ((key==0)|(key==1)|(key==2)) key=k_read();
		int newkey, newscan = 0;
		switch (event.type) {
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_LEFT:     newkey = k_left;    newscan = scan_cursorleft; break;
					case SDLK_RIGHT:    newkey = k_right;   newscan = scan_cursorright; break;
					case SDLK_UP:       newkey = k_up;      newscan = scan_cursorup; break;
					case SDLK_DOWN:     newkey = k_down;    newscan = scan_cursordown; break;
					case SDLK_PAGEUP:   newkey = k_pgdown;  break;
					case SDLK_PAGEDOWN: newkey = k_pgup;    break;
					case SDLK_F1:       newkey = k_f1;      newscan = scan_f1; break;
					case SDLK_F7:       newkey = k_f7;      break;
					case SDLK_SPACE:    newkey = ' ';       newscan = scan_space; break;
					case SDLK_ESCAPE:   newkey = escape;    newscan = scan_esc; break;
					case SDLK_RSHIFT:   newscan = scan_rshift; break;
					case SDLK_LSHIFT:   newscan = scan_lshift; break;
					case SDLK_RCTRL:
					case SDLK_LCTRL:    newscan = scan_ctrl; break;
					case SDLK_RALT:
					case SDLK_LALT:     newscan = scan_alt; break;
					default: newkey = event.key.keysym.sym; break;
				}
				break;
		}
		switch (event.type) {
			case SDL_KEYUP:
				keydown[0][newscan] = 0;
				break;
			case SDL_KEYDOWN:
				keydown[0][newscan] = 1;
				key = newkey;
				break;
		}
	};
	if (key!=0) {
		switch (key) {
			case k_up:
			case '8':
				if (pollflag) {goto reloop;};
				dx1=0; dy1=-1; break;
			case k_left:
			case '4':
				if (pollflag) {goto reloop;};
				dx1=-1; dy1=0; break;
			case k_right:
			case '6':
				if (pollflag) {goto reloop;};
				dx1=1; dy1=0; break;
			case k_down:
			case '2':
				if (pollflag) {goto reloop;};
				dx1=0; dy1=1; break;
			};
		};
	k_status();
	fire1=k_shift;
	fire2=k_alt|k_ctrl;
	if (((dx1==0)&&(dy1==0))&&(joyflag)) {
		readjoy (&x1,&y1);
		xs=(x1-joyxc);
		ys=(y1-joyyc);
		dx1=((2*xs)>joyxr)-((2*xs)<joyxl);
		dy1=((2*ys)>joyyd)-((2*ys)<joyyu);
		if (buttona1()) {fire1=1;};
		if (buttona2()) {fire2=1;};
		};
	if (((dx1==0)&&(dy1==0))&&(pollflag)) {		// keydown[1]
	if (keydown[0][scan_cursorleft ]||keydown[1][scan_cursorleft ]) {dx1--;};
	if (keydown[0][scan_cursorright]||keydown[1][scan_cursorright]) {dx1++;};
	if (keydown[0][scan_cursorup   ]||keydown[1][scan_cursorup   ]) {dy1--;};
	if (keydown[0][scan_cursordown ]||keydown[1][scan_cursordown ]) {dy1++;};
	};

	if (fire1) {fire1^=fire1off;}
	else {fire1off=0;};
	if (fire2) {fire2^=fire2off;}
	else {fire2off=0;};
	dx1old=dx1; dy1old=dy1;
	if (macrecord) recmac();
	};

void checkctrl0 (int pollflag) {
	static int oldclock=0;
	do {} while (oldclock==getclock());
	oldclock=getclock();
	checkctrl (pollflag);
	};

void sensectrlmode (void) {
	joyflag=joypresent();
	};

int gc_config (void) {
	int key=' ';
	if (joypresent()) {
		cputs ("\r\nGame controller:  K)eyboard,  J)oystick?  ");
		do {
			do {} while (!k_pressed());
			key=toupper (k_read());
			} while ((key!='K')&&(key!='J')&&(key!=escape));
		cputs ("\r\n");
		joyflag=0;
		switch (key) {
			case 'J': joyflag=calibratejoy(); break;
			};
		};	return (key!=escape);
	};

void getkey (void) {
	do {
		checkctrl(0);
		} while (key==0);
	};

void stopmac (void) {
	macplay=0;
	macrecord=0;
	if (macptr!=NULL) {free (macptr); macptr=NULL;}
	macofs=0;
	mactime=1;
	srand (12345);		// Aha, seed random generator so seq's repeat!
	};

void playmac (char *fname) {
	int machand;
	stopmac();
	macaborted=0;
	machand=_open(fname,O_BINARY);
	if (machand>=0) {
		maclen=filelength (machand);
		macptr=malloc (maclen);
		if (macptr==NULL) macptr=NULL;
		else if (_read (machand,macptr,maclen)>=0) {
			macplay=1;
			gamecount=0;
			}
		else {
			free (macptr); macptr=NULL;
			};
		_close (machand);
		};
	};

void recordmac (char *fname) {
	stopmac();
	macptr=malloc (8000);
	if (macptr!=NULL) {
		macofs=0;
		macrecord=1;
		strcpy (macfname,fname);
		gamecount=0;
		};
	};

void macrecend (void) {
	int machand;

	if (!macrecord) return;
	machand=_creat (macfname,0);
	if (machand>=0) {
		_write (machand,macptr,macofs);
		_close (machand);
		};
	stopmac();
	};

void recmac (void) {							// Record a key event
	static int curdx1=0, curdy1=0, curfire1=0, curfire2=0, oldclock=0;
	int dt;
	char bits;

	if (key=='[') {mactime=0; key=0;}
	if (key==']') {mactime=1; key=0;}
	if (key=='}') {macrecend(); return;}
	if (macofs==0) {
		curdx1=0; curdy1=0; curfire1=0; curfire2=0; oldclock=gamecount;
		};
	bits=((curdx1!=dx1)<<0)|((curdy1!=dy1)<<1)|((curfire1!=fire1)<<2)|
		((curfire2!=fire2)<<3)|(((key>0)&&(key<=127))<<4);
	if (bits) {
		if (macofs!=0) {
			if (mactime==0) dt=1;
			else dt=(gamecount-oldclock);
			if (dt<128) *(macptr+macofs++)=(char) dt;
			else {
				*(macptr+macofs++)=(char) ((dt&127)|128);
				*(macptr+macofs++)=(char) (dt>>7);
				};
			};
		*(macptr+macofs++)=bits;
		if (bits&1) *(macptr+macofs++)=(char) dx1;
		if (bits&2) *(macptr+macofs++)=(char) dy1;
		if (bits&4) *(macptr+macofs++)=(char) fire1;
		if (bits&8) *(macptr+macofs++)=(char) fire2;
		if (bits&16) *(macptr+macofs++)=(char) key;
		curdx1=dx1;	curdy1=dy1;	curfire1=fire1; curfire2=fire2;
		};
	if (macofs>=30000) macrecend();
	};

void getmac (void) {
	static int oldclock, nextdt;
	int tempkey;
	char bits;

	if (k_pressed()) {
		tempkey=k_read();
		if ((macabort==0)||((macabort==1)&&(tempkey==27))) {
			stopmac();
			macaborted=1;
			};
		};
	key=0;
	if (macofs==0) {
		dx1=0; dy1=0; fire1=0; fire2=0;
		oldclock=gamecount;
		nextdt=0;
		};
	if ((gamecount-oldclock)>=(nextdt)) {
		bits=*(macptr+macofs++);
		if (bits&1) dx1=(int) (char) *(macptr+macofs++);
		if (bits&2) dy1=(int) (char) *(macptr+macofs++);
		if (bits&4) fire1=(int) (char) *(macptr+macofs++);
		if (bits&8) fire2=(int) (char) *(macptr+macofs++);
		if (bits&16) key=(char) *(macptr+macofs++);
		nextdt=*(macptr+macofs++);
		if (nextdt<0) {
			nextdt=(nextdt&127)+(((int)(char) *(macptr+macofs++))<<7);
			};
		};
	if (macofs>=maclen) stopmac();
	};

void gc_init (void) {
	dx1=0; dy1=0; fire1=0; fire1off=0;
	dx1old=0; dy1old=0;
	dx1hold=0;dy1hold=0;
	keybuf[0]=0;
	macplay=0; macrecord=0; macabort=1;
	joyflag=0;
	installhandler(1);
	};

void gc_exit (void) {
	removehandler();
	};
