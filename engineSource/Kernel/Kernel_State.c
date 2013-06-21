#include "Kernel_State.h"

#include "Kernel.h"
#include "../BaseState.h"
#include "../Controls.h"
#include "../Font.h"
#include "../Time/Timer.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Pixel.h"
#include "../Graphics/SurfaceManager.h"
#include "../Graphics/SpriteManager.h"
#include "../Graphics/PixelManager.h"
#include "../Graphics/DrawManager.h"
#include "../Screenshot.h"
#include "../Time/Fps.h"
#include "../Sound.h"

#define KS_DEBUG_VAR 5

struct kernel_State
{
    Control_Event c_Quit;
    Control_Event c_Screenshot;
    Control_Event c_Mute;
    Control_Event c_IncreaseSound;
    Control_Event c_DecreaseSound;

    unsigned int updateScreen;
    unsigned int fps;

    Timer globalTimer;
    Timer fpsTimer;

    SDL_Surface *clear_Screen;
    int cleanScreen;

    Draw_Manager drawManager;

    int staticDebugValue[KS_DEBUG_VAR];
    int debugValue[KS_DEBUG_VAR];
};

int kernel_State_Loop(void)
{
    int exit = 0;

    /*Main engine loop*/
    while(exit == 0) /*While the global state has not quit and there is a state to process.*/
    {
        if(baseState_Process(ker_BaseState()) == STATE_COMPLETE) /*Process the global state*/
        {
            exit = 1;
            break;
        }

        if(*ker_BaseStateList() == NULL) /*No more normal states*/
            baseState_CallQuit(ker_BaseState(), 1); /*So quit the global state*/
        else
            baseState_Process_List(ker_BaseStateList()); /*Process the normal state*/

    }

    return exit;
}

int kernel_Setup_BaseState(void)
{
    *ker_BaseStateList() = NULL;

    baseState_Setup(ker_BaseState(), "Global state", gKer_Init, gKer_Controls, gKer_Logic, gKer_Render, gKer_Exit, ker_BaseState());
    baseState_Activate(ker_BaseState());

    return 0;
}

void gKer_SetupControls(struct kernel_State *ks)
{
    control_Setup(&ks->c_Quit, "Global Quit", NULL, 0);
    control_Setup(&ks->c_Screenshot, "Screenshot", NULL, 600);
    control_Setup(&ks->c_Mute, "Mute", NULL, 500);
    control_Setup(&ks->c_IncreaseSound, "Increase Sound", NULL, 200);
    control_Setup(&ks->c_DecreaseSound, "Decrease Sound", NULL, 200);

    control_AddKey(&ks->c_Quit, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_F10);

    #ifdef GP2X
    control_AddKey(&ks->c_Quit, C_JOYSTICK, C_BUTTON_STATES, 2, GP2X_BUTTON_L, GP2X_BUTTON_START);
    #endif

    control_AddKey(&ks->c_Screenshot, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_F12);
    control_AddKey(&ks->c_Mute, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_F9);
    control_AddKey(&ks->c_IncreaseSound, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_F8);
    control_AddKey(&ks->c_DecreaseSound, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_F7);

    return;
}


void gKer_Init(void *info)
{
	int x = 0;
    Base_State *gs = info;
    struct kernel_State *ks = (struct kernel_State *)mem_Malloc(sizeof(struct kernel_State),__LINE__,__FILE__);
    gs->info = ks;

    /*Global controls*/
    gKer_SetupControls(ks);

    /*Setup the screen clean up*/
    ks->clear_Screen = surf_Copy(ker_Screen());
    fill_Rect(0, 0, ker_Screen_Width(), ker_Screen_Height(), &colourWhite, ks->clear_Screen);
    ks->cleanScreen = 0;

    puts("Setting up draw manager");
    drawManager_Setup(&ks->drawManager, NULL, 1000);

    surfaceMan_Setup(&ks->drawManager);
    spriteMan_Setup(&ks->drawManager);
    pixelMan_Setup(&ks->drawManager);

    ks->updateScreen = 1;
    printf("Screen BPP %d\n",ker_Screen_BPP());

    ks->globalTimer = timer_Setup(NULL, 0, 0, 1);

    ks->fps = 0;
    ks->fpsTimer = timer_Setup(NULL, 0, 0, 1);

    /*Setup some variables to help with debugging*/
    for(x = 0; x < KS_DEBUG_VAR; x++)
    {
        ks->staticDebugValue[x] = 0;
        ks->debugValue[x] = 0;
    }

    return;
}

void gKer_Controls(void *info)
{
    Base_State *gs = info;
    struct kernel_State *ks = gs->info;
    int x = 0;

    control_Update();

    if(control_GetEventType(SDL_QUIT) != NULL || control_IsActivated(&ks->c_Quit) || *ker_BaseStateList() == NULL) /*If user has clicked on cross to close the program or pressed required key to quit all program*/
        baseState_CallQuit(gs, 1);

    if(control_IsActivated(&ks->c_Screenshot) == 1)
    {
        screenshot_Take(ker_Screen(), "Screenshot.bmp");
        text_Draw_Blended_Arg(0, 0, ker_Screen(), font_Get(1, 13), &tColourWhite, -1, 1000, "Screenshot taken.");
    }

    if(control_IsActivated(&ks->c_Mute) == 1)
    {
        if(sound_ToggleMute() == 1)
        {
            text_Draw_Blended_Arg(0, 0, ker_Screen(), font_Get(1, 13), &tColourWhite, -1, 1000, "Muted.");
        }
        else
        {
            text_Draw_Blended_Arg(0, 0, ker_Screen(), font_Get(1, 13), &tColourWhite, -1, 1000, "Unmuted.");
        }
    }

    if(control_IsActivated(&ks->c_IncreaseSound) == 1)
    {
        x = sound_ChangeVolume(4);

        text_Draw_Blended_Arg(0, 0, ker_Screen(), font_Get(1, 13), &tColourWhite, -1, 200, "Volume \%%%d", x);
    }
    else if(control_IsActivated(&ks->c_DecreaseSound) == 1)
    {
        x = sound_ChangeVolume(-4);

        text_Draw_Blended_Arg(0, 0, ker_Screen(), font_Get(1, 13), &tColourWhite, -1, 200, "Volume \%%%d", x);
    }

    return;
}

void gKer_Logic(void *info)
{
    Base_State *gs = info;
    struct kernel_State *ks = gs->info;

    int x = 0;

    for(x = 0; x < KS_DEBUG_VAR; x++)
        ks->debugValue[x] = 0;

    if(ks->fps > 0)
    {
        fps_Cap(&ks->fpsTimer, ks->fps);
        timer_Start(&ks->fpsTimer);
    }

    timer_Calc(&ks->globalTimer);

    sound_Play();

    return;
}

void gKer_Render(void *info)
{
    Base_State *gs = info;
    struct kernel_State *ks = gs->info;

    SDL_Rect test;

    if(!ks->updateScreen)
        return;

    if(ks->cleanScreen)
    {
        ks->cleanScreen = 0;
        surf_Blit(0, 0, ks->clear_Screen, ker_Screen(), NULL);
    }

    /*text_Draw_Arg(ker_Screen_Width() - 100, 5, ker_Screen(), font_Get(1, 11), &tColourBlack, 10, 0, "Debug: %d.%d.%d", K_VERS_1, K_VERS_2, K_VERS_3);
    text_Draw_Arg(ker_Screen_Width() - 100, 25, ker_Screen(), font_Get(1, 11), &tColourBlack, 10, 0, "DM: %d", ks->drawManager.totalObjectLocations);
    */
    drawManager_Draw(&ks->drawManager);

    #ifdef PANDORA
    /*test.x = 0;
    test.y = 0;
    test.w = 400;
    test.h = 240;
    surf_Blit(0, 0, ker_Screen(), ks->clear_Screen, NULL);
    SDL_SoftStretch(ks->clear_Screen,
                    &test,
                    ker_Screen(),
                    NULL);*/
    #endif

    if(SDL_Flip(ker_Screen()) != 0)
    {
        printf("Error can't flip screen\n");
    }

    return;
}

int *gKer_GetDebugVar(int index, int type)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    if(index < 0 || index > KS_DEBUG_VAR)
    {
        printf("Error: debug variable index is not valid - %d\n", index);
        return NULL;
    }

    if(type == 0)
        return &ks->debugValue[index];
    else
        return &ks->staticDebugValue[index];
}

unsigned int gKer_ScreenUpdateEnabled(void)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    return ks->updateScreen;
}

Timer *gKer_Timer(void)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    return &ks->globalTimer;
}

void gKer_SetScreen(int width, int height, int bitsPerPixel, Uint32 flags)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    kernel_SetScreen(width, height, bitsPerPixel, flags);

    if(ks->clear_Screen != NULL)
    {
        SDL_FreeSurface(ks->clear_Screen);
    }

    ks->clear_Screen = surf_Copy(ker_Screen());
    fill_Rect(0, 0, ker_Screen_Width(), ker_Screen_Height(), &colourWhite, ks->clear_Screen);
    ks->cleanScreen = 0;

    return;
}


void gKer_CleanScreen(void)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    ks->cleanScreen = 1;
    return;
}

void gKer_FlushGraphics(void)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    drawManager_Reset(&ks->drawManager);

    return;
}

void gKer_SetFPS(unsigned int fps)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    ks->fps = 1000/fps;

    return;
}

Draw_Manager *gKer_DrawManager(void)
{
    Base_State *gs = ker_BaseState();
    struct kernel_State *ks = gs->info;

    return &ks->drawManager;
}

/*This will clean up the kernel global gamestate, along with all other gamestates that are open*/
void gKer_Exit(void *info)
{
    Base_State *gs = info;
    struct kernel_State *ks = gs->info;

    printf("Kernel state exiting\n");

    printf("Removing all other states\n");
    baseState_Quit_List(ker_BaseStateList());

    control_Clean(&ks->c_Quit);
    control_Clean(&ks->c_Screenshot);
    control_Clean(&ks->c_Mute);
    control_Clean(&ks->c_IncreaseSound);
    control_Clean(&ks->c_DecreaseSound);

    SDL_FreeSurface(ks->clear_Screen);

    printf("Cleaning draw manager\n");
    drawManager_Clean(&ks->drawManager);

    mem_Free(ks);

    puts("Kernel state exited");

    return;
}
