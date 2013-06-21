#include "HealthBar.h"

#include "Graphics/Graphics.h"

#include "Defines.h"

/*
    Function: vHealthBar_Setup

    Description -
    Setup a health bar.

    8 arguments:
    Vent_HealthBar *hBar - The health bar to setup.
    Timer *srcTime - The source time the health bar display timer should use.
    int x - The x-axis position of the health bar.
    int y - The y-axis position of the health bar.
    int width - The width of the health bar.
    int height - The height of the health bar.
    int padding - Padding of the health bar onto the background bar.
    Uint16P barColour - The Uint16/32 colour of the health bar.
*/
void vHealthBar_Setup(Vent_HealthBar *hBar, Timer *srcTime, int x, int y, int width, int height, int padding, Uint16P barColour)
{
    SDL_Surface *healthMeter[2] = {NULL, NULL};

    hBar->padding = padding;
    hBar->width = width;
    hBar->height = height;

    /*Create two surfaces, the background of the health bar and the actual health bar*/
    healthMeter[0] = surf_Create(width, height);
    healthMeter[1] = surf_Create(width - padding, height - padding);

    fill_Rect(0, 0, healthMeter[0]->w, healthMeter[0]->h, &colourBlack, healthMeter[0]);
    fill_Rect(0, 0, healthMeter[1]->w, healthMeter[1]->h, &barColour, healthMeter[1]);

    /*Offset the health bar to fit the background bar*/
    healthMeter[1] = surf_Offset(padding/2, padding/2, healthMeter[1], 1);

    /*Setup the main sprite, with the background as a global frame.*/
    sprite_Setup(&hBar->sHealthMeter, 0, VL_BUILDING + 1, srcTime, 2,
                frame_CreateBasic(-1, healthMeter[0], A_FREE),
                frame_CreateBasic(0, healthMeter[1], A_FREE));

    /*Set the position of the health bar*/
    sprite_SetPos(&hBar->sHealthMeter, x - (healthMeter[0]->w/2), y);

    /*Setup the display timer*/
    hBar->healthMeterTimer = timer_Setup(srcTime, 0, 0, 1);

    return;
}

/*
    Function: vHealthBar_Draw

    Description -
    Draws the health bar if it with camera range and if the display timer has not finished.

    3 arguments:
    Vent_HealthBar *hBar - The health bar to draw.
    Camera_2D *c - The camera the health bar should be inside of.
    SDL_Surface *destination - The destination surface to draw the health bar to.
*/
void vHealthBar_Draw(Vent_HealthBar *hBar, Camera_2D *c, SDL_Surface *destination)
{
    if(timer_Get_Remain(&hBar->healthMeterTimer) > 0)
    {
        timer_Calc(&hBar->healthMeterTimer);

        sprite_DrawAtPos(hBar->sHealthMeter.x - c->iPosition.x, hBar->sHealthMeter.y - c->iPosition.y, &hBar->sHealthMeter, destination);
    }

    return;
}

/*
    Function: vHealthBar_Display

    Description -
    Set the health bar active for a certain amount of time. Updates
    the health bars width.

    3 arguments:
    Vent_HealthBar *hBar - The health bar to update.
    float healthRatio - The health ratio needed to update the health bars width.
    int displayTime - The time in ms that the health bar will be displayed for.
*/
void vHealthBar_Display(Vent_HealthBar *hBar, float healthRatio, int displayTime)
{
    int bWidth = 0;

    /*Get the second frame containing the actual health surface*/
    sprite_SetFrame(&hBar->sHealthMeter, 1);

    /*Set the health bar width at the correct proportion*/
    bWidth = (int)((hBar->width - hBar->padding) * healthRatio);

    /*If the building is pretty much destroyed just set the width to 1 pixel*/
    if(bWidth == 0)
    {
        bWidth = 1;
    }

    /*Update the clipping of the health bar width*/
    sprite_SetCurrentClip(&hBar->sHealthMeter, 0, 0, bWidth, sprite_Height(&hBar->sHealthMeter));

    timer_Start(&hBar->healthMeterTimer);

    hBar->healthMeterTimer.end_Time = displayTime;

    sprite_SetFrame(&hBar->sHealthMeter, 0);

    return;
}

/*
    Function: vHealthBar_Clean

    Description -
    Cleans up the health bar.

    1 argument:
    Vent_HealthBar *hBar - The health bar to clean up.
*/
void vHealthBar_Clean(Vent_HealthBar *hBar)
{
    sprite_Clean(&hBar->sHealthMeter);

    return;
}
