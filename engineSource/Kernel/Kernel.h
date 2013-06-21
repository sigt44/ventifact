#ifndef KERNAL_H
#define KERNAL_H

#include "SDL/SDL.h"

#include "../File.h"
#include "../Flag.h"
#include "../BaseState.h"
#include "../Time/Timer.h"

#define K_VERS_1 0
#define K_VERS_2 7
#define K_VERS_3 0

enum PATH_TYPES
{
    PTH_GRAPHIC = 0,
    PTH_FONT = 1,
    PTH_LOG = 2,
    PTH_FILELOG = 3,
    PTH_SOUNDS = 4,
    PTH_ARRAYEND = 5
};

/*Used for tracking allocated memory structures*/
enum M_FREE
{
    M_FREE = 0, /*Not to track a structure (must be deleted manually)*/
    A_FREE = 1 /*Object manager should remove the structure on cleanup*/
};

struct pe_Kernel
{
    int screen_Width;
    int screen_Height;
    int screen_BPP;
    unsigned int screen_ForceBPP; /*If true set BPP to whatever is valued above else let SDL choose the BPP from the users desktop*/

    Uint32 SDL_Init_Flags;
    Uint32 SDL_Video_Flags;
    Uint32 SDL_Surface_Type;

    Uint32 control_Flags;

    Uint32 colourKey;

    unsigned int sound_Volume;
    int sound_Frequency;
    Uint16 sound_Format;
    int sound_Channels; /*Stereo (2), mono (1)*/
    int sound_Chunksize;

    unsigned int debug;

    FILE *log;

    char paths[PTH_ARRAYEND][128];

    SDL_Surface *screen;

    Timer Time;

	Base_State g_BaseState; /*Global state to be run before any other base state is run in the main loop*/
    struct list *l_BaseState; /*List that holds the base states of the actual program*/
};

int kernel_Init(const char *name);

int kernel_Init_SDL(void);

int kernel_Quit(void);

void kernel_Set_Defaults(void);

void kernel_SetPath(FILE *fileAtLine, unsigned int index);

void kernel_SetPaths(void);

int kernel_SetScreen(int width, int height, int bitsPerPixel, Uint32 flags);

int kernel_Init_Audio(void);
int kernel_Close_Audio(void);

void kernel_PrintSDLVersions(void);
void ker_Report_Video(void);
void ker_ReportAudio(void);

SDL_Surface *ker_Screen(void);

int ker_Screen_Width(void);

int ker_Screen_Height(void);

int ker_Screen_BPP(void);

void kernel_SetupColours(void);

Uint32 *ker_colourKey(void);

Uint32 ker_Video_Flags(void);

Uint32 ker_Surface_Type(void);

FILE *ker_Log(void);

char *kernel_GetPath(unsigned int index);

Base_State *ker_BaseState(void);

struct list **ker_BaseStateList(void);

#endif