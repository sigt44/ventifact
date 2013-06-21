#include "Ai_Building.h"

#include "Graphics/Sprite.h"
#include "Graphics/Graphics.h"

#include "AiScan.h"
#include "../Effects.h"

/*
    Function: aiState_BuildingAttribute_Setup

    Description -
    This function sets up the variables for an ai state ment for a building.

    The behaviour of the state is to have the building increase/descrease a certain variable.
    Acting as a supply.

    9 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Building *entity - The building that this ai will control.
    int *attribute - The attribute to change
    int attributeChange - The amount the attribute will change by.
    int attributeMax - The maximum the attribute can be.
    int updateTime - The time between each change of attribute.
    Timer *srcTime - The timer to base the changes on
*/
Ai_State *aiState_BuildingAttribute_Setup(Ai_State *a,
        int group,
        int priority,
        int completeType,
        Vent_Building *entity,
        int *attribute,
        int attributeChange,
        int attributeMax,
        int updateTime,
        Timer *srcTime
)
{
    AiAttributes_BuildingAttribute *aE = (AiAttributes_BuildingAttribute *)mem_Malloc(sizeof(AiAttributes_BuildingAttribute), __LINE__, __FILE__);

    aE->attribute = attribute;
    aE->attributeChange = attributeChange;
    aE->attributeMax = attributeMax;

    aE->updateTimer = timer_Setup(srcTime, 0, updateTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_BuildingAttribute_Init,
                  aiState_BuildingAttribute_Main,
                  aiState_BuildingAttribute_Exit
                  );
    return a;
}

/*
    Function: aiState_BuildingAttribute_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_BuildingAttribute_Init(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingAttribute *aE = a->extraStructure;

    /*printf("%s init attribute change\n", ventBuilding_Names[building->type]);*/

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_BuildingAttribute_Main

    Description -
    Check to see if its time for an attribute to change, if so change it.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_BuildingAttribute_Main(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingAttribute *aE = a->extraStructure;

    timer_Calc(&aE->updateTimer);

    /*Is it time to update the attribute?*/
    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        /*reset the timer*/
        timer_Start(&aE->updateTimer);

        /*update the attribute*/
        *aE->attribute = *aE->attribute + aE->attributeChange;

        /*if the attribute is larger than the maximum, set it to the maximum*/
        if(aE->attributeMax > 0 && *aE->attribute > aE->attributeMax)
        {
            *aE->attribute = aE->attributeMax;
        }
    }

    return;
}

/*
    Function: aiState_BuildingAttribute_Exit

    Description -
    Exit the attribute change state.

    1 argument:
    Ai_State *a - The ai state to exit.
*/
void aiState_BuildingAttribute_Exit(Ai_State *a)
{
    AiAttributes_BuildingAttribute *aE = a->extraStructure;

    aiState_SetComplete(a);

    return;
}

/*
    Function: aiState_BuildingHealPulse_Setup

    Description -
    This function sets up the variables for an ai state ment for a building.

    The behaviour of the state is to heal all units within a certain radius around the building
    every so often.

    10 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Building *entity - The building that this ai will control.
    int radius - The heal radius
    float healAmount - Percentage of health to heal the unit.
    int maxUnits - Max units to be healed each pulse
    int updateTime - Time between each pulse
    Vent_Game *game - The current game.
*/
Ai_State *aiState_BuildingHealPulse_Setup(Ai_State *a,
        int group,
        int priority,
        int completeType,
        Vent_Building *entity,
        int radius,
        float healAmount,
        int maxUnits,
        int updateTime,
        Vent_Game *game
)
{
    AiAttributes_BuildingHealPulse *aE = (AiAttributes_BuildingHealPulse *)mem_Malloc(sizeof(AiAttributes_BuildingHealPulse), __LINE__, __FILE__);

    aE->radius = radius;
    aE->healAmount = healAmount;
    aE->maxUnits = maxUnits;

    aE->game = game;

    aE->updateTimer = timer_Setup(&game->gTimer, 0, updateTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_BuildingHealPulse_Init,
                  aiState_BuildingHealPulse_Main,
                  aiState_BuildingHealPulse_Exit
                  );
   return a;
}

/*
    Function: aiState_BuildingHealPulse_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_BuildingHealPulse_Init(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingHealPulse *aE = a->extraStructure;

    /*printf("%s init heal pulse\n", ventBuilding_Names[building->type]);*/

    aE->radiusSquared = aE->radius * aE->radius;

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_BuildingHealPulse_Main

    Description -
    Check to see if its time to start healing units. Heal them within a certain radius if so.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_BuildingHealPulse_Main(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingHealPulse *aE = a->extraStructure;

    Vent_Effect *effect = NULL;
    Vent_Unit *unit = NULL;
    Vent_Target target;

    int x = 0;

    timer_Calc(&aE->updateTimer);

    /*Is it time to start the heal pulse?*/
    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        /*reset the timer*/
        timer_Start(&aE->updateTimer);

        /*Heal ring effect*/
        effect = vEffect_Insert(&aE->game->effects,
           vEffect_Create(&ve_Sprites.healRing, NULL, building->middlePosition.x, building->middlePosition.y,
                          500, &aE->game->gTimer,
                          &vEffect_DrawMiddle, dataStruct_Create(1, aE->game),
                          ker_Screen())
        );

        sprite_SetAlphaAll(effect->sEffect, SDL_FAST_ALPHA);

        /*Start trying to heal the friendly units*/
        target = vAi_GetUnit_Closest(aE->game->side[building->team].units,
                                     building->middlePosition.x,
                                     building->middlePosition.y,
                                     aE->radiusSquared,
                                     1);
        unit = target.entity;

        while(unit != NULL && x != aE->maxUnits)
        {
            /*If the unit needs to be healed*/
            if(unit->health < unit->healthStarting)
            {
                unit->health += (int)(aE->healAmount * (float)unit->healthStarting);

                /*Dont heal over the limit*/
                if(unit->health > unit->healthStarting)
                {
                    unit->health = unit->healthStarting;
                }

                /*Create the heal effect*/
                vEffect_Insert(&aE->game->effects,
                   vEffect_Create(&ve_Sprites.heal, NULL, unit->middlePosition.x, unit->middlePosition.y,
                                  400, &aE->game->gTimer,
                                  &vEffect_DrawStandard, dataStruct_Create(1, aE->game),
                                  ker_Screen())
                );

                x++;
            }

            target = vAi_GetUnit_Closest(aE->game->side[building->team].units,
                                 building->middlePosition.x,
                                 building->middlePosition.y,
                                 aE->radiusSquared,
                                 0);
            unit = target.entity;
        }

        vAi_GetUnit_Closest(NULL, 0, 0, 0, -1);
    }

    return;
}

/*
    Function: aiState_BuildingHealPulse_Exit

    Description -
    Exit the attribute change state.

    1 argument:
    Ai_State *a - The ai state to exit.
*/
void aiState_BuildingHealPulse_Exit(Ai_State *a)
{
    AiAttributes_BuildingHealPulse *aE = a->extraStructure;

    aiState_SetComplete(a);

    return;
}

/*
    Function: aiState_BuildingRepair_Setup

    Description -
    This function sets up the variables for an ai state ment for a building.

    The behaviour of the state is to repair itself after a certain amount of time without
    being damaged. The building must be linked to a citadel for it to repair.

    11 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Building *entity - The building that this ai will control.
    int minHealth - At what percentage of health should the building start checking to repair itself.
    int maxHealth - What is the maximum percentage of health the building should repair itself to.
    int repairAmount - How much should be repaired each time.
    int repairTime - Time between each repair.
    int updateTime - Time between each health check.
    Vent_Game *game - The game structure.
*/
Ai_State *aiState_BuildingRepair_Setup(Ai_State *a,
        int group,
        int priority,
        int completeType,
        Vent_Building *entity,
        int minHealth,
        int maxHealth,
        int repairAmount,
        int repairTime,
        int updateTime,
        Vent_Game *game
)
{
    AiAttributes_BuildingRepair *aE = (AiAttributes_BuildingRepair *)mem_Malloc(sizeof(AiAttributes_BuildingRepair), __LINE__, __FILE__);

    aE->repairAmount = repairAmount;
    aE->minHealth = minHealth;
    aE->maxHealth = maxHealth;

    aE->game = game;

    aE->repairTimer = timer_Setup(&game->gTimer, 0, repairTime, 0);
    aE->updateTimer = timer_Setup(&game->gTimer, 0, updateTime, 0);

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_BuildingRepair_Init,
                  aiState_BuildingRepair_Main,
                  aiState_BuildingRepair_Exit
                  );
   return a;
}

/*
    Function: aiState_BuildingRepair_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_BuildingRepair_Init(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingRepair *aE = a->extraStructure;

    /*printf("%s init repair\n", ventBuilding_Names[building->type]);*/

    aE->repairing = 0;

    timer_Start(&aE->updateTimer);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_BuildingRepair_Main

    Description -
    Check to see if its time to start repairing itself.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_BuildingRepair_Main(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingRepair *aE = a->extraStructure;

    Sprite *eSprite = NULL;

    int currentPercentage = 0;

    /*If the building has been damaged*/
    if(vBuilding_IsDamaged(building, 0))
    {
        /*Reset the timer since the building is still being damaged*/
        timer_Start(&aE->updateTimer);

        return;
    }

    /*If the building has no citadel linked*/
    if(building->onTile != NULL && building->onTile->citadel == NULL)
    {
        return;
    }

    timer_Calc(&aE->updateTimer);

    /*If its time to check the health of the building, and the building is not repairing already*/
    if(timer_Get_Remain(&aE->updateTimer) <= 0 && aE->repairing == 0)
    {
        currentPercentage = (int)(100 * building->health/(float)building->healthStarting);

        /*The health of the building is low enough to start repairing*/
        if(currentPercentage <= aE->minHealth)
        {
            aE->repairing = 1;
            timer_Start(&aE->repairTimer);
        }

        timer_Start(&aE->updateTimer);
    }
    else if(aE->repairing == 1)
    {
        timer_Calc(&aE->repairTimer);

        /*If its time to repair the building*/
        if(timer_Get_Remain(&aE->repairTimer) <= 0)
        {
            /*Check if the building still requires repairs*/
            currentPercentage = (int)(100 * building->health/(float)building->healthStarting);

            if(currentPercentage < aE->maxHealth)
            {
                building->health += aE->repairAmount;

                /*Make sure the health does not go over the limit*/
                if(building->health > building->healthStarting)
                {
                    building->health = building->healthStarting;
                }

                /*Add in the repair effect*/
                vEffect_Insert(&aE->game->effects,
                           vEffect_Create(&ve_Sprites.repair, NULL, building->middlePosition.x - (sprite_Width(&ve_Sprites.repair)/2), building->middlePosition.y - (sprite_Height(&ve_Sprites.repair)/2),
                                          800, &aE->game->gTimer,
                                          &vEffect_DrawStandard, dataStruct_Create(1, aE->game),
                                          ker_Screen())
                );

                timer_Start(&aE->repairTimer);
            }
            else
            {
                aE->repairing = 0;

                timer_Start(&aE->updateTimer);
            }
        }
    }


    return;
}

/*
    Function: aiState_BuildingRepair_Exit

    Description -
    Exit the attribute change state.

    1 argument:
    Ai_State *a - The ai state to exit.
*/
void aiState_BuildingRepair_Exit(Ai_State *a)
{
    AiAttributes_BuildingRepair *aE = a->extraStructure;

    aiState_SetComplete(a);

    return;
}

/*
    Function: aiState_BuildingIntro_Setup

    Description -
    This function sets up the variables for an ai state ment for a building.

    The behaviour of the state is to make the building transparent for
    a certain amount of time. If there is a unit on the tile it will remain
    transparent until it has gone. (Unless ignore units is set to 1).

    7 arguments:
    Ai_State *a - The ai state to setup
    int group - The group ID of the ai state.
    int priority - The priority of this state.
    int completeType - The completion type of this state.
    Vent_Building *entity - The building that this ai will control.
    int introLength - How long should the building stay transparent.
    int ignoreUnits - Should it ignore any units on the tile, 1 if so.
    Timer *srcTime - Source timer for the transparency time.
*/
Ai_State *aiState_BuildingIntro_Setup(Ai_State *a,
        int group,
        int priority,
        int completeType,
        Vent_Building *entity,
        int introLength,
        int ignoreUnits,
        Timer *srcTime
)
{
    AiAttributes_BuildingIntro *aE = (AiAttributes_BuildingIntro *)mem_Malloc(sizeof(AiAttributes_BuildingIntro), __LINE__, __FILE__);

    aE->updateTimer = timer_Setup(srcTime, 0, introLength, 0);

    aE->ignoreUnits = ignoreUnits;

    aiState_Setup(a,
                  group,
                  priority,
                  completeType,
                  entity,
                  aE,
                  aiState_BuildingIntro_Init,
                  aiState_BuildingIntro_Main,
                  aiState_BuildingIntro_Exit
                  );
   return a;
}

/*
    Function: aiState_BuildingIntro_Init

    Description -
    Initialises the ai state so that it is ready to be updated by the main loop.

    1 argument:
    Ai_State *a - The ai state to initialise.
*/
void aiState_BuildingIntro_Init(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingIntro *aE = a->extraStructure;

    /*printf("%s init intro\n", ventBuilding_Names[building->type]);*/

    timer_Start(&aE->updateTimer);

    /*Set the building to transparent*/
    sprite_SetAlpha(&building->sBuilding, SDL_FAST_ALPHA);

    aiState_Init(a);

    return;
}

/*
    Function: aiState_BuildingIntro_Main

    Description -
    Set new building to being transparent for a time or until there are no
    units on its tile.

    1 argument:
    Ai_State *a - The ai state to update.
*/
void aiState_BuildingIntro_Main(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingIntro *aE = a->extraStructure;

    timer_Calc(&aE->updateTimer);

    /*If there are units on the tile (and they are not to be ignored) then exit early*/
    /*if(building->team == TEAM_PLAYER)
        printf("units on %d\n", building->onTile->unitsOn);*/

    if(aE->ignoreUnits == 0 && building->onTile->unitsOn > 0)
    {
        return;
    }

    /*If the time is up*/
    if(timer_Get_Remain(&aE->updateTimer) <= 0)
    {
        a->update = a->exit;

        return;
    }

    return;
}

/*
    Function: aiState_BuildingIntro_Exit

    Description -
    Exit the building into state.

    1 argument:
    Ai_State *a - The ai state to exit.
*/
void aiState_BuildingIntro_Exit(Ai_State *a)
{
    Vent_Building *building = a->entity;
    AiAttributes_BuildingIntro *aE = a->extraStructure;

    if(aiState_SetComplete(a) == 0)
        return;

    /*Set the building back to being opaque*/
    sprite_SetAlpha(&building->sBuilding, SDL_ALPHA_OPAQUE);

    return;
}
