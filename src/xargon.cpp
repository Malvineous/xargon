// Xargon Main File

extern unsigned _stklen=8192;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>
#include <ctype.h>
#include <alloc.h>
#include "include/gr.h"
#include "include/keyboard.h"
#include "include/windows.h"
#include "include/gamectrl.h"
#include "include/uncrunch.h"
#include "include/config.h"
#include "include/music.h"
#include "include/x_obj.h"
#include "include/xargon.h"
#include "include/x_snd.h"

wintype menu_win;
vptype gamevp, statvp, textvp, tempvp;
int scrnxs, scrnys;
int bd [boardxs][boardys];
int scrollxd, scrollyd, oldscrollxd, oldscrollyd, oldx0, oldy0;

int stateinfo [numstates];
infotype info[numinfotypes];
int (*kindmsg[numobjkinds])(int n, int msg, int z);
int kindxl[numobjkinds];
int kindyl[numobjkinds];
char *kindname[numobjkinds];
int kindflags[numobjkinds];
int kindtable[numobjkinds];
int kindscore[numobjkinds];

objtype objs [maxobjs+2];
int numobjs, numscrnobjs;
int scrnobjs [maxscrnobjs];
int gameover, gamecount, statmodflg;	// gameover 0=no 1=yes-dead 2=yes-won
char cnt_fruit, old_fruit, icon_fruit;
pltype pl,o_pl;

int designflag=0;
int debug=0;
int granny=0;
int levelmsgclock;
int botcol,bottime,text_flg;

char tempname [16];
char botmsg [50];
char newlevel [16];
char curlevel [16];
char oursong [16];

char hiname [hilen][numhighs];
char savename [numsaves][savelen];
unsigned long hiscore [numhighs];

extern char xshafile[],xvocfile[],cfgfname[],ext[],xintrosong[];
extern char v_msg[];
extern int c_len;
extern unsigned char CFG_WIN[];

extern char *demoboard[1];
extern char demolvl[1];
extern char *demoname[1];
int demonum;

void design (void);
void dodemo (void);
void play (int demoflg);
//char mirrortab[num_samps]=mirrortabd;
void pageview (int pgnum);
void rexit(int num);
void rexit2(int n);

void init_colors (void) {
	int c;

	setcolor (250,0,0,0); setcolor (251,0,0,0);
	for (c=207; c<234; c++) {				// reset hero colors
		setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
		};
	for (c=162; c<168; c++) {				// reset laser colors
		setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
		};
	};

void upd_colors (void) {
	int c,q;

	for (c=192; c<200; c++) {						// flowing lava
		q=192+((c+gamecount)&7);
		setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
		};
	for (c=240; c<248; c++) {						// waterfalls
		q=240+((c+gamecount)&7);
		setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
		};
	for (c=234; c<236; c++) {
		q=234+(((c+gamecount)&3)/2);
		setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
		};
	for (c=236; c<240; c++) {						// underwater pod
		q=236+(((c+gamecount)&7)/2);
		setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
		};
	};

void init_win (void) {
	gamevp.vpox=0; gamevp.vpoy=2;
	gamevp.vpx=0; gamevp.vpy=2;
	gamevp.vpxl=320; gamevp.vpyl=158;
//	gamevp.vpxl=320; gamevp.vpyl=160;

	statvp.vpox=0; statvp.vpoy=0;
	statvp.vpx=0; statvp.vpy=160;
	statvp.vpxl=320; statvp.vpyl=28;

	textvp.vpox=0; textvp.vpoy=0;
	textvp.vpx=12; textvp.vpy=190;
	textvp.vpxl=296; textvp.vpyl=8;					// 46 letters max
	};

void statwin (int flg) {
	int x;
	int num=0;
	char i_msg[]="THE XARGON TRILOGY";
	clrvp (&mainvp,248);
	clrvp (&statvp,121);
	if (flg) {								// status window during game
		for (x=0; x<289; x+=32) {
			drawshape (&statvp,0x1e00+num,x,0);
			num++;
			};
		drawshape (&statvp,kindtable[obj_emerald]*256+28,206,13);
		}
	else {									// bottom display during opening screen
		num=0;
		for (x=0; x<289; x+=32) {
			drawshape (&statvp,0x2100+num,x,0);
			num++;
			};
		};

	drawshape (&mainvp,0x1e0a,0,188);
	for (x=32; x<257; x+=32) {
		drawshape (&mainvp,0x1e0b,x,188);
		};
	drawshape (&mainvp,0x1e0c,288,188);
	if (!flg) {
		fontcolor (&textvp,6,121);
		clearvp (&textvp);
		wprint (&textvp,148-strlen(i_msg)*3,1,2,i_msg);
		};
	};

void init_brd (void) {
	int x,y;

	for (x=0;x<boardxs;x++) {
		for (y=0;y<boardys;y++) {
			setboard(x,y,0);
			};
		};
	gamevp.vpox=0; gamevp.vpoy=2;
	};

void txt (char *msg, int col, int flg) {
	strcpy (botmsg,msg);
	botcol=col;
	if (!flg) bottime=100;
	text_flg=flg;
	statmodflg|=mod_screen;
	};

void upd_botmsg (void) {
	if (bottime>0) bottime--;
	if ((bottime==0)&&(text_flg==0)) txt (v_msg,4,1);
	};

void drawstats(void) {
	int c;
	int d=0;
	char tempstr[32];

	fontcolor (&statvp,6,121);
	wprint (&statvp,12,14,2,"       ");
	ultoa (pl.score,tempstr,10);
	wprint (&statvp,54-(1+strlen(tempstr)*6),14,2,tempstr);

	if ((cnt_fruit>=16)&&(pl.health<5)) {
		cnt_fruit=0; pl.health++;
		snd_play (3,snd_getheart);
		if (!invcount(inv_invin)) pl.ouched=-4;
		};
	for (c=0; c<5; c++) {
		if (c<=pl.health-1) drawshape (&statvp,0x1e0d,c*8+69,12);
		else drawshape (&statvp,0x1e0e,c*8+69,12);
		};

	fontcolor (&statvp,5,121);						// emeralds
	wprint (&statvp,229,14,1,"  ");
	itoa (pl.emeralds,tempstr,10);
	wprint (&statvp,245-(strlen(tempstr)*8),14,1,tempstr);

	if (debug) {
		fontcolor (&statvp,3,121);
		wprint (&statvp,12,14,2,"       ");
		ultoa (coreleft(),tempstr,10);
		wprint (&statvp,54-(1+strlen(tempstr)*6),14,2,tempstr);
		};

	for (c=0; c<4; c++) {
		drawshape (&statvp,0x1e1b,122+(c*12),13);
		};
	for (c=0; c<pl.numinv; c++) {							// key icons 8x10
		if (pl.inv[c]>12) {
			drawshape (&statvp,0x1e00+inv_shape[pl.inv[c]],122+(d*12),13);	d++;
			};
		};

	drawshape (&statvp,0x1e4c,170,13);					// blank space
	drawshape (&statvp,0x1e48+icon_fruit,170,13);	// fruit icon
	itoa (cnt_fruit,tempstr,10);
	fontcolor (&statvp,3,121);
	wprint (&statvp,185,15,2,"  ");
	wprint (&statvp,197-(1+strlen(tempstr)*6),15,2,tempstr);

	if (botcol==3) fontcolor (&textvp,7-pagedraw*botcol,121);
	else fontcolor (&textvp,botcol,121);
	clearvp (&textvp);
	wprint (&textvp,148-strlen(botmsg)*3,1,2,botmsg);
	if ((pl.ouched==1)||(pl.ouched==-1)) {
		pl.ouched=0;
		setcolor (0,0,0,0);
		for (c=207; c<234; c++) {				// change hero colors to normal
			setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
			};
		};
	};

void zapobjs (void) {
	int c;
	for (c=0; c<numobjs; c++) {
		if (objs[c].inside!=NULL) free(objs[c].inside);
		};
	init_objs();
	};

void loadcfg (void) {
	int c, cfgfile;

	cfgfile=_open (cfgfname,O_BINARY);
	if ((cfgfile<0)||(filelength(cfgfile)<=0)) {
		for (c=0; c<numhighs; c++) {hiname[c][0]='\0'; hiscore[c]=0;};
		for (c=0; c<numsaves; c++) {savename[c][0]='\0';};
		cf.firstthru=1;
		}
	else {
		read (cfgfile,&hiname,sizeof (hiname));
		read (cfgfile,&hiscore,sizeof(hiscore));
		read (cfgfile,&savename,sizeof(savename));
		if (read (cfgfile,&cf,sizeof(cf))<0) cf.firstthru=1;
		};
	close (cfgfile);
	};

void savecfg (void) {
	int cfgfile;

	cfgfile=_creat (cfgfname,0);
	if (cfgfile>=0) {
		write (cfgfile,&hiname,sizeof (hiname));
		write (cfgfile,&hiscore,sizeof(hiscore));
		write (cfgfile,&savename,sizeof(savename));
		write (cfgfile,&cf,sizeof(cf));
		};
	close (cfgfile);
	};

void loadboard (char *fname) {
	int boardfile;
	char dest[16];
	int c,tempint;
	int x,y;

	// purging all shapes from memory to prevent memory fragmentation
	for (c=0; c<64; c++) {shm_want[c]=0;};
	shm_do();

	shm_want [1]=1;							// load shapes needed
	shm_want [2]=1;
	shm_want [3]=1;
	shm_want [4]=1;
	shm_want [6]=1;
	shm_want [30]=1;

	strcpy (dest,fname);
	if (strcmp(dest,tempname)!=0) strcat (dest,ext);

	strcpy (curlevel,fname);
	zapobjs ();
	for (c=162; c<168; c++) {					// reset laser colors
		setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
		};

	boardfile=_open (dest,O_BINARY);
	if (!read (boardfile,&bd,sizeof(bd))) rexit(1);
	if (!read (boardfile,&numobjs,sizeof(numobjs))) rexit(2);
	if (!read (boardfile,&objs,numobjs*sizeof(objs[0]))) rexit(3);
	if (!read (boardfile,&pl,sizeof(pl))) rexit(4);
	if (!read (boardfile,&cnt_fruit,sizeof(cnt_fruit))) rexit(5);
	for (c=0; c<numobjs; c++) {
		if (objs[c].inside!=NULL) {
			read (boardfile,&tempint,sizeof(tempint));
			objs[c].inside=malloc(tempint+1);
			read (boardfile,objs[c].inside,tempint+1);
			};
		};
	_close (boardfile);

	for (x=0; x<boardxs; x++) {
		for (y=0; y<boardys; y++) {
			shm_want[(info[board(x,y)].sh>>8)&0x3f]=1;
			};
		};
	for (c=0; c<numobjs; c++) {
		shm_want [kindtable[objs[c].objkind]]=1;
		};
	shm_do();
	};

void saveboard (char *fname) {
	int boardfile;
	char dest[16];
	int c,tempint;

	strcpy (dest,fname);
	if (strcmp(dest,tempname)!=0) strcat (dest,ext);

	boardfile=_creat (dest,0);				// Was O_BINARY
	if (boardfile<0) rexit(20);
	if (write (boardfile,&bd,sizeof(bd))<0) rexit(5);
	write (boardfile,&numobjs,sizeof(numobjs));
	write (boardfile,&objs,numobjs*sizeof(objs[0]));
	write (boardfile,&pl,sizeof(pl));
	write (boardfile,&cnt_fruit,sizeof(cnt_fruit));
	for (c=0; c<numobjs; c++) {
		if (objs[c].inside!=NULL) {
			tempint=strlen (objs[c].inside);
			write (boardfile,&tempint,sizeof(tempint));
			write (boardfile,objs[c].inside,tempint+1);
			};
		};
	_close (boardfile);
	};

int loadsavewin (char *msg, char *blankmsg) {
	int c;
	int moveclock=0;
	int cur=0;
	char s[12]; char tempstr[2];

	dx1hold=1; dy1hold=1; fire1off=1;
	defwin (&menu_win,13,16,6,7,0,0,textbox);			// 96x112
	drawwin (&menu_win);
	fontcolor (&menu_win.inside,7,-1);
	wprint (&menu_win.inside,12,4,1,msg);
	fontcolor (&menu_win.inside,4,-1);
	wprint (&menu_win.inside,0,8,1,"____________");
	wprint (&menu_win.inside,0,84,1,"____________");
	fontcolor (&menu_win.inside,3,-1);
	for (c=0; c<numsaves; c++)
		wprint (&menu_win.inside,16,20+c*10,2,itoa(c+1,s,10));
	fontcolor (&menu_win.inside,6,-1);
	for (c=0; c<numsaves; c++) {
		if (strlen(savename[c])!=0)
			wprint (&menu_win.inside,28,20+c*10,2,savename[c]);
		else wprint (&menu_win.inside,28,20+c*10,2,blankmsg);
			};
	fontcolor (&menu_win.inside,3,-1);
	wprint (&menu_win.inside,6,100,2,"<ESC>");
	wprint (&menu_win.inside,42,100,2,"TO ABORT");
	fontcolor (&menu_win.inside,8,1);
	for (c=0; c<11; c++)	wprint (&menu_win.inside,4,20+c*6,2," ");

	do {
		tempstr[1]=0;
		checkctrl0(0);
		key=toupper(key);
		c=(c&7)+1; tempstr[0]=c;
		wprint (&menu_win.inside,4,20+cur*10,2,tempstr);
		delay (100);
		wprint (&menu_win.inside,4,20+cur*10,2," ");
		if (((dx1+dy1)!=0)&&(abs((*myclock)-moveclock)>1)) {
			moveclock=(*myclock);
			cur+=dx1+dy1;
			if ((cur>=0)&&(cur<(numsaves))) snd_play (4,snd_jump);
			cur=(max(0,min(cur,numsaves-1)));
			};
		} while ((!fire1)&&(key!=enter)&&(key!=escape));
	if (key==escape) {snd_play (4,snd_masher); return (-1);};
	snd_play (4,snd_masher); return (cur);
	};

int loadgame (void) {
	int num, hand;
	char tempstr[16];
	char boardname[16];

	num=loadsavewin("LOAD GAME","<empty>");
	if ((num>=0)&&(strlen(savename[num])!=0)) {
		itoa (num,tempstr,10);
		strcpy (boardname,"save_");
		strcat (boardname,tempstr);
		loadboard (boardname);
		return (1);
		};
	return (0);
	};

void savegame (void) {
	int num,hand;
	char s[savelen];
	char tempstr[16];
	char boardname[16];

	num=loadsavewin("SAVE GAME","");
	if (num>=0) {
		strcpy (s,savename[num]);
		fontcolor (&menu_win.inside,7,1);
		wprint (&menu_win.inside,28,20+num*10,2,"         ");
		winput (&menu_win.inside,28,20+num*10,2,s,9);
		if ((key!=escape)&&(strlen(s)!=0)) {
			strcpy (savename[num],s);
			itoa (num,tempstr,10);
			strcpy (boardname,"save_");
			strcat (boardname,tempstr);
			saveboard (boardname);
			savecfg();
			};
		};
	};

void stats (int num) {
	if (num) {
		memcpy (&o_pl,&pl,sizeof(pltype));
		old_fruit=cnt_fruit;
		}
	else {
		memcpy (&pl,&o_pl,sizeof(pltype));
		cnt_fruit=old_fruit;
		while (invcount(inv_epic)) takeinv(inv_epic);
		while (invcount(inv_laser)>1) takeinv(inv_laser);
		while (invcount(inv_rock)) takeinv(inv_rock);
		while (invcount(inv_jump)) takeinv(inv_jump);
		while (invcount(inv_invin)) takeinv(inv_invin);
		while (invcount(inv_key0)) takeinv(inv_key0);
		while (invcount(inv_key1)) takeinv(inv_key1);
		while (invcount(inv_key2)) takeinv(inv_key2);
		while (invcount(inv_key3)) takeinv(inv_key3);
		};
	};

void moddrawboard (void);

int numlines (void) {
	int a=0;
	int c;

	for (c=0; c<textmsglen; c++) a+=(*(textmsg+c))==13;
	return (a);
	};

int getline (int n, char *line, int dospace) {
	int ourcolor=7;
	int c=0;
	int a=0;
	char ch;

	while ((a<n)&&(c<textmsglen)) a+=(*(textmsg+c++))==13;
	while ((*(textmsg+c)<32)&&(*(textmsg+c)!=13)) c++;
	if ((*(textmsg+c)>='0')&&(*(textmsg+c)<='7')) {
		ourcolor=*(textmsg+c++)-'0';
		};
	a=0;
	while (((ch=*(textmsg+c))!=13)&&(c<textmsglen)&&(a<77)) {
		if (dospace) line[a++]=toupper (ch);
		else line[a++]=ch;
		c++;
		};
	line[a]=0;
	return (ourcolor);
	};

void printline (vptype *ourvp, int y, int n, int flg) {
	char line[80];

	fontcolor (ourvp,getline (n,line,1),((flg)?121:129));			//121
//	fontcolor (ourvp,getline (n,line,1),((flg)?17:129));			//121
	wprint (ourvp,0,y,2,"                                    ");
	wprint (ourvp,(ourvp->vpxl-6*strlen(line))/2,y,2,line);
	};

void ourdelay (void) {
	int c=*myclock;
	do {} while ((*myclock-c)<0);
	};

void rest (void) {
	int done=0;
	key=0; fire1off=1; fire2off=1;

	do {checkctrl0(0);}
		while ((key!=0)||(fire1)||(fire2)||(dx1!=0)||(dy1!=0));
	do {
		checkctrl0(0);
		if ((key==escape)||(key==enter)) done=1;
		else if ((key==' ')||(fire1)||(fire2)||(dx1)||(dy1)) done=1;
		} while (!done); key=0; fire1off=1; fire2off=1;
	};

void dotextmsg (int n, int flg) {
	wintype texwin;
	int c,linecount,y,y0,textys;
	char line[80];

	dx1hold=1; dy1hold=1;

	snd_play (5,snd_menu);
	text_get (n);
	if (textmsg!=NULL) {
		setpagemode(1);
		defwin (&texwin,6,18,13,6,0,0,flg);
		drawwin (&texwin);
		fontcolor (&texwin.inside,7,((flg==dialog)?121:129));
//		fontcolor (&texwin.inside,7,((flg==dialog)?17:129));
		clearvp (&texwin.inside);
		fontcolor (&texwin.border,getline (0,line,0),-1);
		titlewin (&texwin,line,0);
		fontcolor (&texwin.inside,7,0);

		textys=texwin.inside.vpyl/6;
		linecount=numlines();
		if (linecount<=textys) {
			y=((texwin.inside.vpyl)-6*(linecount-1))/2;
			for (c=1; c<linecount; c++) {
				printline (&texwin.inside,y,c,((flg==dialog)?1:0)); y+=6;
				};
			pageflip(); ourdelay();
			do {checkctrl0(1);}
				while ((dx1!=0)||(dy1!=0)||(key!=0)||(fire1)||(fire2));
			do {checkctrl0(1);} while ((key!=' ')&&(key!=enter)&&(!fire1));
			}
		else {
			y0=0; y=0;
			for (c=1; c<=textys; c++) {
				printline (&texwin.inside,y,c,((flg==dialog)?1:0)); y+=6;
				};
			pageflip(); setpagemode (0);
			fire1off=1;
			do checkctrl0(1); while ((dx1!=0)||(dy1!=0)||(key!=0));
			ourdelay();
			do {
				checkctrl0(0);
				dx1+=(key==k_pgdown)-(key==k_pgup);
				if (((dx1+dy1)<0)&&(y0>0)) {
					y0--;
					scrollvp (&texwin.inside,0,6);
					printline (&texwin.inside,0,y0+1,((flg==dialog)?1:0));
					}
				else if (((dx1+dy1)>0)&&((y0+textys)<linecount)) {
					y0++;
					scrollvp (&texwin.inside,0,-6);
					printline (&texwin.inside,6*(textys-1),y0+textys,
						((flg==dialog)?1:0));
					};
				} while ((key!=' ')&&(key!=enter)&&(key!=escape)&&(!fire1));
			setpagemode(1);
			};
		moddrawboard();
		free (textmsg);
		key=0;
		};
	};

void putlevelmsg (int n) {
	wintype levelwin;
	char line[80];
	int c;
	int linecount,y;
	defwin (&levelwin,5,36,14,4,4,0,0);
	levelwin.topleft.vpy=levelwin.inside.vpy;
	levelwin.topleft.vpyl=levelwin.inside.vpyl;

	levelmsgclock=*myclock;
	if (n>=32) return;
	textmsg=leveltxt[n];
	textmsglen=strlen (textmsg);
	if (textmsg!=NULL) {
		setpagemode(1);
		drawwin (&levelwin);
		fontcolor (&levelwin.inside,7,129);
		clearvp (&levelwin.inside);
		for (c=207; c<234; c++) {					// reset hero colors
			setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
			};
		for (c=0; c<16; c++) {						// draws face
			drawshape (&levelwin.topleft,0x4000+4*256+c+20,16*(c&3),16*(c/4));
			};
		linecount=numlines();
		y=((levelwin.inside.vpyl)-6*(linecount-1))/2;

		for (c=0; c<linecount; c++) {
			fontcolor (&levelwin.inside,getline (c,line,0),129);
			wprint (&levelwin.inside,
				(levelwin.inside.vpxl-6*strlen(line))/2,y,2,line);
			y+=6;
			};
		pageflip();
		moddrawboard();
		};
	};

void dialogmsg (int n, int flg) {
	wintype dialogwin;
	int c,linecount,y;
	char line[80];

	dx1hold=1; dy1hold=1;
	snd_play (5,snd_menu);

	text_get (n);							// 25 letters long, 9 lines
	if (textmsg!=NULL) {
		setpagemode(1);
		if (flg==0) defwin (&dialogwin,2,16,14,4,4,0,dialog);
		else defwin (&dialogwin,8,52,14,4,4,0,dialog);
		dialogwin.topleft.vpy=dialogwin.inside.vpy;
		dialogwin.topleft.vpyl=dialogwin.inside.vpyl;
		drawwin (&dialogwin);
		fontcolor (&dialogwin.inside,7,129);
		clearvp (&dialogwin.inside);
		fontcolor (&dialogwin.border,getline (0,line,0),-1);
		titlewin (&dialogwin,line,1);
		fontcolor (&dialogwin.inside,7,0);
		for (c=207; c<234; c++) {					// reset hero colors
			setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
			};
		if (flg==0) {
			for (c=0; c<16; c++) {					// draws hero face
				drawshape (&dialogwin.topleft,0x4000+4*256+c+20,
					16*(c&3),16*(c/4));
				};
			}
		else {
			for (c=0; c<16; c++) {					// draws eagle face
				drawshape (&dialogwin.topleft,0x4000+7*256+c+11,
					16*(c&3),16*(c/4));
				};
			};

		linecount=numlines();
		y=((dialogwin.inside.vpyl)-6*(linecount-1))/2;

		for (c=1; c<linecount; c++) {
			printline (&dialogwin.inside,y,c,0); y+=6;
			};
		pageflip(); ourdelay();
		do {checkctrl0(1);}
			while ((dx1!=0)||(dy1!=0)||(key!=0)||(fire1)||(fire2));
		do {checkctrl0(1);} while ((key!=' ')&&(key!=enter)&&(!fire1));
		moddrawboard();
		free (textmsg);
		key=0;
		};
	};

void donelevelmsg (void) {
	int dt;
	int done=0;

	do checkctrl0(0); while (key!=0);
	do {
		checkctrl0(0);
		dt=((*myclock)-levelmsgclock)/10;
		if ((key==escape)||(key==enter)) done=1;
		else if ((dt>=2)&&(key||fire1)) done=1;
		else if (dt>=4) done=1;
		} while (!done);
	};

int askquit (void) {
	wintype quitwin;

	defwin (&quitwin,10,64,9,1,0,0,textbox);
	drawwin (&quitwin);
	drawshape (&quitwin.inside,0x210b,10,3);
	do {checkctrl0(0);}
		while ((key==0)&&(fire1==0)&&(fire2==0)&&(dx1==0)&&(dy1==0));
	key=toupper(key); return (key);
	};

void drawcell (int x, int y) {
	int boardcell;

	if ((x>=0)&&(x<boardxs)&&(y>=0)&&(y<boardys)) {
		boardcell=board(x,y);
		if (!(info [boardcell].flags&f_msgdraw))
			drawshape (&gamevp, info [boardcell].sh, x<<4,y<<4);
		else msg_block(x,y,msg_draw);					// it needs to set modflg?
		};
	};

void drawboard (void) {
	int x,y;

	for (x=0; x<boardxs; x++)
		for (y=0; y<boardys; y++)
			modboard(x,y);
	upd_objs (0);
	statmodflg=0;
	refresh (0);
	};

void moddrawboard (void) {
	int x,y;

	for (x=0; x<boardxs; x++)
		for (y=0; y<boardys; y++)
			modboard(x,y);
	statmodflg|=mod_screen;
	};

void printhi (int newhi) {
	int c,d,posn;
	char s[10];
	wintype hiwin;

	int back=(newhi?1:-1);
	defwin (&hiwin,11,24,8,6,0,0,textbox);	// 128x96
	drawwin (&hiwin);

	if (newhi) {
		posn=numhighs;
		while ((posn>0)&&(pl.score>hiscore[posn-1])) posn--;
		if (posn>=numhighs) newhi=0;
		else {
			for (c=numhighs-2; c>=posn; c--) {
				hiscore[c+1]=hiscore[c];
				strcpy (hiname[c+1],hiname[c]);
				};
			hiscore[posn]=pl.score;
			hiname [posn][0]='\0';
			};
		};

	fontcolor (&hiwin.inside,7,-1);
	wprint (&hiwin.inside,18,4,2,"- HIGH SCORES -");
	fontcolor (&hiwin.inside,4,-1);
	wprint (&hiwin.inside,15,8,2,"________________");

	fontcolor (&hiwin.inside,3,-1);
	for (c=0; c<numhighs; c++) wprint (&hiwin.inside,8,20+c*7,2,hiname[c]);
	fontcolor (&hiwin.inside,2,-1);
	for (c=0; c<numhighs; c++) {
		wprint (&hiwin.inside,120-(strlen(s)*6),20+c*7,2,
			ultoa (hiscore[c],s,10));
		};

	if (newhi) {
		ultoa (hiscore[posn],s,10);
		fontcolor (&hiwin.inside,7,back);
		wprint (&hiwin.inside,8,20+posn*7,2,"        ");
		winput (&hiwin.inside,8,20+posn*7,2,hiname[posn],8);
		savecfg ();
		};
	if (newhi==0) rest ();
	};

int domenu (char *menutext, char *keytab, int y0, int num, int demoflag,
	int textx0, int winx8, int winxl16, int winy, int menuflag) {
	wintype menuwin;
	int count=0;
	int democlock,gotkey,c,moveclock=0;
	int cur;
	int oldcur=1;
	char line [80];
	gamecount=0;

	textmsg=menutext;
	textmsglen=strlen(textmsg);
	defwin (&menuwin,winx8,winy,winxl16,numlines()/2+1,0,0,textbox);
	drawwin (&menuwin);
	if (menuflag==1) drawshape (&menuwin.inside,0x210a,22,4);
	else if (menuflag==2) {
		fontcolor (&menuwin.inside,granny?6:4,-1);
		wprint (&menuwin.inside,118+(cur>=y0)*textx0,70,2,granny?"ON":"OFF");
		fontcolor (&menuwin.inside,soundf?6:4,-1);
		wprint (&menuwin.inside,82+(cur>=y0)*textx0,78,2,soundf?"ON":"OFF");
		}
	setpagemode(1);
	democlock=*myclock;
	for (cur=numlines()-1; cur>=0; cur--) {
		fontcolor (&menuwin.inside,getline (cur,line,0),-1);
		if (menuflag==1) {
			wprint (&menuwin.inside,4+(cur>=y0)*textx0,13+cur*8,2,line);
			}
		else wprint (&menuwin.inside,4+(cur>=y0)*textx0,6+cur*8,2,line);
		};
	pageflip(); setpagemode(0);
	cur=0;
	fontcolor (&menuwin.inside,8,-1);
	do {
		upd_colors (); gamecount++;
		if (++count>=12) count=0;
		if ((count&1)||(oldcur!=cur)) {
			if (menuflag==1)
				drawshape (&menuwin.inside,0x4413,(textx0-12)&0xfff8,
					12+(y0+oldcur)*8);
			else drawshape (&menuwin.inside,0x4413,(textx0-12)&0xfff8,
				5+(y0+oldcur)*8);
			if (menuflag==1) drawshape (&menuwin.inside,0x0201+(count/2),
				(textx0-12)&0xfff8,13+(y0+cur)*8);
			else drawshape (&menuwin.inside,0x0201+(count/2),
				(textx0-12)&0xfff8,6+(y0+cur)*8);
			};
		oldcur=cur;
		checkctrl0(0);
		key=toupper(key);
		if (((dx1+dy1)!=0)&&(abs((*myclock)-moveclock)>1)) {
			moveclock=(*myclock);
			cur+=dx1+dy1;
			if ((cur>=0)&&(cur<(num))) snd_play (4,snd_jump);
			cur=min (num-1,max (0,cur));
			democlock=(*myclock);
			};

		if ((((*myclock)-democlock)>400)&&demoflag) {
			key='D';
			return (key);
			};

		gotkey=0;
		if ((key==escape)&&demoflag) key='E';
		else if (key==escape) key='R';
		if (key==187) key='I';
		if ((key==enter)||(key==' ')||(fire1)) {key=keytab[cur]; gotkey=1;}
		else {
			for (c=0;c<strlen(keytab);c++) {
				if (key==keytab[c]) gotkey=1;
				};
			};
		} while (!gotkey);
	snd_play (4,snd_masher); return (key);
	};

void mainmenu (void) {
	int done=0;
	char menu1[]="7\r3PLAY NEW GAME\r3LOAD GAME\r3INSTRUCTIONS\r3THE STORY\r3DEMO-PREVIEW\r3HIGH SCORES\r3CREDITS\r3ORDERING INFO\r7EXIT\r\r";

	loadboard ("intro");	statwin(0);
	snd_play (5,snd_menu);
	do {
		setpagemode(1);
		setorigin();
		init_colors();
		drawboard();
		pageflip(); setpagemode(0);
		domenu (menu1,"PLISDHCOE",1,9,1,24,11,8,24,1);

		setpagemode(0);
		if (key=='P') {
			setpagemode(1); fadeout();
			loadboard ("map");
			init_inv();	statwin(1); drawstats(); setorigin();
			pl.level=127;
			p_reenter(0); drawboard();
			pageflip(); setpagemode(0); fadein();
			play(0);
			sb_playtune (xintrosong);
			loadboard ("intro");	statwin(0);	snd_play (5,snd_menu);
			}
		else if (key=='L') {
			if (loadgame()) {
				setpagemode(1); fadeout();
				statwin(1); drawstats(); setorigin();
				drawboard();
				pageflip(); setpagemode(0); fadein();
				play(0);
				sb_playtune (xintrosong);
				loadboard ("intro");	statwin(0);	snd_play (5,snd_menu);
				};
			}
		else if (key=='I') dotextmsg(1,0);
		else if (key=='S') {
			loadboard ("story");
			pageview (0);
			loadboard ("intro"); statwin(0);	snd_play (5,snd_menu);
			}
		else if (key=='D') {						// Demo
			dodemo();
			sb_playtune (xintrosong);
			loadboard ("intro"); statwin(0);	snd_play (5,snd_menu);
			}
		else if (key=='H') printhi(0);
		else if (key=='C') {pageview (20); statwin(0); snd_play (5,snd_menu);}
		else if (key=='O') {pageview (15); statwin(0); snd_play (5,snd_menu);}
		else if (key=='E') {
			pageflip (); setpagemode (0);
			askquit ();
			setpagemode (1); moddrawboard ();
			if (key=='Y') done=1;
			};
		} while (!done); remove (tempname);
	};

void buymenu (void) {
	int done=0;
	char menu3[]="7  What would you like to buy?\r3RETURN TO GAME\r2A. HEALTH UNIT 15 EMERALDS\r2B. EXTRA LASER 10 EMERALDS\r2C. RAPID FIRE  25 EMERALDS\r2D. 1 FIREBALL   5 EMERALDS\r2E. 5 FIREBALLS 20 EMERALDS\r2F. INVINCIBLE  30 EMERALDS\r\r";
	snd_play (5,snd_menu);

	do {
		setpagemode(1);
		drawstats();
		drawboard();
		pageflip(); setpagemode(0);
		domenu (menu3,"RABCDEF\x05",1,7,0,24,7,12,32,0);
		setpagemode(0);

		switch (key) {
			case 'R': done=1; break;		// RETURN TO GAME
			case 'A':							// Health unit
				if (pl.emeralds>=15) {
					if (pl.health<5) {
						if (!invcount(inv_invin)) pl.ouched=-4; 
						pl.health++; pl.emeralds-=15; statmodflg|=mod_screen;
						};
					}
				else dotextmsg (7,0); break;
			case 'B':							// Extra laser
				if (objs[0].objkind!=obj_player) {}
				else if (pl.emeralds>=10) {
					if (invcount(inv_laser)<5) {
						addinv (inv_laser);
						setcolor (162,23,13,33); setcolor (163,29,19,39);
						setcolor (164,35,25,45); setcolor (165,42,32,51);
						setcolor (166,49,40,57); setcolor (167,57,50,63);
						pl.emeralds-=10; statmodflg|=mod_screen;
						};
					}
				else dotextmsg (7,0); break;
			case 'C':							// Rapid fire
				if (objs[0].objkind!=obj_player) {}
				else if (pl.emeralds>=25) {
					while (invcount(inv_laser)<5) addinv (inv_laser);
					setcolor (162,32,0,0); setcolor (163,38,0,0);
					setcolor (164,44,0,0); setcolor (165,50,0,0);
					setcolor (166,56,0,0); setcolor (167,63,0,0);
					pl.emeralds-=25; statmodflg|=mod_screen;
					}
				else dotextmsg (7,0); break;
			case 'D':							// One fireball
				if (objs[0].objkind!=obj_player) {}
				else if (pl.emeralds>=5) {
					if (invcount(inv_fire)<9) {
						addinv (inv_fire);
						pl.emeralds-=5; statmodflg|=mod_screen;
						};
					}
				else dotextmsg (7,0); break;
			case 'E':							// Five fireballs
				if (objs[0].objkind!=obj_player) {}
				else if (pl.emeralds>=20) {
					if (invcount(inv_fire)<5) {
						addinv (inv_fire); addinv (inv_fire);
						addinv (inv_fire); addinv (inv_fire); addinv (inv_fire);
						pl.emeralds-=20; statmodflg|=mod_screen;
						};
					}
				else dotextmsg (7,0); break;
			case 'F':							// Invincibility
				if (pl.emeralds>=30) {
					if (!invcount(inv_invin)) {
						addinv (inv_invin);
						pl.emeralds-=30; statmodflg|=mod_screen;
						};
					}
				else dotextmsg (7,0);
			};
		} while (!done);
	setpagemode(1);
	moddrawboard();
	};

void inv_win (void) {
	int sh=kindtable[obj_icons]*256;
	wintype invwin;
	char tempstr[16];
	int c;

	defwin (&invwin,5,16,14,7,0,0,textbox);		// 224 pixels wide inside
	drawwin (&invwin);
	snd_play (5,snd_menu);
	fontcolor (&invwin.inside,7,-1);
	wprint (&invwin.inside,20,4,1,"INVENTORY STATUS WINDOW");
	wprint (&invwin.inside,20,12,1,"-----------------------");

	drawshape (&invwin.inside,0x1e26,5,26);
	fontcolor (&invwin.inside,6,-1);
	wprint (&invwin.inside,24,25,2,itoa(invcount(inv_laser),tempstr,10));
	fontcolor (&invwin.inside,4,-1);
	wprint (&invwin.inside,34,25,2,"LASER BULLETS");

	drawshape (&invwin.inside,0x251c,132,20);
	fontcolor (&invwin.inside,6,-1);
	wprint (&invwin.inside,152,25,2,itoa(invcount(inv_fire),tempstr,10));
	fontcolor (&invwin.inside,4,-1);
	wprint (&invwin.inside,162,25,2,"FIREBALLS");

	drawshape (&invwin.inside,0x1e14,4,40);
	fontcolor (&invwin.inside,6,-1);
	wprint (&invwin.inside,24,45,2,itoa(invcount(inv_rock),tempstr,10));
	fontcolor (&invwin.inside,4,-1);
	wprint (&invwin.inside,34,45,2,"ROCKS to throw");

	drawshape (&invwin.inside,0x1e10,132,40);
	fontcolor (&invwin.inside,6,-1);
	wprint (&invwin.inside,152,45,2,itoa(invcount(inv_mapkey),tempstr,10));
	fontcolor (&invwin.inside,4,-1);
	wprint (&invwin.inside,162,45,2,"GATE KEYS");

	fontcolor (&invwin.inside,7,-1);
	wprint (&invwin.inside,60,60,1,"POWER OBJECTS");
	fontcolor (&invwin.inside,7,1);
	for (c=0; c<3; c++) wprint (&invwin.inside,68+(c*35),72,2,"   ");
	for (c=0; c<3; c++) wprint (&invwin.inside,68+(c*35),78,2,"   ");
	for (c=0; c<3; c++) wprint (&invwin.inside,68+(c*35),84,2,"   ");
	fontcolor (&invwin.inside,7,0);					// shadow
	for (c=0; c<3; c++) wprint (&invwin.inside,86+(c*35),78,2," ");
	for (c=0; c<3; c++) wprint (&invwin.inside,86+(c*35),84,2," ");
	for (c=0; c<3; c++) wprint (&invwin.inside,74+(c*35),90,2,"   ");
	if (invcount(inv_icon1)) drawshape (&invwin.inside,sh+31,68,72);
	if (invcount(inv_icon2)) drawshape (&invwin.inside,sh+32,103,72);
	if (invcount(inv_icon3)) drawshape (&invwin.inside,sh+33,138,72);

	drawshape (&invwin.inside,0x1e40,26,61);
	drawshape (&invwin.inside,0x1e40,174,61);

	fontcolor (&invwin.inside,7,-1);
	wprint (&invwin.inside,69,100,1,"#1");
	wprint (&invwin.inside,104,100,1,"#2");
	wprint (&invwin.inside,139,100,1,"#3");
	rest (); setpagemode(1); moddrawboard();
	};

void gamemenu (void) {
	int done=0;
	int c,temppage;
	char menu2[]="7    - GAME OPTIONS -    \r3RETURN TO GAME\r2QUIT GAME\r2SAVE GAME\r2LOAD GAME\r2INSTRUCTIONS\r2ADDITIONAL INVENTORY\r2BUY EXTRA ITEMS\r2GRANNY MODE is    \r2NOISE is    \r\r";
	snd_play (5,snd_menu);

	do {
		setpagemode(1);
		drawboard();
		pageflip(); setpagemode(0);
		domenu (menu2,"RQSLIABGN\x05",1,9,0,24,9,10,24,2);
		setpagemode(0);

		if (key=='R') done=1;									// RETURN TO GAME
		else if (key=='Q') {gameover=1; done=1;}			// QUIT GAME
		else if (key=='S') {										// SAVE
			c=findcheckpt(0);
			if (objs[c].state==5) {
				temppage=pagedraw; pagedraw=pageshow; setpages();
				savegame();
				drawstats();
				pagedraw=temppage; setpages();
				moddrawboard();
				done=1;
				}
			else {
				dotextmsg (4,0); done=1;
				};
			}
		else if (key=='L') {										// LOAD
			if (loadgame()) {
				init_colors();	setpagemode(1); fadeout();
				statwin(1); drawstats(); setorigin(); done=1;
				drawboard(); dolevelsong();
				pageflip(); setpagemode(0); fadein();
				}
			else done=1;
			}                                   
		else if (key=='I') {dotextmsg(1,0); done=1;}
		else if (key=='A') {										// INVENTORY
			setpagemode(0); inv_win(); done=1;
			}
		else if (key=='B') {										// BUY ITEMS
			c=findcheckpt(0);
			if (objs[c].state!=5) {
				 setpagemode(0); buymenu();
				 }; done=1;
			}
		else if (key=='G') {granny=!granny; done=1;}		// GRANNY MODE
		else if (key=='N') {soundf=!soundf; done=1;}		// NOISE
		} while (!done);
	setpagemode(1);
	moddrawboard();
	};

void play (int demoflg) {
	int c,d,begclock,temppage;
	int o_col=0;
	int cheatchar=0;
	int cheatcount=0;
	char tempstr[16];
	gamecount=0; gameover=0;

	newlevel[0]='\0';
	setpagemode (1);
	dolevelsong();
	enable();

	do {
//		enable();
		if (newlevel[0]!='\0') {
			txt (v_msg,4,1);
			if (newlevel[0]=='*') {
				memmove (newlevel,newlevel+1,strlen(newlevel));
				strcpy (oursong,newlevel);
				sb_playtune (newlevel);
				newlevel[0]='\0';
				}
			else if (newlevel[0]=='#') {
				memmove (newlevel,newlevel+1,strlen(newlevel));
				strcpy (oursong,newlevel);
				if (!sb_playing()) sb_playtune (newlevel);
				else if ((pl.level>0)&&(pl.level<33)) sb_playtune (newlevel);
				newlevel[0]='\0';
				}
			else if (newlevel[0]=='&') {
				memmove (newlevel,newlevel+1,strlen(newlevel));
				macabort=2;
				playmac (newlevel);
				strcpy (oursong,"song_33.xr1");
				sb_playtune ("song_33.xr1");
//				strcpy (oursong,newlevel);
//				if (!sb_playing()) sb_playtune (newlevel);
				newlevel[0]='\0';
				}
			else if (newlevel[0]=='!') {
				putlevelmsg(0);
				stats(1);
				loadboard (tempname);
				stats(0);
				remove (tempname);
				newlevel[0]='\0';
				p_reenter(0);
				c=findcheckpt(pl.level); killobj (c);
				o_col=1;
				donelevelmsg();
				}
			else {
				putlevelmsg (pl.level);
				saveboard (tempname);
				stats(1);
				loadboard (newlevel);
				newlevel[0]='\0';
				stats(0);
				p_reenter(0);
				donelevelmsg();
				};
			};

		begclock=*myclock;
		gamecount++;
		checkctrl(1);

		if (key!=0) {
			key=toupper(key);
			if (key==cheatchar) cheatcount++;
			else {cheatcount=1; cheatchar=key;};

			if ((cheatchar==k_f7)&&(cheatcount==3)) {
				dotextmsg(5,0);
				cheatchar=0; pl.health=5;
				if (!invcount(inv_key0)) addinv (inv_key0);
				if (!invcount(inv_key1)) addinv (inv_key1);
				if (!invcount(inv_key2)) addinv (inv_key2);
				if (!invcount(inv_key3)) addinv (inv_key3);
				if (!invcount(inv_invin)) addinv (inv_invin);
				statmodflg|=mod_screen;
				}
			else if ((cheatchar=='Z')&&(cheatcount==3)) {
				cheatchar=0; debug=!debug;	statmodflg|=mod_screen;
				};
			switch (key) {												// COMMANDS
				case k_f1: dotextmsg(1,0); break;				// HELP
				case 'S':												// SAVE
					c=findcheckpt(0);
					if (objs[c].state==5) {
						snd_play (4,snd_masher);
						temppage=pagedraw; pagedraw=pageshow; setpages();
						savegame(); drawstats();
						pagedraw=temppage; setpages(); moddrawboard();
						}
					else dotextmsg (4,0); break;
				case 'L':												// LOAD
					snd_play (4,snd_masher);
					temppage=pagedraw; pagedraw=pageshow; setpages();
					if (loadgame()) {
						init_colors(); fadeout();
						statwin(1); drawstats(); setorigin();
						drawboard(); dolevelsong();
						pagedraw=temppage; setpages(); moddrawboard();
						fadein();
						}
					else {
						pagedraw=temppage; setpages(); moddrawboard();
						}; break;
				case 'P': dotextmsg(6,0); break;					// PAUSE
				case 'B':												// BUYMENU
					c=findcheckpt(0);
					if (objs[c].state!=5) {
						 setpagemode(0); buymenu();
						 }; break;
				case enter: setpagemode(0); inv_win(); break;
				case 'G': granny=!granny; break;					// GRANNY MODE
				case 'N': soundf=!soundf; break;					// NOISE
				case escape:
				case 'Q': setpagemode(0); gamemenu();
				};
			};

		if (demoflg) {
			if (!countobj(obj_mapdemo,0)) {
				addobj (obj_mapdemo,objs[0].x,objs[0].y,0,0);
				};
			};

		upd_bkgnd();
		upd_objs(1);
		upd_botmsg();
		refresh(pagemode);
		purgeobjs();
		if (o_col==1) {init_colors(); o_col=0;};

		if ((demoflg)&&(!macplay)) gameover=1;
		while (((*myclock)-begclock)<(granny+1)) continue;
	} while (!gameover);
	key=0; stopmac(); macaborted=1;
	if (gameover==2) pageview (200);					// Won!
	setpagemode (0); if (!demoflg) printhi(1);
	};

void pageview (int pgnum) {
	int c,x,y,obj;
	scrnxs=fullxs; scrnys=fullys;
	reloop: obj=-1;
	setpagemode(1); fadeout();

	for (c=0; c<numobjs; c++) {
		if ((objs[c].objkind==obj_checkpt)&&(objs[c].counter==pgnum)) obj=c;
		};
	do {
		if (obj>0) {
			memcpy (&tempvp,&gamevp,sizeof(vptype));
			memcpy (&gamevp,&mainvp,sizeof(vptype));
			gamevp.vpox=objs[obj].x; gamevp.vpoy=objs[obj].y;
			drawboard();
			if (objs[obj].xd==3) {
				for (x=0; x<10; x++) {
					for (y=0; y<10; y++) {
						drawshape (&gamevp,0x7900+x+y*10,x*16+48,y*16+
							(objs[obj].y+16));
						};
					};
				};
			pageflip(); setpagemode(0); fadein();
			memcpy (&gamevp,&tempvp,sizeof(vptype));

			reloop2:
			key=0; fire1off=1; fire2off=1;
			do {checkctrl0 (0);} while ((key!=escape)&&(key!=200)&&(key!=208)
				&&(fire1==0)&&(fire2==0)&&(key!=' '));

			// to turn the page in both directions
			if (key==200) {
				if ((objs[obj].xd!=1)&&(objs[obj].yd==0)) {
					pgnum--; goto reloop;
					}
				else goto reloop2;
				}
			else if ((key==208)||(fire1)||(fire2)||(key==' ')) {
				if ((objs[obj].xd!=2)&&(objs[obj].yd==0)) {
					pgnum++; goto reloop;
					}
				else if ((objs[obj].xd==2)||(objs[obj].yd!=0)) key=escape;
				else goto reloop2;
				}
			};
		} while (key!=escape);

	scrnxs=normxs; scrnys=normys;
	setpagemode(1); fadeout();
	clrvp (&mainvp,248);
//	drawboard();
	pageflip(); setpagemode(0); fadein();
	};

void dodemo (void) {
	demonum=0;
	macabort=0;
	setpagemode (1); fadeout();

	do {
		if (demonum!=0) {fadeout(); setpagemode (1);};
		if (demolvl[demonum]==0) demonum=0;
		loadboard (demoboard[demonum]);
		init_inv();	statwin(1); drawstats(); setorigin();
		pl.level=demolvl[demonum];
		p_reenter(0); drawboard();
		pageflip(); setpagemode(0); fadein();

		playmac (demoname[demonum]);
		if (macplay) {
			play (1);
			stopmac();
			demonum++;
			}
		else macaborted=1;
		} while (!macaborted);
	};

void main (int argc, char *argv[]) {
//	cfg_getpath(argc,argv); strcpy (tempname,path);
	strcpy (tempname,"board_t"); strcat (tempname,ext);
	if ((coreleft()+205968)<558080) rexit2(0);
	loadcfg();
	clrscr();
	uncrunch (CFG_WIN,scrnaddr,c_len);
	window (12,14,68,22); textcolor (15); textbackground (1);
	gotoxy (1,1); clrscr();

	cfg_init(argc,argv);
	snd_init(xvocfile);
	gc_init();

	if (!doconfig()) goto abortstart;
	gr_init();
		// not enough memory (583,680 needed)
	if ((vocflag||musicflag)&&((coreleft()+205968)<583680)) rexit2(1);

	clrpal();
	savecfg();

	shm_init (xshafile);
	shm_want [1]=1;
	shm_want [2]=1;
	shm_want [53]=1;
	shm_do();

	snd_do();
	sb_playtune (xintrosong);
	wait();

	fadeout(); clrvp (&mainvp,248);
	shm_want [5]=1;
	shm_want [53]=0;

	shm_do();
	scrnxs=normxs; scrnys=normys;

	init_info();
	init_objinfo();
	init_win();
	init_brd();
	init_objs();
	init_colors();

	pageflip(); setpagemode(0); fadein(); mainmenu();

	fadeout();
	shm_exit();
	gr_exit();
	abortstart:
	gc_exit();
	snd_exit();

	window (1,1,80,25); textcolor (15); textbackground (0);
	gotoxy (1,1); clrscr();
	};

void rexit (int num) {
	char errnum[12];

	shm_exit();
	gr_exit();
	gc_exit();
	snd_exit();

	clrscr();
	uncrunch (CFG_WIN,scrnaddr,c_len);
	window (12,14,68,22); textcolor (15); textbackground (1);
	gotoxy (1,1); clrscr();
	cputs ("Sorry, error <");
	cputs (itoa(num,errnum,10));
	cputs ("> has occurred.\r\n");
	cputs ("Please type HELPME for troubleshooting information.\r\n");
	cputs ("\r\n");
	if (num==9) {
		cputs ("  Problem: Not enough free RAM to run Xargon.\r\n");
		cputs ("Solutions: Boot clean with no TSR's loaded.\r\n");
		if (vocflag) {
			cputs ("           Turn off SOUND BLASTER sound effects.");
			};
		}; delay (1000); window (1,1,80,25); gotoxy (1,25); exit(1);
	};

void rexit2 (int n) {
	char errnum[12];

	if (n) {
		gr_exit();
		gc_exit();
		snd_exit();
		};

	clrscr();
	uncrunch (CFG_WIN,scrnaddr,c_len);
	window (12,14,68,22); textcolor (15); textbackground (1);
	gotoxy (1,1); clrscr();
	if (n) {
		cputs ("Sorry, you don't have enough free conventional\r\n");
		cputs ("memory to run XARGON with Sound Blaster support\r\n");
		cputs ("enabled. You need to either free up at least\r\n");
		cputs ("570K of conventional memory, or run XARGON again\r\n");
		cputs ("without Sound Blaster support.");
		}
	else {
		cputs ("Sorry, you don't have enough free conventional\r\n");
		cputs ("memory to run XARGON. You need at least	545K of\r\n");
		cputs ("conventional memory to begin.");
		}; delay (1000); window (1,1,80,25); gotoxy (1,25); exit(1);
	};