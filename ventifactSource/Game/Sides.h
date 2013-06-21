#ifndef SIDES_H
#define SIDES_H

#include "../Defines.h"
#include "../Units/Attributes.h"
#include "../Ai/Ai.h"
#include "../Ai/Ai_State.h"

typedef struct Vent_Side {

    char name[32];

    int captures; /*The amount of citadels owned*/

    int credits;
    int creditsSpent;
    int lastFrameCredits;

    int totalUnits[UNIT_ENDTYPE]; /*Units in the game*/
    int readyUnits[UNIT_ENDTYPE]; /*Units ready to be bought*/
    int lostUnits[UNIT_ENDTYPE]; /*Units lost to enemy*/
    int killedUnits[UNIT_ENDTYPE]; /*Enemy units destroyed*/

    Vent_Unit_Description unitDesc[UNIT_ENDTYPE]; /*Price and name of units*/

    int totalBuildings[BUILDING_TOTAL];

    Vent_Attributes unitAttributes[UNIT_ENDTYPE];
    Vent_Attributes playerAttributes;

    int team;

    int wantedUnit; /*Most wanted unit to buy*/

    Ai_Module ai;

    struct list *units; /*List of units owned*/

    struct list *unitsNeedAi; /*List of units that need to be given an ai state*/

    struct list *buildings; /*List of buildings owned*/

    struct list *citadels; /*List of citadel buildings owned*/

    struct Vent_Side *ally;
    struct Vent_Side *enemy;

} Vent_Side;

void vSide_Setup(Vent_Side *s, int startCredits, unsigned int team);

void vSide_SetupAttributes(Vent_Side *s, int playerLvl, int tankLvl, int infantryLvl, int aircraftLvl, int towerLvl);

void vSide_SetupAi(Vent_Side *s, int aiType, void *ventGame);

void vSide_SetAlly(Vent_Side *s, Vent_Side *a);

void vSide_SetEnemy(Vent_Side *s, Vent_Side *e);

void vSide_Clean(Vent_Side *s);

#endif
