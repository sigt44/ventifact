#include <math.h>

#include "Weapon.h"

#include "Game/VentGame.h"
#include "Bullets.h"
#include "Sounds.h"
#include "Maths.h"

#include "Kernel/Kernel.h"
#include "Sound.h"
#include "File.h"
/*int vWeapon_GetDirection(float xDis, float yDis)
{
    int angle = 90;

    const int subAngles[D_TOTAL] =
    {
        -90,
        90,
        0,
        180,
        -135,
        -45,
        135,
        45

    };
    int x = 0;

    if(xDis != 0)
        angle = 180 * (atanf(yDis/xDis)/PI);
    if(xDis > 0)
    {
        if(yDis < 0)
            angle += 180;
        else if(yDis > 0)
            angle -= 180;
    }


    //printf("Angle = %d (%.2f, %.2f)\n", angle, xDis, yDis);

    if(abs(-180 - angle) < 23)
        return D_RIGHT;

    for(x = 0; x < D_TOTAL; x++)
    {
        //printf("\tS(%d) - A = %d\n",subAngles[x], abs(subAngles[x] - angle));
        if(abs(subAngles[x] - angle) < 23)
            return x;
    }

    puts("Error coudnt find angle");
    return D_UP;

}*/

/*
    Function: vWeapon_GetDirection

    Description -
    Returns a number that will represent 1 of 8 different directions for the weapon to fire at.

    The direction to fire can be at one of 8 angles
    0 (D_RIGHT), 45 (D_UPRIGHT), 90 (D_UP), 135 (D_UPLEFT), 180 (D_LEFT), 225 (D_DOWNLEFT),
    270 (D_DOWN), 315 (D_DOWNRIGHT).

    The direction is chosen based upon which angle above is closest to the real angle of the target
    (found by xDis and yDis).

    3 arguments:
    float tan - The tangent of the angle the weapon should fire at.
    float xDis - The x-axis distance of the target the weapon should fire at.
    float yDis - The y-axis distance of the target the weapon should fire at.
*/
int vWeapon_GetDirection(float tan, float xDis, float yDis)
{
    const float tan225 = 0.414f;/* An estimate of the tangent(22.5 degrees) to 3dp*/
    const float tan675 = 2.414f; /* An estimate of the tangent(67.5 degrees) to 3dp*/

    if(tan > 0) /*If the direction is in the first or third quadrants of a circle*/
    {
        if(xDis > 0) /*Since xDis > 0 then it must be in the first quadrant*/
        {
            if(tan < tan225) /*When tan(angle) = 0.414 angle is close to 22.5*/
                return D_RIGHT;
            else if(tan > tan675) /*tan(angle) = 2.414, angle is close to 67.5*/
                return D_UP;
            else    /*tan(angle) lies between 0.414 and 2.414 therefore its closer to the diagonal, 45 degrees*/
                return D_UPRIGHT;
        }
        else /*Since xDis < 0 or 0, presume to be the third quadrant*/
        {
            if(tan < tan225)
                return D_LEFT;
            else if(tan > tan675)
                return D_DOWN;
            else
                return D_DOWNLEFT;
        }
    }
    else
    {
        if(xDis > 0)
        {
            if(tan > -tan225)
                return D_RIGHT;
            else if(tan < -tan675)
                return D_DOWN;
            else
                return D_DOWNRIGHT;
        }
        else
        {
            if(tan > -tan225)
                return D_LEFT;
            else if(tan < -tan675)
                return D_UP;
            else
                return D_UPLEFT;
        }
    }

    return -1;
}

void vWeapon_SetRadius(Vent_Weapon *w, int radius)
{
    int x = 0;

    for(x = 0; x < 8; x++)
    {
        switch(x)
        {
            case D_UP:
                w->fireRadius[x].x = 0;
                w->fireRadius[x].y = -radius;
            break;

            case D_DOWN:
                w->fireRadius[x].x = 0;
                w->fireRadius[x].y =  radius;
            break;

            case D_LEFT:
                w->fireRadius[x].x = -radius;
                w->fireRadius[x].y = 0;
            break;

            case D_RIGHT:
                w->fireRadius[x].x = radius;
                w->fireRadius[x].y = 0;
            break;

            case D_UPRIGHT:
                w->fireRadius[x].x = (int)(0.7071 * radius);
                w->fireRadius[x].y = -w->fireRadius[x].x;
            break;

            case D_UPLEFT:
                w->fireRadius[x].x = -(int)(0.7071 * radius);
                w->fireRadius[x].y = w->fireRadius[x].x;
            break;

            case D_DOWNLEFT:
                w->fireRadius[x].x = -(int)(0.7071 * radius);
                w->fireRadius[x].y = -w->fireRadius[x].x;
            break;

            case D_DOWNRIGHT:
                w->fireRadius[x].x = (int)(0.7071 * radius);
                w->fireRadius[x].y = w->fireRadius[x].x;
            break;
        }
    }

    return;
}

void vWeapon_Setup(Vent_Weapon *w, int ID, int radius, int xOffset, int yOffset, unsigned int bulletType, int team, int accurate, int isPlayer, Timer *srcTime)
{
    w->ID = ID;
    w->originalBulletType = w->bulletType = bulletType;
    w->team = team;
    w->isPlayer = isPlayer;

    vWeapon_SetRadius(w, radius);

    vWeapon_SetOffset(w, xOffset, yOffset);

    w->baseDmg = 0;

    w->accurate = accurate;

    vWeapon_SetDefaultBulletAttributes(w, srcTime);

    vWeapon_ClearAmmo(&w->ammo[0]);
    vWeapon_ClearAmmo(&w->ammoOriginal[0]);

    w->prevBulletType = bulletType;
    w->bulletType = bulletType;

    vWeapon_AssignBullet(w, bulletType);

    w->refireTimer = timer_Setup(srcTime, 0, w->refireRate, 1);

    return;
}

void vWeapon_Clean(Vent_Weapon *w)
{
    int x = 0;
    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        vBulletAttributes_Clean(&w->attributes[x]);
    }
}

void vWeapon_SetOffset(Vent_Weapon *w, int xOffset, int yOffset)
{
    w->position.x = xOffset;
    w->position.y = yOffset;

    return;
}

void vWeapon_ClearAmmo(int ammo[])
{
    int x = 0;
    for(x = WEAPON_PLASMA; x < VENT_TOTALWEAPONS; x++)
    {
        ammo[x] = AMMO_NONE;
    }

    return;
}

void vWeapon_SetAmmo(Vent_Weapon *w, int ammoType, int ammoAmount)
{
    w->ammo[ammoType] = ammoAmount;

    return;
}

void vWeapon_CycleBullet(Vent_Weapon *w, int amount)
{
    int x = 0;
    int newBulletType = w->bulletType;

    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        newBulletType = (newBulletType + amount) % VENT_TOTALWEAPONS;

        if(w->ammo[newBulletType] != AMMO_NONE)
        {
            w->bulletType = newBulletType;
            x = VENT_TOTALWEAPONS;
        }
    }

    vWeapon_AssignBullet(w, w->bulletType);

    return;
}

/*
    Function: vWeapon_SetDefaultAttributes

    Description -
    Sets the default attributes for a weapon

    1 argument:
    Vent_Weapon *w - The weapon to set the attributes.
    Timer *srcTime - The base timer for the bullet to work with.
*/
void vWeapon_SetDefaultBulletAttributes(Vent_Weapon *w, Timer *srcTime)
{
    int x;

    /*For all weapons*/
    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        w->attributes[x].type = x;
        w->attributes[x].srcTime = NULL;

        /*Assign the default properties to them*/
        vBulletAttributes_SetDefault(&w->attributes[x], srcTime, x);

        //file_Log(ker_Log(), 0, "Weapon[%s] DPS - %.2f\n", w->attributes[x].name, (float)w->attributes[x].damage*(1000.0f/w->attributes[x].refireRate));
    }

    return;
}

/*
    Function: vWeapon_SetDefaultAttributes

    Description -
    Sets the additional base damage of all the bullets in a weapon.

    1 argument:
    Vent_Weapon *w - The weapon to set the base damage.
    int baseDamage - The value of the base damage.
*/
void vWeapon_SetBaseDamage(Vent_Weapon *w, int baseDamage)
{
    int x = 0;

    /*For all weapons*/
    for(x = 0; x < VENT_TOTALWEAPONS; x++)
    {
        /*Revert the previously added damage amount*/
        w->attributes[x].damage -= w->baseDmg;

        /*Add in the new amount*/
        w->attributes[x].damage += baseDamage;
    }

    w->baseDmg = baseDamage;
}


void vWeapon_AssignBullet(Vent_Weapon *w, int bulletType)
{
    w->prevBulletType = w->bulletType;

    w->bulletType = bulletType;

    w->refireRate = w->attributes[bulletType].refireRate;

    w->refireTimer.end_Time = w->refireRate;

    /*printf("Changeing bullet to %s\n", w->attributes[bulletType].name);*/

    return;
}

void vWeapon_AssignPreviousBullet(Vent_Weapon *w)
{
    vWeapon_AssignBullet(w, w->prevBulletType);

    return;
}

/*
    Function: vWeapon_CanFire

    Description -
    Returns the maximum range of the weapon.

    1 argument:
    Vent_Weapon *w - The weapon to get the range from.
*/
int vWeapon_GetRange(Vent_Weapon *w)
{
    return w->attributes[w->bulletType].maxDistance;
}

/*
    Function: vWeapon_CanFire

    Description -
    Returns WEAPON_CANFIRE if the weapon is ready to fire.
    WEAPON_NOAMMO if there is no ammo to fire.
    WEAPON_REFIRE if the refire time has not complete

    1 argument:
    Vent_Weapon *w - The weapon to check.
*/
int vWeapon_CanFire(Vent_Weapon *w)
{
    if(w->ammo[w->bulletType] == AMMO_NONE)
        return WEAPON_NOAMMO;

    timer_Calc(&w->refireTimer);

    if(timer_Get_Remain(&w->refireTimer) > 0)
    {
        return WEAPON_REFIRE;
    }

    return WEAPON_CANFIRE;
}

/*
    Function: vWeapon_DirectionPreset

    Description -
    Returns a structure containing information about the weapons firing direction.
    The direction will be one of either 8 angles in segments of 45 degrees.

    4 arguments:
    int directionType - Preset to be one of 8 angles, such as D_UP, D_UPRIGHT, D_DOWN. If -1 then will attempt to find the closest angle.
    float tan - The tangent of the angle the weapon should fire at. (only used if directionType < 0)
    float xDis - The x-axis distance of the target the weapon should fire at. (only used if directionType < 0)
    float yDis - The y-axis distance of the target the weapon should fire at. (only used if directionType < 0)
*/
Vent_WeaponDirection *vWeapon_DirectionPreset(int direction, float tan, float xDis, float yDis)
{
    static Vent_WeaponDirection wD;

    wD.accurate = 0;

    if(direction > -1)
        wD.direction = direction;
    else
        wD.direction = vWeapon_GetDirection(tan, xDis, yDis);

    vector_Clear(&wD.moveScale);

    return &wD;
}


/*
    Function: vWeapon_DirectionFine

    Description -
    Returns a structure containing accurate information about the weapons firing direction.

    3 arguments:
    float tan - The tangent of the angle the weapon should fire at.
    float xDis - The x-axis distance of the target the weapon should fire at.
    float yDis - The y-axis distance of the target the weapon should fire at.
*/
Vent_WeaponDirection *vWeapon_DirectionFine(float tan, float xDis, float yDis)
{
    static Vent_WeaponDirection wD;

    float angle = atanf(tan);
    unsigned int lAngle = 0;

    wD.accurate = 1;
    wD.direction = vWeapon_GetDirection(tan, xDis, yDis);

    switch(mth_GetQuadrant(xDis, yDis))
    {
        case 0:

        break;

        case 1:
        angle = PI + angle;
        break;

        case 2:
        angle = PI + angle;
        break;

        case 3:
        angle = PI2 + angle;
        break;
    }

    lAngle = (unsigned int)(360.0f * (angle/PI2));

    wD.moveScale.x = f_Angles[lAngle][1];
    wD.moveScale.y = -f_Angles[lAngle][0];

    if(wD.moveScale.x == 0.0f && wD.moveScale.y == 0.0f)
    {
        printf("Error fire angle: %d - %.6f (%.2f %.2f)\n", lAngle, angle, xDis, yDis);
    }

    /*printf("[%.2f -> %d]: [%.2f, %.2f]\n",angle, lAngle, wD.moveScale.x, wD.moveScale.y);*/

    return &wD;
}


/*
    Function: vWeapon_Direction

    Description -
    Returns a structure containing required information about the weapons firing direction.

    4 arguments:
    int preset - If true then the maximum directions to fire at will be 8.
    int direction - If preset is true and this is from 0-7 then it will force a direction.
    float xDis - The x-axis distance of the target the weapon should fire at.
    float yDis - The y-axis distance of the target the weapon should fire at.
*/
Vent_WeaponDirection *vWeapon_Direction(int accurate, int direction, float xDis, float yDis)
{
    Vent_WeaponDirection *wD = NULL;
    float tan = 0.0f;

    if(xDis == 0.0f)
    {
        xDis = 0.0001f;
    }

    tan = yDis/xDis;

    if(accurate == 0)
    {
        wD = vWeapon_DirectionPreset(direction, tan, xDis, yDis);
    }
    else
    {
        wD = vWeapon_DirectionFine(tan, xDis, yDis);
    }

    return wD;
}

/*
    Function: vWeapon_Fire

    Description -
    If the weapon can fire it places a new bullet into a list so that it can be updated in the game.

    4 arguments:
    Vent_Weapon *w - The weapon structure that the bullet is to be fired from
    Vent_Game *game - The game the weapon is firing in.
    int x - The x location of the weapon.
    int y - The y location of the weapon.
    Vent_WeaponDirection *direction - Points to a weapon direction struct that is located inside a direction choosing function.
*/
int vWeapon_Fire(Vent_Weapon *w, void *game, int x, int y, Vent_WeaponDirection *direction)
{
    Vent_Game *g = game;
    int weaponStatus = 0;

    /*Check if the weapon is able to fire*/
    weaponStatus = vWeapon_CanFire(w);

    /*If not, return the reason why*/
    if(weaponStatus != WEAPON_CANFIRE)
    {
        return weaponStatus;
    }

    /*The weapon will fire so reset the refire timer*/
    timer_Start(&w->refireTimer);

    if(w->ammo[w->bulletType] != AMMO_UNLIMITED)
        w->ammo[w->bulletType] --;

    w->direction = *direction;

    /*Create a bullet and place it into a list so that it can be updated in the game*/
    list_Push(&g->bullets,
              vBullet_Create(w->ID, x + w->position.x + w->fireRadius[w->direction.direction].x, y + w->position.y + w->fireRadius[w->direction.direction].y, &w->attributes[w->bulletType], NULL, w->team, &w->direction, w->refireTimer.srcTime),
              0
             );

    /*Create the bullet fired sound*/
    sound_Call(ve_Sounds.bulletFire[w->bulletType], w->bulletType << 2, SND_DUPLICATE_SOFT, vGame_SoundVolume_Distance(g, x, y), 0);

    return WEAPON_FIRED;
}



