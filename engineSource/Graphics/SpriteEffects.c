#include "SpriteEffects.h"
#include <stdarg.h>
#include <stdint.h>

#include "../Memory.h"
#include "../Common/List.h"
#include "../File.h"
#include "../Kernel/Kernel.h"

void spriteEffect_Create(struct sprite *srcSprite, void (*type)(struct sprite *sprite), unsigned int numData, ...)
{
    Sprite_Effect *newEffect = (Sprite_Effect *)mem_Malloc(sizeof(Sprite_Effect), __LINE__, __FILE__);
    unsigned int x;
    va_list args;

    if(numData > 0) /*Create arguments needed for effect function*/
    {
        va_start(args, numData);
        newEffect->argData = dataStruct_CreateEmpty(numData);

        for(x = 0; x < numData; )
        {
            dataStruct_InsertData(newEffect->argData, &x, 1, va_arg( args, int));
        }

        va_end(args);
    }
    else
        newEffect->argData = NULL;

    newEffect->numArg = numData;
    newEffect->effectData = NULL;

    newEffect->sT = timer_Setup(srcSprite->frameTime.srcTime, 0, 0, 0);

    newEffect->update = type;

    list_Push(&srcSprite->effectList, newEffect, 0);

    newEffect->spriteListLocation = srcSprite->effectList;

    return;
}


void spriteEffect_Remove(struct sprite *sprite, Sprite_Effect *effect)
{
    list_Delete_Node(&sprite->effectList, effect->spriteListLocation);

    spriteEffect_Delete(effect);

    return;
}

void spriteEffect_Delete(Sprite_Effect *spEffect)
{
    if(spEffect->argData != NULL)
    {
        dataStruct_Clean(spEffect->argData);
        mem_Free(spEffect->argData);
    }

    if(spEffect->effectData != NULL)
        mem_Free(spEffect->effectData);

    mem_Free(spEffect);

    return;
}

int spriteEffect_Check(struct sprite *sprite)
{
    if(sprite->effectList == NULL)
        return 0;
    else
        return 1;
}

int spriteEffect_Verify(struct sprite *sprite, const char *effectName, int eArgs)
{
    Sprite_Effect *effect = sprite->currentEffect;

    if(effect->numArg != eArgs)
    {
        printf("Error not correct amount of arguments(%d / %d) passed to create %s effect", effect->numArg, eArgs, effectName);
        file_Log(ker_Log(),0,"Error not correct amount of arguments(%d / %d) passed to create %s effect", effect->numArg, eArgs, effectName);
        SDL_Delay(500);

        spriteEffect_Remove(sprite, sprite->currentEffect);

        return 1;
    }

    return 0;
}
