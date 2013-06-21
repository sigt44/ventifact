#include "Blink.h"
#include "../Sprite.h"
#include "../SpriteEffects.h"

#include "../../Memory.h"

struct spE_Blink_Info
{
    unsigned int blinkBool;
    unsigned int iterations;
};

static void spE_Blink_Init(struct sprite *sprite)
{
    Sprite_Effect *effect = sprite->currentEffect;
    unsigned int *blinkBool;
    struct spE_Blink_Info *blink_Info;

    int drawDelay = 0;

    if(spriteEffect_Verify(sprite, "Blink", SPE_BLINK_ARGS) == 1)
        return;

    drawDelay = (int)sprite->currentEffect->argData->dataArray[1];

    /*Create the data needed for the effect function*/
    effect->effectData = (struct spE_Blink_Info *)mem_Malloc(sizeof(struct spE_Blink_Info), __LINE__, __FILE__);

    blink_Info = effect->effectData;

    blink_Info->blinkBool = 0;
    blink_Info->iterations = 0;

    timer_Start(&effect->sT);

    if(drawDelay == 0)
       sprite_SetAlpha(sprite, SDL_ALPHA_TRANSPARENT);

    effect->update = effect->state.update;

    return;
}

static void spE_Blink_Update(struct sprite *sprite)
{
    Sprite_Effect *effect = sprite->currentEffect;

    int deltaTime = timer_Get_Passed(&effect->sT, 1);
    int blankDelay = (int)sprite->currentEffect->argData->dataArray[0];
    int drawDelay = (int)sprite->currentEffect->argData->dataArray[1];
    int max_Iterations = (int)sprite->currentEffect->argData->dataArray[2];

    struct spE_Blink_Info *blink_Info = effect->effectData;

    switch(blink_Info->blinkBool)
    {
        case 1:
        if(timer_Get_Passed(&effect->sT, 1) > (unsigned int)blankDelay)
        {
            timer_Start(&effect->sT);
            sprite_SetAlpha(sprite, sprite->alphaOriginal);
            blink_Info->blinkBool = 0;

            blink_Info->iterations ++;
        }
        break;

        case 0:
        if(timer_Get_Passed(&effect->sT, 1) > (unsigned int)drawDelay)
        {
            timer_Start(&effect->sT);
            sprite_SetAlpha(sprite, SDL_ALPHA_TRANSPARENT);
            blink_Info->blinkBool = 1;
        }
        break;
    }

    if(blink_Info->iterations >= (unsigned int)max_Iterations)
    {
        effect->update = effect->state.exit;
    }

    return;
}

static void spE_Blink_Exit(struct sprite *sprite)
{
    sprite_SetAlpha(sprite, sprite->alphaOriginal);

    spriteEffect_Remove(sprite, sprite->currentEffect);

    return;
}

static void spE_Blink(struct sprite *sprite)
{
    sprite->currentEffect->update = sprite->currentEffect->state.init = &spE_Blink_Init;
    sprite->currentEffect->state.update = &spE_Blink_Update;
    sprite->currentEffect->state.exit = &spE_Blink_Exit;

    sprite->currentEffect->update(sprite);

    return;
}

void spriteSE_Blink(Sprite *sprite, int timeDrawn, int timeVanished, int numBlinks)
{
    spriteEffect_Create(sprite, &spE_Blink, SPE_BLINK_ARGS, timeDrawn, timeVanished, numBlinks);

    return;
}
