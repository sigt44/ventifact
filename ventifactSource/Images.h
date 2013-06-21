#ifndef IMAGES_H
#define IMAGES_H

#include <SDL/SDL.h>

#include "Graphics/Sprite.h"
#include "Defines.h"

struct surface_Holder
{
    /*Menu*/
    SDL_Surface *mainMenu;
    SDL_Surface *curser;
    SDL_Surface *editorCurser;
    SDL_Surface *scroll[4];

    SDL_Surface *bHighlight;

    SDL_Surface *badge;

    /*GUI*/
    SDL_Surface *unitBar;
    SDL_Surface *smallBox;

    SDL_Surface *optionBar;

    SDL_Surface *buildingIcon[5];

    SDL_Surface *healthBar[3];
    SDL_Surface *healthBarOverlay;

    SDL_Surface *gameBar;
    SDL_Surface *buildBar;

    SDL_Surface *infinity;
    SDL_Surface *node[2];
    SDL_Surface *unitPointer[6];

    SDL_Surface *sideStart[2];

    /*Bullets*/
    SDL_Surface *plasmaBullet[2][2];
    SDL_Surface *missileBullet[2];
    SDL_Surface *mineBullet[2];
    SDL_Surface *repairBullet[2];
    SDL_Surface *rocketBullet[2];
    SDL_Surface *flameBullet;

    /*Units*/
    SDL_Surface *units[UNIT_ENDTYPE][2][4]; /*Easy array to locate unit images*/
    SDL_Surface *unitsDamaged[UNIT_ENDTYPE][2][4]; /*Easy array to locate unit images*/

    SDL_Surface *air[2][4];
    SDL_Surface *airDamaged[2][4];
    SDL_Surface *airDestroyed[4];

    SDL_Surface *tower[2][4];
    SDL_Surface *towerDamaged[2][4];

    SDL_Surface *tank[2][4];
    SDL_Surface *tankDamaged[2][4];
    SDL_Surface *tankDestroyed[4];

    SDL_Surface *infantry[2][4];
    SDL_Surface *infantryDamaged[2][4];
    SDL_Surface *infantryDestroyed[4];

    /*Buildings*/
    SDL_Surface *bCitadel[2];
    SDL_Surface *bTank[2];
    SDL_Surface *bInfantry[2];
    SDL_Surface *bAir[2];
    SDL_Surface *bPower[2];

    /*Tiles*/
    SDL_Surface *tileBuilding;
    SDL_Surface *tileCitadel;
    SDL_Surface *tileWater;
    SDL_Surface *tileGrass;
    SDL_Surface *tileSnow;
    SDL_Surface *tileSand;
    SDL_Surface *tileRoad[2];
    SDL_Surface *tileWall;
    SDL_Surface *unknown;

    /*Effects*/
    SDL_Surface *effectSmoke[3];
    SDL_Surface *effectHeal[4];
    SDL_Surface *effectHealRing[4];
    SDL_Surface *effectRepair[4];
    SDL_Surface *effectExplosion[5];
    SDL_Surface *effectSpawn[4];
    SDL_Surface *effectFlame[7];
    SDL_Surface *effectCall[4];

};

extern struct surface_Holder ve_Surfaces;

struct sprite_Holder
{
    /*Effects*/
    Sprite unitExplosion;
    Sprite bulletHit;
    Sprite heal;
    Sprite repair;
    Sprite healRing;
    Sprite spawn;
    Sprite flame;
    Sprite call;

    /*Units*/
    Sprite tankDestroyed;
    Sprite airDestroyed;
    Sprite infantryDestroyed;

    /*UI*/
    Sprite checkBox;
    Sprite scroll[4];
};

extern struct sprite_Holder ve_Sprites;

int image_Init(void);

int vSprite_Init(void);
int vSprite_Clean(void);

#endif
