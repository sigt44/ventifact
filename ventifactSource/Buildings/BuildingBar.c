#include "BuildingBar.h"

#include "GUI/ButtonFunc.h"
#include "GUI/ButtonScroll.h"
#include "GUI/Button.h"
#include "DataStruct.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "Graphics/SurfaceManager.h"
#include "Kernel/Kernel_State.h"
#include "Sound.h"
#include "Font.h"

#include "Buildings.h"
#include "../Menus/MenuAttributes.h"
#include "../Images.h"
#include "../Sounds.h"
#include "../Defines.h"
#include "../Game/VentGame.h"
#include "../Game/State.h"

static const int buildingIdendifier[BUILDBAR_TOTAL] = {
    BUILDING_POWER,
    BUILDING_INFANTRY,
    BUILDING_TANK,
    BUILDING_AIR,
    BUILDING_CITADEL
};

static SDL_Surface **buildingGraphicIdentifier[BUILDBAR_TOTAL] = {
    &ve_Surfaces.bPower[TEAM_PLAYER],
    &ve_Surfaces.bInfantry[TEAM_PLAYER],
    &ve_Surfaces.bTank[TEAM_PLAYER],
    &ve_Surfaces.bAir[TEAM_PLAYER],
    &ve_Surfaces.bCitadel[TEAM_PLAYER]
};


static void vBuilding_ButtonActivate(Ui_Button *button)
{
    int *chosenBuilding = button->info->dataArray[0];
    int *assignBuilding = button->info->dataArray[1];

    *chosenBuilding = *assignBuilding;

    return;
}

static void vBuilding_ButtonHighlight(Ui_Button *button)
{
    int *chosenBuilding = button->info->dataArray[0];
    int *assignBuilding = button->info->dataArray[1];

    *chosenBuilding = *assignBuilding;

    return;
}

void vBuildingBar_Setup(Vent_BuildingBar *b)
{
    Ui_ButtonScroll *buildScroll = NULL;
    Ui_Button *buildingButton[BUILDBAR_TOTAL];
    Ui_Button *buildingTrav[2];

    Sprite *buildSprite[BUILDBAR_TOTAL];

    int x = 0;

    b->buttonWidth = 40;
    b->buttonHeight = 40;
    b->buttonSpacing = 20;

    b->barX = (ker_Screen_Width() - ve_Surfaces.buildBar->w)/2;
    b->barY = (ker_Screen_Height()/2) - (ve_Surfaces.buildBar->h/2);

    b->buttonScrollStartX = 40;
    b->buttonScrollStartY = 20;

    for(x = 0; x < BUILDBAR_TOTAL; x++)
    {
        b->buildingTypes[x] = buildingIdendifier[x];

        buildSprite[x] = sprite_Create();
        sprite_Setup(buildSprite[x], 1, VL_HUD + 1, NULL, 1, frame_CreateClip(0, surf_Copy(*buildingGraphicIdentifier[x]), 0, (*buildingGraphicIdentifier[x])->h - b->buttonHeight, b->buttonWidth, b->buttonHeight, A_FREE));

        buildingButton[x] = uiButton_Create(0, 0, b->buttonWidth, b->buttonHeight, &vBuilding_ButtonActivate, &vBuilding_ButtonHighlight, dataStruct_Create(2, &b->chosenBuilding, &b->buildingTypes[x]), buildSprite[x], 0, 0, NULL);
    }


    buildingTrav[0] = uiButton_Create(0, 0, 0, 0, NULL, NULL, NULL, NULL, 1, 0, NULL);
    control_SetRepeat(&buildingTrav[0]->cActivate[BC_POINTER], 250);
    control_SetRepeat(&buildingTrav[0]->cActivate[BC_KEY], 250);
    buildingTrav[1] = uiButton_Create(0, 0, 0, 0, NULL, NULL, NULL, NULL, 1, 0, NULL);
    control_SetRepeat(&buildingTrav[1]->cActivate[BC_POINTER], 250);
    control_SetRepeat(&buildingTrav[1]->cActivate[BC_KEY], 250);

    for(x = 0; x < BUILDBAR_TOTAL; x++)
    {
        uiButton_CopyControl(buildingButton[x], &ve_Menu.c_Activate, BC_POINTER);
        uiButton_CopyControl(buildingButton[x], &ve_Menu.c_Activate, BC_KEY);
    }

    uiButton_CopyControl(buildingTrav[0], &ve_Menu.c_MoveRight, BC_KEY);
    uiButton_CopyControl(buildingTrav[1], &ve_Menu.c_MoveLeft, BC_KEY);

    buildScroll = uiButtonScroll_Create(b->buttonScrollStartX, b->buttonScrollStartY, SCR_H, b->buttonSpacing, 3, 1, 70, buildingTrav[0], buildingTrav[1], BUILDBAR_TOTAL,
        buildingButton[BUILDBAR_POWER],
        buildingButton[BUILDBAR_TANK],
        buildingButton[BUILDBAR_INFANTRY],
        buildingButton[BUILDBAR_AIRCRAFT],
        buildingButton[BUILDBAR_CITADEL]
        );

    /*Set the middle of scroll to be always hovered*/
    uiButtonScroll_ForceSelect(buildScroll, 1);

    uiSpine_Setup(&b->window, VL_HUD + 1, b->barX, b->barY, frame_CreateBasic(0, ve_Surfaces.buildBar, M_FREE), NULL, 0, NULL, UI_SPINE_ALLFLAGS);

    flag_Off(&b->window.flags, UI_SPINE_UPDATEPOINTER);

    SDL_SetAlpha(ve_Surfaces.buildBar, SDL_SRCALPHA | SDL_RLEACCEL, 200);

    uiSpine_AddScroll(&b->window, buildScroll, "Scroll:Building");

    return;
}

void vBuildingBar_Start(Vent_BuildingBar *b, Base_State *s, Vent_Tile *buildTile)
{
    int tileType = buildTile->base.type;

    b->buildTile = buildTile;

    if(tileType == TILE_CITADELBASE)
        b->chosenBuilding = b->selection = BUILDING_CITADEL;
    else
        b->chosenBuilding = b->selection = BUILDING_POWER;

    s->render = &vBuildingBarState_Render;
    s->controls = &vBuildingBarState_Controls;
    s->logic = &vBuildingBarState_Update;

    return;
}

static int vBuildingBar_CheckBuy(int selection, int chosenBuilding, Vent_Game *game, Vent_Tile *t)
{
    Vent_Unit *u = game->playerUnit;

    Vent_Building *citadel = t->citadel;

    if(selection == BUILDING_CITADEL && chosenBuilding != BUILDING_CITADEL)
        printf("Only a citadel can be built here.\n");
    else if(selection == BUILDING_POWER && chosenBuilding == BUILDING_CITADEL)
        printf("You can't build a citadel here.\n");
    else if(chosenBuilding != BUILDING_CITADEL && (citadel == NULL || (int)u->team != citadel->team)) //If there is no citadel linked up or there is, but it is on the wrong team
        printf("No friendly citadel is linked.\n");
    else if(!vBuilding_Buy(t, chosenBuilding, &game->side[TEAM_PLAYER]))
        printf("Not enough cash!\n");
    else
        return 1;

    return 0;
}

void vBuildingBarState_Controls(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    Vent_HUD *hud = &vgs->game.HUD;
    Vent_BuildingBar *b = &vgs->game.HUD.buildBar;

    timer_Calc(&hud->timer);

    if(control_IsActivated(&hud->cBack))
    {
        timer_Resume(&vgs->game.gTimer);

        bs->render = &Game_Render;
        bs->controls = &Game_Controls;
        bs->logic = &Game_Logic;

        return;
    }

    if(control_IsActivated(&hud->cSelect))
    {
        vBuildingBar_Update(&hud->buildBar);

        if(vBuildingBar_CheckBuy(b->selection, b->chosenBuilding, &vgs->game, b->buildTile) == 1)
        {
            sound_Call(ve_Sounds.menuButtonSelect, -1, SND_DUPLICATE, -1, 0);
            vBuilding_Build(vgs->game.playerUnit->team, b->chosenBuilding, b->buildTile, &vgs->game);
        }

        timer_Resume(&vgs->game.gTimer);

        bs->render = &Game_Render;
        bs->controls = &Game_Controls;
        bs->logic = &Game_Logic;
    }

    return;
}

void vBuildingBarState_Update(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    vBuildingBar_Update(&vgs->game.HUD.buildBar);

    return;
}

void vBuildingBarState_Render(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    Game_Render(info);

    vBuildingBar_Draw(&vgs->game.HUD.buildBar, ker_Screen());

    return;
}


void vBuildingBar_Update(Vent_BuildingBar *b)
{
    uiSpine_Update(&b->window, 0);

    return;
}

void vBuildingBar_Draw(Vent_BuildingBar *b, SDL_Surface *dest)
{
    SDL_Colour white = {255, 255, 255, 0};
    //surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_BUILDING + 1, 0, b->barX, b->barY, ve_Surfaces.buildBar, dest, NULL);

    uiSpine_Draw(&b->window, dest);

    //Draw the name and price of the current selected building
    text_Draw_Arg(b->barX + 250, b->barY + 23, dest, font_Get(1, 11), &white, VL_HUD + 1, 0, "%s", ventBuilding_Names[b->chosenBuilding]);
    text_Draw_Arg(b->barX + 250, b->barY + 44, dest, font_Get(1, 11), &white, VL_HUD + 1, 0, "$%d", ventBuilding_Prices[b->chosenBuilding]);

    return;
}

void vBuildingBar_Clean(Vent_BuildingBar *b)
{
    uiSpine_Clean(&b->window);

    return;
}

