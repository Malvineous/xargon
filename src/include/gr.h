//	GR.H:  Header of data types for shapes

void setcolor (int c, int n1, int n2, int n3);

#define byte unsigned char
#define x_modes 5
#define shm_maxtbls 64

extern int shm_want[shm_maxtbls];
extern char *shm_tbladdr[shm_maxtbls];
extern int shm_tbllen[shm_maxtbls];
extern int shm_flags[shm_maxtbls];

#define shm_fontf 	1
#define shm_xxxxx 	2
#define shm_blflag 	4

//	Shape Manager Stuff

#define st_byte		0
#define st_plain		1
#define st_rle			2

#define numcolors (1<<(numcolorbits))
#define colormask (numcolors-1)
#define shsize_cga(xl,yl) ((yl)*((xl+3)/4))
#define shsize_ega(xl,yl) (4*(yl)*((xl+7)/8))
#define shsize_vga(xl,yl) (4*yl*((xl+3)/4))

//	Graphics Driver Header

typedef struct {
	int vpx, vpy;							// Absolute start wrt Screen
	int vpxl, vpyl;						// Length
	int vpox, vpoy;						// Origin Offset
	int vphi, vpback;
	} vptype;

#define varincrx -16304

extern byte x_ourmode;
extern vptype mainvp;
extern unsigned int cmtab [4][256];
extern int pagemode, pageshow, pagedraw;
extern int showofs, drawofs, pagelen;
extern char pixvalue;

#define x_cga			0
#define x_cgagrey  	1
#define x_ega			2
#define x_egagrey		3
#define x_vga			4
#define x_mcga			6
#define x_modetype (x_ourmode & 0xfe)

extern void drawshape (vptype *vp, int n, int x, int y);
//	n<0 = erase shape (-n)
extern void clrvp (vptype *vp,byte col);
extern void scrollvp (vptype *vp,int xd,int yd);
extern void scroll (vptype *vp,int x0,int y0,int x1,int y1,int xd,int yd);
extern void plot (vptype *vp,int x,int y,int color);
extern void gr_init (void);
extern int  gr_config (void);
extern void gr_exit (void);
extern void fntcolor (int hi,int lo,int back);
extern void waitsafe (void);						// Wait until safe retrace
extern void setpagemode (int mode);
extern void setpages (void);
extern void pageflip (void);
extern void fadein (void);
extern void fadeout (void);
extern void clrpal (void);

typedef unsigned char p_rec[0x300];
extern void vga_setpal (void);
extern p_rec vgapal;

//	Shape Manager SHM.C

extern void shm_init (char *fname);
extern void shm_do (void);
extern void shm_exit (void);

//	Grl : Low-level graphics

//void ldrawsh_cga (vptype *vp, int draw_x, int draw_y, int sh_xlb, int sh_yl,
//   char far *shape, int cmtable);
//void ldrawsh_ega (vptype *vp, int draw_x, int draw_y, int sh_xlb, int sh_yl,
//   char far *shape, int cmtable);
void ldrawsh_vga (vptype *vp, int draw_x, int draw_y, int sh_xlb, int sh_yl,
   char far *shape, int cmtable);
//void lcopypage(void);

void pixaddr_cga (int x, int y, char **vidbuf, unsigned char *bitc);
void pixaddr_ega (int x, int y, char **vidbuf, unsigned char *bitc);
void pixaddr_vga (int x, int y, char **vidbuf, unsigned char *bitc);