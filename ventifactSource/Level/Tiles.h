#ifndef TILES_H
#define TILES_H

#include "Graphics/Sprite.h"
#include "Time/Timer.h"
#include "Common/List.h"
#include "Vector2DInt.h"
#include "Camera.h"

#include "Level.h"
#include "../Defines.h"
#include "../Units/Units.h"
#include "../HealthBar.h"

enum VENT_TILE_STATE
{
    TILESTATE_IDLE = 1,
    TILESTATE_BUILDABLE = 2,
    TILESTATE_DESTROYABLE = 4,
    TILESTATE_IMPORTANT = 8
};

extern char *vTile_Names[TILE_ENDTYPE];

typedef struct Vent_TileBase
{
    Vector2DInt position;
    unsigned int width;
    unsigned int height;
    int rotationDeg;

    unsigned int collision;

    int type;

    int group;

    int healthStarting;

    int layer;

    int team;

    unsigned int stateFlags;

    int graphicType; /*Negative values mean that the graphic is not preloaded.*/

} Vent_TileBase;

typedef struct Vent_Tile
{
    Vent_TileBase base;
    int radius;

    struct list *groupList;
    struct list *inSectors;
    int numSector;

    int unitsOn;

    int health;
    int healthPrev;
    int destroyed;
    Vent_HealthBar *healthBar;

    Sprite *sImage;
    int rotation;

    unsigned int drawCalled;

    void *citadel;
    void *building;

    Dependency_Watcher depWatcher;

} Vent_Tile;

Vent_Tile *vTile_Create(int x, int y, int type, int graphicType, int collision, int layer, int team, int health, int state, int group, int rotationDeg, Sprite *sprite);

void vTile_Setup(Vent_Tile *t, int x, int y, int type, int graphicType, int collision, int layer, int team, int health, int state, int group, int rotationDeg, Sprite *sprite);

void vTile_Init(Vent_Tile *t);

void vTile_Copy(int cleanDest, Vent_Tile *tileDest, Vent_Tile *tileSource);

void vTile_ChangeGraphic(Vent_Tile *t, Sprite *graphic);

void vTile_ChangeGraphicSurface(Vent_Tile *t, struct list *lvlSurfaces, int graphicID);

void vTile_SetRotation(Vent_Tile *t, struct list *lvlSurfaces, int rotationDeg);

Sprite *vTile_CreateSprite(struct list *surfaceList, int layer, Timer *srcTime, int graphicID, int rotationDeg);



void vTile_Draw(Vent_Tile *t, Camera_2D *c, SDL_Surface *dest);

void vTile_Clean(Vent_Tile *t);

void vTile_Save(Vent_Tile *t, struct list *lSurfaces, FILE *save);

int vTile_CheckCollision(Vent_Tile *t, Vent_Unit *u);

int vTile_IsDestroyed(Vent_Tile *tile, void *g);

void vTile_GroupUp(struct list *tileList);

Vent_Tile *vTile_Load(struct list **lSurfaces, Timer *srcTime,  FILE *save);

void vTile_Reset(Vent_Tile *t);

#endif
