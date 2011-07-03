// X_EDITOR.C
//
// Xargon Game Editor
//
// by Allen W. Pilgrim

#include "port.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <alloc.h>
#include "include/gr.h"
#include "include/keyboard.h"
#include "include/windows.h"
#include "include/gamectrl.h"
#include "include/x_obj.h"
#include "include/xargon.h"

int disy;							// Y-disalignment
static int bc_array[normxs*normys];					// Block copy buffer

void infname (char *msg, char *fname) {
	fontcolor (&statvp,7,0);
	clearvp (&statvp);
	wprint (&statvp,2,1,1,msg);
	fontcolor (&statvp,2,0);
	winput (&statvp,2,13,1,fname,12);
	};

void printobjinfo (int n) {
	char tempstr[16];
	wprint (&statvp,2,1,2,    "kind:             ");
	wprint (&statvp,38,1,2,kindname[objs[n].objkind]);
	wprint (&statvp,2,11,2,   "stat:             ");
	wprint (&statvp,38,11,2,itoa(objs[n].state,tempstr,10));
	wprint (&statvp,2,21,2,   "xd:               ");
	wprint (&statvp,38,21,2,itoa(objs[n].xd,tempstr,10));
	wprint (&statvp,110,1,2,  "yd:               ");
	wprint (&statvp,140,1,2,itoa(objs[n].yd,tempstr,10));
	wprint (&statvp,110,11,2, "cnt:              ");
	wprint (&statvp,140,11,2,itoa(objs[n].counter,tempstr,10));
	if (kindflags[objs[n].objkind]&f_inside)
		wprint (&statvp,110,21,2,"Text Inside");
	wprint (&statvp,248,11,2, "Number: ");
	wprint (&statvp,296,11,2,itoa(n,tempstr,10));
	};

int objdesign (int dx, int dy) {				// Returns 1 if need redraw
	static int lastobj=0;
	int n;
	int fontnum;
	int objnum=-1;
	int editobj=0;
	char *ourobjname;
	char tempstr[64];

	dx = dx*16; dy = dy*16+disy;

	for (n=0; n<numobjs; n++) {
		if ((objs[n].x==dx)&&(objs[n].y==dy)) {
			objnum=n;
			ourobjname=kindname[objs[n].objkind];
			};
		};
	if (objnum==-1) ourobjname="NONE";

	fontcolor (&statvp,6,0);
	clearvp (&statvp);
	wprint (&statvp,2,1,2,  "Obj: Add Oov");
	wprint (&statvp,2,11,2, "     Del Paste");
	wprint (&statvp,2,21,2, "     Kopy Mod");
	wprint (&statvp,102,1,2,"Kind: ");
	fontcolor (&statvp,3,0);
	wprint (&statvp,102,11,2,ourobjname);
	fontcolor (&statvp,6,0);
	key=toupper (wgetkey(&statvp,102,21,2));
	clearvp (&statvp);
	switch (key) {
		case 'A':
			objnum=numobjs;
			addobj (obj_killme,dx,dy,0,0);
			editobj=1; break;
		case 'D':
			if (objnum>0) objs[objnum].objkind=obj_killme;
			return (1);
		case 'P':
			addobj (objs[lastobj].objkind,dx,dy,0,0);
			memcpy (&(objs[numobjs-1]),&(objs[lastobj]),
				sizeof(objs[lastobj]));
			objs[numobjs-1].x=dx;
			objs[numobjs-1].y=dy;
			return (1);
		case 'O':										// mOov object
			objs[lastobj].x=dx;
			objs[lastobj].y=dy;
			drawboard();
			return (1);
		case 'K':
			if (objnum>=0) lastobj=objnum; break;
		case 'M':
			if (objnum>=0) editobj=1; break;
		};

	if (editobj) {
		printobjinfo (objnum);
		strcpy (tempstr,kindname[objs[objnum].objkind]);
		winput (&statvp,38,1,2,tempstr,12);
		strupr (tempstr);
		for (n=0; n<numobjkinds; n++) {
			if (strcmp (tempstr,kindname[n])==0) {
				objs[objnum].objkind=n;	break;
				};
			};

		printobjinfo (objnum);
		itoa (objs[objnum].state,tempstr,10);
		winput (&statvp,38,11,2,tempstr,12);
		if (tempstr[0]!='\0') objs[objnum].state=atoi(tempstr);

		printobjinfo (objnum);
		itoa (objs[objnum].xd,tempstr,10);
		winput (&statvp,38,21,2,tempstr, 12);
		if (tempstr[0]!='\0') objs[objnum].xd=atoi(tempstr);

		printobjinfo (objnum);
		itoa (objs[objnum].yd,tempstr,10);
		winput (&statvp,140,1,2,tempstr, 12);
		if (tempstr[0]!='\0') objs[objnum].yd=atoi(tempstr);

		printobjinfo (objnum);
		itoa (objs[objnum].counter,tempstr,10);
		winput (&statvp,140,11,2,tempstr, 12);
		if (tempstr[0]!='\0') objs[objnum].counter=atoi(tempstr);

		objs[objnum].xl=kindxl[objs[objnum].objkind];
		objs[objnum].yl=kindyl[objs[objnum].objkind];

		if (kindflags[objs[objnum].objkind]&f_inside) {
			printobjinfo (objnum);
			if (objs[objnum].objkind==obj_text8) fontnum=1;
			else fontnum=2;
		if (objs[objnum].inside==NULL) tempstr[0]='\0';
		else strcpy (tempstr,(char*)objs[objnum].inside);

		fontcolor (&gamevp,objs[objnum].xd,objs[objnum].yd);
		winput (&gamevp,objs[objnum].x,objs[objnum].y,fontnum,tempstr, 64);

		if (objs[objnum].inside!=NULL) free (objs[objnum].inside);
		objs[objnum].inside=malloc (strlen(tempstr)+1);
		strcpy (objs[objnum].inside, tempstr);
		setobjsize (objnum);
		};
		printobjinfo (objnum);

		shm_want[kindtable[objs[objnum].objkind]]=1;
		shm_do();
		return (1);
		};
	return (0);
	};

void design (void) {
	int updflag=0;
	int dx,dy,tempx,n;
	int lastcell=1;
	int tempint;
	int drawmode=0;
	char tempstr[32];
	char tempfname[32];
	char tempstr2[12];
	char tempstr3[32];
	int new_col=0;
	int bc_x=0,bc_y=0,bc_w=0,bc_h=0;		// Block copy x,y and width,height
	int tx,ty;

	disy=0;
	designflag=1;
	gamecount=0;
	tempstr[0]='\0';
	tempfname[0]='\0';
	setorigin();
	dx=objs[0].x/16;
	dy=objs[0].y/16;
	drawboard();
	fontcolor (&statvp,1,0);
	clearvp (&statvp);

	do {
		if (drawmode) {
			setboard (dx,dy,lastcell);
			drawcell (dx,dy);
			updflag=1;
			}
		fontcolor (&statvp,3,0);						// memory usage
		wprint (&statvp,248,1,2,"       ");
		ultoa (coreleft(),tempstr3,10);
		wprint (&statvp,290-(1+strlen(tempstr3)*6),1,2,tempstr3);
		tempstr3[0]='\0';

		fontcolor (&statvp,1,0);
		wprint (&statvp,248,21,2, "# objs: ");
		fontcolor (&statvp,3,0);
		wprint (&statvp,296,21,2,itoa(numobjs,tempstr2,10));

		drawshape (&gamevp,0x0100,dx*16+4,dy*16+4);
		do {
			checkctrl(0);
			} while ((dx1==0)&&(dy1==0)&&(key==0)&&(updflag==0));
		updflag=0;
		modboard(dx,dy);
		upd_objs(0);
		refresh(0);
		purgeobjs();
		if ((dx1!=0)||(dy1!=0)) {
			dx+=dx1*(1+fire1*(scrnxs/2-1));
			dy+=dy1*(1+fire1*(scrnys/2-1));
			if (dx<0) dx=0;
			if (dx>=boardxs) dx=boardxs-1;
			if (dy<0) dy=0;
			if (dy>=boardys) dy=boardys-1;
			if ((dx*16)<gamevp.vpox) {
				gamevp.vpox-=scrnxs*8;
				if (gamevp.vpox<0) gamevp.vpox=0;
				drawboard();
				};
			if ((dx*16)>=(gamevp.vpox+16*scrnxs-16)) {
				gamevp.vpox+=scrnxs*8;
				if (gamevp.vpox>=(16*(boardxs-scrnxs)+8))
					gamevp.vpox=16*(boardxs-scrnxs)+8;
				drawboard();
				};
			if ((16*dy)<gamevp.vpoy) {
				gamevp.vpoy-=scrnys*8;
				if (gamevp.vpoy<0) gamevp.vpoy=0;
				drawboard();
				};
			if ((16*dy)>=(gamevp.vpoy+16*(scrnys-1))) {
				gamevp.vpoy+=scrnys*8;
				if (gamevp.vpoy>=(16*(boardys-scrnys+1)))
					gamevp.vpoy=(boardys-scrnys+1)*16;
				drawboard();
				};
			};

		switch (toupper(key)) {
			case k_f1:					// Mark top left corner of block
				bc_x=dx; bc_y=dy; break;
			case k_f2:
				// Mark bottom right corner of block and copy
				//	block of tile numbers into temp array bc_array
				if(dx<bc_x) break;
					// 0-width or negative width rectangle	(can't do that)
				if(dy<bc_y) break;
					// 0-height or negative height rectangle (can't do that)
				bc_w=(dx-bc_x)+1; bc_h=(dy-bc_y)+1;
				if((bc_w>normxs)||(bc_h>normys)) {			// Too big!!
					bc_w=0; bc_h=0; break;
					};
				for(ty=0; ty<bc_h; ty++) {
					for(tx=0; tx<bc_w; tx++) {
						bc_array[(ty*bc_w)+tx]=board(bc_x+tx,bc_y+ty);
						};
					};	break;
			case k_f3:
				for(ty=0; ty<bc_h; ty++) {
					for(tx=0; tx<bc_w; tx++) {
						setboard(dx+tx, dy+ty, bc_array[(ty*bc_w)+tx]);
						};
					};	updflag=1; break;
			case k_f4:
				do {
					upd_colors (); gamecount++; checkctrl0(0);
					} while (key==0); break;
			case enter:
				clearvp (&statvp);
				wprint (&statvp,2,1,1,"Put:");
				fontcolor (&statvp,6,0);
				winput (&statvp,2,11,1,tempstr,16);
				strupr (tempstr);
				for (tempint=0; tempint<numinfotypes; tempint++) {
					if (strcmp (tempstr,info[tempint].na)==0) {
						lastcell=tempint;
						setboard(dx,dy,tempint);
						shm_want[(info[tempint].sh>>8)&0x3f]=1;
						shm_do(); break;
						};
					};
				updflag=1; break;
			case 9: drawmode=!drawmode; break;				// tab
			case 'K': lastcell=board(dx,dy); break;
			case ' ':
				setboard(dx,dy,lastcell);
				updflag=1; break;
			case 'I':
				pl.score=1000;
				printhi (1);
				pl.score=0; break;
			case 'V':
				if (pl.numinv==0) addinv (inv_hero);
				else {
					pl.numinv=0;
					init_inv();
					};
				pl.score=0;
				pl.level=0; break;
			case 'H':
				tempint=board(dx,dy);
				tempx=dx;
				while (board(tempx,dy)==tempint) {
					setboard(tempx,dy,lastcell);
					drawcell (tempx,dy);
					tempx--;
					};
				tempx=dx+1;
				while (board(tempx,dy)==tempint) {
					setboard(tempx,dy,lastcell);
					drawcell (tempx,dy);
					tempx++;
					}; break;
			case 'O': updflag=objdesign(dx,dy); break;	// Object Mgmt
			case 'U':
				for (n=0; n<numobjs; n++) {
//					if (((kindflags[objs[n].objkind]&f_inside)==0)&&
//						(objs[n].inside!=NULL)) {
//						objs[n].inside=NULL;
//						sound(240);
//						delay(500);
//						nosound();
//						};
					setobjsize (n);
					}; break;
			case 'Z':
				infname ("Clear?",tempfname);
				if (toupper(tempfname[0])=='Y') {
					init_brd();
					init_objs();
					drawboard();
					}; break;
			case 'L':
				infname ("Load:",tempfname);
				if (tempfname[0]!='\0') {
					loadboard (tempfname);
					setorigin();
					dx=objs[0].x/16; dy=objs[0].y/16;
					drawboard();
					}; break;
			case 'Y':												// Disalign Y
				clearvp (&statvp);
				wprint (&statvp,2,1,1,"Dis Y:");
				itoa (disy,tempstr,10);
				winput (&statvp,2,11,1,tempstr,16);
				disy=atoi (tempstr);
				strupr (tempstr); break;
			case 'N':
				infname ("New board?",tempfname);
				if (toupper (tempfname[0])=='Y') {
					zapobjs();
					init_brd();
					}; break;
			case 'S':
				infname ("Save:",tempfname);
				if (tempfname[0]!='\0') saveboard (tempfname); break;
			case 'C':
				clearvp (&statvp);
				wprint (&statvp,2,1,1,"New Color:");
				fontcolor (&statvp,6,0);
				itoa (new_col,tempstr,10);
				winput (&statvp,2,11,1,tempstr,16);
				new_col=atoi (tempstr);

				switch (new_col) {
					case 0: setcolor (250,0,0,0);
						setcolor (251,0,0,0); break;				// reset all
					case 1: setcolor (251,0,0,0); break;		// reset #251
					case 2: setcolor (250,0,0,32);		 		// dk. blue sky
						setcolor (251,0,0,32); break;
					case 3:												// lt. blue sky
						setcolor (176,8,16,25); setcolor (177,8,20,29);
						setcolor (178,12,24,33); setcolor (179,16,28,41);
						setcolor (180,20,32,45); setcolor (181,24,40,49);
						setcolor (182,28,44,57); setcolor (183,36,48,60);
						setcolor (250,36,48,60); setcolor (251,36,48,60); break;
					case 4:												// yellow sky
						setcolor (176,32,0,0); setcolor (177,40,0,0);
						setcolor (178,52,0,0); setcolor (179,60,0,0);
						setcolor (180,60,28,0); setcolor (181,60,40,0);
						setcolor (182,60,52,0); setcolor (183,60,60,0);
						setcolor (250,60,60,0);	setcolor (251,60,60,0); break;
					case 5:												// emerald sky
						setcolor (176,0,12,12); setcolor (177,0,18,17);
						setcolor (178,0,25,23); setcolor (179,0,32,27);
						setcolor (180,0,39,32); setcolor (181,0,46,35);
						setcolor (182,0,53,38); setcolor (183,0,60,40);
						setcolor (250,0,60,40); setcolor (251,0,60,40); break;
					case 6: setcolor (250,32,32,24);				// olive green
						setcolor (251,32,32,24); break;
					case 7:												// violet sky
						setcolor (176,13,5,22); setcolor (177,18,8,27);
						setcolor (178,23,13,33); setcolor (179,29,19,39);
						setcolor (180,35,25,45); setcolor (181,42,32,51);
						setcolor (182,49,40,57); setcolor (183,57,50,63);
						setcolor (250,57,50,63); setcolor (251,57,50,63); break;
					case 8: setcolor (250,23,23,23);				// factory grey
						setcolor (251,23,23,23); break;
					case 9: setcolor (250,12,23,63);		 		// royal blue
						setcolor (251,12,23,63); break;
					case 10: setcolor (250,20,20,23);			// factory grey v3
						setcolor (251,20,20,23); break;
					};
			};
		} while (key!=escape);
	key=0;
	designflag=0;
	};
