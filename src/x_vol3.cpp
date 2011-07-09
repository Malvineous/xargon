// X_VOL3.C
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

char pgmname[]="XARGON_3";
char xshafile[]="graphics.xr3";
char xvocfile[]="audio.xr3";
char cfgfname[]="config.xr3";
char ext[]=".xr3";
char tilefile[]="tiles.xr3";
char xintrosong[]="song_0.xr3";

char *demoboard[1]={"intro"};
char demolvl[1]={100};
char *demoname[1]={"demo_mac.xr0"};

char v_msg[]="VOLUME THREE --- Xargon's Fury";

char *fidgetmsg[4]={"What are you waiting for?","Xargon is watching you!","Don't give up Malvineous","You'll never defeat Xargon!"};

char *leveltxt[16]={
	"Malvineous returns\r\rto the safety of\r\rthe map level\r",
	"Malvineous finds\r\ra door to an\r\runderground tunnel\r",
	"Malvineous confronts\r\rmore of the\r\rgiant trolls\r",
	"Malvineous explores\r\ran unusual\r\rtribal village\r",
	"Malvineous takes on\r\rthe\r\rmaze of madness\r",
	"Malvineous journeys\r\rinto another\r\rerupting volcano\r",
	"Malvineous gets\r\rcaught in a\r\rviolent storm\r",
	"Malvineous can\r\rsee an\r\runderwater cove\r",
	"Malvineous searches\r\rfor\r\rprecious jewels\r",
	"Malvineous ventures\r\rinto the\r\rruins of Xarg\r",
	"Malvineous is\r\rtrapped in the\r\rLabyrinth level\r",
	"Malvineous slowly\r\renters the\r\rland of leeches\r",
	"Malvineous awakes\r\rin the\r\rdungeon of death\r",
	"Malvineous sneaks\r\rinto the\r\rdismal looking castle\r",
	"Malvineous forces\r\rhis way into\r\rXargon's eerie castle\r",
	"Malvineous enters\r\rthe evil\r\rXargbot factory\r"
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
	wprint (&mainvp,208,8,1,"XARGON'S FURY");
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
