#include "Sides.h"

#include <stdio.h>

#include "Common/List.h"

#include "../Ai/Ai_State.h"
#include "../Ai/Ai_Side.h"
#include "VentGame.h"

void vSide_Setup(Vent_Side *s, int startCredits, unsigned int team)
{
    int x = 0;

    snprintf(s->name, 32, "[Side %d]", team);

    s->lastFrameCredits = s->credits = startCredits;
    s->creditsSpent = 0;
    s->team = team;

    aiModule_Setup(&s->ai);

    s->ally = NULL;
    s->enemy = NULL;

    s->captures = 0;

    s->units = NULL;

    s->unitsNeedAi = NULL;

    s->buildings = NULL;
    s->citadels = NULL;

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        s->totalUnits[x] = 0;
        s->readyUnits[x] = 0;
        s->lostUnits[x] = 0;
        s->killedUnits[x] = 0;

        switch(x)
        {
            case UNIT_TANK:

            s->unitDesc[x].name = "Tank";
            s->unitDesc[x].price = 100;
            s->readyUnits[x] = 2;

            break;

            case UNIT_INFANTRY:

            s->unitDesc[x].name = "Infantry";
            s->unitDesc[x].price = 50;

            s->readyUnits[x] = 2;

            break;

            case UNIT_AIR:

            s->unitDesc[x].name = "Aircraft";
            s->unitDesc[x].price = 125;

            break;

            case UNIT_TOWER:

            s->unitDesc[x].name = "Tower";
            s->unitDesc[x].price = 250;

            break;

            default:

            s->unitDesc[x].name = "Unknown";
            s->unitDesc[x].price = 0;

            break;
        }

        s->unitAttributes[x] = noAttributes;

    }

    s->playerAttributes = noAttributes;

    for(x = 0; x < BUILDING_TOTAL; x++)
    {
        s->totalBuildings[x] = 0;
    }

    return;
}

void vSide_SetupAttributes(Vent_Side *s, int playerLvl, int tankLvl, int infantryLvl, int aircraftLvl, int towerLvl)
{
    int x = 0;
    int i = 0;
    int unitLvl[UNIT_ENDTYPE];

    unitLvl[UNIT_TANK] = tankLvl;
    unitLvl[UNIT_INFANTRY] = infantryLvl;
    unitLvl[UNIT_AIR] = aircraftLvl;
    unitLvl[UNIT_TOWER] = towerLvl;

    for(i = 0; i < playerLvl; i++)
    {
        //printf("player [%d]\n", i);
        /*Sum up the attributes to the players level*/
        vAttributes_Add(&s->playerAttributes, &playerAttributes[i]);
    }

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        for(i = 0; i < unitLvl[x]; i++)
        {
            //printf("unit [%d][%d]\n", x, i);
            vAttributes_Add(&s->unitAttributes[x], &unitAttributes[x][i]);
        }
    }

    return;
}

void vSide_SetupAi(Vent_Side *s, int aiType, void *ventGame)
{
    Vent_Game *game = ventGame;
    Ai_State *ai = NULL;

    s->wantedUnit = UNIT_TANK;

    switch(aiType)
    {
        case AI_DIFF_NONE:
        default:

        /*Purchase units*/
        ai = aiState_SidePurchaseUnit_Setup(aiState_Create(),
                AI_GLOBAL_STATE + 1,    /*group*/
                1,                      /*priority*/
                AI_COMPLETE_SINGLE,     /*complete type*/
                s,                      /*entity*/
                5000,                   /*update time*/
                game                     /*game*/
                );

        aiModule_PushState(&s->ai, ai);

        /*Give units ai*/
        ai = aiState_SideGiveAiUnit_Setup(aiState_Create(),
                AI_GLOBAL_STATE + 1,    /*group*/
                2,                      /*priority*/
                AI_COMPLETE_SINGLE,     /*complete type*/
                s,                      /*entity*/
                6000,                   /*update time*/
                game                     /*game*/
                );

        aiModule_PushState(&s->ai, ai);

        /*Buy buildings*/
        ai = aiState_SidePurchaseBuilding_Setup(aiState_Create(),
                AI_GLOBAL_STATE + 1,    /*group*/
                3,                      /*priority*/
                AI_COMPLETE_SINGLE,     /*complete type*/
                s,                      /*entity*/
                10000,                   /*update time*/
                game                     /*game*/
        );

        aiModule_PushState(&s->ai, ai);

        break;
    }
}

void vSide_SetAlly(Vent_Side *s, Vent_Side *a)
{
    if(a != NULL)
        a->ally = s;
    else if(s->ally != NULL)
        s->ally->ally = NULL;

    s->ally = a;

    return;
}

void vSide_SetEnemy(Vent_Side *s, Vent_Side *e)
{
    if(e != NULL)
        e->enemy = s;
    else if(e->enemy != NULL)
        s->enemy->enemy = NULL;

    s->enemy = e;

    return;
}

void vSide_Clean(Vent_Side *s)
{
    /*Clear up the ai state*/
    aiModule_Clean(&s->ai);

    list_Clear(&s->units);
    list_Clear(&s->unitsNeedAi);
    list_Clear(&s->buildings);

    return;
}
