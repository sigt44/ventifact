#ifndef FUZZYMANAGER_H
#define FUZZYMANAGER_H

#include "../List.h"
#include "FuzzyLV.h"

typedef struct Fuzzy_Desire
{
    int ID;
    float desirability;

} Fuzzy_Desire;

typedef struct Fuzzy_Ruleset
{
    int ID;

    Fuzzy_LV *outcome;

    Fuzzy_Desire *outputValues;

    struct list *fRuleList;

} Fuzzy_Ruleset;

typedef struct Fuzzy_Rule
{
    Fuzzy_LV *input;

    int inputID;
    float *inputValue;

    int outputID;
    float outputValue;
} Fuzzy_Rule;

Fuzzy_Rule *fuzzyRule_Create(Fuzzy_LV *inputFLV, int setID, int outputSetID, float *inputValue);

float fuzzyRule_Calc(Fuzzy_Rule *rule);

void fuzzyRuleset_Setup(Fuzzy_Ruleset *rs, int ID, Fuzzy_LV *outcome, int totalRules, ...);

float fuzzyRuleset_Calc(Fuzzy_Ruleset *rs);

void fuzzyRuleset_Clean(Fuzzy_Ruleset *rs);

#endif
