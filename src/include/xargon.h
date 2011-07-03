// Xargon Header

#define boardxs 128
#define boardys 64
#define normxs 21
#define normys 11
#define fullxs 20
#define fullys 13

#define sgn(x) ((x>0)-(x<0))

extern vptype gamevp, statvp, textvp, tempvp;
extern int scrnxs,scrnys;
extern int bd [boardxs][boardys];

#define board(x,y) (bd[x][y]&0x3fff)
#define modboard(x,y) bd[x][y]|=mod_screen;
#define setboard(x,y,n) bd[x][y]=n|mod_screen;

extern int scrollxd, scrollyd, oldscrollxd, oldscrollyd, oldx0, oldy0;
extern char newlevel[16],curlevel[16];

// for non-pageflipping:
#define mod_screenonly (0x8000)
#define mod_screen (0xc000)	// modflag&this = 1 if screen has changed
#define mod_virtual (0x4000)	// &3: 0=no mod, 1=virtual mod, 3=full mod

// For pageflipping:
#define mod_page0 (0x4000)		// modflag&(pagedraw+1) now means something
#define mod_page1 (0x8000)

// Whew, global:
#define mod_board 4				// if contents of board has changed

typedef struct {
	int sh;
	int flags;
	char *na;
	} infotype;

#define f_playerthru 1			// (blocks)
#define f_notstair 2				// (blocks)
#define f_notvine 4				// (blocks)
#define f_msgtouch 8				// Touch routine? (blocks & objs)
#define f_msgdraw 16				// Draw specially? (blocks)
#define f_msgupdate 32			// If true, redraw every cycle (blocks)
#define f_inside 64				// Text inside (objs)
#define f_front 128				// Puts object in foreground
#define f_trigger 256			// Pad triggers it (objs)
#define f_back 512				// Background objects e.g. torches, stalags
//#define f_tinythru 512			// Tinyhero thru
#define f_always 1024			// Always updates object
#define f_killable 2048			// Laser or rock kills it (objs)
#define f_fireball 4096			// Fireball kills it (objs)
#define f_water 8192				// Is water (blox)
#define f_notwater 16384		// Not water (blox)
#define f_weapon 16384			// Is a weapon (objs)

#define numinfotypes 900
int msg_block (int x, int y, int msg);

#define msg_draw 0
#define msg_touch 1
#define msg_update 2
#define msg_trigger 3
#define msg_trigoff 4
#define msg_trigon 5

typedef struct {
	int level;
	int health;
	int emeralds;
	int numinv;
	int inv[30];					// Inventory
	int ouched;
	char pad[22];
	unsigned long score;
	} pltype;

// Inventory
#define inv_hero   0				// Hero, as in a life? NADA
#define inv_mapkey 1				// Opens doors on MAP
#define inv_epic   2				// For EPIC bonus
#define inv_fire   3				// Fireballs
#define inv_laser  4				// Extra laser bullets
#define inv_bird   5				// Hero turns into bumblebee
#define inv_fish   6				// Underwater bubble
#define inv_rock   7				// Rock to throw
#define inv_jump   8          // High jump
#define inv_invin  9				// Invincibility
#define inv_icon1  10
#define inv_icon2  11
#define inv_icon3  12
#define inv_key0   13			// Keys open doors
#define inv_key1   14
#define inv_key2   15
#define inv_key3   16

#define numinvkinds 17
extern const int inv_shape[numinvkinds];
extern const int inv_xfm[numinvkinds];
extern const char *inv_getmsg[numinvkinds];

// GAME OBJECTS
typedef struct {
	char objkind;
	int x, y;
	int xd, yd;
	int xl,yl;
	int state, substate, statecount, counter;
	int objflags;
	char *inside;
	int info1;						// player=lastxd
	int zaphold;
	} objtype;

#define maxobjs 256
#define maxscrnobjs 192

// STATES
#define numstates 8
extern int stateinfo [numstates];
#define sti_canfire 1
#define sti_invincible 2

#define st_stand 0
//	Player standing or running.
//	xd =0 Standing
//	xd!=0 Running Substates: 0-7 for character + statecount
#define st_still 1
#define st_jumping 2
#define st_climbing 3
#define st_begin 4
#define st_die 5
#define st_transport 6
#define st_platform 7

#define die_ash 0
#define die_bird 1
#define die_fish 2

#define hilen 10
#define savelen 10
#define numhighs 10
#define numsaves 7

extern infotype info[numinfotypes];
extern int (*kindmsg[numobjkinds])(int n, int msg, int z);
extern int kindxl[numobjkinds];
extern int kindyl[numobjkinds];
extern char *kindname[numobjkinds];
extern int kindflags[numobjkinds];
extern int kindtable[numobjkinds];
extern int kindscore[numobjkinds];

extern objtype objs [maxobjs+2];
extern int numobjs, numscrnobjs;
extern int scrnobjs [maxscrnobjs];
extern int designflag;
extern int gameover, gamecount, statmodflg;
extern int peeky;
extern char cnt_fruit, old_fruit, icon_fruit; // extra health for 16 fruits

extern pltype pl,o_pl;
extern char *leveltxt[16];

void init_info (void);
void init_objs (void);
void init_inv (void);
void init_objinfo (void);
void init_brd (void);

void refresh (int pagemode);
void touchbkgnd (int n);
void drawcell (int x, int y);
void drawboard (void);
void setorigin (void);
void p_reenter (int died);		// Put player on current checkpt & set state
void dolevelsong (void);

void dotextmsg (int n, int flg);
void txt (char *msg, int col, int flg);
void wait (void);
void dialogmsg (int n, int flg);			// for dialog with hero and eagle

void sendtrig (int cnt, int msg, int fromobj);
int standfloor (int n, int dx, int dy);
int countobj (int objkind, int flg);
int takeinv (int invthing);
void addinv (int invthing);
int invcount (int invthing);
void stats (int num);

void upd_bkgnd (void);
void upd_objs (int doflag);
void upd_colors (void);

int addobj (int kind, int x, int y, int xd, int yd);
void killobj (int n);
void purgeobjs (void);
void zapobjs (void);
void setobjsize (int n);
void addscore (int sc, int x, int y);
void seekplayer (int n, int *dx, int *dy);

int findcheckpt (int n);
int trymove (int n, int newx, int newy);
int justmove (int n, int newx, int newy);
int cando (int n, int newx, int newy, int flag);
int onscreen (int n);
int objdo (int n, int newx, int newy, int flag);
int fishdo (int n, int xd, int yd);
void pointvect (int n1, int n2, int *ax, int *ay, int len);
int vectdist (int n1,int n2);
void moveobj (int n, int newx, int newy);
int trymovey (int n, int newx, int newy);
int crawl (int n, int dx, int dy);

int playerxfm (int n);
int trybreakwall (int n, int x, int y);
void explode1 (int x, int y, int n, int flg);
void explode2 (int n);
void notemod (int n);
void playerkill (int z);
void hitplayer (int n, int flg);
void p_ouch (int healthtake,int diemode);

// Volume-specific stuff
void initvolume(void);

#define s1l				9
#define s1r				10
#define s2				21
#define s2u				25
#define s3				64
#define c2				65
#define c8				117
#define stlbeam		138
#define elevl			139
#define elevr			140
#define platl        146
#define platr        147
#define breakwall		148
#define arrow1			149
#define arrow2			150
#define motion1		151
#define crum1        160
#define crum_last    171
#define blink        172
#define stldr1 		217
#define stldr3			219
#define bridge       220
#define comp1			260
#define comp3			262
#define eyeop			263
#define eyecl			264
#define noeye        265
#define lava1			290
#define lava8			297
#define lavab			298
#define waterfl1     364
#define waterfl4     367
#define waterflt     371
#define waterbubl    375
#define watert1      376
#define watert8      383
#define seaweedr     384
#define seaweedl     385
#define acid1			528
#define acid8			535
#define acidb			536
#define thrnt        549
#define thrnb        550
#define thornt       563
#define thornb       564
#define waterend1    574
#define waterend8    581
#define watermst1    582
#define watermst8    589
#define waterwav1    590
#define waterwav8    597
#define spike1       598
#define spike6       603
#define flow1        616
#define flow3        618
#define lever1       649
#define lever7       655
#define elec1        676
#define elec9        684
#define mpgate			720
#define s6l          734
#define s6r          735
#define qzap1        852
#define qzap3        854