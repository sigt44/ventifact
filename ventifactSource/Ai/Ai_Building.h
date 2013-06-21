#ifndef AI_BUILDING_H
#define AI_BUILDING_H

#include "Time/Timer.h"

#include "Ai_State.h"
#include "../Buildings/Buildings.h"

typedef struct
{
    int *attribute; /*The attribute the building will change*/
    int attributeChange; /*How much will the attribute change by*/
    int attributeMax; /*What is the max it can change to*/

    Timer updateTimer; /*Time between each change*/

} AiAttributes_BuildingAttribute;

typedef struct
{
    int radius; /*How large is the heal radius*/
    int radiusSquared;

    float healAmount; /*How much %(starting health) to heal each unit by*/
    int maxUnits; /*Maximum amount of units to heal*/

    Timer updateTimer; /*Time between each heal pulse*/
    Vent_Game *game; /*Current game*/

} AiAttributes_BuildingHealPulse;

typedef struct
{
    int minHealth;      /*min health % that the building should repair itself at*/
    int maxHealth;      /*max health % that the building should repair itselt to*/
    int repairAmount;   /*repair amount each tick*/

    int repairing;      /*whether the building is currently repairing or not*/

    Timer repairTimer; /*Time between each repair*/
    Timer updateTimer; /*Time between each repair check*/

    Vent_Game *game; /*Current game*/

} AiAttributes_BuildingRepair;

typedef struct
{
    int ignoreUnits; /*If 0 then the building will remain transparent so long as there is a unit on its tile*/
    Timer updateTimer; /*Timer to hold the transparency length*/

} AiAttributes_BuildingIntro;

Ai_State *aiState_BuildingAttribute_Setup(Ai_State *a,
        int group,              /*group*/
        int priority,           /*priority*/
        int completeType,       /*complete type*/
        Vent_Building *entity,  /*entity*/
        int *attribute,    /*attribute to change*/
        int attributeChange,    /*amount to change*/
        int attributeMax,       /*max the attribute can be*/
        int updateTime,          /*time between each change*/
        Timer *srcTime          /*timer which the update is based on*/
);

void aiState_BuildingAttribute_Init(Ai_State *a);

void aiState_BuildingAttribute_Main(Ai_State *a);

void aiState_BuildingAttribute_Exit(Ai_State *a);

Ai_State *aiState_BuildingHealPulse_Setup(Ai_State *a,
        int group,              /*group*/
        int priority,           /*priority*/
        int completeType,       /*complete type*/
        Vent_Building *entity,  /*entity*/
        int radius,               /*heal radius*/
        float healAmount,         /*how much % to heal each unit*/
        int maxUnits,            /*max units to be healed each pulse*/
        int updateTime,          /*time between each pulse*/
        Vent_Game *game         /*current game*/
);

void aiState_BuildingHealPulse_Init(Ai_State *a);

void aiState_BuildingHealPulse_Main(Ai_State *a);

void aiState_BuildingHealPulse_Exit(Ai_State *a);

Ai_State *aiState_BuildingRepair_Setup(Ai_State *a,
        int group,              /*group*/
        int priority,           /*priority*/
        int completeType,       /*complete type*/
        Vent_Building *entity,  /*entity*/
        int minHealth,          /*min health %*/
        int maxHealth,          /*max health to heal to %*/
        int repairAmount,       /*repair amount each time tick*/
        int repairTime,         /*delay between each repair*/
        int updateTime,         /*time between each check for repair*/
        Vent_Game *game         /*current game*/
);

void aiState_BuildingRepair_Init(Ai_State *a);

void aiState_BuildingRepair_Main(Ai_State *a);

void aiState_BuildingRepair_Exit(Ai_State *a);

Ai_State *aiState_BuildingIntro_Setup(Ai_State *a,
        int group,              /*group*/
        int priority,           /*priority*/
        int completeType,       /*complete type*/
        Vent_Building *entity,  /*entity*/
        int introLength,        /*Length of transparency*/
        int ignoreUnits,        /*Should units effect the length of transparency, 1 if not*/
        Timer *srcTime          /*Source timer for the transparency time*/
);

void aiState_BuildingIntro_Init(Ai_State *a);

void aiState_BuildingIntro_Main(Ai_State *a);

void aiState_BuildingIntro_Exit(Ai_State *a);

#endif
