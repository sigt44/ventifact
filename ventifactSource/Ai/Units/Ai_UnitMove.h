#ifndef AI_UNITMOVE_H
#define AI_UNITMOVE_H

#include "../../Level/Level.h"
#include "../../Units/Units.h"
#include "../Ai_State.h"
#include "../Ai_Steering.h"
#include "Maths.h"
#include "Vector2D.h"

typedef struct
{
    Timer updateTimer; /*Time between each check*/

    float repelDistance; /*How close the unit should be to another unit until the repel force starts*/
    int repelDistanceSqr; /*How close(squared) the unit should be to another unit until the repel force starts*/

    Camera_2D *camera; /*Only if the unit is inside of the game camera will it receive seperation force*/
} AiAttributes_UnitRepel;

typedef struct
{
    Vector2DInt location; /*Location of the point the unit should move to*/
    int withinRangeSqr; /*How close the unit should move to the point*/

} AiAttributes_UnitMovePoint;

typedef struct
{
    Vector2D location; /*Location of the point the unit should move to*/
    int withinRangeSqr; /*Max distance(squared) the unit can be away from its final destination*/
    int withinPathRangeSqr; /*Max distance(squared) the unit can be away from its path*/

    struct list *nodeTravelPath; /*List of nodes the unit will travel to in order to reach destination*/
    Vent_Level *level; /*Level to get the list of nodes to get the travel path from*/

} AiAttributes_UnitTravelPoint;

typedef struct
{
    float wanderDistance; /*The distance the unit will wander from its original point*/
    Vector2D startPosition; /*The original position that the unit should wander from*/

    Timer pointTimer; /*Timer used so that the unit will pause for a bit at each wander point*/

    struct list *wanderList; /*A linked list, holding positions that the unit should move to.*/
    struct list *currentPoint; /*Points to the list node containing the current position to move to.*/

    int nodeRoam; /*If set as 1, then the unit will wander towards nodes on the level*/
    Vent_Level *level; /*The current loaded level*/

} AiAttributes_UnitWander;

typedef struct
{
    Vent_Unit *target; /*The unit to follow*/
    int stopDistanceSqr; /*If true the ai state will stop once the unit is near its target unit*/
    int followDistanceSqr;
    int followDistance; /*The distance for the unit to keep away from its target*/

    Vent_Steering steer; /*Steering behaviour variables of the unit*/

    Timer updateTimer;

} AiAttributes_UnitFollow;

typedef struct
{
    struct list *patrolList; /*List of AiUnitPatrol structs*/
    struct list *patrolPoint; /*Points to the list holding the current patrol point to move to*/

    int numLoops; /*Amount of times to patrol*/
    int loopType; /*(1) if the unit should move to the first node once it has reached the last, (0) to reverse the traversal of nodes once last node is reached*/

    Ai_State *moveState; /*Ai state to move the unit*/

    Timer pauseTimer; /*Timer to pause after reaching each patrol point*/

} AiAttributes_UnitPatrol;

typedef struct
{
    int positionX; /*Position to patrol to*/
    int positionY;
    int pauseTime; /*Time to pause once in position*/

} AiUnitPatrol;


Ai_State *aiState_UnitRepel_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int repelDistanceSqr,
    Camera_2D *camera,
    Timer *srcTimer
    );

void aiState_UnitRepel_Init(Ai_State *a);

void aiState_UnitRepel_Main(Ai_State *a);

void aiState_UnitRepel_Exit(Ai_State *a);



AiUnitPatrol *aiPatrol_Create(int positionX, int positionY, int pauseTime);

Ai_State *aiState_UnitPatrol_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    struct list *patrolList,
    int numLoops,
    int loopType,
    Timer *srcTime
    );

void aiState_UnitPatrol_Init(Ai_State *a);

void aiState_UnitPatrol_Main(Ai_State *a);

void aiState_UnitPatrol_Exit(Ai_State *a);

Ai_State *aiState_UnitMovePoint_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int pointX,
    int pointY,
    int withinRangeSqr
    );

void aiState_UnitMovePoint_Init(Ai_State *a);

void aiState_UnitMovePoint_Main(Ai_State *a);

void aiState_UnitMovePoint_Exit(Ai_State *a);




Ai_State *aiState_UnitTravelPoint_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int pointX,
    int pointY,
    int withinRangeSqr,
    Vent_Level *level
    );

void aiState_UnitTravelPoint_Init(Ai_State *a);

void aiState_UnitTravelPoint_Main(Ai_State *a);

void aiState_UnitTravelPoint_Exit(Ai_State *a);




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
    );

void aiState_UnitWander_Init(Ai_State *a);

void aiState_UnitWander_Main(Ai_State *a);

void aiState_UnitWander_Exit(Ai_State *a);



Ai_State *aiState_UnitFollow_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Unit *target,
    int followDistance,
    int stopDistanceSqr
    );

void aiState_UnitFollow_Init(Ai_State *a);

void aiState_UnitFollow_Main(Ai_State *a);

void aiState_UnitFollow_Exit(Ai_State *a);

int aiUnit_CallIn(Vent_Unit *u, struct list *allyUnits);

#endif
