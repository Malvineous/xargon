#include "port.h"

SDL_Surface *screen;
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#include <stdio.h>

char *ultoa(unsigned long val, char *str, int base)
{
	sprintf(str, "%lu", val);
	return str;
}
char *itoa(int val, char *str, int base)
{
	sprintf(str, "%d", val);
	return str;
}
char *ltoa(long val, char *str, int base)
{
	sprintf(str, "%ld", val);
	return str;
}
char *strupr(char *s)
{
	char *o = s;
	do {
		if ((*s >= 'a') && (*s <= 'z')) *s -= 'a' - 'A';
	} while (*s++);
	return o;
}

unsigned long coreleft(void)
{
	return 1234;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
unsigned long filelength(int fd)
{
	struct stat buf;
	fstat(fd, &buf);
	return buf.st_size;
}

void delay(int ms)
{
	usleep(ms * 1000);
	return;
}

int getclock()
{
	return SDL_GetTicks() / 54.9450549;
}

void cputs(const char *s)
{
	printf("%s", s);
	fflush(stdout);
	return;
}

void textcolor(int c)
{
	printf("\033[%d;%dm", (c & 8) ? 1 : 22, 30 + (((c & 1) << 2) | ((c & 4) >> 2) | (c & 2)));
	return;
}

void textbackground(int c)
{
	printf("\033[%dm", 40 + ((c & 1) << 2) | ((c & 4) >> 2) | (c & 2));
	return;
}

void enable(void)
{
	// ???
}

void disable(void)
{
	// ???
}

void window(int x, int y, int w, int h)
{
	// ???
}

void gotoxy(int x, int y)
{
	printf("\033[%d;%dH", x, y);
	return;
}

void clrscr(void)
{
	printf("\033[2J");
	return;
}

#include <stdlib.h>
int xr_random(int max)
{
	return random() / (RAND_MAX / max);
}

#include <string.h>
void setmem(void *target, unsigned long len, char val)
{
	memset(target, val, len);
	return;
}

unsigned char inportb(unsigned int port)
{
	switch (port) {
		case 0x201: // joystick
			return 0;
	}
	fprintf(stderr, "Read from unknown portb 0x%02X\n", port);
	return 0;
}

unsigned char outportb(unsigned int port, unsigned char value)
{
	switch (port) {
		case 0x201: // joystick
			break;
		case 0x3C8: // VGA
		case 0x3C9:
			break;
		default:
			fprintf(stderr, "Write to unknown portb 0x%04X: 0x%02X\n", port, value);
			break;
	}
	return 0;
}

unsigned char outport(unsigned int port, unsigned int value)
{
	switch (port) {
		case 0x201: // joystick
			return 0;
		case 0x3C4: // VGA
		case 0x3CE:
		case 0x3D4:
			break;
		default:
			fprintf(stderr, "Write to unknown portw 0x%04X: 0x%04X\n", port, value);
			break;
	}
	return 0;
}

void intr(int intnum, struct REGPACK *regs)
{
	switch (intnum) {
		case 0x10: // video
			printf("TODO: video int 10 ax %04X\n", regs->r_ax);
			break;
	}
	return;
}

void nosound(void)
{
	return;
}

void spkr_intr(void)
{
}

void setvect(int intnum, void (*handler)(void))
{
	// TODO
	return;
}

intfunc getvect(int interrupt)
{
	return NULL;
}

int k_pressed(void)
{
	// TODO
	return 0;
}

int k_read(void)
{
	// TODO
	return 0;
}

char k_rshift, k_lshift, k_shift, k_ctrl, k_alt, k_numlock;

void k_status(void)
{
	SDLMod state = SDL_GetModState();
	k_rshift = (state & KMOD_RSHIFT) ? 1 : 0;
	k_lshift = (state & KMOD_LSHIFT) ? 1 : 0;
	k_shift = (state & KMOD_SHIFT) ? 1 : 0;
	k_ctrl = (state & KMOD_CTRL) ? 1 : 0;
	k_alt = (state & KMOD_ALT) ? 1 : 0;
	k_numlock = (state & KMOD_NUM) ? 1 : 0;
	return;
}

unsigned char keydown[2][256];

void installhandler(unsigned char status)
{
	// no op
	return;
}

void removehandler(void)
{
	// no op
	return;
}

#include "include/gr.h"
void clrvp (vptype *vp,byte col)
{
	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	// TODO: optimise by incrementing shape ptr instead
	for (int y = 0; y < vp->vpyl; y++) {
		for (int x = 0; x < vp->vpxl; x++) {
			((char *)::screen->pixels)[(vp->vpy + y) * SCREEN_WIDTH + (vp->vpx + x)] = col;
		}
	}

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);
	return;
}

void scrollvp (vptype *vp,int xd,int yd)
{
	// TODO
	printf("todo: scrollvp\n");
	return;
}

void scroll (vptype *vp,int x0,int y0,int x1,int y1,int xd,int yd)
{
	// TODO
	printf("todo: scroll\n");
	return;
}

void plot_vga (int x, int y, byte color)
{
	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	((char *)::screen->pixels)[y * SCREEN_WIDTH + x] = color;

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);

	return;
}

#include <fcntl.h>
void ldrawsh_vga (vptype *vp, int draw_x, int draw_y, int sh_xlb, int sh_yl,
	char far *shape, int cmtable)
{
	draw_x += vp->vpx;
	draw_y += vp->vpy;

	// If the draw starts off the edge of the screen, skip that bit
	int startx = 0, y = 0;
	/* The game seems to ignore these entirely (see top two rows of screen pixels at main menu)
	if (draw_y < 0) y += -draw_y;*/
	if (draw_x < 0) startx += -draw_x;

	if ((draw_x < 0) || (draw_y < 0)) {
		//printf("Tried to draw at negative screen coordinates! (%d,%d)\n", draw_x, draw_y);
		return;
	}

	if (SDL_MUSTLOCK(::screen)) {
		if (SDL_LockSurface(::screen) < 0) return;
	}

	// TODO: optimise by incrementing shape ptr instead
	for (; y < sh_yl; y++) {
		if (draw_y + y >= SCREEN_HEIGHT) break;
		for (int x = startx; x < sh_xlb; x++) {
			if (draw_x + x >= SCREEN_WIDTH) break;
			uint8_t pixel = shape[y * sh_xlb + x];
			pixel = cmtab[cmtable][pixel];
			if (pixel == 255) continue; // transparent
			((char *)::screen->pixels)[(draw_y + y) * SCREEN_WIDTH + (draw_x + x)] = pixel;
		}
	}

	if (SDL_MUSTLOCK(::screen)) SDL_UnlockSurface(::screen);

	return;
}

void lcopypage(void)
{
	SDL_Flip(::screen);
	// ???
}

void uncrunch (char *sourceptr, char *destptr, int length)
{
	// TODO
	if (destptr == (char *)0xb8000000) {
		printf("decrunch to text screen\n");
	} else {
		memset(destptr, 0, length);
	}
	return;
}

void WorxBugInt8(void)
{
}

extern "C" {

void StartWorx(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	::screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8, SDL_HWPALETTE | SDL_DOUBLEBUF);
}

void CloseWorx(void)
{
	SDL_Quit();
}

int AdlibDetect(void)
{
	return 1;
}

int SetFMVolume(unsigned char left, unsigned char right)
{
	return 0;
}

int DSPReset(void)
{
	return 0; // no DSP
}

void DSPClose(void)
{
	return;
}

int SetMasterVolume(unsigned char left, unsigned char right)
{
	return 0;
}

void StopSequence(void)
{
	return;
}

char *GetSequence(char *f_name)
{
	// TODO: load file contents?
	return NULL;
}

void SetLoopMode(int m)
{
	return;
}

int VOCPlaying(void)
{
	return 0;
}

int PlayVOCBlock(char *voc, int volume)
{
	return 0;
}

void PlayCMFBlock(char *seq)
{
	return;
}

} // extern C
