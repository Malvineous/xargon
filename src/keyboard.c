/* Keyboard Driver */

#include <dos.h>;
#include <mem.h>;
#include <stdlib.h>;
#include "\develop\xargon\include\keyboard.h";

char k_rshift, k_lshift, k_shift, k_ctrl, k_alt, k_numlock;
void k_status (void);
int k_pressed (void);
int k_read (void);
unsigned char keydown[2][256];
void interrupt (*oldint9)(void);
unsigned char bioscall;
int e0code;

int *bhead; /* $40:$1a */
int *btail; /* $40:1c  */

int k_pressed (void) {
   struct REGPACK r;
   r.r_ax = 0x0100;
   intr (0x16,&r);
   if (!((r.r_flags>>6)&1)) {
      return ((r.r_ax&0x00ff)+(128|(r.r_ax>>8))*((r.r_ax&0x00ff)==0));
      }
   return (0);
   }

int k_read (void) {
   struct REGPACK r;
   r.r_ax = 0;
   intr (0x16,&r);
   return ((r.r_ax & 0x00ff)+(128|(r.r_ax>>8))*((r.r_ax&0x00ff)==0));
   }

void k_status (void) {
   struct REGPACK r;
   r.r_ax = 0x0200;
   intr (0x16,&r);
   k_rshift  = (r.r_ax & 1);
   k_lshift  = (r.r_ax >> 1) & 1;
   k_ctrl    = (r.r_ax >> 2) & 1;
   k_alt     = (r.r_ax >> 3) & 1;
   k_numlock = (r.r_ax >> 5) & 1;
   k_shift   = k_rshift | k_lshift;
   }

void interrupt handler (void) {
   asm sti;		/* Enable interupts */
   asm xor  ax,ax;      /* Insure Ah is 0 */
   asm in   al,60h;     /* read data byte */
   asm cmp  al,0E0h;    /* Check for Xtended code */
   asm jne  makebreak;  /* If not then it's a make/break */
   asm mov  e0code,256; /* If so, store the code */
   asm jmp  exit;       /* Then exit */
   makebreak:
   asm test al,80h;     /* Check if make or break */
   asm jnz  breakit;    /* Goto breakit or continue for makeit */
   asm mov  bx,ax;      /* Get position to store */

   /*asm add  bx,e0code;*/  /* If E0code then point to Xarray */
   /* Removed -- don't want to track both!! */

   asm mov  e0code,0;   /* Clear E0code */
   asm mov  byte ptr keydown[bx],1; 	/*Store the make code */
   asm jmp  exit;       /* Alldone */

   breakit:
   asm and  al,7fh;     /* Clear Make/Break bit */
   asm mov  bx,ax;      /* Get position to store */

   /*asm add  bx,e0code;*/  /* If E0code then point to Xarray */

   asm mov  e0code,0;   /* Clear e0code */
   asm mov  byte ptr keydown[bx],0; /* Store the break code */

   exit:

   asm cmp bioscall,1;  /* Check if bios is to be notified */
   asm jne endi;        /* If not, End Interupt */
   asm pushf;           /* If so, call BIOS chain */
   asm call dword ptr oldint9;
   asm jmp doreturn;    /* And then exit properly */
   endi:
   asm cli;
   asm mov  AL,20h;     /* SEND AN EOI */
   asm out  20h,al;     /* TO THE INTERRUPT CONTROLLER */
   doreturn:
   return;
   };

void installhandler (unsigned char status) {
   (long) bhead = 0x0040001a; /* $40:$1a */
   (long) btail = 0x0040001c; /* $40:1c  */
   memset (&keydown,sizeof(keydown),0);
   oldint9 = getvect (9);
   bioscall = status;
   setvect (9,handler);
   }

void removehandler (void) {
   setvect (9,oldint9);
   };

void disablebios (void) {
   bioscall=0;
   };

void enablebios (void) {
   *bhead   = *btail;      /*Clear the BIOS KB buffer*/
   bioscall = 1;
   };

int biosstatus (void) {
   if (bioscall==0) {return 0;}
   else {return 1;};
   };
