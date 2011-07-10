#ifndef _PORT_H_
#define _PORT_H_

#define far

#include <stdio.h>
//#include <stdlib.h>

char *ultoa(unsigned long val, char *str, int base);
#ifndef __WIN32
char *itoa(int val, char *str, int base);
char *ltoa(long val, char *str, int base);
char *strupr(char *s);
unsigned long filelength(int fd);
#define O_BINARY  0 // unused on POSIX
#endif

#define _creat creat
#define _close close
#define _open open
#define _read read
#define _write write

unsigned long coreleft(void);

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
void delay(int s);
int getclock();

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

void cputs(const char *s);
void textcolor(int c);
void textbackground(int c);
void enable(void);
void disable(void);
void window(int x, int y, int w, int h);
void gotoxy(int x, int y);
void clrscr(void);
int xr_random(int max);
void setmem(void *target, unsigned long len, char val);
unsigned char inportb(unsigned int port);
unsigned char outportb(unsigned int port, unsigned char value);
unsigned char outport(unsigned int port, unsigned int value);

struct REGPACK {
	unsigned int r_ax, r_bx, r_cx, r_dx;
};
void intr(int interrupt, struct REGPACK *regs);

void lcopypage(void);

#define pal6to8(x) (((x) << 2) | ((x) >> 4))

// xargon.c

void moddrawboard(void);
void drawstats(void);
void loadboard(char *fname);
void saveboard(char *fname);
void printhi(int newhi);

// graphics.c
void dim(void);
void undim(void);

// music
#define interrupt
void nosound(void);

typedef void (*intfunc)(void);

void setvect(int interrupt, void (*handler)(void));
intfunc getvect(int interrupt);

// SDL stuff
#include <SDL.h>
extern SDL_Surface *screen;

#endif // _PORT_H_
