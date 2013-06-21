#include "Ai_UnitFunction.h"

#include <math.h>

#include "../AiScan.h"
#include "../Ai_Steering.h"
#include "Ai_UnitAttack.h"
#include "Ai_UnitMove.h"
#include "../../Level/Tiles.h"
#include "../../Buildings/BuildingBar.h"
#include "../../Target.h"

#include "Maths.h"
#include "Graphics/Graphics.h"

/*
    Function: aiState_UnitObtainTargets_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    It will search for game entities around the unit and add them to an array.
    These entities are what the unit can see and so can be used to find a target for attacking.

    7 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int range - The maximum an entity can be away from the unit. (the units view distance)
    int update - The base update time for checking new targets.
    Vent_Game *game - Current game structure.
*/
Ai_State *aiState_UnitObtainTargets_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int range,
    int updateTime,
    Vent_Game *game
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitObtainTargets_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitObtainTargets_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitObtainTargets_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitObtainTargets *extraStructure = (AiAttributes_UnitObtainTargets *)mem_Malloc(sizeof(AiAttributes_UnitObtainTargets), __LINE__, __FILE__);

    extraStructure->range = range;
    extraStructure->game = game;

    extraStructure->updateTimer = timer_Setup(&extraStructure->game->gTimer, 0, updateTime, 0);

    /*Setup the state*/
    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  extraStructure,
                  init,
                  mainLoop,
                  exit
                  );
    return a;
}

void aiState_UnitObtainTargets_Init(Ai_State *a)
{
    const int baseUpdateTime = 0; /*Time in milliseconds*/
    const int intervalTime = 100; /*Units will update at intervals of this amount*/
    const int totalIntervals = 10; /*How many intervals there are*/
    static int rotateTimeDelay = 0; /*Changes by +1 on each call to this function, seperates the units
    so that they are not updated on the same frame*/

    AiAttributes_UnitObtainTargets *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;
    int x = 0;
    int updateTime = 0;

    //printf("%s: searching for targets within %d\n", unit->name, aE->range);

    aE->rangeSqr = aE->range * aE->range;

    /*Make sure all units are not updated the same time at every frame*/
    updateTime = baseUpdateTime + (rotateTimeDelay * intervalTime);
    rotateTimeDelay = (rotateTimeDelay + 1) % totalIntervals;

    aE->updateTimer.end_Time += updateTime;

    timer_Start(&aE->updateTimer);

    vTarget_SetupDefaultsArray(&unit->targetEnemy[0], UNIT_MAX_TARGETS);
    vTarget_SetupDefaultsArray(&unit->targetAlly[0], UNIT_MAX_TARGETS);

    aE->targetPriority = rand() % 10;

    if(aE->targetPriority < 6)
    {
        aE->targetPriority = ENTITY_UNIT;
    }
    else if(aE->targetPriority == 9)
    {
        aE->targetPriority = ENTITY_BUILDING;
    }
    else
    {
        aE->targetPriority = ENTITY_TILE;
    }


    /*Move on to the main loop*/
    aiState_Init(a);

    return;
}

int aiState_UnitObtainTargets_Units(struct list *units, Vent_Unit *unit, Vent_Target *targetArray, int maxRangeSqr, int maxTargets)
{
    Vent_Target target;
    int x = 0;
    int index = maxTargets;

    if(maxTargets >= UNIT_MAX_TARGETS)
        return 0;

    target = vAi_GetUnit_Closest(units,
                            unit->iPosition.x,
                            unit->iPosition.y,
                            maxRangeSqr,
                            1);

    while(index < UNIT_MAX_TARGETS)
    {
        if(target.entity != NULL)
        {
            if(target.entity != unit)
            {
                targetArray[index] = target;
                vTarget_SetLink(&targetArray[index], 1);
                index++;
                x++;
            }
        }
        else
        {
            break;
        }

        target = vAi_GetUnit_Closest(units,
                    unit->iPosition.x,
                    unit->iPosition.y,
                    maxRangeSqr,
                    0);
    }

    return x;
}

int aiState_UnitObtainTargets_Buildings(struct list *buildings, Vent_Unit *unit, Vent_Target *targetArray, int maxRangeSqr, int maxTargets)
{
    Vent_Target target;
    int x = 0;
    int index = maxTargets;

    if(maxTargets >= UNIT_MAX_TARGETS)
        return 0;

    /*Check citadel buildings first*/
    target = vAi_GetBuilding_Closest(buildings,
                        unit->iPosition.x,
                        unit->iPosition.y,
                        maxRangeSqr,
                        BUILDING_CITADEL,
                        1);

    if(target.entity == NULL)
    {
        /*No citadel in range so check for other buildings*/
        target = vAi_GetBuilding_Closest(buildings,
                            unit->iPosition.x,
                            unit->iPosition.y,
                            maxRangeSqr,
                            -1,
                            1);
    }

    while(index < UNIT_MAX_TARGETS)
    {
        if(target.entity != NULL)
        {
            targetArray[index] = target;
            vTarget_SetLink(&targetArray[index], 1);
            index++;
            x++;
        }
        else
        {
            break;
        }

        target = vAi_GetBuilding_Closest(buildings,
                unit->iPosition.x,
                unit->iPosition.y,
                maxRangeSqr,
                -1,
                0);
    }

    return x;
}

int aiState_UnitObtainTargets_Tiles(struct list *tiles, int tileTeam, Vent_Unit *unit, Vent_Target *targetArray, int maxRangeSqr, int maxTargets)
{
    Vent_Target target;
    Vent_Tile *tileTarget = NULL;
    int index = maxTargets;

    int x = 0;

    if(maxTargets >= UNIT_MAX_TARGETS)
        return 0;

    target = vAi_GetTile_Closest(tiles,
                                unit->iPosition.x,
                                unit->iPosition.y,
                                maxRangeSqr,
                                -1,
                                1);

    while(index < UNIT_MAX_TARGETS)
    {
        tileTarget = target.entity;

        if(target.entity == NULL)
        {
            break;
        }
        else if(tileTarget->destroyed == 0 && tileTarget->base.team == tileTeam)
        {
            if(tileTeam == unit->team)
            {
            //vTarget_Report(&targetArray[index]);
            }
            targetArray[index] = target;
            if(tileTeam == unit->team)
            {
            //vTarget_Report(&unit->targetAlly[index]);
            //printf("(%s): Adding target tile(%p) %d %d team %d, index %d\n", unit->name, tileTarget, tileTarget->base.position.x, tileTarget->base.position.y, tileTarget->base.team, index);
            }
            vTarget_SetLink(&targetArray[index], 1);
            x++;
            index++;
        }
        break;

        target = vAi_GetTile_Closest(tiles,
                        unit->iPosition.x,
                        unit->iPosition.y,
                        maxRangeSqr,
                        -1,
                        0);
    }

    return x;
}

void aiState_UnitObtainTargets_Main(Ai_State *a)
{
    AiAttributes_UnitObtainTargets *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    Vent_Side *enemySide = aE->game->side[unit->team].enemy;
    Vent_Side *ownSide = &aE->game->side[unit->team];

    int totalTargets = 0;
    int targetPriority = aE->targetPriority;
    int x = 0;

    timer_Calc(&aE->updateTimer);

    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        /*Find enemy entities*/

        /*Clean up the old targets, remove links to their entity*/
        vTarget_ClearArray(&unit->targetEnemy[0], UNIT_MAX_TARGETS);

        totalTargets = 0;

        for(x = 0; x < 3; x++)
        {
            switch(targetPriority)
            {
                default:
                case ENTITY_UNIT:
                /*Check for units*/
                totalTargets += aiState_UnitObtainTargets_Units(enemySide->units, unit, &unit->targetEnemy[0], aE->rangeSqr, totalTargets);

                if(aE->targetPriority != ENTITY_BUILDING)
                    targetPriority = ENTITY_BUILDING;
                else
                    targetPriority = ENTITY_TILE;

                break;

                case ENTITY_BUILDING:
                /*Check for buildings, citadel first*/
                totalTargets += aiState_UnitObtainTargets_Buildings(enemySide->buildings, unit, &unit->targetEnemy[0], aE->rangeSqr, totalTargets);

                if(aE->targetPriority != ENTITY_UNIT)
                    targetPriority = ENTITY_UNIT;
                else
                    targetPriority = ENTITY_TILE;

                break;

                case ENTITY_TILE:
                /*Check for important tiles*/
                totalTargets += aiState_UnitObtainTargets_Tiles(aE->game->level->importantTiles, enemySide->team, unit, &unit->targetEnemy[0], aE->rangeSqr, totalTargets);
                if(aE->targetPriority != ENTITY_BUILDING)
                    targetPriority = ENTITY_BUILDING;
                else
                    targetPriority = ENTITY_UNIT;
                break;
            }
        }

        /*Reset the searches*/
        vAi_GetUnit_Closest(NULL, 0, 0 ,0, -1);
        vAi_GetBuilding_Closest(NULL, 0, 0 ,0, -1, -1);
        vAi_GetTile_Closest(NULL, 0, 0 ,0, -1, -1);

        /*Find ally entity*/

        vTarget_ClearArray(&unit->targetAlly[0], UNIT_MAX_TARGETS);

        totalTargets = 0;

        /*Check for units*/
        totalTargets += aiState_UnitObtainTargets_Units(ownSide->units, unit, &unit->targetAlly[0], aE->rangeSqr, totalTargets);

        /*Check for buildings*/
        totalTargets += aiState_UnitObtainTargets_Buildings(ownSide->buildings, unit, &unit->targetAlly[0], aE->rangeSqr, totalTargets);

        /*Check for important tiles*/
        totalTargets += aiState_UnitObtainTargets_Tiles(aE->game->level->importantTiles, ownSide->team, unit, &unit->targetAlly[0], aE->rangeSqr, totalTargets);

        vAi_GetUnit_Closest(NULL, 0, 0 ,0, -1);
        vAi_GetBuilding_Closest(NULL, 0, 0 ,0, -1, -1);
        vAi_GetTile_Closest(NULL, 0, 0 ,0, -1, -1);

        timer_Start(&aE->updateTimer);
    }

    return;
}

void aiState_UnitObtainTargets_Exit(Ai_State *a)
{
    AiAttributes_UnitObtainTargets *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    int x = 0;

    if(aiState_SetComplete(a) == 0)
        return;

    vTarget_ClearArray(&unit->targetEnemy[0], UNIT_MAX_TARGETS);
    vTarget_ClearArray(&unit->targetAlly[0], UNIT_MAX_TARGETS);

    return;
}

/*
    Function: aiState_UnitBuyCitadel_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit find the closest citadel build tile, move to it and buy
    a citadel.

    8 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int maxTries - The maximum tries the unit will try to buy a citadel.
    Vent_Level *level - The level the unit is in.
    Vent_Game *game - The game structure.
*/
Ai_State *aiState_UnitBuyCitadel_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int maxTries,
    Vent_Level *level,
    Vent_Game *game
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitBuyCitadel_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitBuyCitadel_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitBuyCitadel_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitBuyCitadel *extraStructure = (AiAttributes_UnitBuyCitadel *)mem_Malloc(sizeof(AiAttributes_UnitBuyCitadel), __LINE__, __FILE__);

    extraStructure->maxTries = maxTries;
    extraStructure->level = level;
    extraStructure->game = game;

    /*Setup the state*/
    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  extraStructure,
                  init,
                  mainLoop,
                  exit
                  );
    return a;
}

void aiState_UnitBuyCitadel_Init(Ai_State *a)
{
    AiAttributes_UnitBuyCitadel *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;
    Vent_Target target;

    Vent_Tile *citadelTile = NULL;

    aE->moveState = NULL;
    aE->citadelTile = NULL;

    //printf("%s: Buy citadel Init\n", unit->name);

    /*Find the closest citadel tile to the unit*/
    target = vAi_GetTile_Closest(aE->level->tiles,
                                      unit->iPosition.x,
                                      unit->iPosition.y,
                                      -1,
                                      TILE_CITADELBASE,
                                      1); /*Starting a new search*/

    citadelTile = target.entity;

    /*If there are no citadel tiles to be found*/
    if(citadelTile == NULL)
    {
        //printf("%s: Unable to find a citadel tile.\n", unit->name);

        /*Exit early*/
        a->update = a->exit;
        return;
    }

    /*Make sure it can be built upon, otherwise check for other citadel tiles*/
    do
    {
        /*if(unit->team == TEAM_PLAYER)
            printf("Citadel at pos %d %d\n", citadelTile->base.position.x, citadelTile->base.position.y);*/
        if(flag_Check(&citadelTile->base.stateFlags, TILESTATE_BUILDABLE) == 1) /*If the tile dosnt already have a citadel built on it*/
        {
            /*This is the tile the unit will try to build a citadel on*/
            /*Break out the loop*/
            break;
        }

        target = vAi_GetTile_Closest(aE->level->tiles,
                                  unit->iPosition.x,
                                  unit->iPosition.y,
                                  -1,
                                  TILE_CITADELBASE,
                                  0);

        citadelTile = target.entity;
    }
    while(citadelTile != NULL);


    /*Clear up the search*/
    vAi_GetTile_Closest(NULL,
                        0,
                        0,
                        0,
                        0,
                        -1
                        );

    /*If there are no citadel tiles to be found*/
    if(citadelTile == NULL)
    {
        /*printf("%s: Unable to find a citadel tile that can be built on.\n", unit->name);*/

        /*Exit early*/
        a->update = a->exit;
        return;
    }

    //printf("%s: Attempting to buy a citadel on the tile at %d %d\n", unit->name, citadelTile->position.x, citadelTile->position.y);

    aE->citadelTile = citadelTile;

    /*Now that the citadel tile has been found, give the unit an ai state to move to it*/
    aE->moveState = aiState_UnitTravelPoint_Setup(aiState_Create(),
                                a->group,       /*group*/
                                a->priority,    /*priority*/
                                AI_COMPLETE_WAIT,   /*complete type*/
                                unit,               /*entity*/
                                citadelTile->base.position.x + ((int)citadelTile->base.width/2), /*move position x*/
                                citadelTile->base.position.y + ((int)citadelTile->base.height/2), /*move position y*/
                                8*8,  /*min range to complete (squared)*/
                                aE->level   /*game level*/
                     );

    /*Move on to the main loop*/
    aiState_Init(a);

    return;
}

void aiState_UnitBuyCitadel_Main(Ai_State *a)
{
    AiAttributes_UnitBuyCitadel *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    Vent_Tile *citadelTile = aE->citadelTile;

    /*Check if the citadel tile is still available for building on*/
    if(flag_Check(&citadelTile->base.stateFlags, TILESTATE_BUILDABLE) == 0 || aE->maxTries < 1)
    {
        //printf("%s: Target citadel tile no longer available, exiting early\n", unit->name);

        a->update = a->exit;
        return;
    }

    /*Update the move to citadel state*/
    if(aE->moveState->complete == AI_NOT_COMPLETE)
    {
        aiState_Update(aE->moveState);
    }
    else /*Else the move state is complete and the unit should be close enough to the tile, try to buy the citadel*/
    {
        /*If the unit can build*/
        if(vBuilding_CanBuild(unit, aE->game) == 1)
        {
            /*If the side the unit is on can buy*/
            if(vBuilding_CheckBuy(BUILDING_CITADEL, unit->team, aE->game, citadelTile) == 1)
            {
                vBuilding_Build(unit->team, BUILDING_CITADEL, citadelTile, aE->game);
                /*printf("%s: Citadel purchased at %d %d\n", unit->name, citadelTile->position.x, citadelTile->position.y);*/

                /*Mission complete*/
                a->update = a->exit;

                return;
            }
            else
            {
                aE->maxTries --;
            }
        }
        else
        {
            aE->maxTries --;
            printf("%s: cant build at location %d %d\n", unit->name, unit->iPosition.x, unit->iPosition.y);
        }
    }

    return;
}

void aiState_UnitBuyCitadel_Exit(Ai_State *a)
{
    AiAttributes_UnitBuyCitadel *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    /*printf("%s: Exiting buy citadel state\n", unit->name);*/
    if(aiState_SetComplete(a) == 0)
        return;

    /*Remove the move to citadel state*/
    if(aE->moveState != NULL)
    {
        aiState_Clean(aE->moveState);
        mem_Free(aE->moveState);
    }

    a->update = NULL;

    return;
}

/*
    Function: aiState_UnitDefendEntity_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit defend another unit or building.

    9 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    void *target - The target to defend, must be either a unit or building.
    void *follow - Should the unit follow its defence target or just wander about when there is nothing to do.
    int targetType - ENTITY_UNIT for a unit target, ENTITY_BUILDING for a building target.
    Vent_Level *level - The level the unit is in.
    Vent_Game *game - The game structure.
*/
Ai_State *aiState_UnitDefendEntity_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    void *target,
    int targetType,
    int follow,
    Vent_Level *level,
    Vent_Game *game
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitDefendEntity_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitDefendEntity_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitDefendEntity_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitDefendEntity *extraStructure = (AiAttributes_UnitDefendEntity *)mem_Malloc(sizeof(AiAttributes_UnitDefendEntity), __LINE__, __FILE__);

    extraStructure->target = target;
    extraStructure->targetType = targetType;
    extraStructure->follow = follow;

    extraStructure->level = level;
    extraStructure->game = game;

    /*Setup the state*/
    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  extraStructure,
                  init,
                  mainLoop,
                  exit
                  );
    return a;
}

void aiState_UnitDefendEntity_Init(Ai_State *a)
{
    AiAttributes_UnitDefendEntity *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    Vent_Unit *targetUnit = NULL;
    Vent_Building *targetBuilding = NULL;

    //printf("%s: Defend Entity Init\n", unit->name);

    aE->attackArea = NULL;

    switch(aE->targetType)
    {
        case ENTITY_UNIT:
        targetUnit = aE->target;

        //printf("\tType Unit: %s\n", targetUnit->name);

        /*Keep track of the unit's health*/
        aE->targetHealth = targetUnit->health;

        break;

        case ENTITY_BUILDING:
        targetBuilding = aE->target;

        //printf("\tType building: %s\n", ventBuilding_Names[targetBuilding->type]);

        /*Keep track of the building's health*/
        aE->targetHealth = targetBuilding->health;

        break;

        default:
        printf("\t Error incompatible type, exiting state (%d)\n", aE->targetType);

        a->update = a->exit;
        return;
        break;
    }

    /*Setup the idle ai state*/
    if(aE->follow == 1 && targetUnit != NULL)
    {
        aE->idle = aiState_UnitFollow_Setup(aiState_Create(),
                                            AI_MOVE_STATE,
                                            7,
                                            AI_COMPLETE_SINGLE,
                                            unit,
                                            targetUnit,
                                            100,
                                            -1
                                            );
    }
    else
    {
        aE->idle = aiState_UnitWander_Setup(aiState_Create(),
                                            AI_MOVE_STATE,
                                            7,
                                            AI_COMPLETE_SINGLE,
                                            unit,
                                            150.0f,
                                            (float)unit->iPosition.x,
                                            (float)unit->iPosition.y,
                                            2500,
                                            &aE->game->gTimer,
                                            0,
                                            aE->game->level);
    }

    aE->updateTimer = timer_Setup(&aE->game->gTimer, -1000, 1000, 1);

    aE->xDis = 0;
    aE->yDis = 0;

    aE->targetDistanceSqr = 0;

    aiState_Init(a);

    return;
}

void aiState_UnitDefendEntity_Main(Ai_State *a)
{
    AiAttributes_UnitDefendEntity *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    Vent_Unit *targetUnit = NULL;
    Vent_Building *targetBuilding = NULL;

    int targetCurrentHealth = 0;
    int targetBaseHealth = 0;
    Vector2DInt targetPosition;
    int updateAttackArea = 0;
    int updateIdle = 0;

    const int targetDefendRangeSqr = 150*150;
    const int maxDistanceSqr = 170*170;

    float wanderDist = 150.0f;

    timer_Calc(&aE->updateTimer);

    /*Set the correct attributes based on the target type*/
    switch(aE->targetType)
    {
        case ENTITY_UNIT:

        targetUnit = (Vent_Unit *)aE->target;

        targetCurrentHealth = targetUnit->health;
        targetBaseHealth = targetUnit->healthStarting;
        targetPosition = targetUnit->iPosition;

        break;

        case ENTITY_BUILDING:

        targetBuilding = (Vent_Building *)aE->target;

        targetCurrentHealth = targetBuilding->health;
        targetBaseHealth = targetBuilding->healthStarting;
        targetPosition = targetBuilding->position;

        break;
    }

    /*If the target is destroyed*/
    if(targetCurrentHealth <= 0)
    {
        //printf("%s: Failed to defend target, exiting\n", unit->name);

        a->update = a->exit;
        return;
    }

    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        timer_Start(&aE->updateTimer);

        /*Get the distance to the target*/
        aE->xDis = targetPosition.x - unit->iPosition.x;
        aE->yDis = targetPosition.y - unit->iPosition.y;

        aE->targetDistanceSqr = mth_DistanceSqr(aE->xDis, aE->yDis);

        if(targetCurrentHealth != aE->targetHealth)
        {
            /*If the target has been damaged*/
            if(targetCurrentHealth < aE->targetHealth)
            {
                /*Is the unit already attacking an area*/
                if(aE->attackArea == NULL)
                {
                    updateAttackArea = 1;
                }
                else
                {
                    /*If the unit is attacking too far away from its defence target, then move towards its defence target*/
                    if(aE->targetDistanceSqr > maxDistanceSqr)
                    {
                        updateAttackArea = 1;
                    }
                }
            }

            /*update the tracking of the targets health*/
            aE->targetHealth = targetCurrentHealth;
        }
        else if(aE->follow == 1)
        {
                /*Make sure the unit is not fighting too far away from its defending target*/

                /*If the unit is too far away from its defence target, then move towards it*/
                if(aE->targetDistanceSqr > maxDistanceSqr)
                {
                    updateIdle = 1;
                }
                else if(aE->attackArea == NULL && aE->targetDistanceSqr < 100*100)
                {
                    updateAttackArea = 1;
                }
        }
    }

    /*If unit can repair and the target is damaged*/
    if(unit->weapon.ammo[WEAPON_REPAIR] > 0 && targetCurrentHealth < targetBaseHealth/2)
    {
        if(aE->targetDistanceSqr < 100*100)
        {
            vWeapon_AssignBullet(&unit->weapon, WEAPON_REPAIR);
            vWeapon_Fire(&unit->weapon, aE->game, unit->iPosition.x, unit->iPosition.y, vWeapon_Direction(unit->weapon.accurate, -1, aE->xDis, -aE->yDis));

            vWeapon_AssignPreviousBullet(&unit->weapon);
        }
    }

    if(aE->attackArea != NULL)
    {
        /*Update the attack area state, if it has been completed then assign a wander/follow state*/
        if(aiState_Update(aE->attackArea) == AI_COMPLETE_WAIT)
        {
            aE->attackArea = aiState_Delete(aE->attackArea);

            updateIdle = 1;
        }
    }
    else if(aE->idle != NULL)
    {
        if(aiState_Update(aE->idle) != AI_NOT_COMPLETE)
        {
            aE->idle = NULL;
        }
    }

    /*If there has been a request made to attack a new area*/
    if(updateAttackArea == 1)
    {
        if(aE->attackArea != NULL)
            aE->attackArea = aiState_Delete(aE->attackArea);

        if(aE->idle != NULL)
            aE->idle = aiState_Delete(aE->idle);

        /*Attack the area until there are no units left*/
        aE->attackArea = aiState_UnitAttackArea_Setup(aiState_Create(),
                    AI_MOVE_STATE,
                    2,
                    AI_COMPLETE_WAIT,
                    unit,
                    aE->game,
                    targetPosition.x,
                    targetPosition.y,
                    targetDefendRangeSqr,
                    2000 /*Wait 2 seconds after all units have been destroyed before quitting*/
                    );
    }
    else if(updateIdle == 1)
    {
        if(aE->attackArea != NULL)
            aE->attackArea = aiState_Delete(aE->attackArea);

        if(aE->idle == NULL)
        {
            /*Setup the idle ai state*/
            if(aE->follow == 1 && targetUnit != NULL)
            {
                aE->idle = aiState_UnitFollow_Setup(aiState_Create(),
                                                    AI_MOVE_STATE,          /*group*/
                                                    7,                      /*priority*/
                                                    AI_COMPLETE_SINGLE,     /*complete type*/
                                                    unit,                   /*entity*/
                                                    targetUnit,             /*unit to follow*/
                                                    50,                    /*follow distance*/
                                                    -1                      /*stop distance (squared)*/
                                                    );
            }
            else
            {
                aE->idle = aiState_UnitWander_Setup(aiState_Create(),
                                                    AI_MOVE_STATE,      /*group*/
                                                    7,                  /*priority*/
                                                    AI_COMPLETE_SINGLE, /*complete type*/
                                                    unit,               /*entity*/
                                                    150.0f,             /*wander distance*/
                                                    (float)unit->iPosition.x,  /*wander origin X*/
                                                    (float)unit->iPosition.y,  /*wander origin Y*/
                                                    2500,               /*time to pause at each point*/
                                                    &aE->game->gTimer,  /*timer to use*/
                                                    0,                  /*use nodes on lvl to roam*/
                                                    aE->game->level);   /*level to wander in*/
            }
        }
    }

    return;
}

void aiState_UnitDefendEntity_Exit(Ai_State *a)
{
    AiAttributes_UnitDefendEntity *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    //printf("%s: Exiting defend entity state\n", unit->name);
    if(aiState_SetComplete(a) == 0)
        return;

    /*Remove the attackArea state*/
    if(aE->attackArea != NULL)
    {
        aiState_Delete(aE->attackArea);
    }

    /*Remove the wander state*/
    if(aE->idle != NULL)
    {
        aiState_Delete(aE->idle);
    }

    a->update = NULL;

    return;
}

/*
    Function: aiState_UnitReportAi_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    This state is supposed to trigger after a state with higher priority has completed.
    It will report to the unit's side that the unit is in need of a new ai state. It will
    also update the tracking of how many units are now performing a certain ai state.

    8 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int aiType - The type of ai to update the frequency of.
    Vent_Side *side - The side the unit is on.
    int waitForGroup - If 1 then the state will not exit until called by AI_COMPLETE_GROUP. Otherwise on update will switch directly to exit state.
*/
Ai_State *aiState_UnitReportAi_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int *aiFrequency,
    Vent_Side *side,
    int waitForGroup
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitReportAi_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitReportAi_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitReportAi_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitReportAi *extraStructure = (AiAttributes_UnitReportAi *)mem_Malloc(sizeof(AiAttributes_UnitReportAi), __LINE__, __FILE__);

    extraStructure->aiFrequency = aiFrequency;
    extraStructure->side = side;
    extraStructure->waitForGroup = waitForGroup;

    /*Setup the state*/
    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  extraStructure,
                  init,
                  mainLoop,
                  exit
                  );
    return a;
}

void aiState_UnitReportAi_Init(Ai_State *a)
{
    /*printf("%s: Keeping track of %d ai freq\n", unit->name, *aE->aiFrequency);*/

    /*Update the amount of units with the ai state this is linked with (Ai state with same group/The state whose priority is higher than it)*/
    /* *aE->aiFrequency = *aE->aiFrequency + 1; done before in the ai side state*/

    /*Move on to the main loop*/
    aiState_Init(a);

    return;
}

void aiState_UnitReportAi_Main(Ai_State *a)
{
    AiAttributes_UnitReportAi *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    if(aE->waitForGroup == 1)
    {
        return;
    }
    else
        a->update = a->exit;

    return;
}

void aiState_UnitReportAi_Exit(Ai_State *a)
{
    AiAttributes_UnitReportAi *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    if(aiState_SetComplete(a) == 0)
        return;

    //printf("%s: Reporting that ai freq %d is complete and unit requires new ai.\n", unit->name, *aE->aiFrequency);

    /*Since the ai state that had a priority higher than this one has finished, set the
    frequency back*/
    if(aE->aiFrequency != NULL)
        *aE->aiFrequency = *aE->aiFrequency - 1;
    else
    {
        /*printf("%s: Ai is now delegated to side ai but without tracking\n", unit->name);*/
    }

    /*The unit now needs a new ai state*/
    depWatcher_AddBoth(&unit->depWatcher, &aE->side->unitsNeedAi, 0);

    a->update = NULL;

    return;
}

/*
    Function: aiState_UnitPhase_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    This state sets the unit as invulnerable for a set time when it takes some damage.

    8 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int invulTime - The length of invulnerability.
    int cooldownTime - How long until the unit can phase into invulnerability again.
    Timer *srcTime - The source timer.
*/
Ai_State *aiState_UnitPhase_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int invulTime,
    int cooldownTime,
    Timer *srcTime
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitPhase_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitPhase_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitPhase_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitPhase *extraStructure = (AiAttributes_UnitPhase *)mem_Malloc(sizeof(AiAttributes_UnitPhase), __LINE__, __FILE__);

    extraStructure->invulTimer = timer_Setup(srcTime, 0, invulTime, 0);
    extraStructure->cooldownTimer = timer_Setup(srcTime, -cooldownTime, cooldownTime, 0);

    /*Setup the state*/
    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  extraStructure,
                  init,
                  mainLoop,
                  exit
                  );
    return a;
}

void aiState_UnitPhase_Init(Ai_State *a)
{
    AiAttributes_UnitPhase *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    timer_Start(&aE->cooldownTimer);

    aiState_Init(a);

    return;
}

void aiState_UnitPhase_Main(Ai_State *a)
{
    AiAttributes_UnitPhase *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    timer_Calc(&aE->cooldownTimer);

    /*If the unit is ready to phase*/
    if(timer_Get_Remain(&aE->cooldownTimer) <= 0)
    {
        /*If damaged the unit will phase out and go invulnerable*/
        if(vUnit_IsDamaged(unit, 0))
        {
            timer_Start(&aE->invulTimer);

            sprite_SetAlphaAll(&unit->sUnitBase, SDL_FAST_ALPHA);

            unit->invulnerability = 1;
        }
    }

    if(unit->invulnerability == 1) /*If the unit is currently invulnerable*/
    {
        timer_Calc(&aE->invulTimer);

        if(timer_Get_Remain(&aE->invulTimer) <= 0) /*Check if the invul time is up*/
        {
            unit->invulnerability = 0;

            sprite_SetAlphaAll(&unit->sUnitBase, SDL_ALPHA_OPAQUE);

            timer_Start(&aE->cooldownTimer); /*Start the cooldown timer*/
        }
    }

    return;
}


void aiState_UnitPhase_Exit(Ai_State *a)
{
    AiAttributes_UnitPhase *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    if(aiState_SetComplete(a) == 0)
        return;

    return;
}


/*
    Function: aiState_UnitSelectWeapon_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    This state allows the unit to change its weapon.

    7 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int fireTime - The time to attack with a secondary weapon.
    int updateTime - The time between each check.
    Timer *srcTime - The source timer.
*/
Ai_State *aiState_UnitSelectWeapon_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int fireTime,
    int updateTime,
    Timer *srcTime
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitSelectWeapon_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitSelectWeapon_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitSelectWeapon_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitSelectWeapon *extraStructure = (AiAttributes_UnitSelectWeapon *)mem_Malloc(sizeof(AiAttributes_UnitSelectWeapon), __LINE__, __FILE__);

    extraStructure->updateTimer = timer_Setup(srcTime, 0, updateTime, 0);

    /*Setup the state*/
    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  extraStructure,
                  init,
                  mainLoop,
                  exit
                  );
    return a;
}

void aiState_UnitSelectWeapon_Init(Ai_State *a)
{
    AiAttributes_UnitSelectWeapon *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    int x = 0;

    /*Set the FLV for how much ammo there is for a certain weapon*/
    fuzzyLV_Setup(&aE->ammoAmount, -1, 1000000, 4,
              fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(1, 3, 5, 0)),
              fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(3, 5, 10)),
              fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(5, 10, 1000000, 1)),
              fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(-1, -1, -1, 1))
    );

   /*Set the FLV for the distance to an enemy unit that is in range*/
   fuzzyLV_Setup(&aE->targetDistanceSqr, 0, 250 * 250, 4,
          fuzzySet_Create(FS_V_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(0, 50 * 50, 60 * 60, 0)),
          fuzzySet_Create(FS_LOW_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(50 * 50, 80 * 80, 100 * 100)),
          fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(80 * 80, 100 * 100, 120 * 120)),
          fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(100 * 100, 120 * 120, 250 * 250, 1))
    );

    /*Set the FLV for relative dps of a certain weapon*/
    fuzzyLV_Setup(&aE->dpsDifference, -100, 1000, 3,
      fuzzySet_Create(FS_LOW_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(-100, -50, -25, 0)),
      fuzzySet_Create(FS_MEDIUM_AMOUNT, FS_TRIANGLE, Fs_Triangle_Create(-50, -25, 0)),
      fuzzySet_Create(FS_HIGH_AMOUNT, FS_SHOULDER, Fs_Shoulder_Create(-25, 0, 1000, 1))
    );

    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        /*Set the FLV for the desirability of giving an ai type*/
        fuzzyLV_Setup(&aE->weaponWeight[x], 0.0f, 1.0f, 3,
          fuzzySet_Create(FS_DESIRE_LOW, FS_SHOULDER, Fs_Shoulder_Create(0.0f, 0.25f, 0.50f, 0)),
          fuzzySet_Create(FS_DESIRE_MEDIUM, FS_TRIANGLE, Fs_Triangle_Create(0.25f, 0.50f, 0.75f)),
          fuzzySet_Create(FS_DESIRE_HIGH, FS_SHOULDER, Fs_Shoulder_Create(0.50f, 0.75f, 1.00f, 1))
        );

        /*Setup the ruleset for each weapon*/
        switch(x)
        {
            default:

            printf("Error unknown weapon switch ai to give unit (%d)\n", x);

            break;

            case WEAPON_PLASMA:
            case WEAPON_PLASMA_WEAK:

            fuzzyRuleset_Setup(&aE->bestWeapon[x], x, &aE->weaponWeight[x], 10,
                           fuzzyRule_Create(&aE->ammoAmount, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),

                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_V_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_LOW_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_HIGH_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),

                           fuzzyRule_Create(&aE->dpsDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_DpsDifference)
            );

            break;

            case WEAPON_MISSILE:
            case WEAPON_ROCKET:

            fuzzyRuleset_Setup(&aE->bestWeapon[x], x, &aE->weaponWeight[x], 10,
                           fuzzyRule_Create(&aE->ammoAmount, FS_LOW_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),

                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_V_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_TargetDistanceSqr),

                           fuzzyRule_Create(&aE->dpsDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_DpsDifference)
            );

            break;

            case WEAPON_MINE:

            fuzzyRuleset_Setup(&aE->bestWeapon[x], x, &aE->weaponWeight[x], 10,
                           fuzzyRule_Create(&aE->ammoAmount, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),

                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_V_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_LOW_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_MEDIUM_AMOUNT, FS_DESIRE_LOW, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_TargetDistanceSqr),

                           fuzzyRule_Create(&aE->dpsDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_DpsDifference)
            );

            break;

            case WEAPON_FLAME:

            fuzzyRuleset_Setup(&aE->bestWeapon[x], x, &aE->weaponWeight[x], 10,
                           fuzzyRule_Create(&aE->ammoAmount, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_MEDIUM_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),
                           fuzzyRule_Create(&aE->ammoAmount, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_Ammo),

                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_V_LOW_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_LOW_AMOUNT, FS_DESIRE_HIGH, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_TargetDistanceSqr),
                           fuzzyRule_Create(&aE->targetDistanceSqr, FS_HIGH_AMOUNT, FS_DESIRE_LOW, &aE->input_TargetDistanceSqr),

                           fuzzyRule_Create(&aE->dpsDifference, FS_LOW_AMOUNT, FS_DESIRE_LOW, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_MEDIUM_AMOUNT, FS_DESIRE_MEDIUM, &aE->input_DpsDifference),
                           fuzzyRule_Create(&aE->dpsDifference, FS_HIGH_AMOUNT, FS_DESIRE_HIGH, &aE->input_DpsDifference)
            );

            break;

            case WEAPON_REPAIR:

            /*Should not be an option*/
            fuzzyRuleset_Setup(&aE->bestWeapon[x], x, &aE->weaponWeight[x], 0
            );

            break;
        }
    }

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}

void aiState_UnitSelectWeapon_Main(Ai_State *a)
{
    int x = 0;
    AiAttributes_UnitSelectWeapon *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;
    Vent_Weapon *weapon = &unit->weapon;

    int bestWeapon = weapon->originalBulletType;
    float maxDesire = 0.0f;
    float desire = 0.0f;

    Vent_Target target;

    timer_Calc(&aE->updateTimer);

    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        timer_Start(&aE->updateTimer);

        if(weapon->ammo[x] == 0)
        {
            vWeapon_AssignBullet(weapon, weapon->originalBulletType);
        }

        for(x = 0; x < UNIT_MAX_TARGETS; x++)
        {
            target = unit->targetEnemy[x];

            if(target.type == ENTITY_UNIT)
            {
                break;
            }
        }

        if(target.entity == NULL)
        {
            return;
        }
        /*if(unit->isPlayer == 1)
        {
            printf("Checking bullets\n");
        }*/

        aE->input_TargetDistanceSqr = (float)target.distanceSqr;

        for(x = 0; x < VENT_TOTALWEAPONS; x++)
        {
            if(weapon->ammo[x] != 0)
            {
                aE->input_Ammo = (float)weapon->ammo[x];
                aE->input_DpsDifference = (weapon->attributes[x].damagePerSecond - weapon->attributes[weapon->originalBulletType].damagePerSecond);

                desire = fuzzyRuleset_Calc(&aE->bestWeapon[x]);

                /*if(unit->team == 0 && unit->type == UNIT_INFANTRY)
                {
                    printf("Desire to give weapon[%s] is %.6f\n", weapon->attributes[x].name, desire);
                    printf("\t Ammo %.2f\n\t TargetDis %.2f\n\t DPSDiff %.2f\n", aE->input_Ammo, sqrt(aE->input_TargetDistanceSqr), aE->input_DpsDifference);
                }*/

                if(desire > maxDesire)
                {
                    bestWeapon = x;
                    maxDesire = aE->bestWeapon[x].outcome->desirability;
                }
            }
        }

        /*if(unit->isPlayer == 1)
                printf("Selecting weapon[%s] is %.6f\n", weapon->attributes[bestWeapon].name, maxDesire);*/
        vWeapon_AssignBullet(weapon, bestWeapon);

        timer_Start(&aE->updateTimer);
        //timer_Start(&aE->fireTimer);
    }


    return;
}


void aiState_UnitSelectWeapon_Exit(Ai_State *a)
{
    AiAttributes_UnitSelectWeapon *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    int x = 0;

    if(aiState_SetComplete(a) == 0)
        return;

    fuzzyLV_Clean(&aE->ammoAmount);
    fuzzyLV_Clean(&aE->targetDistanceSqr);
    fuzzyLV_Clean(&aE->dpsDifference);

    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        fuzzyLV_Clean(&aE->weaponWeight[x]);
        fuzzyRuleset_Clean(&aE->bestWeapon[x]);
    }

    return;
}
