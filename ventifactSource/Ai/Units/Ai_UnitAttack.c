#include "Ai_UnitAttack.h"

#include <math.h>

#include "../AiScan.h"
#include "../Ai_Steering.h"
#include "Ai_UnitMove.h"

#include "Maths.h"

#include "../../Weapon.h"

/*
    Function: aiState_UnitAttack_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit attack any enemy units or buildings
    within a specific range.

    7 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    Vent_Game *game - The game structure.
    int searchRange - The max distance the unit will fire at another unit.
    int targetFirst - if 0 will target enemy units first, if 1 will target enemy buildings first.
*/
Ai_State *aiState_UnitAttack_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game,
    int searchRange,
    int targetFirst
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitAttack_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitAttack_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitAttack_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitAttack *extraStructure = (AiAttributes_UnitAttack *)mem_Malloc(sizeof(AiAttributes_UnitAttack), __LINE__, __FILE__);

    extraStructure->searchRange = searchRange;
    extraStructure->targetFirst = targetFirst;
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

/*
    Function: aiState_UnitAttack_Init

    Description -
    This prepares the ai state for main use. Making sure to obtain any information the ai goal
    needs to get started.

    Makes sure that the checks for units in range do not happen every frame.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_UnitAttack_Init(Ai_State *a)
{
    const int baseUpdateTime = 1000; /*Time in milliseconds*/
    const int intervalTime = 100; /*Units will update at intervals of this amount*/
    const int totalIntervals = 10; /*How many intervals there are*/
    static int rotateTimeDelay = 0; /*Changes by +1 on each call to this function, seperates the units
    so that they are not updated on the same frame*/

    int updateTime = 0;

    AiAttributes_UnitAttack *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    /*Make sure all units are not updated the same time at every frame*/
    updateTime = baseUpdateTime + (rotateTimeDelay * intervalTime);

    if(unit->weapon.accurate == 1)
    {
        updateTime /= 4;
    }

    rotateTimeDelay = (rotateTimeDelay + 1) % totalIntervals;

    aE->updateTimer = timer_Setup(&aE->game->gTimer, 0, updateTime, 1);

    aE->trackTarget = aE->currentTarget = NULL;

    /*printf("Unit Attack: Init, update time %d\n", updateTime);*/
    aiState_Init(a);

    return;
}

/*
    Function: aiState_UnitAttack_Main

    Description -
    This function contains the logic for the ai goal to be achieved.
    Fire weapon at the closest unit that is in range.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_UnitAttack_Main(Ai_State *a)
{
    AiAttributes_UnitAttack *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    Vent_Unit *attackUnit = NULL;
    Vent_Building *attackBuilding = NULL;
    Vent_Tile *attackTile = NULL;

    int x = 0;

    float xDis = 0;
    float yDis = 0;

    timer_Calc(&aE->updateTimer);

    /*If its time to check for a new unit/building*/
    if(timer_Get_Remain(&aE->updateTimer) < 0)
    {
        for(x = 0; x < UNIT_MAX_TARGETS; x++)
        {
            if(unit->targetEnemy[x].entity != NULL)
            {
                aE->currentTarget = &unit->targetEnemy[x];
                aE->trackTarget = unit->targetEnemy[x].entity;
                break;
            }
        }

        if(aE->currentTarget != NULL && aE->currentTarget->entity != NULL)
        {
            /*printf("%s: Targeting %s\n", unit->name, entityNames[aE->currentTarget->type]);*/

            switch(aE->currentTarget->type)
            {
                default:

                printf("%s Warning unknown attack target type %d\n", unit->name, aE->currentTarget->type);

                break;

                break;

                case ENTITY_BUILDING:
                attackBuilding = aE->currentTarget->entity;

                aE->attackTarget.x = attackBuilding->middlePosition.x;
                aE->attackTarget.y = attackBuilding->middlePosition.y;

                break;

                case ENTITY_UNIT:
                attackUnit = aE->currentTarget->entity;

                aE->attackTarget.x = attackUnit->middlePosition.x;
                aE->attackTarget.y = attackUnit->middlePosition.y;

                break;

                case ENTITY_TILE:

                attackTile = aE->currentTarget->entity;

                aE->attackTarget.x = attackTile->base.position.x + attackTile->base.width/2;
                aE->attackTarget.y = attackTile->base.position.y + attackTile->base.height/2;

                break;
            }
        }

        timer_Start(&aE->updateTimer);
    }

    /*If the unit cannot fire its weapon*/
    if(vWeapon_CanFire(&unit->weapon) != WEAPON_CANFIRE)
    {
        return; /*exit early*/
    }

    /*If there is a target to fire at*/
    if(aE->currentTarget == NULL || aE->currentTarget->entity == NULL)
    {
        /*printf("%s: Target last seen at (%d %d) destroyed.\n", unit->name, (int)aE->attackTarget.x, (int)aE->attackTarget.y);*/
        return;
    }
    else if(aE->trackTarget != aE->currentTarget->entity) /*The target has changed, exit early*/
    {
        aE->currentTarget = NULL;
        aE->trackTarget = NULL;
    }
    else
    {
        if(aE->currentTarget->type == ENTITY_UNIT)
        {
            attackUnit = aE->currentTarget->entity;

            aE->attackTarget.x = attackUnit->middlePosition.x;
            aE->attackTarget.y = attackUnit->middlePosition.y;
        }

        /*Locate x distance and y distance to enemy entity*/
        xDis = (float)(aE->attackTarget.x - unit->middlePosition.x);
        yDis = (float)(aE->attackTarget.y - unit->middlePosition.y);

        /*Attack enemy unit/building*/
        vWeapon_Fire(&unit->weapon, aE->game, unit->iPosition.x, unit->iPosition.y, vWeapon_Direction(unit->weapon.accurate, -1, xDis, -yDis));
    }

    return;
}

/*
    Function aiState_UnitAttack_Exit

    Description -
    Function that confirms the state is complete so that it can be deleted and also
    cleans up any dynamic memory that has been used.

    1 argument:
    Ai_State *a - The ai state that is to be set as completed and cleaned up
*/
void aiState_UnitAttack_Exit(Ai_State *a)
{
    AiAttributes_UnitAttack *aE = a->extraStructure;

    Vent_Unit *attackUnit = NULL;
    Vent_Building *attackBuilding = NULL;

    /*printf("Unit Attack: Exit\n");*/

    aiState_SetComplete(a);

    a->update = NULL;

    return;
}

/*
    Function: aiState_UnitAttackUnit_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit move to and attack a specific unit.

    7 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    Vent_Game *game - The game structure.
    Vent_Unit *targetUnit - The unit that the entity should try and attack
*/
Ai_State *aiState_UnitAttackUnit_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game,
    Vent_Unit *targetUnit
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitAttackUnit_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitAttackUnit_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitAttackUnit_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitAttackUnit *extraStructure = (AiAttributes_UnitAttackUnit *)mem_Malloc(sizeof(AiAttributes_UnitAttackUnit), __LINE__, __FILE__);

    extraStructure->targetUnit = targetUnit;
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

/*
    Function: aiState_UnitAttackUnit_Init

    Description -

    Makes sure that the check if the unit is in range does not happen every frame.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_UnitAttackUnit_Init(Ai_State *a)
{
    int baseUpdateTime = 0; /*Time in milliseconds*/
    int delayTime = 0;

    AiAttributes_UnitAttackUnit *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    if(aE->targetUnit == NULL)
    {
        printf("%s: warning setting up target unit as NULL, exiting\n", unit->name);
        a->update = a->exit;

        return;
    }

    vSteering_Setup(&aE->steer,
                    &unit->position,
                    &unit->velocity,
                    &unit->targetForce,
                    &aE->targetUnit->position,
                    &aE->targetUnit->velocity
                    );

    aE->maxShootDistance = unit->weapon.attributes[unit->weapon.bulletType].maxDistance;
    aE->steeringForce = 1.0f;

    switch(unit->type)
    {
        case UNIT_TANK:

        aE->maxShootDistance -= (10 + (rand() % 100));

        baseUpdateTime = 5000;

        break;

        case UNIT_AIR:

        baseUpdateTime = 7000;
        delayTime = -7000;

        break;

        default:

        baseUpdateTime = 0;

        break;
    }

    aE->updateTimer = timer_Setup(&aE->game->gTimer, delayTime, baseUpdateTime, 1);

    //if(unit != NULL && aE->targetUnit != NULL)
        //printf("%s: Attacking %s Init\n", unit->name, aE->targetUnit->name);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_UnitAttackUnit_Main

    Description -

    Move to target unit.
    Fire weapon at the target unit if in range.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_UnitAttackUnit_Main(Ai_State *a)
{
    AiAttributes_UnitAttackUnit *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    float xDis = 0.0f;
    float yDis = 0.0f;

    float distanceSqr = 0.0f;

    float maxShootDistanceSqr = (float)unit->weapon.attributes[unit->weapon.bulletType].maxDistanceSqr;

    timer_Calc(&aE->updateTimer);

    if(aE->targetUnit == NULL)
    {
        puts("Warning target unit is NULL");

        a->update = a->exit;

        return; /*exit early*/
    }

    if(vUnit_IsDestroyed(aE->targetUnit) > 0)
    {
        //printf("%s Target unit (%s)(%d %d) destroyed.\n", unit->name, aE->targetUnit->name, (int)aE->targetUnit->position.x, (int)aE->targetUnit->position.y);
        a->update = a->exit;

        return;
    }

    /*Locate x distance and y distance to enemy unit*/
    xDis = (aE->targetUnit->middlePosition.x - unit->middlePosition.x);
    yDis = (aE->targetUnit->middlePosition.y - unit->middlePosition.y);

    distanceSqr = mth_DistanceSqrF(xDis, yDis);

    /*If the unit is in range to attack*/
    if(distanceSqr < maxShootDistanceSqr && vWeapon_CanFire(&unit->weapon) == WEAPON_CANFIRE)
    {
        /*Attack enemy unit*/
        vWeapon_Fire(&unit->weapon, aE->game, unit->iPosition.x, unit->iPosition.y, vWeapon_Direction(unit->weapon.accurate, -1, xDis, -yDis));

    }

    switch(unit->type)
    {
        case UNIT_INFANTRY:
        default:

        /*Get closer directly*/
        if(distanceSqr > (maxShootDistanceSqr - 25*25))
        {
            unit->targetForce.x += vAi_SteerArrive(unit->velocity.x, xDis, 1.0f, 50);
            unit->targetForce.y += vAi_SteerArrive(unit->velocity.y, yDis, 1.0f, 50);
        }

        break;

        case UNIT_AIR:

        vAi_SteerSwoop(&aE->steer,
                       aE->steeringForce,
                       aE->maxShootDistance,
                       &aE->updateTimer);
        break;

        case UNIT_TANK:

        /*If the unit is not close to the target*/
        if(distanceSqr > (maxShootDistanceSqr + 100*100))
        {
            /*Get closer directly*/
            unit->targetForce.x += vAi_SteerArrive(unit->velocity.x, xDis, 1.0f, 50);
            unit->targetForce.y += vAi_SteerArrive(unit->velocity.y, yDis, 1.0f, 50);
        }
        else
        {
            /*Make sure the target unit is moving*/
            if(vector_Is_Zero(aE->steer.targetVelocity) == 0)
            {
                /*Since the unit is close, try to flank the target*/
                vAi_SteerFlank(&aE->steer,
                               aE->steeringForce,
                               aE->maxShootDistance,
                               50.0f,
                               &aE->updateTimer);
            }
            else
            {
                vAi_SteerSwoop(&aE->steer,
                       aE->steeringForce,
                       aE->maxShootDistance,
                       &aE->updateTimer);
            }
        }

        break;
    }

    return;
}


/*
    Function aiState_UnitAttackUnit_Exit

    Description -
    Function that confirms the state is complete so that it can be deleted and also
    cleans up any dynamic memory that has been used.

    1 argument:
    Ai_State *a - The ai state that is to be set as completed and cleaned up
*/
void aiState_UnitAttackUnit_Exit(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    //printf("%s: Attack Unit Exit\n", unit->name);

    aiState_SetComplete(a);

    a->update = NULL;

    return;
}

/*
    Function: aiState_UnitAttackArea_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit move to a specific area and attack the
    enemy units that are within range.

    10 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    Vent_Game *game - The game structure.
    int xArea - The x-axis location of the centre of the area.
    int yArea - The y-axis location of the centre of the area.
    int attackRangeSqr - The radius of the area to attack in (squared)
    unsigned int maxDurationTime - The maximum time that the unit will wait if there are no units in range.
*/
Ai_State *aiState_UnitAttackArea_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game,
    int xArea,
    int yArea,
    int attackRangeSqr,
    unsigned int maxDurationTime
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitAttackArea_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitAttackArea_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitAttackArea_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitAttackArea *extraStructure = (AiAttributes_UnitAttackArea *)mem_Malloc(sizeof(AiAttributes_UnitAttackArea), __LINE__, __FILE__);

    extraStructure->attackRangeSqr = attackRangeSqr;
    extraStructure->durationTimer = timer_Setup(&game->gTimer, 0, maxDurationTime, 0);
    extraStructure->game = game;
    extraStructure->areaLocation.x = xArea;
    extraStructure->areaLocation.y = yArea;

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

/*
    Function: aiState_UnitAttackArea_Init

    Description -
    This prepares the ai state for main use. Making sure to obtain any information the ai goal
    needs to get started.


    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_UnitAttackArea_Init(Ai_State *a)
{
    AiAttributes_UnitAttackArea *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;
    int updateFrequency = 100 + 10*(rand() % 50);

    aE->moveState = aE->attackUnitState = NULL;

    aE->updateTimer = timer_Setup(&aE->game->gTimer, 0, updateFrequency, 0);

    /*Start the duration timer*/
    timer_Start(&aE->durationTimer);

    timer_Start(&aE->updateTimer);

    //printf("%s: Attacking Area(%d %d : %d) for %d length\n", unit->name, aE->areaLocation.x, aE->areaLocation.y, aE->attackRangeSqr, aE->durationTimer.end_Time);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_UnitAttackArea_Main

    Description -
    Make sure the unit is in the area to attack.
    Search for any enemy units in that area and attack them.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_UnitAttackArea_Main(Ai_State *a)
{
    AiAttributes_UnitAttackArea *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    int xDis = 0;
    int yDis = 0;
    int unitAttackArea = 0;
    int callUpdate = 0;

    const int maxOutsideRange = 150*150;

    Vent_Unit *targetUnit = NULL;
    Vent_Target target;

    timer_Calc(&aE->updateTimer);
    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        callUpdate = 1;
        timer_Start(&aE->updateTimer);

        xDis = aE->areaLocation.x - unit->iPosition.x;
        yDis = aE->areaLocation.y - unit->iPosition.y;

        unitAttackArea = mth_DistanceSqr(xDis, yDis) - aE->attackRangeSqr;
    }

    if(aE->moveState != NULL)
    {
        /*Move the unit towards the area to attack*/
        if(aiState_Update(aE->moveState) == AI_COMPLETE_WAIT)
        {
            /*If the state has been completed then deleted it and set it to NULL*/
            aE->moveState = aiState_Delete(aE->moveState);
        }
    }
    else if(callUpdate == 1 && unitAttackArea > 0 && aE->attackUnitState == NULL)
    {
        /*If the unit is outside of the attack area*/
        /*Move the unit close to the area to attack*/
        aE->moveState = aiState_UnitTravelPoint_Setup(aiState_Create(),
                                    a->group, /*Matching group and priority*/
                                    a->priority,
                                    AI_COMPLETE_WAIT,
                                    unit,
                                    aE->areaLocation.x, /*Move to the area to attack*/
                                    aE->areaLocation.y,
                                    aE->attackRangeSqr - (aE->attackRangeSqr/5), /*Slightly further in than the maximum range*/
                                    aE->game->level);
    }

    if(aE->attackUnitState != NULL)
    {
        /*Keep attacking the unit*/
        aiState_Update(aE->attackUnitState);

        if(aE->attackUnitState->complete == AI_COMPLETE_WAIT || unitAttackArea > maxOutsideRange)
        {
            /*If the unit is too far outside of its area or the attack state is complete*/
            aE->attackUnitState = aiState_Delete(aE->attackUnitState);

            timer_Start(&aE->durationTimer);
        }
    }
    else if(callUpdate == 1 && aE->moveState == NULL && aE->attackUnitState == NULL)
    {
        /*Search for an enemy unit within range of the area the unit is suppose to attack*/
        target = vAi_GetUnit_ClosestType(aE->game->side[unit->team].enemy->units,
                                                    aE->areaLocation.x,
                                                    aE->areaLocation.y,
                                                    aE->attackRangeSqr + maxOutsideRange,
                                                    aE->game->side[unit->team].enemy->team,
                                                    -1);

        targetUnit = target.entity;

        /*If there is a unit to attack*/
        if(targetUnit != NULL)
        {
            aE->attackUnitState = aiState_UnitAttackUnit_Setup(aiState_Create(),
                                        a->group,
                                        a->priority,
                                        AI_COMPLETE_WAIT,
                                        unit,
                                        aE->game,
                                        targetUnit
                                        );
        }
        else
        {
            /*There are no units to attack, start the countdown until state complete*/
            if(aE->durationTimer.end_Time != 0)
            {
                timer_Calc(&aE->durationTimer);

                if(timer_Get_Remain(&aE->durationTimer) <= 0)
                {
                    //printf("%s: No units to attack at (%d %d : %d)\n", unit->name, aE->areaLocation.x, aE->areaLocation.y, aE->attackRangeSqr);
                    a->update = a->exit;
                }
            }
        }

    }

    return;
}

/*
    Function aiState_UnitAttackUnit_Exit

    Description -
    Function that confirms the state is complete so that it can be deleted and also
    cleans up any dynamic memory that has been used.

    1 argument:
    Ai_State *a - The ai state that is to be set as completed and cleaned up
*/
void aiState_UnitAttackArea_Exit(Ai_State *a)
{
    AiAttributes_UnitAttackArea *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    //printf("%s: Attack Area: Exit\n", unit->name);

    aiState_SetComplete(a);

    if(a->hasInit == 1)
    {
        if(aE->moveState != NULL)
        {
            aE->moveState = aiState_Delete(aE->moveState);
        }

        if(aE->attackUnitState != NULL)
        {
            aE->attackUnitState = aiState_Delete(aE->attackUnitState);
        }
    }

    return;
}

/*
    Function: aiState_UnitAttackBuilding_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit find an enemy building/tile and move towards it
    with the attack area state.

    6 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    Vent_Game *game - The game structure. Contains the buildings and tiles.
*/
Ai_State *aiState_UnitAttackBuilding_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game
    )
{
    void (*init)(Ai_State *AiState) = &aiState_UnitAttackBuilding_Init;
    void (*mainLoop)(Ai_State *AiState) = &aiState_UnitAttackBuilding_Main;
    void (*exit)(Ai_State *AiState) = &aiState_UnitAttackBuilding_Exit;

    /*Setup custom attributes*/
    AiAttributes_UnitAttackBuilding *extraStructure = (AiAttributes_UnitAttackBuilding *)mem_Malloc(sizeof(AiAttributes_UnitAttackBuilding), __LINE__, __FILE__);

    extraStructure->target = NULL;
    extraStructure->game = game;
    extraStructure->moveState = NULL;
    extraStructure->attackRangeSqr = 0;

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

static void *aiState_UnitAttackBuilding_GetTarget(Ai_State *a)
{
    AiAttributes_UnitAttackBuilding *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    struct list *buildingList = aE->game->side[unit->team].enemy->buildings;
    struct list *tileList = aE->game->level->importantTiles;

    Vent_Tile *tile = NULL;
    Vent_Building *building = NULL;
    Vent_Target target;

    int targetPriority = rand() % 2;
    int targetCitadelFirst = rand() % 4;
    int x = 0;
    Vector2DInt targetPos = {0, 0};

    aE->target = NULL;

    for(x = 0; x < 2; x++)
    {
        if(aE->target != NULL)
            break;

        if(targetPriority == 0)
        {
            target = vAi_GetTile_Closest(tileList,
                         unit->iPosition.x,
                         unit->iPosition.y,
                         -1,
                         -1,
                         1);

            tile = target.entity;

            while(tile != NULL)
            {
                if(tile->base.team != unit->team && tile->destroyed != 1 && flag_Check(&tile->base.stateFlags, TILESTATE_IMPORTANT) == 1)
                {
                    aE->target = tile;
                    aE->targetType = ENTITY_TILE;
                    aE->attackRangeSqr = (70 + tile->radius);
                    aE->attackRangeSqr *= aE->attackRangeSqr;
                    targetPos = tile->base.position;
                    break;
                }
                else
                {
                    target = vAi_GetTile_Closest(tileList,
                         unit->iPosition.x,
                         unit->iPosition.y,
                         -1,
                         -1,
                         0);
                    tile = target.entity;
                }
            }

            targetPriority ++;
        }
        else if(targetPriority == 1)
        {
            /*If unit should target citadel buildings before any others*/
            if(targetCitadelFirst != 0)
            {
                if(aE->game->side[unit->team].enemy->citadels != NULL)
                {
                    /*Change the list to point to only citadels, if there are none it remains the list pointing to all buildings*/
                    buildingList = aE->game->side[unit->team].enemy->citadels;
                }
            }

            target = vAi_GetBuilding_Closest(buildingList,
                     unit->iPosition.x,
                     unit->iPosition.y,
                     -1,
                     -1,
                     1);

            aE->target = target.entity;

            while(aE->target != NULL)
            {
                building = aE->target;

                if(building->destroyed > 0)
                    aE->target = NULL;
                else
                    break;

                target = vAi_GetBuilding_Closest(buildingList,
                                 unit->iPosition.x,
                                 unit->iPosition.y,
                                 -1,
                                 -1,
                                 0);

                aE->target = target.entity;
            }



            if(aE->target != NULL)
            {
                aE->targetType = ENTITY_BUILDING;
                aE->attackRangeSqr = (50 + building->radius);
                aE->attackRangeSqr *= aE->attackRangeSqr;
                targetPos = building->position;
            }

            vAi_GetBuilding_Closest(buildingList,
                                 unit->iPosition.x,
                                 unit->iPosition.y,
                                 -1,
                                 -1,
                                 -1);
            targetPriority--;
        }
    }

    if(aE->target != NULL)
    {
        //printf("Found target tile/building %p of type %d, located at (%d, %d)\n", aE->target, aE->targetType, targetPos.x, targetPos.y);
    }

    return aE->target;
}

void aiState_UnitAttackBuilding_Init(Ai_State *a)
{
    AiAttributes_UnitAttackBuilding *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    int updateTime = 2000;

    aE->updateTimer = timer_Setup(&aE->game->gTimer, 0, updateTime, 1);

    /*printf("%s: Init attack building state\n", unit->name);*/

    aE->target = aiState_UnitAttackBuilding_GetTarget(a);

    if(aE->target == NULL)
    {
        //printf("%s: Init not found a building to attack, exiting early\n", unit->name);
        a->update = a->exit;
    }
    else
    {
        a->update = a->mainLoop;
    }

    return;
}

void aiState_UnitAttackBuilding_Main(Ai_State *a)
{
    AiAttributes_UnitAttackBuilding *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    Vent_Building *building = NULL;
    Vent_Tile *tile = NULL;

    Vector2DInt targetPosition;

    if(aE->target == NULL)
    {
        aE->target = aiState_UnitAttackBuilding_GetTarget(a);

        if(aE->target == NULL)
        {
            /*printf("%s: All enemy buildings and tiles destroyed\n", unit->name);*/
            a->update = a->exit;
            return;
        }
    }

    if(aE->target != NULL)
    {
        switch(aE->targetType)
        {
            case ENTITY_TILE:
            tile = aE->target;

            targetPosition.x = tile->base.position.x + tile->base.width/2;
            targetPosition.y = tile->base.position.y + tile->base.height/2;

            if(tile->destroyed > 0)
            {
                aE->target = NULL;
            }

            break;

            case ENTITY_BUILDING:
            building = aE->target;

            targetPosition = building->middlePosition;

            if(building->destroyed > 0)
            {
                aE->target = NULL;
            }

            break;
        }
    }

    if(aE->moveState != NULL)
    {
        if(aiState_Update(aE->moveState) == AI_COMPLETE_WAIT)
        {
            //printf("%s: Reached minimum range of building target\n", unit->name);
            aE->moveState = aiState_Delete(aE->moveState);
        }
    }
    else if(aE->target != NULL)
    {
        if(mth_DistanceSqr(targetPosition.x - unit->iPosition.x, targetPosition.y - unit->iPosition.y) > aE->attackRangeSqr)
        {
            aE->moveState = aiState_UnitTravelPoint_Setup(aiState_Create(),
                                        a->group, /*Matching group and priority*/
                                        a->priority,
                                        AI_COMPLETE_WAIT,
                                        unit,
                                        targetPosition.x, /*Move to the area to attack*/
                                        targetPosition.y,
                                        aE->attackRangeSqr, /*max distance to be from target*/
                                        aE->game->level);
        }
    }

    return;
}

void aiState_UnitAttackBuilding_Exit(Ai_State *a)
{
    AiAttributes_UnitAttackBuilding *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    //printf("%s: Exiting attack building state\n", unit->name);

    if(aE->moveState != NULL)
    {
        aE->moveState = aiState_Delete(aE->moveState);
    }

    aiState_SetComplete(a);

    a->update = NULL;

    return;
}

