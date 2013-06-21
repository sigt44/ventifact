#ifndef SURFACEMANAGER_H
#define SURFACEMANAGER_H

#include "../Common/List.h"
#include "Sprite.h"
#include "DrawManager.h"
#include "../Time/Timer.h"

#define MAX_SURFACES 2000

struct surface_Info
{
    SDL_Surface *surface; /*Surface to draw*/
    SDL_Rect clip; /*The clipping of the surface to draw*/

    int x; /*x-axis location*/
    int y; /*y-axis location*/

    int free; /*If A_FREE then the manager should delete the surface after use*/
};

struct surface_Manager
{
    Draw_Manager dM; /*Draw manager to handle the layering of the surfaces*/

    unsigned int surfacesDrawn; /*Total surfaces drawn in last frame*/
    unsigned int maxSurfaces; /*Max surfaces drawn*/
};

void surfaceMan_Setup(Draw_Manager *dM);

void surfaceMan_Push(Draw_Manager *dM, int autoFree, int layer, int time, int x, int y, SDL_Surface *surface, SDL_Surface *destSurface, SDL_Rect *clip);

void surfaceMan_DrawObject(Draw_Object *drawObj);

void surfaceMan_CleanObject(Draw_Object *drawObj);

#endif
