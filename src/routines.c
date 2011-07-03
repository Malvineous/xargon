// Routines that may be needed for game

void calc_scroll (int peeky) {
	int xscroll=8,vpytop,vpybot,vpypeek;
	objtype *obj0;	obj0=&(objs[0]);
	
	if (obj0->xl==10) xscroll=4;
	if ((obj0->x)<(gamevp.vpox+112)) {
		if (gamevp.vpox>=24) scrollxd=-xscroll;
		}
	if ((obj0->x)>(gamevp.vpox+16*scrnxs-136)) {
		if (gamevp.vpox<(16*(boardxs-scrnxs-1))) scrollxd=xscroll;
		};

	vpytop=max (0,min (16*(boardys-scrnys+1),obj0->y-16*scrnys+96));
	vpybot=max (0,min (16*(boardys-scrnys+1),obj0->y-32));
	vpypeek=gamevp.vpoy+peeky;

	if ((vpypeek>=vpytop)&&(vpypeek<=vpybot)) scrollyd=peeky;
	else if (gamevp.vpoy>vpybot) scrollyd=vpybot-gamevp.vpoy;
	else if (gamevp.vpoy<vpytop) scrollyd=vpytop-gamevp.vpoy;
	};

int msg_pushblock (int n, int msg, int z) {
	switch (msg) {
		case msg_update:
			if (cando (n,objs[n].x,objs[n].y+1,
				f_playerthru|f_notstair)==(f_playerthru|f_notstair)) {
				if ((objs[n].yd+=2)>12) objs[n].yd=12;
				if (trymove(n,objs[n].x+objs[n].xd,objs[n].y+objs[n].yd)!=1) {
					trymove(n,objs[n].x,((objs[n].y+objs[n].yd-1)&0xfff0)+
					(16-kindyl[obj_pushblock]));
					objs[n].xd=0;
					};
				}; break;
		case msg_draw:	// drawshape (&gamevp,0x1f38,objs[n].x,objs[n].y); break;
				drawshape (&gamevp,info[stldr1].sh,objs[n].x,objs[n].y); break;
		case msg_touch:
			if (z==0) {
				if (cando (n,objs[n].x+objs[z].xd*8,objs[n].y,f_playerthru)) {
					objs [n].x+=objs[z].xd*8;
					};
				};
		}; return(1);
	};

#define obj_drop       75
#define obj_splash     76

defobj(75,obj_drop,msg_drop,12,8,"DROP",0,59,0);
defobj(76,obj_splash,msg_splash,34,6,"SPLASH",0,59,0);

int msg_drop (int n, int msg, int z) {
	int sh=kindtable[obj_drop]*256;
	int c;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update:
			if (pobj->state==0) {
				if (++pobj->counter>=60) {
					pobj->counter=0;
					addobj (obj_drop,pobj->x,pobj->y+2,0,2);
					objs[numobjs-1].state=1;
					};
				}
			else {
				if (!justmove(n,pobj->x,pobj->y+pobj->yd+2)) {
					c=((pobj->y+pobj->yd+2)&0xfff0)+16-pobj->yl;
					if ((c==pobj->y)||(!justmove(n,pobj->x,c))) {
						addobj (obj_splash,pobj->x-11,pobj->y+2,0,0);
						killobj (n);
						};
					};
				if ((pobj->yd+=2)>12) pobj->yd=12;
				}; return (1);
		case msg_draw:
			if (pobj->state==0) {
				drawshape (&gamevp,sh+12+pobj->counter/20,pobj->x,pobj->y);
				}
			else drawshape (&gamevp,sh+11,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) hitplayer (n,0);
		}; return (0);
	};

int msg_splash (int n, int msg, int z) {
	int sh=kindtable[obj_splash]*256+15;
	objtype *pobj; pobj=&(objs[n]);

	switch (msg) {
		case msg_update: if (++pobj->counter>=28) killobj (n); break;
		case msg_draw:
			drawshape (&gamevp,sh+pobj->counter/2,pobj->x,pobj->y); break;
		case msg_touch: if (z==0) hitplayer (n,0);
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
				drawshape (&gamevp,0x0300+score[c]-48,pobj->x+4*c,pobj->y);
				};
		}; return (0);
	};

		else if (key==6) {				// record end macro
			setpagemode(1); fadeout();
			loadboard ("board_33");
			init_inv();	statwin(1); drawstats(); setorigin();
			pl.level=33;
			p_reenter(0); drawboard();
			pageflip(); setpagemode(0); fadein();
			recordmac ("end_mac.xr1");
			play(0);
			sb_playtune (xintrosong);
			loadboard ("intro");	statwin(0);	snd_play (5,snd_menu);
			}
		else if (key==6) {				// record demo macro
			setpagemode(1); fadeout();
			loadboard (demoboard[0]);
			init_inv();	statwin(1); drawstats(); setorigin();
			pl.level=100;
			p_reenter(0); drawboard();
			pageflip(); setpagemode(0); fadein();
			recordmac ("temp_mac");
			play(0);
			sb_playtune (xintrosong);
			loadboard ("intro");	statwin(0);	snd_play (5,snd_menu);
			}
		else if (key==6) {				// start from any level
			setpagemode(1); fadeout();
			winput (&statvp,16,28,1,itoa(pl.level,lvl,10),2);
			strcpy (brd,"board_"); strcat (brd,lvl);
			loadboard (brd);
			init_inv();	statwin(1); drawstats(); setorigin();
			pl.level=atoi(lvl);
			p_reenter(0); drawboard();
			pageflip(); setpagemode(0); fadein();
			play(0);
			sb_playtune (xintrosong);
			loadboard ("intro");	statwin(0);	snd_play (5,snd_menu);
			}
		else if (key==7) {				// play sound effects
			nextsnd:
			if (numsnd<1) numsnd=1; if (numsnd>49) numsnd=49;
			fire1off=1; snd_play (5,numsnd);
			do {checkctrl0 (0);} while ((key!=escape)&&(key!=200)&&(key!=208));
			if (key==escape) numsnd=1;
			else if (key==200) {
				numsnd--; goto nextsnd;
				}
			else if (key==208) {
				numsnd++; goto nextsnd;
				};
			}