#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include "Graphics/Pixel.h"
#include "Graphics/Surface.h"
#include "Graphics/Sprite.h"
#include "Time/Timer.h"

#include "Camera.h"

typedef struct Vent_HealthBar
{
    Sprite sHealthMeter; /*Sprite of the health bar*/

    Timer healthMeterTimer; /*Timer to judge how long the health bar should display*/

    int width; /*Width of the health bar*/
    int height; /*Height of the health bar*/
    int padding; /*Padding of the health bar onto the background bar*/

} Vent_HealthBar;

void vHealthBar_Setup(Vent_HealthBar *hBar, Timer *srcTime, int x, int y, int width, int height, int padding, Uint16P barColour);

void vHealthBar_Draw(Vent_HealthBar *hBar, Camera_2D *c, SDL_Surface *destination);

void vHealthBar_Display(Vent_HealthBar *hBar, float healthRatio, int displayTime);

void vHealthBar_Clean(Vent_HealthBar *hBar);

#endif
