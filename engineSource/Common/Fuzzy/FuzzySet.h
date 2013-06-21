#ifndef FUZZYSET_H
#define FUZZYSET_H

#include <stdio.h>
#include <SDL/SDL.h>

enum FUZZYSHAPE_TYPES
{
    FS_TRIANGLE = 0,
    FS_SHOULDER = 1,
    FS_TRAPEZIUM = 2,
    FS_BELL = 3,
    FS_POINT = 4
};

typedef struct fs_Triangle
{
    float left; /*Left-most point of the triangle*/
    float peak; /*Tip of triangle*/
    float right; /*Right-most point of the triangle*/

} Fs_Triangle;

typedef struct fs_Shoulder
{
    float left; /*Left-most point of the shoulder*/
    float peak; /*Point at which shoulder starts its peak*/
    float right; /*Right-most point of the shoulder*/

    int direction; /*Whether the shoulder is left or right, 0 or 1*/

} Fs_Shoulder;

typedef struct Fuzzy_Set
{
    int ID; /*Used to identify the set*/

    int shapeType; /*The shape of the membership functions*/
    void *shapeData;

    float min;
    float max;

    float middlePeakMembership;

    float DOM; /*Degree of membership*/
} Fuzzy_Set;

Fs_Shoulder *Fs_Shoulder_Create(float left, float peak, float right, int direction);

float Fs_Shoulder_DOM(Fs_Shoulder *shoulder, float position);

void Fs_Shoulder_Draw(Fs_Shoulder *shoulder, int x, int y, SDL_Surface *dest);

Fs_Triangle *Fs_Triangle_Create(float left, float peak, float right);

float Fs_Triangle_DOM(Fs_Triangle *triangle, float position);

void Fs_Triangle_Draw(Fs_Triangle *triangle, int x, int y, SDL_Surface *dest);

Fuzzy_Set *fuzzySet_Create(int ID, int shapeType, void *shapeData);

void fuzzySet_Setup(Fuzzy_Set *set, int ID, int shapeType, void *shapeData);

void fuzzySet_Clean(Fuzzy_Set *set);

void fuzzySet_Reset(Fuzzy_Set *set);

float fuzzySet_GetDOM(Fuzzy_Set *set, float position);

#endif
