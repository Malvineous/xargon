// X_OBJ.C
//
// Xargon Objects
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

int first_elev=1;
int first_heart=1;
int first_emerald=1;
int first_fruit=1;
int first_platform=1;
int first_box=1;

#define defobj(objnum,objname,objmsg,objxl,objyl,objstr,objflgs,objtbl,objscore)\
	int objmsg (int n, int msg, int z);
#include "\develop\xargon\include\x_obj.def"

#undef defobj
#define defobj(objnum,objname,objmsg,objxl,objyl,objstr,objflgs,objtbl,objscore)\
	kindmsg[objnum]   = objmsg;\
	kindxl[objnum]    = objxl;\
	kindyl[objnum]    = objyl;\
	kindname[objnum]  = objstr;\
	kindflags[objnum] = objflgs;\
	kindtable[objnum] = objtbl;\
	kindscore[objnum] = objscore

const int inv_shape[numinvkinds]={0,1,0,0,2,3,0,5,6,7,0,0,0,23,24,25,26};
const int inv_xfm[numinvkinds]={1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0};
const char *inv_getmsg[numinvkinds]={
	"You are magically transformed into a HUMAN!",
	"USE KEY TO OPEN GATE ON MAP",
	"",
	"",
	"Wow! An extra LASER bullet",
	"You are mysteriously changed into a BUMBLEBEE!",
	"You enter the Submersible Underwater Pod-S.U.P.",
	"YOU FOUND A ROCK TO THROW",
	"These XIBOK shoes help you jump higher",
	"You found a shield of INVINCIBILITY",
	"",
	"",
	"",
	"",
	"",
	"",
	""
	};
//int inv_first[numinvkinds]={-1,1,1,1,1,-1,-1,1,1,1,1,1,1,1,1,1,1};

void init_objinfo (void) {
	#include "\develop\xargon\include\x_obj.def"
	};

int msg_null (int n, int msg, int z) {
	return (0);
	};

int msg_mine (int n, int msg, int z) {
	int sh=kindtable[obj_mine]*256+20;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->counter==8) {
				pobj->counter=0; pobj->yd=-pobj->yd;
				};
			if (!fishdo(n,pobj->x+pobj->xd,pobj->y)) pobj->xd=-pobj->xd;
			if (!fishdo(n,pobj->x,pobj->y+pobj->yd)) pobj->yd=-pobj->yd;
			if (random (16)==0) addobj (obj_bubble,pobj->x+6,pobj->y-2,0,0);
			pobj->counter++; return (1);
		case msg_draw:	drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				p_ouch(2,die_fish); snd_play (4,snd_enemykill1);
				explode2 (n); killobj (n); return (1);
				};
			if (kindflags[objs[z].objkind]&f_weapon) {
				snd_play (4,snd_enemykill1); explode2 (n);
				addobj (obj_bullet,pobj->x-4,pobj->y+8,-4,-2);
				addobj (obj_bullet,pobj->x-4,pobj->y+8,-4,0);
				addobj (obj_bullet,pobj->x-4,pobj->y+8,-4,+2);
				addobj (obj_bullet,pobj->x+11,pobj->y+8,+4,-2);
				addobj (obj_bullet,pobj->x+11,pobj->y+8,+4,0);
				addobj (obj_bullet,pobj->x+11,pobj->y+8,+4,+2);
				killobj (n); killobj (z); return (1);
				};
		}; return (0);
	};

int msg_text6 (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->counter>0) {
				if (--pobj->counter<=0) {
					killobj (n); return (1);
					};
				}; break;
		case msg_draw:
			fontcolor (&gamevp,pobj->xd,pobj->yd);
			wprint (&gamevp,pobj->x,pobj->y+1,2,pobj->inside);
		}; return (0);
	};

int msg_text8 (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->counter>0) {
				if (--pobj->counter<=0) {
					killobj (n); return (1);
					};
				}; break;
		case msg_draw:
			fontcolor (&gamevp,pobj->xd,pobj->yd);
			wprint (&gamevp,pobj->x,pobj->y,1,pobj->inside);
		}; return (0);
	};

int msg_bridger (int n, int msg, int z) {
	int c,d,xc,yc;
	int oldcell,newcell,dircell;
	int dochange=0;
	objtype *pobj;	pobj=&(objs[n]);
	xc=pobj->x/16; yc=pobj->y/16;
	dircell=(pobj->xd==0)?stlbeam:bridge;

	switch (msg) {
		case msg_draw:					// Invisible!
			if (designflag) {
				drawshape (&gamevp,0x0123,pobj->x+4,pobj->y+4);
				}; break;
		case msg_trigon:
			if (pobj->state==1) snd_play (2,snd_switchon);
			dochange=1; oldcell=board(xc,yc); newcell=dircell; break;
		case msg_trigoff:
			if (pobj->state==1) snd_play (2,snd_switchoff);
			dochange=1; oldcell=dircell; newcell=0; break;
		case msg_trigger:
			if (board(xc,yc)==dircell)	{
				newcell=0; oldcell=dircell;
				if (pobj->state==1) snd_play (2,snd_switchoff);
				}
			else newcell=dircell; {
				oldcell=board(xc,yc);
				if (pobj->state==1) snd_play (2,snd_switchon);
				};
			dochange=1;
			if (objs[z].objkind==obj_pad) killobj (z);
		};
		if (dochange) {
			if (newcell==0) {
				for (c=-1; c<=1; c+=2) {
					d=board(xc+c*pobj->yd,yc+c*pobj->xd);
					if ((info[d].flags&(f_playerthru|f_notstair))==
						(f_playerthru|f_notstair)) newcell=d;
					};
				};
			while (board(xc,yc)==oldcell) {
				setboard(xc,yc,newcell); xc+=pobj->xd; yc+=pobj->yd;
				}; return (1);
		};	return (0);
	};

int msg_door (int n, int msg, int z) {
	int sh=kindtable[obj_door]*256+32;
	int c,xc,yc;
	const char *doormsg[4]={
		"YOU NEED THE KEY. NOW WHERE IS THAT KEY?",
		"YOU MUST FIND THE KEY TO THIS DOOR FIRST.",
		"FIND THE KEY THAT MATCHES THE KEYHOLE.",
		"THE DOOR IS LOCKED. YOU NEED THE KEY!"
		};
	const char *openmsg[4]={
		"THE DOOR OPENS SLOWLY",
		"YOU GAIN ACCESS TO A NEW AREA",
		"YOU FOUND THE RIGHT KEY AND THE DOOR OPENS",
		"CONGRATULATIONS, YOUR KEY FIT!"
		};
	objtype *pobj; pobj=&(objs[n]);
	xc=pobj->x/16; yc=pobj->y/16;

	switch (msg) {
		case msg_update:
			if (pobj->statecount==0) return (0);
			if (++pobj->statecount>16) {
				drawcell (xc,yc-1); drawcell (xc,yc+1);
				killobj (n); moddrawboard (); return (1);
				}; break;
		case msg_draw:
			if ((pobj->statecount==0)&&(pobj->state!=-1)) {
				drawshape (&gamevp,sh+pobj->state,pobj->x+6,pobj->y+8);
				return (0);
				};
			if ((pobj->statecount!=0)&&(pobj->state!=-1)) {
				drawcell (xc,yc-1); drawcell (xc,yc+1);
				drawshape (&gamevp,info[stldr1].sh,pobj->x,
					pobj->y-16-pobj->statecount);
				drawshape (&gamevp,info[stldr3].sh,pobj->x,
					pobj->y+16+pobj->statecount);
				drawcell (xc,yc-2); drawcell (xc,yc+2);
				return (0);
				};
			if (designflag) drawshape (&gamevp,0x015f,pobj->x+4,pobj->y+4);
			break;
		case msg_trigger:
			if (pobj->statecount!=0) return (0);
			if (board(xc,yc)==mpgate) {
				if (takeinv (inv_mapkey)) {
					snd_play (3,snd_gate);
					txt ("THE GATE OPENS",2,0);
					setboard (xc,yc,board(xc+pobj->xd,yc+pobj->yd));
					killobj (n);
					if (objs[z].objkind==obj_pad) killobj (z); return (1);
					}
				else txt ("YOU NEED A GATE KEY TO PASS",5,0); break;
				};
			switch (pobj->state) {
				case 0: if (takeinv (inv_key0)) pobj->substate=1; break;
				case 1: if (takeinv (inv_key1)) pobj->substate=1; break;
				case 2: if (takeinv (inv_key2)) pobj->substate=1; break;
				case 3: if (takeinv (inv_key3)) pobj->substate=1;
				};
			if (pobj->substate==1) {
				txt ((char*)openmsg[random(4)],2,0);
				snd_play (4,snd_opendoor);
				pobj->statecount=1;
				for (c=0; c<=1; c++)	setboard(xc,yc+c,board(xc-1,yc+c));
				setboard(xc,yc-1,board(xc-1,yc-1));
				if (objs[z].objkind==obj_pad) killobj (z); return (1);
				}
			else txt ((char*)doormsg[random(4)],5,0);
		}; return (0);
	};

int msg_centexpl (int n, int msg, int z) {				// for centipede only
	int sh=kindtable[obj_centexpl]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((++pobj->statecount>=50)||(!onscreen(n))) killobj (n);
			if (++pobj->yd>12) pobj->yd=12;
			pobj->state=random(6)+((pobj->xd>0)?9:1);
			if (!trymovey(n,pobj->x+pobj->xd,pobj->y+pobj->yd)) {
				pobj->yd=-pobj->yd; snd_play (1,snd_hopperland);
				}; return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->state,pobj->x,pobj->y); break;
		case msg_touch: if (z!=0) return (0);
			snd_play (3,snd_emeralds);
			addobj (obj_flash,pobj->x-3,pobj->y-1,0,1);
			playerkill (n); return (1);
		}; return (0);
	};

int msg_addstep (int n, int msg, int z) {
	int xc,yc;
	objtype *pobj; pobj=&(objs[n]);
	xc=pobj->x/16; yc=pobj->y/16;

	switch (msg) {
		case msg_update:
			if (pobj->state==0) return (0);
			switch (pobj->xd) {
				case 1:
					setboard (xc,yc,s1l);
					setboard (xc+1,yc,s1r); break;
				case 2:
					setboard (xc,yc,s2);
					setboard (xc,yc+1,s2u); break;
				case 3:
					setboard (xc,yc,s3); break;
				case 4:
					setboard (xc,yc,s3);
					setboard (xc+1,yc,s3);
					setboard (xc,yc+1,c2);
					setboard (xc+1,yc+1,c2);
					setboard (xc+2,yc,c8);
					setboard (xc+2,yc+1,c8); break;
				case 6:
					setboard (xc,yc,s6l);
					setboard (xc+1,yc,s6r); break;
				case 7:
					setboard (xc,yc,platl);
					setboard (xc+1,yc,platr);
				}; killobj (n); break;
		case msg_draw:
			if (designflag) {
				drawshape (&gamevp,0x013f,pobj->x+4,pobj->y+4);
				}; break;
		case msg_trigger:
			pobj->state=1;
			snd_play (4,snd_switchon);
			if (objs[z].objkind==obj_pad) {
				addobj (obj_flash,objs[z].x-1,objs[z].y-3,0,0);
				killobj (z); return (1);
				};
		}; return (0);
	};

int msg_pad (int n, int msg, int z) {
	int sh=kindtable[obj_pad]*256;
	int ourmsg;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->xd==0) return (0);
			if (++pobj->statecount>15) pobj->statecount=0;
			return ((pobj->statecount&1)==0);
		case msg_draw:
			if (pobj->xd==1) drawshape (&gamevp,sh+pobj->statecount/4,
				pobj->x,pobj->y);
			else if (designflag) drawshape (&gamevp,0x0140,pobj->x+4,pobj->y+4);
			break;
		case msg_touch:
			if (z==0) {
				if (pobj->zaphold==0) {
					if (pobj->state==-1) ourmsg=msg_trigoff;
					else if (pobj->state==1) ourmsg=msg_trigon;
					else ourmsg=msg_trigger;
					sendtrig (pobj->counter,ourmsg,n);
					}; pobj->zaphold=3; return (1);
				};
		}; return (0);
	};

int msg_spring (int n, int msg, int z) {
	int sh=kindtable[obj_spring]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->statecount>0) pobj->statecount--; return (1); break;
		case msg_draw:
			if (pobj->statecount>6) sh+=1;
			else if (pobj->statecount>3) sh+=3;
			else sh+=2;
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if ((objs[z].objkind==obj_player)&&(pobj->zaphold==0)) {
				if ((objs[0].yd>0)&&((objs[0].y+objs[0].yl)>=(pobj->y+6))) {
					objs [0].state=st_jumping;
					objs [0].y-=6;
					objs [0].yd=-(16+4*pobj->counter);
					if (pobj->xd==1) {
						if (pobj->counter++>=4) pobj->counter=0;
						};
					objs [0].xd=dx1;
					objs [0].substate=0;
					snd_play (1,snd_spring);
					pobj->zaphold=5;
					pobj->statecount=10; return (1);
					};
				};
		}; return (0);
	};

int msg_arrow (int n, int msg, int z) {
	int sh=kindtable[obj_arrow]*256+17;
	const int ydtab[4]={1,0,-1,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (!onscreen(n)) {killobj (n); return (1);};
			pobj->counter=(pobj->counter+1)&7;
			if (pobj->xd>0) pobj->xd++;
			else pobj->xd--; 
			if (!justmove (n,pobj->x+pobj->xd,pobj->y+
				ydtab[pobj->counter/2])) killobj (n); return (1);
		case msg_draw:	drawshape (&gamevp,sh+pobj->yd,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				hitplayer (n,0); killobj (n); return (1);
				};
		}; return (0);
	};

int msg_masher (int n, int msg, int z) {
	int mod1=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			switch (pobj->state) {
				case 1:
					pobj->counter=0;
					pobj->yl=4; mod1=1; break;
				case 20:
					pobj->counter=1;
					pobj->yl=12; mod1=1; break;
				case 22:
					pobj->counter=2;
					pobj->yl=20;
					snd_play (1,snd_masher); mod1=1; break;
				case 24:
					pobj->state=0;
					pobj->counter=1;
					pobj->yl=12;
				};	pobj->state++; return (mod1);
		case msg_draw:
			drawshape (&gamevp,kindtable[obj_masher]*256+4+pobj->counter,
				pobj->x,pobj->y); break;
		case msg_touch: if (z==0) hitplayer (n,0); return (1);
		}; return (0);
	};

int msg_platform (int n, int msg, int z) {
	int landy=objs[0].y+objs[0].yl;
	objtype *pobj,*obj0;
	pobj=&(objs[n]); obj0=&(objs[0]);

	switch (msg) {
		case msg_update:
			pobj->statecount=(pobj->statecount+1)&7;
			if (pobj->state!=1) {						// platform moves
				if ((pobj->yd<0)&&(pobj->substate==st_platform)) {	// going up
					if (cando(0,obj0->x,pobj->y-42,
						f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
						moveobj (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
						}
					else {
						pobj->xd=-pobj->xd;
						pobj->yd=-pobj->yd;
						if (cando(0,obj0->x,pobj->y-42,
							f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
							moveobj (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
							};
						};
					}
				else {
					if (cando(n,pobj->x+pobj->xd,pobj->y+pobj->yd,
						f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
						moveobj (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
						}
					else {
						pobj->xd=-pobj->xd;
						pobj->yd=-pobj->yd;
						if (cando (n,pobj->x+pobj->xd,pobj->y+pobj->yd,
							f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
							moveobj (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
							};
						};
					};
				};
			if (pobj->substate==st_platform) {
				if ((dy1>0)&&(fire2)) {
					fire2off=1;
					obj0->state=st_jumping;
					obj0->yd=0;
					obj0->substate=2;					// In flight
					obj0->xl=kindxl[obj_player];
					pobj->substate=st_stand;
					pobj->zaphold=10; return (1);
					};
				if (fire2) {
					fire2off=1;
					obj0->state=st_jumping;
					obj0->yd=-(16+4*invcount(inv_jump));
					obj0->substate=0;
					obj0->xd=dx1;
					obj0->xl=kindxl[obj_player];
					snd_play (2,snd_jump);
					pobj->substate=st_stand;
					pobj->zaphold=5; return (1);
					};
				if (dx1!=0) {							// face left or right
					obj0->xd=dx1;
					obj0->statecount=0;
					}
				else if (obj0->statecount>20) {
					obj0->xd=0;
//					obj0->info1=0;
					obj0->statecount=0;
					};
				obj0->x=pobj->x;
				obj0->y=pobj->y-obj0->yl;
				if (pobj->yd!=0) obj0->yd=0;
				else obj0->yd=dy1;
				if (pobj->yd<0) obj0->substate=0;
				else if (pobj->yd>0) obj0->substate=1;
				moveobj (0,obj0->x,obj0->y); 
			}; if (onscreen(n)) return (1); break;
		case msg_draw:
			drawshape (&gamevp,kindtable[obj_platform]*256+13+
				pobj->statecount/2,pobj->x,pobj->y); break;
		case msg_touch:
			if ((objs[z].objkind!=obj_player)||(pobj->zaphold!=0)
				||(pobj->substate==st_platform)
				||(obj0->state==st_platform)) return (0);
			if (landy>(pobj->y+pobj->yl+obj0->yd)) return (0);
			if ((first_platform)&&!macplay) {
				first_platform=0; dotextmsg (2,0);
				};
			pobj->substate=st_platform;
			obj0->state=st_platform;
			obj0->counter=6;
			obj0->xl=kindxl[obj_player]+3;
			snd_play (2,snd_land); return (1);
		case msg_trigger: if (pobj->state==-1) return (0);
			pobj->state=1-pobj->state; return (1);
		}; return (0);
	};

int msg_checkpt (int n, int msg, int z) {
	char temp[16];
	objtype *pobj,*obj0;	
	pobj=&(objs[n]); obj0=&(objs[0]);

	switch (msg) {
		case msg_update:
			if ((obj0->x<(pobj->x+pobj->xl))&&(pobj->x<(obj0->x+obj0->xl))&&
				(obj0->y<(pobj->y+pobj->yl))&&(pobj->y<(obj0->y+obj0->yl))) {
					pobj->xd=obj0->x;
					pobj->yd=obj0->y;			// Save player's coordinates
				};	break;
		case msg_draw:									// Invisible!
			if (designflag) {
				fontcolor (&gamevp,5,-1);
				itoa (pobj->counter,temp,10);
				wprint (&gamevp,pobj->x+4,pobj->y+4,1,temp);
				}; break;
		case msg_touch:
			if (z==0) {
				if ((pobj->state==2)&&(countobj(obj_token,1))) {
					txt ("FIND THE GATE KEY ON THIS LEVEL BEFORE YOU LEAVE",5,0);
					return (1);
					};
				if ((pobj->state==3)&&(countobj(obj_icons,0))) {
					txt ("YOU CAN'T LEAVE WITHOUT THE POWER OBJECT",5,0);
					return (1);
					};
				if (pobj->state==4) {					// end of game
					macrecend();
					gameover=2;
					return (0);
					};
				if (pl.level!=pobj->counter) {
					snd_play (5,snd_level);
					if (pobj->counter!=0) pl.level=pobj->counter;
					if (obj0->objkind==obj_tiny) moveobj (0,pobj->xd,pobj->yd);
					else {
						objs [0].x=pobj->x;
						objs [0].y=pobj->y-24;
						objs [0].state=st_begin;
						objs [0].statecount=0;
						};
					if (pobj->inside!=NULL) {
						strcpy (newlevel,pobj->inside);
						}; return (1);
				};
			};
		}; return (0);
	};

int msg_elev (int n, int msg, int z) {
	int sh=kindtable[obj_elev]*256;
	int xc,yc;
	objtype *pobj; pobj=&(objs[n]);
	xc=pobj->x/16; yc=pobj->y/16;

	switch (msg) {
		case msg_update:
			if (pobj->state>0) pobj->state--;
			if ((pobj->state==0)&&(board(xc,yc+1)==elevl)&&
				(pobj->counter!=-1)) {
				setboard(xc,yc+1,board(xc,yc));
				setboard(xc+1,yc+1,board(xc+1,yc));
				moveobj (n,pobj->x,pobj->y+16); return (1);
				};
			if ((pobj->info1)&&(board(xc,yc+1)==elevl)) {
				setboard(xc,yc+1,board(xc,yc));
				setboard(xc+1,yc+1,board(xc+1,yc));
				moveobj (n,pobj->x,pobj->y+16);
				justmove (0,objs[0].x,yc*16-8);
				pobj->info1=0; return (1);
				};	break;
		case msg_draw:	drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (objs[z].objkind==obj_player) {
				pobj->state=6;
				if (first_elev) {
					first_elev=0;
					txt ("Press UP/DOWN to activate TurboLift",7,0);
					};
				if (dy1<0) {
//					objs[z].yd=0;
					if (pobj->substate!=dy1) snd_play (2,snd_switchoff);
					if (justmove(0,objs[0].x,(yc-2)*16-8)) {
						moveobj (n,pobj->x,pobj->y-16);
						setboard(xc,yc,elevl);
						setboard(xc+1,yc,elevr);
						};
					}
				else if (dy1>0) {
//					objs[z].yd=0;
					if (pobj->substate!=dy1) snd_play (2,snd_switchon);
					if (board(xc,yc+1)==elevl) pobj->info1=1;
					};
				pobj->substate=dy1; return (1);
			};
		}; return (0);
	};

int msg_mapdemo (int n, int msg, int z) {
	int sh=0x4000+kindtable[obj_mapdemo]*256;
	int nshofs[2]={34,38};
	int nshlen[2]={4,3};
	int c;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (!designflag) {
				pobj->x=gamevp.vpox+16+scrollxd;
				pobj->y=gamevp.vpoy+10+scrollyd;
				return (1);
				}; break;
		case msg_draw:
			if (pobj->info1==0) {
				pobj->info1=1;
				pobj->x+=pobj->yd;
				};
			for (c=0; c<nshlen[pobj->xd]; c++) {
				drawshape (&gamevp,sh+nshofs[pobj->xd]+c,pobj->x+c*16,pobj->y);
				};
		}; return (0);
	};

int msg_key (int n, int msg, int z) {
	int sh=kindtable[obj_key]*256;
	const char *inv_keymsg[4]={
		"You found a YELLOW key!",
		"You found a GREEN key!",
		"You found a RED key!",
		"You found a BLUE key!"
		};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (++pobj->counter>=16) pobj->counter=0;
			return ((pobj->counter&1)==0);
		case msg_draw:
			drawshape (&gamevp,sh+(pobj->state*8)+pobj->counter/2,pobj->x,
				pobj->y); break;
		case msg_touch:
			if (z!=0) return (0);
			switch (pobj->state) {
				case 0: if (!invcount(inv_key0)) {
					addinv (inv_key0); pobj->zaphold=1;
					}; break;
				case 1: if (!invcount(inv_key1)) {
					addinv (inv_key1); pobj->zaphold=1;
					}; break;
				case 2: if (!invcount(inv_key2)) {
					addinv (inv_key2); pobj->zaphold=1;
					}; break;
				case 3: if (!invcount(inv_key3)) {
					addinv (inv_key3); pobj->zaphold=1;
					};
				};
			if (pobj->zaphold==1) {
				addobj (obj_flash,pobj->x-3,pobj->y-2,0,0);
				killobj (n); snd_play (3,snd_getkey);
				txt ((char*)inv_keymsg[pobj->state],3,0); return (1);
				}
			else txt ("You already have a key of that COLOR!",5,0);
		}; return (0);
	};

int msg_heart (int n, int msg, int z) {
	int sh=kindtable[obj_heart]*256;
	const int heartbeat[4]={33,34,35,34};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: pobj->counter=(pobj->counter+1)&7;
			return ((pobj->counter&1)==0);
		case msg_draw:
			drawshape (&gamevp,sh+heartbeat[pobj->counter/2],
				pobj->x,pobj->y); break;
		case msg_touch:
			if (z!=0) return (0);
			if (pl.health<5) {
				if (!invcount(inv_invin)) pl.ouched=-4;
				pl.health++; statmodflg|=mod_screen;
				};
			addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
			playerkill (n);
			snd_play (3,snd_getheart);
			if (first_heart) {
				first_heart=0; txt ("HEARTS GIVE YOU HEALTH",7,0);
				}; return (1);
			}; return (0);
		};

int msg_emerald (int n, int msg, int z) {
	int sh=kindtable[obj_emerald]*256+28;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: if (++pobj->counter>=6) pobj->counter=0;
			return ((pobj->counter&1)==0);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2,pobj->x,pobj->y); break;
		case msg_touch:
			if (z!=0) return (0);
			if (pl.emeralds<99) pl.emeralds++;
			snd_play (3,snd_emeralds);
			addobj (obj_flash,pobj->x+1,pobj->y-2,0,1);
			playerkill (n);
			if ((first_emerald)&&!macplay) {
//				txt ("EMERALDS CAN BE USED TO BUY ITEMS",7,0);
				first_emerald=0; dotextmsg (39,0);
				}; return (1);
			}; return (0);
		};

int msg_eagle (int n, int msg, int z) {
	int sh=kindtable[obj_eagle]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->statecount=(pobj->statecount+1)&15;
			if (pobj->info1<2) return (0);
			if (pobj->substate<pobj->yd) {
				switch (pobj->substate) {
					case 0: dialogmsg (pobj->state,0); break;
					case 1: dialogmsg (pobj->state+1,1); break;
					case 2: dialogmsg (pobj->state+2,0); break;
					case 3: dialogmsg (pobj->state+3,1); break;
					case 4: dialogmsg (pobj->state+4,0); break;
					case 5: dialogmsg (pobj->state+5,1); break;
					}; pobj->substate++;
				};
			if (pobj->substate==pobj->yd) {
				if ((!justmove(n,pobj->x+pobj->xd,pobj->y-1))||(!onscreen(n))) {
					killobj (n); return (1);
					};
				}; return ((pobj->statecount&1)==0);
		case msg_draw:
			if ((designflag)||(pobj->info1>0)) {
				drawshape (&gamevp,sh+pobj->statecount/2,pobj->x,pobj->y);
				}; break;
		case msg_touch:
			if ((pobj->info1==0)||(pobj->info1==2)||(z!=0)) return (0);
			pobj->info1=2; return (1);
		case msg_trigger:
			snd_play (4,snd_geticon);
			addobj (obj_hithero,pobj->x+5,pobj->y+2,0,0);
			addobj (obj_flash,pobj->x-8,pobj->y-4,0,0);
			addobj (obj_flash,pobj->x,pobj->y+12,0,0);
			addobj (obj_flash,pobj->x+12,pobj->y+6,0,0);
			pobj->info1=1; if (objs[z].objkind==obj_pad) killobj (z);
			return (1);
		}; return (0);
	};

int msg_epic (int n, int msg, int z) {
	int sh=kindtable[obj_epic]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			drawshape (&gamevp,sh+(gamecount&7),pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				addscore (25,pobj->x,pobj->y);
				if (++pobj->state>12) {
					explode1 (pobj->x,pobj->y,5,0);
					killobj (n); snd_play (3,snd_stalagcrash);
					}
				else snd_play (3,snd_getfruit); return (1);
				};
			}; return (0);
		};

int msg_box (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->yd==-1) {
				pobj->substate=1; pobj->yd=0; return (0);
				};
			if (pobj->substate==0) return (0);
			if (cando (n, objs [n].x, pobj->y+1,
				f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
				if ((pobj->yd++)>9) pobj->yd=9;
				if (trymove(n,pobj->x,pobj->y+pobj->yd)!=1) {
					trymove(n,pobj->x,((pobj->y+pobj->yd-1)&0xfff0)+
					(16-kindyl[obj_box]));
					}; return (1);
				}
			else {
				if (pobj->yd!=0) snd_play (2,snd_bouldland);
				pobj->yd=0;
				}; break;
		case msg_draw:
			if (designflag) {
				drawshape (&gamevp,0x017f,pobj->x+4,pobj->y+4);
				}
			else if (pobj->substate==1) {
				drawshape (&gamevp,kindtable[obj_box]*256+24+pobj->xd,
					pobj->x,pobj->y);
				};	break;
		case msg_touch:
			if ((z==0)&&(first_box)&&(pobj->substate==1)) {
				if (!macplay) {
					first_box=0; dotextmsg (3,0);
					};
				};
			if ((kindflags[objs[z].objkind]&f_weapon)&&(pobj->substate==1)) {
				switch (pobj->state) {
					case 0: addobj (obj_heart,pobj->x,pobj->y,0,0); break;
					case 1: addobj (obj_bonus,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=2; break;		// grapes
					case 2: addobj (obj_bonus,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=6; break;		// cherries
					case 3: addobj (obj_bonus,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=8; break;		// stawberry
					case 4: addobj (obj_bonus,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=14; break;		// orange
					case 5: addobj (obj_bonus,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=3; break;		// 1.44 disk
					case 6: addobj (obj_key,pobj->x,pobj->y,0,0); break;
					case 7:
						addobj (obj_key,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=1; break;
					case 8:
						addobj (obj_key,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=2; break;
					case 9:
						addobj (obj_key,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=3; break;
					case 10:
						addobj (obj_token,pobj->x,pobj->y,0,0);
						objs[numobjs-1].state=8; break;
					case 11: addobj (obj_emerald,pobj->x,pobj->y,0,0); break;
					case 12:
						snd_play (4,snd_enemykill1);
						addobj(obj_fire,pobj->x-32,pobj->y-16,0,0);
						addobj(obj_fire,pobj->x-16,pobj->y-16,0,0);
						addobj(obj_fire,pobj->x,pobj->y-16,0,0);
						addobj(obj_fire,pobj->x+16,pobj->y-16,0,0);
						addobj(obj_fire,pobj->x+32,pobj->y-16,0,0); break;
					case 13: break;
					};
				snd_play (3,snd_stalagcrash);
				explode1 (pobj->x,pobj->y,4,0);
				if (pobj->yd!=0) addscore (50,pobj->x,pobj->y);
				killobj (n);
				if (objs[z].objkind!=obj_fireball) killobj (z); return (1);
				}; break;
		case msg_trigger: 
			pobj->substate=1;
			if (objs[z].objkind==obj_pad) killobj (z); return (1);
		}; return (0);
	};

int msg_score (int n, int msg, int z) {
	int c,sh;
	char score[12];
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((--pobj->counter<0)||(!onscreen(n))) killobj (n);
			else {
				pobj->x+=pobj->xd; pobj->y+=pobj->yd;
				pobj->yd--;	return (1);
				}; break;
		case msg_draw:
			fontcolor (&gamevp,2+(pobj->counter&3),-1);
			itoa (pobj->state,score,10);
			for (c=0;score[c];c++) {
//				wprint (&gamevp,pobj->x+8*c,pobj->y,1,score);
				drawshape (&gamevp,0x0300+score[c]-48,pobj->x+8*c,pobj->y);
				};
		}; return (0);
	};

int msg_token (int n, int msg, int z) {
	int sh=kindtable[obj_token]*256+15;
	const int ydtab[8]={1,1,0,0,-1,-1,0,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: if (pobj->state!=6) return (0);
			if (pobj->info1==0) {
				pobj->info1=1;
				pobj->xl=30; pobj->yl=38;
				};
			if (++pobj->statecount>=8) pobj->statecount=0;
			justmove(n,pobj->x,pobj->y+ydtab[pobj->statecount]); return (1);
		case msg_draw:
			if (pobj->state==6) {
				if (pobj->substate==0) {
					drawshape (&gamevp,kindtable[obj_heroswim]*256+21,
						pobj->x,pobj->y);
					};
				}
			else {
				drawshape (&gamevp,sh+inv_shape[pobj->state],pobj->x,pobj->y);
				}; break;
		case msg_touch:
			if ((z==0)&&(pobj->substate==0)) {
				if (inv_xfm[pobj->state]) {
					if (pobj->state==6) {
						pobj->substate=1; snd_play (5,snd_splash);
						}
					else if (pobj->state==5) {
						pobj->substate=1; snd_play (5,snd_bee);
						}
					else if (pobj->state==0) {
						pobj->substate=1; snd_play (5,snd_stalagcrash);
						};
					playerxfm (pobj->state);
					}
				else {
//					if (inv_first[pobj->state]) {
//						inv_first[pobj->state]--;
//						txt ((char*)inv_getmsg[pobj->state],3,0);
//						};
					txt ((char*)inv_getmsg[pobj->state],3,0);
					snd_play (3,snd_gettoken);
					addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
					if (pobj->state==4) {
						if (invcount(inv_laser)<5) {
							addinv (inv_laser);
							setcolor (162,23,13,33); setcolor (163,29,19,39);
							setcolor (164,35,25,45); setcolor (165,42,32,51);
							setcolor (166,49,40,57); setcolor (167,57,50,63);
							};
						}
					else addinv (pobj->state);
					killobj(n);
					}; statmodflg|=mod_screen; return (1);
				}; break;
		case msg_trigger: pobj->substate=0;
		}; return (0);
	};

int msg_switch (int n, int msg, int z) {
	int sh=kindtable[obj_switch]*256+23;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			drawshape (&gamevp,sh+pobj->state,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
//				if (dy1!=0) objs[z].yd=0;
				if (pobj->zaphold==0) {
					pobj->zaphold=25;
					txt ("Press UP/DOWN to toggle switch",7,0);
					};
				if ((dy1<0)&&(pobj->state==1)) {
					pobj->state=0; snd_play (2,snd_switchon);
					if (pobj->xd==1) sendtrig (pobj->counter,msg_trigger,n);
					else sendtrig (pobj->counter,msg_trigon,n); return (1);
					}
				else if ((dy1>0)&&(pobj->state==0)) {
					pobj->state=1; snd_play (2,snd_switchoff);
					if (pobj->xd==1) sendtrig (pobj->counter,msg_trigger,n);
					else sendtrig (pobj->counter,msg_trigoff,n); return (1);
					};
				};
		}; return (0);
	};

int msg_button (int n, int msg, int z) {
	int sh=kindtable[obj_button]*256+9;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			drawshape (&gamevp,sh-pobj->state,pobj->x+8,pobj->y); break;
		case msg_touch:
			if (pobj->zaphold==0) {
				pobj->state=1-pobj->state;
				if (pobj->state==1) {
					sendtrig(pobj->counter,msg_trigoff,n);
					snd_play (2,snd_switchoff);
					}
				else {
					sendtrig (pobj->counter,msg_trigon,n);
					snd_play (2,snd_switchon);
					};
				}; pobj->zaphold=10; return (1);
		}; return (0);
	};

int msg_poker (int n, int msg, int z) {
	int sh=kindtable[obj_poker]*256+13;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->statecount==0) return (0);
			pobj->statecount--; return (1);
		case msg_draw:
			drawshape (&gamevp,sh+(pobj->statecount==0?0:1),
				pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				if (pobj->state==0) hitplayer (n,0);
				else p_ouch (5,die_ash);
				pobj->statecount=15; return (1);
				}; break;
		case msg_trigger:	pobj->statecount=15;
		}; return (0);
	};

int msg_bullet (int n, int msg, int z) {
	int sh=kindtable[obj_bullet]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state==1) {
				pobj->state=2;
				pobj->xl=6; pobj->yl=4;
				};
			pobj->counter=(pobj->counter+1)&7;
			pobj->x+=pobj->xd; pobj->y+=pobj->yd;
			if (pobj->xd>12) pobj->xd=12;
			if (pobj->xd<-12) pobj->xd=-12;
			if ((!onscreen(n))||((pobj->xd==0)&&(pobj->yd==0))) {
				killobj (n); return (1);
				};
			if (!justmove (n,pobj->x+pobj->xd,pobj->y+pobj->yd)) killobj (n);
			return (1);
		case msg_draw:
			if (pobj->state==0) drawshape (&gamevp,sh+39,pobj->x,pobj->y);
			else drawshape (&gamevp,sh+41+pobj->counter/2,pobj->x,pobj->y);
			break;
		case msg_touch:
			if (objs[z].objkind==obj_heroswim) {
				p_ouch(1,die_fish);
				addobj (obj_hithero,objs[0].x,objs[0].y,0,0);
				killobj (n); return (1);
				};
			if (z==0) {hitplayer (n,0); killobj (n); return (1);}
		}; return (0);
	};

int msg_bonus (int n, int msg, int z) {
	int sh=kindtable[obj_bonus]*256;
	int cnt;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			drawshape (&gamevp,sh+pobj->state,pobj->x+((pobj->xd==1)?4:0),
				pobj->y); break;
		case msg_touch:
			if (z!=0) return (0);
			switch (pobj->state) {
				case 1:
					while (invcount(inv_laser)<5) addinv (inv_laser);
					setcolor (162,32,0,0); setcolor (163,38,0,0);
					setcolor (164,44,0,0); setcolor (165,50,0,0);
					setcolor (166,56,0,0); setcolor (167,63,0,0);
					kindscore[pobj->objkind]=100;
					txt ("You now have RAPID FIRE",3,0);
					snd_play (3,snd_gettoken); break;
				case 2:
					if ((first_fruit)&&!macplay) {
						first_fruit=0; dotextmsg (38,0);
						};
					if (cnt_fruit<16) cnt_fruit++;
					kindscore[pobj->objkind]=100; icon_fruit=0;
					snd_play (3,snd_getfruit); statmodflg|=mod_screen; break;
				case 3: kindscore[pobj->objkind]=120;
					snd_play (3,snd_bonus); break;
				case 6:
					if ((first_fruit)&&!macplay) {
						first_fruit=0; dotextmsg (38,0);
						};
					if (cnt_fruit<16) cnt_fruit++;
					kindscore[pobj->objkind]=100; icon_fruit=1;
					snd_play (3,snd_getfruit); statmodflg|=mod_screen; break;
				case 7: kindscore[pobj->objkind]=200;
					snd_play (3,snd_bonus2); break;
				case 8:
					if ((first_fruit)&&!macplay) {
						first_fruit=0; dotextmsg (38,0);
						};
					if (cnt_fruit<16) cnt_fruit++;
					kindscore[pobj->objkind]=100; icon_fruit=2;
					snd_play (3,snd_getfruit); statmodflg|=mod_screen; break;
				case 9:
					if (invcount(inv_epic)==0) addinv (inv_epic);
					addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
					kindscore[pobj->objkind]=100;
					txt ("HINT: Collect EPIC poolballs in order for bonus",3,0);
					snd_play (3,snd_getheart);	break;
				case 10:
					if (invcount(inv_epic)==1) addinv (inv_epic);
					addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
					kindscore[pobj->objkind]=100;
					snd_play (3,snd_getheart); break;
				case 11:
					if (invcount(inv_epic)==2) addinv (inv_epic);
					addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
					kindscore[pobj->objkind]=100;
					snd_play (3,snd_getheart);	break;
				case 12:
					if (invcount(inv_epic)==3) {
						addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
						kindscore[pobj->objkind]=4000;
						txt ("Special EPIC bonus: 4,000 Points!",3,0);
						snd_play (3,snd_getkey);
						}
					else {
						addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
						kindscore[pobj->objkind]=100; snd_play (3,snd_getheart);
						}; break;
				case 13:								// Journal Entries
					dotextmsg (pobj->counter,dialog); killobj (n); return (0);
				case 14:
					if ((first_fruit)&&!macplay) {
						first_fruit=0; dotextmsg (38,0);
						};
					if (cnt_fruit<16) cnt_fruit++;
					kindscore[pobj->objkind]=100; icon_fruit=3;
					snd_play (3,snd_getfruit); statmodflg|=mod_screen; break;
				case 18: kindscore[pobj->objkind]=75;
					snd_play (3,snd_bonus); break;
				case 19:	kindscore[pobj->objkind]=2000;
					snd_play (3,snd_getkey); break;
				case 23:	kindscore[pobj->objkind]=50;
					snd_play (3,snd_bonus2); break;
				case 28:
					for (cnt=0; cnt<5; cnt++) {
						if (invcount(inv_fire)<9) addinv (inv_fire);
						};
					kindscore[pobj->objkind]=1000;
					txt ("You now have 5 FIREBALLS--Use them wisely",3,0);
					snd_play (3,snd_gettoken); break;
				default:	kindscore[pobj->objkind]=25; snd_play (3,snd_bonus);
				};
			if ((pobj->state<9)||(pobj->state>12))
				addobj (obj_flash,pobj->x+((pobj->xd==1)?5:1),pobj->y,0,1);
			playerkill (n); return (1);
		}; return (0);
	};

int msg_rock (int n, int msg, int z) {
	int sh=kindtable[obj_rock]*256+20;
	int c,obj,closeobj,objdist,dist;
	int ax=0; int ay=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
//			pobj->counter=(pobj->counter+sgn(pobj->xd))&3;
			if ((++pobj->substate>=64)||(!onscreen(n))) {
				killobj (n); return (1);
				};
			closeobj=-1; objdist=32767;
			for (c=0; c<numscrnobjs; c++) {
				obj=scrnobjs[c];
				if ((kindflags[objs[obj].objkind]&f_killable)&&(obj!=0)) {
					dist=vectdist(n,obj);
//					dist=(abs(objs[n].x-objs[obj].x)+abs(objs[n].y-objs[obj].y));
					if ((dist<objdist)&&(dist<96)) {
						closeobj=obj;
						objdist=dist;
						};
					};
				};
			if (closeobj>=0) pointvect (closeobj,n,&ax,&ay,3);
			ay=1;	pobj->xd+=ax;
			if (pobj->xd>12) pobj->xd=12;
			if (pobj->xd<-12) pobj->xd=-12;
			pobj->yd+=ay;
			if (pobj->yd>12) pobj->yd=12;
			if (pobj->yd<-12) pobj->yd=-12;
			pobj->substate+=(trybreakwall
				(n,pobj->x+pobj->xd,pobj->y+pobj->yd)*10);
			if (!justmove(n,pobj->x+pobj->xd,pobj->y)) pobj->xd=-pobj->xd;

			c=((pobj->y+pobj->yd)&0xfff0)+16-kindyl[obj_rock];
			if (!justmove(n,pobj->x,pobj->y+pobj->yd)) {
				snd_play (1,snd_rockbounce);
				if ((c==pobj->y)||(!justmove(n,pobj->x,c))) pobj->yd=-pobj->yd;
				};
			if (!justmove(n,pobj->x,pobj->y)) killobj (n); return (1);
		case msg_draw: drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if ((z==0)&&(pobj->substate>7)) {killobj (n); return (1);}
			else if (kindflags[objs[z].objkind]&f_killable) {
				killobj(n);
				switch (objs[z].objkind) {
					case obj_bat:
					case obj_hopper:
					case obj_bee:
						snd_play (4,snd_hopperdie);
						explode1 (pobj->x,pobj->y,4,1); break;
					case obj_skull:
						snd_play (4,snd_enemykill3);
						explode1 (objs[z].x,objs[z].y,5,1); break;
					default:
						snd_play (4,snd_enemykill1);
						explode1 (objs[z].x,objs[z].y,5,0);
					};
				playerkill(z); return (1);
				};
		}; return (0);
	};

int msg_fireball (int n, int msg, int z) {
	int sh=kindtable[obj_fireball]*256+34;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (!onscreen(n)) {killobj (n); return (1);};
			pobj->counter=(pobj->counter+1)&3;
			if (pobj->xd>0) pobj->xd++;
			else pobj->xd--;
			trybreakwall (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
			if (!justmove (n,pobj->x+pobj->xd,pobj->y+pobj->yd)) {
				addobj (obj_fireexpl,pobj->x+((pobj->xd>0)?12:-20),
					pobj->y-5,0,0);
				if ((board(pobj->x+pobj->xd,pobj->y+pobj->yd))!=breakwall) {
					snd_play (2,snd_hitwall); killobj (n);
					};
				};	return ((pobj->counter&1)==0);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2+((pobj->xd>0)?2:0),
				pobj->x,pobj->y); break;
		case msg_touch:
			if (kindflags[objs[z].objkind]&f_fireball) {
//				killobj (n);
				explode2 (z);
				playerkill(z);
				snd_play (4,snd_enemykill1);
				}
			else if (kindflags[objs[z].objkind]&f_killable) {
//				killobj (n);
				explode1 (objs[z].x,objs[z].y,5,0);
				switch (objs[z].objkind) {
					case obj_bat:
					case obj_hopper:
					case obj_bee: snd_play (4,snd_hopperdie); break;
					case obj_skull:
						snd_play (4,snd_enemykill3);
						explode1 (objs[z].x,objs[z].y,5,1); break;
					default: snd_play (4,snd_enemykill1);
					};
				playerkill(z); return (1);
				};
		}; return (0);
	};

int msg_laser (int n, int msg, int z) {
	int sh=kindtable[obj_laser]*256+38;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (!onscreen(n)) {
				killobj(n); return (1);
				};
			if (!justmove (n,pobj->x+pobj->xd,pobj->y)) {
				addobj (obj_hitwall,pobj->x+(pobj->xd>0?12:-12),pobj->y-5,0,0);
				snd_play (2,snd_hitwall);
				killobj(n); return (1);
				};
			if (pobj->xd>0) pobj->xd++;
			else pobj->xd--;
			trybreakwall (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
			justmove (n,pobj->x,pobj->y+(dy1*2)); return (1);
		case msg_draw:	drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (kindflags[objs[z].objkind]&f_killable) {
				killobj(n);
				switch (objs[z].objkind) {
					case obj_bat:
					case obj_hopper:
					case obj_bee:
						snd_play (4,snd_hopperdie);
						explode1 (pobj->x,pobj->y,4,1); break;
					case obj_skull:
						snd_play (4,snd_enemykill3);
						explode1 (objs[z].x,objs[z].y,5,1); break;
					default:
						snd_play (4,snd_enemykill1);
						explode1 (objs[z].x,objs[z].y,5,0);
					};
				playerkill(z); return (1);
				};
		}; return (0);
	};

int msg_torpedo (int n, int msg, int z) {
	int sh=kindtable[obj_torpedo]*256+40;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (!onscreen(n)) {
				killobj(n); return (1);
				};
			if (!justmove (n,pobj->x+pobj->xd,pobj->y)) {
				addobj (obj_hitwall,pobj->x+(pobj->xd>0?8:-12),pobj->y-4,0,0);
				snd_play (2,snd_hitwall);
				killobj(n); return (1);
				};
			if (pobj->xd>0) pobj->xd++;
			else pobj->xd--;
			justmove (n,pobj->x,pobj->y+(dy1*2)); return (1);
		case msg_draw:
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (kindflags[objs[z].objkind]&f_killable) {
				killobj(n);
				addobj (obj_flash,pobj->x+2,pobj->y,0,1);
				addobj (obj_flash,pobj->x+8,pobj->y+2,0,1);
				addobj (obj_bubble,pobj->x,pobj->y+1,0,0);
				addobj (obj_bubble,pobj->x+4,pobj->y+2,0,0);
				addobj (obj_bubble,pobj->x+6,pobj->y+4,0,0);
				snd_play (4,snd_fishdie); playerkill(z); return (1);
				};
		}; return (0);
	};

void explode1 (int x, int y, int num, int flg) {
	int c;

	for (c=0; c<num; c++) {
		if (flg) {							// pebbles
			addobj (obj_fragexpl,x,y,random(7)-3,random(11)-8);
			objs[numobjs-1].state=-1;
			}
		else {
			addobj (obj_fragexpl,x,y,random(7)-3,random(11)-8);
			objs[numobjs-1].state=random(5);
			};
		};
	};

void explode2 (int n) {
	objtype *pobj; pobj=&(objs[n]);

	addobj (obj_fireexpl,pobj->x+(pobj->xl-kindxl[obj_fireexpl])/2,
		pobj->y+pobj->yl-kindyl[obj_fireexpl],0,0);
	};

int msg_fragexpl (int n, int msg, int z) {				// fragments
	int sh=kindtable[obj_fragexpl]*256+50;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state==-1) {
				pobj->state=-2; pobj->yl=4;
				};
			if ((++pobj->statecount>=40)||(!onscreen(n))) killobj (n);
			if (++pobj->yd>12) pobj->yd=12;
			moveobj (n,pobj->x+pobj->xd,pobj->y+pobj->yd); return (1);
		case msg_draw:
			if (pobj->state>=0)
				drawshape (&gamevp,sh+pobj->state,pobj->x,pobj->y);
			else drawshape (&gamevp,0x2422,pobj->x,pobj->y);
		}; return (0);
	};

int msg_fireexpl (int n, int msg, int z) {				// fireball explosion
	int sh=kindtable[obj_fireexpl]*256+45;
	const int exptab[9]={0,1,0,1,0,1,2,3,4};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((++pobj->counter>=9)||(!onscreen(n))) killobj (n); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+exptab[pobj->counter],pobj->x,pobj->y);
		}; return (0);
	};

int msg_flash (int n, int msg, int z) {				// for bonus items
	int sh=kindtable[obj_flash]*256;
	const int expltab[16]={3,3,2,2,1,1,0,0,0,0,1,1,2,2,3,3};
	const int expltab2[16]={0,0,1,1,2,2,3,3,3,3,2,2,1,1,0,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((pobj->yd==1)&&(pobj->info1==0)) {
				pobj->info1=1; pobj->xl=14; pobj->yl=15;
				};
			if ((++pobj->counter>=16)||(!onscreen(n))) killobj (n); return (1);
		case msg_draw:
			if (pobj->yd==0) {
				drawshape (&gamevp,sh+59+expltab[pobj->counter],pobj->x,pobj->y);
				}
			else {
				drawshape (&gamevp,sh+68+expltab2[pobj->counter],pobj->x,pobj->y);
				};
		}; return (0);
	};

int msg_hitwall (int n, int msg, int z) {
	int sh=kindtable[obj_hitwall]*256+55;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((++pobj->counter>=7)||(!onscreen(n))) killobj (n); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+(pobj->counter/2),pobj->x,pobj->y);
		}; return (0);
	};

int msg_gems (int n, int msg, int z) {
	int sh=kindtable[obj_gems]*256+29;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			drawshape (&gamevp,sh+pobj->state,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				snd_play (3,snd_emeralds);
				addobj (obj_flash,pobj->x-1,pobj->y-1,0,1);
				playerkill (n); return (1);
				};
		}; return (0);
	};

int msg_spear (int n, int msg, int z) {
	int sh=kindtable[obj_spear]*256+7;
	int mod1=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			switch (pobj->statecount) {
				case 0:
					pobj->counter=0;
					pobj->yl=4;
					pobj->y+=pobj->yd*12; mod1=1; break;
				case 20:
					pobj->counter=1;
					pobj->yl=12;
					pobj->y-=pobj->yd*8; mod1=1; break;
				case 22:
					pobj->counter=2;
					pobj->yl=20;
					pobj->y-=pobj->yd*8;
					snd_play (1,snd_spikes); mod1=1; break;
				case 24:
					pobj->statecount=0;
					pobj->counter=0;
					pobj->yl=4;
					pobj->y+=pobj->yd*16; mod1=1;
					}; pobj->statecount++; return (mod1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter+pobj->yd*3,pobj->x,pobj->y);
			break;
		case msg_touch: if (z==0) hitplayer (n,pobj->state); return (1);
		}; return (0);
	};

int msg_stalag (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->yd!=0) {
				if (!trymovey(n,pobj->x,pobj->y+pobj->yd)) {
					snd_play (3,snd_stalagcrash);
					explode1 (pobj->x,pobj->y+10,5,1);
					killobj (n); return (1);
					};
				if ((pobj->yd+=2)>16) {pobj->yd=16; return (1);};
				}; break;
		case msg_draw:
			drawshape (&gamevp,0x4f02,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				if ((objs[0].yd<0)&&(pobj->yd==0)) {
					objs[0].y=pobj->y+pobj->yl;
					objs[0].state=st_jumping;
					objs[0].yd=0;
					objs[0].substate=2;
					};
				hitplayer (n,0); return (1);
				}; break;
		case msg_trigger:
			if (pobj->xd-->0) return (0);
			if (pobj->yd==0) {
				pobj->yd=2;
				snd_play (2,snd_rockbounce);
				killobj(z); return (1);
				};
		}; return (0);
	};

int msg_boulder (int n, int msg, int z) {
	int sh=kindtable[obj_boulder]*256+19;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=3) {
				snd_play (4,snd_enemykill3);
				explode1 (pobj->x,pobj->y,7,1);
				playerkill (n);
				};
			if (cando (n,pobj->x,pobj->y+1,
				f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
				if ((pobj->yd+=2)>12) pobj->yd=12;
				if (trymove(n,pobj->x+pobj->xd,pobj->y+pobj->yd)!=1) {
					trymove(n,pobj->x,((pobj->y+pobj->yd-1)&0xfff0)+
					(16-kindyl[obj_boulder]));
					pobj->xd=0;
					};
				}
			else {
				if (pobj->yd!=0) snd_play (2,snd_bouldland);
				pobj->yd=0;
				if (pobj->xd==0) {
					seekplayer (n,&pobj->xd,&pobj->yd);
					pobj->xd*=4;
					};
				pobj->counter=(pobj->counter+((pobj->xd>0)?1:-1))&3;
				pobj->yd=0;
				if (trymove(n,pobj->x+pobj->xd,pobj->y)!=1)
					pobj->xd=-pobj->xd;
				}; return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				snd_play (2,snd_hitwall);
				explode1 (pobj->x,pobj->y,5,1);
				killobj (z);
				pobj->state++; return (1);
				};
		}; return (0);
	};

int msg_ball (int n, int msg, int z) {
	int sh=kindtable[obj_ball]*256+4;
	int c,obj,closeboll,bolldist,dist;
	int ax; int ay=1;
	const int ltab[4]={12,14,16,16};
	objtype *pobj; pobj=&(objs[n]);
	ax=pobj->xd;

	switch (msg) {
		case msg_update:
			if (pobj->info1==0) {
				pobj->xl=ltab[pobj->counter];
				pobj->yl=ltab[pobj->counter];
				pobj->info1=1;
				};
			closeboll=-1; bolldist=96;

			for (c=0; c<numscrnobjs; c++) {
				obj=scrnobjs[c];
				if ((objs[obj].objkind==obj_ball)&&
					(objs[obj].counter>pobj->counter)) {
					dist=vectdist(n,obj);
					if (dist<bolldist) {
						closeboll=obj;
						bolldist=dist;
						};
					};
				};
			if (closeboll>=0) pointvect (closeboll,n,&ax,&ay,4);
			pobj->xd+=ax;
			if (pobj->xd>12) pobj->xd=12;
			if (pobj->xd<-12) pobj->xd=-12;
			pobj->yd+=ay;
			if (pobj->yd>12) pobj->yd=12;
			if (pobj->yd<-12) pobj->yd=-12;

			if (!justmove(n,pobj->x+pobj->xd,pobj->y)) pobj->xd=-pobj->xd;
			if (!justmove(n,pobj->x,pobj->y+pobj->yd)) {
				c=((pobj->y+pobj->yd)&0xfff0)+16-pobj->yl;
				if ((c==pobj->y)||(!justmove(n,pobj->x,c))) {
					if (pobj->state==1) pobj->yd=0;
					pobj->yd=-pobj->yd;
					if (pobj->yd!=0) snd_play (1,snd_ballbounce);
					};
				}; return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter,pobj->x,pobj->y); break;
		case msg_touch: 
			if (z==0) {
				hitplayer (n,0);
				if (pobj->state==1) {
					snd_play (4,snd_enemykill1);
					explode2 (n); killobj (n);
					}; return (1);
				};
		}; return (0);
	};

int msg_fire (int n, int msg, int z) {
	int sh=kindtable[obj_fire]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (((++pobj->counter)>=12)||(pobj->counter<0)) killobj (n);
			return((pobj->counter&1)==0);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2+(pobj->yd>0)*6,
				pobj->x,pobj->y); break;
		case msg_touch:
			if ((z==0)&&(pobj->state!=1)&&
				!(stateinfo[objs[0].state]&sti_invincible)) {
				p_ouch (2,die_ash); explode2(0); pobj->state=1;		// safe now
				return (1);
				};
		}; return (0);
	};

int msg_searock (int n, int msg, int z) {
	int sh=kindtable[obj_searock]*256+19;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (random(14)==0) {
				addobj (obj_bubble,pobj->x+2,pobj->y+4,0,0);	return (1);
				}; break;
		case msg_draw:
			if ((pobj->state==0)||(designflag)) {
				drawshape (&gamevp,sh,pobj->x,pobj->y);
				};
		}; return (0);
	};

int msg_torch (int n, int msg, int z) {
	int sh=kindtable[obj_torch]*256+12;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: pobj->counter=(pobj->counter+1)&7;
			return((pobj->counter&1)==0);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2,pobj->x,pobj->y);
		}; return (0);
	};

int msg_cloud (int n, int msg, int z) {
	int sh=kindtable[obj_cloud]*256+33;
	int destx;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: if (pobj->xd==0) return (0);
			pobj->counter=(pobj->counter+1)&15;
			if (pobj->counter&1) {
				destx=pobj->x+pobj->xd;
				if (!justmove (n,destx,pobj->y)) pobj->xd=-pobj->xd;
				}; return (1);
		case msg_draw: drawshape (&gamevp,sh,pobj->x,pobj->y);
		}; return (0);
	};

int msg_hithero (int n, int msg, int z) {				// when player gets hit
	int sh=kindtable[obj_hithero]*256+63;
	const int exptab[11]={4,4,3,2,1,0,1,2,3,4,4};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((++pobj->counter>=11)||(!onscreen(n))) killobj (n); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+exptab[pobj->counter],pobj->x,pobj->y);
		}; return (0);
	};

int msg_spikes (int n, int msg, int z) {
	int sh=kindtable[obj_spikes]*256;
	const int spiketab[8]={1,2,3,3,3,2,1,0};
	objtype *pobj; pobj=&(objs[n]);
	
	switch (msg) {
		case msg_update:
			if (pobj->info1==0) {
				if (++pobj->statecount==pobj->counter) {
					pobj->info1=1;
					pobj->statecount=0;
					}
				else return (1);
				};
			pobj->state=spiketab[pobj->substate];
			if (pobj->substate==3) snd_play (1,snd_spikes);
			if (pobj->state==0) {
				pobj->substate=0;
				pobj->info1=0;
				return (1);
				}; pobj->substate++; break;
		case msg_draw:
			if (designflag) {
				drawshape (&gamevp,0x015e,pobj->x+4,pobj->y+4);
				return (1);
				};
			sh+=(25+pobj->state+(pobj->yd*4));
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if ((z==0)&&(pobj->state>0)) hitplayer (n,0); return (1);
		}; return (0);
	};

int msg_transport (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:									// Invisible!
			if (designflag) {
				drawshape (&gamevp,0x013e,pobj->x+4,pobj->y+4);
				}; break;
		case msg_trigger:
			moveobj (0,pobj->x,pobj->y-24);
			setorigin(); moddrawboard();
			if (pobj->xd==1) killobj (n); return (1);
		}; return (0);
	};

int msg_transpad (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: if (pobj->state==0) return (0);
			pobj->state--; return (1);
		case msg_draw:
			if (designflag) {
				drawshape (&gamevp,0x013c,pobj->x+4,pobj->y+4);
				}; break;
		case msg_touch: if (z!=0) return (0);
			if ((pobj->counter==-1)&&(pobj->zaphold==0)) {
				txt ("This is a ONE-WAY door and cannot be entered",7,0);
				pobj->zaphold=25; return (0);
				};
			if (pobj->zaphold==0) {
				if (dy1!=0) objs[z].yd=0;
				if ((pobj->xd==0)&&(pobj->state==0)) {
					txt ("Press the UP arrow key to walk through doors",7,0);
					}
				else if ((pobj->xd==2)&&(pobj->state==0)) {
					txt ("Press the UP arrow key to go to the other side",3,0);
					};
				if ((dy1<0)||(pobj->xd==1)) {
					snd_play (2,snd_switchon);
					if (pobj->xd==1) {
						killobj (n);
						objs[0].statecount=16;
						}
					else {
						objs[0].statecount=0;
						pobj->zaphold=20;
						};
					objs[0].state=st_transport;
					objs[0].x=pobj->x;
					objs[0].y=pobj->y-8;
					objs[0].substate=pobj->counter;
					};
				}; pobj->state=3; return (1);
		}; return (0);
	};

int msg_effects (int n, int msg, int z) {		// yd FREE
	int c,q;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->statecount++==4) {	// alternate 2 colors
				pobj->statecount=0;
				if (pobj->substate==0) {
					pobj->substate=1;
					setcolor (249,32,0,0);
					if (invcount(inv_invin)) {
						for (c=207; c<234; c++) {	// change all blue to max
							setcolor (c,vgapal[c*3+0],vgapal[c*3+1],63);
							};
						if (pobj->info1++>=30) {
							pobj->info1=0; takeinv(inv_invin);
							pl.ouched=-1; statmodflg|=mod_screen;
							};
						}
					else if (pl.health==1) {
						for (c=207; c<234; c++) {	// change all red to max
							setcolor (c,63,vgapal[c*3+1],vgapal[c*3+2]);
							};
						}
					else if (invcount(inv_jump)) setcolor (233,0,15,52);
					}
				else {
					pobj->substate=0;
					setcolor (249,60,0,0);
					if ((pobj->info1>0)||(pl.health==1)) {
						pl.ouched=-1; statmodflg|=mod_screen;
						}
					else if (invcount(inv_jump))
						setcolor (233,8,8,8);		// change boots back to normal
					};
				};
			switch (pobj->counter) {
				case 0:									// levels without lava, water
					for (c=234; c<236; c++) {		// mines, xargbots
						q=234+(((c+gamecount)&3)/2);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						};
					for (c=236; c<240; c++) {		// S.U.P., turret, factory
						q=236+(((c+gamecount)&7)/2);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						}; break;
				case 1:									// for underwater only
					for (c=240; c<248; c++) {		// waterfalls, dungeon
						q=240+((c+gamecount)&7);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						};
					for (c=234; c<236; c++) {		// mines, xargbots
						q=234+(((c+gamecount)&3)/2);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						};
					for (c=236; c<240; c++) {		// S.U.P., turret, factory
						q=236+(((c+gamecount)&7)/2);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						}; break;
				case 2:									// levels with lava only
					for (c=192; c<200; c++) {		// flowing lava
						q=192+((c+gamecount)&7);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						};
					for (c=234; c<236; c++) {		// mines, xargbots
						q=234+(((c+gamecount)&3)/2);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						};
					for (c=236; c<240; c++) {		// S.U.P., turret, factory
						q=236+(((c+gamecount)&7)/2);
						setcolor (c,vgapal[q*3+0],vgapal[q*3+1],vgapal[q*3+2]);
						}; break;
				case 3: upd_colors ();				// updates all colors
				};
			if (pobj->state==-1) return (0);
			switch (pobj->state) {
				case 0: setcolor (250,0,0,0);			 		// black sky
					setcolor (251,0,0,0); pobj->state=-1; break;
				case 1:									// simulate lightning
					if (random (25)==0) {
						pobj->zaphold=3;
						setcolor (250,60,60,63);
						if (pobj->xd==0) setcolor (251,60,60,63);
//						snd_play (2,snd_thunder1+random(3));
						};
					if (pobj->zaphold==0) {
						pobj->zaphold=-1;
						setcolor (250,0,0,0); setcolor (251,0,0,0);
						}; break;
				case 2: setcolor (250,0,0,32);		 		// dk. blue sky
					if (pobj->xd==0) setcolor (251,0,0,32); break;
				case 3:												// lt. blue sky
					setcolor (176,8,16,25); setcolor (177,8,20,29);
					setcolor (178,12,24,33); setcolor (179,16,28,41);
					setcolor (180,20,32,45); setcolor (181,24,40,49);
					setcolor (182,28,44,57); setcolor (183,36,48,60);
					if (pobj->yd==0) setcolor (250,36,48,60);
					if (pobj->xd==0) setcolor (251,36,48,60); break;
				case 4:												// yellow sky
					setcolor (176,32,0,0); setcolor (177,40,0,0);
					setcolor (178,52,0,0); setcolor (179,60,0,0);
					setcolor (180,60,28,0); setcolor (181,60,40,0);
					setcolor (182,60,52,0); setcolor (183,60,60,0);
					if (pobj->yd==0) setcolor (250,60,60,0);
					if (pobj->xd==0) setcolor (251,60,60,0); break;
				case 5:												// emerald sky
					setcolor (176,0,12,12); setcolor (177,0,18,17);
					setcolor (178,0,25,23); setcolor (179,0,32,27);
					setcolor (180,0,39,32); setcolor (181,0,46,35);
					setcolor (182,0,53,38); setcolor (183,0,60,40);
					if (pobj->yd==0) setcolor (250,0,60,40);
					if (pobj->xd==0) setcolor (251,0,60,40); break;
				case 6: setcolor (250,32,32,24);				// olive green
					if (pobj->xd==0) setcolor (251,32,32,24); break;
				case 7:												// violet sky
					setcolor (176,13,5,22); setcolor (177,18,8,27);
					setcolor (178,23,13,33); setcolor (179,29,19,39);
					setcolor (180,35,25,45); setcolor (181,42,32,51);
					setcolor (182,49,40,57); setcolor (183,57,50,63);
					if (pobj->yd==0) setcolor (250,57,50,63);
					if (pobj->xd==0) setcolor (251,57,50,63); break;
				case 8: setcolor (250,23,23,23);				// factory grey
					if (pobj->xd==0) setcolor (251,23,23,23); break;
				case 9: setcolor (250,12,23,63);		 		// royal blue
					if (pobj->xd==0) setcolor (251,12,23,63); break;
				case 10: setcolor (250,20,20,23);			// factory grey v3
					if (pobj->xd==0) setcolor (251,20,20,23); break;
				}; if (pobj->state>1) pobj->state=-1; break;
		case msg_draw:
			if (designflag) drawshape (&gamevp,0x017e,pobj->x+4,pobj->y+4);
		}; return (0);
	};

int msg_bubble (int n, int msg, int z) {
	int sh=kindtable[obj_bubble]*256+16;
	int c;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (random(15)==0) pobj->counter++;
			if ((pobj->counter>2)||(!onscreen(n))) killobj (n);
			else {
				if (!fishdo (n,pobj->x+random(3)-1,
					pobj->y-pobj->counter-1)) killobj(n);
				}; return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter,pobj->x,pobj->y);
		}; return (0);
	};

int msg_txtmsg (int n, int msg, int z) {
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			if (designflag) {
				drawshape (&gamevp,0x0121,pobj->x+4,pobj->y+4);
				}; break;
		case msg_touch:
			if (z==0) {
				if (pobj->state==0)
					txt ("Press UP arrow key to read the sign",7,0);
				if ((dy1<0)||(pobj->state==1)) {
					dotextmsg (pobj->counter,0);
					if (pobj->state==1) killobj (n);
					}; return (1);
				};
		}; return (0);
	};

int msg_front (int n, int msg, int z) {
	int sh=kindtable[obj_front]*256;
	int nl,flash;
	int mod1=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: if ((pobj->state<13)||(pobj->state>14)) return (0);
			if (pobj->state==13) {
				if ((pobj->info1>=3)&&(random (abs(pobj->info1-42))==0)) {
					addobj(obj_fire,pobj->x+random(36),pobj->y+random(8),0,0);
					objs[numobjs-1].state=1; mod1=0;
					};
				}
			else {
				if ((pobj->info1>=3)&&(random(abs(pobj->info1-42))==0)) {
					addobj(obj_fire,pobj->x+random(56),pobj->y+random(32),0,0);
					objs[numobjs-1].state=1; mod1=0;
					};
				};
			if (pobj->info1>=40) {						// reactor destroyed
				pobj->substate=(pobj->substate+1)&7;
				if (pobj->substate==0) dim(); else undim();
				if (pobj->counter==2) setcolor (250,20,20,23);
				explode1 (pobj->x+random(36+((pobj->state==14)?20:0)),
					pobj->y+random(36+((pobj->state==14)?20:0)),1,1);
				explode1 (pobj->x+random(36+((pobj->state==14)?20:0)),
					pobj->y+random(36+((pobj->state==14)?20:0)),1,0);
				addobj (obj_fireexpl,pobj->x+random(44+((pobj->state==14)?20:0))
					-15,pobj->y+random(44+((pobj->state==14)?20:0))-15,0,0);
				if (pobj->statecount==0) {
					objs[0].state=st_still;
					snd_play (5,snd_enemykill1); addscore (20000,pobj->x,pobj->y);
//					if (!invcount(inv_invin)) addinv (inv_invin);
					pl.health=5; statmodflg|=mod_screen;
					};
				if ((pobj->statecount==58)&&(pobj->counter!=2)) {
					snd_play (4,snd_enemykill2);
					txt ("REACTOR DESTROYED! Press ESC to continue",7,0);
					};
				if (++pobj->statecount>=60) {
					if (pobj->counter==0) {				// end of volume one
						snd_play (5,snd_enemykill1);
						key=0; fire1off=1; fire2off=1; undim();
						do {checkctrl0(1);} while (key!=escape);
						dialogmsg (22,0);
						pl.level=33;
						nl=findcheckpt (pl.level);
						objs[0].x=objs[nl].x;
						objs[0].y=objs[nl].y-24;
						objs[0].state=st_begin;
						objs[0].statecount=0;
						strcpy (newlevel,"board_33");
						}
					else if (pobj->counter==1) {		// end of volume two
						key=0; fire1off=1; fire2off=1; undim();
						snd_play (5,snd_enemykill1);
						do {checkctrl0(1);} while (key!=escape);
						gameover=2;
						}
					else {									// end of volume three
						undim(); setcolor (250,20,20,23);
						setcolor (162,23,13,33); setcolor (163,29,19,39);
						setcolor (164,35,25,45); setcolor (165,42,32,51);
						setcolor (166,49,40,57); setcolor (167,57,50,63);
						sendtrig (pobj->counter,msg_trigger,n);
						snd_play (5,snd_enemykill1); killobj (n);
						objs[0].state=st_stand;
						txt ("BEWARE MALVINEOUS! XARGON IS VERY CLOSE!",3,0);
						}; mod1=0;
				};
			}; return (mod1);
		case msg_draw:
			switch (pobj->state) {
				case 0: break;												// pillar top
				case 1: sh+=1; break;									// pillar mid
				case 2: sh+=2; break;									// pillar bot
				case 3: sh=0x5408; break;								// red block
				case 4: sh=0x2423; break;								// EXIT post
				case 5: sh=0x4b01; break;								// s3
				case 6: sh=0x530f; break;								// s7
				case 7: sh+=3; break;									// weed
				case 8: sh+=4; break;									// mushroom
				case 9: sh+=5; break;									// mptr1
				case 10: sh+=6; break;									// mptr2
				case 11: sh=0x2424; pobj->xl=32; break;			// EXIT sign
				case 12: sh=0x5310; break;								// wl4
				case 13: sh=0x2603;										// reactor
					pobj->xl=44; pobj->yl=45; break;
				case 14: sh=0x2604;										// main reactor
					pobj->xl=64; pobj->yl=64; break;
				case 15: sh=0x2605;										// <- TO (text)
					pobj->xl=32; pobj->yl=10; break;
				case 16: sh=0x2606;										// REACTOR (text)
					pobj->xl=66; pobj->yl=10; break;
				};
			if (pobj->info1<40) {
				drawshape (&gamevp,sh,pobj->x+pobj->xd,pobj->y+pobj->yd);
				}; break;
		case msg_touch: if ((pobj->info1>=40)||(pobj->state<13)
			||(pobj->state>14)) return (0);
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->info1++;
				snd_play (4,snd_hitenemy4);
				if (pobj->state==13) {
					explode1 (pobj->x+random(36),pobj->y+random(36),2,0);
					explode1 (pobj->x+random(36),pobj->y+random(36),3,1);
					addobj (obj_fireexpl,pobj->x+random(28),
						pobj->y+random(28),0,0);
					addobj (obj_fireexpl,pobj->x+random(28),
						pobj->y+random(28),0,0);
					}
				else {
					explode1 (pobj->x+random(56),pobj->y+random(56),2,0);
					explode1 (pobj->x+random(56),pobj->y+random(56),3,1);
					addobj (obj_fireexpl,pobj->x+random(48),
						pobj->y+random(48),0,0);
					addobj (obj_fireexpl,pobj->x+random(48),
						pobj->y+random(48),0,0);
					};
				killobj (z); return (1);
				};
		}; return (0);
	};

int msg_timerpad (int n, int msg, int z) {
	int sh=kindtable[obj_timerpad]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->yd==1) {
				if ((random(70)==0)&&(pobj->info1==0)) {
					pobj->statecount=15;
					pobj->info1=1;
					};
				if (pobj->info1==0) return (0);
				if (pobj->statecount==5) {
					addobj(obj_fire,pobj->x+7,pobj->y+27,0,1);
					snd_play (2,snd_firebrth);
					};
				if (pobj->statecount==0) pobj->info1=0;
				pobj->statecount--; return (0);
				};
			if (pobj->yd==2) {
				if (random(50)==0) pobj->info1=2;
				if (pobj->info1==0) return (0);
				addobj(obj_fire,pobj->x,pobj->y-26,0,0);
				snd_play (2,snd_firebrth);
				pobj->info1=0; return (1);
				};
			if ((pobj->yd==3)&&(objs[0].state!=st_still)) {
				if (random(pobj->counter)==0) {
					addobj (obj_flash,pobj->x-1,pobj->y-1,0,0);
					addobj(obj_xargbot,pobj->x,pobj->y,0,0);
					objs[numobjs-1].state=random(2);
					snd_play (3,snd_xargfire); return (1);
					}; return (0);
				};
			if (pobj->yd==4) {
				if (random(200)==0) {
					addobj (obj_flash,pobj->x-1,pobj->y-3,0,0);
					addobj(obj_bonus,pobj->x,pobj->y,0,0);
					objs[numobjs-1].state=pobj->state;
					snd_play (3,snd_bonus2); killobj (n); return (1);
					}; return (0);
				};
			if ((pobj->yd==5)&&(objs[0].state!=st_still)) {
				if (random(pobj->counter)==0) {
					addobj (obj_flash,pobj->x+6,pobj->y-6,0,0);
					addobj(obj_creeper,pobj->x,pobj->y+2,-1,0);
					snd_play (3,snd_grunt); return (1);
					}; return (0);
				};
			if (pobj->substate++==pobj->state) {
				sendtrig (pobj->counter,msg_trigger,n);
				pobj->substate=0; return (1);
				}; break;
		case msg_draw:
			if (pobj->info1==1) {
				drawshape (&gamevp,sh+12,pobj->x+2,pobj->y+7);
				}
			else if (designflag) {
				drawshape (&gamevp,0x013b,pobj->x+4,pobj->y+4);
				};
		}; return (0);
	};

int msg_turret (int n, int msg, int z) {
	int sh=kindtable[obj_turret]*256+11;
	int c,dx,dy,newx,newy;
	int mod1=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state==1) return (0);
			pointvect (0,n,&dx,&dy,3);
			if (dx<-2) dx=-2; if (dx>2) dx=2;
			if (dx!=pobj->xd) {pobj->xd=dx; mod1=1;};
			if (pobj->statecount==32) {
				for (c=168; c<172; c++) {				// change all blue to max
					setcolor (c,vgapal[c*3+0],vgapal[c*3+1],63);
					};
				};
			if (pobj->statecount==36) {
				pobj->statecount=0;
				for (c=168; c<172; c++) {				// change turret to normal
					setcolor (c,vgapal[c*3+0],vgapal[c*3+1],vgapal[c*3+2]);
					};
				switch (pobj->xd) {
					case -2: newx=pobj->x+1; newy=pobj->y+6; break;
					case -1: newx=pobj->x+5; newy=pobj->y+9; break;
					case 0: newx=pobj->x+8; newy=pobj->y+10; break;
					case 1: newx=pobj->x+11; newy=pobj->y+11; break;
					case 2: newx=pobj->x+15; newy=pobj->y+8;
					};
				snd_play (2,snd_enemyfire);
				addobj (obj_bullet,newx,newy,pobj->xd,2);
				objs[numobjs-1].state=1; mod1=1;
				}; pobj->statecount++; return (mod1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->xd,pobj->x,pobj->y); break;
		case msg_touch:
			if (kindflags[objs[z].objkind]&f_weapon) {
				explode1 (pobj->x+random(16),pobj->y+(8),4,1);
				snd_play (2,snd_hitwall);
				if (objs[z].objkind!=obj_fireball) killobj (z); return (1);
				};
			if (pobj->state==1) return (0);
			if (z==0) hitplayer (n,0); return (1);
		case msg_trigger: pobj->state=1-pobj->state; return (1);
		}; return (0);
	};

int msg_icons (int n, int msg, int z) {
	int sh=kindtable[obj_icons]*256+31;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_draw:
			drawshape (&gamevp,sh+pobj->state,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				switch (pobj->state) {
					case 0: dotextmsg(8,0); addinv (inv_icon1); break;
					case 1: dotextmsg(9,0); addinv (inv_icon2); break;
					case 2: dotextmsg(10,0); addinv (inv_icon3);
					};
				snd_play (3,snd_geticon);
				addobj (obj_flash,pobj->x-1,pobj->y-1,0,1);
				playerkill (n); return (1);
				};
		}; return (0);
	};

int msg_story1 (int n, int msg, int z) {
	int sh=kindtable[obj_story1]*256;
	objtype *pobj; pobj=&(objs[n]);

	if (msg==msg_draw) {
		drawshape (&gamevp,sh+pobj->state,pobj->x+pobj->xd*4,
			pobj->y+pobj->yd*4);
		}; return (0);
	};

int msg_story2 (int n, int msg, int z) {
	int sh=kindtable[obj_story2]*256;
	objtype *pobj; pobj=&(objs[n]);

	if (msg==msg_draw) {
		drawshape (&gamevp,sh+pobj->state,pobj->x+pobj->xd*4,
			pobj->y+pobj->yd*4);
		}; return (0);
	};

int msg_map (int n, int msg, int z) {
	int sh=kindtable[obj_map]*256+18;
	int sh2=kindtable[obj_map]*256+16;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->info1==0) {
				pobj->info1=1;
				switch (pobj->state) {
					case -1: pobj->xl=16; pobj->yl=16; break;
					case 6: pobj->xl=68; pobj->yl=52; break;
					case 7:;
					case 8:;
					case 9:;
					case 10:;
					case 11:;
					case 12: pobj->xl=38; pobj->yl=42;
					};
				}; break;
		case msg_draw:
			if (pobj->state==-1) {
				drawshape (&gamevp,sh2+pobj->substate,pobj->x+2,pobj->y+2);
				}
			else drawshape (&gamevp,sh+pobj->state,pobj->x+pobj->xd,
				pobj->y+pobj->yd); break;
		case msg_touch:
			if ((z==0)&&(pobj->substate==0)) pobj->substate=1; return (1);
		}; return (0);
	};

//int msg_nullkind (int x, int y, int msg) {return (0);};