#ifndef AI_UNITFUNCTION_H
#define AI_UNITFUNCTION_H

#include "Common/Fuzzy/FuzzyLV.h"
#include "Common/Fuzzy/FuzzyManager.h"

#include "../Ai_State.h"
#include "../Ai_Side.h"
/*Ai_Side for fuzzy set enum*/

#include "../../Units/Units.h"
#include "../../Game/VentGame.h"
#include "../../Buildings/Buildings.h"
#include "Time/Timer.h"

typedef struct
{
    int maxTries; /*The maximum amount of times the unit will try to purchase the citadel*/

    Vent_Level *level; /*The level the unit is in.*/
    Vent_Game *game; /*The game the unit is in.*/

    Ai_State *moveState;/*Used to track the ai state that moves the unit to the citadel*/

    void *citadelTile; /*The tile the unit will try and build a citadel on*/

} AiAttributes_UnitBuyCitadel;

typedef struct
{
    Timer invulTimer; /*The timer to count for how long the unit should be invulnerable*/
    Timer cooldownTimer; /*How long should the time be before the unit can go invulnerable again*/

} AiAttributes_UnitPhase;

typedef struct
{
    void *target; /*Points to the target entity to defend*/

    int targetType; /* ENTITY_UNIT or ENTITY_BUILDING */
    int follow; /*If true then the unit will follow its target to defend*/

    int targetHealth; /*Keeps track of the entity's health*/

    Ai_State *attackArea; /*Ai state to attack area around the target.*/
    Ai_State *idle; /*Ai state to wander/follow unit at location when nothing to attack.*/

    Vent_Level *level; /*The current level the unit is in*/
    Vent_Game *game; /*The current game*/

    int xDis; /*X axis distance to defend target*/
    int yDis; /*Y axis distance to defend target*/
    int targetDistanceSqr; /*Distance squared to defend target*/

    Timer updateTimer;

} AiAttributes_UnitDefendEntity;

typedef struct
{
    int *aiFrequency; /*Used to keep track of a certain ai type that this state is associated with(By group/Priority)*/

    Vent_Side *side; /*The side the unit is on*/

    int waitForGroup; /*If (1) then dont exit until AI_COMPLETE_GROUP is called on the state*/

} AiAttributes_UnitReportAi;

typedef struct
{
    Timer updateTimer; /*Time between the weapon choose check*/

    Fuzzy_LV ammoAmount; /*How much ammo is there*/
    float input_Ammo;

    Fuzzy_LV targetDistanceSqr; /*How far away is the targeted unit*/
    float input_TargetDistanceSqr;

    Fuzzy_LV dpsDifference; /*Does the weapon have a greater damage per second amount*/
    float input_DpsDifference;

    Fuzzy_LV weaponWeight[VENT_TOTALWEAPONS]; /*How desirable is the weapon*/

    Fuzzy_Ruleset bestWeapon[VENT_TOTALWEAPONS]; /*Find the most desirable weapon*/

} AiAttributes_UnitSelectWeapon;

typedef struct
{
    Timer updateTimer; /*Time between the each target check*/
    int range; /*Distance to obtain target*/
    int rangeSqr;

    int targetPriority; /*Entity type which will be targeted first*/

    Vent_Game *game;

} AiAttributes_UnitObtainTargets;



Ai_State *aiState_UnitObtainTargets_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int range,
    int updateTime,
    Vent_Game *game
    );


int aiState_UnitObtainTargets_Units(struct list *units, Vent_Unit *unit, Vent_Target *targetArray, int maxRangeSqr, int maxTargets);

int aiState_UnitObtainTargets_Buildings(struct list *buildings, Vent_Unit *unit, Vent_Target *targetArray, int maxRangeSqr, int maxTargets);

int aiState_UnitObtainTargets_Tiles(struct list *tiles, int tileTeam, Vent_Unit *unit, Vent_Target *targetArray, int maxRangeSqr, int maxTargets);


void aiState_UnitObtainTargets_Init(Ai_State *a);

void aiState_UnitObtainTargets_Main(Ai_State *a);

void aiState_UnitObtainTargets_Exit(Ai_State *a);



Ai_State *aiState_UnitBuyCitadel_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int maxTries,
    Vent_Level *level,
    Vent_Game *game
    );

void aiState_UnitBuyCitadel_Init(Ai_State *a);

void aiState_UnitBuyCitadel_Main(Ai_State *a);

void aiState_UnitBuyCitadel_Exit(Ai_State *a);



Ai_State *aiState_UnitPhase_Setup(Ai_State *a,
    int group,              /*group*/
    int priority,           /*priority*/
    int completeType,       /*complete type*/
    Vent_Unit *entity,      /*entity*/
    int invulTime,          /*Invulnerability time*/
    int cooldownTime,       /*Cooldown time*/
    Timer *srcTime          /*Source timer*/
    );

void aiState_UnitPhase_Init(Ai_State *a);

void aiState_UnitPhase_Main(Ai_State *a);

void aiState_UnitPhase_Exit(Ai_State *a);



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
    );

void aiState_UnitDefendEntity_Init(Ai_State *a);

void aiState_UnitDefendEntity_Main(Ai_State *a);

void aiState_UnitDefendEntity_Exit(Ai_State *a);



Ai_State *aiState_UnitReportAi_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int *aiFrequency,
    Vent_Side *side,
    int waitForGroup
    );

void aiState_UnitReportAi_Init(Ai_State *a);

void aiState_UnitReportAi_Main(Ai_State *a);

void aiState_UnitReportAi_Exit(Ai_State *a);

Ai_State *aiState_UnitSelectWeapon_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Unit *entity,
    int fireTime,
    int updateTime,
    Timer *srcTime
    );

void aiState_UnitSelectWeapon_Init(Ai_State *a);

void aiState_UnitSelectWeapon_Main(Ai_State *a);

void aiState_UnitSelectWeapon_Exit(Ai_State *a);

#endif
