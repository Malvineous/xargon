// X_VOL2.C
//
// Xargon: Volume-specific Stuff
//
// by Allen W. Pilgrim

#include <stdlib.h>
#include "\develop\xargon\include\gr.h"
#include "\develop\xargon\include\keyboard.h"
#include "\develop\xargon\include\windows.h"
#include "\develop\xargon\include\gamectrl.h"
#include "\develop\xargon\include\x_obj.h"
#include "\develop\xargon\include\xargon.h"
#include "\develop\xargon\include\cfg_win.h"

int c_len=CFG_WIN_LENGTH;
unsigned char CFG_WIN[];

char pgmname[]="XARGON_2";
char xshafile[]="graphics.xr2";
char xvocfile[]="audio.xr2";
char cfgfname[]="config.xr2";
char ext[]=".xr2";
char tilefile[]="tiles.xr2";
char xintrosong[]="song_0.xr2";

char *demoboard[1]={"intro"};
char demolvl[1]={100};
char *demoname[1]={"demo_mac.xr0"};

char v_msg[]="VOLUME TWO --- The Secret Chamber";

char *fidgetmsg[4]={"Apogee never made games this fun","I can't imagine anyone being bored playing this","Don't just stand there looking stupid","Come on Malvineous get moving"};

char *leveltxt[16]={
	"Malvineous returns\r\rto the safety of\r\rthe map level\r",
	"Malvineous continues\r\rhis quest to find\r\rXargon's Castle\r",
	"Malvineous cautiously\r\renters the\r\rmassive volcano\r",
	"Malvineous explores\r\rmore of this\r\rstrange land\r",
	"Malvineous finds\r\rthe cave of the\r\rdeadly xbat\r",
	"Malvineous is surrounded\r\rby strange\r\ralien creatures\r",
	"Malvineous sloshes\r\rinto the\r\rswamps of despair\r",
	"Malvineous begins\r\rto sweat in\r\rthe lava pits\r",
	"Malvineous gets\r\rlost in the\r\rsurreal forest\r",
	"Malvineous journeys\r\rbeneath\r\rthe earth\r",
	"Malvineous approaches\r\rthe\r\rgrunt's hideout\r",
	"Malvineous dives\r\rinto the\r\rdangerous waters\r",
	"Malvineous discovers\r\rsome\r\rancient ruins\r",
	"Malvineous enters\r\ra\r\rmysterious maze\r",
	"Malvineous storms\r\rinto the\r\rhidden fortress\r",
	"Malvineous creeps\r\rinto the\r\rSecret Chamber\r"
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
	wprint (&mainvp,168,8,1,"THE SECRET CHAMBER");
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