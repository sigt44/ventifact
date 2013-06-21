#include <string.h>
#include <time.h>
#include "SDL/SDL_ttf.h"
#include "Kernel.h"

#include "Kernel_State.h"
#include "../Controls.h"
#include "../Font.h"
#include "../Graphics/Surface.h"
#include "../Sound.h"
#include "../Maths.h"

static struct pe_Kernel kernel_Main;

void kernel_Set_Defaults(void)
{
    int x = 0;

    kernel_Main.screen_Width = 640;
    kernel_Main.screen_Height = 480;
    kernel_Main.screen_BPP = 16;
    kernel_Main.screen_ForceBPP = 0;

    kernel_Main.SDL_Init_Flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO;
    kernel_Main.SDL_Surface_Type = SDL_SWSURFACE;
    kernel_Main.SDL_Video_Flags = 0;

    kernel_Main.sound_Volume = 0;
    kernel_Main.sound_Frequency = MIX_DEFAULT_FREQUENCY;
    kernel_Main.sound_Format = MIX_DEFAULT_FORMAT;
    kernel_Main.sound_Channels = 2; /*Stereo (2), mono (1)*/
    kernel_Main.sound_Chunksize = 1024;

    #ifdef GP2X
    kernel_Main.SDL_Init_Flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO;
    kernel_Main.control_Flags = C_JOYSTICK;
    kernel_Main.screen_BPP = 16;
    kernel_Main.SDL_Surface_Type = SDL_HWSURFACE;
    kernel_Main.SDL_Video_Flags = SDL_DOUBLEBUF;

    kernel_Main.screen_Width = 320;
    kernel_Main.screen_Height = 240;
    #endif

    #ifdef DINGOO
    kernel_Main.control_Flags = C_KEYBOARD;
    kernel_Main.SDL_Init_Flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

    kernel_Main.screen_Width = 320;
    kernel_Main.screen_Height = 240;

    kernel_Main.SDL_Surface_Type = SDL_SWSURFACE;
    kernel_Main.SDL_Video_Flags = 0;
    #endif

    #ifdef PANDORA
    kernel_Main.control_Flags = C_KEYBOARD | C_JOYSTICK | C_MOUSE;
    kernel_Main.SDL_Init_Flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

    kernel_Main.screen_Width = 800;
    kernel_Main.screen_Height = 480;

    kernel_Main.SDL_Surface_Type = SDL_HWSURFACE;
    kernel_Main.SDL_Video_Flags = SDL_FULLSCREEN | SDL_DOUBLEBUF;
    #endif

    #if defined (LINUX) || defined (WINDOWS)
    kernel_Main.screen_Width = 800;
    kernel_Main.screen_Height = 480;
    kernel_Main.control_Flags = C_KEYBOARD | C_JOYSTICK | C_MOUSE;
    kernel_Main.SDL_Surface_Type = SDL_HWSURFACE;
    kernel_Main.SDL_Video_Flags = SDL_DOUBLEBUF;
    #endif

    kernel_Main.debug = 0;

    kernel_Main.log = NULL;

    kernel_Main.screen = NULL;

    return;
}

void kernel_SetPath(FILE *fileAtLine, unsigned int index) /*Set the directory of a certain component*/
{
    char newPath[128];

    if(index > (PTH_ARRAYEND-1))
    {
        printf("Failed to add path using index %d\n",index);
        return;
    }

    fgets(newPath, 127, fileAtLine); /*Get the line that the FILE pointer is at*/

    if(strlen(newPath) >= 127)
    {
        printf("Warning path (%s) for index %d may be too large %d\n", newPath, index, strlen(newPath));
    }

    string_Line(newPath); /*Get rid of the enter symbol at the of the line to properly assign the path*/
    strncpy(kernel_Main.paths[index], newPath, 127);

    return;
}

void kernel_SetPaths(void)
{
    FILE *checkFile = NULL;

    /*First setup defaults*/
    strncpy(kernel_Main.paths[PTH_GRAPHIC], "../Textures/", 127);
    strncpy(kernel_Main.paths[PTH_FONT], "../Fonts/", 127);
    strncpy(kernel_Main.paths[PTH_LOG], "../Log/log.txt", 127);
    strncpy(kernel_Main.paths[PTH_FILELOG], "../Log/file.txt", 127);
    strncpy(kernel_Main.paths[PTH_SOUNDS], "../Sounds/", 127);

    /*Next check the path file*/
    if((checkFile = fopen("../Path.txt","r")) != NULL)
    {
        if(file_Check_Line("Graphics:", checkFile))
        {
            kernel_SetPath(checkFile, PTH_GRAPHIC);
        }
        if(file_Check_Line("Fonts:", checkFile))
        {
            kernel_SetPath(checkFile, PTH_FONT);
        }
        if(file_Check_Line("Logs:", checkFile))
        {
            kernel_SetPath(checkFile, PTH_LOG);
            kernel_SetPath(checkFile, PTH_FILELOG);
        }
        if(file_Check_Line("Sounds:", checkFile))
        {
            kernel_SetPath(checkFile, PTH_SOUNDS);
        }
    }
    else
    {
        printf("Warning: Could not find Path.txt in directory, using default paths\n");
    }

    return;
}

int kernel_Init(const char *name)
{
    printf("Engine Initializing V %d.%d.%d\n", K_VERS_1, K_VERS_2, K_VERS_3);

    if(mem_Init())
    {
		printf("\tFailed to load memory manager\n");
		return 1;
	}

    kernel_Set_Defaults();
    kernel_SetPaths();

    if(file_Init(kernel_Main.paths[PTH_FILELOG]))
	{
		printf("\tFailed to load file manager\n");
		return 1;
	}

    #ifndef NO_LOG
    kernel_Main.log = file_Open(kernel_Main.paths[PTH_LOG], "w");

    if(kernel_Main.log == NULL)
    {
        printf("\nKernel log failed to load.\n");
        return 1;
    }
    #endif

    #ifdef NO_LOG
    printf("File logging disabled.\n");
    #endif

    file_Log(ker_Log(), 1, "Engine: %d.%d.%d\n", K_VERS_1, K_VERS_2, K_VERS_3);

	mth_FillAngles();

	srand(time(NULL));

    if(kernel_Init_SDL())
        return 1;

    SDL_WM_SetCaption(name, name);

	ker_Report_Video();

	surf_Init(kernel_Main.paths[PTH_GRAPHIC]);

	if(control_Init(kernel_Main.control_Flags))
	{
		printf("\tFailed to load controls\n");
		return 1;
	}

	if(font_Init(kernel_Main.paths[PTH_FONT]))
	{
		return 1;
	}

	font_Load("Arial.ttf", 11, 1);
	font_Load("Arial.ttf", 13, 1);

	if(kernel_Setup_BaseState())
	{
	    printf("\tFailed to set up kernel gamestate\n");
        return 1;
	}

    puts("Complete");
	return 0;
}

/*
    Function: kernel_SetScreen

    Description -
    Sets up the screen surface for use. Returns 1 if there is a failure.

    4 arguments:
    int width - The width of the screen.
    int height - The height of the screen.
    int bitsPerPixel - The BPP of the screen.
    Uint32 - The SDL flags to be passed for the extra screen attributes.
*/
int kernel_SetScreen(int width, int height, int bitsPerPixel, Uint32 flags)
{
    /*Need this to check for the current BPP the user has in desktop mode, otherwise could be poor performance*/
    const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

    if(kernel_Main.screen_ForceBPP == 1)
        kernel_Main.screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
    else
        kernel_Main.screen = SDL_SetVideoMode(width, height, videoInfo->vfmt->BitsPerPixel, flags);
    if(kernel_Main.screen == NULL)
    {
        printf("\tScreen failed to load - %s.\n", SDL_GetError());
        return 1;
    }

    kernel_Main.screen_BPP = kernel_Main.screen->format->BitsPerPixel; /*Update the BPP variable incase it has changed*/
    kernel_Main.screen_Width = kernel_Main.screen->w;
    kernel_Main.screen_Height = kernel_Main.screen->h;


    kernel_Main.colourKey = SDL_MapRGB(ker_Screen()->format, 255, 0, 255);

    return 0;
}

int kernel_Init_SDL(void)
{
    if(SDL_Init(kernel_Main.SDL_Init_Flags) == -1)
    {
        printf("\tSDL failed to load - %s.\n", SDL_GetError());
        return 1;
    }

    #if defined (LINUX) || defined (WINDOWS)
    SDL_ShowCursor(SDL_DISABLE);
    #else
    SDL_ShowCursor(SDL_DISABLE);
    #endif

    if(kernel_SetScreen(kernel_Main.screen_Width, kernel_Main.screen_Height, kernel_Main.screen_BPP, kernel_Main.SDL_Surface_Type | kernel_Main.SDL_Video_Flags) == 1)
        return 1;

    if(TTF_Init() == -1)
    {
        printf("\tTTF failed to load - %s.\n", SDL_GetError());
        return 1;
    }

    if(kernel_Init_Audio() == -1)
    {
        printf("\tAudio failed to load - %s\n", Mix_GetError());
        return 1;
    }

    kernel_SetupColours();

    kernel_PrintSDLVersions();

    return 0;
}

int kernel_Init_Audio(void)
{
    if(flag_Check(&kernel_Main.SDL_Init_Flags, SDL_INIT_AUDIO) == 0)
    {
        file_Log(ker_Log(), 1, "No sound device loaded.\n");
        /*Initialise sound manager with it not accepting any sounds*/
        sound_Init(0, 0, 0, 0);

        return 0;
    }

    printf("Opening audio device\n\n");
    if(Mix_OpenAudio(kernel_Main.sound_Frequency, kernel_Main.sound_Format, kernel_Main.sound_Channels, kernel_Main.sound_Chunksize) == -1)
    {
        return -1;
    }

    printf("Initalising sound manager\n");
    sound_Init(16, 32, 50, 1);

    ker_ReportAudio();

    return 0;
}

int kernel_Close_Audio(void)
{
    sound_Clean();

    if(flag_Check(&kernel_Main.SDL_Init_Flags, SDL_INIT_AUDIO) == 1)
    {
        Mix_CloseAudio();
    }

    return 0;
}

void kernel_PrintSDLVersions(void)
{
    const SDL_version *v = NULL;
    SDL_version compiled_V;

    v = SDL_Linked_Version();
    SDL_VERSION(&compiled_V);
    file_Log(ker_Log(), 1, "(Linked) SDL Version: %u.%u.%u\n", v->major, v->minor, v->patch);
    file_Log(ker_Log(), 1, "(Compiled) SDL Version: %u.%u.%u\n", compiled_V.major, compiled_V.minor, compiled_V.patch);

    v = TTF_Linked_Version();
    SDL_TTF_VERSION(&compiled_V);
    file_Log(ker_Log(), 1, "(Linked) SDL TTF Version: %u.%u.%u\n", v->major, v->minor, v->patch);
    file_Log(ker_Log(), 1, "(Compiled) SDL TTF Version: %u.%u.%u\n", compiled_V.major, compiled_V.minor, compiled_V.patch);

    file_Log(ker_Log(), 1, "SDL_Init flags (%d) -\n", kernel_Main.SDL_Init_Flags);

    if(flag_Check(&kernel_Main.SDL_Init_Flags, SDL_INIT_VIDEO) == 1)
        file_Log(ker_Log(), 1, "\tSDL_INIT_VIDEO\n");
    if(flag_Check(&kernel_Main.SDL_Init_Flags, SDL_INIT_AUDIO) == 1)
        file_Log(ker_Log(), 1, "\tSDL_INIT_AUDIO\n");
    if(flag_Check(&kernel_Main.SDL_Init_Flags, SDL_INIT_JOYSTICK) == 1)
        file_Log(ker_Log(), 1, "\tSDL_INIT_JOYSTICK\n");

    return;
}

void kernel_SetupColours(void)
{
    SDL_PixelFormat *pFormat = ker_Screen()->format;
    colourWhite = (Uint16P)SDL_MapRGB(pFormat, 255, 255, 255);
    colourBlack = (Uint16P)SDL_MapRGB(pFormat, 0, 0, 0);
    colourLightBlue = (Uint16P)SDL_MapRGB(pFormat, 85, 155, 255);
    colourLightGreen = (Uint16P)SDL_MapRGB(pFormat, 60, 255, 60);
    colourYellow = (Uint16P)SDL_MapRGB(pFormat, 255, 255, 0);
    colourRed = (Uint16P)SDL_MapRGB(pFormat, 255, 0, 0);
    colourGreen = (Uint16P)SDL_MapRGB(pFormat, 0, 255, 0);
    colourGrey = (Uint16P)SDL_MapRGB(pFormat, 180, 180, 180);
    //colourPinkT = (Uint16P)SDL_MapRGB(pFormat, 255, 0, 255);

    return;
}

int kernel_Quit(void)
{
    puts("Quitting");

    puts("Surfaces");
    surf_Quit();

    puts("Controls");
    control_Quit();

    puts("Fonts");
    font_Quit();

    puts("SDL");
    /*SDL stuff here*/
    TTF_Quit();

    kernel_Close_Audio();

    SDL_Quit();
    /*End SDL stuff*/

    puts("File");
    file_Quit();

    puts("Memory");
    mem_Quit(); /* Keep this last*/

    puts("Complete");
    return 0;
}


void ker_Report_Video(void)
{
    const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();
    SDL_PixelFormat *px = videoInfo->vfmt;

    file_Log(ker_Log(), 1, "Video report - \n\n");

    if(videoInfo->hw_available)
    {
        file_Log(ker_Log(), 1, "\tHardware surfaces are available.\n");
        if(flag_Check(&kernel_Main.SDL_Video_Flags, SDL_HWSURFACE))
           file_Log(ker_Log(), 1, "\t\tTotal video memory: %d\n",videoInfo->video_mem);
    }

    if(!flag_Check(&ker_Screen()->flags, SDL_HWSURFACE)) /* The SDL_SWSURFACE flag is actually 0*/
    {
        file_Log(ker_Log(), 1, "\tCurrently using software surfaces.\n");
    }
    else
    {
        file_Log(ker_Log(), 1, "\tCurrently using hardware surfaces.\n");
    }

    file_Log(ker_Log(), 1, "\tPixel format: %d (%d)\n", px->BitsPerPixel, BYTES_PER_PIXEL * 8);

    if(kernel_Main.screen_BPP != px->BitsPerPixel)
    {
        file_Log(ker_Log(), 1, "\tWarning pixel format (%d) does not match specified format (%d), expect low performance!\n", px->BitsPerPixel, kernel_Main.screen_BPP);
    }
    /*file_Log(ker_Log(),1,"\tCurrent width: %d\n",videoInfo->current_w);
    file_Log(ker_Log(),1,"\tCurrent Height: %d\n",videoInfo->current_h);*/

    return;
}

void ker_ReportAudio(void)
{
    char *formatText = "None";
    file_Log(ker_Log(), 1, "Audio report - \n\n");

    if(flag_Check(&kernel_Main.SDL_Init_Flags, SDL_INIT_AUDIO) == 0)
    {
        file_Log(ker_Log(), 1, "No audio device initialised.\n");
    }

    Mix_QuerySpec(&kernel_Main.sound_Frequency, &kernel_Main.sound_Format, &kernel_Main.sound_Channels);

    switch(kernel_Main.sound_Format)
    {
        case AUDIO_U8:
        formatText = "AUDIO_U8, Unsigned 8-bit samples";
        break;

        case AUDIO_S8:
        formatText = "AUDIO_S8, Signed 8-bit samples";
        break;

        case AUDIO_U16LSB:
        formatText = "AUDIO_U16LSB, Unsigned 16-bit samples, in little-endian byte order";
        break;

        case AUDIO_S16LSB:
        formatText = "AUDIO_S16LSB, Signed 16-bit samples, in little-endian byte order";
        break;

        case AUDIO_U16MSB:
        formatText = "AUDIO_U16MSB, Unsigned 16-bit samples, in big-endian byte order";
        break;

        case AUDIO_S16MSB:
        formatText = "AUDIO_S16MSB, Signed 16-bit samples, in big-endian byte order";
        break;
    }

    file_Log(ker_Log(), 1, "Frequency: %dHz\nFormat: %s\nChannels: %d\nMax mixing channels %d\n", kernel_Main.sound_Frequency, formatText, kernel_Main.sound_Channels, Mix_AllocateChannels(-1));

    return;
}

SDL_Surface *ker_Screen(void)
{
    return kernel_Main.screen;
}

int ker_Screen_Width(void)
{
    return kernel_Main.screen_Width;
}

int ker_Screen_Height(void)
{
    return kernel_Main.screen_Height;
}

int ker_Screen_BPP(void)
{
    return kernel_Main.screen_BPP;
}

Uint32 ker_Video_Flags(void)
{
    return kernel_Main.SDL_Video_Flags;
}

Uint32 ker_Surface_Type(void)
{
    return kernel_Main.SDL_Surface_Type;
}

Uint32 *ker_colourKey(void)
{
    return &kernel_Main.colourKey;
}

FILE *ker_Log(void)
{
    return kernel_Main.log;
}

char *kernel_GetPath(unsigned int index)
{
    return kernel_Main.paths[index];
}

Base_State *ker_BaseState(void)
{
    return &kernel_Main.g_BaseState;
}

struct list **ker_BaseStateList(void)
{
    return &kernel_Main.l_BaseState;
}
