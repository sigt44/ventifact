#ifndef SUPPLY_H
#define SUPPLY_H

#include "Time/Timer.h"
#include "Common/List.h"

#include "Units.h"
#include "../Game/VentGame.h"

enum
{
    SUPPLY_AI_NONE = 0,
    SUPPLY_AI_HOLD = 1,
    SUPPLY_AI_ATTACKPLAYER = 2,
    SUPPLY_AI_ATTACKBUILDING = 3,
    SUPPLY_AI_PATROL = 4,
    SUPPLY_AI_DEFENDPLAYER = 5,
    SUPPLY_AI_SIDE = 6,
    SUPPLY_AI_TOTAL = 7

} SUPPLY_AI;

Data_Struct *vSupply_AiToData_AttackPlayer(void);
Data_Struct *vSupply_AiToData_AttackBuilding(void);
Data_Struct *vSupply_AiToData_DefendPlayer(void);
Data_Struct *vSupply_AiToData_Side(void);
Data_Struct *vSupply_AiToData_Hold(int originX, int originY, int rangeSqr);
Data_Struct *vSupply_AiToData_Patrol(int loopType, struct list *patrolList);


typedef struct Vent_Supply
{
    Timer supplyTime;

    int respawnAmount;
    int finished;

	int deathWait;
	int startSpawned;

    int team;
    int type;

    int level;

    int oX;
    int oY;

    Vent_Unit *sponserUnit;

    Vent_Game *game;

    int aiState;
    Data_Struct *aiData; /*Hold the data for the specific ai state the supply will give*/

} Vent_Supply;

Data_Struct *vSupply_GetDefaultAiData(int aiState);

Vent_Supply *vSupply_Create(Vent_Game *game, Timer *srcTime, int amount, int respawnTime, int startSpawned, int waitForDeath, int team, int type, int xLoc, int yLoc, int level, int aiState, Data_Struct *aiData);

int vSupply_CreateUnit(Vent_Supply *s);

void vSupply_Setup(Vent_Supply *s, Vent_Game *game, Timer *srcTime, int amount, int respawnTime, int startSpawned, int waitForDeath, int team, int type, int xLoc, int yLoc, int level, int aiState, Data_Struct *aiData);

/*Check if a unit needs to be spawned and spawn it if so*/
int vSupply_Update(Vent_Supply *s);

/*Check if the supply has finished, if free == 1 then delete it */
int vSupply_CheckFinished(Vent_Supply *s, int free);

Vent_Supply *vSupply_GetCollision(struct list *supplyList, int colX, int colY, int colW, int colH);

Data_Struct *vSupply_CopyAi(Vent_Supply *s);

void vSupply_Copy(Vent_Supply *copyTo, Vent_Supply *copy);

Vent_Supply *vSupply_Clone(Vent_Supply *copy);

void vSupply_Draw(Vent_Supply *s, Camera_2D *c, SDL_Surface *destination);

void vSupply_Save(Vent_Supply *s, FILE *save);

Vent_Supply *vSupply_Load(Vent_Game *game, Timer *srcTime, FILE *load);

void vSupply_Clean(Vent_Supply *s);

#endif
