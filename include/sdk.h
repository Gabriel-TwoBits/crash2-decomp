#ifndef SDK_H
#define SDK_H

/* PsyQ 4.0's library functions the game calls (config/symbol_addrs.txt has their addresses), and their types */

/* libgpu: an area of VRAM */
typedef struct {
    short x, y;
    short w, h;
} RECT;

/* libapi */
long StopRCnt(unsigned long spec);
long CloseEvent(long event);
long DisableEvent(long event);
void EnterCriticalSection(void);
void ExitCriticalSection(void);
void StopPAD(void);

/* libc */
int sprintf(char *buffer, const char *format, ...);

/* libcd */
int CdControl(unsigned char com, unsigned char *param, unsigned char *result);

/* libetc */
int StopCallback(void);

/* libgte */
long ratan2(long y, long x);

/* libgpu */
int ResetGraph(int mode);

#endif /* SDK_H */
