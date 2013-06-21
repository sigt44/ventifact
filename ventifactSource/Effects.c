#include "Effects.h"

#include "Sound.h"
#include "Graphics/Sprite.h"
#include "DataStruct.h"
#include "Memory.h"

#include "Camera.h"
#include "Time/Timer.h"

#include "Game/VentGame.h"

/*
    Function: vEffect_Insert

    Description -
    Inserts an effect to a list.

    2 arguments:
    struct list **effectList - The list to insert the effect into.
    Vent_Effect *effect - The effect to insert in the list.
*/
Vent_Effect *vEffect_Insert(struct list **effectList, Vent_Effect *effect)
{
    list_Push(effectList, effect, 0);

    return effect;
}

/*
    Function: vEffect_Create

    Description -
    Returns a new effect.

    9 arguments:
    Sprite *sEffect - The sprite that the effect will use. This will be copied.
    Mix_Chunk *sound - The sound that may be played by the effect.
    int posX - The x-axis position of the effect.
    int posY - The y-axis position of the effect.
    Timer *srcTime - The source time that the effect timer will use.
    void (*drawFunction)(Vent_Effect *effect) - The drawing function of the effect.
    void *extraData - any extra data that the drawing function needs to perform.
    SDL_Surface *destination - The surface to blit the sprite onto.
*/
Vent_Effect *vEffect_Create(Sprite *sEffect, Mix_Chunk *sound, int posX, int posY, int effectTime, Timer *srcTime, void (*drawFunction)(Vent_Effect *effect, Camera_2D *offsetCamera), void *extraData, SDL_Surface *destination)
{
    Vent_Effect *nEffect = (Vent_Effect *)mem_Malloc(sizeof(Vent_Effect), __LINE__, __FILE__);

    nEffect->position.x = posX;
    nEffect->position.y = posY;

    nEffect->effectTime = timer_Setup(srcTime, 0, effectTime, 1);

    nEffect->sEffect = sprite_Copy(sEffect, 0);
    nEffect->sound = sound;

    nEffect->state = 0;

    nEffect->sEffect->frameTime.srcTime = &nEffect->effectTime;

    nEffect->extraData = extraData;

    nEffect->draw = drawFunction;

    nEffect->destination = destination;

    return nEffect;
}

/*
    Function: vEffect_Clean

    Description -
    Releases any dynamically allocated memory used by the effect.

    1 argmument:
    Vent_Effect *effect - the effect to clean
*/
void vEffect_Clean(Vent_Effect *effect)
{
    sprite_Clean(effect->sEffect);

    mem_Free(effect->sEffect);

    if(effect->extraData != NULL)
    {
        dataStruct_Delete(&effect->extraData);
    }

    return;
}

/*
    Function: vEffect_IsComplete

    Description -
    Returns 1 if the effect is complete, otherwise returns 0.

    1 argument:
    Vent_Effect *effect - The effect to check if it is complete.
*/
int vEffect_IsComplete(Vent_Effect *effect)
{
    timer_Calc(&effect->effectTime);

    if(timer_Get_Remain(&effect->effectTime) <= 0)
    {
        return 1;
    }

    return 0;
}

/*
    Function: vEffect_DrawStandard

    Description -
    The standard drawing function for an effect. Blits the sprite at a constant position.

    Expects extra variable to contain
    [0]: Vent_Game *currentGame

    2 arguments:
    Vent_Effect *effect - The effect to draw.
    Camera_2D *offsetCamera - The camera the effect is seen in.
*/
void vEffect_DrawStandard(Vent_Effect *effect, Camera_2D *offsetCamera)
{
    Vent_Game *game = effect->extraData->dataArray[0];

    if(timer_Get_Stopped(&effect->effectTime) > 0)
    {
        effect->state = 0;
        return;
    }
    else if(effect->state == 0)
    {
        effect->state = 1;

        if(effect->sound != NULL)
        {
            sound_Call(effect->sound, (int)effect->sound, SND_DUPLICATE_SOFT, vGame_SoundVolume_Distance(game, effect->position.x, effect->position.y), 0);
        }
    }

    if(camera2D_Collision(offsetCamera, effect->position.x, effect->position.y, sprite_Width(effect->sEffect), sprite_Height(effect->sEffect)) == 1)
    {
        sprite_DrawAtPos(effect->position.x - offsetCamera->iPosition.x, effect->position.y - offsetCamera->iPosition.y, effect->sEffect, effect->destination);
    }
    else
    {
        /*Dont blit the sprite but update its frames*/
        sprite_Draw(effect->sEffect, NULL);
    }

    return;
}

/*
    Function: vEffect_DrawMiddle

    Description -
    Blits the sprite at the same position relative to its middle point.

    Expects extra variable to contain
    [0]: Vent_Game *currentGame

    2 arguments:
    Vent_Effect *effect - The effect to draw.
    Camera_2D *offsetCamera - The camera the effect is seen in.
*/
void vEffect_DrawMiddle(Vent_Effect *effect, Camera_2D *offsetCamera)
{
    Vent_Game *game = effect->extraData->dataArray[0];

    int x = effect->position.x - (sprite_Width(effect->sEffect)/2);
    int y = effect->position.y - (sprite_Height(effect->sEffect)/2);

    if(timer_Get_Stopped(&effect->effectTime) > 0)
    {
        effect->state = 0;
        return;
    }
    else if(effect->state == 0)
    {
        effect->state = 1;
        if(effect->sound != NULL)
        {
            sound_Call(effect->sound, (int)effect->sound, SND_DUPLICATE_SOFT, vGame_SoundVolume_Distance(game, effect->position.x, effect->position.y), 0);
        }
    }

    if(camera2D_Collision(offsetCamera, x, y, sprite_Width(effect->sEffect), sprite_Height(effect->sEffect)) == 1)
    {
        sprite_DrawAtPos(x - offsetCamera->iPosition.x, y - offsetCamera->iPosition.y, effect->sEffect, effect->destination);
    }
    else
    {
        /*Dont blit the sprite but update its frames*/
        sprite_Draw(effect->sEffect, NULL);
    }

    return;
}

/*
    Function: vEffect_DrawScatter

    Description -
    Blits the sprite multiple times at random positions around a point.

    Expects the effects extraData variable to contain a Data_Struct of 5 objects
    [0]: Vent_Game *currentGame
    [1]: (struct list **) - effectList
    [2]: (int *) - scatteredEffects
    [3]: (int *) - scatterAmount
    [4]: (int *) - scatterTimeDelta

    2 arguments:
    Vent_Effect *effect - The effect to draw.
    Camera_2D *offsetCamera - The camera the effect is seen in.
*/
void vEffect_DrawScatter(Vent_Effect *effect, Camera_2D *offsetCamera)
{
    Data_Struct *scatterInfo = effect->extraData;

    Vent_Game *game = scatterInfo->dataArray[0];
    struct list **effectList = scatterInfo->dataArray[1];
    int *scatteredEffects = scatterInfo->dataArray[2];
    int *scatterAmount = scatterInfo->dataArray[3];
    int *scatterTimeDelta = scatterInfo->dataArray[4];

    int x = 0;
    Vector2DInt scatterPos = {0, 0};

    Vent_Effect *newEffect = NULL;
    Sprite *eSprite = NULL;

    /*Set the first effect to draw as standard*/
    effect->draw = vEffect_DrawStandard;

    dataStruct_Clean(effect->extraData);
    mem_Free(effect->extraData);
    effect->extraData = dataStruct_Create(1, game);

    /*Scatter new effects around the first effects position.*/
    for(x = 0; x < *scatteredEffects; x++)
    {
        /*Scatter the new effects position*/
        if(x <= *scatteredEffects/4)
        {
            scatterPos.x = effect->position.x + (rand() % *scatterAmount);
            scatterPos.y = effect->position.y + (rand() % *scatterAmount);
        }
        else if(x <= *scatteredEffects/2)
        {
            scatterPos.x = effect->position.x - (rand() % *scatterAmount);
            scatterPos.y = effect->position.y + (rand() % *scatterAmount);
        }
        else if(x <= 3 * (*scatteredEffects/4))
        {
            scatterPos.x = effect->position.x + (rand() % *scatterAmount);
            scatterPos.y = effect->position.y - (rand() % *scatterAmount);
        }
        else
        {
            scatterPos.x = effect->position.x - (rand() % *scatterAmount);
            scatterPos.y = effect->position.y - (rand() % *scatterAmount);
        }

        /*Create the new effect*/
        newEffect = vEffect_Create(effect->sEffect, effect->sound, scatterPos.x, scatterPos.y, effect->effectTime.end_Time, effect->effectTime.srcTime, vEffect_DrawStandard, dataStruct_Create(1, game), effect->destination);

        /*Delay the new effect by a random amount*/
        timer_Stop(&newEffect->effectTime, (x * (*scatterTimeDelta + (rand() % 100))) + rand() % 400);

        vEffect_Insert(effectList, newEffect);
    }

    effect->state = 0;

    return;
}

