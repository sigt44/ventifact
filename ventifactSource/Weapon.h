#ifndef WEAPON_H
#define WEAPON_H

#include "Time/Timer.h"
#include "Common/List.h"
#include "Vector2DInt.h"

#include "Bullet_Attributes.h"
#include "Bullets.h"
#include "Defines.h"

typedef struct Vent_WeaponDirection
{
    int accurate;
    int direction;

    Vector2D moveScale;
} Vent_WeaponDirection;

typedef struct Vent_Weapon
{
    int ID;
    int prevBulletType;
    int bulletType;
    int originalBulletType;

    int baseDmg;
    int refireRate;

    Vector2DInt fireRadius[8];

    Timer refireTimer;

    int team;
    int isPlayer;

    int accurate;

    Vector2DInt position;

    int ammo[VENT_TOTALWEAPONS];
    int ammoOriginal[VENT_TOTALWEAPONS];
    Vent_Bullet_Attributes attributes[VENT_TOTALWEAPONS];

    Vent_WeaponDirection direction;

} Vent_Weapon;

void vWeapon_Clean(Vent_Weapon *w);

int vWeapon_GetDirection(float tan, float xDis, float yDis);

void vWeapon_SetDefaultBulletAttributes(Vent_Weapon *w, Timer *srcTime);

void vWeapon_SetRadius(Vent_Weapon *w, int radius);

void vWeapon_CycleBullet(Vent_Weapon *w, int amount);

void vWeapon_AssignBullet(Vent_Weapon *w, int bulletType);

void vWeapon_AssignPreviousBullet(Vent_Weapon *w);

int vWeapon_GetRange(Vent_Weapon *w);

void vWeapon_SetOffset(Vent_Weapon *w, int xOffset, int yOffset);

void vWeapon_Setup(Vent_Weapon *w, int ID, int radius, int xOffset, int yOffset, unsigned int bulletType, int team, int accurate8, int isPlayer, Timer *srcTime);

void vWeapon_ClearAmmo(int ammo[]);

void vWeapon_SetAmmo(Vent_Weapon *w, int ammoType, int ammoAmount);

void vWeapon_SetBaseDamage(Vent_Weapon *w, int baseDamage);

int vWeapon_CanFire(Vent_Weapon *w);

Vent_WeaponDirection *vWeapon_DirectionPreset(int direction, float tan, float xDis, float yDis);

Vent_WeaponDirection *vWeapon_DirectionFine(float tan, float xDis, float yDis);

Vent_WeaponDirection *vWeapon_Direction(int accurate, int direction, float xDis, float yDis);

int vWeapon_Fire(Vent_Weapon *w, void *game, int x, int y, Vent_WeaponDirection *direction);

#endif
