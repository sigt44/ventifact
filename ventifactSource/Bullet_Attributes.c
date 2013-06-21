#include "Bullet_Attributes.h"

#include "Graphics/Sprite.h"
#include "Graphics/Surface.h"

#include "Bullets.h"
#include "Defines.h"
#include "Images.h"

char *bulletNames[VENT_TOTALWEAPONS] = {"Plasma", "Plasma(Weak)", "Missile", "Rocket", "Repair", "Flamethrower", "Mine"};

/*
    Function vBulletAttributes_SetDefault

    Description -
    Sets the default base attributes for a bullet.

    2 arguments:
    Vent_Bullet_Attributes *b - The attribute structure for the bullet
    int type - The type of bullet
*/
void vBulletAttributes_SetDefault(Vent_Bullet_Attributes *bA, Timer *srcTime, int type)
{
    bA->type = type;
    bA->srcTime = srcTime;

    switch(type)
    {
        case WEAPON_PLASMA:
        default:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_BUILDING, srcTime, 2,
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER], M_FREE),
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_1], M_FREE));

        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_BUILDING, srcTime, 2,
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[VERTICAL][TEAM_PLAYER], M_FREE),
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[VERTICAL][TEAM_1], M_FREE));

        vBulletAttributes_SetProp(bA,
                                   "Plasma", /*Name*/
                                   VL_AIR, /*Max hit height*/
                                   10, /*Base damage*/
                                   120, /*Speed*/
                                   200, /*Max distance*/
                                   0, /*Life time limit*/
                                   0, /*Strikethrough amount*/
                                   0, /*The delta time between each strikethrough attack*/
                                   200 /*Refire rate*/
                                   );
        break;

        case WEAPON_PLASMA_WEAK:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_BUILDING, srcTime, 2,
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER], M_FREE),
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_1], M_FREE));

        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_BUILDING, srcTime, 2,
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[VERTICAL][TEAM_PLAYER], M_FREE),
                     frame_CreateBasic(0, ve_Surfaces.plasmaBullet[VERTICAL][TEAM_1], M_FREE));

        vBulletAttributes_SetProp(bA,
                                   "Plasma(weak)", /*Name*/
                                   VL_AIR, /*Max hit height*/
                                   20, /*Base damage*/
                                   120, /*Speed*/
                                   200, /*Max distance*/
                                   0, /*Life time limit*/
                                   0, /*Strikethrough amount*/
                                   0, /*The delta time between each strikethrough attack*/
                                   1000 /*Refire rate*/
                                   );
        break;

        case WEAPON_MISSILE:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_TREE, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.missileBullet[HORIZONTAL], M_FREE));
        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_TREE, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.missileBullet[VERTICAL], M_FREE));


        vBulletAttributes_SetProp(bA,
                           "Missile", /*Name*/
                           VL_AIR, /*Max hit height*/
                           80, /*Base damage*/
                           150, /*Speed*/
                           300, /*Max distance*/
                           0, /*Life time limit*/
                           0, /*Strikethrough amount*/
                           0, /*The delta time between each strikethrough attack*/
                           400 /*Refire rate*/
                           );
        break;

        case WEAPON_FLAME:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_TREE, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.flameBullet, M_FREE));
        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_TREE, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.flameBullet, M_FREE));


        vBulletAttributes_SetProp(bA,
                           "Flamethrower", /*Name*/
                           VL_BUILDING, /*Max hit height*/
                           12, /*Base damage*/
                           180, /*Speed*/
                           90, /*Max distance*/
                           0, /*Life time limit*/
                           4, /*Strikethrough amount*/
                           150, /*The delta time between each strikethrough attack*/
                           100 /*Refire rate*/
                           );
        break;

        case WEAPON_ROCKET:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_AIR, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.rocketBullet[HORIZONTAL], M_FREE));
        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_AIR, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.rocketBullet[VERTICAL], M_FREE));


        vBulletAttributes_SetProp(bA,
                           "Rocket", /*Name*/
                           VL_AIR, /*Max hit height*/
                           200, /*Base damage*/
                           90, /*Speed*/
                           1000, /*Max distance*/
                           0, /*Life time limit*/
                           3, /*Strikethrough amount*/
                           500, /*The delta time between each strikethrough attack*/
                           2000 /*Refire rate*/
                           );
        break;

        case WEAPON_MINE:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_GROUND, srcTime, 2,
                     frame_CreateBasic(0, ve_Surfaces.mineBullet[TEAM_PLAYER], M_FREE),
                     frame_CreateBasic(0, ve_Surfaces.mineBullet[TEAM_1], M_FREE)
        );

        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_GROUND, srcTime, 2,
                     frame_CreateBasic(0, ve_Surfaces.mineBullet[TEAM_PLAYER], M_FREE),
                     frame_CreateBasic(0, ve_Surfaces.mineBullet[TEAM_1], M_FREE)
        );

        vBulletAttributes_SetProp(bA,
                           "Mine", /*Name*/
                           VL_BUILDING, /*Max hit height*/
                           150, /*Base damage*/
                           0, /*Speed*/
                           1, /*Max distance*/
                           0, /*Life time limit*/
                           0, /*Strikethrough amount*/
                           500, /*The delta time between each strikethrough attack*/
                           1000 /*Refire rate*/
                           );
        break;

        case WEAPON_REPAIR:

        sprite_Setup(&bA->sBullet[HORIZONTAL], 0, VL_AIR, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.repairBullet[HORIZONTAL], M_FREE));
        sprite_Setup(&bA->sBullet[VERTICAL], 0, VL_AIR, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.repairBullet[VERTICAL], M_FREE));


        vBulletAttributes_SetProp(bA,
                           "Repair", /*Name*/
                           VL_AIR, /*Max hit height*/
                           30, /*Base damage*/
                           80, /*Speed*/
                           90, /*Max distance*/
                           0, /*Life time limit*/
                           0, /*Strikethrough amount*/
                           250, /*The delta time between each strikethrough attack*/
                           500 /*Refire rate*/
                           );
        break;
    }

    return;
}

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
                               )
{
    if(bA->name != name)
        strncpy(bA->name, name, 63);

    bA->maxHeight = maxHeight;
    bA->damage = damage;
    bA->speed = speed;

    bA->maxDistance = maxDistance;
    bA->maxDistanceSqr = bA->maxDistance * bA->maxDistance;

    bA->lifeTime = lifeTime;
    bA->strikethroughAmount = strikethroughAmount;
    bA->strikethroughTime = strikethroughTime;
    bA->refireRate = refireRate;

    bA->damagePerSecond = (float)damage/((float)refireRate/1000.0f);

    return;
}

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
                               )
{
    vBulletAttributes_SetProp(bA,
                              name,
                              bA->maxHeight + maxHeight,
                              bA->damage + damage,
                              bA->speed + speed,
                              bA->maxDistance + maxDistance,
                              bA->lifeTime + lifeTime,
                              bA->strikethroughAmount + strikethroughAmount,
                              bA->strikethroughTime + strikethroughTime,
                              bA->refireRate + refireRate
                              );
    return;
}

void vBulletAttributes_GiveProp(void *bullet, Vent_Bullet_Attributes *bA)
{
    Vent_Bullet *b = bullet;
    int imageDirection;

    if(b->direction == D_LEFT || b->direction == D_RIGHT)
        imageDirection = HORIZONTAL;
    else
        imageDirection = VERTICAL;

    b->type = bA->type;

    b->sBullet = sprite_Copy(&bA->sBullet[imageDirection], 1);

    if(b->team > TEAM_PLAYER)
    {
        sprite_SetFrame(b->sBullet, b->team);
    }

    b->damage = bA->damage;

    b->speed = bA->speed;

    b->maxDistance = bA->maxDistance;

    b->maxHeight = bA->maxHeight;

    b->lifeTimer = timer_Setup(bA->srcTime, 0, bA->lifeTime, 1);

    b->strikethroughStart = b->strikethrough = bA->strikethroughAmount;

    if(b->strikethrough > 0)
    {
        b->strikethroughTimer = timer_Setup(bA->srcTime, -bA->strikethroughTime, bA->strikethroughTime, 1);
    }

    return;
}

void vBulletAttributes_Clean(Vent_Bullet_Attributes *bA)
{
    sprite_Clean(&bA->sBullet[VERTICAL]);
    sprite_Clean(&bA->sBullet[HORIZONTAL]);

    return;
}
