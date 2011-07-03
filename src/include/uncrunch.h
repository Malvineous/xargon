//	Uncrunch header

#define scrnaddr ((char far *)0xB8000000)
extern void uncrunch (char far *sourceptr,char far *destptr,int length);
