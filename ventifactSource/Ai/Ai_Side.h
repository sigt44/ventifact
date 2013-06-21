#ifndef AI_SIDE_H
#define AI_SIDE_H


#include "Common/Fuzzy/FuzzyLV.h"
#include "Common/Fuzzy/FuzzyManager.h"

#include "Ai.h"
#include "Ai_State.h"
#include "../Game/Sides.h"
#include "../Game/VentGame.h"
#include "../Level/Level.h"

enum
{
    FS_V_LOW_AMOUNT = 0,
    FS_LOW_AMOUNT = 1,
    FS_MEDIUM_AMOUNT = 2,
    FS_HIGH_AMOUNT = 3,
    FS_V_HIGH_AMOUNT = 4,

    FS_DESIRE_LOW = 5,
    FS_DESIRE_MEDIUM = 6,
    FS_DESIRE_HIGH = 7

} FS_SETMEMBERS;

typedef struct
{
    Fuzzy_LV unitCost; /*How pricey is the unit*/
    float input_UnitCost;

    Fuzzy_LV unitAmount; /*How many units in the game are there*/
    float input_UnitAmount;

    Fuzzy_LV unitDestroyed; /*How many units have been destroyed already*/
    float input_UnitDestroyed;

    Fuzzy_LV unitEnemyAmount; /*How many units of type does the enemy have*/
    float input_UnitEnemyAmount;

    Fuzzy_LV unitPurchase[UNIT_ENDTYPE]; /*How desirable is the unit*/

    Fuzzy_Ruleset bestUnit[UNIT_ENDTYPE]; /*Find the most desirable unit*/

    Timer updateTimer; /*Time between each check*/

    Vent_Game *game; /*Current game*/

} AiAttributes_SidePurchaseUnit;

typedef struct
{
    Fuzzy_LV unitChosen; /*Which unit is chosen to be the most desirable*/
    float input_UnitChosen;

    Fuzzy_LV creditAmount; /*How much credits has the side got*/
    float input_CreditAmount;

    Fuzzy_LV powerStations; /*How many power stations are there*/
    float input_PowerStations;

    Fuzzy_LV buildingDifference;  /*The difference between the amount of building(type) and the building that is built the most*/
    float input_BuildingDifference;

    Fuzzy_LV buildingPurchase[BUILDING_TOTAL-1]; /*How desirable is each building*/

    Fuzzy_Ruleset bestBuilding[BUILDING_TOTAL-1]; /*Which is the most desirable building*/

    Timer updateTimer; /*Time between each check*/

    Vent_Game *game; /*Current game*/

} AiAttributes_SidePurchaseBuilding;


enum
{
    UAI_ATTACKPLAYER = 0,
    UAI_ATTACKBUILDING = 1,
    UAI_DEFENDAREA = 2,
    UAI_ATTACKUNIT = 3,
    UAI_BUYCITADEL = 4,
    UAI_TOTALAI = 5
} SIDE_AI_TYPES;

typedef struct
{
    Fuzzy_LV unitCloseToCitadel; /*Fuzzy logic variable to determine how many units are close to a citadel*/
    float input_UnitCloseToCitadel; /*Input for FLV*/

    Fuzzy_LV aiAmount; /*How many units have this type of ai*/
    float input_AiAmount;

    int totalAi[UAI_TOTALAI]; /*Keeps track of the amount of units performing a certain ai*/
    int totalJudgeAi[UAI_TOTALAI]; /*This will be used by the ai to judge how many units are currently performing an ai, it will not be the exact amount.*/

    Fuzzy_LV emptyCitadel; /*How many empty citadels are there*/
    float input_EmptyCitadel;

    Fuzzy_LV aiWeight[UAI_TOTALAI]; /*How much of a good decision is it to have this ai*/

    Fuzzy_Ruleset bestAi[UAI_TOTALAI]; /*Find the best ai to give a unit*/

    Timer updateTimer; /*Time between each check*/

    Vent_Game *game; /*Current game*/

} AiAttributes_SideGiveAiUnit;

Ai_State *aiState_SidePurchaseUnit_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Side *entity,
    int updateTime,
    Vent_Game *game
    );

void aiState_SidePurchaseUnit_Init(Ai_State *a);

void aiState_SidePurchaseUnit_Main(Ai_State *a);

void aiState_SidePurchaseUnit_Exit(Ai_State *a);

Ai_State *aiState_SideGiveAiUnit_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Side *entity,
    int updateTime,
    Vent_Game *game
    );

void aiState_SideGiveAiUnit_Init(Ai_State *a);

void aiState_SideGiveAiUnit_Main(Ai_State *a);

void aiState_SideGiveAiUnit_Exit(Ai_State *a);

Ai_State *aiState_SidePurchaseBuilding_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    Vent_Side *entity,
    int updateTime,
    Vent_Game *game
    );

void aiState_SidePurchaseBuilding_Init(Ai_State *a);

void aiState_SidePurchaseBuilding_Main(Ai_State *a);

void aiState_SidePurchaseBuilding_Exit(Ai_State *a);

#endif
