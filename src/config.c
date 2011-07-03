// CONFIG.C,H: Configuration routines

//	systime =
//		5452 (286-8)
//		17819 (386-16)
//		36392 (386-33)

#include <alloc.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "\develop\xargon\include\gr.h"
#include "\develop\xargon\include\keyboard.h"
#include "\develop\xargon\include\gamectrl.h"
#include "\develop\xargon\include\music.h"

int nosnd=0;
//int cfgdemo=0;

struct {
	int firstthru;				// Are we set up?
	int joyflag0,joyxl0, joyxc0, joyxr0, joyyu0, joyyc0, joyyd0;
	int x_ourmode0;
	int musicflag0;
	int vocflag0;
	char pad [64];
	} cf;

void cfg_init(int argc, char *argv[]) {
	int c;
	char s[16];

	clrscr();
	cputs ("\r\n\r\nDetecting your hardware...\r\n");
	cputs ("\r\nIf your system locks, reboot and type:\r\n");
	cputs ("   "); cputs ("XARGON");
		cputs (" /NOSB  (No Sound Blaster card)\r\n");
	cputs ("   "); cputs ("XARGON");
		cputs (" /SB    (With a Sound Blaster)\r\n");
	cputs ("   "); cputs ("XARGON");
		cputs (" /NOSND (If all else fails)\r\n");

	readspeed();
	for (c=0; c<argc; c++) {
		strupr (argv[c]);
		if (!strcmp (argv[c],"/TEST")) {
			ltoa (systime,s,10);
			cputs (s);
			getkey();
			}
		else if (!strcmp (argv[c],"/NOSB")) {
			vocflag=0; musicflag=0;
			}
		else if (!strcmp (argv[c],"/SB")) {
			}
		else if (!strcmp(argv[c],"/NOSND")) {
			vocflag=0; musicflag=0; nosnd=1;
			};
//		else if (!strcmp(argv[c],"/DEMO")) cfgdemo=1;
		};
	};

int doconfig (void) {
	int redo;
	char tempstr[16];

	redo=cf.firstthru;

	if (!redo) {
		x_ourmode=cf.x_ourmode0;

		joyflag=joypresent();						// Check joystick
		if (!joyflag) {cf.joyflag0=0;}
		else if (cf.joyflag0) {
			joyxl=cf.joyxl0; joyxc=cf.joyxc0; joyxr=cf.joyxr0;
			joyyu=cf.joyyu0; joyyc=cf.joyyc0; joyyd=cf.joyyd0;
			checkctrl (0);
			redo|=((dx1!=0)||(dy1!=0));
			};

		if (!musicflag) cf.musicflag0=0;			// Check music

		if (!vocflag) cf.vocflag0=0;				// Check digital sound
		};
	if (!redo) {
		clrscr();
		cputs ("\r\n");
		cputs (" Your configuration:\r\n");
		if (cf.vocflag0)
			cputs ("    Digital Sound Blaster sound effects ON\r\n");
		else cputs ("    No digitized sound effects\r\n");
		if (cf.musicflag0)
			cputs ("    Sound Blaster musical sound track ON\r\n");
		else cputs ("    No musical sound track\r\n");
		if (cf.joyflag0) cputs ("    A joystick\r\n");
		else cputs ("    No joystick\r\n");
		cputs ("    256-color VGA graphics\r\n");
		cputs ("\r\n");
		cputs ("  Press ENTER if this is correct\r\n");
		cputs ("      or press 'C' to configure: ");

		do {
			getkey(); key=toupper(key);
			} while ((key!=13)&&(key!='C')&&(key!=escape));
		if (key=='C') redo=1;
		if (key==escape) return 0;
		};
	if (redo) {
		clrscr();
		if (!(vocflag||musicflag)) {				// Music card
			cputs ("\r\n");
			cputs (" No Sound Blaster-compatible music card has been\r\n");
			cputs (" detected.\r\n\r\n");
			cputs (" Press any key to continue...");
			getkey();
			};
		if (vocflag&&(systime<4000)) {			// Digital Sound
			cputs ("\r\n\r\n");
			cputs (" A Sound Blaster card was detected, but your CPU is\r\n");
			cputs (" too slow to support digitized sound.  Digital sound\r\n");
			cputs (" is now OFF.\r\n\r\n");
			cputs (" Press any key to continue...");
			getkey();
			}
		else if (vocflag) {
			cputs (" A Sound Blaster card has been detected.\r\n\r\n");
			cputs (" This game will play high-quality digital sound\r\n");
			cputs (" through your Sound Blaster if you wish.\r\n\r\n");
			cputs (" Warning: There is a small chance this will cause\r\n");
			cputs (" problems if you have less than 640K of RAM, or\r\n");
			cputs (" if your computer is not totally compatible.\r\n\r\n");
			cputs (" Do you want digital sound? ");
			do {
				getkey();
				key=toupper(key);
//				if (key=='~') {
//					ltoa (coreleft(),tempstr,10);
//					cputs (tempstr);
//					};
				if (key==escape) return 0;
				} while ((key!='Y')&&(key!='N'));
			cf.vocflag0=(key=='Y');
			};
		if (musicflag) {								// Musical sound track
			clrscr();
			cputs ("\r\n\r\n\r\n");
			cputs (" This game features a Sound Blaster-compatible\r\n");
			cputs (" musical sound track.\r\n\r\n\r\n");
			cputs (" Do you want the musical sound track? ");
			do {
				getkey();
				key=toupper(key);
				if (key==escape) return 0;
				} while ((key!='Y')&&(key!='N'));
			cf.musicflag0=(key=='Y');
			};
		clrscr();										// Game controller
		cputs ("\r\n");
		if (!gc_config()) return 0;
		cf.joyflag0=joyflag;
		};
	if (systime<4000) {vocflag=0; cf.vocflag0=0;};
	cf.firstthru=0;
	joyflag=cf.joyflag0;
	cf.joyxl0=joyxl; cf.joyxc0=joyxc; cf.joyxr0=joyxr;
	cf.joyyu0=joyyu; cf.joyyc0=joyyc; cf.joyyd0=joyyd;
	cf.x_ourmode0=x_ourmode;
	vocflag=cf.vocflag0; musicflag=cf.musicflag0;
	return 1;
	};