//	Keyboard.H

extern char k_rshift, k_lshift, k_shift, k_ctrl, k_alt, k_numlock;
extern unsigned char keydown[2][256];

void k_status (void);
int k_pressed (void);
int k_read (void);

void installhandler (unsigned char status);
void removehandler(void);
void enablebios(void);
void disablebios(void);
int biosstatus (void);

//	Characters
#define enter 13
#define esc 27
#define escape 27
#define k_tab 9
#define k_bs 8
#define k_up 200
#define k_down 208
#define k_left 203
#define k_right 205
#define k_pgup 201
#define k_pgdown 209
#define k_f1 187
#define k_f2 188
#define k_f3 189
#define k_f4 190
#define k_f5 191
#define k_f6 192
#define k_f7 193
#define k_f8 194
#define k_f9 195
#define k_f10 196

#define scan_esc 0x01
#define scan_ctrl 0x1D
#define scan_lshift 0x2A
#define scan_rshift 0x36
#define scan_alt 0x38
#define scan_space 0x39
#define scan_f1 0x3B
#define scan_cursorup 0x48
#define scan_cursorleft 0x4B
#define scan_cursorright 0x4D
#define scan_cursordown 0x50

/*
scan_1=$2;
scan_2=$3;
scan_3=$4;
scan_4=$5;
scan_5=$6;
scan_6=$7;
scan_7=$8;
scan_8=$9;
scan_9=$A;
scan_0=$B;
scan_MINUS=$C;
scan_PLUS=$D;
scan_BACKSPACE=$E;
scan_TAB=$F;
scan_Q=$10;
scan_W=$11;
scan_E=$12;
scan_R=$13;
scan_T=$14;
scan_Y=$15;
scan_U=$16;
scan_I=$17;
scan_O=$18;
scan_P=$19;
scan_LBRACKET=$1A;
scan_RBRACKET=$1B;
scan_ENTER=$1C;
scan_CTRL=$1D;
scan_A=$1E;
scan_S=$1F;
scan_D=$20;
scan_F=$21;
scan_G=$22;
scan_H=$23;
scan_J=$24;
scan_K=$25;
scan_L=$26;
scan_SEMICOLON=$27;
scan_QUOTE=$28;
scan_TILDE=$29;
scan_LSHIFT=$2A;
scan_BACKSLASH=$2B;
scan_Z=$2C;
scan_X=$2D;
scan_C=$2E;
scan_V=$2F;
scan_B=$30;
scan_N=$31;
scan_M=$32;
scan_COMMA=$33;
scan_PERIOD=$34;
scan_SLASH=$35;
scan_RSHIFT=$36;
scan_ASTERISK=$37;
scan_ALT=$38;
scan_SPACE=$39;
scan_CAPSLOCK=$3A;
scan_F1=$3B;
scan_F2=$3C;
scan_F3=$3D;
scan_F4=$3E;
scan_F5=$3F;
scan_F6=$40;
scan_F7=$41;
scan_F8=$42;
scan_F9=$43;
scan_F10=$44;
scan_NUMLOCK=$45;
scan_SCROLLOCK=$46;
scan_HOME=$47;
scan_CURSORUP=$48;
scan_PGUP=$49;
scan_GREYMINUS=$4A;
scan_CURSORLEFT=$4B;
scan_CURSORCENTER=$4C;
scan_CURSORRIGHT=$4D;
scan_GREYPLUS=$4E;
scan_END=$4F;
scan_CURSORDOWN=$50;
scan_PGDN=$51;
scan_INS=$52;
scan_DEL=$53;
scan_SYSRQ=$54;
scan_UNKNOWN1=$55;
scan_UNKNOWN2=$56;
scan_F11=$57;
scan_F12=$58;
*/