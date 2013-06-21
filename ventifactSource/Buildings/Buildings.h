#ifndef BUILDINGS_H
#define BUILDINGS_H

#include "../Level/Tiles.h"
#include "../Ai/Ai.h"
#include "../Game/VentGame.h"
#include "../Units/Units.h"
#include "../HealthBar.h"

typedef struct Vent_Building
{
    Sprite sBuilding;

    Vector2DInt position;
    Vector2DInt middlePosition;
    int radius;

    int health;
    int healthStarting;
    int healthPrevious;
    int destroyed;

    int width;
    int height;

    Vent_Tile *onTile;

    struct list *tileList; /*Points to a list of all the tiles in a level*/

    Ai_Module ai;

    int type;
    int team;

    Timer *srcTime;

    Dependency_Watcher depWatcher; /*Any lists that are using this building, so that they can be ammended when it is destroyed*/

    Vent_HealthBar healthBar;

} Vent_Building;

typedef struct Vent_Building_Description
{
    char *name;

    int price;
} Vent_Building_Description;

extern char *ventBuilding_Names[BUILDING_TOTAL];

extern int ventBuilding_Prices[BUILDING_TOTAL];

int vBuilding_Buy(Vent_Tile *buildTile, int buildingType, Vent_Side *side);

void vBuilding_GiveCitadelProp(Vent_Building *b);
void vBuilding_GivePowerProp(Vent_Building *b);
void vBuilding_GiveTankProp(Vent_Building *b);
void vBuilding_GiveInfantryProp(Vent_Building *b);
void vBuilding_GiveAirProp(Vent_Building *b);

void vBuilding_GiveProperties(Vent_Building *b, int type);

void vBuilding_GiveAi(Vent_Building *b, Vent_Game *game);

Vent_Building *vBuilding_Create(int x, int y, Timer *srcTime, Vent_Tile *onTile, int type, int team, struct list *tileList);

void vBuilding_Setup(Vent_Building *b, int x, int y, Timer *srcTime, Vent_Tile *onTile, int type, int team, struct list *tileList);

void vBuilding_Draw(Vent_Building *b, Camera_2D *c, SDL_Surface *destination);

int vBuilding_IsDamaged(Vent_Building *building, int update);

int vBuilding_IsDestroyed(Vent_Building *building, Vent_Game *game);

int vBuilding_HasCitadel(Vent_Building *building);

Vent_Tile *vBuilding_GetFreeTile(Vent_Building *citadel);

int vBuilding_Build(int team, int buildingType, Vent_Tile *buildTile, Vent_Game *g);

int vBuilding_CheckBuy(int chosenBuilding, int team, Vent_Game *game, Vent_Tile *t);

int vBuilding_CanBuild(Vent_Unit *u, Vent_Game *g);

void vBuilding_Clean(Vent_Building *b, Vent_Game *g);

#endif
