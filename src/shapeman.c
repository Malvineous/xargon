//	SHM.C:  Shape manager for

//	CGA/EGA/VGA/CGA GRAY/EGA GRAY

//	Handles the following functions:
//		shm_init :Init shape manager
//		shm_do   :Prepare shape tables (load/trash) using shm_want[]
//		shm_exit :Terminate shape manager

#include <stdlib.h>
//#include <io.h>
//#include <alloc.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
//#include <mem.h>
#include "include/gr.h";

extern void rexit (int num);

int shm_want[shm_maxtbls];
char *shm_tbladdr[shm_maxtbls];
int shm_tbllen[shm_maxtbls];
int shm_flags[shm_maxtbls];

char shm_fname[80];
char colortab[256];

void shm_init (char *fname) {
	int c;

	strcpy (shm_fname, fname);
	for (c=0; c<shm_maxtbls; c++) {
		shm_want[c]=0;
		shm_tbladdr[c]=NULL;
		};
	};

void init8bit (void) {					// Init pallette for numcolorbits=8
	int x;

	for (x=0; x<256; x++) {
		colortab[x]=x;
		};
	};

#define readin(dest,addr,len) memcpy (&dest,addr,len); addr+=len
#define wr(addr,ofs,src,len) memcpy (addr+ofs,&src,len); ofs+=len
#define wr1(addr,ofs,byt) *(addr+(ofs++))=byt

void xlate_table (int n, char *addr, char *bucket1) {
	unsigned char numshapes=0;
	char numcolorbits=1;
	//int16_t numrots;
	uint16_t numrots;
	int32_t longcolor;
	//int16_t len_cga, len_ega, len_vga, len;
	uint16_t len_cga, len_ega, len_vga, len;
	byte xs, xsb, ys;
	byte storetype;
	byte c;
	int16_t x,y,b,flags;
	//uint16_t x,y,flags;
	//int b;
	char *bucket,*tempbucket;
	unsigned char databyte,shapebyte;
	unsigned int colorand, colorshift;
	char *dest;
	//int16_t tblofs, dataofs;
	uint16_t tblofs, dataofs;

	readin (numshapes, addr, 1);
	readin (numrots, addr, 2);
	readin (len_cga, addr, 2);
	readin (len_ega, addr, 2);
	readin (len_vga, addr, 2);
	readin (numcolorbits, addr, 1);
	readin (flags, addr, 2);

	len=len_vga+16;
	colorand=255;
	colorshift=16;

	if (flags & shm_fontf) {
		for (c=0; c<(numcolors); c++) colortab[c]=c;
		}
	else if (numcolorbits==8) init8bit();
	else {
		for (c=0; c<numcolors; c++) {			// Numcolors is a macro
			readin (longcolor, addr, 4);
			colortab [c]=(longcolor>>colorshift)&colorand;
			};
		};

	dest=malloc (len);
	if (dest==NULL) rexit (9);
	shm_tbllen[n]=len;
	shm_tbladdr[n]=dest;
	shm_flags[n]=flags;
	tblofs=0;
	dataofs=numshapes*4;

	for (c=0; c<numshapes; c++) {
		readin (xs, addr, 1);
		readin (ys, addr, 1);
		readin (storetype, addr, 1);
		bucket=bucket1;
		//	Copy the actual pixel bitmap of the shape into bucket
		if (storetype==st_byte) {
			memcpy (bucket, addr, xs*ys);
			addr+=(xs*ys);
			};

//	Now the shape definition is in memory at BUCKET
//	Translate to our mode:
		if ((numcolorbits==8)&&(xs==64)&&(ys==12)) {
			memmove (&vgapal,bucket,sizeof (vgapal));
			vga_setpal();
			};

		xsb=xs;
		wr (dest, tblofs, dataofs, 2);
		wr1 (dest, tblofs, xsb);
		wr1 (dest, tblofs, ys);
flags=0;
		if (flags & shm_blflag) {
			for (b=3; b>=0; b--) {
				for (y=0; y<ys; y++) {
					for (x=0; x<xs; x+=4) {
						shapebyte=colortab[*(byte*)(bucket+(x+b)+y*xs)];
						wr1 (dest,dataofs,shapebyte);
						};
					};
				};
			}
		else {
			for (y=0; y<ys; y++) {
				for (x=0; x<xs; x++) {
					shapebyte=colortab[(*(byte*)(bucket+x+y*xs))];
					wr1 (dest, dataofs, shapebyte);
					};
				};
			};
		};
	};

void shm_do (void) {
	int32_t shoffset[128];
	char *shaddr[128];
	//int shlen[128];					// Dual purpose
	uint16_t shlen[128];					// Dual purpose
	int shafile;
	int c;
	char*bucket1;
 
	bucket1=malloc (4096);
	if (bucket1==NULL) rexit (9);
	
	for (c=0; c<shm_maxtbls; c++) {
		shaddr [c] = NULL;
		};
  
	shafile=_open (shm_fname,O_BINARY|O_RDONLY);
	// Read 128 long ints ... offsets of shape files in big file
	if (!read (shafile,shoffset,128*4)) rexit(9);

	// Read 128 words (short ints) ... lengths of each shape file
	read (shafile,shlen,128*2);
 
	for (c=0; c<shm_maxtbls; c++) {
		if (!shm_want[c]) {
	// This shape file exists, but not needed now.  Purge it from memory
			if (shm_tbladdr[c]!=NULL) {
//PORT				free (shm_tbladdr[c]);
				shm_tbladdr[c]=NULL;
				};
			}
		else if ((shm_tbladdr[c]==NULL)&&(shlen[c]!=0)) {
	// Shape file 'c' is needed, but not loaded, so load it.
			lseek (shafile, shoffset [c], SEEK_SET);	// Seek to position in file
			shaddr[c]=malloc (shlen[c]);					// Allocate mem to hold it
			if (shaddr[c]==NULL) rexit (9);
			read (shafile, shaddr[c], shlen[c]);		// Read it in
			};
		};

	close (shafile);
	for (c=0; c<shm_maxtbls; c++) {
		if (shaddr[c]!=NULL) {
			xlate_table (c,shaddr[c], bucket1);
//PORT			free (shaddr[c]);
			};
		};
//PORT	free (bucket1);
	};

void shm_exit (void) {
	int c;
	for (c=0; c<shm_maxtbls; c++) {
		if (shm_tbladdr[c]!=NULL) {
			free (shm_tbladdr[c]);
			shm_tbladdr[c]=NULL;
			};
		};
	};
