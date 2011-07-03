//	Gamectrl Header

extern int dx1, dy1, fire1, fire2, fire1off, fire2off;
extern int joyxl, joyxc, joyxr, joyyu, joyyc, joyyd;
extern int joyflag;
extern int key;
extern int dx1hold, dy1hold, flow1;
extern int joyxsense, joyysense;
extern long systime;

void readspeed (void);
int joypresent (void);
void gc_init (void);
int  gc_config (void);
void gc_exit (void);
void checkctrl (int pollflag);
void checkctrl0 (int pollflag);
int  getctrlmode (void);
void getkey (void);

void recordmac (char *fname);
void playmac (char *fname);
void stopmac (void);
void macrecend (void);

extern int macrecord;
extern int macplay;
extern int macabort;
extern int macaborted;