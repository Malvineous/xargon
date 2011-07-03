typedef struct {
	int firstthru;									// Are we set up?
	int joyflag0,joyxl0, joyxc0, joyxr0, joyyu0, joyyc0, joyyd0;
	int x_ourmode0;
	union {
		int sb_exists0;
		int musicflag0;
	};
	int vocflag0;
	char pad [64];
	} config;
extern config cf;

void cfg_init(int argc, char *argv[]);
int doconfig (void);
extern int nosnd;
// extern int cfgdemo;
