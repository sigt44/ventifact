#include <math.h>

#include "Images.h"
#include "Graphics/Surface.h"
#include "Defines.h"
#include "Maths.h"
#include "Graphics/Pixel.h"
#include "Kernel/Kernel.h"
#include "Menus/MenuAttributes.h"

struct surface_Holder ve_Surfaces;
struct sprite_Holder ve_Sprites;

int image_Init(void)
{
    int x = 0;
    int y = 0;

    /*Menu*/
    surf_Load(&ve_Surfaces.mainMenu, "menu.png", A_FREE);

    surf_Load(&ve_Surfaces.curser, "pointer.png", A_FREE);
    surf_Load(&ve_Surfaces.editorCurser, "editorPointer.png", A_FREE);

    surf_Load(&ve_Surfaces.bHighlight, "bHighlight.png", A_FREE);

    surf_Load(&ve_Surfaces.scroll[D_UP], "scrollUp.png", A_FREE);
    ve_Surfaces.scroll[D_DOWN] = surface_Rotate(ve_Surfaces.scroll[D_UP], PI, A_FREE);
    ve_Surfaces.scroll[D_LEFT] = surface_Rotate(ve_Surfaces.scroll[D_UP], PI_2, A_FREE);
    ve_Surfaces.scroll[D_RIGHT] = surface_Rotate(ve_Surfaces.scroll[D_DOWN], PI_2, A_FREE);

    surf_Load(&ve_Surfaces.badge, "badge.png", A_FREE);
    SDL_SetAlpha(ve_Surfaces.badge, SDL_SRCALPHA | SDL_RLEACCEL, 140);


    /*GUI*/
    surf_Load(&ve_Surfaces.smallBox, "Hud/smallBox.png", A_FREE);
    surf_Load(&ve_Surfaces.unitBar, "Hud/unitBar.png", A_FREE);

    surf_Load(&ve_Surfaces.buildingIcon[0], "Hud/buildIcon1.png", A_FREE);
    surf_Load(&ve_Surfaces.buildingIcon[1], "Hud/buildIcon2.png", A_FREE);
    surf_Load(&ve_Surfaces.buildingIcon[2], "Hud/buildIcon3.png", A_FREE);
    surf_Load(&ve_Surfaces.buildingIcon[3], "Hud/buildIcon4.png", A_FREE);
    surf_Load(&ve_Surfaces.buildingIcon[4], "Hud/buildIcon5.png", A_FREE);

    surf_Load(&ve_Surfaces.healthBar[0], "Hud/healthBarL.png", A_FREE);
    surf_Load(&ve_Surfaces.healthBar[1], "Hud/healthBarM.png", A_FREE);
    surf_Load(&ve_Surfaces.healthBar[2], "Hud/healthBarH.png", A_FREE);
    surf_Load(&ve_Surfaces.healthBarOverlay, "Hud/hBar.png", A_FREE);

    surf_Load(&ve_Surfaces.gameBar, "Hud/gameBar.png", A_FREE);
    surf_Load(&ve_Surfaces.buildBar, "Hud/buildBar.png", A_FREE);

    surf_Load(&ve_Surfaces.optionBar, "Hud/optionBar.png", A_FREE);

    surf_Load(&ve_Surfaces.node[0], "node.png", A_FREE);
    surf_Load(&ve_Surfaces.node[1], "node2.png", A_FREE);

    surf_Load(&ve_Surfaces.infinity, "Hud/infinity.png", A_FREE);

    surf_Load(&ve_Surfaces.unitPointer[0], "Hud/unitPointer1.png", A_FREE);
    surf_Load(&ve_Surfaces.unitPointer[1], "Hud/unitPointer2.png", A_FREE);
    surf_Load(&ve_Surfaces.unitPointer[2], "Hud/unitPointer3.png", A_FREE);
    surf_Load(&ve_Surfaces.unitPointer[3], "Hud/unitPointer4.png", A_FREE);
    surf_Load(&ve_Surfaces.unitPointer[4], "Hud/unitPointer5.png", A_FREE);
    surf_Load(&ve_Surfaces.unitPointer[5], "Hud/unitPointer6.png", A_FREE);

    surf_Load(&ve_Surfaces.sideStart[TEAM_PLAYER], "pStart.png", A_FREE);
    surf_Load(&ve_Surfaces.sideStart[TEAM_1], "eStart.png", A_FREE);

    /*Bullets*/
    surf_Load(&ve_Surfaces.plasmaBullet[VERTICAL][TEAM_PLAYER], "Bullets/aPlasma.png", A_FREE);
    ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER] = surface_Rotate(ve_Surfaces.plasmaBullet[VERTICAL][TEAM_PLAYER], PI_2, A_FREE);

    surf_Load(&ve_Surfaces.plasmaBullet[VERTICAL][TEAM_1], "Bullets/ePlasma.png", A_FREE);
    ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_1] = surface_Rotate(ve_Surfaces.plasmaBullet[VERTICAL][TEAM_1], PI_2, A_FREE);

    surf_Load(&ve_Surfaces.missileBullet[VERTICAL], "Bullets/missile.png", A_FREE);
    ve_Surfaces.missileBullet[HORIZONTAL] = surface_Rotate(ve_Surfaces.missileBullet[VERTICAL], PI_2, A_FREE);

    surf_Load(&ve_Surfaces.rocketBullet[VERTICAL], "Bullets/rocket.png", A_FREE);
    ve_Surfaces.rocketBullet[HORIZONTAL] = surface_Rotate(ve_Surfaces.rocketBullet[VERTICAL], PI_2, A_FREE);

    surf_Load(&ve_Surfaces.repairBullet[VERTICAL], "Bullets/repair.png", A_FREE);
    ve_Surfaces.repairBullet[HORIZONTAL] = surface_Rotate(ve_Surfaces.repairBullet[VERTICAL], PI_2, A_FREE);

    surf_Load(&ve_Surfaces.mineBullet[TEAM_PLAYER], "Bullets/aMine.png", A_FREE);
    surf_Load(&ve_Surfaces.mineBullet[TEAM_1], "Bullets/eMine.png", A_FREE);
    surf_Load(&ve_Surfaces.flameBullet, "Bullets/flame.png", A_FREE);

    /*Units*/

    /*Tanks*/
    surf_Load(&ve_Surfaces.tank[TEAM_PLAYER][D_UP], "Units/Tank/aTank.png", A_FREE);
    ve_Surfaces.tank[TEAM_PLAYER][D_LEFT] = surface_Rotate(ve_Surfaces.tank[TEAM_PLAYER][D_UP], PI_2, A_FREE);
    ve_Surfaces.tank[TEAM_PLAYER][D_RIGHT] = surface_Rotate(ve_Surfaces.tank[TEAM_PLAYER][D_UP], -PI_2, A_FREE);
    ve_Surfaces.tank[TEAM_PLAYER][D_DOWN] = surface_Rotate(ve_Surfaces.tank[TEAM_PLAYER][D_UP], PI, A_FREE);

    surf_Load(&ve_Surfaces.tank[TEAM_1][D_UP], "Units/Tank/eTank.png", A_FREE);
    ve_Surfaces.tank[TEAM_1][D_LEFT] = surface_Rotate(ve_Surfaces.tank[TEAM_1][D_UP], PI_2, A_FREE);
    ve_Surfaces.tank[TEAM_1][D_RIGHT] = surface_Rotate(ve_Surfaces.tank[TEAM_1][D_UP], -PI_2, A_FREE);
    ve_Surfaces.tank[TEAM_1][D_DOWN] = surface_Rotate(ve_Surfaces.tank[TEAM_1][D_UP], PI, A_FREE);

    surf_Load(&ve_Surfaces.tankDamaged[TEAM_PLAYER][D_UP], "Units/Tank/aTank.png", A_FREE);
    ve_Surfaces.tankDamaged[TEAM_PLAYER][D_LEFT] = surface_Rotate(ve_Surfaces.tankDamaged[TEAM_PLAYER][D_UP], PI_2, A_FREE);
    ve_Surfaces.tankDamaged[TEAM_PLAYER][D_RIGHT] = surface_Rotate(ve_Surfaces.tankDamaged[TEAM_PLAYER][D_UP], -PI_2, A_FREE);
    ve_Surfaces.tankDamaged[TEAM_PLAYER][D_DOWN] = surface_Rotate(ve_Surfaces.tankDamaged[TEAM_PLAYER][D_UP], PI, A_FREE);

    surf_Load(&ve_Surfaces.tankDamaged[TEAM_1][D_UP], "Units/Tank/eTank.png", A_FREE);
    ve_Surfaces.tankDamaged[TEAM_1][D_LEFT] = surface_Rotate(ve_Surfaces.tankDamaged[TEAM_1][D_UP], PI_2, A_FREE);
    ve_Surfaces.tankDamaged[TEAM_1][D_RIGHT] = surface_Rotate(ve_Surfaces.tankDamaged[TEAM_1][D_UP], -PI_2, A_FREE);
    ve_Surfaces.tankDamaged[TEAM_1][D_DOWN] = surface_Rotate(ve_Surfaces.tankDamaged[TEAM_1][D_UP], PI, A_FREE);

    surf_Load(&ve_Surfaces.tankDestroyed[D_UP], "Units/Tank/tankDestroyed.png", A_FREE);
    ve_Surfaces.tankDestroyed[D_LEFT] = surface_Rotate(ve_Surfaces.tankDestroyed[D_UP], PI_2, A_FREE);
    ve_Surfaces.tankDestroyed[D_RIGHT] = surface_Rotate(ve_Surfaces.tankDestroyed[D_UP], -PI_2, A_FREE);
    ve_Surfaces.tankDestroyed[D_DOWN] = surface_Rotate(ve_Surfaces.tankDestroyed[D_UP], PI, A_FREE);

    /*Infantry*/
    surf_Load(&ve_Surfaces.infantry[TEAM_PLAYER][D_UP], "Units/Infantry/aInfantry.png", A_FREE);
    ve_Surfaces.infantry[TEAM_PLAYER][D_LEFT] = surface_Rotate(ve_Surfaces.infantry[TEAM_PLAYER][D_UP], PI_2, A_FREE);
    ve_Surfaces.infantry[TEAM_PLAYER][D_RIGHT] = surface_Rotate(ve_Surfaces.infantry[TEAM_PLAYER][D_UP], -PI_2, A_FREE);
    ve_Surfaces.infantry[TEAM_PLAYER][D_DOWN] = surface_Rotate(ve_Surfaces.infantry[TEAM_PLAYER][D_UP], PI, A_FREE);

    ve_Surfaces.infantryDamaged[TEAM_PLAYER][D_UP] = ve_Surfaces.infantry[TEAM_PLAYER][D_UP];
    ve_Surfaces.infantryDamaged[TEAM_PLAYER][D_LEFT] = ve_Surfaces.infantry[TEAM_PLAYER][D_LEFT];
    ve_Surfaces.infantryDamaged[TEAM_PLAYER][D_RIGHT] = ve_Surfaces.infantry[TEAM_PLAYER][D_RIGHT];
    ve_Surfaces.infantryDamaged[TEAM_PLAYER][D_DOWN] = ve_Surfaces.infantry[TEAM_PLAYER][D_DOWN];

    surf_Load(&ve_Surfaces.infantry[TEAM_1][D_UP], "Units/Infantry/eInfantry.png", A_FREE);
    ve_Surfaces.infantry[TEAM_1][D_LEFT] = surface_Rotate(ve_Surfaces.infantry[TEAM_1][D_UP], PI_2, A_FREE);
    ve_Surfaces.infantry[TEAM_1][D_RIGHT] = surface_Rotate(ve_Surfaces.infantry[TEAM_1][D_UP], -PI_2, A_FREE);
    ve_Surfaces.infantry[TEAM_1][D_DOWN] = surface_Rotate(ve_Surfaces.infantry[TEAM_1][D_UP], PI, A_FREE);


    ve_Surfaces.infantryDamaged[TEAM_1][D_UP] = ve_Surfaces.infantry[TEAM_1][D_UP];
    ve_Surfaces.infantryDamaged[TEAM_1][D_LEFT] = ve_Surfaces.infantry[TEAM_1][D_LEFT];
    ve_Surfaces.infantryDamaged[TEAM_1][D_RIGHT] = ve_Surfaces.infantry[TEAM_1][D_RIGHT];
    ve_Surfaces.infantryDamaged[TEAM_1][D_DOWN] = ve_Surfaces.infantry[TEAM_1][D_DOWN];

    surf_Load(&ve_Surfaces.infantryDestroyed[D_UP], "Units/Infantry/destroyedInfantry.png", A_FREE);
    ve_Surfaces.infantryDestroyed[D_LEFT] = surface_Rotate(ve_Surfaces.infantryDestroyed[D_UP], PI_2, A_FREE);
    ve_Surfaces.infantryDestroyed[D_RIGHT] = surface_Rotate(ve_Surfaces.infantryDestroyed[D_UP], -PI_2, A_FREE);
    ve_Surfaces.infantryDestroyed[D_DOWN] = surface_Rotate(ve_Surfaces.infantryDestroyed[D_UP], PI, A_FREE);

    /*Aircraft*/
    surf_Load(&ve_Surfaces.air[TEAM_PLAYER][D_UP], "Units/Air/aAircraft.png", A_FREE);
    ve_Surfaces.air[TEAM_PLAYER][D_LEFT] = surface_Rotate(ve_Surfaces.air[TEAM_PLAYER][D_UP], PI_2, A_FREE);
    ve_Surfaces.air[TEAM_PLAYER][D_RIGHT] = surface_Rotate(ve_Surfaces.air[TEAM_PLAYER][D_UP], -PI_2, A_FREE);
    ve_Surfaces.air[TEAM_PLAYER][D_DOWN] = surface_Rotate(ve_Surfaces.air[TEAM_PLAYER][D_UP], PI, A_FREE);

    ve_Surfaces.airDamaged[TEAM_PLAYER][D_UP] = ve_Surfaces.air[TEAM_PLAYER][D_UP];
    ve_Surfaces.airDamaged[TEAM_PLAYER][D_LEFT] = ve_Surfaces.air[TEAM_PLAYER][D_LEFT];
    ve_Surfaces.airDamaged[TEAM_PLAYER][D_RIGHT] = ve_Surfaces.air[TEAM_PLAYER][D_RIGHT];
    ve_Surfaces.airDamaged[TEAM_PLAYER][D_DOWN] = ve_Surfaces.air[TEAM_PLAYER][D_DOWN];

    surf_Load(&ve_Surfaces.air[TEAM_1][D_UP], "Units/Air/eAircraft.png", A_FREE);
    ve_Surfaces.air[TEAM_1][D_LEFT] = surface_Rotate(ve_Surfaces.air[TEAM_1][D_UP], PI_2, A_FREE);
    ve_Surfaces.air[TEAM_1][D_RIGHT] = surface_Rotate(ve_Surfaces.air[TEAM_1][D_UP], -PI_2, A_FREE);
    ve_Surfaces.air[TEAM_1][D_DOWN] = surface_Rotate(ve_Surfaces.air[TEAM_1][D_UP], PI, A_FREE);

    ve_Surfaces.airDamaged[TEAM_1][D_UP] = ve_Surfaces.air[TEAM_1][D_UP];
    ve_Surfaces.airDamaged[TEAM_1][D_LEFT] = ve_Surfaces.air[TEAM_1][D_LEFT];
    ve_Surfaces.airDamaged[TEAM_1][D_RIGHT] = ve_Surfaces.air[TEAM_1][D_RIGHT];
    ve_Surfaces.airDamaged[TEAM_1][D_DOWN] = ve_Surfaces.air[TEAM_1][D_DOWN];

    surf_Load(&ve_Surfaces.airDestroyed[D_UP], "Units/Air/destroyedAircraft.png", A_FREE);
    ve_Surfaces.airDestroyed[D_LEFT] = surface_Rotate(ve_Surfaces.airDestroyed[D_UP], PI_2, A_FREE);
    ve_Surfaces.airDestroyed[D_RIGHT] = surface_Rotate(ve_Surfaces.airDestroyed[D_UP], -PI_2, A_FREE);
    ve_Surfaces.airDestroyed[D_DOWN] = surface_Rotate(ve_Surfaces.airDestroyed[D_UP], PI, A_FREE);


    /*Towers*/
    surf_Load(&ve_Surfaces.tower[TEAM_PLAYER][D_UP], "Units/Tower/aTower.png", A_FREE);
    ve_Surfaces.tower[TEAM_PLAYER][D_LEFT] = surface_Rotate(ve_Surfaces.tower[TEAM_PLAYER][D_UP], PI_2, A_FREE);
    ve_Surfaces.tower[TEAM_PLAYER][D_RIGHT] = surface_Rotate(ve_Surfaces.tower[TEAM_PLAYER][D_UP], -PI_2, A_FREE);
    ve_Surfaces.tower[TEAM_PLAYER][D_DOWN] = surface_Rotate(ve_Surfaces.tower[TEAM_PLAYER][D_UP], PI, A_FREE);

    ve_Surfaces.towerDamaged[TEAM_PLAYER][D_UP] = ve_Surfaces.tower[TEAM_PLAYER][D_UP];
    ve_Surfaces.towerDamaged[TEAM_PLAYER][D_LEFT] = ve_Surfaces.tower[TEAM_PLAYER][D_LEFT];
    ve_Surfaces.towerDamaged[TEAM_PLAYER][D_RIGHT] = ve_Surfaces.tower[TEAM_PLAYER][D_RIGHT];
    ve_Surfaces.towerDamaged[TEAM_PLAYER][D_DOWN] = ve_Surfaces.tower[TEAM_PLAYER][D_DOWN];

    surf_Load(&ve_Surfaces.tower[TEAM_1][D_UP], "Units/Tower/eTower.png", A_FREE);
    ve_Surfaces.tower[TEAM_1][D_LEFT] = surface_Rotate(ve_Surfaces.tower[TEAM_1][D_UP], PI_2, A_FREE);
    ve_Surfaces.tower[TEAM_1][D_RIGHT] = surface_Rotate(ve_Surfaces.tower[TEAM_1][D_UP], -PI_2, A_FREE);
    ve_Surfaces.tower[TEAM_1][D_DOWN] = surface_Rotate(ve_Surfaces.tower[TEAM_1][D_UP], PI, A_FREE);

    ve_Surfaces.towerDamaged[TEAM_1][D_UP] = ve_Surfaces.tower[TEAM_1][D_UP];
    ve_Surfaces.towerDamaged[TEAM_1][D_LEFT] = ve_Surfaces.tower[TEAM_1][D_LEFT];
    ve_Surfaces.towerDamaged[TEAM_1][D_RIGHT] = ve_Surfaces.tower[TEAM_1][D_RIGHT];
    ve_Surfaces.towerDamaged[TEAM_1][D_DOWN] = ve_Surfaces.tower[TEAM_1][D_DOWN];

    /*surf_Load(&ve_Surfaces.towerDestroyed[D_UP], "Units/Infantry/destroyedInfantry.png", A_FREE);
    ve_Surfaces.towerDestroyed[D_LEFT] = surface_Rotate(ve_Surfaces.towerDestroyed[D_UP], -PI_2, A_FREE);
    ve_Surfaces.towerDestroyed[D_RIGHT] = surface_Rotate(ve_Surfaces.towerDestroyed[D_UP], PI_2, A_FREE);
    ve_Surfaces.towerDestroyed[D_DOWN] = surface_Rotate(ve_Surfaces.towerDestroyed[D_UP], PI, A_FREE);
    */

    /*Fill the unit image array*/
    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        switch(x)
        {
            default:
            case UNIT_TANK:

            for(y = 0; y < 4; y++)
            {
                ve_Surfaces.units[x][0][y] = ve_Surfaces.tank[0][y];
                ve_Surfaces.units[x][1][y] = ve_Surfaces.tank[1][y];
            }

            break;


            case UNIT_INFANTRY:

            for(y = 0; y < 4; y++)
            {
                ve_Surfaces.units[x][0][y] = ve_Surfaces.infantry[0][y];
                ve_Surfaces.units[x][1][y] = ve_Surfaces.infantry[1][y];
            }

            break;

            case UNIT_TOWER:

            for(y = 0; y < 4; y++)
            {
                ve_Surfaces.units[x][0][y] = ve_Surfaces.tower[0][y];
                ve_Surfaces.units[x][1][y] = ve_Surfaces.tower[1][y];
            }

            break;

            case UNIT_AIR:

            for(y = 0; y < 4; y++)
            {
                ve_Surfaces.units[x][0][y] = ve_Surfaces.air[0][y];
                ve_Surfaces.units[x][1][y] = ve_Surfaces.air[1][y];
            }

            break;
        }
    }

    /*Buildings*/
    surf_Load(&ve_Surfaces.bCitadel[TEAM_PLAYER], "Level/Buildings/aCitadel.png", A_FREE);
    surf_Load(&ve_Surfaces.bCitadel[TEAM_1], "Level/Buildings/eCitadel.png", A_FREE);

    surf_Load(&ve_Surfaces.bPower[TEAM_PLAYER], "Level/Buildings/aPower.png", A_FREE);
    surf_Load(&ve_Surfaces.bPower[TEAM_1], "Level/Buildings/ePower.png", A_FREE);

    surf_Load(&ve_Surfaces.bInfantry[TEAM_PLAYER], "Level/Buildings/aInfantry.png", A_FREE);
    surf_Load(&ve_Surfaces.bInfantry[TEAM_1], "Level/Buildings/eInfantry.png", A_FREE);

    surf_Load(&ve_Surfaces.bTank[TEAM_PLAYER], "Level/Buildings/aTank.png", A_FREE);
    surf_Load(&ve_Surfaces.bTank[TEAM_1], "Level/Buildings/eTank.png", A_FREE);

    surf_Load(&ve_Surfaces.bAir[TEAM_PLAYER], "Level/Buildings/aAir.png", A_FREE);
    surf_Load(&ve_Surfaces.bAir[TEAM_1], "Level/Buildings/eAir.png", A_FREE);

    /*Tiles*/
    surf_Load(&ve_Surfaces.tileBuilding, "Level/buildingBase.png", A_FREE);
    surf_Load(&ve_Surfaces.tileCitadel, "Level/citadelBase.png", A_FREE);
    surf_Load(&ve_Surfaces.tileWater, "Level/Ground/water.png", A_FREE);
    surf_Load(&ve_Surfaces.tileGrass, "Level/Ground/grass.png", A_FREE);
    surf_Load(&ve_Surfaces.tileSnow, "Level/Ground/snow.png", A_FREE);
    surf_Load(&ve_Surfaces.tileSand, "Level/Ground/sand.png", A_FREE);

    surf_Load(&ve_Surfaces.tileRoad[0], "Level/Road/roadH.png", A_FREE);
    surf_Load(&ve_Surfaces.tileRoad[1], "Level/Road/roadCUL.png", A_FREE);

    surf_Load(&ve_Surfaces.tileWall, "Level/Wall/wall.png", A_FREE);

    surf_Load(&ve_Surfaces.unknown, "Level/noTile.png", A_FREE);

    /*Effects*/
    surf_Load(&ve_Surfaces.effectSmoke[0], "Effects/smoke1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectSmoke[1], "Effects/smoke2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectSmoke[2], "Effects/smoke3.png", A_FREE);

    surf_Load(&ve_Surfaces.effectExplosion[0], "Effects/explosion1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectExplosion[1], "Effects/explosion2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectExplosion[2], "Effects/explosion3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectExplosion[3], "Effects/explosion4.png", A_FREE);
    surf_Load(&ve_Surfaces.effectExplosion[4], "Effects/explosion5.png", A_FREE);

    surf_Load(&ve_Surfaces.effectSpawn[0], "Effects/spawn1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectSpawn[1], "Effects/spawn2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectSpawn[2], "Effects/spawn3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectSpawn[3], "Effects/spawn4.png", A_FREE);

    surf_Load(&ve_Surfaces.effectHeal[0], "Effects/hRegen1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectHeal[1], "Effects/hRegen2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectHeal[2], "Effects/hRegen3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectHeal[3], "Effects/hRegen4.png", A_FREE);

    surf_Load(&ve_Surfaces.effectHealRing[0], "Effects/healRing1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectHealRing[1], "Effects/healRing2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectHealRing[2], "Effects/healRing3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectHealRing[3], "Effects/healRing4.png", A_FREE);

    surf_Load(&ve_Surfaces.effectRepair[0], "Effects/bRegen1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectRepair[1], "Effects/bRegen2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectRepair[2], "Effects/bRegen3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectRepair[3], "Effects/bRegen4.png", A_FREE);

    surf_Load(&ve_Surfaces.effectFlame[0], "Effects/fire1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectFlame[1], "Effects/fire2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectFlame[2], "Effects/fire3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectFlame[3], "Effects/fire4.png", A_FREE);
    surf_Load(&ve_Surfaces.effectFlame[4], "Effects/fire5.png", A_FREE);
    surf_Load(&ve_Surfaces.effectFlame[5], "Effects/fire6.png", A_FREE);
    surf_Load(&ve_Surfaces.effectFlame[6], "Effects/fire7.png", A_FREE);

    surf_Load(&ve_Surfaces.effectCall[0], "Effects/uCall1.png", A_FREE);
    surf_Load(&ve_Surfaces.effectCall[1], "Effects/uCall2.png", A_FREE);
    surf_Load(&ve_Surfaces.effectCall[2], "Effects/uCall3.png", A_FREE);
    surf_Load(&ve_Surfaces.effectCall[3], "Effects/uCall4.png", A_FREE);

    return 0;
}

int vSprite_Init(void)
{
    int x = 0;

    /*Effects*/

    /*Explosion*/
    sprite_Setup(&ve_Sprites.unitExplosion, 0, VL_AIR, NULL, 5,
                 frame_CreateBasic(50, ve_Surfaces.effectExplosion[0], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectExplosion[1], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectExplosion[2], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectExplosion[3], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectExplosion[4], M_FREE)
    );

    /*Units destroyed*/
    sprite_Setup(&ve_Sprites.tankDestroyed, 0, VL_VEHICLE, NULL, 4,
                 frame_CreateBasic(0, ve_Surfaces.tankDestroyed[D_UP], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.tankDestroyed[D_DOWN], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.tankDestroyed[D_LEFT], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.tankDestroyed[D_RIGHT], M_FREE)
    );

    sprite_Setup(&ve_Sprites.airDestroyed, 0, VL_VEHICLE, NULL, 4,
                 frame_CreateBasic(0, ve_Surfaces.airDestroyed[D_UP], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.airDestroyed[D_DOWN], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.airDestroyed[D_LEFT], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.airDestroyed[D_RIGHT], M_FREE)
    );

    sprite_Setup(&ve_Sprites.infantryDestroyed, 0, VL_VEHICLE, NULL, 4,
                 frame_CreateBasic(0, ve_Surfaces.infantryDestroyed[D_UP], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.infantryDestroyed[D_DOWN], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.infantryDestroyed[D_LEFT], M_FREE),
                 frame_CreateBasic(0, ve_Surfaces.infantryDestroyed[D_RIGHT], M_FREE)
    );

    /*healing effect*/
    sprite_Setup(&ve_Sprites.heal, 0, VL_AIR, NULL, 4,
                 frame_CreateBasic(100, ve_Surfaces.effectHeal[0], M_FREE),
                 frame_CreateBasic(100, ve_Surfaces.effectHeal[1], M_FREE),
                 frame_CreateBasic(100, ve_Surfaces.effectHeal[2], M_FREE),
                 frame_CreateBasic(100, ve_Surfaces.effectHeal[3], M_FREE)
    );

    /*Building heal ring effect*/
    sprite_Setup(&ve_Sprites.healRing, 0, VL_AIR, NULL, 4,
                 frame_CreateBasic(100, ve_Surfaces.effectHealRing[0], M_FREE),
                 frame_CreateBasic(100, ve_Surfaces.effectHealRing[1], M_FREE),
                 frame_CreateBasic(100, ve_Surfaces.effectHealRing[2], M_FREE),
                 frame_CreateBasic(200, ve_Surfaces.effectHealRing[3], M_FREE)
    );

    /*Spawn effect*/
    sprite_Setup(&ve_Sprites.spawn, 0, VL_AIR, NULL, 4,
                 frame_CreateBasic(50, ve_Surfaces.effectSpawn[0], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectSpawn[1], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectSpawn[2], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectSpawn[3], M_FREE)
    );

    /*bullet hit effect*/
    sprite_Setup(&ve_Sprites.bulletHit, 0, VL_AIR, NULL, 3,
                 frame_CreateBasic(50, ve_Surfaces.effectSmoke[0], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectSmoke[1], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectSmoke[2], M_FREE)
    );

    /*flame effect*/
    sprite_Setup(&ve_Sprites.flame, 0, VL_AIR, NULL, 7,
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[0], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[1], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[2], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[3], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[4], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[5], M_FREE),
                 frame_CreateBasic(50, ve_Surfaces.effectFlame[6], M_FREE)
    );

    /*Building repair effect*/
    sprite_Setup(&ve_Sprites.repair, 0, VL_AIR, NULL, 4,
                 frame_CreateBasic(200, ve_Surfaces.effectRepair[0], M_FREE),
                 frame_CreateBasic(200, ve_Surfaces.effectRepair[1], M_FREE),
                 frame_CreateBasic(200, ve_Surfaces.effectRepair[2], M_FREE),
                 frame_CreateBasic(200, ve_Surfaces.effectRepair[3], M_FREE)
    );

    /*Unit call effect*/
    sprite_Setup(&ve_Sprites.call, 0, VL_HUD, NULL, 4,
                 frame_CreateBasic(200, ve_Surfaces.effectCall[0], M_FREE),
                 frame_CreateBasic(250, ve_Surfaces.effectCall[1], M_FREE),
                 frame_CreateBasic(100, ve_Surfaces.effectCall[2], M_FREE),
                 frame_CreateBasic(200, ve_Surfaces.effectCall[3], M_FREE)
    );

    /*GUI*/

    /*Check box*/

    sprite_Setup(&ve_Sprites.checkBox, 0, VL_HUD, NULL, 3,
               frame_CreateBasic(-1, surf_SimpleBox(14, 14, &colourGrey, &colourBlack, 1), A_FREE),
               frame_CreateBasic(0, NULL, M_FREE),
               frame_CreateBasic(0, surf_Offset(2, 2, surf_SimpleBox(10, 10, &colourBlack, &colourBlack, 1), 1), A_FREE)
    );

    /*Scroll buttons*/

    for(x = 0; x < 4; x++)
    {
        sprite_Setup(&ve_Sprites.scroll[x], 0, VL_HUD + 1, NULL, 3,
                    frame_CreateBasic(-1, ve_Surfaces.scroll[x], M_FREE),
                    frame_CreateBasic(0, NULL, M_FREE),
                    frame_CreateBasic(25, veMenu_SurfaceHoverShade(ve_Surfaces.scroll[x]->w, ve_Surfaces.scroll[x]->h, &colourWhite, 35, ve_Surfaces.scroll[x]), A_FREE)
        );
    }

    return 0;
}

int vSprite_Clean(void)
{
    int x = 0;

    /*Effects*/
    sprite_Clean(&ve_Sprites.unitExplosion);

    sprite_Clean(&ve_Sprites.tankDestroyed);
    sprite_Clean(&ve_Sprites.airDestroyed);
    sprite_Clean(&ve_Sprites.infantryDestroyed);

    sprite_Clean(&ve_Sprites.heal);
    sprite_Clean(&ve_Sprites.healRing);
    sprite_Clean(&ve_Sprites.repair);
    sprite_Clean(&ve_Sprites.spawn);
    sprite_Clean(&ve_Sprites.bulletHit);
    sprite_Clean(&ve_Sprites.flame);
    sprite_Clean(&ve_Sprites.call);

    /*UI*/
    sprite_Clean(&ve_Sprites.checkBox);

    for(x = 0; x < 4; x++)
    {
        sprite_Clean(&ve_Sprites.scroll[x]);
    }

    return 0;
}
