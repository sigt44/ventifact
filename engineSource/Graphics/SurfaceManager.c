#include "SurfaceManager.h"

#include "../Common/List.h"
#include "Surface.h"
#include "Animation.h"
#include "DrawManager.h"
#include "../File.h"
#include "../Kernel/Kernel.h"
#include "../Kernel/Kernel_State.h"

void surfaceMan_Setup(Draw_Manager *dM)
{
    drawManager_AddType(dM, drawType_Create(KER_DRAWTYPE_SURFACE, &surfaceMan_DrawObject, &surfaceMan_CleanObject, sizeof(struct surface_Info), &dM->totalObjectLocations));

    return;
}

/*
    Function: surfaceMan_Push

    Description -
    Inserts a surface into the manager so that it can be drawn.

    9 arguments:
    struct surface_Manager *sM - The surface manager the surface is to be inserted in.
    int autoFree - If A_FREE then the surface will be deleted once it is no longer used by the manager.
    int layer - The layer of the surface.
    int time - The time the surface should be drawn.
    int x - The x-axis distance the surface is drawn along the destination surface.
    int y - The y-axis distance the surface is drawn along the destination surface.
    SDL_Surface *surface - The surface to draw.
    SDL_Surface *destSurface - The surface that is being drawn onto.
    SDL_Rect *clip - The clipping of the surface.
*/
void surfaceMan_Push(Draw_Manager *dM, int autoFree, int layer, int time, int x, int y, SDL_Surface *surface, SDL_Surface *destSurface, SDL_Rect *clip)
{
    struct surface_Info *sI = NULL;

    /*Obtain the space that the surface should be placed in*/
    sI = (struct surface_Info *)drawManager_Push(dM, layer, KER_DRAWTYPE_SURFACE, time, destSurface);

    /*If NULL then there is probably too many surfaces to draw so exit early*/
    if(sI == NULL)
    {
        //printf("Error possible max surfaces reached (%d)\n", sM->dM.totalObjectLocations);
        if(autoFree == A_FREE)
        {
            //printf("Releasing surface(%p) %d (%d / %d)\n", surface, autoFree, M_FREE, A_FREE);
            SDL_FreeSurface(surface);
        }

        return;
    }

    /*Fill in the surface info struct*/
    sI->surface = surface;

    sI->x = x;
    sI->y = y;
    sI->free = autoFree;

    if(clip != NULL)
    {
        sI->clip.x = clip->x;
        sI->clip.y = clip->y;
        sI->clip.w = clip->w;
        sI->clip.h = clip->h;
    }
    else
    {
        sI->clip.w = 0;
    }

    return;
}


void surfaceMan_DrawObject(Draw_Object *drawObj)
{
    struct surface_Info *sI = drawObj->object;

    /*If the surface is clipped*/
    if(sI->clip.w != 0)
        surf_Blit(sI->x, sI->y, sI->surface, drawObj->destSurface, &sI->clip);
    else
        surf_Blit(sI->x, sI->y, sI->surface, drawObj->destSurface, NULL);

    return;
}

void surfaceMan_CleanObject(Draw_Object *drawObj)
{
    struct surface_Info *sI = drawObj->object;

    if(sI->free == A_FREE)
        SDL_FreeSurface(sI->surface);

    return;
}
