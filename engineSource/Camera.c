#include "Camera.h"

#include <SDL/SDL.h>
#include "Collision.h"
#include "Memory.h"

Camera_2D *camera2D_Create(void)
{
    Camera_2D *newCamera = (Camera_2D *)mem_Alloc(sizeof(Camera_2D));

    camera2D_Setup(newCamera, 0, 0, 0, 0, 0, 0, 0, 0);

    return newCamera;
}

void camera2D_Setup(Camera_2D *c, int oX, int oY, int width, int height, int maxX, int maxY, int offsetX, int offsetY)
{
    c->originRect.w = c->posRect.w = c->width = width;
    c->originRect.h = c->posRect.h = c->height = height;

    camera2D_SetConstraints(c, 0, 0, maxX, maxY);

    c->locked = 0;

    camera2D_SetOrigin(c, oX, oY);

    camera2D_SetOffsets(c, offsetX, offsetY);
    camera2D_SetPosition(c, oX, oY);

    return;
}

void camera2D_SetConstraints(Camera_2D *c, int minX, int minY, int maxX, int maxY)
{
    c->minX = minX;
    c->minY = minY;
    c->maxX = maxX;
    c->maxY = maxY;

    return;
}

void camera2D_SetOffsets(Camera_2D *c, int offsetX, int offsetY)
{
    c->offset.x = offsetX;
    c->offset.y = offsetY;

    return;
}

void camera2D_SetOrigin(Camera_2D *c, int x, int y)
{
    c->originRect.x = x;
    c->originRect.y = y;

    return;
}

void camera2D_ChangePosition(Camera_2D *c, float xDelta, float yDelta)
{
    c->fPosition.x += xDelta;
    c->fPosition.y += yDelta;

    c->iPosition.x = (int)c->fPosition.x;
    c->iPosition.y = (int)c->fPosition.y;

    //camera2D_CorrectPosition(c);

    /*Make sure the camera does not go out of bounds*/
    camera2D_FixConstraints(c);

    c->posRect.x = c->iPosition.x;
    c->posRect.y = c->iPosition.y;

    return;
}

void camera2D_SetPosition(Camera_2D *c, int x, int y)
{
    c->iPosition.x = x;
    c->iPosition.y = y;

    camera2D_CorrectPosition(c);

    c->fPosition.x = (float)c->iPosition.x;
    c->fPosition.y = (float)c->iPosition.y;

    /*Make sure the camera does not go out of bounds*/
    camera2D_FixConstraints(c);

    c->posRect.x = c->iPosition.x;
    c->posRect.y = c->iPosition.y;
    return;
}

void camera2D_FixConstraints(Camera_2D *c)
{
    if(c->iPosition.x < c->minX)
    {
        c->iPosition.x = c->minX;
        c->fPosition.x = (float)c->iPosition.x;
    }
    else if((c->iPosition.x + c->width) > c->maxX)
    {
        c->iPosition.x = (c->maxX - c->width);
        c->fPosition.x = (float)c->iPosition.x;
    }

    if(c->iPosition.y < c->minY)
    {
        c->iPosition.y = c->minY;
        c->fPosition.y = (float)c->iPosition.y;
    }
    else if((c->iPosition.y + c->height) > c->maxY)
    {
        c->iPosition.y = (c->maxY - c->height);
        c->fPosition.y = (float)c->iPosition.y;
    }

    return;
}

void camera2D_CorrectPosition(Camera_2D *c)
{
    c->iPosition = vector2DInt_Add(&c->iPosition, &c->offset);

    return;
}

int camera2D_Collision(Camera_2D *c, int x, int y, int w, int h)
{
    SDL_Rect B = {x, y, w, h};

    return collisionStatic_Rectangle(&c->posRect, &B);
}

int camera2D_RelativeX(Camera_2D *c, int x)
{
    return c->originRect.x + (x - c->iPosition.x);
}

int camera2D_RelativeY(Camera_2D *c, int y)
{
    return c->originRect.y + (y - c->iPosition.y);
}
