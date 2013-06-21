#ifndef SPRITEEFFECTS_H
#define SPRITEEFFECTS_H

#include "../DataStruct.h"
#include "../Time/Timer.h"
#include "Sprite.h"

typedef struct sprite_Effect_State
{
    void (*init)(struct sprite *sprite); /*Points to function that will init the effect*/
    void (*update)(struct sprite *sprite); /*Points to function that will mess with the sprite :)*/
    void (*exit)(struct sprite *sprite); /*Points to function that will finish the effect*/
} SE_State;


typedef struct sprite_Effect
{
    int numArg; /* How many arguments does the effect need? Will be used to check for errors incase user does not pass the correct amount*/

    Data_Struct *argData; /*User inserted arguments*/

    void *effectData; /*Effect inserted data*/

    SE_State state;

    struct list *spriteListLocation; /*To easily remove the effect*/

    void (*update)(struct sprite *sprite); /*Update the effect*/

    Timer sT;
} Sprite_Effect;

/*Create the effect and place it into the sprite*/
void spriteEffect_Create(struct sprite *srcSprite, void (*type)(struct sprite *sprite), unsigned int numData, ...);

/*Remove effect from a sprite*/
void spriteEffect_Remove(struct sprite *sprite, Sprite_Effect *effect);

/*Delete effect*/
void spriteEffect_Delete(struct sprite_Effect *spEffect);

/*Used by effect functions such as fadeout to detect if the correct number of arguments have been passed*/
int spriteEffect_Verify(struct sprite *sprite, const char *effectName, int eArgs);

/*Check if the sprite has an effect on it*/
int spriteEffect_Check(struct sprite *sprite);


#endif
