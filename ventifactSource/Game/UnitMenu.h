#ifndef UNITMENU_H
#define UNITMENU_H

#include "GUI/Spine.h"

#include "../Units/Units.h"
#include "../Defines.h"

enum
{
    UMENU_STATE_BASE = 0, /*Menu is at the top most state, choosing which units to buy or select*/
    UMENU_STATE_PLACE = 1, /*Menu is at the place state where you buy a unit at a certain position*/
    UMENU_STATE_SELECT = 2 /*Menu is at the state where you can select units and change their ai*/
};

enum
{
    UMENU_SUBSTATE_BUY = 0,
    UMENU_SUBSTATE_SELECT = 1
};

typedef struct vUnitMenu
{
    Ui_Spine spine; /*The main structure holding the menu*/
    Ui_Pointer pointer; /*Main mouse pointer*/

    Vector2DInt pntPos; /*Position of the pointer*/

    Sprite sUnitBox[UNIT_ENDTYPE]; /*Unit inside of box*/

    void *currentGame; /*Current game that is in progress*/

    int chosenUnitType; /*Current chosen unit type to buy*/

    int chosenAiType; /*Currently chosen ai type to give to a unit*/

    struct list *selectedUnits; /*List of selected units*/
    int selectRadius; /*The radius used for selecting units*/
    int numSelected; /*The number of units selected*/

    int state; /*State of the unit menu*/
    int subState; /*Sub state of the unit menu*/

    Control_Event cSelect; /*Control to use when a unit is bought/selected*/
    Control_Event cBack; /*Control to use to go back in the menu*/
    Control_Event cChangeAi; /*Control to change the ai to give the units*/

} Unit_Menu;

void vUnitMenu_InitBase(Unit_Menu *menu);
void vUnitMenu_InitPlace(Unit_Menu *menu);
void vUnitMenu_InitSelect(Unit_Menu *menu);

void vUnitMenu_GiveAi(Unit_Menu *menu, Vent_Unit *u, Vector2DInt *position);
void vUnitMenu_GiveAiList(Unit_Menu *menu, struct list *unitList);

void vUnitMenu_Init(Unit_Menu *menu, Base_State *bs);

void vUnitMenu_Setup(Unit_Menu *menu, int team, void *game);

void vUnitMenu_Update(Unit_Menu *menu, Base_State *bs, float deltaTime);

void vUnitMenu_Draw(Unit_Menu *menu, SDL_Surface *dest);

void vUnitMenu_Clean(Unit_Menu *menu);

void vUnitMenuState_Update(void *info);

void vUnitMenuState_Render(void *info);

int vUnitMenu_PurchaseShortcut(Unit_Menu *menu, int unitType, int posX, int posY);

#endif
