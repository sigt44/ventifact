#ifndef BULLETS_H
#define BULLETS_H

#include "Vector2D.h"
#include "Time/Timer.h"
#include "Graphics/Sprite.h"

#include "Camera.h"
#include "Bullet_Attributes.h"
#include "Level/Level.h"
#include "Level/Sector.h"

#define BULLET_NO_HIT 0
#define BULLET_HIT 1
#define BULLET_HIT_STRIKE 2
#define BULLET_DESTROYED 3

typedef struct Vent_Bullet
{
    int ID;
    int team;
    int direction;
    int type;

    int strikethroughStart;
    int strikethrough;
    Timer strikethroughTimer;
    Timer lifeTimer;

    Vector2D velocity;
    Vector2D position;
    Vector2DInt dPosition;

    float currentDistance;
    int maxDistance;
    int maxHeight;

    int damage;
    int speed;

    int status;

    Vent_Sector *inSector;

    Sprite *sBullet;

    Vent_Bullet_Attributes *attributes;

} Vent_Bullet;

void vBullet_GiveDirection(Vent_Bullet *b, int accurate);

Vent_Bullet *vBullet_Create(int ID, int x, int y, Vent_Bullet_Attributes *bA, Vent_Sector *inSector, int team, void *direction, Timer *srcTime);

void vBullet_Setup(Vent_Bullet *b, int ID, int x, int y, Vent_Bullet_Attributes *bA, Vent_Sector *inSector, int team, void *direction, Timer *srcTime);

int vBullet_Update(Vent_Bullet *b, void *game, struct list *buildings, float dt);

void vBullet_SetSector(Vent_Bullet *b, Vent_Level *level);

void vBullet_Clean(Vent_Bullet *b);

void vBullet_Draw(Vent_Bullet *b, Camera_2D *c, SDL_Surface *dest);

int vBullet_UpdateBuildings(Vent_Bullet *b, struct list *buildings);

int vBullet_UpdateUnits(Vent_Bullet *b, void *game);

int vBullet_UpdateTiles(Vent_Bullet *b, struct list *tiles);

int vBullet_HitUnits(Vent_Bullet *b, struct list *units, void *game);

int vBullet_Hit(Vent_Bullet *b, int *targetHealth, int targetHeight);

int vBullet_TeamCheck(Vent_Bullet *b, int team);

void vBullet_UpdateEffect(Vent_Bullet *b, void *game);
void vBullet_DamageEffect(Vent_Bullet *b, void *game);


#endif
