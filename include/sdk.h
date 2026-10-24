#ifndef SDK_H
#define SDK_H

/* PsyQ 4.0's library functions the game calls (config/symbol_addrs.txt has their addresses), and their types */

/* libgpu: an area of VRAM */
typedef struct {
    short x, y;
    short w, h;
} RECT;

/* libgpu: the display environment PutDispEnv takes (0x14 bytes) */
typedef struct {
    RECT disp;
    RECT screen;
    unsigned char isinter, isrgb24;
    unsigned char pad0, pad1;
} DISPENV;

/* libgpu: the drawing environment PutDrawEnv takes (0x5C bytes), the GPU packet it builds from the rest last (PsyQ's
   DR_ENV, 16 words) */
typedef struct {
    RECT clip;
    short ofs[2];
    RECT tw;
    unsigned short tpage;
    unsigned char dtd, dfe;
    unsigned char isbg;
    unsigned char r0, g0, b0;
    unsigned int dr_env[16];
} DRAWENV;

/* libapi */
long StopRCnt(unsigned long spec);
long CloseEvent(long event);
long DisableEvent(long event);
long TestEvent(unsigned long event);
void EnterCriticalSection(void);
void ExitCriticalSection(void);
void StopPAD(void);
void StopCARD(void);
long close(long fd);
long _card_info(long channel);
long _card_write(long channel, long block, unsigned char *buffer);
void _new_card(void);

/* libc */
int sprintf(char *buffer, const char *format, ...);
void bzero(unsigned char *buffer, int size);
char *strcpy(char *destination, const char *source);
void *memset(void *buffer, int value, int size);

/* libcd: CdControl's commands, and the mode bit CdRead sets for double speed (psx-spx, "CDROM Controller Command
   Summary" and Setmode) */
#define CdlSetloc 0x02
#define CdlStop 0x08
#define CdlSeekL 0x15
#define CdlModeSpeed 0x80
int CdControl(unsigned char com, unsigned char *param, unsigned char *result);
int CdDataSync(int mode);
int CdReadSync(int mode, unsigned char *result);

/* libetc */
int StopCallback(void);

/* libgte */
long ratan2(long y, long x);

/* libgpu */
int ResetGraph(int mode);
int MoveImage(RECT *rect, int x, int y);
void AddPrim(void *ot, void *primitive);

/* libsnd */
void SsEnd(void);
void SsSepPlay(short access_num, short seq_num, char play_mode, short l_count);
void SsSepPause(short access_num, short seq_num);
void SsSepReplay(short access_num, short seq_num);

#endif /* SDK_H */
