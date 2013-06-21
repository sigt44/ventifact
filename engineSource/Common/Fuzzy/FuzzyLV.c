#include "FuzzyLV.h"
#include "FuzzySet.h"
#include "../../Memory.h"

void fuzzyLV_Setup(Fuzzy_LV *flv, float min, float max, int totalSets, ...)
{
    va_list args;
    int x = 0;

    flv->desirability = 0.0f;

    flv->totalSets = totalSets;

    flv->min = min;
    flv->max = max;

    /*Create the required amount of fuzzy set pointers*/
    flv->sets = (Fuzzy_Set **)mem_Malloc(totalSets * sizeof(Fuzzy_Set **), __LINE__, __FILE__);

    va_start(args, totalSets);

    for(x = 0; x < totalSets; x++)
    {
        flv->sets[x] = va_arg(args, Fuzzy_Set *);

        if(flv->sets[x]->min < flv->min || flv->sets[x]->max > flv->max)
        {
            printf("Warning: [%d]Fuzzy set outside range of FLV. (%.2f, %.2f) (%.2f, %.2f)\n",flv->sets[x]->ID, flv->sets[x]->min, flv->sets[x]->max, flv->min, flv->max);
        }
    }

    va_end(args);

    return;
}

void fuzzyLV_Clean(Fuzzy_LV *flv)
{
    int x = 0;

    for(x = 0; x < flv->totalSets; x++)
    {
        fuzzySet_Clean(flv->sets[x]);
        mem_Free(flv->sets[x]);
    }

    mem_Free(flv->sets);

    return;
}

Fuzzy_Set *fuzzyLV_GetID(Fuzzy_LV *flv, int ID)
{
    int x = 0;

    for(x = 0; x < flv->totalSets; x++)
    {
        if(flv->sets[x]->ID == ID)
            return flv->sets[x];
    }

    printf("Warning no ID(%d) in FLV\n", ID);

    return NULL;
}

float fuzzyLV_Rule(Fuzzy_LV *flv, int ruleID, float position)
{
    Fuzzy_Set *ruleSet = NULL;
    ruleSet = fuzzyLV_GetID(flv, ruleID);

    if(ruleSet != NULL)
        return fuzzySet_GetDOM(ruleSet, position);
    else
        return 0.0f;
}

void fuzzyLV_SetDOM(Fuzzy_LV *flv, int ID, float DOM)
{
    Fuzzy_Set *set = fuzzyLV_GetID(flv, ID);

    if(set != NULL)
    {
        set->DOM = DOM;
    }

    return;
}

float fuzzyLV_Defuzzify(Fuzzy_LV *flv)
{
    int x = 0;

    float crisp = 0.0f;
    float numerator = 0.0f;
    float denominator = 0.0f;

    /*Calculate the average of maxima of all the sets*/
    for(x = 0; x < flv->totalSets; x++)
    {
        //printf("SetID(%d) DOM = %.6f PEAK = %.6f\n", flv->sets[x]->ID, flv->sets[x]->DOM, flv->sets[x]->middlePeakMembership);

        /*Numerator consists of the sum of (maximum value of the set * the confidence value of the set)*/
        numerator = numerator + (flv->sets[x]->DOM * flv->sets[x]->middlePeakMembership);

        /*Denominator consists of the sum of the confidence value of the sets*/
        denominator = denominator + flv->sets[x]->DOM;
    }

    if(denominator != 0.0f)
        flv->desirability = numerator/denominator;
    else
        flv->desirability = 0.0f;

    return flv->desirability;
}

void fuzzyLV_Draw(Fuzzy_LV *flv, int x, int y, SDL_Surface *dest)
{
    int i = 0;

    /*Find the shape type of each fuzzy set and draw it, making the fuzzy manifold*/
    for(i = 0; i < flv->totalSets; i++)
    {
        switch(flv->sets[i]->shapeType)
        {
            case FS_TRIANGLE:

            Fs_Triangle_Draw(flv->sets[i]->shapeData, x, y, dest);

            break;

            case FS_SHOULDER:

            Fs_Shoulder_Draw(flv->sets[i]->shapeData, x, y, dest);

            break;

            default:

            printf("Error unknown shape type to draw (%d)\n", flv->sets[i]->shapeType);

            break;
        }
    }

    return;
}
