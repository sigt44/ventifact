#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include "../Common/List.h"
#include "Sprite.h"
#include "DrawManager.h"

#define MAX_SPRITES 100

#define S_FREE 1
#define SS_FREE 2
#define N_FREE 0

struct sprite_Info
{
    Sprite *sprite; /*Sprite to draw*/
    int x;          /*x-axis position to draw at*/
    int y;          /*y-axis position to draw at*/
    int free;       /*if A_FREE then sprite will be released from memory after it has been drawn/time is up*/
};

struct sprite_Manager
{
    Draw_Manager dM; /*Draw manager to handle the layering of the sprites*/

    unsigned int spritesDrawn; /*Total sprites drawn in last frame*/
    unsigned int maxSprites; /*Max sprites drawn*/

};

void spriteMan_Setup(Draw_Manager *dM);

/*Push a sprite into the manager*/
void spriteMan_Push(Draw_Manager *dM, int autoFree, int time, int x, int y, Sprite *sprite, SDL_Surface *destSurface);

void spriteMan_DrawObject(Draw_Object *drawObj);
void spriteMan_CleanObject(Draw_Object *drawObj);

#endif
