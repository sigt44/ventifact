#include "UnitMenu.h"

#include "Graphics/Sprite.h"
#include "GUI/ButtonFunc.h"
#include "GUI/Spine.h"
#include "GUI/ButtonPaths.h"
#include "Collision.h"

#include "../Menus/MenuAttributes.h"
#include "../ControlMap.h"
#include "VentGame.h"
#include "State.h"
#include "../Ai/Units/Ai_UnitAttack.h"
#include "../Ai/Units/Ai_UnitFunction.h"
#include "../Ai/Units/Ai_UnitMove.h"

const int totalMenuAi = 3;

const char *aiUnitMenuNames[3] = {"Follow",
                                    "Hold area",
                                    "Buy citadel"
};

enum
{
    MENU_AI_FOLLOW = 0,
    MENU_AI_HOLD = 1,
    MENU_AI_CITADEL = 2
};

/*Start button functions - */

/*Hover over the unit select button*/
static void UM_SelectUnitType(Ui_Button *button)
{
    Data_Struct *uInfo = button->info;
    Unit_Menu *menu = uInfo->dataArray[0];
    int unitType = (int)uInfo->dataArray[1];

    menu->chosenUnitType = unitType;

    /*Force the origin of the state scroll to be the buying state*/
    //uiButtonScroll_SetOrigin(uiSpine_GetEntity(&menu->spine, "Scroll:State"), 0);

    //printf("Unit type chosen %d\n", menu->chosenUnitType);

    return;
}

/*Hover over the invisible scroll traverse buttons*/
static void BFH_ScrollTraverse(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = (Ui_ButtonScroll *)button->info->dataArray[0];
    MABFH_HoverLine(button);
    uiButtonScroll_ForceSelect(scroll, 0);

    if(control_IsActivated(&ve_Menu.c_MoveLeft) == 1)
    {
        uiButtonScroll_DecreaseSelected(scroll);
    }
    else if(control_IsActivated(&ve_Menu.c_MoveRight) == 1)
    {
        uiButtonScroll_IncreaseSelected(scroll);
    }

    return;
}

/*Hover over the ai type button*/
static void UM_SelectAiType(Ui_Button *button)
{
    Data_Struct *uInfo = button->info;
    Unit_Menu *menu = uInfo->dataArray[0];
    int aiType = (int)uInfo->dataArray[1];

    menu->chosenAiType = aiType;

    return;
}

/*Activate on the state button*/
static void BFH_HoverState(Ui_Button *button)
{
    Data_Struct *uInfo = button->info;
    Unit_Menu *menu = uInfo->dataArray[0];
    int subState = (int)uInfo->dataArray[1];

    if(menu->subState != subState)
    {
        menu->subState = subState;

        switch(subState)
        {
            case UMENU_SUBSTATE_BUY:
            uiSpine_OpenTemplate(&menu->spine, "BuyMenu", SELECT_ON);
            break;

            case UMENU_SUBSTATE_SELECT:
            uiSpine_OpenTemplate(&menu->spine, "SelectMenu", SELECT_ON);
            break;
        }
    }


    return;
}

static void UM_SetState(Unit_Menu *menu, int state)
{
    menu->state = state;

    switch(menu->state)
    {
        case UMENU_STATE_BASE:

        vUnitMenu_InitBase(menu);

        break;

        case UMENU_STATE_PLACE:

        vUnitMenu_InitPlace(menu);

        break;

        case UMENU_STATE_SELECT:

        vUnitMenu_InitSelect(menu);

        break;
    }

    return;
}

/* End button functions*/

/*
    Function: vUnitMenu_SetupControls

    Description -
    Setup and assign controls for the unit menu.

    1 argument:
    Unit_Menu *menu - The unit menu to have its controls setup.
*/
static void vUnitMenu_SetupControls(Unit_Menu *menu)
{
    int x = 0;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;
    Ui_Spine_Template *sTemplate[2] = {NULL, NULL};

    control_Setup(&menu->cSelect, "Select", &menu->spine.sTimer, 200);
    control_Setup(&menu->cBack, "Back", &menu->spine.sTimer, 200);
    control_Setup(&menu->cChangeAi, "Change Ai", &menu->spine.sTimer, 200);

    control_CopyKey(&menu->cSelect, &ve_Menu.c_Activate);
    control_CopyKey(&menu->cSelect, &ve_Menu.c_PointerActivate);
    control_CopyKey(&menu->cChangeAi, &ve_Controls.cMenuBackward);

    control_CopyKey(&menu->cBack, &ve_Menu.c_Back);

    /*Set the keys to move the pointer*/
    uiPointer_SetControls(menu->spine.pnt, 200.0f,
                                            &ve_Controls.cMoveUp,
                                            &ve_Controls.cMoveDown,
                                            &ve_Controls.cMoveLeft,
                                            &ve_Controls.cMoveRight);

    /*Set the boundry of the pointer to allow checks for scrolling the map*/
    uiPointer_SetBoundry(menu->spine.pnt, ker_Screen_Width(), ker_Screen_Height());

    /*Setup the button chooser */
    sTemplate[0] = uiSpine_GetTemplate(&menu->spine, "BuyMenu");
    sTemplate[1] = uiSpine_GetTemplate(&menu->spine, "SelectMenu");

    veMenu_SetMapControl(&sTemplate[0]->map, 0);
    uiMap_SetDefaultPath(&sTemplate[0]->map, uiSpine_GetEntityBase(&menu->spine, "Scroll:State"));

    veMenu_SetMapControl(&sTemplate[1]->map, 0);
    uiMap_SetDefaultPath(&sTemplate[1]->map, uiSpine_GetEntityBase(&menu->spine, "Scroll:State"));

    veMenu_SetSpineControl(&menu->spine);
    uiSpine_MapEntity(&menu->spine);

    return;
}

/*
    Function: vUnitMenu_InitBase

    Description -
    Initialise the menu to be in the base state. The state where the player can
    select which units to buy and set the ai.

    1 argument:
    Unit_Menu *menu - The unit menu to initialise to the base state.
*/
void vUnitMenu_InitBase(Unit_Menu *menu)
{
    Ui_Spine_Template *sTemplate = NULL;
    menu->state = UMENU_STATE_BASE;

    /*Enable the overlay*/
    switch(menu->subState)
    {
        case UMENU_SUBSTATE_BUY:
        uiSpine_OpenTemplate(&menu->spine, "BuyMenu", SELECT_ON);
        break;

        case UMENU_SUBSTATE_SELECT:
        uiSpine_OpenTemplate(&menu->spine, "SelectMenu", SELECT_ON);
        break;
    }

    /*Ensure that the mouse pointer is off*/
    uiPointer_Clamp(&menu->pointer);
    uiPointer_SetDraw(&menu->pointer, 0);

    /*Force the origin of the state scroll to be the buying state*/
    //uiButtonScroll_SetOrigin(uiSpine_GetEntity(&menu->spine, "Scroll:State"), 0);

    return;
}

/*
    Function: vUnitMenu_InitPlace

    Description -
    Initialise the menu to be in the place state. The state where the player can
    select where to buy the unit.

    1 argument:
    Unit_Menu *menu - The unit menu to initialise to the place state.
*/
void vUnitMenu_InitPlace(Unit_Menu *menu)
{
    Vent_Game *game = menu->currentGame;
    Camera_2D *camera = &game->camera;
    Vent_Unit *playerUnit = game->playerUnit;

    menu->state = UMENU_STATE_PLACE;

    /*Disable the overlay*/
    uiSpine_OpenTemplate(&menu->spine, "Place", SELECT_ON);

    /*Ensure that the mouse pointer is on*/
    uiPointer_Unclamp(&menu->pointer);
    uiPointer_SetDraw(&menu->pointer, 1);

    /*Force the origin of the state scroll to be the place state*/
    //uiButtonScroll_SetOrigin(uiSpine_GetEntity(&menu->spine, "Scroll:State"), 1);

    /*Set the pointer to the player*/
    uiPointer_SetPos(menu->spine.pnt, 1, playerUnit->middlePosition.x - camera->iPosition.x, playerUnit->middlePosition.y - camera->iPosition.y);

    control_Stop(&menu->cSelect, 500);

    return;
}

/*
    Function: vUnitMenu_InitSelect

    Description -
    Initialise the menu to be in the select state. The state where the player can
    select units and change their ai.

    1 argument:
    Unit_Menu *menu - The unit menu to initialise to the select state.
*/
void vUnitMenu_InitSelect(Unit_Menu *menu)
{
    Vent_Game *game = menu->currentGame;
    Camera_2D *camera = &game->camera;
    Vent_Unit *playerUnit = game->playerUnit;

    menu->state = UMENU_STATE_SELECT;

    /*Disable the overlay*/
    uiSpine_OpenTemplate(&menu->spine, "Select", SELECT_ON);

    /*Ensure that the mouse pointer is on*/
    uiPointer_Unclamp(&menu->pointer);
    uiPointer_SetDraw(&menu->pointer, 1);

    /*Force the origin of the state scroll to be the select state*/
    uiButtonScroll_SetOrigin(uiSpine_GetEntity(&menu->spine, "Scroll:State"), 1);

    /*Set the pointer to the player*/
    uiPointer_SetPos(menu->spine.pnt, 1, playerUnit->middlePosition.x - camera->iPosition.x, playerUnit->middlePosition.y - camera->iPosition.y);

    /*Reset the selected unit list*/
    list_Clear(&menu->selectedUnits);
    menu->numSelected = 0;

    control_Stop(&menu->cSelect, 500);

    return;
}

/*
    Function: vUnitMenu_Init

    Description -
    Initialise the menu to be ready for all states and makes it active. Sets the default state to be
    the base state.

    2 argument:
    Unit_Menu *menu - The unit menu to initialise.
    Base_State *bs - The main state that the game is played in. Changed to point to the unit menu states.
*/
void vUnitMenu_Init(Unit_Menu *menu, Base_State *bs)
{
    vUnitMenu_InitBase(menu);

    bs->render = vUnitMenuState_Render;
    bs->logic = vUnitMenuState_Update;

    timer_Stop(&menu->spine.sTimer, 500);

    return;
}

/*
    Function: vUnitMenu_Setup

    Description -
    Setup the default values for the unit menu.

    3 arguments:
    Unit_Menu *menu - The unit menu to setup.
    int team - The team the menu will represent.
   void *game - Points to the Vent_Game structure of the current game.
*/
void vUnitMenu_Setup(Unit_Menu *menu, int team, void *game)
{
    int x = 0;
    SDL_Rect boxClip = {0, 0, 48, 48};

    SDL_Surface *boxSurface = NULL;
    SDL_Surface *unitSurface = NULL;

    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    Sprite *curser = NULL;

    menu->currentGame = game;

    /*Setup the spine*/
    curser = sprite_Setup(sprite_Create(), 1, VL_HUD + 1, NULL, 6, frame_CreateBasic(150, ve_Surfaces.unitPointer[0], M_FREE),
                                                                   frame_CreateBasic(150, ve_Surfaces.unitPointer[1], M_FREE),
                                                                   frame_CreateBasic(150, ve_Surfaces.unitPointer[2], M_FREE),
                                                                   frame_CreateBasic(150, ve_Surfaces.unitPointer[3], M_FREE),
                                                                   frame_CreateBasic(150, ve_Surfaces.unitPointer[4], M_FREE),
                                                                   frame_CreateBasic(250, ve_Surfaces.unitPointer[5], M_FREE)
    );

    uiPointer_Setup(&menu->pointer, curser, VL_HUD + 1);

    uiSpine_Setup(&menu->spine, VL_HUD + 1, 0, 0, NULL, &menu->pointer, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    /*Setup the unit boxes*/
    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        /*Create the background box*/
        boxSurface = surf_SimpleBox(50, 50, &colourBlack, &colourWhite, 1);

        /*Find the unit surface to blit on the box*/
        unitSurface = ve_Surfaces.units[x][team][D_UP];

        /*Blit the unit surface in the middle of the box*/
        surf_Blit(boxSurface->w/2 - unitSurface->w/2, boxSurface->h/2 - unitSurface->h/2, unitSurface, boxSurface, &boxClip);

        /*Setup the sprite*/
        sprite_Setup(&menu->sUnitBox[x], 0, VL_HUD + 1, &menu->spine.sTimer, 1,
                frame_CreateBasic(0, boxSurface, A_FREE)
             );
    }

    /*Create the sprite for the background of unit purchase/select and place it into the spine*/
    uiSpine_AddSprite(&menu->spine, sprite_SetPos(sprite_Setup(sprite_Create(), 1, VL_HUD + 1, &menu->spine.sTimer, 1,
                                        frame_CreateBasic(0, surf_SimpleBox(175, 25, &colourBlack, &colourWhite, 1), A_FREE)
                                        ), 50, 20),
                                    A_FREE,
                                    "Sprite:BuySelect");

    sprite_SetAlpha(uiSpine_GetEntity(&menu->spine, "Sprite:BuySelect"), 180);

    /*Create the sprite for the unit info and place it into the spine*/
    uiSpine_AddSprite(&menu->spine, sprite_SetPos(sprite_Setup(sprite_Create(), 1, VL_HUD + 1, &menu->spine.sTimer, 1,
                                        frame_CreateBasic(0, surf_SimpleBox(120, 50, &colourBlack, &colourWhite, 1), A_FREE)
                                        ), 105, 50),
                                    A_FREE,
                                    "Sprite:UnitInfo");

    sprite_SetAlpha(uiSpine_GetEntity(&menu->spine, "Sprite:UnitInfo"), 180);


    /*Create the sprite for the unit ai info and place it into the spine*/
    uiSpine_AddSprite(&menu->spine, sprite_SetPos(sprite_Setup(sprite_Create(), 1, VL_HUD + 1, &menu->spine.sTimer, 1,
                                    frame_CreateBasic(0, surf_SimpleBox(175, 25, &colourBlack, &colourWhite, 1), A_FREE)
                                    ), 50, 105),
                                A_FREE,
                                "Sprite:UnitAiInfo");

    sprite_SetAlpha(uiSpine_GetEntity(&menu->spine, "Sprite:UnitAiInfo"), 180);

    /*Create the unit buying scroll*/
    scroll = uiButtonScroll_Create(50, 50,
                                       SCR_H, 0, 1, 1, 0,
                                       veMenu_ButtonEmpty(&menu->spine.sTimer),
                                       veMenu_ButtonEmpty(&menu->spine.sTimer),
                                       4,
                                       uiButton_Create(0, 0, 50, 50, NULL, &UM_SelectUnitType, dataStruct_Create(2, menu, UNIT_TANK), &menu->sUnitBox[UNIT_TANK], 0, 250, &menu->spine.sTimer),
                                       uiButton_Create(0, 0, 50, 50, NULL, &UM_SelectUnitType, dataStruct_Create(2, menu, UNIT_INFANTRY), &menu->sUnitBox[UNIT_INFANTRY], 0, 250, &menu->spine.sTimer),
                                       uiButton_Create(0, 0, 50, 50, NULL, &UM_SelectUnitType, dataStruct_Create(2, menu, UNIT_AIR), &menu->sUnitBox[UNIT_AIR], 0, 250, &menu->spine.sTimer),
                                       uiButton_Create(0, 0, 50, 50, NULL, &UM_SelectUnitType, dataStruct_Create(2, menu, UNIT_TOWER), &menu->sUnitBox[UNIT_TOWER], 0, 250, &menu->spine.sTimer)
                                       );

    uiButtonScroll_ActivateOnTraverse(scroll, 1);

    uiButtonScroll_SetHoverState(scroll, &BFH_ScrollTraverse);


    /*Add the scroll into the spine*/
    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Unit");

    /*Create the menu state scroll, "Buy | Select"*/
    scroll = uiButtonScroll_Create(50, 20,
                                       SCR_H, 0, 1, 1, 0,
                                       veMenu_ButtonEmpty(&menu->spine.sTimer),
                                       veMenu_ButtonEmpty(&menu->spine.sTimer),
                                       2,
                                       uiButton_Create(0, 0, 175, 25, NULL, &BFH_HoverState, dataStruct_Create(2, menu, UMENU_SUBSTATE_BUY), sprite_Setup(sprite_Create(), 1, VL_HUD + 2, &menu->spine.sTimer, 1, frame_CreateBasic(0, surf_Offset(10, 3, text_Arg(font_Get(2, 13), &tColourWhite, "BUY"), 1), A_FREE)), 0, 250, &menu->spine.sTimer),
                                       uiButton_Create(0, 0, 175, 25, NULL, &BFH_HoverState, dataStruct_Create(2, menu, UMENU_SUBSTATE_SELECT), sprite_Setup(sprite_Create(), 1, VL_HUD + 2, &menu->spine.sTimer, 1, frame_CreateBasic(0, surf_Offset(10, 3, text_Arg(font_Get(2, 13), &tColourWhite, "SELECT"), 1), A_FREE)), 0, 250, &menu->spine.sTimer)
                                       );

    uiButtonScroll_ActivateOnTraverse(scroll, 1);

    uiButtonScroll_SetHoverState(scroll, &BFH_ScrollTraverse);

    /*Add the scroll into the spine*/
    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:State");

    /*Create the unit ai scroll*/
    scroll = uiButtonScroll_Create(50, 105,
                                       SCR_H, 0, 1, 1, 0,
                                       veMenu_ButtonEmpty(&menu->spine.sTimer),
                                       veMenu_ButtonEmpty(&menu->spine.sTimer),
                                       3,
                                       uiButton_Create(0, 0, 175, 25, NULL, &UM_SelectAiType, dataStruct_Create(2, menu, MENU_AI_FOLLOW), NULL, 0, 250, &menu->spine.sTimer),
                                       uiButton_Create(0, 0, 175, 25, NULL, &UM_SelectAiType, dataStruct_Create(2, menu, MENU_AI_HOLD), NULL, 0, 250, &menu->spine.sTimer),
                                       uiButton_Create(0, 0, 175, 25, NULL, &UM_SelectAiType, dataStruct_Create(2, menu, MENU_AI_CITADEL), NULL, 0, 250, &menu->spine.sTimer)
                                       );

    uiButtonScroll_ActivateOnTraverse(scroll, 1);

    uiButtonScroll_SetHoverState(scroll, &BFH_ScrollTraverse);

    /*Add the scroll into the spine*/
    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Ai");

    /*Add in the spine templates for each state of the menu*/
    uiSpine_CreateTemplate(&menu->spine, 0, "BuyMenu", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER, 6,
                                                    uiSpine_CreateTemplateEntity("Scroll:Ai", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Scroll:State", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Scroll:Unit", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:BuySelect", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitInfo", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitAiInfo", UIS_ENTITY_UPDATE, 1)
    );

    uiSpine_CreateTemplate(&menu->spine, 1, "SelectMenu", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER, 5,
                                                    uiSpine_CreateTemplateEntity("Scroll:Ai", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Scroll:State", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:BuySelect", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitInfo", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitAiInfo", UIS_ENTITY_UPDATE, 1)
    );


    uiSpine_CreateTemplate(&menu->spine, 2, "Place", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER - UI_SPINE_UPDATECHOOSER, 6,
                                                    uiSpine_CreateTemplateEntity("Scroll:Ai", UIS_ENTITY_NO_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Scroll:State", UIS_ENTITY_NO_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Scroll:Unit", UIS_ENTITY_NO_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:BuySelect", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitInfo", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitAiInfo", UIS_ENTITY_UPDATE, 1)
    );

    uiSpine_CreateTemplate(&menu->spine, 3, "Select", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER - UI_SPINE_UPDATECHOOSER, 5,
                                                    uiSpine_CreateTemplateEntity("Scroll:Ai", UIS_ENTITY_NO_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Scroll:State", UIS_ENTITY_NO_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:BuySelect", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitInfo", UIS_ENTITY_UPDATE, 1),
                                                    uiSpine_CreateTemplateEntity("Sprite:UnitAiInfo", UIS_ENTITY_UPDATE, 1)
    );

    vUnitMenu_SetupControls(menu);

    menu->chosenUnitType = UNIT_TANK;

    menu->chosenAiType = MENU_AI_FOLLOW;

    menu->selectRadius = 5;
    menu->selectedUnits = NULL;
    menu->numSelected = 0;
    menu->pntPos.x = 0;
    menu->pntPos.y = 0;
    menu->state = UMENU_STATE_BASE;
    menu->subState = UMENU_SUBSTATE_BUY;

    timer_Stop(&menu->spine.sTimer, 500);

    return;
}

/*
    Function: vUnitMenu_GiveAi

    Description -
    Gives a unit a specific ai.
    Any ai that the unit has which is in group AI_GLOBAL_STATE + 1 is removed.

    3 arguments:
    Unit_Menu *menu - The unit menu that contains what ai to give the unit.
    Vent_Unit *u - The unit that is given ai.
    Vector2DInt *position - The location of where the ai originates.
*/
void vUnitMenu_GiveAi(Unit_Menu *menu, Vent_Unit *u, Vector2DInt *position)
{
    Vent_Game *game = menu->currentGame;

    /*Remove the ai state that is already in the unit*/
    aiState_SetDestroyedAll(u->ai.localList);

    switch(menu->chosenAiType)
    {
        case MENU_AI_FOLLOW:

        /*Unit defend target ai*/
        vUnit_GiveAi(u,
                     aiState_UnitDefendEntity_Setup(aiState_Create(),
                     AI_GLOBAL_STATE + 1,    /*group*/
                     1,                      /*priority*/
                     AI_COMPLETE_SINGLE,     /*complete type*/
                     u,                      /*Entity*/
                     game->playerUnit,       /*Entity to defend*/
                     ENTITY_UNIT,            /*If entity is unit or building*/
                     1,                      /*Should the unit always follow the entity? (1)*/
                     game->level,            /*Level the unit is in*/
                     game)                   /*Current game*/
        );

        break;

        case MENU_AI_HOLD:

        /*Unit attack inside of area ai*/
        vUnit_GiveAi(u,
            aiState_UnitAttackArea_Setup(aiState_Create(),
            AI_GLOBAL_STATE + 1,    /*group*/
            1,                      /*priority*/
            AI_COMPLETE_SINGLE,     /*complete type*/
            u,                      /*entity*/
            game,                   /*game*/
            position->x,            /*x-axis origin of area*/
            position->y,            /*y-axis origin of area*/
            100*100,                /*range of area (squared)*/
            -1                      /*max time to wait at area*/
            )
        );

        break;

        case MENU_AI_CITADEL:

        /*Unit buy closest citadel ai*/
        vUnit_GiveAi(u,
            aiState_UnitBuyCitadel_Setup(aiState_Create(),
            AI_GLOBAL_STATE + 1,        /*group*/
            1,                          /*priority*/
            AI_COMPLETE_SINGLE,         /*complete type*/
            u,                          /*entity*/
            1,                          /*max attempts at buying*/
            game->level,                /*game level*/
            game                        /*game*/
            )
        );

        /*Unit defend target ai, once the citadel ai has complete*/
        vUnit_GiveAi(u,
             aiState_UnitDefendEntity_Setup(aiState_Create(),
             AI_GLOBAL_STATE + 2,    /*group*/
             2,                      /*priority*/
             AI_COMPLETE_SINGLE,     /*complete type*/
             u,                      /*Entity*/
             game->playerUnit,       /*Entity to defend*/
             ENTITY_UNIT,            /*If entity is unit or building*/
             1,                      /*Should the unit always follow the entity? (1)*/
             game->level,            /*Level the unit is in*/
             game)                   /*Current game*/
        );

        break;
    }

    return;
}

/*
    Function: vUnitMenu_GiveAiList

    Description -
    Gives a list of units a specific ai.
    Any ai that the units have which is in group AI_GLOBAL_STATE + 1 is removed.

    2 arguments:
    Unit_Menu *menu - The unit menu that contains what ai to give the unit.
    struct list *unitList - The list of units that will be given the ai.
*/
void vUnitMenu_GiveAiList(Unit_Menu *menu, struct list *unitList)
{
    Vent_Game *game = menu->currentGame;

    /*Unit ai position*/
    Vector2DInt position = {menu->pntPos.x, menu->pntPos.y};

    while(unitList != NULL)
    {
        vUnitMenu_GiveAi(menu, unitList->data, &position);

        unitList = unitList->next;
    }

    return;
}

/*
    Function: vUnitMenu_ScrollCamera

    Description -
    Check to see if the camera needs to scroll. If so then scroll it.

    3 arguments:
    Unit_Menu *menu - The unit menu used to check for key presses and pointer location.
    Camera_2D *camera - The camera to scroll.
    float deltaTime - Time between each frame, to judge the magnitude of the movement.
*/
static int vUnitMenu_ScrollCamera(Unit_Menu *menu, Camera_2D *camera, float deltaTime)
{
    Ui_Pointer *pnt = menu->spine.pnt;

    float singleAxis = 300 * deltaTime;
    float doubleAxis = 300 * deltaTime * 0.7071f;

    /*If the menu pointer is outside of screen range, scroll the camera*/
    if(uiPointer_HasCollided(pnt) == 1)
    {
        if(pnt->collidedX == PNT_COLLIDED_RIGHT)
        {
            camera2D_ChangePosition(camera, singleAxis, 0.0f);
        }
        else if(pnt->collidedX == PNT_COLLIDED_LEFT)
        {
            camera2D_ChangePosition(camera, -singleAxis, 0.0f);
        }

        if(pnt->collidedY == PNT_COLLIDED_UP)
        {
            camera2D_ChangePosition(camera, 0.0f, -singleAxis);
        }
        else if(pnt->collidedY == PNT_COLLIDED_DOWN)
        {
            camera2D_ChangePosition(camera, 0.0f, singleAxis);
        }
    }

    /*Check the camera move controls*/
    if(control_IsActivated(&ve_Controls.cMoveCameraUpRight) == 1)
    {
        camera2D_ChangePosition(camera, doubleAxis, -doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraUpLeft) == 1)
    {
        camera2D_ChangePosition(camera, -doubleAxis, -doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraDownRight) == 1)
    {
        camera2D_ChangePosition(camera, doubleAxis, doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraDownLeft) == 1)
    {
        camera2D_ChangePosition(camera, -doubleAxis, doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraLeft) == 1)
    {
        camera2D_ChangePosition(camera, -singleAxis, 0.0f);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraRight) == 1)
    {
        camera2D_ChangePosition(camera, singleAxis, 0.0f);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraUp) == 1)
    {
        camera2D_ChangePosition(camera, 0.0f, -singleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraDown) == 1)
    {
        camera2D_ChangePosition(camera, 0.0f, singleAxis);
    }

    return 0;
}



/*
    Function: vUnitMenu_Purchase

    Description -
    Buys a unit in the game at the ui pointers position.

    1 argument:
    Unit_Menu *menu - The unit menu used to decide which and where a unit should be bought.
*/
static int vUnitMenu_Purchase(Unit_Menu *menu)
{
    Vent_Game *game = menu->currentGame;
    Vent_Side *playerSide = &game->side[game->playerUnit->team];

    Vent_Unit *u = NULL;

    /*Unit purchase position*/
    Vector2DInt position = {menu->pntPos.x, menu->pntPos.y};

    /*If the unit type selected is avaliable to buy*/
    if(playerSide->readyUnits[menu->chosenUnitType] > 0)
    {
        u = vUnit_Buy(position.x, position.y, menu->chosenUnitType, playerSide->team, 0, playerSide, &game->gTimer);

        if(u != NULL)
        {
            vGame_AddUnit(game, u);

            vUnitMenu_GiveAi(menu, u, &position);

            return 1;
        }
    }

    return 0;
}

/*
    Function: vUnitMenu_PurchaseShortcut

    Description -
    Buys a unit of given type in the game at the given location.
    The ai of the unit will be to follow the player.

    4 arguments:
    Unit_Menu *menu - The unit menu structure.
    int unitType - The type of unit to buy.
    int posX - The x-axis location of the unit to spawn at.
    int posY - The y-axis location of the unit to spawn at.
*/
int vUnitMenu_PurchaseShortcut(Unit_Menu *menu, int unitType, int posX, int posY)
{
    menu->pntPos.x = posX;
    menu->pntPos.y = posY;

    menu->chosenUnitType = unitType;
    menu->chosenAiType = MENU_AI_FOLLOW;

    return vUnitMenu_Purchase(menu);
}
/*
    Function: vUnitMenu_Select

    Description -
    Select a unit/s in the game at the ui pointers position.

    1 argument:
    Unit_Menu *menu - The unit menu containing the data needed to select units.
*/
static int vUnitMenu_Select(Unit_Menu *menu)
{
    Vent_Game *game = menu->currentGame;
    Vent_Side *playerSide = &game->side[game->playerUnit->team];

    Vent_Unit *u = NULL;
    struct list *unitList = NULL;

    /*Unit select origin*/
    Vector2DInt position = {menu->pntPos.x, menu->pntPos.y};

    /*Set the amount selected to 0*/
    list_Clear(&menu->selectedUnits);
    menu->numSelected = 0;

    /*For all the units in the players side*/
    unitList = playerSide->units;

    while(unitList != NULL)
    {
        u = unitList->data;

        /*If the unit collides with the curser and its not the player*/
        if(u->isPlayer == 0 && collisionStatic_CircRectInt(position.x, position.y, &menu->selectRadius, u->iPosition.x, u->iPosition.y, u->width, u->height) == 1)
        {
            /*Place it into the selected list and update the counter*/
            menu->numSelected ++;
            list_Push(&menu->selectedUnits, u, 0);
        }

        unitList = unitList->next;
    }

    return menu->numSelected;
}

/*
    Function: vUnitMenu_Update

    Description -
    Update the unit menu.

    3 arguments:
    Unit_Menu *menu - The unit menu to update.
    Base_State *bs - The base state of the main game.
    float deltaTime - The delta time of each call to this function.
*/
void vUnitMenu_Update(Unit_Menu *menu, Base_State *bs, float deltaTime)
{
    Vent_GameState *vgs = bs->info;
    Vent_Game *game = menu->currentGame;
    Ui_ButtonScroll *scroll = NULL;

    Vector2D pntPos = {0.0f, 0.0f};

    /*Update the main ui of the menu*/
    uiSpine_Update(&menu->spine, deltaTime);

    /*Update the pointer position*/
    uiPointer_Update(&menu->pointer, deltaTime);
    pntPos = uiPointer_GetPos(&menu->pointer);
    menu->pntPos.x = (int)pntPos.x + game->camera.iPosition.x;
    menu->pntPos.y = (int)pntPos.y + game->camera.iPosition.y;


    if(control_IsActivated(&menu->cChangeAi) == 1)
    {
        scroll = uiSpine_GetEntity(&menu->spine, "Scroll:Ai");
        uiButtonScroll_ForceSelect(scroll, 0);
        uiButtonScroll_IncreaseSelected(scroll);
        uiButtonScroll_ForceSelect(scroll, -1);
    }

    /*Logic depends on which state the menu is in*/
    switch(menu->state)
    {
        case UMENU_STATE_BASE:

        /*Go back to the game*/
        if(control_IsActivated(&menu->cBack) == 1)
        {
            bs->render = Game_Render;
            bs->logic = Game_Logic;

            timer_Resume(&vgs->game.gTimer);
        }

        if(control_IsActivated(&menu->cSelect) == 1)
        {
            switch(menu->subState)
            {
                case UMENU_SUBSTATE_BUY:

                UM_SetState(menu, UMENU_STATE_PLACE);

                break;

                case UMENU_SUBSTATE_SELECT:

                UM_SetState(menu, UMENU_STATE_SELECT);

                break;
            }
        }

        break;

        case UMENU_STATE_PLACE:

        /*Buy a unit*/
        if(control_IsActivated(&menu->cSelect) == 1)
        {
            vUnitMenu_Purchase(menu);
        }

        /*Go back to the base state of the menu*/
        if(control_IsActivated(&menu->cBack) == 1)
        {
            UM_SetState(menu, UMENU_STATE_BASE);
        }

        vUnitMenu_ScrollCamera(menu, &vgs->game.camera, deltaTime);

        break;

        case UMENU_STATE_SELECT:

        /*Select a unit*/
        if(control_IsActivated(&menu->cSelect) == 1)
        {
            /*If no units or selected then collision detect search for them*/
            if(menu->numSelected == 0)
            {
                vUnitMenu_Select(menu);
            }
            else /*Otherwise give the currently selected units ai*/
            {
                /*Give the ai to the selected units*/
                vUnitMenu_GiveAiList(menu, menu->selectedUnits);

                /*Confirm that the selected units have been given an ai*/
                text_Draw_Arg(115, 75, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 2, 1000, "Ai set");
            }
        }

        /*Go back to the base state of the menu, or if units are select, unselect them*/
        if(control_IsActivated(&menu->cBack) == 1)
        {
            if(menu->numSelected == 0)
            {
                UM_SetState(menu, UMENU_STATE_BASE);
            }
            else
            {
                menu->numSelected = 0;
                list_Clear(&menu->selectedUnits);
            }
        }

        vUnitMenu_ScrollCamera(menu, &vgs->game.camera, deltaTime);

        break;
    }

    return;
}

/*
    Function: vUnitMenu_Draw

    Description -
    Draw the unit menu.

    2 arguments:
    Unit_Menu *menu - The unit menu to draw.
    SDL_Surface *dest - The destination surface to draw the menu on.
*/
void vUnitMenu_Draw(Unit_Menu *menu, SDL_Surface *dest)
{
    Vent_Game *game = menu->currentGame;
    Vent_Side *playerSide = &game->side[game->playerUnit->team];

    uiSpine_Draw(&menu->spine, dest);

    /*Current state*/
    //text_Draw_Arg(60, 23, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 1, 0, "%s", stateUnitMenuNames[menu->state]);

    /*Draw pointer*/
    uiPointer_Draw(&menu->pointer, ker_Screen());

    /*Unit info*/
    switch(menu->subState)
    {
        case UMENU_SUBSTATE_BUY:
        text_Draw_Arg(115, 55, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 1, 0, "%s", playerSide->unitDesc[menu->chosenUnitType].name);
        text_Draw_Arg(190, 55, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 1, 0, "%d",  playerSide->readyUnits[menu->chosenUnitType]);
        text_Draw_Arg(115, 75, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 1, 0, "$%d", playerSide->unitDesc[menu->chosenUnitType].price);
        break;

        case UMENU_SUBSTATE_SELECT:
        text_Draw_Arg(115, 55, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 1, 0, "Selected: %d", menu->numSelected);
        break;
    }

    /*Unit ai*/
    text_Draw_Arg(60, 108, ker_Screen(), font_Get(2, 13), &tColourWhite, VL_HUD + 1, 0, "Ai: %s", aiUnitMenuNames[menu->chosenAiType]);

    return;
}

/*
    Function: vUnitMenu_Clean

    Description -
    Clean up the unit menu.

    1 arguments:
    Unit_Menu *menu - The unit menu to clean
*/
void vUnitMenu_Clean(Unit_Menu *menu)
{
    int x = 0;

    uiSpine_Clean(&menu->spine);

    uiPointer_Clean(&menu->pointer);

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        sprite_Clean(&menu->sUnitBox[x]);
    }

    control_Clean(&menu->cSelect);
    control_Clean(&menu->cChangeAi);
    control_Clean(&menu->cBack);

    list_Clear(&menu->selectedUnits);

    return;
}

/*
    Function: vUnitMenuState_Update

    Description -
    This is the new update state for the Base_State structure.
    It performs the logic of the main (paused)game and also the unit menu logic.

    1 argument:
    void *info - The Base_State that the update is a part of.
*/
void vUnitMenuState_Update(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    /*Perform the main game logic*/
    Game_Logic(info);

    /*Perform the unit menu logic*/
    vUnitMenu_Update(&vgs->game.HUD.unitMenu, bs, vgs->game.deltaTime);

    return;
}

/*
    Function: vUnitMenuState_Render

    Description -
    This is the new render state for the Base_State structure.
    It performs the render of the main (paused)game and also the unit menu.

    1 argument:
    void *info - The Base_State that the render is a part of.
*/
void vUnitMenuState_Render(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    /*Draw the main game*/
    Game_Render(info);

    /*Draw the unit menu*/
    vUnitMenu_Draw(&vgs->game.HUD.unitMenu, ker_Screen());

    return;
}
