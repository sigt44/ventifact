#include "HUD.h"

#include "Kernel/Kernel_State.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "Graphics/SurfaceManager.h"
#include "Font.h"

#include "../Images.h"
#include "../Defines.h"
#include "../Menus/MenuAttributes.h"
#include "VentGame.h"
#include "../Buildings/Buildings.h"
#include "Sides.h"

static void vHUD_SetupControls(Vent_HUD *h)
{
    control_Setup(&h->cSelectUp, "Up", &h->timer, 200);
    control_Setup(&h->cSelectDown, "Down", &h->timer, 200);
    control_Setup(&h->cSelectLeft, "Left", &h->timer, 200);
    control_Setup(&h->cSelectRight, "Right", &h->timer, 200);
    control_Setup(&h->cSelect, "Select", &h->timer, 500);
    control_Setup(&h->cBack, "Back", &h->timer, 500);

    control_CopyKey(&h->cSelectUp, &ve_Menu.c_MoveUp);
    control_CopyKey(&h->cSelectDown, &ve_Menu.c_MoveDown);
    control_CopyKey(&h->cSelectLeft, &ve_Menu.c_MoveLeft);
    control_CopyKey(&h->cSelectRight, &ve_Menu.c_MoveRight);

    control_CopyKey(&h->cSelect, &ve_Menu.c_Activate);
    control_CopyKey(&h->cBack, &ve_Menu.c_Back);

    return;
}

void vHUD_Setup(Vent_HUD *h, void *game, Vent_Side *s, Timer *srcTime)
{
    h->srcTime = srcTime;
    h->timer = timer_Setup(NULL, 0, 0, 1);

    vHUD_SetupControls(h);

    h->chosenUnit = UNIT_TANK;
    h->HUDLayer = VL_AIR + 1;

    /*Building icon*/
    sprite_Setup(&h->buildingIcon, 0, h->HUDLayer, srcTime, 5,
        frame_CreateBasic(300, ve_Surfaces.buildingIcon[0], M_FREE),
        frame_CreateBasic(300, ve_Surfaces.buildingIcon[1], M_FREE),
        frame_CreateBasic(300, ve_Surfaces.buildingIcon[2], M_FREE),
        frame_CreateBasic(300, ve_Surfaces.buildingIcon[3], M_FREE),
        frame_CreateBasic(300, ve_Surfaces.buildingIcon[4], M_FREE)
    );

    /*Unit info sprites*/
    sprite_Setup(&h->tankInfo, 0, h->HUDLayer, srcTime, 1,
        frame_CreateBasic(0, text_Arg(font_Get(1, 11), &tColourWhite, "T %d", s->readyUnits[UNIT_TANK]), A_FREE));
    h->prevTankAmount = s->readyUnits[UNIT_TANK];

    sprite_Setup(&h->infantryInfo, 0, h->HUDLayer, srcTime, 1,
        frame_CreateBasic(0, text_Arg(font_Get(1, 11), &tColourWhite, "I %d", s->readyUnits[UNIT_INFANTRY]), A_FREE));
    h->prevInfantryAmount = s->readyUnits[UNIT_INFANTRY];

    sprite_Setup(&h->aircraftInfo, 0, h->HUDLayer, srcTime, 1,
        frame_CreateBasic(0, text_Arg(font_Get(1, 11), &tColourWhite, "A %d", s->readyUnits[UNIT_AIR]), A_FREE));
    h->prevAircraftAmount = s->readyUnits[UNIT_AIR];

    sprite_Setup(&h->unitSelect, 0, h->HUDLayer, srcTime, 1,
        frame_CreateBasic(0, ve_Surfaces.smallBox, M_FREE));

    sprite_Setup(&h->optionBar, 0, h->HUDLayer, srcTime, 1,
        frame_CreateBasic(0, ve_Surfaces.optionBar, M_FREE));

    sprite_Setup(&h->healthBar, 0, h->HUDLayer, srcTime, 3,
        frame_CreateBasic(0, ve_Surfaces.healthBar[2], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.healthBar[1], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.healthBar[0], M_FREE));

    sprite_Setup(&h->healthOverlay, 0, h->HUDLayer, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.healthBarOverlay, M_FREE));

    sprite_Setup(&h->weapon, 0, h->HUDLayer, srcTime, VENT_TOTALWEAPONS,
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER]->w/2, 7 - ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER]->h/2, ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER], M_FREE), 0, 0, 16, 15, A_FREE),
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER]->w/2, 7 - ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER]->h/2, ve_Surfaces.plasmaBullet[HORIZONTAL][TEAM_PLAYER], M_FREE), 0, 0, 16, 15, A_FREE),
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.missileBullet[HORIZONTAL]->w/2, 7 - ve_Surfaces.missileBullet[HORIZONTAL]->h/2, ve_Surfaces.missileBullet[HORIZONTAL], M_FREE), 0, 0, 16, 15, A_FREE),
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.rocketBullet[HORIZONTAL]->w/2, 7 - ve_Surfaces.rocketBullet[HORIZONTAL]->h/2, ve_Surfaces.rocketBullet[HORIZONTAL], M_FREE), 0, 0, 16, 15, A_FREE),
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.repairBullet[HORIZONTAL]->w/2, 7 - ve_Surfaces.repairBullet[HORIZONTAL]->h/2, ve_Surfaces.repairBullet[HORIZONTAL], M_FREE), 0, 0, 16, 15, A_FREE),
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.flameBullet->w/2, 7 - ve_Surfaces.flameBullet->h/2, ve_Surfaces.flameBullet, M_FREE), 0, 0, 16, 15, A_FREE),
        frame_CreateClip(0, surf_Offset(10 - ve_Surfaces.mineBullet[TEAM_PLAYER]->w/2, 7 - ve_Surfaces.mineBullet[TEAM_PLAYER]->h/2, ve_Surfaces.mineBullet[TEAM_PLAYER], M_FREE), 0, 0, 16, 15, A_FREE)
    );

    sprite_Setup(&h->gameBar, 0, h->HUDLayer, srcTime, 1, frame_CreateBasic(0, ve_Surfaces.gameBar, M_FREE));
    sprite_SetAlpha(&h->gameBar, SDL_FAST_ALPHA);

    /*Setup the building purchase bar*/
    vBuildingBar_Setup(&h->buildBar);

    /*Setup the unit menu*/
    vUnitMenu_Setup(&h->unitMenu, s->team, game);

    vHUD_SetPositions(h);

    return;
}

void vHUD_SetPositions(Vent_HUD *h)
{
    /*Position of the game bar*/
    sprite_SetPos(&h->gameBar, 0, ker_Screen_Height() - sprite_Height(&h->gameBar));

    /*Position of the health bar, relative to the game bar*/
    sprite_SetPos(&h->healthBar, h->gameBar.x + 5, h->gameBar.y + 3);
    sprite_SetPos(&h->healthOverlay, h->gameBar.x + 6, h->gameBar.y + 4);

    /*Position of the building icon, relative to the health bar*/
    sprite_SetPos(&h->buildingIcon, h->healthOverlay.x + sprite_Width(&h->healthOverlay) + 6, h->gameBar.y + 3);

    /*Position of the weapon bar, relative to the building icon*/
    sprite_SetPos(&h->weapon,
        h->buildingIcon.x + sprite_Width(&h->buildingIcon) + 6,
        h->gameBar.y +  (sprite_Height(&h->gameBar)/2) - 7 + 1);

    /*Position of the unit info bars, relative to the weapon bar*/
    sprite_SetPos(&h->tankInfo, h->weapon.x + sprite_Width(&h->weapon) + 48, h->gameBar.y + (sprite_Height(&h->tankInfo)/2) - 4);
    sprite_SetPos(&h->infantryInfo, h->tankInfo.x + sprite_Width(&h->tankInfo) + 10, h->gameBar.y + (sprite_Height(&h->infantryInfo)/2) - 4);
    sprite_SetPos(&h->aircraftInfo, h->infantryInfo.x + sprite_Width(&h->infantryInfo) + 10, h->gameBar.y + (sprite_Height(&h->aircraftInfo)/2) - 4);

    /*Position of the option bar*/
    sprite_SetPos(&h->optionBar, 118, 34);

    return;
}

void vHUD_Draw(Vent_HUD *h, void *game, SDL_Surface *dest)
{
    int width;
    float healthRatio;
    Vent_Game *g = game;
    Vent_Unit *playerUnit = g->playerUnit;
    Vent_Side *side = &g->side[playerUnit->team];

    sprite_Draw(&h->gameBar, dest);

    //sprite_Draw(&h->optionBar, dest);
    healthRatio = (float)playerUnit->health/playerUnit->healthStarting;

    if(healthRatio <= 0.25)
    {
        sprite_SetFrame(&h->healthBar, 2);
    }
    else if(healthRatio <= 0.50)
    {
        sprite_SetFrame(&h->healthBar, 1);
    }
    else
        sprite_SetFrame(&h->healthBar, 0);

    sprite_Draw(&h->healthBar, dest);

    width = (int)(healthRatio * spriteFrame_MaxWidth(&h->healthOverlay));

    if(width > 0)
    {
        sprite_SetCurrentClip(&h->healthOverlay, 0, 0, width, sprite_Height(&h->healthOverlay));
        sprite_Draw(&h->healthOverlay, dest);
    }

    /*Draw credit amount*/
    text_Draw_Arg(h->gameBar.x + 270, h->gameBar.y + 2, dest, font_Get(1, 11), &tColourWhite, VL_AIR + 1, 0, "$: %d", g->side[playerUnit->team].credits);

    if(g->side[playerUnit->team].credits > g->side[playerUnit->team].lastFrameCredits)
        text_Draw_Arg(h->gameBar.x + 280, h->gameBar.y - 15, dest, font_Get(1, 11), &tColourGreen, VL_AIR + 2, 500, "+ %d", g->side[playerUnit->team].credits - g->side[playerUnit->team].lastFrameCredits);
    else if(g->side[playerUnit->team].credits < g->side[playerUnit->team].lastFrameCredits)
                text_Draw_Arg(h->gameBar.x + 280, h->gameBar.y - 15, dest, font_Get(1, 11), &tColourRed, VL_AIR + 2, 500, "- %d", g->side[playerUnit->team].lastFrameCredits - g->side[playerUnit->team].credits);

    g->side[playerUnit->team].lastFrameCredits = g->side[playerUnit->team].credits;

    /*Draw unit info*/
    if(h->prevTankAmount != side->readyUnits[UNIT_TANK])
    {
       h->prevTankAmount = side->readyUnits[UNIT_TANK];
       sprite_ReplaceFrame(&h->tankInfo, frame_CreateBasic(0, text_Arg(font_Get(1, 11), &tColourWhite, "T %d", side->readyUnits[UNIT_TANK]), A_FREE), 0);

       sprite_SetPos(&h->infantryInfo, h->tankInfo.x + sprite_Width(&h->tankInfo) + 10, h->gameBar.y + (sprite_Height(&h->infantryInfo)/2) - 4);
       sprite_SetPos(&h->aircraftInfo, h->infantryInfo.x + sprite_Width(&h->infantryInfo) + 10, h->gameBar.y + (sprite_Height(&h->aircraftInfo)/2) - 4);

    }

    if(h->prevInfantryAmount != side->readyUnits[UNIT_INFANTRY])
    {
       h->prevInfantryAmount = side->readyUnits[UNIT_INFANTRY];
       sprite_ReplaceFrame(&h->infantryInfo, frame_CreateBasic(0, text_Arg(font_Get(1, 11), &tColourWhite, "I %d", side->readyUnits[UNIT_INFANTRY]), A_FREE), 0);

       sprite_SetPos(&h->aircraftInfo, h->infantryInfo.x + sprite_Width(&h->infantryInfo) + 10, h->gameBar.y + (sprite_Height(&h->aircraftInfo)/2) - 4);

    }

    if(h->prevAircraftAmount != side->readyUnits[UNIT_AIR])
    {
       h->prevAircraftAmount = side->readyUnits[UNIT_AIR];
       sprite_ReplaceFrame(&h->aircraftInfo, frame_CreateBasic(0, text_Arg(font_Get(1, 11), &tColourWhite, "A %d", side->readyUnits[UNIT_AIR]), A_FREE), 0);
    }

    sprite_Draw(&h->tankInfo, dest);
    sprite_Draw(&h->infantryInfo, dest);
    sprite_Draw(&h->aircraftInfo, dest);

    /*Draw weapon info*/
    sprite_SetFrame(&h->weapon, playerUnit->weapon.bulletType);

    sprite_Draw(&h->weapon, dest);

    /*If ammo is not infinite draw the amount remaining*/
    if(playerUnit->weapon.ammo[playerUnit->weapon.bulletType] >= 0)
        text_Draw_Arg(h->weapon.x + sprite_Width(&h->weapon) + 10, h->gameBar.y + 2, dest, font_Get(1, 11), &tColourWhite, VL_AIR + 1, 0, "%d", playerUnit->weapon.ammo[playerUnit->weapon.bulletType]);
    else
        surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_AIR + 1, 0, h->weapon.x + sprite_Width(&h->weapon) + 10, h->gameBar.y + 6, ve_Surfaces.infinity, ker_Screen(), NULL);

    /*Draw building icon if player is somewhere that can be built on*/
    if(vBuilding_CanBuild(playerUnit, game) == 1)
    {
        sprite_Draw(&h->buildingIcon, dest);
    }

    return;
}

void vHUD_Clean(Vent_HUD *h)
{
    vBuildingBar_Clean(&h->buildBar);
    vUnitMenu_Clean(&h->unitMenu);

    sprite_Clean(&h->optionBar);
    sprite_Clean(&h->unitSelect);

    sprite_Clean(&h->buildingIcon);

    sprite_Clean(&h->healthBar);
    sprite_Clean(&h->healthOverlay);

    sprite_Clean(&h->gameBar);

    sprite_Clean(&h->weapon);

    sprite_Clean(&h->tankInfo);
    sprite_Clean(&h->infantryInfo);
    sprite_Clean(&h->aircraftInfo);

    control_Clean(&h->cSelectUp);
    control_Clean(&h->cSelectDown);
    control_Clean(&h->cSelectLeft);
    control_Clean(&h->cSelectRight);
    control_Clean(&h->cSelect);
    control_Clean(&h->cBack);

    return;
}
