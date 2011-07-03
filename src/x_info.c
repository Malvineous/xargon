// X_INFO.C
//
// Xargon Info
//
// by Allen W. Pilgrim

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include "\develop\xargon\include\gr.h"
#include "\develop\xargon\include\windows.h"
#include "\develop\xargon\include\x_obj.h"
#include "\develop\xargon\include\xargon.h"

extern char tilefile[];

void init_info (void) {
	int c,d,temp;
	char len;
	int flagsdef=f_notvine|f_notstair|f_notwater;
	int blockfile;

	for (c=0; c<numinfotypes; c++) {
		info [c].sh=0x4500;
		info [c].na="";
		info [c].flags=flagsdef;
		};

	blockfile=open (tilefile,O_BINARY);
	while (read(blockfile,&c,2)>0) {
		read (blockfile,&info[c].sh,2);
		read (blockfile,&temp,2);
		info[c].flags^=temp;
		read (blockfile,&len,1);
		info [c].na=malloc (len+1);
		read (blockfile,info[c].na,len);
		info[c].na[len]=0;
		};

	for (c=0; c<numstates; c++) {
		stateinfo [c]=0;
		};

	stateinfo [st_begin]    |= sti_invincible;
	stateinfo [st_stand]    |= sti_canfire;
	stateinfo [st_still]    |= sti_invincible;
	stateinfo [st_jumping]  |= sti_canfire;
	stateinfo [st_climbing] |= 0;
	stateinfo [st_die]      |= sti_invincible;
	stateinfo [st_transport]|= sti_invincible;
	stateinfo [st_platform] |= sti_canfire;
	};