#include "Target.h"

#include "Units/Units.h"
#include "Buildings/Buildings.h"
#include "Level/Tiles.h"

const char *entityNames[ENTITY_END] = {"None", "Unit", "Building", "Tile", "Bullet"};

/*
    Function: vTarget_Setup

    Description -
    Set up the default values for a target entity.

    3 arguments:
    Vent_Target *target - The target to setup.
    void *entity - The entity to give the target.
    int type - The type of entity.
*/
void vTarget_Setup(Vent_Target *target, void *entity, int type)
{
    target->entity = entity;
    target->type = type;

    target->distanceSqr = target->distance = -1;

    return;
}

/*
    Function: vTarget_SetupRange

    Description -
    Set up the default values for a target entity, along with the distance to the target.

    5 arguments:
    Vent_Target *target - The target to setup.
    void *entity - The entity to give the target.
    int type - The type of entity.
    int dis - distance to the target.
    int disSqr - distance squared to the target.
*/
void vTarget_SetupRange(Vent_Target *target, void *entity, int type, int dis, int disSqr)
{
   vTarget_Setup(target, entity, type);

   target->distance = dis;
   target->distanceSqr = disSqr;

   return;
}

/*
    Function: vTarget_SameTeam
    Description -
    Returns 1 if the entity in the target is the same team as the given parameter.

    2 arguments:
    Vent_Target *target - The target to check the entitys team.
    int team - The team the entity should match.
*/
int vTarget_SameTeam(Vent_Target *target, int team)
{
    Vent_Unit *unitTarget = NULL;
    Vent_Building *buildingTarget = NULL;
    Vent_Tile *tileTarget = NULL;

    if(target->entity == NULL)
    {
        return 0;
    }

    switch(target->type)
    {
        default:
        case ENTITY_NONE:
        return 0;
        break;

        case ENTITY_UNIT:
        unitTarget = target->entity;

        if(unitTarget->team == team)
        {
            return 1;
        }

        break;

        case ENTITY_BUILDING:
        buildingTarget = target->entity;

        if(buildingTarget->team == team)
        {
            return 1;
        }

        break;

        case ENTITY_TILE:
        tileTarget = target->entity;

        if(tileTarget->base.team == team)
        {
            return 1;
        }

        break;
    }

    return 0;
}

void vTarget_Report(Vent_Target *target)
{
    printf("Target report %p: type %s, entity %p, distance %d distanceSqr %d\n", target, entityNames[target->type], target->entity, target->distance, target->distanceSqr);

    return;
}

/*
    Function: vTarget_Set
    Description -

    link = 1 -
    This will create a link to the targets entity. If that entity is now destroyed
    the pointer to it will be set to NULL.

    link = 0 -
    This removes a link from the targets entity. This needs to happen if the entity
    that owns this target is destroyed before the targets entity.

    2 arguments:
    Vent_Target *target - The target to link/unlink.
    int link - If(1) link, if(0) unlink
*/
int vTarget_SetLink(Vent_Target *target, int link)
{
    Vent_Unit *unitTarget = NULL;
    Vent_Building *buildingTarget = NULL;
    Vent_Tile *tileTarget = NULL;

    if(target->entity == NULL)
    {
        return 0;
    }

    switch(target->type)
    {
        default:
        printf("Unknown entity to link (%d)\n", target->type);

        return 0;
        break;

        case ENTITY_NONE:

        return 0;

        break;

        case ENTITY_UNIT:
        unitTarget = target->entity;

        if(link == 0)
        {
            depWatcher_RemovePnt(&unitTarget->depWatcher, &target->entity);
        }
        else
        {
            depWatcher_AddPnt(&unitTarget->depWatcher, &target->entity);
        }

        break;

        case ENTITY_BUILDING:
        buildingTarget = target->entity;

        if(link == 0)
        {
            depWatcher_RemovePnt(&buildingTarget->depWatcher, &target->entity);
        }
        else
        {
            depWatcher_AddPnt(&buildingTarget->depWatcher, &target->entity);
        }

        break;

        case ENTITY_TILE:
        tileTarget = target->entity;

        if(link == 0)
        {
            depWatcher_RemovePnt(&tileTarget->depWatcher, &target->entity);
        }
        else
        {
            //printf("Adding tile to dep watcher %p: tile pp %p\n", &tileTarget->depWatcher, &target->entity);
            depWatcher_AddPnt(&tileTarget->depWatcher, &target->entity);
        }

        break;
    }

    return 1;
}

/*
    Function: vTarget_SetupDefaultsArray

    Description -
    Sets the target values to defaults.

    2 arguments:
    Vent_Target *targetArray - Pointer to the start of the array of targets.
    int numArray - Number of targets in the array.
*/
void vTarget_SetupDefaultsArray(Vent_Target *targetArray, int numArray)
{
    int x = 0;

    for(x = 0; x < numArray; x++)
    {
        vTarget_Setup(&targetArray[x], NULL, ENTITY_NONE);
    }

    return;
}

/*
    Function: vTarget_ClearArray

    Description -
    Removes the link to the entity in each target and resets the target values to defaults.

    2 arguments:
    Vent_Target *targetArray - Pointer to the start of the array of targets.
    int numArray - Number of targets in the array.
*/
void vTarget_ClearArray(Vent_Target *targetArray, int numArray)
{
    int x = 0;

    for(x = 0; x < numArray; x++)
    {
        vTarget_SetLink(&targetArray[x], 0);
        vTarget_Setup(&targetArray[x], NULL, ENTITY_NONE);
    }

    return;
}

