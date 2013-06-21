#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "../Defines.h"

#define MAX_ATTRIBUTE_LEVEL 5

typedef struct Vent_Unit_Description
{
    char *name;

    int price;
} Vent_Unit_Description;

typedef struct Vent_Attributes
{
    int health;
    int speed;
    int baseDamage;
    int refireRate;
    int invincibility;

    int ammo[VENT_TOTALWEAPONS];

} Vent_Attributes;

extern Vent_Attributes unitAttributes[UNIT_ENDTYPE][MAX_ATTRIBUTE_LEVEL];
extern Vent_Attributes playerAttributes[MAX_ATTRIBUTE_LEVEL];
extern Vent_Attributes noAttributes;

void vAttributes_Setup(Vent_Attributes *a,
    int health,
    int speed,
    int baseDamage,
    int refireRate,
    int invincibility,

    int plasmaAmmo,
    int plasmaWeakAmmo,
    int missileAmmo,
    int rocketAmmo,
    int repairAmmo,
    int flameAmmo,
    int mineAmmo);

void vAttributes_SetupDefaults(void);

void vAttributes_Add(Vent_Attributes *base, Vent_Attributes *add);

void vAttributes_Set(void *unit, Vent_Attributes *a);

int vAttributes_WriteDifference(char *text, Vent_Attributes *base);

#endif
