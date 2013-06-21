#ifndef BUILDINGBAR_H
#define BUILDINGBAR_H

#include "GUI/Spine.h"

#include "../Images.h"
#include "../Defines.h"
#include "../Level/Tiles.h"

enum Vent_BuildingBarTypes
{
    BUILDBAR_POWER = 0,
    BUILDBAR_INFANTRY = 1,
    BUILDBAR_TANK = 2,
    BUILDBAR_AIRCRAFT = 3,
    BUILDBAR_CITADEL = 4,
    BUILDBAR_TOTAL = 5
};

typedef struct Vent_BuildingBar
{
    Ui_Spine window;

    int barX;
    int barY;

    int buildingTypes[BUILDING_TOTAL];

    int buttonWidth;
    int buttonHeight;
    int buttonSpacing;
    int buttonScrollStartX;
    int buttonScrollStartY;

    int selection; //What buildings will be on display

    int chosenBuilding; //The building the user wants to create

    Vent_Tile *buildTile;

} Vent_BuildingBar;

void vBuildingBar_Start(Vent_BuildingBar *b, Base_State *s, Vent_Tile *buildTile);

void vBuildingBar_Setup(Vent_BuildingBar *b);

void vBuildingBar_Update(Vent_BuildingBar *b);

void vBuildingBar_Clean(Vent_BuildingBar *b);

void vBuildingBar_Draw(Vent_BuildingBar *b, SDL_Surface *dest);

/* The states */
void vBuildingBarState_Render(void *info);
void vBuildingBarState_Update(void *info);
void vBuildingBarState_Controls(void *info);

#endif
