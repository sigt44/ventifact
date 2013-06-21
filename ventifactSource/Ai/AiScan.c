#include "AiScan.h"

#include "Maths.h"
#include "../Level/Tiles.h"
#include "../Buildings/Buildings.h"

/*
    Function: vAi_ScanUnits

    Description -
    This function searches through a linked list containing units, it will return
    the first unit it finds that is within range of maxDistanceSqr. The search can
    be continued (freshSearch = 0) to find the next unit if the one it returned before is not wanted.

    5 arguments:
    struct list *unitList - The linked list containing units, if the search is to be continued then
                            this list should not have changed since the last call of this function.
    int fromX - The X location of the point to measure the max distance from.
    int fromY - The Y location of the point to measure the max distance from.
    int maxDistanceSqr - The max squared distance the unit must be from the XY point given. If (-1) no range checks will be used.
    int freshSearch - If 0 then the search will continue from last time, otherwise it will start a new search.
*/
Vent_Target vAi_ScanUnits(struct list *unitList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int freshSearch)
{
    Vent_Target target;
    Vent_Unit *checkUnit = NULL;

    static struct list *nextUnit;

    int distanceSqr = maxDistanceSqr;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*If freshSearch = 0 then continue at the same position as last time the function was called*/
    if(freshSearch == 0)
        unitList = nextUnit;

    /*Iterate through all the units*/
    while(unitList != NULL)
    {
        nextUnit = unitList->next;

        checkUnit = unitList->data;

        /*If the unit needs to be within a range*/
        if(maxDistanceSqr > -1)
        {
            /*Calculate the distance squared to the unit*/

            distanceSqr = mth_DistanceSqr(checkUnit->iPosition.x - fromX, checkUnit->iPosition.y - fromY);;
        }
        else
        {
            break;
        }

        /*If unit is in range*/
        if(distanceSqr <= maxDistanceSqr)
        {
            break;
        }

        checkUnit = NULL;
        unitList = nextUnit;
    }

    if(checkUnit != NULL)
    {
        vTarget_SetupRange(&target, checkUnit, ENTITY_UNIT, -1, distanceSqr);
    }

    return target;
}

/*
    Function: vAi_GetUnit_Closest

    Description -
    Returns the closest unit from a given point. If freshSearch = 0 then it will return
    the next closest unit that was found from the previous search.

    5 Arguments:
    struct list *unitList - The list of units to search through
    int fromX - The X location of the point the unit is to be closest to.
    int fromY - The Y location of the point the unit is to be closest to.
    int maxDistanceSqr - The maximum distance (squared) the unit can be from the given point. If -1 then any distance will work.
    int freshSearch - If 1 then function will return closest unit, otherwise it will return the next closest unit that was found in the functions previous search.
*/
Vent_Target vAi_GetUnit_Closest(struct list *unitList,
                                 int fromX,
                                 int fromY,
                                 int maxDistanceSqr,
                                 int freshSearch)
{
    Vent_Target target;
    Vent_Target checkTarget;

    int distanceSqr = 0;

    static struct list *closestList = NULL;

    Vent_Unit *closestUnit = NULL;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*Clean up the search and return NULL*/
    if(freshSearch == -1)
    {
        list_Clear(&closestList);
        closestList = NULL;

        return target;
    }
    else if(freshSearch == 1) /*If the first most closest unit is to be returned*/
    {
        if(closestList != NULL)
            list_Clear(&closestList);

        closestList = NULL;
    }
    else if(freshSearch == 0)
    {
        if(closestList != NULL)
        {
            closestUnit = closestList->data;

            /*return the unit that was next most closest*/
            vTarget_SetupRange(&target, closestUnit, ENTITY_UNIT, -1, closestList->info);

            list_Pop(&closestList);
        }

        return target;
    }

    /*Get the first unit thats within range*/
    target = vAi_ScanUnits(unitList,
                            fromX,
                            fromY,
                            maxDistanceSqr,
                            1);

    /*If there isnt a unit in range, return early*/
    if(target.entity == NULL)
    {
        return target;
    }

    /*Check the rest of the units in the list*/
    do
    {
        closestUnit = target.entity;

        if(maxDistanceSqr == -1)
        {
            target.distanceSqr = mth_DistanceSqr(closestUnit->iPosition.x - fromX, closestUnit->iPosition.y - fromY);
        }

        distanceSqr = target.distanceSqr;

        list_Push_Sort(&closestList, target.entity, distanceSqr);

        target = vAi_ScanUnits(unitList,
                    fromX,
                    fromY,
                    maxDistanceSqr,
                    0);
    }
    while(target.entity != NULL);

    if(closestList != NULL)
    {
        closestUnit = closestList->data;

        vTarget_SetupRange(&target, closestUnit, ENTITY_UNIT, -1, closestList->info);

        list_Pop(&closestList);
    }
    else
    {
        vTarget_Setup(&target, NULL, ENTITY_NONE);
    }

    return target;
}

/*
    Function: vAi_UnitIsValid

    Description -
    Returns 1 if the unit passed has the same properties as the supplied arguments.
    Any arguments that are given the value of -1 will be ignored.

    3 Arguments:
    Vent_Unit *u - Unit to check
    int team - The team the unit should be.
    int type - The type the unit should be.
*/
int vAi_UnitIsValid(Vent_Unit *u, int team, int type)
{
    /*If the unit needs to be the correct team*/
    if(team != -1)
    {
        if((int)u->team != team)
            return 0; /*Unit is not valid if the team isnt the same*/
    }

    if(type != -1)
    {
        if((int)u->type != type)
            return 0; /*Unit is not valid if the type is not the same*/
    }

    /*All checks have been made and the unit is valid*/
    return 1;
}

/*
    Function: vAi_GetUnit_ClosestType

    Description -
    Returns the closest unit from a point that contains the same properties (team, type) supplied.

    6 Arguments:
    struct list *unitList - The list of units to search through
    int fromX - The X location of the point the unit is to be closest to.
    int fromY - The Y location of the point the unit is to be closest to.
    int maxDistanceSqr - The maximum distance (squared) the unit can be from the given point. If -1 then any distance will work.
    int team - The team the unit must be, if -1 any team is allowed.
    int type - The type the unit must be, if -1 any type is allowed.
*/
Vent_Target vAi_GetUnit_ClosestType(struct list *unitList,
                                 int fromX,
                                 int fromY,
                                 int maxDistanceSqr,
                                 int team,
                                 int type)
{
    Vent_Target target;

    int closestDistance = -1;
    int distanceSqr = 0;

    Vent_Unit *closestUnit = NULL;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*Get the first unit thats within range*/
    target = vAi_ScanUnits(unitList,
                            fromX,
                            fromY,
                            maxDistanceSqr,
                            1);

    /*If there isnt a unit in range, return early*/
    if(target.entity == NULL)
        return target;

    /*Check the the units in the list*/
    do
    {
        if(vAi_UnitIsValid(target.entity, team, type))
        {
            closestUnit = target.entity;

            if(maxDistanceSqr == -1)
            {
                target.distanceSqr = mth_DistanceSqr(closestUnit->iPosition.x - fromX, closestUnit->iPosition.y - fromY);
            }

            distanceSqr = target.distanceSqr;

            /*If the distance to this unit is less than the closest found so far*/
            if(closestDistance >= distanceSqr || closestDistance == -1)
            {
                closestDistance = distanceSqr;
                /*update new closest unit*/
                closestUnit = target.entity;
            }
        }

        target = vAi_ScanUnits(unitList,
                            fromX,
                            fromY,
                            maxDistanceSqr,
                            0);
    }
    while(target.entity != NULL);

    vTarget_SetupRange(&target, closestUnit, ENTITY_UNIT, -1, closestDistance);

    return target;
}

/*
    Function: vAi_ScanTiles

    Description -
    Returns a tile from a list that is within a distance from a point. If maxDistanceSqr == -1
    then the distance can be any amount.

    5 Arguments:
    struct list *tileList - The list of tiles to search through.
    int fromX - The x-axis point that the distance is to be determined from.
    int fromY - The y-axis point that the distance is to be determined from.
    int maxDistanceSqr - The maximum distance squared that the tile is to be from the point.
    int freshSearch - If 1 then a new search will start, if 0 then the next tile in range will be returned from the previous search.
*/
Vent_Target vAi_ScanTiles(struct list *tileList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int freshSearch)
{
    Vent_Target target;
    Vent_Tile *checkTile = NULL;

    static struct list *nextTile;

    int distanceSqr = maxDistanceSqr;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*If freshSearch = 0 then continue at the same position as last time the function was called*/
    if(freshSearch == 0)
        tileList = nextTile;

    /*Iterate through all the tiles*/
    while(tileList != NULL)
    {
        nextTile = tileList->next;

        checkTile = tileList->data;

        /*If the tile needs to be within a range*/
        if(maxDistanceSqr > -1)
        {
            /*Calculate the distance squared to the tile*/
            distanceSqr = mth_DistanceSqr(checkTile->base.position.x - fromX, checkTile->base.position.y - fromY);
        }
        else
        {
            break;
        }

        /*If tile is in range*/
        if(distanceSqr <= maxDistanceSqr)
        {
            break;
        }

        checkTile = NULL;
        tileList = nextTile;
    }

    if(checkTile != NULL)
    {
        vTarget_SetupRange(&target, checkTile, ENTITY_TILE, -1, distanceSqr);
    }

    return target;
}


/*
    Function: vAi_GetTile_Closest

    Description -
    Returns the closest tile from a given point. If freshSearch = 0 then it will return
    the next closest tile that was found from the previous search.

    6 Arguments:
    struct list *tileList - The list of tiles to search through
    int fromX - The X location of the point the tile is to be closest to.
    int fromY - The Y location of the point the tile is to be closest to.
    int maxDistanceSqr - The maximum distance (squared) the tile can be from the given point. If -1 then any distance will work.
    int type - The type of tile to get the closest of.
    int freshSearch - If 1 then function will return closest tile, otherwise it will return the next closest tile that was found in the functions previous search.
*/
Vent_Target vAi_GetTile_Closest(struct list *tileList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int type,
                         int freshSearch)
{
    static struct list *closestList = NULL; /*Holds the tiles of type found in order of closeness*/

    Vent_Target target;

    Vent_Tile *closestTile = NULL;
    Vent_Tile *checkTile = NULL;

    int closestDistanceSqr = -1;
    int distanceSqr = 0;
    int x = 0;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*Clean up the search and return NULL*/
    if(freshSearch == -1)
    {
        list_Clear(&closestList);
        closestList = NULL;

        return target;
    }
    else if(freshSearch == 1) /*If the first most closest tile is to be returned*/
    {
        if(closestList != NULL)
            list_Clear(&closestList);

        closestList = NULL;
    }
    else if(freshSearch == 0)
    {
        if(closestList != NULL)
        {
            closestTile = closestList->data;

            /*return the tile that was next most closest*/
            vTarget_SetupRange(&target, closestTile, ENTITY_TILE, -1, closestList->info);

            list_Pop(&closestList);
        }

        return target;
    }

    /*Get the first tile that is within a certain distance*/
    target = vAi_ScanTiles(tileList,
                              fromX,
                              fromY,
                              maxDistanceSqr,
                              1);

    /*If there is no tiles within a certain distance*/
    if(target.entity == NULL)
    {
        return target;
    }

    checkTile = target.entity;

    /*If the tile is the same type as required*/
    if(checkTile->base.type == type || type == -1)
    {
        /*Set the closest distance found so far*/
        closestDistanceSqr = target.distanceSqr;
    }

    /*While there are tiles left that are within the requested distance*/
    do
    {
        if(checkTile->base.type == type || type == -1)
        {
            if(maxDistanceSqr == -1)
            {
                target.distanceSqr = mth_DistanceSqr(checkTile->base.position.x - fromX, checkTile->base.position.y - fromY);
            }

            distanceSqr = target.distanceSqr;

            list_Push_Sort(&closestList, checkTile, distanceSqr);
        }

        target = vAi_ScanTiles(tileList,
                                    fromX,
                                    fromY,
                                    maxDistanceSqr,
                                    0);
        checkTile = target.entity;
    }
    while(target.entity != NULL);

    /*Remove the closest tile from the list*/
    if(closestList != NULL)
    {
        closestTile = closestList->data;

        vTarget_SetupRange(&target, closestTile, ENTITY_TILE, -1, closestList->info);

        list_Pop(&closestList);
    }
    else
    {
        vTarget_Setup(&target, NULL, ENTITY_NONE);
    }

    /*Return the closest tile*/
    return target;
}

/*
    Function: vAi_ScanBuildings

    Description -
    Returns a building from a list that is within a distance from a point. If maxDistanceSqr == -1
    then the distance can be any amount.

    5 Arguments:
    struct list *buildingList - The list of buildings to search through.
    int fromX - The x-axis point that the distance is to be determined from.
    int fromY - The y-axis point that the distance is to be determined from.
    int maxDistanceSqr - The maximum distance squared that the building is to be from the point.
    int freshSearch - If 1 then a new search will start, if 0 then the next building in range will be returned from the previous search.
*/
Vent_Target vAi_ScanBuildings(struct list *buildingList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int freshSearch)
{
    Vent_Target target;
    Vent_Building *checkBuilding = NULL;

    static struct list *nextBuilding;

    int distanceSqr = maxDistanceSqr;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*If freshSearch = 0 then continue at the same position as last time the function was called*/
    if(freshSearch == 0)
        buildingList = nextBuilding;

    /*Iterate through all the buildings*/
    while(buildingList != NULL)
    {
        nextBuilding = buildingList->next;

        checkBuilding = buildingList->data;

        /*If the building needs to be within a range*/
        if(maxDistanceSqr > -1)
        {
            /*Calculate the distance squared to the building*/
            distanceSqr = mth_DistanceSqr(checkBuilding->position.x - fromX, checkBuilding->position.y - fromY);
        }
        else
        {
            break;
        }


        /*If building is in range*/
        if(distanceSqr <= maxDistanceSqr)
        {
            break;
        }

        checkBuilding  = NULL;
        buildingList = nextBuilding;
    }

    if(checkBuilding != NULL)
    {
        vTarget_SetupRange(&target, checkBuilding, ENTITY_BUILDING, -1, distanceSqr);
    }

    return target;
}

/*
    Function: vAi_GetBuilding_Closest

    Description -
    Returns the closest building from a given point. If freshSearch = 0 then it will return
    the next closest building that was found from the previous search.

    6 Arguments:
    struct list *buildingList - The list of buildings to search through
    int fromX - The X location of the point the building is to be closest to.
    int fromY - The Y location of the point the building is to be closest to.
    int maxDistanceSqr - The maximum distance (squared) the building can be from the given point. If -1 then any distance will work.
    int type - The type of building to get the closest of.
    int freshSearch - If 1 then function will return closest building, otherwise it will return the next closest building that was found in the functions previous search.
*/
Vent_Target vAi_GetBuilding_Closest(struct list *buildingList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int type,
                         int freshSearch)
{
    static struct list *closestList = NULL; /*Holds the buildings of type found in order of closeness*/

    Vent_Target target;

    Vent_Building *closestBuilding = NULL;
    Vent_Building *checkBuilding = NULL;

    int closestDistanceSqr = -1;
    int distanceSqr = 0;

    vTarget_Setup(&target, NULL, ENTITY_NONE);

    /*Clean up the search and return NULL*/
    if(freshSearch == -1)
    {
        list_Clear(&closestList);
        closestList = NULL;

        return target;
    }
    else if(freshSearch == 1) /*If the first most closest building is to be returned*/
    {
        if(closestList != NULL)
            list_Clear(&closestList);

        closestList = NULL;
    }
    else if(freshSearch == 0)
    {
        if(closestList != NULL)
        {
            closestBuilding = closestList->data;

            /*return the building that was next most closest*/
            vTarget_SetupRange(&target, closestBuilding, ENTITY_BUILDING, -1, closestList->info);

            list_Pop(&closestList);
        }

        return target;
    }

    /*Get the first building that is within a certain distance*/
    target = vAi_ScanBuildings(buildingList,
                              fromX,
                              fromY,
                              maxDistanceSqr,
                              1);

    /*If there is no buildings within a certain distance*/
    if(target.entity == NULL)
    {
        return target;
    }

    checkBuilding = target.entity;

    /*If the building is the same type as required*/
    if(type == -1 || checkBuilding->type == type)
    {
        /*Set the closest distance found so far*/
        closestDistanceSqr = target.distanceSqr;

        closestBuilding = checkBuilding;

        /*Push the building into the list so that it may be possible to obtain the next closest building later*/
        list_Push(&closestList, checkBuilding, distanceSqr);
    }

    /*While there are buildings left that are within the requested distance*/
    do
    {
        if(type == -1 || checkBuilding->type == type)
        {
            if(maxDistanceSqr == -1)
            {
                target.distanceSqr = mth_DistanceSqr(checkBuilding->position.x - fromX, checkBuilding->position.y - fromY);
            }

            distanceSqr = target.distanceSqr;

            list_Push_Sort(&closestList, checkBuilding, distanceSqr);
        }

        target = vAi_ScanBuildings(buildingList,
                                    fromX,
                                    fromY,
                                    maxDistanceSqr,
                                    0);

        checkBuilding = target.entity;
    }
    while(target.entity != NULL);

    /*Remove the closest building from the list*/
    if(closestList != NULL)
    {
        closestBuilding = closestList->data;

        vTarget_SetupRange(&target, closestBuilding, ENTITY_BUILDING, -1, closestList->info);

        list_Pop(&closestList);
    }
    else
    {
        vTarget_Setup(&target, NULL, ENTITY_NONE);
    }

    /*Return the closest building*/
    return target;
}
