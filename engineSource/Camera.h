#ifndef CAMERA_H
#define CAMERA_H

#include <SDL/SDL.h>
#include "Vector2DInt.h"
#include "Vector2D.h"

typedef struct Camera_2D
{
    Vector2D fPosition;
    Vector2DInt iPosition;
    Vector2DInt offset;

    int width;
    int height;

    int minX;
    int minY;
    int maxX;
    int maxY;

    int locked;

    SDL_Rect posRect;
    SDL_Rect originRect;

} Camera_2D;

Camera_2D *camera2D_Create(void);

void camera2D_Setup(Camera_2D *c, int oX, int oY, int width, int height, int maxX, int maxY, int offsetX, int offsetY);

void camera2D_SetOffsets(Camera_2D *c, int offsetX, int offsetY);

void camera2D_SetConstraints(Camera_2D *c, int minX, int minY, int maxX, int maxY);

void camera2D_SetOrigin(Camera_2D *c, int x, int y);

void camera2D_SetPosition(Camera_2D *c, int x, int y);

void camera2D_ChangePosition(Camera_2D *c, float xDelta, float yDelta);

void camera2D_CorrectPosition(Camera_2D *c);

void camera2D_FixConstraints(Camera_2D *c);

int camera2D_Collision(Camera_2D *c, int x, int y, int w, int h);

int camera2D_RelativeX(Camera_2D *c, int x);

int camera2D_RelativeY(Camera_2D *c, int y);

#endif
