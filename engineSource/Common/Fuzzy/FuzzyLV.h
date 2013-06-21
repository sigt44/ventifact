#ifndef FUZZYLV_H
#define FUZZYLV_H

#include <stdarg.h>

#include "FuzzySet.h"

enum
{
    FZ_DESIREPOOR = 0,
    FZ_DESIREAVERAGE = 1,
    FZ_DESIREGOOD = 2
} FUZZY_DESIRABILITY;

typedef struct Fuzzy_LV
{
    int totalSets;

    float min;
    float max;

    Fuzzy_Set **sets;

    float desirability;

} Fuzzy_LV;

void fuzzyLV_Setup(Fuzzy_LV *flv, float min, float max, int totalSets, ...);

void fuzzyLV_Clean(Fuzzy_LV *flv);

Fuzzy_Set *fuzzyLV_GetID(Fuzzy_LV *flv, int ID);

void fuzzyLV_SetDOM(Fuzzy_LV *flv, int ID, float DOM);

float fuzzyLV_Rule(Fuzzy_LV *flv, int ruleID, float position);

float fuzzyLV_Defuzzify(Fuzzy_LV *flv);

void fuzzyLV_Draw(Fuzzy_LV *flv, int x, int y, SDL_Surface *dest);



#endif
