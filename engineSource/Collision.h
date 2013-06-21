#ifndef COLLISION_H
#define COLLISION_H

#include "SDL/SDL.h"

#include "Vector2D.h"

struct col_Info
{
    int xDis;
    int yDis;
    int tDis;

    int data;

    unsigned int collision;
};

unsigned int collision_Circle(Vector2D *pA, Vector2D *vA, float radiusA, Vector2D *pB, Vector2D *vB, float radiusB, int fixVelocity);

unsigned int collision_CircleLine(Vector2D *lineStart, Vector2D *lineEnd, Vector2D *circlePos, Vector2D *velocity, float radius, Vector2D *cPoint);

int collisionStatic_Circle(int aX, int aY, int aRadius, int bX, int bY, int bRadius);

int collisionStatic_SemiCircle(int aX, int aY, int aRadius, int bX, int bY, int bRadius);

int collisionStatic_CircRect(Vector2D *posCircle, int *radius, Vector2D *posRect, SDL_Rect *box);

int collisionStatic_CircRectInt(int circX, int circY, int *radius, int rectX, int rectY, int rectW, int rectH);

int collisionStatic_Rectangle(SDL_Rect *A, SDL_Rect *B);

int collisionStatic_Rectangle2(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

SDL_Rect collisionFit_Rect(SDL_Rect *A, SDL_Rect *B);

Vector2D collision_Point_PerpLine(Vector2D *startA, Vector2D *endA, Vector2D *point);
int collision_Point_IsInRange(float startX, float endX, float pointX);

#endif
