#ifndef VENTGAME_H
#define VENTGAME_H

#include "Common/List.h"
#include "Time/Timer.h"
#include "Camera.h"

#include "Stats.h"
#include "Sides.h"
#include "Rules.h"
#include "HUD.h"
#include "../Player.h"
#include "../Units/Units.h"
#include "../Level/Level.h"

typedef struct Vent_Game
{
    Vent_Side side[TEAM_END];
    int totalTeams;

    int totalCitadels;

    Vent_Level *level;

    Vent_Stats stats;

    Camera_2D camera;
    Camera_2D camera2;

    Vent_HUD HUD;

    Vent_Player *player; /*Player game information*/

    struct list *units;

    struct list *bullets;

    struct list *buildings;

    struct list  *effects;

    Timer staticTimer;
    Timer gTimer;
    float deltaTime;

    Vent_Unit *playerUnit; /*Unit the player is controlling*/
    int playerInCamera; /*1 if the player collides with the game camera*/

    Vent_Tile *buildTile;

    Vent_Rules rules;

} Vent_Game;

void vGame_Setup(Vent_Game *g, Vent_Player *p);

void vGame_SetupLevel(Vent_Game *g, char *levelName);

int vGame_SoundVolume_Distance(Vent_Game *g, int xOrigin, int yOrigin);

Vent_Unit *vGame_AddUnit(Vent_Game *g, Vent_Unit *u);

void *vGame_AddBuilding(Vent_Game *g, void *building);

void vGame_UpdateUnits(Vent_Game *g, float deltaTime);
void vGame_UpdateSupply(Vent_Game *g);
void vGame_UpdateBullets(Vent_Game *g, float deltaTime);
void vGame_UpdateBuildings(Vent_Game *g, float deltaTime);
void vGame_UpdateTiles(Vent_Game *g, float deltaTime);
void vGame_UpdateSides(Vent_Game *g);

void vGame_DrawEffects(Vent_Game *g);

void vGame_Clean(Vent_Game *g);

#endif
