#ifndef HUD_H
#define HUD_H

#include "Graphics/Sprite.h"
#include "Time/Timer.h"
#include "Controls.h"
#include "GUI/Button.h"
#include "GUI/ButtonChooser.h"
#include "GUI/Spine.h"

#include "Sides.h"
#include "../Buildings/BuildingBar.h"
#include "UnitMenu.h"

typedef struct Vent_HUD
{
    Ui_Spine spine;
    Ui_ButtonChooser buttonChooser;

    Sprite optionBar;
    Sprite unitSelect;

    Sprite healthBar;
    Sprite healthOverlay;

    Sprite tankInfo;
    int prevTankAmount;

    Sprite infantryInfo;
    int prevInfantryAmount;

    Sprite aircraftInfo;
    int prevAircraftAmount;

    Sprite buildingIcon;

    Sprite gameBar;

    Sprite weapon;

    Control_Event cSelectUp;
    Control_Event cSelectDown;
    Control_Event cSelectLeft;
    Control_Event cSelectRight;
    Control_Event cSelect;
    Control_Event cBack;

    int chosenUnit;
    int HUDLayer;

    Vent_BuildingBar buildBar;
    Unit_Menu unitMenu;

    Timer *srcTime;
    Timer timer;

} Vent_HUD;

void vHUD_Setup(Vent_HUD *h, void *game, Vent_Side *s, Timer *srcTime);

void vHUD_SetPositions(Vent_HUD *h);

void vHUD_Draw(Vent_HUD *h, void *game, SDL_Surface *dest);

void vHUD_DrawUnitSelection(Vent_HUD *h, Vent_Side *s, SDL_Surface *dest);

void vHUD_Clean(Vent_HUD *h);

#endif
