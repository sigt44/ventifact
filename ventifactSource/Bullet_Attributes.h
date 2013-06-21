#ifndef BULLET_ATTRIBUTES_H
#define BULLET_ATTRIBUTES_H

#include "Graphics/Sprite.h"
#include "Time/Timer.h"

#include "Defines.h"

typedef struct Vent_Bullet_Attributes
{
    char name[64];
    int type;

    int strikethrough;
    int strikethroughTime;
    int strikethroughAmount;
    int lifeTime;

    Timer *srcTime;

    int maxDistance;
    int maxDistanceSqr;
    int maxHeight;

    int speed;

    int damage;

    float damagePerSecond;

    int refireRate;

   // Vector2DInt collision[2];

    Sprite sBullet[2];

} Vent_Bullet_Attributes;

extern char *bulletNames[VENT_TOTALWEAPONS];

void vBulletAttributes_SetDefault(Vent_Bullet_Attributes *bA, Timer *srcTime, int type);

void vBulletAttributes_SetProp(Vent_Bullet_Attributes *bA,
                               char *name,
                               int maxHeight,
                               int damage,
                               int speed,
                               int maxDistance,
                               int lifeTime,
                               int strikethroughAmount,
                               int strikethroughTime,
                               int refireRate
                               );

void vBulletAttributes_AddProp(Vent_Bullet_Attributes *bA,
                               char *name,
                               int maxHeight,
                               int damage,
                               int speed,
                               int maxDistance,
                               int lifeTime,
                               int strikethroughAmount,
                               int strikethroughTime,
                               int refireRate
                               );

void vBulletAttributes_GiveProp(void *bullet, Vent_Bullet_Attributes *bA);

void vBulletAttributes_Clean(Vent_Bullet_Attributes *bA);

#endif
