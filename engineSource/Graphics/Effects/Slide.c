#include "Slide.h"
#include "../SpriteEffects.h"
#include "../../Maths.h"
#include "../../Memory.h"
#include <math.h>

static void spE_Slide_Init(struct sprite *sprite)
{
    Sprite_Effect *effect = sprite->currentEffect;
    const float alpha_Opaque = SDL_ALPHA_OPAQUE * 0.01f;
    int *startX;
    int *startY;

    float *distance;
    float *fX;
    float *fY;

    if(spriteEffect_Verify(sprite, "Slide", SPE_SLIDE_ARGS) == 1)
            return;

    /*Create the data needed for the effect function*/
    effect->effectData = (int *)mem_Malloc((sizeof(int) * 2) + (sizeof(float) * 3),  __LINE__, __FILE__);

    startX = effect->effectData;
    *startX = sprite->x;

    startY = (int *)((char *)effect->effectData + sizeof(int));
    *startY = sprite->y;

    distance = (float *)((char *)effect->effectData + (sizeof(int) * 2));
    *distance = 0.0f;

    fX = (float *)((char *)effect->effectData + (sizeof(int) * 2) + sizeof(float));
    *fX = (float)(*startX);

    fY = (float *)((char *)effect->effectData + (sizeof(int) * 2) + (sizeof(float) * 2));
    *fY = (float)(*startY);

    timer_Start(&effect->sT);

    effect->update = effect->state.update;

    return;
}

static void spE_Slide_Update(struct sprite *sprite)
{
    Sprite_Effect *effect = sprite->currentEffect;

    int deltaTime = timer_Get_Passed(&effect->sT, 1);
    int speed = (int)sprite->currentEffect->argData->dataArray[0]; /*Speed to move the sprite*/
    float angle = ((int)sprite->currentEffect->argData->dataArray[1]/180.0f) * PI; /*Direction to move the sprite*/
    int maxDistance = (int)sprite->currentEffect->argData->dataArray[2]; /*Distance the sprite must move*/

    float *distanceMoved = (float *)((char *)effect->effectData + (sizeof(int) * 2));
    float *fX = (float *)((char *)effect->effectData + (sizeof(int) * 2) + sizeof(float));
    float *fY = (float *)((char *)effect->effectData + (sizeof(int) * 2) + (sizeof(float) * 2));

    float speedMulti = speed * (deltaTime * 0.001f);

    timer_Start(&effect->sT);

    *distanceMoved += speedMulti;
    *fX += speedMulti * cosf(angle);
    *fY += speedMulti * sinf(angle);
    sprite->x = (int)*fX;
    sprite->y = (int)*fY;

    if((int)(*distanceMoved) > maxDistance)
    {
        effect->update = effect->state.exit;
        sprite->x = *((int *)sprite->currentEffect->effectData) + (int)(maxDistance * cos(angle));
        sprite->y = *((int *)((char *)sprite->currentEffect->effectData + sizeof(int))) + (int)(maxDistance * sin(angle));
    }

    return;
}

static void spE_Slide_Exit(struct sprite *sprite)
{
    int reset = (int)sprite->currentEffect->argData->dataArray[3]; /*Should the sprites location reset after completion*/

    if(reset > 0)
    {
        sprite->x = *((int *)sprite->currentEffect->effectData);
        sprite->y = *((int *)((char*)sprite->currentEffect->effectData + sizeof(int)));
    }

    spriteEffect_Remove(sprite, sprite->currentEffect);

    return;
}

static void spE_Slide(struct sprite *sprite)
{
    sprite->currentEffect->update = sprite->currentEffect->state.init = &spE_Slide_Init;
    sprite->currentEffect->state.update = &spE_Slide_Update;
    sprite->currentEffect->state.exit = &spE_Slide_Exit;

    sprite->currentEffect->update(sprite);

    return;
}

void spriteSE_Slide(Sprite *sprite, int speed, int angle, int distance, int resetOnCompletion)
{
    spriteEffect_Create(sprite, &spE_Slide, SPE_SLIDE_ARGS, speed, angle, distance, resetOnCompletion);

    return;
}
