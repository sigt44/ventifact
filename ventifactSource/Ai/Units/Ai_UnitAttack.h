#ifndef AI_UNITATTACK_H
#define AI_UNITATTACK_H

#include "../Ai_State.h"
#include "../Ai_Steering.h"

#include "../../Units/Units.h"
#include "../../Game/VentGame.h"
#include "../../Buildings/Buildings.h"
#include "Time/Timer.h"

/*
    Ai state structures
*/

typedef struct
{
    int searchRange;
    int targetFirst; /*Target a building(0) or unit(1) first*/

    Vent_Game *game;

    Vector2DInt attackTarget; /*The place to aim at*/

    Vent_Target *currentTarget; /*The current attack target*/
    void *trackTarget;

    Timer updateTimer;

} AiAttributes_UnitAttack;

typedef struct
{
    Vent_Unit *targetUnit;

    Vent_Steering steer; /*The unit needs specific steering behaviour for this ai state, this holds the important variables needed by the functions to change it*/

    int moveVariance; /*Used to change the way the unit moves*/

    float maxShootDistance;
    float steeringForce;

    Vent_Game *game;

    Timer updateTimer;

} AiAttributes_UnitAttackUnit;

typedef struct
{
    Timer durationTimer; /*A timer to limit the time spent attacking the area*/
    Vector2DInt areaLocation; /*Location of the area to attack*/
    int attackRangeSqr; /*Maximum attack range for the unit to attack another unit*/

    Vent_Game *game;

    Ai_State *moveState;
    Ai_State *attackUnitState;

    Timer updateTimer;

} AiAttributes_UnitAttackArea;

typedef struct
{
    Timer updateTimer; /*Update timer for detection of selecting a new building to attack*/

    Ai_State *moveState; /*state to enter when a building has been found*/

    void *target; /*Points to the target tile/building which can be destroyed*/
    int targetType;
    int attackRangeSqr;

    Vent_Game *game; /*Current game*/

} AiAttributes_UnitAttackBuilding;


/*
    Ai state functions
*/
Ai_State *aiState_UnitAttack_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game,
    int searchRange,
    int targetFirst
    );

void aiState_UnitAttack_Init(Ai_State *a);

void aiState_UnitAttack_Main(Ai_State *a);

void aiState_UnitAttack_Exit(Ai_State *a);




Ai_State *aiState_UnitAttackUnit_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game,
    Vent_Unit *targetUnit
    );

void aiState_UnitAttackUnit_Init(Ai_State *a);

void aiState_UnitAttackUnit_Main(Ai_State *a);

void aiState_UnitAttackUnit_Exit(Ai_State *a);




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
    );

void aiState_UnitAttackArea_Init(Ai_State *a);

void aiState_UnitAttackArea_Main(Ai_State *a);

void aiState_UnitAttackArea_Exit(Ai_State *a);

Ai_State *aiState_UnitAttackBuilding_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    Vent_Game *game
    );

void aiState_UnitAttackBuilding_Init(Ai_State *a);

void aiState_UnitAttackBuilding_Main(Ai_State *a);

void aiState_UnitAttackBuilding_Exit(Ai_State *a);


#endif
