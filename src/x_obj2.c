// X_OBJ2.C
//
// Xargon Objects, Part 2
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

int msg_grunt (int n, int msg, int z) {
	int sh=kindtable[obj_grunt]*256;
	const int grunt_die[10]={7,16,7,16,7,16,7,16,7,16};
	const int grunt_std[5]={0,1,2,3,3};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=4) {
				if (pobj->statecount==0) {
					snd_play (4,snd_enemykill3);
					addscore (kindscore[obj_grunt],pobj->x,pobj->y);
					};
				if (++pobj->statecount>=10) killobj (n);
				return (1);
				};
			if (++pobj->counter>=10) pobj->counter=0;
			if ((pobj->counter&1)!=0) return (0);
			if (xr_random(20)==0) {
				pobj->xd=0; snd_play (2,snd_grunt);
				};
			if (xr_random(15)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);
				pobj->xd*=4;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd; return (1);
		case msg_draw:
			if (pobj->state>=4) sh+=grunt_die[pobj->statecount];
			else if (pobj->xd==0) sh+=grunt_std[pobj->counter/2]+
				((pobj->xd>0)?9:0);
			else sh+=pobj->counter/2+((pobj->xd>0)?13:4);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (pobj->state>=4) return (0);
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy4);
				explode1 (pobj->x+4,pobj->y+4,3,0);
				explode1 (pobj->x+8,pobj->y+6,4,1);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_biter (int n, int msg, int z) {
	int sh=kindtable[obj_biter]*256;
	int mod1=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			switch (pobj->state) {
				case 0: 
					pobj->counter=0;
					pobj->xl=10;
					pobj->x+=pobj->xd*6; mod1=1; break;
				case 27:
					snd_play (1,snd_biter);
					pobj->counter=1;
					pobj->xl=24;
					pobj->x-=pobj->xd*14; mod1=1; break;
				case 34:
					pobj->counter=2; mod1=1; break;
				case 36:
					pobj->state=0;
					pobj->counter=0;
					pobj->xl=10;
					pobj->x+=pobj->xd*14; mod1=1;
					}; pobj->state++; return (mod1);
		case msg_draw:
			sh+=pobj->counter+pobj->xd*3;
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
		}; return (0);
	};

int msg_centipede (int n, int msg, int z) {
	int sh=kindtable[obj_centipede]*256;
	int c;
	const int midtab [6][6]={
		{1,2,3,4,5,6},
		{2,3,4,5,6,1},
		{3,4,5,6,1,2},
		{4,5,6,1,2,3},
		{5,6,1,2,3,4},
		{6,1,2,3,4,5}
		};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (++pobj->counter>=12) pobj->counter=0;
			if (xr_random(30)==0) seekplayer (n,&pobj->xd,0);
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd;
			return(1);
		case msg_draw:
			if (pobj->xd>0) {						// centipede going right
				drawshape (&gamevp,sh+8,pobj->x+pobj->xl-16,pobj->y	// head
					+((pobj->counter<4)?1:0));
				for (pobj->state=1;pobj->state<=((pobj->xl-28)/8);pobj->state++)
					drawshape (&gamevp,sh+midtab[pobj->state-1][pobj->counter/2]
						+8,pobj->x+4+(pobj->state*8),pobj->y+5);			// body
				drawshape (&gamevp,sh+15,pobj->x,pobj->y    				// tail
					+((pobj->counter>8)?6:7));
				}
			else {									// centipede going left
				drawshape (&gamevp,sh,pobj->x,pobj->y						// head
					+((pobj->counter<4)?1:0));
				for (pobj->state=1;pobj->state<=((pobj->xl-28)/8);pobj->state++)
					drawshape (&gamevp,sh+midtab[pobj->state-1][pobj->counter/2],
						pobj->x+8*(pobj->state+1),pobj->y+5);				// body
				drawshape (&gamevp,sh+7,pobj->x+pobj->xl-12,pobj->y	// tail
					+((pobj->counter>8)?6:7));
				}; break;
		case msg_touch:
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->xl-=8;
				killobj (z);
				if (pobj->xl<37) {
					for (c=0; c<7; c++) {
						addobj (obj_centexpl,pobj->x+((pobj->xd>0)?pobj->xl-24:16),
							pobj->y+5,xr_random(7)-3,xr_random(4)-10);
						};
					explode1 (objs[z].x,objs[z].y,4,0);
					playerkill (n); snd_play (4,snd_enemykill3);
					}
				else {
					addobj (obj_centexpl,pobj->x+((pobj->xd>0)?pobj->xl-24:16),
						pobj->y+5,xr_random(7)-3,xr_random(4)-10);
					snd_play (4,snd_hitenemy3);
					};
				}; return (1);
		}; return (0);
	};

int msg_alien (int n, int msg, int z) {
	int sh=kindtable[obj_alien]*256;
	const int alien_die[20]={18,18,19,19,20,20,18,19,20,
		18,19,20,18,19,20,20,20,20,20,20};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=3) {
				if (pobj->statecount==0) {
					snd_play (4,snd_enemykill3);
					addscore (kindscore[obj_alien],pobj->x,pobj->y);
					};
				if (++pobj->statecount>=20) killobj (n);
				return (1);
				};
			if (++pobj->counter>=20) pobj->counter=0;
			if (++pobj->info1>=32) pobj->info1=0;
			if ((pobj->counter&1)!=0) return (0);
			if (pobj->xd!=0) pobj->substate=pobj->xd;
			if (xr_random(20)==0) pobj->xd=(pobj->xd>0)?0:pobj->substate;
			if (xr_random(15)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);
				pobj->xd*=4;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd; return (1);
		case msg_draw:
			if (pobj->state>=3) sh+=alien_die[pobj->statecount];
			else if (pobj->xd==0) sh+=(pobj->info1/4+10);
			else sh+=pobj->counter/4+((pobj->xd>0)?0:5);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (pobj->state>=3) return (0);
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy1);
				explode1 (pobj->x,pobj->y,4,0);
				explode1 (pobj->x+8,pobj->y+4,4,1);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_leech (int n, int msg, int z) {
	int sh=kindtable[obj_leech]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&7;
			if ((pobj->counter&1)!=0) return (0);
			if (pobj->statecount--==0) pobj->xd=pobj->substate;
			if (pobj->xd!=0) pobj->substate=pobj->xd;
			if (xr_random(20)==0) {
				pobj->xd=0; pobj->statecount=2; return (0);
				};
			if (xr_random(40)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd); pobj->xd*=4;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd; return (1);
		case msg_draw:
			if (pobj->xd==0) sh+=((pobj->substate<0)?4:9);
			else sh+=pobj->counter/2+((pobj->xd<0)?0:5);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				if (pobj->state>=3) {
					explode1 (pobj->x,pobj->y+2,5,0);
					explode1 (pobj->x+8,pobj->y+4,6,1);
					playerkill (n); snd_play (4,snd_enemykill2);
					}
				else {
					explode1 (pobj->x,pobj->y+2,3,0);
					explode1 (pobj->x+8,pobj->y+4,4,1);
					snd_play (4,snd_hitenemy2);
					};
				if (objs[z].objkind!=obj_fireball) killobj (z);
				pobj->state++;
				}; return (1);
		}; return (0);
	};

int msg_troll (int n, int msg, int z) {
	int sh=kindtable[obj_troll]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=4) {
				snd_play (4,snd_enemykill3);
				addscore (kindscore[obj_troll],pobj->x,pobj->y);
				addobj (obj_hithero,pobj->x,pobj->y,0,0);
				killobj (n);
				};
			if (++pobj->counter>=32) pobj->counter=0;
			if ((pobj->counter&1)!=0) return (0);
			if (xr_random(20)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);	pobj->xd*=4;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd;
			if ((xr_random(30)==0)&&(pobj->xd!=0)) {
				addobj (obj_bullet,pobj->x+6+(pobj->xd>0)
					*20,pobj->y+10,sgn(pobj->xd)*6,0);
				snd_play (2,snd_enemyfire); 
				}; return (1);
		case msg_draw:
			sh+=((pobj->xd>0)?0:8)+pobj->counter/4;
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy4);
				explode1 (pobj->x,pobj->y,3,0);
				explode1 (pobj->x+4,pobj->y+10,3,1);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_blob (int n, int msg, int z) {
	int sh=kindtable[obj_blob]*256;
	const int blobtab[8]={0,0,1,1,2,2,3,3};
	const int ltab[8]={4,5,6,7,11,10,9,8};
	const int rtab[8]={8,9,10,11,7,6,5,4};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&15;
			switch (pobj->state) {
				case 0:
					if ((pobj->counter>=((pobj->xd<0)?0:12))&&
						(pobj->counter<((pobj->xd<0)?4:16))) {
						if ((!crawl (n,pobj->xd,0))||(xr_random(40)==0))
							pobj->xd=-pobj->xd;
						};
					if (xr_random(50)==0) {
						if (pobj->xd<0) {
							if (!(standfloor (n,-78,0))) return (0);
							pobj->state=1; pobj->x-=23;
							snd_play (2,snd_blob);
							}
						else {
							if (!(standfloor (n,+78,0))) return (0);
							pobj->state=2; pobj->x+=2;
							snd_play (2,snd_blob);
							};
						pobj->xl=52; pobj->yl=28;
						pobj->y-=15;
						pobj->counter=0;
						};	return (1);
				case 1:
					if (pobj->counter==8) pobj->x-=52;
					if (pobj->counter>14) {
						pobj->xl=30; pobj->yl=13;
						pobj->y+=15; pobj->x-=2;
						pobj->counter=0;
						pobj->state=0;
						};	return (1);
				case 2:
					if (pobj->counter==8) pobj->x+=52;
					if (pobj->counter>14) {
						pobj->xl=30; pobj->yl=13;
						pobj->y+=15; pobj->x+=23;
						pobj->counter=12;
						pobj->state=0;
						};
				}; return (1);
		case msg_draw:
			switch (pobj->state) {
				case 0: sh+=blobtab[pobj->counter/2]; break;
				case 1: sh+=ltab[pobj->counter/2]; break;
				case 2: sh+=rtab[pobj->counter/2];
				};
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
		}; return (0);
	};

int msg_lizard (int n, int msg, int z) {
	int sh=kindtable[obj_lizard]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&7;
			if (xr_random(20)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);	pobj->xd*=2;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd;
			if ((xr_random(55)==0)&&(pobj->xd!=0)) {
				addobj (obj_bullet,pobj->x-4+(pobj->xd>0)
					*16,pobj->y+4,pobj->xd*4,0);
				snd_play (2,snd_enemyfire);
				}; return (1);
		case msg_draw:
			sh+=pobj->counter/2+((pobj->xd<0)?4:0);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
		}; return (0);
	};

int msg_xargbot (int n, int msg, int z) {
	int sh=kindtable[obj_xargbot]*256;
	const int xdtab[2]={-8,8};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->info1==1) {
				pobj->counter=(pobj->counter+1)&1;
				if (pobj->counter==0) {
					addobj(obj_fire,pobj->x,pobj->y-24,0,0);
					objs[numobjs-1].state=1;
               };
				justmove(n,pobj->x+xdtab[pobj->counter],
					pobj->y+pobj->yd);
				if ((!justmove(n,pobj->x,pobj->y+pobj->yd))||(!onscreen(n))) {
					explode1 (pobj->x+4,pobj->y+4,5,0);
					killobj (n);
					}; return (1);
				};
			if (xr_random(15)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);
				pobj->yd*=4; pobj->xd*=2;
				};
			if (!justmove(n,pobj->x+pobj->xd,pobj->y))
				pobj->xd=-pobj->xd;
			if ((!justmove(n,pobj->x,pobj->y+pobj->yd))||(!onscreen(n)))
				pobj->yd=-pobj->yd; 
			if (xr_random(20)==0) {
				pobj->xd=0;	pobj->substate=1;	return (1);
				};
			if (pobj->xd<0) pobj->substate=0;
			else if (pobj->xd>0) pobj->substate=2;
			if ((xr_random(((pobj->state!=2)?50:40))==0)&&(pobj->xd!=0)) {
				addobj (obj_bullet,pobj->x-4+(pobj->xd>0)
					*16,pobj->y+18,pobj->xd*3,0);
				snd_play (2,snd_xargfire);
				}; return (1);
		case msg_draw:
			sh+=pobj->substate+(pobj->state*3);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (pobj->info1==1) return (0);
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				if ((pobj->state==2)&&(pobj->statecount==0)) {
					pobj->statecount=1; snd_play (4,snd_hitenemy4);
					explode1 (pobj->x,pobj->y,5,1);
					if (objs[z].objkind!=obj_fireball) killobj (z);
					}
				else {
					pobj->info1=1; pobj->yd=2; pobj->substate=1;
					if (pobj->state==2) addscore (130,pobj->x,pobj->y);
					else addscore (kindscore[obj_xargbot],pobj->x,pobj->y);
					snd_play (4,snd_enemykill1);
					if (objs[z].objkind!=obj_fireball) killobj (z);
					};
				}; return (1);
		};	return (0);
	};

int msg_krusty (int n, int msg, int z) {
	int sh=kindtable[obj_krusty]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=3) {
				snd_play (4,snd_enemykill2);
				explode1 (objs[z].x,objs[z].y,5,0);
				playerkill (n); return (1);
				};
			if (++pobj->counter>=12) pobj->counter=0;
			if (xr_random(30)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd); pobj->yd=0;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd;
			if (xr_random(12)==0)
				addobj (obj_bubble,pobj->x+6,pobj->y-2,0,0); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2+
				(pobj->xd<0)*6,pobj->x,pobj->y); break;
		case msg_touch:
			if (pobj->state>=3) return (0);
			if (z==0) hitplayer (n,1);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy3);
				explode1 (pobj->x,pobj->y,4,1);
				killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_ghoul (int n, int msg, int z) {
	int sh=kindtable[obj_ghoul]*256;
	int dist;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->info1>=8) {
				snd_play (4,snd_enemykill1);
				addscore (kindscore[obj_ghoul],pobj->x,pobj->y);
				explode2 (n); killobj (n); return (1);
				};
			if (pobj->statecount>0) pobj->statecount--;
			if (!justmove(n,pobj->x,pobj->y+pobj->yd)) pobj->yd=-pobj->yd;
			if (xr_random(20)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);
				pobj->xd*=2; pobj->yd*=2;
				};
			if (!justmove(n,pobj->x+pobj->xd,pobj->y))
				pobj->xd=-pobj->xd;
			dist=vectdist(n,0);
			if (dist<224) pobj->state=0;
			if (dist<192) pobj->state=1;
			if (dist<160) pobj->state=2;
			if (dist<128) pobj->state=3;
			if (dist<96) {
				pobj->state=4;
				if (pobj->statecount==0) {
					pobj->statecount=30;
					addobj (obj_bullet,pobj->x-4+(pobj->xd>0)
						*30,pobj->y+4,pobj->xd*3,0);
					};
				};	return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->state+((pobj->xd>0)?0:5),
				pobj->x,pobj->y); break;
		case msg_touch: if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->info1++;
				snd_play (4,snd_hitenemy4); explode2 (n);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_eel (int n, int msg, int z) {
	int sh=kindtable[obj_eel]*256+12;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (++pobj->counter>=10) pobj->counter=0;
			if (xr_random(20)==0) {
				pobj->xd=xr_random(3)*4-4;
				if (pobj->xd==0) pobj->yd=xr_random(2)*4-2;
				else pobj->yd=xr_random(3)*2-2;
				};
			if (!fishdo(n,pobj->x+pobj->xd,pobj->y))
				pobj->xd=-pobj->xd;
			if (!fishdo(n,pobj->x,pobj->y+pobj->yd))
				pobj->yd=-pobj->yd;
			if (xr_random(10)==0)
				addobj (obj_bubble,pobj->x+6,pobj->y-2,0,0); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2+(pobj->xd>0)*5,
				pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,1); return (1);};
		}; return (0);
	};

int msg_badfish (int n, int msg, int z) {
	int sh=kindtable[obj_badfish]*256+6;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=2) {
				if (pobj->statecount==0) {
					snd_play (4,snd_fishdie);
					addscore (kindscore[obj_badfish],pobj->x,pobj->y);
					};
				fishdo(n,pobj->x,pobj->y+2);
				if (++pobj->statecount>=20) killobj (n);
				return (1);
				};
			pobj->counter=(pobj->counter+1)&7;
			if (xr_random(20)==0) {
				pobj->xd=xr_random(3)*4-4;
				if (pobj->xd==0) pobj->yd=xr_random(2)*4-2;
				else pobj->yd=xr_random(3)*2-2;
				};
			if (!fishdo(n,pobj->x+pobj->xd,pobj->y))
				pobj->xd=-pobj->xd;
			if (!fishdo(n,pobj->x,pobj->y+pobj->yd))
				pobj->yd=-pobj->yd;
			if (xr_random(10)==0)
				addobj (obj_bubble,pobj->x+6,pobj->y-2,0,0); return (1);
		case msg_draw:
			if (pobj->state>=2) {
				drawshape (&gamevp,sh+4+(pobj->xd>0)*5,pobj->x,pobj->y);
				}
			else drawshape (&gamevp,sh+pobj->counter/2+(pobj->xd>0)*5,
				pobj->x,pobj->y); break;
		case msg_touch:
			if (pobj->state>=2) return (0);
			if (z==0) hitplayer (n,1);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy3);
				explode1 (pobj->x,pobj->y,4,1);
				killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_bat (int n, int msg, int z) {
	int sh=kindtable[obj_bat]*256+8;
	const int ydtab[4]={1,0,-1,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&7;
			if (++pobj->statecount>=4) pobj->statecount=0;
			if (!justmove(n,pobj->x+pobj->xd,pobj->y))
				pobj->xd=-pobj->xd;
			justmove(n,pobj->x,pobj->y+ydtab[pobj->statecount]); return (1);
		case msg_draw:
//			sh+=pobj->counter/2+((pobj->xd>0)?4:0);
			drawshape (&gamevp,sh+pobj->counter/2,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
		}; return (0);
	};

int msg_hopper (int n, int msg, int z) {
	int sh=kindtable[obj_hopper]*256;
	int16_t dx,dy;
	int mod1=0;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state==0) {
				if (++pobj->counter>16) {
					pobj->state=1;
					seekplayer (n,&dx,&dy);
					pobj->xd=dx*4;
					pobj->yd=-12;
//					pobj->yd=-10;
					mod1=1;
					};
				}
			else {
				mod1=1;
				if(++pobj->yd>12) pobj->yd=10;
				dx=pobj->x+pobj->xd;
				dy=pobj->y+pobj->yd;
				if (!(trymove(n,dx,dy)&3)) {
					if (pobj->yd<0) pobj->yd=0;
					else {
						dy=(dy&0xfff0)+16-pobj->yl;
						trymove (n,dx,dy);
						seekplayer (n,&dx,&dy);
						pobj->xd=dx;
						pobj->state=0;
						pobj->counter=0;
						snd_play (1,snd_hopperland);
						};
					};
				};	return (mod1);
		case msg_draw:
			sh+=(pobj->xd<0)*3;
			if (pobj->state==1) sh+=(pobj->yd>0)*2+(pobj->yd<=0);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
		}; return (0);
	};

int msg_creeper (int n, int msg, int z) {
	int sh=kindtable[obj_creeper]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=3) {
				if (pobj->statecount==0) {
					snd_play (4,snd_enemykill3);
					addscore (kindscore[obj_creeper],pobj->x,pobj->y);
					};
				if (++pobj->statecount>=20) killobj (n);
				return (1);
				};
			if (++pobj->counter>=12) pobj->counter=0;
			if (xr_random(30)==0) seekplayer (n,&pobj->xd,&pobj->yd);
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd; return (1);
		case msg_draw:
			if (pobj->state>=3) {
				drawshape (&gamevp,sh+(pobj->statecount/4)+6,pobj->x,pobj->y);
				}
			else {
				drawshape (&gamevp,sh+pobj->counter/4+((pobj->xd>0)?3:0),
					pobj->x,pobj->y);
				}; break;
		case msg_touch:
			if (pobj->state>=3) return (0);
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++; snd_play (4,snd_hitenemy1);
				if (pobj->state>=3) {
					pobj->xl=20; pobj->yl=36;
					pobj->x+=5; pobj->y-=22;
					};
				explode1 (pobj->x+8,pobj->y+4,4,1);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_bee (int n, int msg, int z) {
	int sh=kindtable[obj_bee]*256;
	const int ydtab[4]={1,0,-1,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&3;
			if (++pobj->statecount>=4) pobj->statecount=0;
			if (pobj->xd!=0) pobj->substate=pobj->xd;
			if (xr_random(20)==0) pobj->xd=(pobj->xd>0)?0:pobj->substate;
			if (xr_random(40)==0) {
				snd_play (1,snd_bee);
				seekplayer (n,&pobj->xd,&pobj->yd);
				pobj->xd*=4; pobj->yd*=2;
				};
			if (!justmove(n,pobj->x+pobj->xd,pobj->y)) pobj->xd=-pobj->xd;
			justmove(n,pobj->x,pobj->y+ydtab[pobj->statecount]+pobj->yd);
			return (1);
		case msg_draw:
			if (pobj->xd==0) sh+=pobj->counter/2+4;
			else sh+=pobj->counter/2+((pobj->xd>0)?2:0);
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
		}; return (0);
	};

int msg_climber (int n, int msg, int z) {
	int sh=kindtable[obj_climber]*256;
	int k;
	const int climb_dwn[8]={7,6,5,4,3,2,1,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->info1==0) {
				pobj->info1=1; pobj->x+=4;
				if (pobj->yd==0) pobj->yd=2;
				};
			pobj->counter=(pobj->counter+1)&15;
			if (pobj->yd>0) {							// going down vine
				if (!objdo(n,pobj->x,pobj->y+pobj->yd+kindyl[obj_climber],
					f_notvine))	moveobj (n,pobj->x,pobj->y+pobj->yd);
				else pobj->yd=-pobj->yd;
				}
			else {
				if (!objdo(n,pobj->x,pobj->y+pobj->yd-kindyl[obj_climber],
					f_notvine)) moveobj (n,pobj->x,pobj->y+pobj->yd);
				else pobj->yd=-pobj->yd;
				}; return (1);
		case msg_draw:
			if (pobj->yd>0) {							// going down vine
				sh+=climb_dwn[pobj->counter/2];
				}
			else sh+=pobj->counter/2;				// going up vine
			drawshape(&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				if (!(stateinfo[objs[0].state]&sti_invincible)) {
					k=justmove(0,objs[0].x-16,objs[0].y);
					if (!k) k=justmove(0,objs[0].x+16,objs[0].y);
					objs[0].state=0; objs[0].substate=0;
					};
				hitplayer (n,0); return (1);
				};
		}; return (0);
	};

int msg_spider (int n, int msg, int z) {
	int sh=kindtable[obj_spider]*256;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (++pobj->counter>=18) pobj->counter=0;
			if (pobj->state>=2) {
				snd_play (4,snd_enemykill2);
				addscore (kindscore[obj_spider],pobj->x,pobj->y);
				killobj (n);
				};
			if (xr_random(40)==0) {
				snd_play (2,snd_spider); seekplayer (n,&pobj->xd,&pobj->yd);
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd; return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2+(pobj->xd<0)*9,
				pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy1);
				explode1 (pobj->x+30,pobj->y+4,5,0);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				}; return (1);
		}; return (0);
	};

int msg_skull (int n, int msg, int z) {
	int sh=kindtable[obj_skull]*256+2;
//	const int ydtab[4]={1,0,-1,0};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&7;
			if (!onscreen(n)) {killobj(n); return (1);};
			if (!justmove (n,pobj->x+pobj->xd,pobj->y)) {
				addobj (obj_hitwall,pobj->x+((pobj->xd>0)?16:-12),pobj->y,0,0);
				snd_play (2,snd_hitwall); killobj(n); return (1);
				};	pobj->xd--;
			if (pobj->xd<-10) pobj->xd=-10; return (1);
		case msg_draw:	drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); killobj (n); return (1);}
		}; return (0);
	};

int msg_seamonster (int n, int msg, int z) {
	int sh=kindtable[obj_seamonster]*256+22;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (++pobj->counter>=20) pobj->counter=0;
			if (xr_random(30)==0)
				addobj (obj_bubble,pobj->x+10,pobj->y+4,0,0); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/4,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,1); return (1);};
		}; return (0);
	};

int msg_robot (int n, int msg, int z) {
	int sh=kindtable[obj_robot]*256+14;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state>=3) {
				snd_play (4,snd_enemykill1);
				addscore (kindscore[obj_robot],pobj->x,pobj->y);
				killobj (n);
				};
			if (++pobj->counter>=16) pobj->counter=0;
			if ((pobj->counter&1)!=0) return (0);
			if (xr_random(40)==0) {
				seekplayer (n,&pobj->xd,&pobj->yd);	pobj->xd*=4;
				};
			if (!crawl (n,pobj->xd,0)) pobj->xd=-pobj->xd;
			if ((xr_random(50)==0)&&(pobj->xd!=0)) {
				addobj (obj_bullet,pobj->x-3+(pobj->xd>0)*28,
					pobj->y+12,sgn(pobj->xd)*4,0);
				objs[numobjs-1].state=1;
				snd_play (2,snd_enemyfire); 
				}; return (1);
		case msg_draw:
			sh+=((pobj->xd>0)?0:4)+pobj->counter/4;
			drawshape (&gamevp,sh,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) {hitplayer (n,0); return (1);};
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->state++;
				snd_play (4,snd_hitenemy4); explode2 (n);
				if (objs[z].objkind!=obj_fireball) killobj (z);
				};
		}; return (0);
	};

int msg_redfish (int n, int msg, int z) {
	int sh=kindtable[obj_redfish]*256+22;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&7;
			if (xr_random(20)==0) {
				pobj->xd=xr_random(3)*4-4;
				if (pobj->xd==0) pobj->yd=xr_random(2)*4-2;
				else pobj->yd=xr_random(3)*2-2;
				};
			if (!fishdo(n,pobj->x+pobj->xd,pobj->y))
				pobj->xd=-pobj->xd;
			if (!fishdo(n,pobj->x,pobj->y+pobj->yd))
				pobj->yd=-pobj->yd;
			if (xr_random(14)==0)
				addobj (obj_bubble,pobj->x+6,pobj->y-2,0,0); return (1);
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2+(pobj->xd>0)*4,
				pobj->x,pobj->y); break;
		case msg_touch:
			if (z==0) {
				hitplayer (n,1); explode1 (pobj->x+4,pobj->y+4,5,0);
				killobj (n); return (1);
				};
		}; return (0);
	};

int msg_xargon (int n, int msg, int z) {
	int sh=kindtable[obj_xargon]*256;
	int dist;
	const int ydtab[8]={1,2,1,0,-1,-2,-1,0};
	const int ydietab[8]={4,-2,6,-4,8,-6,8,-6};
	const int xdietab[8]={4,-4,4,-4,4,-4,4,-4};
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			pobj->counter=(pobj->counter+1)&15;
			if (pobj->state>0) pobj->state--;
			if (pobj->info1>0) pobj->info1--;
			if (pobj->substate>=50) {				// Xargon destroyed
				pobj->state=3;
				justmove(n,pobj->x+xdietab[pobj->counter/2],
					pobj->y+ydietab[pobj->counter/2]);
				if (pobj->counter/2==0) dim(); else undim();
				setcolor (250,20,20,23);
				explode1 (pobj->x+xr_random(54),pobj->y+xr_random(60),1,1);
				explode1 (pobj->x+xr_random(54),pobj->y+xr_random(60),1,0);
				addobj (obj_fireexpl,pobj->x+xr_random(60)-15,
					pobj->y+xr_random(68)-15,0,0);
				addobj (obj_hithero,pobj->x+xr_random(60)-12,
					pobj->y+xr_random(68)-17,0,0);
				if (pobj->statecount==0) {
					objs[0].state=st_still;
					snd_play (5,snd_enemykill1);
					addscore (kindscore[obj_xargon],pobj->x,pobj->y);
//					if (!invcount(inv_invin)) addinv (inv_invin);
					pl.health=5; statmodflg|=mod_screen;
					};
				if (pobj->statecount==58) {
					snd_play (4,snd_enemykill2);
					txt ("XARGON IS DEAD! Press ESC to continue",7,0);
					};
				if (++pobj->statecount>=60) {
					key=0; fire1off=1; fire2off=1;
					undim(); setcolor (250,20,20,23);
					snd_play (5,snd_enemykill1);
					do {checkctrl0(1);} while (key!=escape);
					gameover=2;
					}; return (1);
				};
			if (xr_random(20)==0) {
				seekplayer (n,&pobj->xd,0); pobj->xd*=4;
				};
			dist=vectdist(n,0);
			if ((dist<80)&&(pobj->xd<0)) pobj->xd=-pobj->xd;
			if ((dist>256)&&(pobj->xd>=0)) pobj->xd=-pobj->xd;
			if (!justmove(n,pobj->x+pobj->xd,pobj->y)) pobj->xd=-pobj->xd;
			justmove(n,pobj->x,pobj->y+ydtab[pobj->counter/2]);
			if ((xr_random(20)==0)&&(pobj->info1==0)&&(pobj->yd==0)) {
				pobj->info1=15;
				addobj (obj_skull,pobj->x-10,pobj->y+50,-2,0);
				pobj->state=5; snd_play (2,snd_enemyfire); 
				}; return (1);
		case msg_draw:
			if (pobj->statecount<59) {
				drawshape (&gamevp,sh+((pobj->state>0)?1:0),pobj->x,pobj->y);
				}; break;
		case msg_touch: if (pobj->substate>=50) return (0);
			if (z==0) hitplayer (n,0);
			if (kindflags[objs[z].objkind]&f_weapon) {
				pobj->substate++; snd_play (4,snd_hitenemy4);
				explode1 (pobj->x+xr_random(55),pobj->y+xr_random(60),2,1);
				explode1 (pobj->x+xr_random(55),pobj->y+xr_random(60),3,1);
				addobj (obj_fireexpl,pobj->x+xr_random(44),pobj->y+xr_random(52),0,0);
				addobj (obj_fireexpl,pobj->x+xr_random(44),pobj->y+xr_random(52),0,0);
				killobj (z);
				}; return (1);
		}; return (0);
	};
