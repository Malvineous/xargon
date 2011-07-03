// X_VOL1.C
//
// Xargon: Volume-specific Stuff
//
// by Allen W. Pilgrim

#include <stdlib.h>
#include "include/gr.h"
#include "include/keyboard.h"
#include "include/windows.h"
#include "include/gamectrl.h"
#include "include/x_obj.h"
#include "include/xargon.h"
#include "include/cfg_win.h"

int c_len=CFG_WIN_LENGTH;
extern unsigned char CFG_WIN[];

char xshafile[]="graphics.xr1";
char xvocfile[]="audio.xr1";
char cfgfname[]="config.xr1";
char ext[]=".xr1";
char tilefile[]="tiles.xr1";
char xintrosong[]="song_0.xr1";

char *demoboard[1]={"intro"};
char demolvl[1]={100};
char *demoname[1]={"demo_mac.xr0"};

char v_msg[]="VOLUME ONE --- Beyond Reality";

char *fidgetmsg[4]={"Wow! These EPIC games are great","The graphics in this game are incredible","Have you tried KILOBLASTER yet?","I can't wait to get volumes 2 and 3!"};

char *leveltxt[16]={
	"Malvineous returns\r\rto the safety of\r\rthe map level\r",
	"Malvineous begins\r\rhis journey in\r\rthis bizarre world\r",
	"Malvineous enters\r\ra strange\r\rnew dimension\r",
	"Malvineous enters\r\rthe forest of\r\renchanted trees\r",
	"Malvineous discovers\r\ra robot\r\rdevelopment plant\r",
	"Malvineous falls\r\rinto the\r\rcorridors of doom\r",
	"Malvineous ventures\r\rinto the\r\rcave of the ancients\r",
	"Malvineous gets lost\r\rin the mysterious\r\rcorkscrew cave\r",
	"Malvineous plunges\r\rinto the chilly waters\r\rof the coral grave\r",
	"Malvineous faces\r\rcertain death in the\r\rtunnels of terror\r",
	"Malvineous stumbles\r\rinto Xargon's\r\rsecret factory\r",
	"",
	"",
	"",
	"",
	""
	};

void wait (void) {
	int x, y;
	int c,q;

	clrpal (); setpagemode (1);
	for (x=0; x<20; x++) {
		for (y=0; y<12; y++) {
			drawshape (&mainvp,0x7501+x+y*20,x*16,y*16);
			};
		};
	fontcolor (&mainvp,71,-1);
	wprint (&mainvp,200,8,1,"BEYOND REALITY");
//	fontcolor (&mainvp,7,-1);
//	wprint (&mainvp,79,194,2,"-Press Any Key to Continue-");
	pageflip (); setpagemode (0);	fadein ();
	do {
		gamecount++; checkctrl0(0);
		for (c=64; c<80; c++) {
			q=64+((c+gamecount)&15);
			setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
			};
		} while ((key==0)&&(fire1==0)&&(fire2==0)&&(dx1==0)&&(dy1==0));
	};
