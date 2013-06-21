#include "FuzzySet.h"

#include "../../Kernel/Kernel.h"
#include "../../Graphics/Graphics.h"

Fs_Shoulder *Fs_Shoulder_Create(float left, float peak, float right, int direction)
{
    Fs_Shoulder *shoulder = (Fs_Shoulder *)mem_Malloc(sizeof(Fs_Shoulder), __LINE__, __FILE__);

    shoulder->left = left;
    shoulder->peak = peak;
    shoulder->right = right;
    shoulder->direction = direction;

    return shoulder;
}

float Fs_Shoulder_DOM(Fs_Shoulder *shoulder, float position)
{
    float DOM = 0.0f;

    switch(shoulder->direction)
    {
        /*Left shoulder*/
        case 0:

        /*Position is on the platue of the shoulder*/
        if(position >= shoulder->left && position <= shoulder->peak)
        {
            DOM = 1.0f;
        }
        /*Position is on the slant of the shoulder*/
        else if(position > shoulder->peak && position <= shoulder->right)
        {
            DOM = 1.0f - ((position - shoulder->left - shoulder->peak) / (shoulder->right - shoulder->peak));
        }

        /*else, position is outside of the boundary*/

        break;

        /*Right shoulder*/
        case 1:
        /*Position is on the platue of the shoulder*/
        if(position <= shoulder->right && position >= shoulder->peak)
        {
            DOM = 1.0f;
        }
        /*Position is on the slant of the shoulder*/
        else if(position < shoulder->peak && position >= shoulder->left)
        {
            DOM = (position - shoulder->left) / (shoulder->peak - shoulder->left);
        }

        /*else, position is outside of the boundary*/
        break;

        default:
        printf("Error invalid shoulder direction\n");
        break;
    }

    return DOM;
}

void Fs_Shoulder_Draw(Fs_Shoulder *shoulder, int x, int y, SDL_Surface *dest)
{
    switch(shoulder->direction)
    {
        /*Left shoulder*/
        case 0:

        draw_Line(x + (int)shoulder->left, y - 100, x + (int)shoulder->peak, y - 100, 1, 0, &colourBlack, dest);
        draw_Line(x + (int)shoulder->peak, y - 100, x + (int)shoulder->right, y, 1, 0, &colourBlack, dest);

        break;

        /*Right shoulder*/
        case 1:

        draw_Line(x + (int)shoulder->peak, y - 100, x + (int)shoulder->right, y - 100, 1, 0, &colourBlack, dest);
        draw_Line(x + (int)shoulder->left, y, x + (int)shoulder->peak, y - 100, 1, 0, &colourBlack, dest);

        break;
    }

    return;
}

Fs_Triangle *Fs_Triangle_Create(float left, float peak, float right)
{
    Fs_Triangle *triangle = (Fs_Triangle *)mem_Malloc(sizeof(Fs_Triangle), __LINE__, __FILE__);

    triangle->left = left;
    triangle->peak = peak;
    triangle->right = right;

    return triangle;
}

/*Obtain the degree of membership in a certain position*/
float Fs_Triangle_DOM(Fs_Triangle *triangle, float position)
{
    float DOM = 0.0f;

    /*Position is to the right of the peak and left of the max point*/
    if(position <= triangle->right && position > triangle->peak)
    {
        DOM = 1.0f + ((position - triangle->peak)/(triangle->peak - triangle->right));
    }
    else if(position >= triangle->left && position < triangle->peak) /*Right of the min point and left of the peak*/
    {
        DOM = ((position - triangle->left)/(triangle->peak - triangle->left));
    }
    else if(position == triangle->peak)
    {
        DOM = 1.0f;
    }

    return DOM;
}

void Fs_Triangle_Draw(Fs_Triangle *triangle, int x, int y, SDL_Surface *dest)
{
    draw_Triangle(x + (int)triangle->left, y, x + (int)triangle->peak, y - 100, x + (int)triangle->right, y, 1, &colourBlack, dest);

    return;
}

Fuzzy_Set *fuzzySet_Create(int ID, int shapeType, void *shapeData)
{
    Fuzzy_Set *newSet = (Fuzzy_Set *)mem_Malloc(sizeof(Fuzzy_Set), __LINE__, __FILE__);

    fuzzySet_Setup(newSet, ID, shapeType, shapeData);

    return newSet;
}

void fuzzySet_Setup(Fuzzy_Set *set, int ID, int shapeType, void *shapeData)
{
    Fs_Triangle *triangle = NULL;
    Fs_Shoulder *shoulder = NULL;

    set->ID = ID;

    set->min = 0.0f;
    set->max = 0.0f;

    set->shapeType = shapeType;

    set->shapeData = shapeData;

    switch(set->shapeType)
    {
        case FS_TRIANGLE:

        triangle = set->shapeData;

        set->min = triangle->left;
        set->max = triangle->right;

        set->middlePeakMembership = triangle->peak;

        break;

        case FS_SHOULDER:

        shoulder = set->shapeData;

        set->min = shoulder->left;
        set->max = shoulder->right;

        switch(shoulder->direction)
        {
            /*Left shoulder*/
            case 0:
            set->middlePeakMembership = (shoulder->left + shoulder->peak)/2.0f;
            break;

            /*Right shoulder*/
            case 1:
            set->middlePeakMembership = (shoulder->right + shoulder->peak)/2.0f;
            break;
        }

        break;

        default:

        printf("Error invalid fuzzy shape %d\n", set->shapeType);

        break;
    }

    set->DOM = -1;

    return;
}

void fuzzySet_Clean(Fuzzy_Set *set)
{
    mem_Free(set->shapeData);

    return;
}

void fuzzySet_Reset(Fuzzy_Set *set)
{
    set->DOM = -1.0f;

    return;
}

float fuzzySet_GetDOM(Fuzzy_Set *set, float position)
{
    switch(set->shapeType)
    {
        case FS_TRIANGLE:

        set->DOM = Fs_Triangle_DOM(set->shapeData, position);

        break;

        case FS_SHOULDER:

        set->DOM = Fs_Shoulder_DOM(set->shapeData, position);

        break;

        default:

        printf("Error invalid fuzzy shape %d\n", set->shapeType);

        break;
    }

    return set->DOM;
}
