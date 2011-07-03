// X_OBJMAN.C
//
// Xargon Object Management
//
// by Allen W. Pilgrim

#include <stdlib.h>
#include <string.h>
#include "\develop\xargon\include\gr.h"
#include "\develop\xargon\include\keyboard.h"
#include "\develop\xargon\include\windows.h"
#include "\develop\xargon\include\gamectrl.h"
#include "\develop\xargon\include\music.h"
#include "\develop\xargon\include\x_obj.h"
#include "\develop\xargon\include\xargon.h"
#include "\develop\xargon\include\x_snd.h"

//byte updtab [boardxs][20];			// refresh (0) only

void init_objs (void) {				// Not to be confused with zapobjs()
	numobjs=1;
	objs [0].objkind   = obj_player;
	objs [0].x         = 40;
	objs [0].y         = 40;
	objs [0].xd        = 0;
	objs [0].yd        = 0;
	objs [0].state     = st_stand;
	objs [0].substate  = 0;
	objs [0].statecount= 0;
	objs [0].counter   = 0;
	objs [0].xl        = kindxl[obj_player];
	objs [0].yl        = kindyl[obj_player];
	objs [0].info1     = 0;
	objs [0].zaphold   = 0;
	objs [0].inside    = NULL;
	pl.numinv=0;
	pl.level=1;
	setmem (pl.pad,sizeof(pl.pad),0);
	};

void playerkill (int z) {
	addscore (kindscore[objs[z].objkind],objs[z].x,objs[z].y);
	notemod(z);
	killobj (z);
	};

int countobj (int objkind, int flg) {
	int c;
	int n=0;

	if (flg) {
		for (c=0; c<numobjs; c++) {
			if ((objs[c].objkind==objkind)&&(objs[c].state==1)) {
				n+=objs[c].objkind==objkind;
				};
			};
		}
	else {
		for (c=0; c<numobjs; c++) {
			n+=objs[c].objkind==objkind;
			};
		}; return (n);
	};

void notemod (int n) {
	int startx, endx, starty, endy, x, y;

	startx = objs[n].x/16;
	starty = objs[n].y/16;
	endx   = (objs[n].x+objs[n].xl+15)/16;
	endy   = (objs[n].y+objs[n].yl+15)/16;

	for (y=starty; y<endy; y++) {
		for (x=startx; x<endx; x++) {
			modboard (x,y);
			};
		};
	};

void setobjsize (int n) {
	char tempstr[8];
	int slen=0;

	objs[n].xl=kindxl[objs[n].objkind];
	objs[n].yl=kindyl[objs[n].objkind];

	if (objs[n].inside!=NULL) slen=strlen(objs[n].inside);
	if (objs[n].objkind==obj_text6) objs[n].xl=slen*6;
	else if (objs[n].objkind==obj_text8) objs[n].xl=slen*8;
	else if (objs[n].objkind==obj_score) {
		objs[n].xl=(2+strlen (itoa (objs[n].state,tempstr,10)))*
			kindxl[obj_score];
		};
	};

int findcheckpt (int level) {
	int n;

	for (n=0; n<numobjs; n++) {
		if ((objs[n].objkind==obj_checkpt)&&(objs[n].counter==level)) {
			return (n);
			};
		};
	return (0);
	};

void dolevelsong (void) {
	int n,c,d;

	n=findcheckpt (pl.level);							// =0 (player) if not there
	if ((n>0)&&(objs[n].inside)&&((objs[n].inside[0]=='*')||
		(objs[n].inside[0]=='#')||(objs[n].inside[0]=='&')))
			strcpy (newlevel,objs[n].inside);		// Level song
	else {
		c=findcheckpt(0);
		d=objs[c].inside[0];
		if ((d=='*')||(d=='#')||(d=='&'))
			strcpy (newlevel,objs[c].inside);		// Level song
		};
	};

void p_reenter (int died) {
	int n,destx,desty,x,y;
	statmodflg|=mod_screen;
	n=findcheckpt (pl.level);							// =0 (player) if not there
	destx=objs[n].x; desty=objs[n].y;

	if (objs[0].objkind!=obj_tiny) desty-=24;
	if (n>0) {
		if ((died)&&(objs[n].state==1)) {			// restart when die
			loadboard (curlevel);
			stats(0);
			n=findcheckpt (pl.level);
			};
		};

	dolevelsong();
	destx&=0xfff8;
	objs[0].x=destx; objs[0].y=desty;
	setorigin();

	for (x=0; x<boardxs; x++)
		for (y=0; y<boardys; y++)
			bd[x][y]|=mod_screen;
	objs[0].state=st_begin;
	objs[0].statecount=0;
	pl.health=5;
	};

void p_ouch (int healthtake, int diemode) {
	if (objs[0].objkind==obj_tiny) return;
	if ((objs[0].objkind==obj_player)&&
		(stateinfo [objs[0].state]&sti_invincible)) return;
	if (objs[0].objkind==obj_herobee) diemode=die_bird;
	healthtake-=invcount(inv_invin);
	if (healthtake<=0) return;
	pl.health-=healthtake; pl.ouched=4; statmodflg|=mod_screen;
	if (pl.health>0) snd_play (5,snd_ouch);
	else {
		pl.health=0;
		objs[0].objkind=obj_player;
		if (objs[0].state==st_platform) objs[0].statecount=18;
		else objs[0].statecount=0;
		objs[0].state=st_die;
		objs[0].substate=diemode;
		if ((diemode==die_bird)||(diemode==die_fish)) pl.ouched=0;
		if (diemode==die_bird) objs[0].yd=2;
		snd_play (5,snd_herodie);
		explode1 (objs[0].x,objs[0].y,10,0);
		};
	};

void seekplayer (int n, int *dx, int *dy) {
	*dx=(objs[0].x>objs[n].x)-(objs[0].x<objs[n].x);
	*dy=(objs[0].y>objs[n].y)-(objs[0].y<objs[n].y);
	};

void modscroll (int sxd, int syd, int modcode) {
	int x,y,newx,newy,vpey,endy,endx,vpex;

	if (sxd>0) {
		vpex=(gamevp.vpox+gamevp.vpxl-sxd)/16;
		endx=min((gamevp.vpox+gamevp.vpxl-1)/16,boardxs-1);
		for (x=vpex; x<=endx; x++) {
			for (y=0; y<(scrnys+1);y++) {
				newy=min(gamevp.vpoy/16+y,boardys-1);
				bd[x][newy]|=modcode;
				if (modcode==mod_virtual) drawcell(x,newy);
				};
			};
		}
	else if (sxd<0) {
		newx=gamevp.vpox/16;
		for (y=0; y<(scrnys+1); y++) {
			newy=gamevp.vpoy/16+y;
			bd[newx][newy]|=modcode;
			if (modcode==mod_virtual) drawcell(newx,newy);
			};
		};
	if (syd>0) {									// Redrawing bottom
		vpey=gamevp.vpoy+gamevp.vpyl;
		endy=min((vpey-1)/16,boardys-1);
		for (newy=(vpey-syd)/16; newy<=endy; newy++) {
			for (x=0; x<(scrnxs+1);x++) {
				newx=min(gamevp.vpox/16+x,boardxs-1);
				bd [newx][newy]|=modcode;
				if (modcode==mod_virtual) drawcell(newx,newy);
				};
			};
		}
	else if (syd<0) {								// Redrawing top
		for (newy=gamevp.vpoy/16; newy<=((gamevp.vpoy-syd-1)/16); newy++) {
			for (x=0; x<(scrnxs+1);x++) {
				newx=min(gamevp.vpox/16+x,boardxs-1);
				bd [newx][newy]|=modcode;
				if (modcode==mod_virtual) drawcell(newx,newy);
				};
			};
		};
	};

void scroll1 (int sxd, int syd) {
	int x,y;
	int cut0, cut1, cut2, cut3;
	int startx, starty,startx2,starty2;
	int endx, endy,endx2,endy2;

	startx = oldx0/16;
	starty = oldy0/16;
	endx   = (oldx0+objs[0].xl+15)/16;
	endy   = (oldy0+objs[0].yl+15)/16;

	startx2 = objs[0].x/16;
	starty2 = objs[0].y/16;
	endx2   = (objs[0].x+objs[0].xl+15)/16;
	endy2   = (objs[0].y+objs[0].yl+15)/16;

	if (sxd==0) {
		cut0=0;
		cut1=min (startx,startx2)*16-gamevp.vpox;
		cut2=max (endx,endx2)*16-gamevp.vpox;
		cut3=gamevp.vpxl;
		scroll (&gamevp,cut0,0,cut1,gamevp.vpyl,0,-syd);
		}
	else if (syd==0) {
		cut0=0;
		cut1=min (starty,starty2)*16-gamevp.vpoy;
		cut2=max (endy,  endy2  )*16 -gamevp.vpoy;
		cut3=gamevp.vpyl;
		scroll (&gamevp,0,cut0,gamevp.vpxl,cut1,-sxd,0);
		};

	for (x=startx; x<endx; x++) {
		for (y=starty; y<endy; y++) {
			drawcell (x,y);
			bd [x][y]&=(mod_screenonly^0xffff);
			};
		};

	if (sxd==0) {
		scroll (&gamevp,cut1,0,cut2,gamevp.vpyl,0,-syd);
		gamevp.vpoy+=syd;
		(*kindmsg [objs[0].objkind])(0,msg_draw,0);
		scroll (&gamevp,cut2,0,cut3,gamevp.vpyl,0,-syd);
		}
	else if (syd==0) {
		scroll (&gamevp,0,cut1,gamevp.vpxl,cut2,-sxd,0);
		gamevp.vpox+=sxd;
		(*kindmsg [objs[0].objkind])(0,msg_draw,0);
		scroll (&gamevp,0,cut2,gamevp.vpxl,cut3,-sxd,0);
		}
	else {
		scrollvp (&gamevp,-sxd,-syd);
		gamevp.vpox+=sxd;
		gamevp.vpoy+=syd;
		(*kindmsg [objs[0].objkind])(0,msg_draw,0);
		};
	modscroll (sxd,syd,mod_virtual);
	};

void pagedscroll (int sxd, int syd) {
	scrollvp (&gamevp,-sxd,-syd);
	gamevp.vpox += sxd;
	gamevp.vpoy += syd;
	modscroll (sxd,syd,mod_screen);
	};

void refresh (int pgmode) {
	int x,y,c,n;
	int startx, starty, endx, endy;
	byte updtab [boardxs][20];					// refresh (0) only

	if (pgmode) {
		if (statmodflg) {
			drawstats();
			statmodflg&=(pagedraw+1)*mod_page0;
			};
		if (((scrollxd+oldscrollxd)!=0)||((scrollyd+oldscrollyd)!=0)) {
			gamevp.vpox-=oldscrollxd; gamevp.vpoy-=oldscrollyd;
			pagedscroll (scrollxd+oldscrollxd,scrollyd+oldscrollyd);
			};
		oldscrollxd=scrollxd; oldscrollyd=scrollyd;

		startx=min((gamevp.vpox/16)+scrnxs,boardxs-1);
		starty=min((gamevp.vpoy/16)+scrnys-1,boardys-1);
		endx=max(0,(gamevp.vpox/16)-2);
		endy=max(0,(gamevp.vpoy/16)-2);

		for (x=startx; x>=endx; x--) {
			for (y=starty; y>=endy; y--) {
				if (bd[x][y]&mod_screen) {
					drawcell (x,y);
					bd[x][y]&=((pagedraw+1)*mod_page0)^0xffff;
					};
				};
			};
		for (n=0; n<numobjs; n++) {
			if ((objs[n].objflags&mod_screen)&&
				(kindflags[objs[n].objkind]&f_back)) {
				(*kindmsg [objs[n].objkind])(n,msg_draw,0);
				objs[n].objflags&=((pagedraw+1)*mod_page0)^0xffff;
				};
			};
		for (n=numobjs-1; n>=0; n--) {
			if ((objs[n].objflags&mod_screen)&&
				(!(kindflags[objs[n].objkind]&f_front))&&
				(!(kindflags[objs[n].objkind]&f_back))) {
				(*kindmsg [objs[n].objkind])(n,msg_draw,0);
				objs[n].objflags&=((pagedraw+1)*mod_page0)^0xffff;
				};
			};
		for (n=0; n<numobjs; n++) {
			if ((objs[n].objflags&mod_screen)&&
				(kindflags[objs[n].objkind]&f_front)) {
				(*kindmsg [objs[n].objkind])(n,msg_draw,0);
				objs[n].objflags&=((pagedraw+1)*mod_page0)^0xffff;
				};
			}; pageflip();
		}
	else {
		if (statmodflg) {
			drawstats();
			statmodflg=0;
			};
		for (c=0; c<boardxs; c++) updtab[c][0]=255;		// zero updtab
		if ((scrollxd!=0)||(scrollyd!=0)) scroll1(scrollxd,scrollyd);

		startx=(gamevp.vpox/16)+scrnxs-1;
		starty=(gamevp.vpoy/16)+scrnys-1;
		endx=max(0,(gamevp.vpox/16)-2);
		endy=max(0,(gamevp.vpoy/16)-2);

		for (n=0; n<numobjs; n++) {
			if (objs[n].objflags&mod_screen) {				// Add to Updtab
				x=objs[n].x/16; if (x<endx) x=endx;
				c=0; while (updtab[x][c]!=255) {c++;};
				updtab[x][c]=n;
				updtab[x][c+1]=255;
				objs[n].objflags&=(mod_screen^0xffff);
				};
			};
		for (x=startx; x>=endx; x--) {
			for (y=starty; y>=endy; y--) {
				if (bd[x][y]&mod_screenonly) {
					drawcell (x,y);
					bd[x][y]&=(mod_screen^0xffff);
					};
				};
			for (c=0;(updtab[x][c]!=255)&&(c<20);c++) {
				n=updtab[x][c];
				(*kindmsg [objs [n].objkind])(n,msg_draw,0);
				};
			};
		};
	if (pl.ouched>=2) {
		for (c=207; c<234; c++) {				// change all red to max
			setcolor (c,63,vgapal[c*3+1],vgapal[c*3+2]);
			};
		setcolor (0,63,0,0); pl.ouched--; statmodflg|=mod_screen;
		}
	else if (pl.ouched<=-2) {
//		for (c=207; c<234; c++) {				// change all green to max
//			setcolor (c,vgapal[c*3+0],50,vgapal[c*3+2]);
//			};
		for (c=207; c<234; c++) {				// change all blue to max
			setcolor (c,vgapal[c*3+0],vgapal[c*3+1],63);
			};
		pl.ouched++; statmodflg|=mod_screen;
		};
	};

void upd_bkgnd (void) {
	int x,y,temp;
	int startx, starty, endx, endy;

	startx=gamevp.vpox/16;
	starty=gamevp.vpoy/16;
	endx=min(startx+scrnxs,boardxs-1);
	endy=min(starty+scrnys,boardys-1);

	for (x=startx; x<=endx; x++) {
		for (y=starty; y<=endy; y++) {
			temp=board(x,y);
			if (info[temp].flags&f_msgupdate) {
				bd[x][y]|=(msg_block(x,y,msg_update)!=0)*mod_screen;
				};
			};
		};
	};

void upd_objs (int doflag) {
	int countn,count2;
	int n,n2;
	int flag,oldx,oldy,oldxl,oldyl;
	int x,y,startx,endx,starty,endy;

// Determine which objs are on screen

	numscrnobjs=1; scrnobjs[0]=0;
	startx=gamevp.vpox-96; endx=gamevp.vpox+gamevp.vpxl+96;
	starty=gamevp.vpoy-48; endy=gamevp.vpoy+gamevp.vpyl+48;

	for (n=1; (n<numobjs)&&(numscrnobjs<maxscrnobjs); n++) {
		if ((((objs[n].x+objs[n].xl)>=startx)&&((objs[n].x)<=endx)&&
			((objs[n].y+objs[n].yl)>=starty)&&((objs[n].y)<=endy))||
			(kindflags[objs[n].objkind]&f_always)) {
			scrnobjs[numscrnobjs++]=n;
			objs[n].objflags&=(pagedraw*mod_page0)^0xffff;
			};
		};

	scrollxd=0; scrollyd=0; oldx0=objs[0].x; oldy0=objs[0].y;

// Update objects & Handle TOUCH

	for (countn=0; countn<numscrnobjs; countn++) {
		n=scrnobjs[countn]; 
		oldx=objs[n].x; oldy=objs[n].y;
		oldxl=objs[n].xl; oldyl=objs[n].yl;
		if (doflag) {
			if (objs[n].zaphold>0) objs[n].zaphold--;
			if ((*kindmsg [objs[n].objkind])(n,msg_update,0)) {
				objs[n].objflags|=mod_screen;
				};
			}
		else objs[n].objflags|=mod_screen;
		if (kindflags[objs[n].objkind]&f_msgtouch) {
			for (count2=0; count2<=numscrnobjs; count2++) {
				n2=scrnobjs[count2];
if ((objs[n].objkind==0)&&(objs[0].state==st_stand)&&
	(objs[0].xd==0)&&(objs[0].yd==3)) {
				if ((n2!=0)&&(objs[n2].x<(objs[0].x+objs[0].xl))&&
					(objs[0].x<(objs[n2].x+objs[n2].xl))&&
					(objs[n2].y<(objs[0].y+objs[0].yl))&&
					(objs[0].y+18<(objs[n2].y+objs[n2].yl))) {
						(*kindmsg [objs[0].objkind])(0,msg_touch,n2);
						objs[n].objflags|=mod_screen;
						(*kindmsg [objs[n2].objkind])(n2,msg_touch,0);
						objs[n].objflags|=mod_screen;
					};
}
else {
				if ((n2!=n)&&(objs[n2].x<(objs[n].x+objs[n].xl))&&
					(objs[n].x<(objs[n2].x+objs[n2].xl))&&
					(objs[n2].y<(objs[n].y+objs[n].yl))&&
					(objs[n].y<(objs[n2].y+objs[n2].yl))) {
						(*kindmsg [objs[n].objkind])(n,msg_touch,n2);
						objs[n].objflags|=mod_screen;
						(*kindmsg [objs[n2].objkind])(n2,msg_touch,n);
						objs[n].objflags|=mod_screen;
					};		// end if ((n2!=n)&&
};
				};
			};
		if (objs[n].objflags&mod_screen) {
			startx=oldx/16; starty=oldy/16;
			endx=(oldx+oldxl+15)/16; endy=(oldy+oldyl+15)/16;
			for (y=starty; y<endy; y++) {
				for (x=startx; x<endx; x++) {
					bd[x][y]|=mod_screen;
					};
				};
			};
		};                                

	for (countn=0; countn<numscrnobjs; countn++) {
		n=scrnobjs[countn];

// Note whether under was modded
		x=objs[n].x; y=objs[n].y;
		startx=x/16; starty=y/16;
		endx=(x+objs[n].xl+15)/16; endy=(y+objs[n].yl+15)/16;
		flag=0;
		for (y=starty; y<endy; y++) {
			for (x=startx; x<endx; x++) {
				flag=flag||(bd[x][y]&mod_screen);
				};
			};
		if (flag) objs[n].objflags|=mod_screen;
		};
	};

void killobj (int n) {
	objs[n].objflags|=mod_screen;
	objs[n].objkind=obj_killme;
	};

int addobj (int kind, int x, int y, int xd, int yd) {
	objs [numobjs].objkind= kind;
	objs [numobjs].x      = x;
	objs [numobjs].y      = y;
	objs [numobjs].state  = 0;
	objs [numobjs].xd     = xd;
	objs [numobjs].yd     = yd;
	objs [numobjs].xl     = kindxl [kind];
	objs [numobjs].yl     = kindyl [kind];
	objs [numobjs].substate = 0;
	objs [numobjs].statecount=0;
	objs [numobjs].objflags=0;
	objs [numobjs].inside=NULL;
	objs [numobjs].info1=0;
	objs [numobjs].zaphold=0;
	objs [numobjs].counter=0;
	if (numobjs<maxobjs) numobjs++;
	return (numobjs-1);
	};

void addinv (int invthing) {
	if (pl.numinv>=29) return;
	statmodflg|=mod_screen;
	pl.inv [pl.numinv]=invthing;
	pl.numinv++;
	};

int takeinv (int invthing) {
	int c,d;

	for (c=0; c<pl.numinv; c++) {
		if (pl.inv[c]==invthing) {
			for (d=c+1; d<pl.numinv; d++) {
				pl.inv[d-1]=pl.inv[d];
				};
			pl.numinv--; statmodflg|=mod_screen; return (1);
			};
		};	return (0);
	};

int invcount (int invthing) {
	int c,a;

	a=0;
	for (c=0; c<pl.numinv; c++) {
		a+=(pl.inv[c]==invthing);
		}; return (a);
	};

void init_inv (void) {
	pl.emeralds=0;
	pl.health=5;
	cnt_fruit=0;
	icon_fruit=0;
	addinv (inv_laser);
	};


void moveobj (int n, int newx, int newy) {
	if (newy<0) newy=0;
	else if (newy>(boardys*16-16-objs[n].yl))
	newy=boardys*16-16-objs[n].yl;
	if (newx<0) newx=0;
	else if (newx>(boardxs*16-16-objs[n].xl))
	newx=boardxs*16-16-objs[n].xl;
	objs [n].x=newx; objs [n].y=newy;
	};

int standfloor (int n, int dx, int dy) {	// 1 if will be standing on floor
	int newx,newy,temp;
	int startx,endx,xc;

	newx=objs[n].x+dx; newy=objs[n].y+dy;
	if (((newy+objs[n].yl)&15)!=0) return (0);
	newy=((newy+objs[n].yl-1)/16)+1;
	startx=newx/16;
	endx=(newx+objs[n].xl+15)/16;

	for (xc=startx; xc<endx; xc++) {
		temp=info[board(xc,newy)].flags&(f_playerthru|f_notstair);
		if (temp==(f_playerthru|f_notstair)) return (0);
		}; return (1);
	};

/* Returns
	0 no move
	1 correct move
	2 move y only
	4 move x only
*/

int trymove (int n, int newx, int newy) {
	int ourflags;
	ourflags=f_playerthru;

	if (newy>objs[n].y) ourflags|=f_notstair;
	if (cando (n,newx,newy,ourflags)==ourflags) {
		moveobj (n,newx,newy);
		return (1);
		}
	else if (cando (n,objs[n].x,newy,ourflags)==ourflags) {
		moveobj (n,objs[n].x,newy);
		return (2);
		}
	else if (cando (n,newx,objs[n].y,ourflags)==ourflags) {
		moveobj (n,newx,objs[n].y);
		return (4);
		};
	return (0);
	};

int justmove (int n, int newx, int newy) {
	if (cando(n,newx,newy,f_playerthru)) {
		moveobj (n,newx,newy); return (1);
		};
	return (0);
	};

int onscreen (int n) {
	if (((objs[n].x+objs[n].xl)>=gamevp.vpox)&&
		((objs[n].y+objs[n].yl)>=gamevp.vpoy)&&
		(objs[n].x<=(gamevp.vpox+gamevp.vpxl))&&
		(objs[n].y<=(gamevp.vpoy+gamevp.vpyl)))
		return (1);
	return (0);
	};

int trymovey (int n, int newx, int newy) {
	int ourflags=f_playerthru|f_notstair;

	if (cando (n, newx, newy,ourflags)==ourflags) {
		moveobj (n, newx, newy); return (1);
		}
	else if (cando (n,objs [n].x, newy,ourflags)==ourflags) {
		moveobj (n,objs[n].x,newy); return (1);
		};
	objs [n].xd=0;	return (0);
	};

int crawl (int n, int dx, int dy) {
	int newx=objs[n].x+dx;
	int newy=objs[n].y+dy;

	if (standfloor (n,dx,dy)) {
		if (cando (n,newx,newy,f_playerthru)==f_playerthru) {
			moveobj (n,newx,newy);
			return (1);
			};
		};	return (0);
	};

void addscore (int sc,int x,int y) {
	int n;

	n=addobj (obj_score,x,y,0,0);
	if (n) {
		objs[n].state=sc;
		objs[n].counter=30;
//		objs[n].counter=24;
		objs[n].xd=random(4)*((x>objs[0].x)-(x<objs[0].x));
		objs[n].yd=random(4)+4;
		setobjsize(n);
		};
	pl.score+=sc; statmodflg|=mod_screen;
	};

/*void addtext (char *text, int fontobj, int x, int y) {
	int n;

	n=addobj (fontobj,x,y,0,0);
	if (n) {
		objs[n].counter=64;
		objs[n].inside=strdup(text);
		objs[n].xd=2;
		objs[n].yd=-1;
		setobjsize(n);
		};
	};*/

void sendtrig (int cnt, int msg, int fromobj) {
	int obj;

	for (obj=0; obj<numobjs; obj++) {
		if ((kindflags[objs[obj].objkind]&f_trigger)&&
			(objs[obj].counter==cnt)) {
			(*kindmsg [objs[obj].objkind])(obj,msg,fromobj);
			};
		};
	};

void setorigin (void) {
	gamevp.vpox=(objs[0].x-scrnxs*8)&0xfff8;
	if (gamevp.vpox<16) gamevp.vpox=16;
//	if (gamevp.vpox<0) gamevp.vpox=0;
	else if (gamevp.vpox>((boardxs-scrnxs)*16)) {
		gamevp.vpox=(boardxs-scrnxs)*16;
		};
	gamevp.vpoy=objs[0].y+16-scrnys*8;
	if (gamevp.vpoy<0) gamevp.vpoy=0;
	else if (gamevp.vpoy>((boardys+1-scrnys)*16)) {
		gamevp.vpoy=(boardys+1-scrnys)*16;
		};
	oldscrollxd=0; oldscrollyd=0;
	};

// Specially handles f_notstair & f_msgtouch

int cando (int n, int newx, int newy, int ourflags) {
	int x,y,temp;
	int flagor, result;
	int startx, endx, starty, splity, endy;

	startx=newx/16;
	starty=newy/16;
	endx=(newx+objs[n].xl+15)/16;
	endy=(newy+objs[n].yl+15)/16;
	splity=(objs[n].y+kindyl[objs[n].objkind]+15)/16;

	flagor=f_notstair;
	result=0xffff;
	for (y=starty; y<endy; y++) {
		if (y>=splity) flagor=0;
		for (x=startx; x<endx; x++) {
			temp=(info[board(x,y)].flags|flagor)&ourflags;
			result&=temp;
			};
		}; return (result);
	};

int objdo (int n, int newx, int newy, int ourflags) {
	int x,y;
	int result=0xffff;
	int startx, endx, starty, endy;

	startx=newx/16;
	starty=newy/16;
	endx=(newx+objs[n].xl+15)/16;
	endy=(newy+objs[n].yl+15)/16;

	for (y=starty; y<endy; y++) {
		for (x=startx; x<endx; x++) {
			result&=((info[board(x,y)].flags)&ourflags);
			};
		}; return (result);
	};

void touchbkgnd (int n) {
	int x,y;
	int startx, endx, starty, endy;

	if (stateinfo [objs[n].state]&sti_invincible) return;

	startx=objs[n].x/16;
	starty=objs[n].y/16;
	endx=(objs[n].x+objs[n].xl+15)/16;
	endy=(objs[n].y+objs[n].yl+15)/16;

	for (y=starty; y<endy; y++) {
		for (x=startx; x<endx; x++) {
			if (info[board(x,y)].flags&f_msgtouch) msg_block(x,y,msg_touch);
			else if ((board(x,y+1)>=crum1)&&(board(x,y+1)<=crum_last)&&
				((gamecount&3)==2)) {
				addobj (obj_fragexpl,objs[n].x+random(10),objs[n].y+44,0,2);
				objs[numobjs-1].state=-1;
				setboard (x,y+1,board(x,y+1)+1);
				if (board(x,y+1)>crum_last) setboard(x,y+1,0);
				};
			};
		};
	};

void purgeobjs (void) {
	int c,d;
	d=0;

	for (c=0; c<numobjs; c++) {
		if (c!=d) memcpy (&objs[d],&objs[c],sizeof(objtype));
		if (objs[c].objkind==obj_killme) {
			if (objs[c].inside!=NULL) free (objs[c].inside);
			}
		else d++;
		};	numobjs=d;
	};

void hitplayer (int n, int flg) {
	if ((objs[n].zaphold==0)&&
		(!(stateinfo[objs[0].state]&sti_invincible))) {
		if (flg) p_ouch(1,die_fish);
		else p_ouch(1,die_ash);
		addobj (obj_hithero,objs[0].x,objs[0].y,0,0);
		};
	objs[n].zaphold=3;							// Set plyr safe from obj safe
	};

int fishdo (int n, int x, int y) {
	if (objdo (n,x,y, f_playerthru|f_water)==(f_playerthru|f_water)) {
		moveobj (n,x,y);
		return (1);
		};
	return (0);
	};

void pointvect (int n1, int n2, int *ax, int *ay, int len) {
   int x=objs[n1].x-objs[n2].x;
   int y=objs[n1].y-objs[n2].y;

	if (x==0) {
      if (y!=0) y=len*sgn(y);
      }
   else if (y==0) x=len*sgn(x);
   else if (abs(x)>abs(y)) {y=(y*len)/(abs(x)); x=len*sgn(x);}
   else {x=(x*len)/(abs(y)); y=len*sgn(y);};
   *ax=x; *ay=y;
   };

int vectdist (int n1,int n2) {
	return (abs(objs[n1].x-objs[n2].x)+abs(objs[n1].y-objs[n2].y));
	};

int trybreakwall (int n, int x, int y) {
	int xc,yc;
	int f=0;

	for (xc=x/16;xc<=((x+objs[n].xl)/16); xc++) {
		for (yc=y/16;yc<=((y+objs[n].yl)/16); yc++) {
			if (board(xc,yc)==breakwall) {
				setboard(xc,yc,0); pl.score+=10;
				if (objs[n].objkind==obj_fireball) {
					addobj (obj_fireexpl,xc*16-16,yc*16-16,0,0);
					}
				else killobj (n);
				if (f++==0) {
					explode1 (xc*16,yc*16,5,1);
					snd_play (3,snd_stalagcrash);
					};
				}
			else if (board(xc,yc)==eyeop) {
				addobj (obj_flash,xc*16-1,yc*16-4,0,0);
				setboard(xc,yc,noeye);
				addscore (100,xc*16,yc*16); killobj (n);
				snd_play (3,snd_stalagcrash);
				};
			}; statmodflg|=mod_screen;
		};	return (f);
	};