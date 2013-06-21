#include "Attributes.h"
#include "Units.h"

#include "../Defines.h"

Vent_Attributes unitAttributes[UNIT_ENDTYPE][MAX_ATTRIBUTE_LEVEL];
Vent_Attributes playerAttributes[MAX_ATTRIBUTE_LEVEL];
Vent_Attributes noAttributes;

void vAttributes_SetupDefaults(void)
{
    /*No attributes*/
    vAttributes_Setup(&noAttributes,
                       0,   /*Health*/
                       0,    /*Speed*/
                       0,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 0, 0, 0 /*ammo*/
     );

    /*Player attributes*/
    vAttributes_Setup(&playerAttributes[0],
                       20,   /*Health*/
                       5,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 5, 0, 0, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&playerAttributes[1],
                       80,   /*Health*/
                       0,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 5, 0, 0, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&playerAttributes[2],
                       50,   /*Health*/
                       5,    /*Speed*/
                       2,    /*Damage*/
                       -10,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 5, 0, 25, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&playerAttributes[3],
                       150,   /*Health*/
                       0,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 5, 0, 25, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&playerAttributes[4],
                       200,   /*Health*/
                       20,    /*Speed*/
                       10,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 15, 0, AMMO_UNLIMITED, 0, 16 /*ammo*/
     );

     /*Tank attributes*/
     vAttributes_Setup(&unitAttributes[UNIT_TANK][0],
                       10,   /*Health*/
                       5,    /*Speed*/
                       0,    /*Damage*/
                       -5,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TANK][1],
                       20,   /*Health*/
                       5,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TANK][2],
                       20,   /*Health*/
                       5,    /*Speed*/
                       2,    /*Damage*/
                       -10,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 5, 0, 0, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TANK][3],
                       50,   /*Health*/
                       0,    /*Speed*/
                       5,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 5, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TANK][4],
                       100,   /*Health*/
                       10,    /*Speed*/
                       10,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 15, 2, 0, 0, 0 /*ammo*/
     );

     /*Infantry attributes*/
     vAttributes_Setup(&unitAttributes[UNIT_INFANTRY][0],
                       5,   /*Health*/
                       5,    /*Speed*/
                       0,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_INFANTRY][1],
                       10,   /*Health*/
                       5,    /*Speed*/
                       0,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_INFANTRY][2],
                       10,   /*Health*/
                       5,    /*Speed*/
                       0,    /*Damage*/
                       -10,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_INFANTRY][3],
                       25,   /*Health*/
                       5,    /*Speed*/
                       0,    /*Damage*/
                       -10,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_INFANTRY][4],
                       50,   /*Health*/
                       5,    /*Speed*/
                       10,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 10, AMMO_UNLIMITED, 0 /*ammo*/
     );

     /*Aircraft attributes*/
     vAttributes_Setup(&unitAttributes[UNIT_AIR][0],
                       15,   /*Health*/
                       0,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_AIR][1],
                       15,   /*Health*/
                       5,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_AIR][2],
                       25,   /*Health*/
                       5,    /*Speed*/
                       2,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_AIR][3],
                       25,   /*Health*/
                       0,    /*Speed*/
                       5,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 0, 0, 0 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_AIR][4],
                       50,   /*Health*/
                       10,    /*Speed*/
                       10,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 0, 0, 0, 0, 0 /*ammo*/
     );

     /*Tower attributes*/
     vAttributes_Setup(&unitAttributes[UNIT_TOWER][0],
                       50,   /*Health*/
                       0,    /*Speed*/
                       0,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TOWER][1],
                       50,   /*Health*/
                       0,    /*Speed*/
                       0,    /*Damage*/
                       -10,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TOWER][2],
                       50,   /*Health*/
                       0,    /*Speed*/
                       0,    /*Damage*/
                       -10,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TOWER][3],
                       50,   /*Health*/
                       0,    /*Speed*/
                       0,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, 2, 0, 10, 0, 1 /*ammo*/
     );

     vAttributes_Setup(&unitAttributes[UNIT_TOWER][4],
                       50,   /*Health*/
                       0,    /*Speed*/
                       0,    /*Damage*/
                       0,    /*Refire rate*/
                       0,    /*Invincible*/
                       0, 0, AMMO_UNLIMITED, 0, 10, 0, 0 /*ammo*/
     );


     return;
}

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
    int mineAmmo)
{
    int x = 0;

    a->health = health;
    a->speed = speed;
    a->baseDamage = baseDamage;
    a->refireRate = refireRate;
    a->invincibility = invincibility;

    vWeapon_ClearAmmo(&a->ammo[x]);

    a->ammo[WEAPON_PLASMA] = plasmaAmmo;
    a->ammo[WEAPON_PLASMA_WEAK] = plasmaWeakAmmo;
    a->ammo[WEAPON_MISSILE] = missileAmmo;
    a->ammo[WEAPON_ROCKET] = rocketAmmo;
    a->ammo[WEAPON_REPAIR] = repairAmmo;
    a->ammo[WEAPON_FLAME] = flameAmmo;
    a->ammo[WEAPON_MINE] = mineAmmo;

    return;
}

void vAttributes_Add(Vent_Attributes *base, Vent_Attributes *add)
{
    int x = 0;

    base->health += add->health;
    base->speed += add->speed;
    base->baseDamage += add->baseDamage;
    base->refireRate += add->refireRate;

    if(base->invincibility == 0)
        base->invincibility = add->invincibility;

    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        if(base->ammo[x] != AMMO_UNLIMITED && add->ammo[x] != AMMO_UNLIMITED)
        {
            base->ammo[x] += add->ammo[x];
        }
        else if(add->ammo[x] == AMMO_UNLIMITED)
        {
            base->ammo[x] = AMMO_UNLIMITED;
        }
    }

    return;
}

void vAttributes_Set(void *unit, Vent_Attributes *a)
{
    int x = 0;
    Vent_Unit *u = unit;

    u->healthStarting += a->health;
    u->health = u->healthStarting;

    u->maxSpeed += a->speed;

    vWeapon_SetBaseDamage(&u->weapon, a->baseDamage);

    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        /*Additional attributes for the bullet*/
        vBulletAttributes_AddProp(&u->weapon.attributes[x],
                               u->weapon.attributes[x].name, /*name*/
                               0, /*max height*/
                               0,    /*damage*/
                               0,     /*speed*/
                               0, /*distance*/
                               0,    /*lifetime*/
                               0, /*strikethrough amount*/
                               0, /*strikethrough time*/
                               -a->refireRate   /*refire rate*/
                               );

        if(u->weapon.ammo[x] != AMMO_UNLIMITED)
        {
            if(a->ammo[x] != AMMO_UNLIMITED)
            {
                u->weapon.ammo[x] += a->ammo[x];
            }
            else
                u->weapon.ammo[x] = AMMO_UNLIMITED;
        }
    }

    if(a->invincibility == 1)
    {
        u->invulnerability = a->invincibility;
    }

    return;
}

int vAttributes_WriteDifference(char *text, Vent_Attributes *base)
{
    const int bufferSize = 256;
    char textBuffer[256];
    char *textLocation = textBuffer;
    int numWritten = 0;
    int textAdd = 0;
    int x = 0;

    strcpy(textLocation, "");

    if(base->health != 0)
    {
        numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "Health: %d\n", base->health);
        textLocation += textAdd;
    }

    if(base->speed != 0)
    {
        numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "Speed: %d\n", base->speed);
        textLocation += textAdd;
    }

    if(base->baseDamage != 0)
    {
        numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "Damage: %d\n", base->baseDamage);
        textLocation += textAdd;
    }

    if(base->refireRate != 0)
    {
        numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "Refire: %d\n", base->refireRate);
        textLocation += textAdd;
    }

    if(base->invincibility != 0)
    {
        numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "Invincible\n");
        textLocation += textAdd;
    }

    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        if(base->ammo[x] > 0)
        {
            numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "%s: %d\n", bulletNames[x], base->ammo[x]);
            textLocation += textAdd;
        }
        else if(base->ammo[x] == AMMO_UNLIMITED)
        {
            numWritten += textAdd = snprintf(textLocation, bufferSize - numWritten - 1, "%s: Infinite\n", bulletNames[x]);
            textLocation += textAdd;
        }
    }

    textBuffer[numWritten - 1] = '\0';

    strncpy(text, textBuffer, numWritten);

    return numWritten;
}

