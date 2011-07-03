//	Win.H:  Window header file

//	Flags
#define dialog 1
#define textbox 2

typedef struct {
	int winflags;
	int winx, winx8, winy;
	int winxl, winxl16, winyl, winyl16;
	int winh, winh16, winv, winv16;
	vptype border;
	vptype inside;
	vptype topleft;
	} wintype;

extern void defwin (wintype *win,int x8,int y,int xl16,int yl16,int h16,int v16,int flags);
extern void undrawwin (wintype *win);
extern void drawwin (wintype *win);
extern void wprint (vptype *vp,int x,int y,int font,char *text);
extern int wgetkey (vptype *vp,int x,int y,int font);
extern void wprintc (vptype *vp,int y,int font,char *text);
extern void winput (vptype *vp,int x,int y,int font,char *text,int maxlen);
extern void initvp (vptype *vp,int bkgnd);
extern void clearvp (vptype *vp);
extern void fontcolor (vptype *vp,int hi,int back);

extern void titlewin (wintype *win,char *text,int flg);