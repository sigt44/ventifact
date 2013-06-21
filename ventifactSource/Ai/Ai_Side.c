#include "Ai_Side.h"

#include "AiScan.h"
#include "Ai_State.h"
#include "./Units/Ai_UnitAttack.h"
#include "./Units/Ai_UnitFunction.h"

/*
    Function: aiState_SidePurchaseUnit_Setup

    Description -
    This function sets up the variables for an ai state ment for a side/team.

    The behaviour of the state is to have the side choose a vehicle to purchase.
    close to it.

    6 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Side *entity - The side that this ai will control.
    int updateTime - The time between each check of new units to purchase.
    Camera_2D *game - The game structure.
*/
Ai_State *aiState_SidePurchaseUnit_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Side *entity,
    int updateTime,
    Vent_Game *game
    )
{
    AiAttributes_SidePurchaseUnit *aE = (AiAttributes_SidePurchaseUnit *)mem_Malloc(sizeof(AiAttributes_SidePurchaseUnit), __LINE__, __FILE__);

    aE->game = game;

    aE->updateTimer = timer_Setup(&aE->game->gTimer, 0, updateTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_SidePurchaseUnit_Init,
                  aiState_SidePurchaseUnit_Main,
                  aiState_SidePurchaseUnit_Exit
                  );
    return a;
}

/*
    Function: aiState_SidePurchaseUnit_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_SidePurchaseUnit_Init(Ai_State *a)
{
    Vent_Side *side = a->entity;
    AiAttributes_SidePurchaseUnit *aE = a->extraStructure;

    int x = 0;

    //printf("Side: %s init unit purchase\n", side->name);

    /*Set the FLV for how much money would be left if you were to buy the unit*/
    fuzzyLV_Setup(&aE->unitCost, 0, 1000000, 3,
              fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 500, 1500, 0)),
              fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(500, 1500, 5000)),
              fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(1500, 6000, 1000000, 1))
    );

   /*Set the FLV for how many units of type this side already has*/
   fuzzyLV_Setup(&aE->unitAmount, 0, 1000000, 3,
          fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 2, 5, 0)),
          fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(2, 5, 20)),
          fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(5, 20, 1000000, 1))
    );

    /*Set the FLV for how many of its units of type has been destroyed recently by the enemy*/
    fuzzyLV_Setup(&aE->unitDestroyed, 0, 1000000, 3,
      fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 2, 5, 0)),
      fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(2, 5, 7)),
      fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(5, 7, 1000000, 1))
    );

    /*Set the FLV for how many units of type the enemy side already has*/
    fuzzyLV_Setup(&aE->unitEnemyAmount, 0, 1000000, 3,
      fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 2, 5, 0)),
      fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(2, 5, 15)),
      fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(5, 15, 1000000, 1))
    );

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        /*Set the FLV for the desirability of purchase of unit[type]*/
        fuzzyLV_Setup(&aE->unitPurchase[x], 0.0f, 1.0f, 3,
          fuzzySet_Create(FS_DESIRE_LOW, FS_SHOULDER, Fs_Shoulder_Create(0.0f, 0.25f, 0.40f, 0)),
          fuzzySet_Create(FS_DESIRE_MEDIUM, FS_TRIANGLE, Fs_Triangle_Create(0.25f, 0.40f, 0.75f)),
          fuzzySet_Create(FS_DESIRE_HIGH, FS_SHOULDER, Fs_Shoulder_Create(0.40f, 0.75f, 1.00f, 1))
        );

        /*Setup the ruleset to choose the unit*/
        fuzzyRuleset_Setup(&aE->bestUnit[x], x, &aE->unitPurchase[x], 12,
                       fuzzyRule_Create(&aE->unitCost, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitCost),
                       fuzzyRule_Create(&aE->unitCost, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_UnitCost),
                       fuzzyRule_Create(&aE->unitCost, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitCost),

                       fuzzyRule_Create(&aE->unitAmount, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitAmount),
                       fuzzyRule_Create(&aE->unitAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_UnitAmount),
                       fuzzyRule_Create(&aE->unitAmount, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitAmount),

                       fuzzyRule_Create(&aE->unitDestroyed, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitDestroyed),
                       fuzzyRule_Create(&aE->unitDestroyed, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_UnitDestroyed),
                       fuzzyRule_Create(&aE->unitDestroyed, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitDestroyed),

                       fuzzyRule_Create(&aE->unitEnemyAmount, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitEnemyAmount),
                       fuzzyRule_Create(&aE->unitEnemyAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitEnemyAmount),
                       fuzzyRule_Create(&aE->unitEnemyAmount, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitEnemyAmount)
        );
    }



    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_SidePurchaseUnit_Main

    Description -
    This function performs the logic needed for the side ai to choose
    which is the best unit to buy next.

    1 argument:
    Ai_State *a - The ai state of a unit to update.
*/
void aiState_SidePurchaseUnit_Main(Ai_State *a)
{
    Vent_Side *side = a->entity;
    AiAttributes_SidePurchaseUnit *aE = a->extraStructure;

    Vent_Unit *unit = NULL;
    Vent_Building *citadel = NULL; /*Units are to be placed next to a citadel*/
    Vent_Target target;
    Vector2DInt placementPos = aE->game->level->header.startLoc[side->team];

    int x = 0;
    int buyAmount = 0;
    int citadelPriority = 0;

    int bestUnit = -1;
    float maxDesire = 0.0f;
    float desire = 0.0f;

    struct list *citadelList = NULL;

    timer_Calc(&aE->updateTimer);

    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        timer_Start(&aE->updateTimer);

        for(x = 0; x < UNIT_ENDTYPE; x++)
        {
            if(x != UNIT_TOWER)
            {
                aE->input_UnitAmount = (float)side->totalUnits[x];
                aE->input_UnitEnemyAmount = (float)side->enemy->totalUnits[x];
                aE->input_UnitCost = (float)side->credits - (float)side->unitDesc[x].price;
                aE->input_UnitDestroyed = (float)side->lostUnits[x];

                /*printf("Side units - %d\n", aE->input_UnitAmount);

                printf("\tLOW: %.2f\n", fuzzyLV_Rule(&aE->unitAmount, FS_LOW_AMOUNT, aE->input_UnitAmount));
                printf("\tMEDIUM: %.2f\n", fuzzyLV_Rule(&aE->unitAmount, FS_MEDIUM_AMOUNT, aE->input_UnitAmount));
                printf("\tHIGH: %.2f\n", fuzzyLV_Rule(&aE->unitAmount, FS_HIGH_AMOUNT, aE->input_UnitAmount));
                */

                desire = fuzzyRuleset_Calc(&aE->bestUnit[x]);

                //printf("Desire to buy %s is %.6f\n", side->unitDesc[x].name, desire);

                if(desire > maxDesire)
                {
                    bestUnit = x;
                    maxDesire = aE->bestUnit[x].outcome->desirability;
                }
            }
        }

        if(bestUnit != -1)
        {
            side->wantedUnit = bestUnit;
        }

        /*Make sure that there is enough credits to keep buying citadels*/
        citadelPriority = aE->game->totalCitadels - side->captures - side->enemy->captures;

        if(citadelPriority > 0)
        {
            side->credits -= citadelPriority * ventBuilding_Prices[BUILDING_CITADEL];
        }

        buyAmount = side->readyUnits[side->wantedUnit];

        /*Need to place a unit at a friendly citadel*/
        if(aE->game->playerUnit != NULL)
        {
            /*Make it random as to which citadel, but favor the closest*/
            if(rand() % 4 != 0 && side->citadels != NULL)
            {
                x = rand() % side->captures;

                citadelList = side->citadels;

                for(x=x; x > 0; x--)
                {
                    if(citadelList->next != NULL)
                        citadelList = citadelList->next;
                }

                citadel = citadelList->data;
            }
            else
            {
                target = vAi_GetBuilding_Closest(side->citadels,
                                 aE->game->playerUnit->iPosition.x,
                                 aE->game->playerUnit->iPosition.y,
                                 -1,
                                 BUILDING_CITADEL,
                                 1);

                citadel = target.entity;

                vAi_GetBuilding_Closest(NULL, 0, 0, 0, 0, -1); /*Clear up search*/
            }

            if(citadel != NULL)
            {
                placementPos.x = citadel->position.x;
                placementPos.y = citadel->position.y;
            }
        }

        /*First try and buy the most wanted unit*/
        for(x = 0; x < buyAmount; x++)
        {
            unit = vUnit_Buy(placementPos.x, placementPos.y, side->wantedUnit, side->team, 0, side, &aE->game->gTimer);

            if(unit != NULL)
            {
                vGame_AddUnit(aE->game, unit);

                depWatcher_AddBoth(&unit->depWatcher, &side->unitsNeedAi, 0); /*Unit now needs some ai*/

            }
        }

        /*Then try to buy the other types once*/
        for(x = 0; x < UNIT_ENDTYPE; x++)
        {
            if(side->readyUnits[x] > 0 && x != UNIT_TOWER)
            {
                unit = vUnit_Buy(placementPos.x, placementPos.y, x, side->team, 0, side, &aE->game->gTimer);

                if(unit != NULL)
                {
                    vGame_AddUnit(aE->game, unit);
                    depWatcher_AddBoth(&unit->depWatcher, &side->unitsNeedAi, 0);
                }
            }
        }

        if(citadelPriority > 0)
        {
            side->credits += citadelPriority * ventBuilding_Prices[BUILDING_CITADEL];
        }
    }


    return;
}

/*
    Function: aiState_SidePurchaseUnit_Exit

    Description -
    Sets the ai state as complete.
    Releases the memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state to exit
*/
void aiState_SidePurchaseUnit_Exit(Ai_State *a)
{
    AiAttributes_SidePurchaseUnit *aE = a->extraStructure;
    int x = 0;

    if(aiState_SetComplete(a) == 0)
        return;

    fuzzyLV_Clean(&aE->unitCost);
    fuzzyLV_Clean(&aE->unitAmount);
    fuzzyLV_Clean(&aE->unitDestroyed);
    fuzzyLV_Clean(&aE->unitEnemyAmount);

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        fuzzyLV_Clean(&aE->unitPurchase[x]);
        fuzzyRuleset_Clean(&aE->bestUnit[x]);
    }

    return;
}

/*
    Function: aiState_SideGiveAiUnit_Setup

    Description -
    This function sets up the variables for an ai state ment for a side/team.

    The behaviour of the state is to have the side choose which ai might be best to give
    a unit.

    6 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Side *entity - The side that this ai will control.
    int updateTime - The time between each check to give units a new ai.
    Camera_2D *game - The game structure.
*/
Ai_State *aiState_SideGiveAiUnit_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Side *entity,
    int updateTime,
    Vent_Game *game
    )
{
    AiAttributes_SideGiveAiUnit *aE = (AiAttributes_SideGiveAiUnit *)mem_Malloc(sizeof(AiAttributes_SideGiveAiUnit), __LINE__, __FILE__);

    aE->game = game;

    aE->updateTimer = timer_Setup(&aE->game->gTimer, 0, updateTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_SideGiveAiUnit_Init,
                  aiState_SideGiveAiUnit_Main,
                  aiState_SideGiveAiUnit_Exit
                  );
    return a;
}

/*
    Function: aiState_SideGiveAiUnit_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_SideGiveAiUnit_Init(Ai_State *a)
{
    Vent_Side *side = a->entity;
    AiAttributes_SideGiveAiUnit *aE = a->extraStructure;

    int x = 0;

    /*printf("Side: %s init unit ai give\n", side->name);*/

    /*Set the FLV for how many enemy units are located near one of its citadels*/
    fuzzyLV_Setup(&aE->unitCloseToCitadel, 0, 1000000, 3,
              fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(2, 3, 5, 0)),
              fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(3, 5, 10)),
              fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(5, 10, 1000000, 1))
    );

   /*Set the FLV for the difference between percentage of current ai and requested amount*/
   fuzzyLV_Setup(&aE->aiAmount, 0, 1000000, 3,
          fuzzySet_Create(FS_LOW_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(0.0f, 0.10f, 0.20f)),
          fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(0.10f, 0.20f, 0.40f)),
          fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0.20f, 0.40f, 1.0f, 1))
    );

    /*Set the FLV for if there is an empty citadel base on the map*/
    fuzzyLV_Setup(&aE->emptyCitadel, 0, 1000000, 1,
      fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(1, 1, 1000000, 1))
    );

    for(x = 0; x < UAI_TOTALAI; x++)
    {
        /*Setup the frequency tracking of this ai*/
        aE->totalAi[x] = 0;
        aE->totalJudgeAi[x] = 0;

        /*Set the FLV for the desirability of giving an ai type*/
        fuzzyLV_Setup(&aE->aiWeight[x], 0.0f, 1.0f, 3,
          fuzzySet_Create(FS_DESIRE_LOW, FS_SHOULDER, Fs_Shoulder_Create(0.0f, 0.25f, 0.50f, 0)),
          fuzzySet_Create(FS_DESIRE_MEDIUM, FS_TRIANGLE, Fs_Triangle_Create(0.25f, 0.50f, 0.75f)),
          fuzzySet_Create(FS_DESIRE_HIGH, FS_SHOULDER, Fs_Shoulder_Create(0.50f, 0.75f, 1.00f, 1))
        );

        /*Setup the ruleset for each ai*/
        switch(x)
        {
            default:

            printf("Error unknown ai to give unit (%d)\n", x);

            break;

            case UAI_ATTACKPLAYER:
            case UAI_ATTACKUNIT:

            fuzzyRuleset_Setup(&aE->bestAi[x], x, &aE->aiWeight[x], 7,
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitCloseToCitadel),
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_MEDIUM_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitCloseToCitadel),
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitCloseToCitadel),

                           fuzzyRule_Create(&aE->aiAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_AiAmount),
                           fuzzyRule_Create(&aE->aiAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_AiAmount),
                           fuzzyRule_Create(&aE->aiAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_AiAmount),

                           fuzzyRule_Create(&aE->emptyCitadel, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_EmptyCitadel)
            );

            break;

            case UAI_ATTACKBUILDING:

            fuzzyRuleset_Setup(&aE->bestAi[x], x, &aE->aiWeight[x], 7,
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitCloseToCitadel),
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_MEDIUM_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitCloseToCitadel),
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitCloseToCitadel),

                           fuzzyRule_Create(&aE->aiAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_AiAmount),
                           fuzzyRule_Create(&aE->aiAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_AiAmount),
                           fuzzyRule_Create(&aE->aiAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_AiAmount),

                           fuzzyRule_Create(&aE->emptyCitadel, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_EmptyCitadel)
            );

            break;

            case UAI_DEFENDAREA:

            fuzzyRuleset_Setup(&aE->bestAi[x], x, &aE->aiWeight[x], 7,
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_UnitCloseToCitadel),
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_UnitCloseToCitadel),
                           fuzzyRule_Create(&aE->unitCloseToCitadel, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_UnitCloseToCitadel),

                           fuzzyRule_Create(&aE->aiAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_AiAmount),
                           fuzzyRule_Create(&aE->aiAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_AiAmount),
                           fuzzyRule_Create(&aE->aiAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_AiAmount),

                           fuzzyRule_Create(&aE->emptyCitadel, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_EmptyCitadel)
            );

            break;

            case UAI_BUYCITADEL:

            fuzzyRuleset_Setup(&aE->bestAi[x], x, &aE->aiWeight[x], 1,
                           fuzzyRule_Create(&aE->emptyCitadel, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_EmptyCitadel)
            );

            break;
        }
    }

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}


/*
    Function: aiState_SideGiveAiUnit_Main

    Description -
    This function performs the logic needed for the ai state to move a unit
    away from other units close to it.

    1 argument:
    Ai_State *a - The ai state of a unit to update.
*/
void aiState_SideGiveAiUnit_Main(Ai_State *a)
{
    Vent_Side *side = a->entity;
    AiAttributes_SideGiveAiUnit *aE = a->extraStructure;

    Vent_Unit *unit = NULL;
    Vent_Unit *targetUnit = NULL;
    Vent_Building *citadel = NULL;
    Vent_Building *building = NULL;
    Vector2DInt position;

    int x = 0;
    int i = 0;

    int bestAi = -1;
    float maxDesire = 0.0f;
    float desire = -1.0f;

    float percentage[UAI_TOTALAI] = {0.40f, 0.25f, 0.25f, 0.1f, 0.0f};

    timer_Calc(&aE->updateTimer);

    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        timer_Start(&aE->updateTimer);

        /*Tweak the sides judgement of how many units are performing a certain ai.*/
        for(x = 0; x < UAI_TOTALAI; x++)
        {
            /*The reason this is done is that sometimes units will be destroyed too quickly
            and the ai will just keep giving a unit the same order instead of trying something
            different.*/
            if(rand() % 2 == 0)
            {
            }
            else if(aE->totalJudgeAi[x] < aE->totalAi[x])
            {
                aE->totalJudgeAi[x] ++;
            }
            else if(aE->totalJudgeAi[x] > aE->totalAi[x])
            {
                aE->totalJudgeAi[x] --;
            }
        }


        /*Find the amount of empty citadels*/
        aE->input_EmptyCitadel = (float)aE->game->totalCitadels - (float)side->captures - (float)side->enemy->captures;

        aE->input_UnitCloseToCitadel = 0;

        /*Go through all the units that need to be given an ai state*/
        while(side->unitsNeedAi != NULL)
        {
            unit = side->unitsNeedAi->data;

            maxDesire = 0.0f;
            bestAi = -1;

            //printf("Starting unit ai checks\n");

            for(x = 0; x < UAI_TOTALAI; x++)
            {
                aE->input_AiAmount = 0.0f;

                for(i = 0; i < UAI_TOTALAI; i++)
                    aE->input_AiAmount += (float)aE->totalJudgeAi[i];

                if(aE->input_AiAmount != 0.0f)
                    aE->input_AiAmount = percentage[x] - ((float)aE->totalJudgeAi[x]/aE->input_AiAmount);
                else
                    aE->input_AiAmount = percentage[x];

                //printf("[%d] ai percentage is %.4f\n", x, aE->input_AiAmount);

                desire = fuzzyRuleset_Calc(&aE->bestAi[x]);

                //printf("[%d]Desire to give ai is %.6f\n", x, desire);

                if(desire > maxDesire)
                {
                    bestAi = x;
                    maxDesire = aE->bestAi[x].outcome->desirability;
                }
            }

            switch(bestAi)
            {
                default:

                printf("Error unknown ai to give unit (%d)\n", bestAi);

                break;

                case UAI_ATTACKPLAYER:

                if(aE->game->playerUnit == NULL || aE->game->playerUnit->team == side->team)
                {
                    if(side->enemy->units != NULL)
                        targetUnit = side->enemy->units->data;
                }
                else
                    targetUnit = aE->game->playerUnit;

                vUnit_GiveAi(unit,
                    aiState_UnitAttackUnit_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 1,          /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_GROUP,      /*complete type*/
                    unit,                      /*entity*/
                    aE->game,               /*game*/
                    targetUnit    /*player unit*/
                    )
                );

                break;

                case UAI_ATTACKUNIT:

                if(side->enemy->units != NULL)
                {
                    vUnit_GiveAi(unit,
                        aiState_UnitAttackUnit_Setup(aiState_Create(),
                        AI_GLOBAL_STATE + 1,          /*group*/
                        1,                      /*priority*/
                        AI_COMPLETE_GROUP,      /*complete type*/
                        unit,                      /*entity*/
                        aE->game,               /*game*/
                        side->enemy->units->data    /*unit*/
                        )
                    );
                }

                break;

                case UAI_ATTACKBUILDING:

                vUnit_GiveAi(unit,aiState_UnitAttackBuilding_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 1,    /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_GROUP,     /*complete type*/
                    unit,                   /*entity*/
                    aE->game                 /*game*/
                    )
                );


                break;

                case UAI_DEFENDAREA:

                position = aE->game->level->header.startLoc[side->team];

                if(side->citadels != NULL)
                {
                    citadel = side->citadels->data;
                    position.x = citadel->middlePosition.x;
                    position.y = citadel->middlePosition.y;
                }

                vUnit_GiveAi(unit,
                    aiState_UnitAttackArea_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 1,          /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_GROUP,      /*complete type*/
                    unit,                      /*entity*/
                    aE->game,             /*game*/
                    position.x,                    /*x-axis origin of area*/
                    position.y,                    /*y-axis origin of area*/
                    100*100,                /*range of area (squared)*/
                    120000                       /*max time to wait at area*/
                    )
                );

                break;

                case UAI_BUYCITADEL:

                vUnit_GiveAi(unit,
                    aiState_UnitBuyCitadel_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 1,          /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_GROUP,      /*complete type*/
                    unit,                      /*entity*/
                    1,                      /*max attempts at buying*/
                    aE->game->level,        /*game level*/
                    aE->game              /*game*/
                    )
                );

                break;
            }

            /*If a chosen ai has been found, add in another state to report when it has finished*/
            if(bestAi != -1)
            {
                if(unit->trackStat == 1)
                {
                    /*Give the unit a tracking state so the side will know when it needs another ai state*/
                    vUnit_GiveAi(unit,
                        aiState_UnitReportAi_Setup(aiState_Create(),
                        AI_GLOBAL_STATE + 1,    /*group*/
                        5,                  /*priority*/
                        AI_COMPLETE_SINGLE, /*complete type*/
                        unit,                  /*entity*/
                        &aE->totalAi[bestAi],         /*ai type tracking*/
                        side,                /*side of the unit*/
                        1                   /*wait for AI_COMPLETE_GROUP before exiting*/
                        )
                    );

                    aE->totalJudgeAi[bestAi]++;
                    aE->totalAi[bestAi]++;
                }
                else
                {
                    /*Give the unit a tracking state so the side will know when it needs another ai state*/
                    /*However dont record any completion/update of this state, so it wont affect the fuzzy logic*/
                    vUnit_GiveAi(unit,
                        aiState_UnitReportAi_Setup(aiState_Create(),
                        AI_GLOBAL_STATE + 1,    /*group*/
                        5,                  /*priority*/
                        AI_COMPLETE_SINGLE, /*complete type*/
                        unit,                  /*entity*/
                        NULL,         /*ai type tracking*/
                        side,                /*side of the unit*/
                        1               /*wait for AI_COMPLETE_GROUP before exiting*/
                        )
                    );
                }
            }

            list_Pop(&side->unitsNeedAi);
            depWatcher_Remove(&unit->depWatcher, &side->unitsNeedAi);
        }
    }

    return;
}

/*
    Function: aiState_SideGiveAiUnit_Exit

    Description -
    Sets the ai state as complete.
    Releases the memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state to exit
*/
void aiState_SideGiveAiUnit_Exit(Ai_State *a)
{
    AiAttributes_SideGiveAiUnit *aE = a->extraStructure;
    int x = 0;

    if(aiState_SetComplete(a) == 0)
        return;

    fuzzyLV_Clean(&aE->unitCloseToCitadel);
    fuzzyLV_Clean(&aE->aiAmount);
    fuzzyLV_Clean(&aE->emptyCitadel);

    for(x = 0; x < UAI_TOTALAI; x++)
    {
        fuzzyLV_Clean(&aE->aiWeight[x]);
        fuzzyRuleset_Clean(&aE->bestAi[x]);
    }

    return;
}

Ai_State *aiState_SidePurchaseBuilding_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Side *entity,
    int updateTime,
    Vent_Game *game
    )
{
    AiAttributes_SidePurchaseBuilding *aE = (AiAttributes_SidePurchaseBuilding *)mem_Malloc(sizeof(AiAttributes_SidePurchaseBuilding), __LINE__, __FILE__);

    aE->game = game;

    aE->updateTimer = timer_Setup(&aE->game->gTimer, 0, updateTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_SidePurchaseBuilding_Init,
                  aiState_SidePurchaseBuilding_Main,
                  aiState_SidePurchaseBuilding_Exit
                  );
    return a;
}

/*
    Function: aiState_SidePurchaseBuilding_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_SidePurchaseBuilding_Init(Ai_State *a)
{
    Vent_Side *side = a->entity;
    AiAttributes_SidePurchaseBuilding *aE = a->extraStructure;

    int x = 0;

    /*printf("Side: %s init building purchase\n", side->name);*/

    /*Set the FLV for which is the best unit to buy*/
    fuzzyLV_Setup(&aE->unitChosen, 0, UNIT_AIR, 3,
              fuzzySet_Create(UNIT_TANK, FS_TRIANGLE, Fs_Triangle_Create(UNIT_TANK, UNIT_TANK, UNIT_TANK)),
              fuzzySet_Create(UNIT_INFANTRY, FS_TRIANGLE, Fs_Triangle_Create(UNIT_INFANTRY, UNIT_INFANTRY, UNIT_INFANTRY)),
              fuzzySet_Create(UNIT_AIR, FS_TRIANGLE,Fs_Triangle_Create(UNIT_AIR, UNIT_AIR, UNIT_AIR))
    );

   /*Set the FLV for how much credits the side has*/
   fuzzyLV_Setup(&aE->creditAmount, 0, 1000000, 3,
          fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 1000, 5000, 0)),
          fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(1000, 5000, 10000)),
          fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(5000, 10000, 1000000, 1))
    );

    /*Set the FLV for how many power stations the side has*/
    fuzzyLV_Setup(&aE->powerStations, 0, 1000000, 1,
      fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 1, 2, 0))
    );

    /*Set the FLV for the difference in building amounts*/
    fuzzyLV_Setup(&aE->buildingDifference, 0, 1000000, 3,
      fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 1, 2, 0)),
      fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(1, 2, 3)),
      fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(2, 3, 1000000, 1))
    );

    for(x = 0; x < BUILDING_TOTAL-1; x++)
    {
        /*Set the FLV for the desirability of purchase of building[type]*/
        fuzzyLV_Setup(&aE->buildingPurchase[x], 0.0f, 1.0f, 3,
          fuzzySet_Create(FS_DESIRE_LOW, FS_SHOULDER, Fs_Shoulder_Create(0.0f, 0.25f, 0.50f, 0)),
          fuzzySet_Create(FS_DESIRE_MEDIUM, FS_TRIANGLE, Fs_Triangle_Create(0.25f, 0.50f, 0.75f)),
          fuzzySet_Create(FS_DESIRE_HIGH, FS_SHOULDER, Fs_Shoulder_Create(0.50f, 0.75f, 1.00f, 1))
        );

        /*Setup the ruleset to choose the building*/

        switch(x + 1) /*Ignoring citadel building*/
        {
            default:

            printf("Error unknown building type for ai - %d\n", x);

            break;

            case BUILDING_TANK:

            fuzzyRuleset_Setup(&aE->bestBuilding[x], x, &aE->buildingPurchase[x], 8,
                               fuzzyRule_Create(&aE->unitChosen, UNIT_TANK, FS_DESIRE_HIGH, &aE->input_UnitChosen),

                               fuzzyRule_Create(&aE->creditAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_CreditAmount),

                               fuzzyRule_Create(&aE->powerStations, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_PowerStations),

                               fuzzyRule_Create(&aE->buildingDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_BuildingDifference),
                               fuzzyRule_Create(&aE->buildingDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_BuildingDifference),
                               fuzzyRule_Create(&aE->buildingDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_BuildingDifference)
            );

            break;

            case BUILDING_INFANTRY:

            fuzzyRuleset_Setup(&aE->bestBuilding[x], x, &aE->buildingPurchase[x], 8,
                               fuzzyRule_Create(&aE->unitChosen, UNIT_INFANTRY, FS_DESIRE_HIGH, &aE->input_UnitChosen),

                               fuzzyRule_Create(&aE->creditAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_CreditAmount),

                               fuzzyRule_Create(&aE->powerStations, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_PowerStations),

                               fuzzyRule_Create(&aE->buildingDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_BuildingDifference),
                               fuzzyRule_Create(&aE->buildingDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_BuildingDifference),
                               fuzzyRule_Create(&aE->buildingDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_BuildingDifference)
            );

            break;

            case BUILDING_AIR:

            fuzzyRuleset_Setup(&aE->bestBuilding[x], x, &aE->buildingPurchase[x], 8,
                               fuzzyRule_Create(&aE->unitChosen, UNIT_AIR, FS_DESIRE_HIGH, &aE->input_UnitChosen),

                               fuzzyRule_Create(&aE->creditAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_CreditAmount),

                               fuzzyRule_Create(&aE->powerStations, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_PowerStations),

                               fuzzyRule_Create(&aE->buildingDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_BuildingDifference),
                               fuzzyRule_Create(&aE->buildingDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_BuildingDifference),
                               fuzzyRule_Create(&aE->buildingDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_BuildingDifference)
            );

            break;

            case BUILDING_POWER:

            fuzzyRuleset_Setup(&aE->bestBuilding[x], x, &aE->buildingPurchase[x], 4,
                               fuzzyRule_Create(&aE->creditAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_LOW, &aE->input_CreditAmount),
                               fuzzyRule_Create(&aE->creditAmount, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_CreditAmount),

                               fuzzyRule_Create(&aE->powerStations, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_PowerStations)
            );

            break;
        }
    }

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_SidePurchaseBuilding_Main

    Description -
    This function performs the logic needed for the side ai to choose
    which is the best building to buy next.

    1 argument:
    Ai_State *a - The ai state of a unit to update.
*/
void aiState_SidePurchaseBuilding_Main(Ai_State *a)
{
    Vent_Side *side = a->entity;
    AiAttributes_SidePurchaseBuilding *aE = a->extraStructure;

    struct list *citadelList = NULL;
    Vent_Tile *buildTile = NULL;

    int x = 0;
    int i = 0;

    const int maxBuild = 2;
    int citadelPriority = 0;
    float maxBuildings = 0.0f;

    int bestBuilding = -1;

    float maxDesire = 0.0f;
    float desire = 0.0f;

    timer_Calc(&aE->updateTimer);

    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        timer_Start(&aE->updateTimer);

        citadelList = side->citadels;

        /*Make sure that there are enough credits to keep buying citadels*/
        citadelPriority = aE->game->totalCitadels - side->captures - side->enemy->captures;

        if(citadelPriority > 0)
        {
            side->credits -= citadelPriority * ventBuilding_Prices[BUILDING_CITADEL];
        }


        /*For all the citadels in the side*/
        while(citadelList != NULL)
        {
            /*Get the empty building tiles*/
            buildTile = vBuilding_GetFreeTile(citadelList->data);

            for(i = 0; i < maxBuild && buildTile != NULL; i++)
            {
                /*Input the correct values*/
                aE->input_UnitChosen = (float)side->wantedUnit;
                aE->input_CreditAmount = (float)side->credits;
                aE->input_PowerStations = (float)side->totalBuildings[BUILDING_POWER];

                for(x = 0; x < BUILDING_TOTAL-1; x++)
                {
                    if(x != BUILDING_POWER && x != BUILDING_CITADEL)
                    {
                        if(maxBuildings < (float)side->totalBuildings[x])
                            maxBuildings = (float)side->totalBuildings[x];
                    }
                }

                /*For all buildings except the citadel*/
                for(x = 0; x < BUILDING_TOTAL-1; x++)
                {
                    aE->input_BuildingDifference = maxBuildings - (float)side->totalBuildings[x+1];

                    desire = fuzzyRuleset_Calc(&aE->bestBuilding[x]);

                    if(side->team == 0)
                    {
                    //printf("Desire to buy %s is %.6f\n", ventBuilding_Names[x+1],  desire);
                    //printf("\tBuild diff = %.2f\n\tChosen unit = %.2f\n", aE->input_BuildingDifference, aE->input_UnitChosen);
                    }

                    if(desire > maxDesire)
                    {
                        bestBuilding = x + 1;
                        maxDesire = aE->bestBuilding[x].outcome->desirability;
                    }
                }

                if(bestBuilding != -1)
                {
                    if(vBuilding_CheckBuy(bestBuilding, side->team, aE->game, buildTile) == 1)
                    {
                        vBuilding_Build(side->team, bestBuilding, buildTile, aE->game);
                    }

                    bestBuilding = -1;
                    maxDesire = 0.0f;
                }

                buildTile = vBuilding_GetFreeTile(citadelList->data);
            }

            citadelList = citadelList->next;
        }

        if(citadelPriority > 0)
        {
            side->credits += citadelPriority * ventBuilding_Prices[BUILDING_CITADEL];
        }

    }

    return;
}

/*
    Function: aiState_SidePurchaseBuilding_Exit

    Description -
    Sets the ai state as complete.
    Releases the memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state to exit
*/
void aiState_SidePurchaseBuilding_Exit(Ai_State *a)
{
    AiAttributes_SidePurchaseBuilding *aE = a->extraStructure;
    int x = 0;

    if(aiState_SetComplete(a) == 0)
        return;

    fuzzyLV_Clean(&aE->unitChosen);
    fuzzyLV_Clean(&aE->creditAmount);
    fuzzyLV_Clean(&aE->powerStations);
    fuzzyLV_Clean(&aE->buildingDifference);

    for(x = 0; x < BUILDING_TOTAL-1; x++)
    {
        fuzzyLV_Clean(&aE->buildingPurchase[x]);
        fuzzyRuleset_Clean(&aE->bestBuilding[x]);
    }

    return;
}
