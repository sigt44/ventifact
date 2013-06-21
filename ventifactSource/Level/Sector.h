#ifndef SECTOR_H
#define SECTOR_H

#define SECTOR_WIDTH 500
#define SECTOR_HEIGHT 500
#define SECTOR_ADJACENT 8

#include <SDL/SDL.h>

#include "Common/List.h"
#include "Vector2DInt.h"
#include "Camera.h"

typedef struct Vent_Sector
{
    Vector2DInt position;

    int width;
    int height;

    struct list *tiles;
    struct list *collisionTiles;
    struct list *units;

    struct Vent_Sector *adjacent[SECTOR_ADJACENT];

} Vent_Sector;

void vSector_Report(Vent_Sector *s);

struct list *vSector_Tile(int sWidth, int sHeight, int gWidth, int gHeight);

Vent_Sector *vSector_Create(int x, int y, int w, int h);

void vSector_Setup(Vent_Sector *s, int x, int y, int w, int h);

int vSector_Draw(Vent_Sector *s, Camera_2D *c, SDL_Surface *dest);

void vSector_Update(Vent_Sector *s, struct list *units);

void vSector_AssignAdjacent(Vent_Sector *p, Vent_Sector *c);

void vSector_Map(struct list *sectorList);

int vSector_MapTileInsert(Vent_Sector *s, void *tile);

int vSector_MapTile(struct list *sectors, void *tile);

void vSector_MapTiles(struct list *sectors, struct list *tiles);

void *vSector_GetTile(Vent_Sector *s, int closest, int posX, int posY, int w, int h);

Vent_Sector *vSector_Find(Vent_Sector *startSector, int x, int y);

void vSector_Clean(Vent_Sector *s);

#endif
