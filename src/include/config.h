extern struct {
	int firstthru;									// Are we set up?
	int joyflag0,joyxl0, joyxc0, joyxr0, joyyu0, joyyc0, joyyd0;
	int x_ourmode0;
	int sb_exists0;
	int vocflag0;
	} cf;

void cfg_init(int argc, char *argv[]);
int doconfig (void);
extern int nosnd;
// extern int cfgdemo;