#ifndef EFFECTS_H
#define EFFECTS_H

#include <SDL/SDL_mixer.h>

#include "DataStruct.h"
#include "Graphics/Sprite.h"
#include "Time/Timer.h"

#include "Camera.h"

typedef struct Vent_Effect
{
    int state; /*State of the effect*/

    Vector2DInt position; /*Position of the effect*/
    Sprite *sEffect; /*Sprite of the effect*/

    Mix_Chunk *sound; /*Sound of the effect*/

    Timer effectTime; /*Timer to determine how long the effect will last*/

    SDL_Surface *destination; /*Which surface to blit the effect to*/

    Data_Struct *extraData; /*Points to any extra data that the drawing function requires*/

    void (*draw)(struct Vent_Effect *effect, Camera_2D *offsetCamera); /*Points to the drawing function of the effect*/

} Vent_Effect;

Vent_Effect *vEffect_Insert(struct list **effectList, Vent_Effect *effect);

Vent_Effect *vEffect_Create(Sprite *sEffect, Mix_Chunk *sound, int posX, int posY, int effectTime, Timer *srcTime, void (*drawFunction)(Vent_Effect *effect, Camera_2D *offsetCamera), void *extraData, SDL_Surface *destination);

void vEffect_Clean(Vent_Effect *effect);

int vEffect_IsComplete(Vent_Effect *effect);

void vEffect_DrawStandard(Vent_Effect *effect, Camera_2D *offsetCamera);

void vEffect_DrawMiddle(Vent_Effect *effect, Camera_2D *offsetCamera);

void vEffect_DrawScatter(Vent_Effect *effect, Camera_2D *offsetCamera);

#endif
