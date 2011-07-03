/********************************************* 
 *   WORX Sound Blaster Developers Toolkit   * 
 *   Version 2.01(L) 05/27/93                * 
 *   (c) 1993 by Mystic Software             * 
 *   All rights reserved.                    * 
 *********************************************/

#include "../port.h"
#ifdef __cplusplus
extern "C" {
#endif

void StartWorx(void);
void CloseWorx(void);
int ForceConfig(int Port);
int AdlibDetect(void);

void DSPClose(void);
int DSPReset(void);
int DSPPortSetting(void);
int ResetMPU401(void);

int SetMasterVolume(unsigned char left,unsigned char right);
int SetVOCVolume(unsigned char left,unsigned char right);
int SetFMVolume(unsigned char left,unsigned char right);

int StartResource(char *f_name);
long OpenElement(char *f_name);
char *ElementGets(char *data,unsigned char maxlen);
unsigned ElementRead(void far *buffer,unsigned int length);

void SetLoopMode(int m);
void GoNote(unsigned char midi_channel,unsigned char note,unsigned char velo);
void StopNote(unsigned char midi_channel,unsigned char note);

char far *GetSequence(char *f_name);
char far *LoadOneShot(char *f_name);
int LoadSBIFile(char *f_name,char pnum);
void ProgramChange(int midi_channel,int program);
int LoadIBKFile(char *f_name);

int PlayPWMBlock(char far *pwm);
int PlayVOCBlock(char far *voc,int volume);
void PlayCMFBlock(char far *seq);
void PlayMIDBlock(char far *seq);
int PlayVOCFile(char *f_name,int volume);
int PlayWAVBlock(char far *wav);
int PlayWAVFile(char *f_name);

void ResetRealTime(void);
int SetRealTime(int count);
int TimerDone(void);

void ContinueSequence(void);
void StopSequence(void);
void StopVOC(void);
void StopPWM(void);
unsigned GetLastVOCMarker(void);
void SetVOCIndex(unsigned x);

int SequencePlaying(void);
int VOCPlaying(void);
int PWMPlaying(void);
int GetMIDIBeat(void);

void SetAudioMode(unsigned char Mode);
void SetMIDISpeaker(unsigned char Channel);

int WorxPresent(void);
void CloseWorxDriver(void);

int JoyStickY(void);
int JoyStickX(void);
int JoyStickButton(int num);
void JoyStickUpdate(void);
void EnableExtenderMode(void);

/********************************************* 
 *   WORX+ Polyphonic Sample Player          * 
 *   Version 1.0(C)  05/05/93                * 
 *   (c) 1993 by Mystic Software             * 
 *   All rights reserved.                    * 
 *********************************************/

int StartPoly(void);
void ClosePoly(void);
int PlaySMPBlock(char *sample,int cell,int note);
int PolyCellStatus(int cell);

#ifdef __cplusplus
}
#endif

