#ifndef LEVEL_H
#define LEVEL_H

#include <SDL/SDL.h>

#include "Time/Timer.h"
#include "Common/List.h"
#include "Graphs/Graph.h"
#include "Graphs/Search/DijkstraSearch.h"
#include "Vector2DInt.h"
#include "Camera.h"

#include "../Defines.h"
#include "../Images.h"
#include "Sector.h"

enum LEVEL_PLAYER_UNIT
{
    VLEVEL_FORCE_ANY = -1,
    VLEVEL_FORCE_TANK = UNIT_TANK,
    VLEVEL_FORCE_INFANTRY = UNIT_INFANTRY,
    VLEVEL_FORCE_AIR = UNIT_AIR,
    VLEVEL_FORCE_TOWER = UNIT_TOWER
};

typedef struct Vent_LevelSurface
{
    char name[255];

    /*Each node contains the surface at a different angle*/
    struct list *surfaceAngleList;
    int numRotated;

    SDL_Surface *surface;

    int ID;

} Vent_LevelSurface;

typedef struct Vent_Level_Header
{
    char name[21];

    int enemyUnitLevel[UNIT_ENDTYPE];

    int forcedPlayerUnit;

    int width;
    int height;

    int numSides;
    int numTiles;
    int numSupply;
    int numCustomSurfaces;

    int baseTileID;

    Vector2DInt startLoc[TEAM_END];
} Vent_Level_Header;


typedef struct Vent_Level
{
    Vent_Level_Header header;

    SDL_Surface *ground;

    struct list *tiles;

    struct list *importantTiles;

    struct list *destroyableTiles;

    struct list *sectors;

    struct list *unitSupply;

    struct list *surfacesLoaded;

    B_Graph pathGraph;

    D_GraphSearch pathSearch;

    Timer *srcTime;

} Vent_Level;

Vent_LevelSurface *vLevelSurface_Create(char *fullPath, SDL_Surface *surface, int ID);

void vLevel_SetupHeader(Vent_Level_Header *header, char *name, int width, int height);

void vLevel_Setup(Vent_Level *l, char *name, int width, int height);

void vLevel_SetupGround(Vent_Level *l, SDL_Surface *ground);

SDL_Surface *vLevel_SurfaceRetrieve(struct list *s, int type, int rotationDeg);

SDL_Surface *vLevel_SurfaceRetrieveRotation(Vent_LevelSurface *lvlSurface, int rotationDeg);

int vLevel_SurfaceNewID(struct list *s);

void vLevel_SurfaceInsert(struct list **s, char *fullPath, int graphicType);

void vLevel_SurfaceInsertRotation(struct list *s, int graphicID, int rotationDeg);

int vLevel_SurfaceRemove(struct list **surfaceList, int graphicType);

int vLevel_SurfaceDisable(struct list **surfaceList, int graphicType);

void vLevel_SurfaceClean(Vent_LevelSurface *lvlSurface);

void *vLevel_AddTile(Vent_Level *l, void *tile);

void vLevel_RemoveTile(Vent_Level *l, void *t);

void vLevel_AddSupply(Vent_Level *l, void *s);

void vLevel_RemoveSupply(Vent_Level *l, void *s);

void vLevel_Draw(Vent_Level *l, Vent_Sector *sectorOrigin, Camera_2D *c, SDL_Surface *dest);

void vLevel_DrawGround(Vent_Level *l, Camera_2D *c, SDL_Surface *dest);

void vLevel_Update(Vent_Level *l, struct list *units);

void vLevel_SetupSectors(Vent_Level *l);

void vLevel_ClearSectors(Vent_Level *l);

void vLevel_CleanGround(Vent_Level *l);

void vLevel_Clean(Vent_Level *l);

int vLevel_NumTileType(Vent_Level *l, int tileType);

int vLevel_GetCustomSurfaces(Vent_Level *l);

int vLevel_ChangeTileGraphic(Vent_Level *l, int oldGraphicType, int newGraphicType);

int vLevel_CheckTileGraphic(Vent_Level *l, int graphicType);


#endif
