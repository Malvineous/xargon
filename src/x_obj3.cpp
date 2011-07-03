// X_OBJ3.C
//
// Xargon Objects, Part 3
//
// by Allen W. Pilgrim

#include "port.h"
#include <stdlib.h>
#include <string.h>
#include "include/gr.h"
#include "include/keyboard.h"
#include "include/windows.h"
#include "include/gamectrl.h"
#include "include/music.h"
#include "include/x_obj.h"
#include "include/xargon.h"
#include "include/x_snd.h"

int blinkshtab[16]={12,13,14,15,16,17,18,19,19,19,18,17,16,15,14,13};
int lastwater;

int msg_block (int x, int y, int msg) {
	int bk=board(x,y);
	int gc=(gamecount&3);
	int xc=x<<4;
	int yc=y<<4;
	int c,a;

	switch (msg) {
		case msg_update:
			if ((bk>=lava1)&&(bk<=lava8)&&(gc==0)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>lava8) setboard(x,y,lava1);
				return (1);
				}
			else if ((bk>=spike1)&&(bk<=spike6)&&(gc==1)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>spike6) setboard(x,y,spike1);
				return (1);
				}
			else if ((bk==motion1)&&(gc==1)) return (1);
			else if ((bk>=comp1)&&(bk<=comp3)&&(gc==0)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>comp3) setboard(x,y,comp1);
				return (1);
				}
			else if (((bk==eyecl)||(bk==eyeop))&&(xr_random(200)==0)) {
				if (board(x,y)==eyecl) {setboard(x,y,eyeop); return (1);}
				else {setboard(x,y,eyecl); return (1);};
				}
			else if (bk==mpgate) return (gc==1);
			else if ((bk==blink)&&((gamecount&7)==2)) {
				gc=((gamecount>>3)&15);
				info [blink].flags|=f_notstair;
				info [blink].sh=(info[blink].sh&0xff00)+blinkshtab[gc];
				if ((gc<6)||(gc>=12)) info[blink].flags^=f_notstair;
				return (1);
				}
			else if ((bk==arrow1)&&(xr_random(100)==0)) {
				addobj (obj_arrow,xc+12,yc+6,4,0);
				snd_play (2,snd_enemyfire);
				}
			else if ((bk==arrow2)&&(xr_random(90)==0)) {
				addobj (obj_arrow,xc-8,yc+6,-4,1);
				snd_play (2,snd_enemyfire);
				}
			else if ((bk==waterbubl)&&(gc&1)) return (1);
			else if ((bk==seaweedr)&&(gc==1)) return (1);
			else if ((bk==seaweedl)&&(gc==2)) return (1);
			else if ((bk>=watert1)&&(bk<=watert8)&&(gc&1)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>watert8) setboard(x,y,watert1); return (1);
				} 
			else if ((bk>=waterend1)&&(bk<=waterwav8)&&(gc&1)) {
				setboard (x,y,board(x,y)+1);
				switch (board(x,y)) {
					case (waterend1+8): setboard(x,y,waterend1); break;
					case (watermst1+8): setboard(x,y,watermst1); break;
					case (waterwav1+8): setboard(x,y,waterwav1);
					};
				}
			else if ((bk>=acid1)&&(bk<=acid8)&&(gc==0)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>acid8) setboard(x,y,acid1);
				return (1);
				}
			else if ((bk>=flow1)&&(bk<=flow3)&&(gc==1)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>flow3) setboard(x,y,flow1);
				return (1);
				}
			else if ((bk>=lever1)&&(bk<=lever7)&&(gc==2)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>lever7) setboard(x,y,lever1);
				return (1);
				}
			else if ((bk>=elec1)&&(bk<=elec9)&&(gc&1)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>elec9) setboard(x,y,elec1);
				return (1);
				}
			else if ((bk>=qzap1)&&(bk<=qzap3)&&(gc&2)) {
				setboard (x,y,board(x,y)+1);
				if (board(x,y)>qzap3) setboard(x,y,qzap1);
				return (1);
				}; break;
		case msg_draw:
			if (bk==mpgate) {
				drawshape (&gamevp,info[mpgate].sh+((gamecount>>2)&3),xc,yc);
				}
			else if ((bk>=platl)&&(bk<=platr)) {
				drawshape (&gamevp,info[board(x,y-1)].sh,xc,yc);
				drawshape (&gamevp,info[bk].sh^0x4000,xc,yc);
				}
			else if (bk==motion1) {
				drawshape (&gamevp,info[motion1].sh+((gamecount>>2)&3),xc,yc);
				} 
			else if (bk==waterbubl) {
				drawshape (&gamevp,info[waterbubl].sh+((gamecount>>2)&7),xc,yc);
				}
			else if (bk==seaweedr) {
				drawshape (&gamevp,info[seaweedr].sh+((gamecount>>2)&3),xc,yc);
				}
			else if (bk==seaweedl) {
				drawshape (&gamevp,info[seaweedl].sh+((gamecount>>2)&3),xc,yc);
				}; break;
		case msg_touch:
			if ((bk>=lava1)&&(bk<=lavab)) {
				p_ouch (5,die_ash);
				addobj(obj_fire,objs[0].x-4,objs[0].y+4,0,0);
				addobj(obj_fire,objs[0].x,objs[0].y-4,0,0);
				addobj(obj_fire,objs[0].x+12,objs[0].y+4,0,0);
				addobj (obj_hithero,objs[0].x,objs[0].y,0,0);
				}
			else if ((bk==thornt)||(bk==thornb)||(bk==thrnt)||(bk==thrnb)) {
				hitplayer (0,0);
				}
			else if ((bk>=waterend1)&&(bk<=watermst8)) {
				if	(objs[0].objkind!=obj_heroswim) p_ouch (5,die_ash);
					}
			else if ((bk>=watert1)&&(bk<=watert8)) {
				if (objs[0].objkind!=obj_heroswim) p_ouch (5,die_ash);
					}
			else if ((bk>=waterfl1)&&(bk<=(waterfl4))||(bk==waterflt)) {
				if ((gamecount-lastwater)>10) snd_play (2,snd_touchwater);
				lastwater=gamecount;
				}
			else if ((bk>=acid1)&&(bk<=acidb)) {
				p_ouch (5,die_ash);
				addobj(obj_fire,objs[0].x-4,objs[0].y+4,0,0);
				addobj(obj_fire,objs[0].x,objs[0].y-4,0,0);
				addobj(obj_fire,objs[0].x+12,objs[0].y+4,0,0);
				addobj (obj_hithero,objs[0].x,objs[0].y,0,0);
				}
			else if ((bk>=spike1)&&(bk<=spike6)) p_ouch (5,die_ash);
			else if ((bk>=qzap1)&&(bk<=qzap3)) hitplayer (0,0);
		};	return (0);
	};
