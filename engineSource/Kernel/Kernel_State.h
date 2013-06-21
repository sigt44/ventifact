#ifndef KERNEL_STATE_H
#define KERNEL_STATE_H
#include "SDL/SDL.h"
#include "../BaseState.h"
#include "../Time/Timer.h"

#include "../Graphics/SurfaceManager.h"
#include "../Graphics/SpriteManager.h"
#include "../Graphics/PixelManager.h"

enum
{
    KER_DRAWTYPE_SURFACE = 0,
    KER_DRAWTYPE_SPRITE = 1,
    KER_DRAWTYPE_PIXEL = 2,
} KER_DRAWTYPES;

int kernel_State_Loop(void);

int kernel_Setup_BaseState(void);


void gKer_Init(void *info);

void gKer_Controls(void *info);

void gKer_Logic(void *info);

void gKer_Render(void *info);

void gKer_Exit(void *info);


int *gKer_GetDebugVar(int index, int type);

unsigned int gKer_ScreenUpdateEnabled(void);

Timer *gKer_Timer(void);

void gKer_SetScreen(int width, int height, int bitsPerPixel, Uint32 flags);

void gKer_CleanScreen(void);

void gKer_SetFPS(unsigned int fps);

void gKer_FlushGraphics(void);

Draw_Manager *gKer_DrawManager(void);

#endif
