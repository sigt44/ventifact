#include "PixelManager.h"

#include "../Kernel/Kernel_State.h"

void pixelMan_Setup(Draw_Manager *dM)
{
    drawManager_AddType(dM, drawType_Create(KER_DRAWTYPE_PIXEL, &pixelMan_DrawObject, NULL, sizeof(Pixel_DrawInfo), &dM->totalObjectLocations));

    return;
}

void pixelMan_PushLine(Draw_Manager *dM, int layer, int sX, int sY, int eX, int eY, int thickness, Uint16P colour, SDL_Surface *destSurface)
{
    Pixel_DrawInfo *pI = NULL;

    /*Obtain the linked list node that the pixel should be placed in*/
    pI = (Pixel_DrawInfo *)drawManager_Push(dM, layer, KER_DRAWTYPE_PIXEL, 0, destSurface);

    /*If NULL then there is probably too many pixels to draw so exit early*/
    if(pI == NULL)
    {
        return;
    }

    /*Fill in the pixel info struct*/
    pI->shapeType = PM_LINE;
    pI->pixelThickness = thickness;
    pI->shape.line.sX = sX;
    pI->shape.line.sY = sY;
    pI->shape.line.eX = eX;
    pI->shape.line.eY = eY;

    pI->colour = colour;

    return;
}

void pixelMan_PushCircle(Draw_Manager *dM, int layer, int cX, int cY, int radius, int thickness, Uint16P colour, SDL_Surface *destSurface)
{
    Pixel_DrawInfo *pI = NULL;

    /*Obtain the linked list node that the pixel should be placed in*/
    pI = (Pixel_DrawInfo *)drawManager_Push(dM, layer, KER_DRAWTYPE_PIXEL, 0, destSurface);

    /*If NULL then there is probably too many pixels to draw so exit early*/
    if(pI == NULL)
    {
        return;
    }

    /*Fill in the pixel info struct*/
    pI->shapeType = PM_LINE;
    pI->pixelThickness = thickness;
    pI->shape.circle.cX = cX;
    pI->shape.circle.cY = cY;
    pI->shape.circle.radius = radius;

    pI->colour = colour;

    return;
}

void pixelMan_Push(Draw_Manager *dM, int layer, int x, int y, int w, int h, Uint16P colour, SDL_Surface *destSurface)
{
    Pixel_DrawInfo *pI = NULL;

    /*Obtain the linked list node that the pixel should be placed in*/
    pI = (Pixel_DrawInfo *)drawManager_Push(dM, layer, KER_DRAWTYPE_PIXEL, 0, destSurface);

    /*If NULL then there is probably too many pixels to draw so exit early*/
    if(pI == NULL)
    {
        return;
    }

    /*Fill in the pixel info struct*/
    pI->shapeType = PM_RECT;
    pI->pixelThickness = 1;
    pI->shape.rect.x = x;
    pI->shape.rect.y = y;
    pI->shape.rect.w = w;
    pI->shape.rect.h = h;
    pI->colour = colour;

    if(x < 0)
    {
        pI->shape.rect.w += x;
        pI->shape.rect.x = 0;
    }
    else if(x >= destSurface->w)
    {
        pI->shape.rect.w = 0;
        pI->shape.rect.h = 0;

        return;
    }
    else if((w + x) > destSurface->w)
    {
        pI->shape.rect.w = destSurface->w - x;
    }

    if(y < 0)
    {
        pI->shape.rect.h += y;
        pI->shape.rect.y = 0;
    }
    else if(y >= destSurface->h)
    {
        pI->shape.rect.h = 0;
        pI->shape.rect.w = 0;

        return;
    }
    else if(h + y > destSurface->h)
    {
        pI->shape.rect.h = destSurface->h - y;
    }

    return;
}

void pixelMan_DrawObject(Draw_Object *drawObj)
{
    Pixel_DrawInfo *pI = drawObj->object;

    switch(pI->shapeType)
    {
        case PM_RECT:

        pixel_Fill(pI->shape.rect.x, pI->shape.rect.y, pI->shape.rect.w, pI->shape.rect.h, pI->colour, drawObj->destSurface);

        break;

        case PM_LINE:
        //printf("Line %d: %d %d to %d %d\n", x, pI->shape.line.sX, pI->shape.line.sY, pI->shape.line.eX, pI->shape.line.eY);
        //x++;
        pixel_Line(pI->shape.line.sX, pI->shape.line.sY, pI->shape.line.eX, pI->shape.line.eY, pI->pixelThickness, 1, &pI->colour, drawObj->destSurface);
        //lineColor(pI->destSurface, pI->shape.line.sX, pI->shape.line.sY, pI->shape.line.eX, pI->shape.line.eY, pI->colour);
        break;

        case PM_CIRCLE:

        break;

        default:
        printf("Error unknown shape type to draw (%d)\n", pI->shapeType);
        break;
    }

    return;
}

