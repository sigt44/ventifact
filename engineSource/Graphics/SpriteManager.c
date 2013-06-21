#include "SpriteManager.h"

#include "../Common/List.h"
#include "Surface.h"
#include "Sprite.h"
#include "Animation.h"
#include "../Kernel/Kernel.h"
#include "../Kernel/Kernel_State.h"


void spriteMan_Setup(Draw_Manager *dM)
{
    drawManager_AddType(dM, drawType_Create(KER_DRAWTYPE_SPRITE, &spriteMan_DrawObject, &spriteMan_CleanObject, sizeof(struct sprite_Info), &dM->totalObjectLocations));

    return;
}

/*
    Function: spriteMan_Push

    Description -
    Inserts a sprite into the manager so that it can be drawn.

    7 arguments:
    struct sprite_Manager *sM - The sprite manager the sprite is to be inserted in.
    int autoFree - If A_FREE then the sprite will be deleted once it is no longer used by the manager.
    int time - The time the surface should be drawn.
    int x - The x-axis distance the sprite is drawn along the destination surface.
    int y - The y-axis distance the sprite is drawn along the destination surface.
    Sprite *sprite - The sprite to draw.
    SDL_Surface *destSurface - The surface that is being drawn onto.
*/
void spriteMan_Push(Draw_Manager *dM, int autoFree, int time, int x, int y, Sprite *sprite, SDL_Surface *destSurface)
{
    struct sprite_Info *sI = NULL;

    /*Obtain the space that the sprite should be placed in*/
    sI = (struct sprite_Info *)drawManager_Push(dM, sprite->layer, KER_DRAWTYPE_SPRITE, time, destSurface);

    /*If NULL then there is probably too many sprites to draw so exit early*/
    if(sI == NULL)
    {
        //printf("Error possible max sprites reached (%d)\n", sM->dM.totalObjectLocations);
        if(autoFree == A_FREE)
        {
            //printf("Releasing sprite(%p) %d (%d / %d)\n", sprite, autoFree, M_FREE, A_FREE);
            sprite_Clean(sprite);
            mem_Free(sprite);
        }

        return;
    }

    /*Fill in the sprite info struct*/
    sI->x = x;
    sI->y = y;
    sI->sprite = sprite;
    sI->free = autoFree;

    return;
}

/*
    Function: spriteMan_Clean

    Description -
    Delete all links to the sprites the manager contains and if requested delete the sprites.

    1 argument:
    struct sprite_Manager *sM - The sprite manager to clean.
*/
void spriteMan_Clean(struct sprite_Manager *sM)
{
    struct sprite_Info *sI = NULL;
    Draw_Layer *layer = NULL;

    return;
}


void spriteMan_DrawObject(Draw_Object *drawObj)
{
    struct sprite_Info *sI = drawObj->object;

    /*Dont use the draw manager to draw each surface in the sprite, draw it directly on screen*/
    sI->sprite->useDirectDraw = 1;
    sprite_DrawAtPos(sI->x, sI->y, sI->sprite, drawObj->destSurface);
    sI->sprite->useDirectDraw = 0;

    return;
}

void spriteMan_CleanObject(Draw_Object *drawObj)
{
    struct sprite_Info *sI = drawObj->object;

    if(sI->free == A_FREE)
    {
        sprite_Clean(sI->sprite);
        mem_Free(sI->sprite);
    }

    return;
}
