#include "FadeIn.h"
#include <stdint.h>
#include "../SpriteEffects.h"

#include "../../Memory.h"

static void spE_FadeIn_Init(struct sprite *sprite)
{
    Sprite_Effect *effect = sprite->currentEffect;
    const float alpha_Opaque = SDL_ALPHA_OPAQUE * 0.01f;
    float *percentage;

    if(spriteEffect_Verify(sprite, "Fade in", SPE_FADEIN_ARGS) == 1)
            return;

    /*Create the data needed for the effect function*/
    effect->effectData = (float *)mem_Malloc(sizeof(float), __LINE__, __FILE__);

    percentage = effect->effectData;
    *percentage = 0;

    sprite_SetAlpha(sprite, (uint8_t)(alpha_Opaque * (int)(*percentage)));

    timer_Start(&effect->sT);

    effect->update = effect->state.update;

    return;
}

static void spE_FadeIn_Update(struct sprite *sprite)
{
    Sprite_Effect *effect = sprite->currentEffect;
    const float alpha_Opaque = SDL_ALPHA_OPAQUE * 0.01f;

    int deltaTime = timer_Get_Passed(&effect->sT, 1);
    int speed = (int)sprite->currentEffect->argData->dataArray[0]; /*Percentage increase per second of the alpha value*/

    float *percentage = effect->effectData;

    sprite_SetAlpha(sprite, (uint8_t)(alpha_Opaque * (int)(*percentage)));
    timer_Start(&effect->sT);

    *percentage += speed * (deltaTime * 0.001f);
    /*printf("d = %d dt = %d P = %.2f : %.2f\n",speed,deltaTime, *percentage, speed * (deltaTime * 0.001f));*/

    if(*percentage > 100.0f)
    {
        effect->update = effect->state.exit;
    }

    return;
}

static void spE_FadeIn_Exit(struct sprite *sprite)
{
    int reset = (int)sprite->currentEffect->argData->dataArray[1]; /*Should the sprites alpha value reset after completion*/

    if(reset > 0)
        sprite_SetAlpha(sprite, sprite->alphaOriginal);

    spriteEffect_Remove(sprite, sprite->currentEffect);

    return;
}

static void spE_FadeIn(struct sprite *sprite)
{
    sprite->currentEffect->update = sprite->currentEffect->state.init = &spE_FadeIn_Init;
    sprite->currentEffect->state.update = &spE_FadeIn_Update;
    sprite->currentEffect->state.exit = &spE_FadeIn_Exit;

    sprite->currentEffect->update(sprite);

    return;
}

void spriteSE_FadeIn(Sprite *sprite, int percentageIncreasePS, int resetOnCompletion)
{
    spriteEffect_Create(sprite, &spE_FadeIn, SPE_FADEIN_ARGS, percentageIncreasePS, resetOnCompletion);

    return;
}
