#include "Ai_UnitMove.h"
#include "Graphs/Search/DijkstraSearch.h"

#include <math.h>
#include "math.h"

#include "../AiScan.h"
#include "../Ai_Steering.h"
#include "Ai_UnitFunction.h"
#include "../../Game/VentGame.h"

/*
    Function: aiState_UnitRepel_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit move away from other units that are too
    close to it.

    8 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int repelDistanceSqr - The maximum distance the unit should be to another.
    Camera_2D *camera - The game camera. If the unit is not inside of it then no seperation.
    Timer *srcTimer - Source time for update check.
*/
Ai_State *aiState_UnitRepel_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int repelDistanceSqr,
    Camera_2D *camera,
    Timer *srcTimer
    )
{
    AiAttributes_UnitRepel *aE = (AiAttributes_UnitRepel *)mem_Malloc(sizeof(AiAttributes_UnitRepel), __LINE__, __FILE__);

    aE->repelDistanceSqr = repelDistanceSqr;
    aE->camera = camera;
    aE->updateTimer = timer_Setup(srcTimer, 0, 0, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_UnitRepel_Init,
                  aiState_UnitRepel_Main,
                  aiState_UnitRepel_Exit
                  );
    return a;
}

/*
    Function: aiState_UnitRepel_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_UnitRepel_Init(Ai_State *a)
{
    const int baseUpdateTime = 100; /*Time in milliseconds*/
    const int intervalTime = 100; /*Units will update at intervals of this amount*/
    const int totalIntervals = 10; /*How many intervals there are*/
    static int rotateTimeDelay = 0; /*Changes by +1 on each call to this function, seperates the units
    so that they are not updated on the same frame*/

    Vent_Unit *unit = a->entity;
    AiAttributes_UnitRepel *aE = a->extraStructure;

    int updateTime = baseUpdateTime + (rotateTimeDelay * intervalTime);

    rotateTimeDelay = (rotateTimeDelay + 1) % totalIntervals;

    aE->updateTimer.end_Time = updateTime;

    aE->repelDistance = (float)sqrt(aE->repelDistanceSqr);

    /*printf("Unit repel starting: %s to %d (%f)\n", unit->name, aE->repelDistanceSqr, aE->repelDistance);*/

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}
/*
    Function: aiState_UnitRepel_Main

    Description -
    This function performs the logic needed for the ai state to move a unit
    away from other units close to it.

    1 argument:
    Ai_State *a - The ai state of a unit to update.
*/
void aiState_UnitRepel_Main(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitRepel *aE = a->extraStructure;

    Vent_Unit *closestUnit = NULL;

    float xDis, yDis;
    int seperate = 0;
    int maxSeperations = 5; /*Maximum 5 seperations per unit, to save performance*/
    int x = 0;
    int i = 0;

    timer_Calc(&aE->updateTimer);

    if(timer_Get_Remain(&aE->updateTimer) > 0)
    {
        return;
    }

    timer_Start(&aE->updateTimer);

    /*If the unit is not in a sector or the unit is not in the game camera, dont bother to seperate units*/
    if(unit->inSector == NULL || camera2D_Collision(aE->camera, unit->iPosition.x, unit->iPosition.y, unit->width, unit->height) == 0)
    {
        return;
    }

    i = 0;

    do
    {
        for(i = i; i < UNIT_MAX_TARGETS; i++)
        {
            if(unit->targetAlly[i].type == ENTITY_UNIT)
            {
                closestUnit = unit->targetAlly[i].entity;
                i++;
                break;
            }
        }

        if(closestUnit == NULL)
        {
            break;
        }

        /*//Make sure its not found the closest unit as itself*/


        /*Decide which units to seperate from*/
        switch(unit->type)
        {
            case UNIT_INFANTRY:
            default:
            if(closestUnit->type != UNIT_AIR)
            {
                seperate = 1;
            }
            break;

            case UNIT_TANK:
            if(closestUnit->type == UNIT_TANK)
            {
                seperate = 1;
            }
            break;

            case UNIT_AIR:
            if(closestUnit->type == UNIT_AIR)
            {
                seperate = 1;
            }
            break;
        }

        if(seperate == 1)
        {
            xDis = closestUnit->middlePosition.x - unit->middlePosition.x;
            yDis = closestUnit->middlePosition.y - unit->middlePosition.y;

            if(unit->isPlayer == 0)
            {
                unit->seperationForce.x += vAi_SteerTerror(xDis, 1.0f, aE->repelDistance);
                unit->seperationForce.y += vAi_SteerTerror(yDis, 1.0f, aE->repelDistance);
            }

            /*Update the closest unit force as well, if that unit hasnt received a force yet*/
            if(closestUnit->isPlayer == 0)
            {
                if(closestUnit->seperationForce.x == 0.0f)
                    closestUnit->seperationForce.x += vAi_SteerTerror(-xDis, 1.0f, aE->repelDistance);
                if(closestUnit->seperationForce.y == 0.0f)
                    closestUnit->seperationForce.y += vAi_SteerTerror(-yDis, 1.0f, aE->repelDistance);
            }
            /*if(unit->isPlayer == 1)
            {
                printf("Seperation: %.2f %.2f\n", unit->seperationForce.x, unit->seperationForce.y);
            }*/
            seperate = 0;
            x++;
        }

        closestUnit = NULL;
    }
    while(x < maxSeperations);

    return;
}

/*
    Function: aiState_UnitRepel_Exit

    Description -
    Sets the ai state as complete.
    Releases the memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state to exit
*/
void aiState_UnitRepel_Exit(Ai_State *a)
{
    aiState_SetComplete(a);

    return;
}



AiUnitPatrol *aiPatrol_Create(int positionX, int positionY, int pauseTime)
{
    AiUnitPatrol *newPatrol = (AiUnitPatrol *)mem_Malloc(sizeof(AiUnitPatrol), __LINE__, __FILE__);

    newPatrol->positionX = positionX;
    newPatrol->positionY = positionY;
    newPatrol->pauseTime = pauseTime;

    return newPatrol;
}

/*
    Function: aiState_UnitPatrol_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit move through each of the patrol
    points in a list.

    9 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    struct list *patrolList - A list of patrol locations the unit should move to.
    int numLoops - The amount of times the patrol should be traversed (-1 if infinite)
    int loopType - if (1) then just rotate back to the head of the patrol, otherwise (0) reverse the order of patrol once unit has reached the end.
    Timer *srcTime - The source timer for this state to be based on.
*/
Ai_State *aiState_UnitPatrol_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    struct list *patrolList,
    int numLoops,
    int loopType,
    Timer *srcTime
    )
{
    AiAttributes_UnitPatrol *aE = (AiAttributes_UnitPatrol *)mem_Malloc(sizeof(AiAttributes_UnitPatrol), __LINE__, __FILE__);

    aE->numLoops = numLoops;
    aE->loopType = loopType;

    aE->patrolPoint = aE->patrolList = patrolList;

    aE->moveState = NULL;

    aE->pauseTimer = timer_Setup(srcTime, 0, 0, 1);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_UnitPatrol_Init,
                  aiState_UnitPatrol_Main,
                  aiState_UnitPatrol_Exit
                  );
    return a;
}

void aiState_UnitPatrol_Init(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitPatrol *aE = a->extraStructure;

    AiUnitPatrol *patrol = NULL;

    printf("%s: Patrol init, %d nodes\n", unit->name, list_Size(aE->patrolList));

    /*if(aE->patrolPoint != NULL)
    {
        patrol = aE->patrolPoint->data;

        aE->pauseTimer.end_Time = patrol->pauseTime;

        timer_Start(&aE->pauseTimer);
    }*/

    aiState_Init(a);

    return;
}

void aiState_UnitPatrol_Main(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitPatrol *aE = a->extraStructure;

    AiUnitPatrol *patrol = NULL;

    /*Make sure there are points to patrol*/
    if(aE->patrolPoint == NULL)
    {
        file_Log(ker_Log(), 1, "%s: Warning patrol point is NULL\n", unit->name);

        a->update = a->exit;
        return;
    }

    /*If the patrol has been completed by the required amount of times, exit the state*/
    if(aE->numLoops == 0)
    {
        a->update = a->exit;

        return;
    }

    /*If the unit has reached a point in the patrol and needs to stop for a while*/
    if(timer_Get_Remain(&aE->pauseTimer) > 0)
    {
        timer_Calc(&aE->pauseTimer);
        return;
    }

    patrol = aE->patrolPoint->data;

    /*Setup the move state to get to the next patrol node*/
    if(aE->moveState == NULL)
    {
        /*printf("%s: Patrol to %d %d\n", unit->name, patrol->positionX, patrol->positionY);*/

        aE->moveState = aiState_UnitMovePoint_Setup(aiState_Create(),
                                a->group,       /*group*/
                                a->priority,    /*priority*/
                                AI_COMPLETE_WAIT,   /*complete type*/
                                unit,               /*entity*/
                                patrol->positionX, /*move position x*/
                                patrol->positionY, /*move position y*/
                                10 * 10  /*min range to complete (squared)*/
                        );
    }

    if(aE->moveState->complete == AI_NOT_COMPLETE)
    {
        aiState_Update(aE->moveState);
    }
    else /*Since the move state is done, obtain the next patrol point*/
    {
        aE->moveState = aiState_Delete(aE->moveState);
        timer_Start(&aE->pauseTimer);

        switch(aE->loopType)
        {
            default:
            case 1: /*Rotate back to the start once at the end*/
            aE->patrolPoint = aE->patrolPoint->next;

            if(aE->patrolPoint == NULL)
            {
                aE->patrolPoint = aE->patrolList;

                if(aE->numLoops > 0)
                    aE->numLoops --;
            }

            break;

            case 0: /*Reverse direction once at the end*/

            if(aE->patrolPoint->next == NULL)
            {
                aE->patrolPoint = aE->patrolPoint->previous;
                aE->loopType = -1;

                if(aE->numLoops > 0)
                    aE->numLoops --;
            }
            else
                aE->patrolPoint = aE->patrolPoint->next;

            break;

            case -1: /*Reverse direction once at the end*/

            if(aE->patrolPoint == aE->patrolList)
            {
                aE->loopType = 0;

                if(aE->numLoops > 0)
                    aE->numLoops --;
            }
            else
                aE->patrolPoint = aE->patrolPoint->previous;

            break;
        }

        /*Setup the pause time for the current patrol node*/
        if(aE->patrolPoint != NULL)
        {
            patrol = aE->patrolPoint->data;
            aE->pauseTimer.end_Time = patrol->pauseTime;
        }

    }

    return;
}

void aiState_UnitPatrol_Exit(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitPatrol *aE = a->extraStructure;

    //printf("%s: Exiting patrol\n", unit->name);

    if(aiState_SetComplete(a) == 1)
    {
        if(aE->moveState != NULL)
            aE->moveState = aiState_Delete(aE->moveState);
    }

    while(aE->patrolList != NULL)
    {
        mem_Free(aE->patrolList->data); /*Delete the patrol structure*/

        list_Pop(&aE->patrolList);
    }

    a->update = NULL;

    return;
}


/*
    Function: aiState_UnitMovePoint_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit move directly to a specific point on the
    level.

    8 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int pointX - The x axis location of the point on the map the unit should move to.
    int pointY - The y axis location of the point on the map the unit should move to.
    int withinRangeSqr - The squared distance of the minimum range the unit should move to the point.
*/
Ai_State *aiState_UnitMovePoint_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int pointX,
    int pointY,
    int withinRangeSqr
    )
{
    AiAttributes_UnitMovePoint *aE = (AiAttributes_UnitMovePoint *)mem_Malloc(sizeof(AiAttributes_UnitMovePoint), __LINE__, __FILE__);

    aE->location.x = pointX;
    aE->location.y = pointY;
    aE->withinRangeSqr = withinRangeSqr;

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_UnitMovePoint_Init,
                  aiState_UnitMovePoint_Main,
                  aiState_UnitMovePoint_Exit
                  );
    return a;
}


/*
    Function: aiState_UnitMovePoint_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_UnitMovePoint_Init(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitMovePoint *aE = a->extraStructure;

    /*printf("Unit move point init: %s to %d %d\n", unit->name, aE->location.x, aE->location.y);*/

    aiState_Init(a);

    return;
}

/*
    Function: aiState_UnitMovePoint_Main

    Description -
    This function performs the logic needed for the ai state to move a unit
    towards a certain point in the game world.

    1 argument:
    Ai_State *a - The ai state of a unit to update.
*/
void aiState_UnitMovePoint_Main(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitMovePoint *aE = a->extraStructure;

    float xDis, yDis;

    const float brakeRange = 40.0f;

    int distanceSqr = 0;

    xDis = aE->location.x - unit->position.x;
    yDis = aE->location.y - unit->position.y;

    distanceSqr = mth_DistanceSqr(xDis, yDis);

    if(distanceSqr <= (aE->withinRangeSqr))
    {
        //printf("%d %d vs %d %d -> %d vs %d\n", aE->location.x, aE->location.y, unit->iPosition.x, unit->iPosition.y, distanceSqr, aE->withinRangeSqr);
        a->update = a->exit;
        return;
    }

    /*Update the units force so that it arrives at the point, it slows down as it gets nearer*/
    unit->targetForce.x += vAi_SteerArrive(unit->velocity.x, xDis, 1.0f, brakeRange);
    unit->targetForce.y += vAi_SteerArrive(unit->velocity.y, yDis, 1.0f, brakeRange);

    return;
}

/*
    Function: aiState_UnitMovePoint_Exit

    Description -
    Sets the ai state as complete.
    Releases the memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state to exit
*/
void aiState_UnitMovePoint_Exit(Ai_State *a)
{
    aiState_SetComplete(a);

    return;
}

/*
    Function: aiState_UnitWander_Setup

    Description -
    This function sets up a unit ai state causing it to wander around a certain point.
    If nodeRoam == 1 then the points the unit will wander to will be based on the level nodes.

    12 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    float wanderDistance - The distance the unit will wander away from its starting point.
    float startX - The starting x-axis position of the unit.
    float startY - The starting y-axis position of the unit.
    int pointPauseTime - The time the unit will pause once it reaches a wander point.
    Timer *srcTime - The timer to base the pause time on.
    int nodeRoam - If 1 the unit will wander to points at level nodes.
    Vent_Level *level - The current level loaded.
*/
Ai_State *aiState_UnitWander_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    float wanderDistance,
    float startX,
    float startY,
    int pointPauseTime,
    Timer *srcTime,
    int nodeRoam,
    Vent_Level *level
    )
{
    AiAttributes_UnitWander *aE = (AiAttributes_UnitWander *)mem_Malloc(sizeof(AiAttributes_UnitWander), __LINE__, __FILE__);

    aE->startPosition.x = startX;
    aE->startPosition.y = startY;
    aE->wanderDistance = wanderDistance;
    aE->level = level;

    aE->wanderList = NULL;
    aE->currentPoint = NULL;
    aE->nodeRoam = nodeRoam;

    aE->pointTimer = timer_Setup(srcTime, 0, pointPauseTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_UnitWander_Init,
                  aiState_UnitWander_Main,
                  aiState_UnitWander_Exit
                  );
    return a;
}

void aiState_UnitWander_Init(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitWander *aE = a->extraStructure;

    Vector2D *newPoint = NULL;
    int x = 0;
    B_Node *closestStartNode;
    B_Node *closestEndNode;
    float angle = 0.0f;

    /*printf("Unit wander init: %s from %.2f %.2f\n Getting node list.", unit->name, aE->startPosition.x, aE->startPosition.y);*/

    /*Create 4 points to wander to, each in a different quadrant of a circle*/
    for(x = 0; x < 4; x++)
    {
        /*Find the first quadrant position to move towards*/
        angle = ((rand() % 90)/180.0f) * PI;
        newPoint = (Vector2D *)mem_Malloc(sizeof(Vector2D), __LINE__, __FILE__);
        newPoint->x = cosf(angle) * aE->wanderDistance;
        newPoint->y = sinf(angle) * aE->wanderDistance;

        /*Depending on the value of x, convert the points to each quadrant*/
        switch(x)
        {
            default:
            break;

            case 1:
            /*Switch to 2nd quadrant*/
            newPoint->x = -newPoint->x;
            break;

            case 2:
            /*Switch to 3rd quadrant*/
            newPoint->x = -newPoint->x;
            newPoint->y = -newPoint->y;
            break;

            case 3:
            /*Switch to 4th quadrant*/
            newPoint->y = -newPoint->y;
            break;
        }

        *newPoint = vector_Add(&aE->startPosition, newPoint);

        /*If the unit should wander based on nodes placed in the level*/
        if(aE->nodeRoam == 1)
        {
            /*Get the closest node to the starting wander position*/
            closestStartNode = node_GetClosest(aE->level->pathGraph.nodes, (int)aE->startPosition.x, (int)aE->startPosition.y, NULL);
            /*Get the closest node to the ending wander position*/
            closestEndNode = node_GetClosest(aE->level->pathGraph.nodes, (int)newPoint->x, (int)newPoint->y, NULL);

            /*Obtain the list of nodes that the unit should travel through in order to reach its final point,
                then add that list into the wanderList*/
            list_Concatenate(&aE->wanderList, Dijkstra_Search(&aE->level->pathSearch, &aE->level->pathGraph, closestStartNode->index, closestEndNode->index));

            mem_Free(newPoint);
        }
        else
        {
            /*If the point to wander to is outside of the level region, then set it back to its starting point*/
            if(newPoint->x < 0 || newPoint->x > aE->level->header.width)
                newPoint->x = aE->startPosition.x;
            if(newPoint->y < 0 || newPoint->y > aE->level->header.height)
                newPoint->y = aE->startPosition.y;

            //printf("Point[%d]: %.2f %.2f, range %.2f\n", x, newPoint->x, newPoint->y, aE->wanderDistance);
            /*Push this point into the list*/
            list_Stack(&aE->wanderList, newPoint, 0);
        }
    }

    aE->currentPoint = aE->wanderList;

    unit->seperationWeight = 0.2f;

    timer_Start(&aE->pointTimer);

    aiState_Init(a);

    return;
}

void aiState_UnitWander_Main(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitWander *aE = a->extraStructure;

    float xDis, yDis;

    const float relaxDistance = 50.0f; /*The distance the unit will receive a reduced force*/
    const float minDistanceSqr = 20.0f * 20.0f; /*The distance the unit has to be until it coints as reaching its target point*/

    float distanceSqr = 0;

    Vector2D location;
    Vector2D *locationPoint;
    B_Node *locationNode;

    if(aE->currentPoint == NULL)
    {
        a->update = a->exit;
        return;
    }

    if(aE->nodeRoam == 1)
    {
        /*Get the location points from a level node*/
        locationNode = aE->currentPoint->data;
        location.x = (float)locationNode->x;
        location.y = (float)locationNode->y;
    }
    else
    {
        /*Get the location point directly*/
        locationPoint = aE->currentPoint->data;
        location.x = locationPoint->x;
        location.y = locationPoint->y;
    }

    xDis = location.x - unit->position.x;
    yDis = location.y - unit->position.y;

    distanceSqr = mth_DistanceSqrF(xDis, yDis);

    if(distanceSqr > minDistanceSqr)
    {
        timer_Start(&aE->pointTimer);

        switch(unit->type)
        {
            default:

            unit->targetForce.x += vAi_SteerMove(xDis, 1.0f, relaxDistance, 0.2f);
            unit->targetForce.y += vAi_SteerMove(yDis, 1.0f, relaxDistance, 0.2f);

            break;

            case UNIT_AIR:

            unit->targetForce.x += vAi_SteerArrive(unit->velocity.x, xDis, 1.0f, 100.0f);
            unit->targetForce.y += vAi_SteerArrive(unit->velocity.y, yDis, 1.0f, 100.0f);

            break;

            case UNIT_TANK:

            if(fabs(xDis) > fabs(yDis))
                unit->targetForce.x += vAi_SteerMove(xDis, 2.0f, relaxDistance, 0.2f);
            else
                unit->targetForce.y += vAi_SteerMove(yDis, 2.0f, relaxDistance, 0.2f);

            break;
        }
    }
    else
    {
        timer_Calc(&aE->pointTimer);

        /*Move to next wander point*/
        if(timer_Get_Remain(&aE->pointTimer) <= 0)
        {
            aE->currentPoint = aE->currentPoint->next;

            if(aE->currentPoint == NULL)
            {
                aE->currentPoint = aE->wanderList;
            }
        }
    }

    return;
}

void aiState_UnitWander_Exit(Ai_State *a)
{
    AiAttributes_UnitWander *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    if(aiState_SetComplete(a) == 0)
        return;

    /*Delete all the wander points*/
    while(aE->wanderList != NULL)
    {
        if(aE->nodeRoam == 0)
        {
            mem_Free(aE->wanderList->data);
        }

        list_Pop(&aE->wanderList);
    }

    unit->seperationWeight = 1.0f;

    return;
}


/*
    Function: aiState_UnitTravelPoint_Setup

    Description -
    This function sets up the variables for an ai state ment for a unit.

    The behaviour of the state is to have the unit move to a specific point on the
    level, using any path nodes to get there without colliding with level objects.

    9 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that this ai will control.
    int pointX - The x axis location of the point on the map the unit should move to.
    int pointY - The y axis location of the point on the map the unit should move to.
    int withinRangeSqr - The squared distance of the minimum range the unit should move to the point.
    Vent_Level *level - The level of the game
*/
Ai_State *aiState_UnitTravelPoint_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int pointX,
    int pointY,
    int withinRangeSqr,
    Vent_Level *level
    )
{
    AiAttributes_UnitTravelPoint *aE = (AiAttributes_UnitTravelPoint *)mem_Malloc(sizeof(AiAttributes_UnitTravelPoint), __LINE__, __FILE__);

    aE->location.x = (float)pointX;
    aE->location.y = (float)pointY;
    aE->withinRangeSqr = withinRangeSqr;
    aE->level = level;

    aE->nodeTravelPath = NULL;

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_UnitTravelPoint_Init,
                  aiState_UnitTravelPoint_Main,
                  aiState_UnitTravelPoint_Exit
                  );
    return a;
}

void aiState_UnitTravelPoint_Init(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitTravelPoint *aE = a->extraStructure;

    B_Node *startLocation = NULL;
    B_Node *endLocation = NULL;
    int distanceSqrToPoint = 0;
    int distanceSqrToNode = 0;

    //printf("%s: Travel point to %.2f %.2f in rangeSqr %d\n", unit->name, aE->location.x, aE->location.y, aE->withinRangeSqr);

    switch(unit->type)
    {
        default:
        aE->withinPathRangeSqr = 20*20;

        /*Start finding a path to the target location*/
        startLocation = node_GetClosest(aE->level->pathGraph.nodes, unit->iPosition.x, unit->iPosition.y, NULL);
        endLocation = node_GetClosest(aE->level->pathGraph.nodes, (int)aE->location.x, (int)aE->location.y, NULL);

        //printf("%s: Found path end nodes - (%d %d) to (%d %d)\n", unit->name, startLocation->x, startLocation->y, endLocation->x, endLocation->y);

        /*Obtain the list of nodes that the unit should travel through in order to reach its final point*/
        if(startLocation == NULL || endLocation == NULL)
        {
            aE->nodeTravelPath = NULL;
        }
        else
        {
            aE->nodeTravelPath = Dijkstra_Search(&aE->level->pathSearch, &aE->level->pathGraph, startLocation->index, endLocation->index);
        }

        if(aE->nodeTravelPath == NULL)
        {
            //printf("Warning could not find a path for %s\n", unit->name);
        }
        else
        {
            /*Make sure the location of the end node is closer than the units current position*/
            distanceSqrToPoint = mth_DistanceSqr(aE->location.x - unit->iPosition.x, aE->location.y- unit->iPosition.y);
            distanceSqrToNode = mth_DistanceSqr(endLocation->x - (int)aE->location.x, endLocation->y - (int)aE->location.y);

            /*If the unit is closer to its destination than the final node is to the destination*/
            if(distanceSqrToPoint < distanceSqrToNode)
            {
                /*Clear the node path and have the unit move to the final destination directly*/
                //printf("%s: %d < %d so moving directly\n", unit->name, distanceSqrToPoint, distanceSqrToNode);
                //printf("\tStartLoc (%d %d) TargetLoc (%d %d), GraphStartLoc (%d %d) GraphEndLoc (%d %d)\n", unit->iPosition.x, unit->iPosition.y, (int)aE->location.x, (int)aE->location.y, startLocation->x, startLocation->y, endLocation->x, endLocation->y);
                list_Clear(&aE->nodeTravelPath);
            }
        }
        break;

        case UNIT_AIR:
        aE->withinPathRangeSqr = 60*60;

        break;
    }

    unit->seperationWeight = 0.2f;

    aiState_Init(a);

    return;
}

void aiState_UnitTravelPoint_Main(Ai_State *a)
{
    Vent_Unit *unit = a->entity;
    AiAttributes_UnitTravelPoint *aE = a->extraStructure;

    float xDis, yDis;

    const float brakeRange = 100.0f; /*Max distance for the unit to start slowing down*/
    const float relaxDistance = 10.0f; /*The distance the unit will receive a reduced force*/

    int distanceSqr = 0;

    Vector2D location;
    B_Node *locationNode;

    /*If there are still nodes left to move to*/
    if(aE->nodeTravelPath != NULL)
    {
        locationNode = aE->nodeTravelPath->data;
        location.x = (float)locationNode->x;
        location.y = (float)locationNode->y;
    }
    else
    {
        /*Otherwise directly move to the original requested location*/
        location.x = aE->location.x;
        location.y = aE->location.y;
    }

    xDis = location.x - unit->position.x;
    yDis = location.y - unit->position.y;

    distanceSqr = (int)mth_DistanceSqrF(xDis, yDis);

    /*If the unit is within range of the current location it needs to go to*/
    /*If it was to a node on the level*/
    if(aE->nodeTravelPath != NULL && (distanceSqr <= aE->withinPathRangeSqr))
    {
        list_Pop(&aE->nodeTravelPath); /*Go to the next node*/
    }
    else if(aE->nodeTravelPath == NULL && distanceSqr <= (aE->withinRangeSqr)) /*If it was the final destination*/
    {
        /*printf("%s: Arrived at location (%.2f %.2f) with distanceSqr %d\n", unit->name, location.x, location.y, distanceSqr);*/
        /*The unit must of reached the original point so the state is complete*/
        a->update = a->exit;
        return;
    }

    if(aE->nodeTravelPath != NULL)
    {
        /*Move directly to the level node*/
        unit->targetForce.x += vAi_SteerArrive(unit->velocity.x, xDis, 1.0f, relaxDistance);
        unit->targetForce.y += vAi_SteerArrive(unit->velocity.y, yDis, 1.0f, relaxDistance);
    }
    else /*Unit is travelling to the final position, set the steering behaviour to arrive*/
    {
        unit->seperationWeight = 0.0f;
        unit->targetForce.x += vAi_SteerArrive(unit->velocity.x, xDis, 1.0f, brakeRange);
        unit->targetForce.y += vAi_SteerArrive(unit->velocity.y, yDis, 1.0f, brakeRange);
    }

    return;
}

/*
    Function: aiState_UnitTravelPoint_Exit

    Description -
    Sets the ai state as complete.
    Releases the memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state to exit
*/
void aiState_UnitTravelPoint_Exit(Ai_State *a)
{
    AiAttributes_UnitTravelPoint *aE = a->extraStructure;
    Vent_Unit *unit = a->entity;

    if(aiState_SetComplete(a) == 0)
        return;

    /*Clean up the level node list*/
    while(aE->nodeTravelPath != NULL)
    {
        list_Pop(&aE->nodeTravelPath);
    }

    unit->seperationWeight = 1.0f;

    return;
}

/*
    Function: aiState_UnitFollow

    Description -
    This function sets up an ai state for a unit to follow another unit.
    It can either follow its target until it gets to a certain distance or
    follow it indefinitly.

    8 Arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Unit *entity - The unit that will follow another.
    Vent_Unit *target - The target unit that the follower unit will follow.
    int followDistanceSqr - The distance the unit should stay away from its target.
    int stopDistanceSqr - The distance that the follow unit needs to reach its target for the ai state to complete. If < 0 then it will follow until the target is destroyed.
*/
Ai_State *aiState_UnitFollow_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Unit *target,
    int followDistance,
    int stopDistanceSqr
    )
{
    AiAttributes_UnitFollow *aE = (AiAttributes_UnitFollow *)mem_Malloc(sizeof(AiAttributes_UnitFollow), __LINE__, __FILE__);

    aE->target = target;
    aE->followDistance = followDistance;
    aE->stopDistanceSqr = stopDistanceSqr;

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_UnitFollow_Init,
                  aiState_UnitFollow_Main,
                  aiState_UnitFollow_Exit
                  );
    return a;
}

void aiState_UnitFollow_Init(Ai_State *a)
{
    AiAttributes_UnitFollow *aE = a->extraStructure;

    Vent_Unit *unit = a->entity;

    /*printf("%s: following %s\n", unit->name, aE->target->name);*/

    /*If the unit to follow is destroyed exit early*/
    if(vUnit_IsDestroyed(aE->target) > 0)
    {
        a->update = a->exit;
        return;
    }

    aE->followDistanceSqr = aE->followDistance * aE->followDistance;

    vSteering_Setup(&aE->steer,
                &unit->position,
                &unit->velocity,
                &unit->targetForce,
                &aE->target->position,
                &aE->target->velocity
                );

    switch(unit->type)
    {
        case UNIT_AIR:
        aE->updateTimer = timer_Setup(NULL, 0, 6000, 1);
        break;

        default:
        aE->updateTimer = timer_Setup(NULL, 0, 3000, 1);
        break;
    }

    aiState_Init(a);

    return;
}

void aiState_UnitFollow_Main(Ai_State *a)
{
    AiAttributes_UnitFollow *aE = a->extraStructure;

    Vent_Unit *unit = a->entity;

    int xDis = 0;
    int yDis = 0;
    int distanceSqr = 0;

    /*If the unit to follow is destroyed exit early*/
    if(vUnit_IsDestroyed(aE->target) > 0)
    {
        a->update = a->exit;
        return;
    }

    xDis = aE->target->iPosition.x - unit->iPosition.x;
    yDis = aE->target->iPosition.y - unit->iPosition.y;

    distanceSqr = mth_DistanceSqr(xDis, yDis);

    /*If the unit is close enough to the target in order for the ai state to be completed*/
    if(aE->stopDistanceSqr > -1 && aE->stopDistanceSqr < distanceSqr)
    {
        a->update = a->exit;
        return;
    }

    switch(unit->type)
    {
        case UNIT_INFANTRY:
        default:

        /*Get closer directly*/
        if(distanceSqr > aE->followDistanceSqr)
        {
            unit->targetForce.x += vAi_SteerMove((float)xDis, 1.0f, 25.0f, 0.3f);
            unit->targetForce.y += vAi_SteerMove((float)yDis, 1.0f, 25.0f, 0.3f);
        }

        break;

        case UNIT_TANK:
        vAi_SteerFlank(&aE->steer,
                       (float)unit->steeringForce,
                       (float)aE->followDistance,
                       80.0f,
                       &aE->updateTimer);
        break;

        case UNIT_AIR:

        if(distanceSqr > (aE->followDistanceSqr + 150*150))
        {
            vAi_SteerFlank(&aE->steer,
                   (float)unit->steeringForce,
                   (float)aE->followDistance,
                   100.0f,
                   &aE->updateTimer);
        }
        else
        {
            vAi_SteerSwoop(&aE->steer,
                         (float)unit->steeringForce,
                         (float)aE->followDistance,
                         &aE->updateTimer);
        }
        break;
    }

    return;
}

void aiState_UnitFollow_Exit(Ai_State *a)
{
    AiAttributes_UnitFollow *aE = a->extraStructure;

    Vent_Unit *unit = a->entity;

    /*printf("%s: Exiting follow unit\n", unit->name);*/

    aiState_SetComplete(a);

    return;
}

/*
    Function: aiUnit_CallIn

    Description -
    Produces a force towards the unit supplied. The force is received by any
    units which are defending it and are within a certain range.

    2 arguments:
    Vent_Unit *u - The unit thatthe force is directed to.
    struct list *allyUnits - List of allied units to the unit u, which may receive the force.
*/
int aiUnit_CallIn(Vent_Unit *u, struct list *allyUnits)
{
    int callRangeSqr = 300 * 300;
    float brakeRange = 20.0f;
    float xDis, yDis;
    int numCalled = 0;

    Vent_Unit *targetUnit = NULL;
    Vent_Target target;

    /*Search for an ally unit within call range*/
    target = vAi_ScanUnits(allyUnits,
                                u->iPosition.x,
                                u->iPosition.y,
                                callRangeSqr,
                                1);

    targetUnit = target.entity;

    while(targetUnit != NULL)
    {
        /*If targetUnit is not the supplied unit and has the defend entity state*/
        if(targetUnit != u && aiModule_HasAi(&targetUnit->ai, &aiState_UnitDefendEntity_Init) == AI_NOT_COMPLETE)
        {
            numCalled ++;

            xDis = u->position.x - targetUnit->position.x;
            yDis = u->position.y - targetUnit->position.y;

            targetUnit->targetForce.x = vAi_SteerArrive(targetUnit->velocity.x, xDis, 2.0f, brakeRange);
            targetUnit->targetForce.y = vAi_SteerArrive(targetUnit->velocity.y, yDis, 2.0f, brakeRange);
        }

        target = vAi_ScanUnits(allyUnits,
                                    u->iPosition.x,
                                    u->iPosition.y,
                                    callRangeSqr,
                                    0); /*Continue from last search*/

        targetUnit = target.entity;
    }

    return numCalled;
}






