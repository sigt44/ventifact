#include "FuzzyManager.h"

#include "../../Memory.h"

Fuzzy_Rule *fuzzyRule_Create(Fuzzy_LV *inputFLV, int setID, int outputSetID, float *inputValue)
{
    Fuzzy_Rule *rule = (Fuzzy_Rule *)mem_Malloc(sizeof(Fuzzy_Rule), __LINE__, __FILE__);

    rule->input = inputFLV;

    rule->inputValue = inputValue;

    rule->inputID = setID;

    rule->outputID = outputSetID;

    rule->outputValue = 0.0f;

    return rule;
}

float fuzzyRule_Calc(Fuzzy_Rule *rule)
{
    rule->outputValue = fuzzyLV_Rule(rule->input, rule->inputID, *rule->inputValue);

    return rule->outputValue;
}

void fuzzyRuleset_Setup(Fuzzy_Ruleset *rs, int ID, Fuzzy_LV *outcome, int totalRules, ...)
{
    va_list args;
    int x = 0;

    rs->ID = ID;

    rs->outcome = outcome;

    rs->fRuleList = NULL;

    va_start(args, totalRules);

    for(x = 0; x < totalRules; x++)
    {
        list_Push(&rs->fRuleList, va_arg(args, Fuzzy_Rule *), 0);
    }

    va_end(args);

    rs->outputValues = (Fuzzy_Desire *)mem_Malloc(rs->outcome->totalSets * sizeof(Fuzzy_Desire), __LINE__, __FILE__);

    for(x = 0; x < rs->outcome->totalSets; x++)
    {
        (rs->outputValues + x)->ID = rs->outcome->sets[x]->ID;
        (rs->outputValues + x)->desirability = 0.0f;
    }

    return;
}

float fuzzyRuleset_Calc(Fuzzy_Ruleset *rs)
{
    struct list *ruleList = rs->fRuleList;
    Fuzzy_Rule *rule = NULL;
    int x = 0;

    /*For all the rules in the ruleset*/
    while(ruleList != NULL)
    {
        rule = ruleList->data;

        /*Calculate the desirability*/
        fuzzyRule_Calc(rule);

        /*Go through the outcome FLV*/
        for(x = 0; x < rs->outcome->totalSets; x++)
        {
            /*If this rule that has been calculated matches the outcomes ID*/
            if((rs->outputValues + x)->ID == rule->outputID)
            {
                /*if((rs->outputValues + x)->desirability < rule->outputValue)
                {
                    (rs->outputValues + x)->desirability = rule->outputValue;
                }*/
                /*Add in the desirability of the rule to the final outcome*/
                (rs->outputValues + x)->desirability += rule->outputValue;

                /*So if the rule had output ID of LOW_PRIORITY, increase
                the desirability of LOW_PRIORITY in the outcome FLV*/
            }
        }

        ruleList = ruleList->next;
    }

    for(x = 0; x < rs->outcome->totalSets; x++)
    {
        fuzzyLV_SetDOM(rs->outcome, (rs->outputValues + x)->ID, (rs->outputValues + x)->desirability);

        /*printf("Setting up outcome: %d - %d --> %.2f\n", x, (rs->outputValues + x)->ID, (rs->outputValues + x)->desirability);*/
        (rs->outputValues + x)->desirability = 0.0f;
    }

    fuzzyLV_Defuzzify(rs->outcome);

    return rs->outcome->desirability;
}


void fuzzyRuleset_Clean(Fuzzy_Ruleset *rs)
{
    while(rs->fRuleList != NULL)
    {
        /*fuzzyRule_Clean(rs->fRuleList->data);*/

        mem_Free(rs->fRuleList->data);

        list_Pop(&rs->fRuleList);
    }

    mem_Free(rs->outputValues);

    return;
}
