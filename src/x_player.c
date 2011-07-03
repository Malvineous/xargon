// X_PLAYER.C
//
// Xargon: Player controlling routines (all modes)
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

extern char *fidgetmsg[4];
/*int fidgetseq[4][4]={
	{0x0a,0x0a,0x0a,0x0a},
	{0x0b,0x0b,0x0b,0x0b},
	{0x1c,0x1c,0x1c,0x1d},
	{0x08,0x0c,0x21,0x0d}
	};*/
int fidgetnum;
#define fidgetmax 300

void calc_scroll (int peeky) {
	int xscroll=8,vpytop,vpybot,vpypeek;
	objtype *obj0;	obj0=&(objs[0]);
	
	if (obj0->xl==10) xscroll=4;
	if ((obj0->x)<(gamevp.vpox+116)) {
		if (gamevp.vpox>=24) scrollxd=-xscroll;
		}
	if ((obj0->x)>(gamevp.vpox+16*scrnxs-140)) {
		if (gamevp.vpox<(16*(boardxs-scrnxs-1))) scrollxd=xscroll;
		};

	vpytop=max (0,min (16*(boardys-scrnys+1),obj0->y-16*scrnys+96));
	vpybot=max (0,min (16*(boardys-scrnys+1),obj0->y-32));
//	vpytop=max (0,min (16*(boardys-scrnys+1),obj0->y-16*scrnys+112));
//	vpybot=max (0,min (16*(boardys-scrnys+1),obj0->y-48));
	vpypeek=gamevp.vpoy+peeky;

	if ((vpypeek>=vpytop)&&(vpypeek<=vpybot)) scrollyd=peeky;
	else if (gamevp.vpoy>vpybot) scrollyd=vpybot-gamevp.vpoy;
	else if (gamevp.vpoy<vpytop) scrollyd=vpytop-gamevp.vpoy;
	};

int msg_player (int n, int msg, int z) {
	int sh=kindtable[obj_player]*256;
	int c, destx, desty;
	int mod1=0;
	int peeky=0;
	const char climbsh[7]={0x0c,0x0c,0x0d,0x0e,0x0e,0x0d,0x0d};
	const char climby[7]={4,0,0,6,4,4,0};
	objtype *pobj;	pobj=&(objs[n]);

	if (msg==msg_update) {
		switch (pobj->state) {
			case st_stand:
				if (!cando(n,pobj->x,pobj->y,f_playerthru)) {
					pobj->substate=-1;
//					pobj->info1=0;
					};
				if (pobj->yd!=0) {						// Decelerate ???
					pobj->yd-=sgn(pobj->yd);
					mod1=1;
					};
				if (pobj->statecount<0) {				// Land from jump
					mod1=1;
					if (pobj->statecount==-1) pobj->statecount=3;
					}
				else if (pobj->xd==0) {
					if (dx1!=0) {
						pobj->substate=7;	pobj->xd=dx1;
						pobj->statecount=0; mod1=1; break;
						}
					else if (pobj->statecount>fidgetmax) {
//						pobj->info1=0;
						pobj->statecount=20;	mod1=1;
						}
					else if (pobj->statecount>=(fidgetmax-32)) mod1=1;
					else if (pobj->statecount==(fidgetmax-42)) {
						fidgetnum=random(4); txt (fidgetmsg[fidgetnum],6,0);
						}
					else if (pobj->statecount==3) mod1=1;
					if (cando (n,pobj->x,(pobj->y&0xfff0)+16,
						f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
																	// can not stand here
						mod1=1; pobj->state=st_jumping;
						pobj->yd=0;	pobj->substate=2;		// In flight
						};
					}
				else if (dx1!=0) {
					mod1=1;
					if (dx1==pobj->xd) {
						if (cando (n,pobj->x+dx1*8,pobj->y,f_playerthru)) {
							// Walk in direction 'dx1'
							pobj->x+=dx1*8; pobj->yd=0;
							};
						pobj->substate=(pobj->substate+1)&7; pobj->statecount=0;
						}
					else {
						pobj->info1=pobj->xd; pobj->xd=0; pobj->statecount=4;
						};
					}
				else {										// xd!=0 and dx1=0
					if ((pobj->statecount>=2)&&(pobj->substate!=0)) {
						mod1=1; pobj->xd=0; pobj->substate=0; pobj->statecount=0;
						};
					};
				if (cando (n,pobj->x,(pobj->y&0xfff0)+16,
					f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
								// Can't walk here...This is dupe code you fool!
					mod1=1; pobj->state=st_jumping; pobj->yd=0; pobj->substate=0;
					};
				if (fire2) {
					fire2off=1;	mod1=1;
					pobj->state=st_jumping;
					// Set jump -y velocity
					pobj->yd=-(16+4*invcount(inv_jump));
					pobj->substate=0;
					pobj->xd=dx1;
					snd_play (2,snd_jump);
					}
				else if (dy1!=0) {						// Up or (down) vine
					// Up or down arrow key is pressed
					if ((pobj->x&15)==0) {
						if (cando(n,pobj->x,pobj->y+dy1*4,f_playerthru)) {
							if ((!cando(n,pobj->x,pobj->y+dy1*4,f_notvine))&&
								(!cando(n,pobj->x-8,pobj->y+dy1*4,f_notvine))) {
								// No vine in direction I want to go (nor 8 pixels
								// to left of where I want to go).
								mod1=1;
								moveobj (n,pobj->x,pobj->y+dy1*4);
								pobj->state=st_climbing;
								pobj->substate=3;
								};
							};
						};
						if ((pobj->state==st_stand)&&(dx1==0)) {
							// Standing and x-velocity is 0 ...
							pobj->yd=min(max(-3,pobj->yd+dy1*2),3);
//							pobj->info1=0;
							pobj->xd=0;
							pobj->substate=0;
							pobj->statecount=3;
							if (pobj->yd>1) peeky=2;
							else if (pobj->yd<-1) peeky=-2;
							};
					}; break;
			case st_begin:							// Beginning of level
				mod1=1;
				pobj->xl=kindxl[obj_player];	// Initialize normal player w,h
				pobj->yl=kindyl[obj_player];
				pl.ouched=-1; statmodflg|=mod_screen;	// reset hero colors

				if (pobj->statecount>=40) {
					pobj->state=st_stand;
					pobj->statecount=60;
					pobj->info1=1;
					pobj->xd=0; pobj->yd=0;
					mod1=1;
					}; break;
			case st_die:
				if (pobj->substate==die_bird) {
					pobj->yd=min (pobj->yd++,8); pobj->xd=random(7)-3;
					justmove (n,pobj->x+pobj->xd,pobj->y+pobj->yd);
					}
				else if (pobj->substate==die_fish) {
					fishdo (n,pobj->x,pobj->y-pobj->statecount);
					};
				if (pobj->statecount>=19) p_reenter(1);
				pobj->statecount++;
				return (1);		// end of 'st_die' case
			case st_transport:
				if (pobj->statecount>=15) {
					// go into st_stand state,
					// move to new location via msg_trigger
					pobj->state=st_stand;
					sendtrig (pobj->substate,msg_trigger,n);
					};
				pobj->statecount++;
				return (1);    // end of 'st_transport' case
			case st_platform:
				if (pobj->yd!=0) {
					pobj->yd=min(max(-3,pobj->yd+dy1*2),3);
					if (pobj->yd>1) peeky=2;
					else if (pobj->yd<-1) peeky=-2;
					}; break;
			case st_jumping:
				mod1=1;
				pobj->counter=0;
				if ((pobj->x&15)==0) {
					// Exactly on a tile boundary, so check for vine ...
					if ((!cando(n,pobj->x,pobj->y,f_notvine))&&
						(!cando(n,pobj->x-8,pobj->y,f_notvine))) {
						// Caught a vine, set to climbing state
						mod1=1;
						pobj->state=st_climbing;
						pobj->substate=6; break;
						};
					};
				// Not exactly on a tile boundary ...
				if (++pobj->substate>2) {
					pobj->yd+=2;				// Accelerate down due to gravity
					if (pobj->yd>16) pobj->yd=16;		// Limit falling speed
					if (dx1!=0) {
						// left or right arrow pressed
						pobj->substate=2;
						pobj->xd=dx1;	// Set x-speed/velocity appropriately
						};
					if (pobj->substate>8) pobj->xd=0;
					if (!trymovey (n,pobj->x+dx1*8,pobj->y+pobj->yd)) {
						// Unable to fall further ...
						if (pobj->yd>=0) {
							// We have y velocity in down direction
							c=0;
							// Exactly on tile boundary (y)
							if (((pobj->y+pobj->yl)&15)==0) c=1;
							else if (!trymovey (n,pobj->x+dx1*8,
								((pobj->y+pobj->yl)&0xfff0)+16-pobj->yl)) c=1;
							if (c==1) {
								// We fell and hit a platform or ground.
								// Go into standing state
								pobj->state=st_stand;
								pobj->counter=6;
								pobj->statecount=((pobj->yd>=16)?-7:-4)+(dx1!=0);
								pobj->xd=0;
								pobj->yd=0;
								snd_play (2,snd_land);
								};
							}
						else {
							// yd <=0, we're NOT moving down
							desty=(pobj->y-1)&0xfff0;
							if (desty==pobj->y) pobj->yd=0;
							else if (!trymovey (n,pobj->x+dx1*8,desty))
								pobj->yd=0;					// Bump R Head
							};
						};
					}; break;
			case st_climbing:								// x&15 = 0
				if (dx1!=0) {
					if (pobj->substate!=6) {
						if (cando (n,pobj->x+dx1*8,pobj->y,f_playerthru)) {
							pobj->counter=0;
							mod1=1;
							moveobj (n,pobj->x+dx1*8,pobj->y);
							pobj->state=st_jumping;
							pobj->substate=2;
							pobj->xd=dx1;
							if (fire2) {
								fire2off=1;
								pobj->yd=-16;
								snd_play (2,snd_jump);
								}
							else pobj->yd=-4;
							};
						};
					}
				else {
					pobj->xd=0;
					if (pobj->substate==6) pobj->substate=0;			// OPT
					if (dy1!=0) {
						desty=pobj->y;
						if (dy1<0) desty-=climby[pobj->substate];
						else desty+=4;
						if (!cando (n,pobj->x,desty,f_playerthru)) {
							if (dy1>0) desty=(pobj->y&0xfff0)+16;
							else desty=(pobj->y-1)&0xfff0;
							if (!cando (n,pobj->x,desty,f_playerthru)) desty=0;
							};
						if (desty!=0) {
							if (dy1<0) {
								pobj->substate+=dy1;
								if (pobj->substate>=6) pobj->substate=0;
								}
							else pobj->substate=2;
							mod1=1;
							moveobj (n,pobj->x,desty);
							if (cando (n,pobj->x,pobj->y,f_notvine)) {
								pobj->state=st_stand;
								pobj->xd=0;
//								pobj->info1=0;
								};
							};
						};
					if (pobj->substate<0) pobj->substate=5;
					else if (pobj->substate>6) pobj->substate=6;
					}; break;
				};
				if (pobj->xd!=0) pobj->info1=pobj->xd;
				if (((fire1)||(key==' '))&&
					(stateinfo[pobj->state]&sti_canfire)) {
					fire1off=1;
//					if ((pobj->info1!=0)&&((pobj->state!=st_stand)||
//						((pobj->state==st_stand)&&(pobj->yd==0)))) {
//					if ((pobj->info1!=0)&&(pobj->yd<=0)) {
					if (pobj->info1!=0) {
						// makes player face last XD
						if ((pobj->xd==0)&&(pobj->state==st_stand)) {
							pobj->substate=0;
							pobj->statecount=3;
							mod1=1;
							}
						else if ((pobj->xd==0)&&(pobj->state==st_platform)) {
							pobj->xd=pobj->info1;
							mod1=1;
							};
						if (invcount(inv_fire)) {
							destx=0;
							for (c=0; c<numscrnobjs; c++)
								destx+=(objs[scrnobjs[c]].objkind==obj_fireball);
							if (destx<1) {
								takeinv(inv_fire);
								addobj (obj_fireball,pobj->x-8+(pobj->info1>0)
									*16,pobj->y+8,pobj->info1*8,0);
								snd_play (2,snd_firebrth); mod1=1;
								}
							else snd_play (1,snd_error);
							}
						else if (invcount(inv_rock)) {
							destx=0;
							for (c=0; c<numscrnobjs; c++)
								destx+=(objs[scrnobjs[c]].objkind==obj_rock);
							if (destx<invcount(inv_rock)) {
								addobj (obj_rock,pobj->x+4,pobj->y+8,pobj->info1*6,-8);
								snd_play (2,snd_rockbounce); mod1=1;
								}
							else snd_play (1,snd_error);
							}
						else {
							destx=0;
							for (c=0; c<numscrnobjs; c++)
								destx+=(objs[scrnobjs[c]].objkind==obj_laser);
							if (destx<(invcount(inv_laser))) {
								addobj (obj_laser,pobj->x+4+(pobj->info1>0)*4,
									pobj->y+18+((pobj->yd==3)?12:0),pobj->info1*6,0);
								snd_play (2,snd_shoot); mod1=1;
								}
							else snd_play (1,snd_error);
							};
						};									// end (if pobj->info1!=0)
					};										// end if (fire1)
				pobj->statecount++;
				if (pobj->counter!=0) {
					mod1=1;
					pobj->counter--;
					};
				touchbkgnd(0);
				calc_scroll(peeky);
				return (mod1);
		}
	else if (msg==msg_draw) {
		if (pl.ouched>0) {
			sh+=0x1b+((pobj->xd>0)?1:0);
			drawshape (&gamevp,sh,pobj->x,pobj->y); return (1);
			};
		switch (pobj->state) {
			case st_stand:						// stand or walk
				if (pobj->statecount<0) sh+=0x1a;
//					if (pobj->xd==0) sh+=0x1a;					// land forward
//					else sh+=0x13+((pobj->xd>0)?3:0);		// land left/right
//					}
//				else if (pobj->yd<0) sh+=0x0b;				// head up
				else if (pobj->yd==3) sh+=0x1a;				// squat
//				else if (pobj->yd>0) sh+=0x0a;				// head down
//				else if ((pobj->info1==0)||((pobj->xd==0)&&
//					(pobj->statecount>=3))) {
				else if ((pobj->xd==0)&&(pobj->statecount>=3)) {
//					if ((pobj->statecount<60)&&(pobj->info1!=0))
					if (pobj->statecount<60) sh+=0x0a+((pobj->info1>0)?1:0);
//					else if (pobj->statecount>(fidgetmax-32)) sh+=0x08;
//						sh+=fidgetseq[fidgetnum][(pobj->statecount/2)&3];
					else if (pobj->statecount>120) sh+=0x09;
					else sh+=0x08;
					}
				else {
					if (pobj->substate==8) sh+=0x0a+(pobj->xd>0);		// l/r
					else sh+=((pobj->substate&7)/2)+(pobj->xd>0?4:0);	// running
					}; break;
			case st_jumping:
				if (pobj->xd==0) {
					if (pobj->yd<=0) sh+=0x18;					// straight up
					else sh+=0x19;									// straight down
					}
				else {
					if (pobj->yd<=0) sh+=0x0f;					// jump up left
					else sh+=0x10;									// going down left
					if (pobj->xd>0) sh+=2;						// face right
					}; break;
			case st_climbing:
				sh+=climbsh [pobj->substate]; break;
			case st_begin:
//				if (pobj->statecount<16) sh+=0x0b;
//				else if (pobj->statecount<24) sh+=0x08;
//				else sh+=0x0a;
				sh+=0x08;
				memcpy (&tempvp,&gamevp,sizeof(vptype));
				tempvp.vpyl=pobj->y+pobj->yl-gamevp.vpoy;
				drawshape (&tempvp,sh,pobj->x,pobj->y+41-pobj->statecount);
				return (1);
			case st_die:
				switch (pobj->substate) {
					case die_ash: sh+=0x13+pobj->statecount/4; break;
					case die_bird:
						drawshape (&gamevp,kindtable[obj_herobee]*256+6,
							pobj->x,pobj->y); return (1);
					case die_fish:
						drawshape (&gamevp,kindtable[obj_heroswim]*256+5,
							pobj->x,pobj->y); return (1);
					}; break;
			case st_transport:
				sh+=0x1d+pobj->statecount/4; break;
			case st_platform:
				if ((pobj->xd!=0)&&(dy1<=0)) {
					sh+=0x0a+((pobj->xd>0)?1:0);
					drawshape (&gamevp,sh,pobj->x+3,pobj->y);
					return (1);
					};
//				if ((pobj->yd<0)||(pobj->substate==0)) {
//					if (dy1>0) sh+=0x1c;
//					else sh+=0x0b;							// look up
//					}
				if (dy1>0) sh+=0x1a;						// so player can squat
//				else if ((pobj->yd>0)||(pobj->substate==1)) sh+=0x0a;	// down
				else sh+=0x08;								// standing
				drawshape (&gamevp,sh,pobj->x+3,pobj->y); break;
			case st_still: sh+=0x09; break;
			};													// end of switch statement
			if (pobj->state!=st_platform)
				drawshape (&gamevp,sh,pobj->x,pobj->y);
			if (pobj->counter>0)							// dust
				drawshape (&gamevp,0x0600+37-pobj->counter/2,
					pobj->x+4,pobj->y+37);
		}; return (0);
	};

int msg_tiny (int n, int msg, int z) {
	int sh=kindtable[obj_tiny]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if ((dx1!=0)||(dy1!=0)) {
				pobj->statecount=(pobj->statecount+1)&7;
				pobj->xd=dx1; pobj->yd=dy1;
				if (cando (n,pobj->x+dx1*4,pobj->y+dy1*4,f_playerthru)) {
					pobj->x+=dx1*4; pobj->y+=dy1*4;
					}
				}; calc_scroll(0); touchbkgnd(0);
			return ((pobj->statecount&1)==0);
		case msg_draw:
			if (pobj->xd!=0) sh+=pobj->statecount/2+((pobj->xd<0)?0:12);
			else sh+=pobj->statecount/2+((pobj->yd<0)?4:8);
			drawshape (&gamevp,sh,pobj->x,pobj->y);
		};	return (0);
	};

int msg_heroswim  (int n, int msg, int z) {
	int c,f;
	int onwater,destx,desty,tempy;
	int sh=kindtable[obj_heroswim]*256;
	objtype *pobj;	pobj=&(objs[n]);


	switch (msg) {
		case msg_update:
			onwater=fishdo(n,pobj->x,pobj->y);
			pobj->xd=dx1*8;
			if (onwater) {
				pobj->yd+=dy1*3+(pobj->yd<2);
				if (random(15)==0)
					addobj (obj_bubble,pobj->x+8,pobj->y-2,0,0);
				pobj->yd=min(8,max(pobj->yd,-8));
				}
			else {
				pobj->yd+=4;
				pobj->yd=min(16,max(pobj->yd,-16));
				};
			if (fire2&&onwater) {
				fire2off=1;
				pobj->yd=-3;
				};
			if ((fire1)||(key==' ')) {
				fire1off=1;
				f=0;
				for (c=0; c<numscrnobjs; c++)
					f+=(objs[scrnobjs[c]].objkind==obj_torpedo);
					if (f<1) {
						addobj (obj_torpedo,pobj->x+22,pobj->y+25,6,0);
						addobj (obj_torpedo,pobj->x-3,pobj->y+25,-6,0);
						snd_play (2,snd_torpedo);
						}
					else snd_play (1,snd_error);
				};
			if (pobj->yd>8) pobj->yd=8;
			else if (pobj->yd<-8) pobj->yd=-8;

			destx=pobj->x+pobj->xd;
			desty=pobj->y+pobj->yd;
			if (!justmove (n,pobj->x,desty)) {
				if (pobj->yd<0) tempy=(desty+16)&0xfff0;
				else if (pobj->yd>0) tempy=(desty&0xfff0)+32-pobj->yl;
				if (pobj->yd!=0) {
					if (!justmove(n,pobj->x,tempy)) pobj->yd=0;
					};
				pobj->yd=0;
				};
			fishdo (n,destx,pobj->y);							// try do
			calc_scroll(0); return (1);
		case msg_draw:
			if ((pobj->yd>0)&&(pobj->xd==0)) pobj->state=0;
			else if ((pobj->yd<0)&&(pobj->xd==0)) pobj->state=1;
			else if (pobj->xd<0) pobj->state=2;
			else if (pobj->xd>0) pobj->state=3;
			else if (pobj->yd==0) pobj->state=4;
			drawshape (&gamevp,sh+pobj->state,pobj->x,pobj->y);
		}; return (0);
	};

int msg_herobee (int n, int msg, int z) {
	int sh=kindtable[obj_herobee]*256;
	int c,f;
	int destx,desty;
	objtype *pobj;	pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&3;
			if (pobj->xd!=0) pobj->substate=pobj->xd;
			if (((pobj->x&7)!=0)||(pobj->xd==0)) objs [n].xd=dx1*4;
			else pobj->xd=dx1*8;
			if ((pobj->counter&1)!=0) pobj->yd++;
			if (fire2) {
				snd_play (1,snd_hopperland);
				fire2off=1; pobj->yd=-6;
				};
			if (fire1||(key==' ')) {
				fire1off=1;
				f=0;
				for (c=0; c<numscrnobjs; c++)
					f+=(objs[scrnobjs[c]].objkind==obj_laser);
					if (f<1) {
						pobj->xd=pobj->substate;
						addobj (obj_laser,pobj->x+sgn(pobj->substate)*8,
							pobj->y+14,sgn(pobj->substate)*8,dy1*2);
						snd_play (2,snd_shoot);
						};
					};
			if (pobj->yd>8) pobj->yd=8;
			else if (pobj->yd<-8) pobj->yd=-8;

			destx=pobj->x+pobj->xd;
			if (!justmove (n,destx,pobj->y)) destx=pobj->x;
			if (pobj->yd!=0) {
				desty=pobj->y+pobj->yd;
				if (!justmove (n,destx,desty)) pobj->yd=0;
				};
			calc_scroll(0); touchbkgnd(0); return (1);
		case msg_draw:
			if (pobj->xd==0) sh+=pobj->counter/2+4;
			else sh+=pobj->counter/2+((pobj->xd>0)?2:0);
			drawshape (&gamevp,sh,pobj->x,pobj->y);
		}; return (0);
	};

int playerxfm (int xfmto) {
	int c=0;
	int newobj=obj_player;
	int oldobj,oldxl,oldyl;
	int newx,newy;
	objtype *obj0;	obj0=&(objs[0]);
	oldobj=obj0->objkind;
	oldxl=obj0->xl;
	oldyl=obj0->yl;

	switch (xfmto) {
		case inv_bird: newobj=obj_herobee; break;
		case inv_fish: newobj=obj_heroswim; break;
		};

	if (obj0->objkind==newobj) return;
	obj0->objkind=newobj;
	obj0->xl=kindxl[newobj];
	obj0->yl=kindyl[newobj];
	newx=obj0->x&0xfff8;
	newy=obj0->y+oldyl-obj0->yl;
	if (cando(0,newx,newy,f_playerthru)) c=1;
	else {
		newy=obj0->y;
		if (cando(0,newx,newy,f_playerthru)) c=1;
		};
	if (c) {
		addinv (xfmto);
		obj0->y=newy;
		obj0->x=newx;
		obj0->state=0;
		obj0->substate=0;
		obj0->counter=0;
		obj0->xd=0; obj0->yd=0;
		for (c=0; c<numinvkinds; c++)
			if (inv_xfm[c])
			while (takeinv (c)) {};
		explode1 (obj0->x,obj0->y,10,0);
		txt ((char*)inv_getmsg[xfmto],7,0);
		}
	else {
		obj0->objkind=oldobj;
		obj0->xl=oldxl;
		obj0->yl=oldyl;
		}; return;
	};