#include "LevelEditor.h"

#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "Time/Fps.h"
#include "Font.h"

#include "Graphics/PixelManager.h"
#include "Graphics/Pixel.h"
#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Vector2D.h"
#include "Collision.h"

#include "../Level/LevelFile.h"
#include "../Menus/MenuAttributes.h"
#include "../ControlMap.h"
#include "../Ai/Units/Ai_UnitMove.h"
#include "SupplyMenu.h"
#include "MiscMenu.h"
#include "LevelMenu.h"
#include "TileMenu.h"
#include "NodeMenu.h"
#include "ShortcutMenu.h"
#include "../Images.h"

char *leTileTemplateNames[TILEG_TOTALGRAPHICTYPES] = {"None", "Decor", "Water", "Citadel Base", "Building Base",
                             "Wall", "Road", "Road Corner", "Grass", "Snow",
                             "Sand", "Custom"};

char *leStateNames[VLE_NUMSTATES] = {"Menu", "Place", "Replace", "Remove", "Copy"};

char *leObjectNames[VLE_OBJECT_NUMTYPES] = {"Tile", "Unit", "Node", "Player start", "Enemy start", "Positions", "Edge"};

char *leLayerNames[6] = {"Ground", "Vehicle", "Tree", "Building", "Air", "HUD"};

char *leTeamNames[3] = {"Player", "Enemy", "None"};

char *leAiNames[7] = {"None", "Guard area", "Attack player", "Attack building", "Patrol", "Defend player", "Side ai"};

static void BFA_Quit(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];

    /*Quit the game menu state*/
    baseState_CallQuit(gState, 1);

    return;
}

/*Enable an editing window*/
static void BFA_OpenEditMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int type = (int)button->info->dataArray[1];

    switch(type)
    {
        case 0:

        uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:LevelEdit"), &editor->spine);

        editor->levelEdit.previousHeader = editor->level.header;

        break;

        case 1:

        uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:TileEdit"), &editor->spine);

        editor->tileEdit.tileTemplateType = TILEG_CUSTOM;
        editor->tileEdit.selectedTile = editor->tileEdit.customTile;

        /*Initialise any entities in the spine before it is drawn*/
        uiSpine_Update(uiSpine_GetEntity(&editor->spine, "Spine:TileEdit"), 0);

        break;

        case 2:

        uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:UnitEdit"), &editor->spine);

        break;

        case 3:

        uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:NodeEdit"), &editor->spine);

        break;

        case 4:

        uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:MiscEdit"), &editor->spine);

        uiSpine_Update(uiSpine_GetEntity(&editor->spine, "Spine:MiscEdit"), 0);

        break;
    }

    return;
}

/*Clear all the template tiles that can be used*/
static void LE_ClearTileTemplates(Vent_Level_Editor *editor)
{
    while(editor->tileEdit.loadedTiles != NULL)
    {
        vTile_Clean(editor->tileEdit.loadedTiles->data);
        mem_Free(editor->tileEdit.loadedTiles->data);

        list_Pop(&editor->tileEdit.loadedTiles);
    }

    return;
}

void vLE_InitLevelSectors(Vent_Level_Editor *editor)
{
    vLevel_SetupSectors(&editor->level);

    camera2D_Setup(&editor->camera, 0, 0,
        ker_Screen_Width(), ker_Screen_Height(), editor->level.header.width, editor->level.header.height, 0, 0);

    camera2D_SetConstraints(&editor->camera, 0, 0, editor->level.header.width, editor->level.header.height);

    if(editor->level.sectors != NULL)
    {
        editor->currentSector = editor->level.sectors->data;
    }
    else
        editor->currentSector = NULL;

    /*Setup the boundry of the pointer so that collision checks can be made to scroll the camera*/
    uiPointer_SetBoundry(editor->spine.pnt, (float)editor->camera.width, (float)editor->camera.height);

    return;
}

/*Setup the main components needed to edit the level that has been loaded*/
void vLE_InitLevel(Vent_Level_Editor *editor)
{
    printf("Starting level setup of %s\n", editor->level.header.name);

    vLevel_SetupGround(&editor->level, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, editor->level.header.baseTileID, 0));

    vLE_InitLevelSectors(editor);

    LE_ClearTileTemplates(editor);

    vLE_SetupTiles(editor);

    editor->levelName = editor->level.header.name;

    vLE_NodeEdit_Init(&editor->nodeEdit, &editor->level.pathGraph);

    return;
}

/*Create the base tile templates of a level*/
void vLE_SetupTiles(Vent_Level_Editor *editor)
{
    Vent_Tile *tile = NULL;
    Sprite *sTile = NULL;

    struct list *loadedSurfaces = editor->level.surfacesLoaded;
    Vent_LevelSurface *customSurface = NULL;

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    int x = 0;

    /*Citadel template*/
    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_CITADELBASE, 0), M_FREE));
    tile = vTile_Create(0, 0, TILE_CITADELBASE, TILEG_CITADELBASE, 0, VL_GROUND, 0,  0, TILESTATE_BUILDABLE, 1, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_CITADELBASE);

    /*Building base template*/
    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_BUILDINGBASE, 0), M_FREE));
    tile = vTile_Create(0, 0, TILE_BUILDINGBASE, TILEG_BUILDINGBASE, 0, VL_GROUND, 0,  0, TILESTATE_BUILDABLE, 1, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_BUILDINGBASE);

    /*Road template*/
    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_ROAD_H, 0), M_FREE));
    tile = vTile_Create(0, 0, TILE_DECOR, TILEG_ROAD_H, 0, VL_GROUND, 0,  0, TILESTATE_IDLE, 0, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_ROAD_H);

    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_ROAD_CUL, 0), M_FREE));
    tile = vTile_Create(0, 0, TILE_DECOR, TILEG_ROAD_CUL, 0, VL_GROUND, 0,  0, TILESTATE_IDLE, 0, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_ROAD_CUL);

    /*Wall template*/
    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_WALL_H, 0), M_FREE));
    tile = vTile_Create(0, 0, TILE_DECOR, TILEG_WALL_H, 1, VL_GROUND, 0,  0, TILESTATE_IDLE, 0, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_WALL_H);

    /*Water template*/
    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_WATER, 0), M_FREE));
    tile = vTile_Create(0, 0, TILE_DECOR, TILEG_WATER, 0, VL_GROUND, 0,  0, TILESTATE_IDLE, 0, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_WATER);

    /*Ground template*/
    for(x = TILEG_GRASS; x <= TILEG_SAND; x++)
    {
        sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, x, 0), M_FREE));
        tile = vTile_Create(0, 0, TILE_DECOR, x, 0, VL_GROUND, 0,  0, TILESTATE_IDLE, 0, 0, sTile);

        list_Stack(&tEditor->loadedTiles, tile, x);
    }

    /*Custom template*/
    sTile = sprite_Setup(sprite_Create(), 1, VL_GROUND, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, TILEG_NULL, 0), M_FREE));
    tEditor->customTile = tile = vTile_Create(0, 0, TILE_DECOR, TILEG_NULL, 0, VL_GROUND, 0,  0, TILESTATE_IDLE, 0, 0, sTile);

    list_Stack(&tEditor->loadedTiles, tile, TILEG_CUSTOM);

    tEditor->selectedTile = tEditor->customTile;
    tEditor->tileTemplateType = TILEG_CUSTOM;

    return;
}

Ui_Button *vLE_ButtonOption(char *name, Data_Struct *nameVars, int layer, void (*onActivate)(Ui_Button *button), Data_Struct *optionVars)
{
    return veMenu_ButtonOption(name, nameVars, font_Get(2, 13), NULL, layer, onActivate, &MABFH_ButtonHover, optionVars);
}

Ui_Button *vLE_ButtonCheckBox(char *text, int layer, void (*onActivate)(Ui_Button *button), void (*onUpdate)(Ui_Button *button), Data_Struct *info)
{
    Ui_Button *checkBox = veMenu_ButtonCheckBox(text, font_Get(2, 13), 0, 0, layer, NULL, onActivate, onUpdate, info);

    checkBox->onHover = &MABFH_HoverLineThin;

    return checkBox;
}

Ui_Button *vLE_CloseButton(void (*onActivate)(Ui_Button *button), Data_Struct *optionVars)
{
    Ui_Button *closeButton = uiButton_Create(0, 0, 0, 0, onActivate, NULL, optionVars, NULL, 1, 500, NULL);

    uiButton_CopyControl(closeButton, &ve_Controls.cBack, BC_KEY);

    return closeButton;
}

void vLE_SaveLevel(Vent_Level_Editor *editor)
{
    vLevel_Save(&editor->level, kernel_GetPath("PTH_VentCustomLevels"), editor->level.header.name);

    veMenu_WindowOk(&editor->spine, "Spine:SaveConfirmed", "Save complete.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, 100);

    return;
}

void vLE_SetState(Base_State *gState, int state)
{
    Vent_Level_Editor *editor = gState->info;

    switch(state)
    {
        default:
        printf("Error: unknown state to switch to in level editor (%d)\n", state);

        break;

        case VLE_STATE_BASE:
        vLE_InitBase(gState);
        break;

        case VLE_STATE_PLACE:
        vLE_InitPlace(gState);
        break;

        case VLE_STATE_REPLACE:
        vLE_InitReplace(gState);
        break;

        case VLE_STATE_REMOVE:
        vLE_InitRemove(gState);
        break;

        case VLE_STATE_COPY:
        vLE_InitCopy(gState);
        break;
    }

    vLE_SetObjectType(editor, editor->objectType);

    /*Temp fix*/
    uiPointer_IsActivated(&ve_Menu.pointer, 250);

    return;
}

void vLE_SetObjectType(Vent_Level_Editor *editor, int objectType)
{
    Vent_Level_TileEditor *tEditor = &editor->tileEdit;
    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;
    Vent_Level_NodeEditor *nEditor = &editor->nodeEdit;

    editor->objectType = objectType;

    switch(objectType)
    {
        default:

        editor->miscEdit.objGridWidth = 1;
        editor->miscEdit.objGridHeight = 1;

        break;

        case VLE_OBJECT_TILE:

        if(editor->tileEdit.selectedTile != NULL && editor->tileEdit.selectedTile->sImage != NULL)
        {
            /*Ensure that the tile graphic contains the correct layer and rotation*/
            editor->tileEdit.selectedTile->sImage->layer = editor->tileEdit.selectedTile->base.layer;


            editor->miscEdit.objGridWidth = editor->tileEdit.selectedTile->base.width;
            editor->miscEdit.objGridHeight = editor->tileEdit.selectedTile->base.height;
        }

        break;

        case VLE_OBJECT_UNIT:

        editor->miscEdit.objGridWidth = ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->w;
        editor->miscEdit.objGridHeight = ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->h;

        break;

        case VLE_OBJECT_EDGE:

        editor->nodeEdit.selectedNodeA = editor->nodeEdit.selectedNodeB = NULL;

        break;

    }

    return;
}

/*The base state is active when traversing menus*/
void vLE_InitBase(Base_State *gState)
{
    Vent_Level_Editor *editor = gState->info;

    baseState_Edit(gState,
        gState->init,
        vLE_Controls,
        vLE_Logic,
        vLE_Render,
        gState->exit,
        gState->info,
        0);

    /*Change the curser image*/
    uiPointer_SetFrame(&ve_Menu.pointer, 0);
    /*Remove the grid restriction on the pointer*/
    uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, 0, 0);

    /*Make sure the state is correct*/
    editor->state = VLE_STATE_BASE;

    /*Open up the main menu*/
    uiSpine_Open(&editor->spine, NULL);

    return;
}

/*Placing objects in the level*/
void vLE_InitPlace(Base_State *gState)
{
    Vent_Level_Editor *editor = gState->info;

    baseState_Edit(gState,
        gState->init,
        vLE_ControlsPlace,
        vLE_LogicPlace,
        vLE_RenderPlace,
        gState->exit,
        gState->info,
        0);

    uiPointer_SetFrame(&ve_Menu.pointer, 1);

    if(editor->miscEdit.useGrid == 1 && editor->miscEdit.useObjectGrid == 0)
    {
        uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, editor->miscEdit.gridWidth, editor->miscEdit.gridHeight);
    }
    else if(editor->miscEdit.useGrid == 1 && editor->miscEdit.useObjectGrid == 1)
    {
        uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, editor->miscEdit.objGridWidth, editor->miscEdit.objGridHeight);
    }

    editor->state = VLE_STATE_PLACE;

    uiSpine_Close(&editor->spine);

    return;
}

/*Replacing objects in the level*/
void vLE_InitReplace(Base_State *gState)
{
    Vent_Level_Editor *editor = gState->info;

    baseState_Edit(gState,
        gState->init,
        vLE_ControlsReplace,
        vLE_Logic,
        vLE_Render,
        gState->exit,
        gState->info,
        0);

    uiPointer_SetFrame(&ve_Menu.pointer, 1);

    if(editor->miscEdit.useGrid == 1 && editor->miscEdit.useObjectGrid == 0)
    {
        uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, editor->miscEdit.gridWidth, editor->miscEdit.gridHeight);
    }
    else if(editor->miscEdit.useGrid == 1 && editor->miscEdit.useObjectGrid == 1)
    {
        uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, editor->miscEdit.objGridWidth, editor->miscEdit.objGridHeight);
    }

    editor->state = VLE_STATE_REPLACE;

    uiSpine_Close(&editor->spine);

    return;
}

/*Removing objects in the level*/
void vLE_InitRemove(Base_State *gState)
{
    Vent_Level_Editor *editor = gState->info;

    baseState_Edit(gState,
        gState->init,
        vLE_ControlsRemove,
        vLE_Logic,
        vLE_RenderRemove,
        gState->exit,
        gState->info,
        0);

    uiPointer_SetFrame(&ve_Menu.pointer, 1);

    editor->state = VLE_STATE_REMOVE;

    uiSpine_Close(&editor->spine);

    return;
}

/*Copying objects in the level*/
void vLE_InitCopy(Base_State *gState)
{
    Vent_Level_Editor *editor = gState->info;

    baseState_Edit(gState,
        gState->init,
        vLE_ControlsCopy,
        vLE_Logic,
        vLE_Render,
        gState->exit,
        gState->info,
        0);

    uiPointer_SetFrame(&ve_Menu.pointer, 1);

    editor->state = VLE_STATE_COPY;

    uiSpine_Close(&editor->spine);

    return;
}

static void vLE_SetupControls(Vent_Level_Editor *editor)
{
    control_Setup(&editor->cRotateState, "Rotate state", &editor->spine.sTimer, 250);

    control_CopyKey(&editor->cRotateState, &ve_Controls.cMenuBackward);

    veMenu_SetSpineControl(&editor->spine);
    uiSpine_MapEntity(&editor->spine);


    return;
}

void vLE_Init(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = (Vent_Level_Editor *)mem_Malloc(sizeof(Vent_Level_Editor), __LINE__, __FILE__);

    Ui_ButtonScroll *scroll = NULL;
    int mainButtonHeight = ker_Screen_Height() - 40;
    Sprite *curserSprite = NULL;

    gState->info = editor;

    editor->bState = gState;

    /*Setup timer*/
    editor->fpsTimer = timer_Setup(NULL, 0, 0, 1);
    editor->fps = 0.0f;
    editor->deltaTime = 0.0320f;

    /*Setup menu spine*/
    uiSpine_Setup(&editor->spine, VL_HUD + 1, 0, 0, NULL, &ve_Menu.pointer, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    /*Create main buttons*/

    uiSpine_AddButton(&editor->spine,
                      veMenu_ButtonText(0, 0, 100, 30, ve_Menu.mainFont[FS_LARGE], "Level", &editor->spine.sTimer, VL_HUD + 2, &BFA_OpenEditMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, 0)),
                      "Button:Level");

    uiSpine_AddButton(&editor->spine,
                      veMenu_ButtonText(0, 0, 100, 30, ve_Menu.mainFont[FS_LARGE], "Tiles", &editor->spine.sTimer, VL_HUD + 2, &BFA_OpenEditMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, 1)),
                      "Button:Tiles");

    uiSpine_AddButton(&editor->spine,
                      veMenu_ButtonText(0, 0, 100, 30, ve_Menu.mainFont[FS_LARGE], "Units", &editor->spine.sTimer, VL_HUD + 2, &BFA_OpenEditMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, 2)),
                      "Button:Units");

    uiSpine_AddButton(&editor->spine,
                      veMenu_ButtonText(0, 0, 100, 30, ve_Menu.mainFont[FS_LARGE], "Nodes", &editor->spine.sTimer, VL_HUD + 2, &BFA_OpenEditMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, 3)),
                      "Button:Nodes");

    uiSpine_AddButton(&editor->spine,
                      veMenu_ButtonText(0, 0, 100, 30, ve_Menu.mainFont[FS_LARGE], "Misc", &editor->spine.sTimer, VL_HUD + 2, &BFA_OpenEditMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, 4)),
                      "Button:Misc");

    uiSpine_AddButton(&editor->spine,
                      veMenu_ButtonText(0, 0, 100, 30, ve_Menu.mainFont[FS_LARGE], "Exit", &editor->spine.sTimer, VL_HUD + 2, &BFA_Quit, &MABFH_ButtonHover, dataStruct_Create(1, gState)),
                      "Button:Exit");

    uiSpine_AlignEntities(&editor->spine, 45, mainButtonHeight, 20, SCR_H, 6,
                          "Button:Level",
                          "Button:Tiles",
                          "Button:Units",
                          "Button:Nodes",
                          "Button:Misc",
                          "Button:Exit");

    /*Add in the main header text*/
    editor->levelName = "None";

    /*Create the sprite for the main buttons and place it into the spine*/
    uiSpine_AddSprite(&editor->spine, sprite_SetPos(sprite_Setup(sprite_Create(), 1, VL_HUD + 1, &editor->spine.sTimer, 1,
                                    frame_CreateBasic(0, surf_SimpleBox(ker_Screen_Width(), 50, &ve_Menu.colourBackgroundBasic, &colourBlack, 1), A_FREE)
                                    ), 0, ker_Screen_Height() - 50),
                                A_FREE,
                                "Sprite:MainBar");

    /*Initialise variables*/
    vPlayer_Setup(&editor->testPlayer, "---");
    editor->graphicID = NULL;
    graph_Setup(&editor->positionGraph);
    editor->maxPositions = 0;

    editor->gridObject = 0;

    editor->tileObject = 0;
    editor->tilePos.x = 0;
    editor->tilePos.y = 0;

    vLE_TileEdit_Init(&editor->tileEdit);
    vLE_LevelEdit_Init(&editor->levelEdit);
    vLE_UnitEdit_Init(&editor->unitEdit);
    vLE_NodeEdit_Init(&editor->nodeEdit, NULL);
    vLE_MiscEdit_Init(&editor->miscEdit);

    vLE_SetObjectType(editor, VLE_OBJECT_TILE);

    //vLevel_LoadOld(&editor->level, NULL, &editor->spine.sTimer, "Level_Ten.map");
    vLevel_Setup(&editor->level, NULL, 1000, 1000);
    vLE_InitLevel(editor);

    vLE_SetupControls(editor);

    /*Setup other sub menus*/
    vLE_SetupShortcutMenu(editor);

    vLE_SetupTileGraphicMenu(editor, &editor->spine);

    vLE_SetupTileEditMenu(editor);

    vLE_SetupLevelEditMenu(editor);

    vLE_SetupUnitEditMenu(editor);

    vLE_SetupNodeEditMenu(editor);

    vLE_SetupMiscEditMenu(editor);


    vLE_SetState(gState, VLE_STATE_BASE);

    /*Open the level menu*/
    uiButton_TempActive(uiSpine_GetEntity(&editor->spine, "Button:Level"), 1);

    return;
}

void vLE_Controls(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    float singleAxis = editor->miscEdit.cameraSpeed * editor->deltaTime;
    float doubleAxis = editor->miscEdit.cameraSpeed * editor->deltaTime * 0.707106781f;

    /*Update the pointer*/
    uiPointer_Update(&ve_Menu.pointer, editor->deltaTime);

    /*Store the correct pointer co-ordinates*/
    editor->pntPos = uiPointer_GetPos(editor->spine.pnt);
    editor->cPntPos.x = (int)editor->pntPos.x + editor->camera.iPosition.x;
    editor->cPntPos.y = (int)editor->pntPos.y + editor->camera.iPosition.y;

    /*If not in any submenus*/
    if(editor->spine.state != SPINE_STATE_SPINE)
    {
        /*Rotate the menu state*/
        if(control_IsActivated(&editor->cRotateState) == 1)
        {
            editor->state = (editor->state + 1) % VLE_NUMSTATES;
            vLE_SetState(gState, editor->state);
        }

        /*Go to the main base menu state*/
        if(control_IsActivated(&ve_Controls.cBack) == 1)
        {
            vLE_SetState(gState, VLE_STATE_BASE);
        }
    }

    /*Camera moveing controls*/

    if(editor->state != VLE_STATE_BASE && uiPointer_HasCollided(editor->spine.pnt) == 1)
    {
        if(editor->spine.pnt->collidedX == PNT_COLLIDED_RIGHT)
        {
            camera2D_ChangePosition(&editor->camera, singleAxis, 0.0f);
        }
        else if(editor->spine.pnt->collidedX == PNT_COLLIDED_LEFT)
        {
            camera2D_ChangePosition(&editor->camera, -singleAxis, 0.0f);
        }

        if(editor->spine.pnt->collidedY == PNT_COLLIDED_UP)
        {
            camera2D_ChangePosition(&editor->camera, 0.0f, -singleAxis);
        }
        else if(editor->spine.pnt->collidedY == PNT_COLLIDED_DOWN)
        {
            camera2D_ChangePosition(&editor->camera, 0.0f, singleAxis);
        }
    }

    if(control_IsActivated(&ve_Controls.cMoveCameraUpRight) == 1)
    {
        camera2D_ChangePosition(&editor->camera,  doubleAxis, -doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraUpLeft) == 1)
    {
        camera2D_ChangePosition(&editor->camera, -doubleAxis, -doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraDownRight) == 1)
    {
        camera2D_ChangePosition(&editor->camera, doubleAxis, doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraDownLeft) == 1)
    {
        camera2D_ChangePosition(&editor->camera, -doubleAxis, doubleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraLeft) == 1)
    {
        camera2D_ChangePosition(&editor->camera, -singleAxis, 0.0f);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraRight) == 1)
    {
        camera2D_ChangePosition(&editor->camera, singleAxis, 0.0f);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraUp) == 1)
    {
        camera2D_ChangePosition(&editor->camera, 0.0f, -singleAxis);
    }
    else if(control_IsActivated(&ve_Controls.cMoveCameraDown) == 1)
    {
        camera2D_ChangePosition(&editor->camera, 0.0f, singleAxis);
    }

    /*Set the start point for tileing objects*/
    if(control_IsActivated(&ve_Controls.cTileObjects) == 1)
    {
        if(editor->tileObject == 0)
        {
            text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Tiling: On");

            editor->tilePos = editor->cPntPos;
            editor->tileObject = 1;
        }
    }
    else if(editor->tileObject == 1)
    {
        text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Tiling: Off");

        editor->tileObject = 0;
    }

    /*Start a grid with the same dimensions as the current object and origin at the pointer*/
    if(control_IsActivated(&ve_Controls.cGridObjects) == 1)
    {
        if(editor->gridObject == 0)
        {
            text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Object grid: On");

            editor->gridObject = 1;
            uiPointer_SetGrid(&ve_Menu.pointer, (int)editor->pntPos.x, (int)editor->pntPos.y, editor->miscEdit.objGridWidth, editor->miscEdit.objGridHeight);
        }
    }
    else if(editor->gridObject == 1)
    {
        text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Object grid: Off");

        editor->gridObject = 0;
        /*Reset the grid back to its original state*/
        if(editor->miscEdit.useGrid == 1 && editor->miscEdit.useObjectGrid == 0)
        {
            uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, editor->miscEdit.gridWidth, editor->miscEdit.gridHeight);
        }
        else if(editor->miscEdit.useGrid == 1 && editor->miscEdit.useObjectGrid == 1)
        {
            uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, editor->miscEdit.objGridWidth, editor->miscEdit.objGridHeight);
        }
        else
        {
            uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, 0, 0);
        }
    }

    /*Set the pointer to only move 1 pixel at a time*/
    if(control_IsActivated(&ve_Controls.cNudgePointer) == 1)
    {
        if(ve_Menu.pointer.nudge <= 0)
        {
            uiPointer_SetNudge(&ve_Menu.pointer, 1);
            text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Pointer nudge: On");
        }
        else
        {
            uiPointer_SetNudge(&ve_Menu.pointer, 0);
            text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Pointer nudge: Off");
        }

    }

    if(control_IsActivated(&ve_Controls.cRotateObject) == 1)
    {
        text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Rotated object");

        vTile_SetRotation(&editor->tileEdit.templateTile,  editor->level.surfacesLoaded, editor->tileEdit.templateTile.base.rotationDeg - 90);
    }

    if(control_IsActivated(&ve_Controls.cEditorSave) == 1)
    {
        vLE_SaveLevel(editor);
    }

    /*Enable the key shortcut menu overlay*/
    if(control_IsActivated(&ve_Controls.cEditorHelp) == 1)
    {
        uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:Shortcuts"), NULL);
    }
    else
    {
        uiSpine_Close(uiSpine_GetEntity(&editor->spine, "Spine:Shortcuts"));
    }

    return;
}

/*
    Function: vLE_ObjectTile

    Description -
    Sets up variables to help with placing objects directly adjacent to each other.
    If the direction between the tiling origin and the pointer position is more along
    the x-axis than y-axis then the tiling will be along the x-axis. Otherwise it will
    be along the y-axis.

    7 arguments:
    int *objX - x-axis point of where to finish tiling.
    int *objY - y-axis point of where to finish tiling.
    int objWidth - The width of the object to tile.
    int objHeight - The height of the object to tile.
    Vector2DInt *tileOrigin - The position vector of where to start tiling.
    int *r_TileLength - A pointer to a variable defining how much to update the x or y-axis position of the object to tile.
    int **r_Axis - A double pointer to another pointer which will change the x or y-axis position of the object to tile.
*/
static int vLE_ObjectTile(int *objX, int *objY, int objWidth, int objHeight, Vector2DInt *tileOrigin, int *r_TileLength, int **r_Axis)
{
    int xDis = 0;
    int yDis = 0;
    int x = 0;

    xDis = (*objX - tileOrigin->x);
    yDis = (*objY - tileOrigin->y);

    if(abs(yDis) > abs(xDis))
    {
        x = (yDis/objHeight);
        *r_TileLength = objHeight;
        *r_Axis = objY;
    }
    else
    {
        x = (xDis/objWidth);
        *r_TileLength = objWidth;
        *r_Axis = objX;
    }

    /*Fix the sign*/
    if(x < 0)
    {
        x = -(x - 1); /*add an extra tile for when x = 0*/
        *r_TileLength = -*r_TileLength;

    }
    else
    {
        /*Add an extra tile so that even with x=0 there will still be an object placed*/
        x = x + 1;
    }

    *objX = tileOrigin->x - (objWidth/2);
    *objY = tileOrigin->y - (objHeight/2);

    /*Set the tile origin point to be at the end of where the objects will be tiled.*/
    if(*r_Axis == objX)
    {
        tileOrigin->x = *objX + (*r_TileLength * x) + (objWidth/2);
        tileOrigin->y = *objY + (objHeight/2);
    }
    else
    {
        tileOrigin->x = *objX + (objWidth/2);
        tileOrigin->y = *objY + (*r_TileLength * x) + (objHeight/2);
    }

    return x;
}

void vLE_ControlsPlace(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;
    Vent_Level_TileEditor *tEditor = &editor->tileEdit;
    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;
    Vent_Level_NodeEditor *nEditor = &editor->nodeEdit;

    Vent_Tile *tile = NULL;
    B_Node *node = NULL;

    int pX = editor->cPntPos.x;
    int pY = editor->cPntPos.y;
    int distanceSqr = 0;
    int x = 0;
    int xDis = 0;
    int yDis = 0;
    int *axisChange = NULL;
    int tileLength = 0;

    vLE_Controls(info);

    if(uiPointer_IsActivated(&ve_Menu.pointer, 250) == 1 || control_IsActivated(&ve_Controls.cConfirm) == 1)
    {
        switch(editor->objectType)
        {
            default:
            case VLE_OBJECT_TILE:

            /*Check if the object should be tiled*/
            if(editor->tileObject == 1)
            {

                x = vLE_ObjectTile(&pX, &pY,
                                   tEditor->selectedTile->base.width, tEditor->selectedTile->base.height,
                                   &editor->tilePos, &tileLength, &axisChange);

            }
            else
            {
                x = 1;
                pX = tEditor->selectedTile->base.position.x;
                pY = tEditor->selectedTile->base.position.y;
            }

            for(x = x; x > 0; x --)
            {
                tile = vLevel_AddTile(&editor->level, vTile_Create(
                                                pX, pY,
                                                tEditor->selectedTile->base.type, tEditor->selectedTile->base.graphicType,
                                                tEditor->selectedTile->base.collision,
                                                tEditor->selectedTile->base.layer,
                                                tEditor->selectedTile->base.team,
                                                tEditor->selectedTile->base.healthStarting,
                                                tEditor->selectedTile->base.stateFlags,
                                                tEditor->selectedTile->base.group,
                                                tEditor->selectedTile->base.rotationDeg,
                                                NULL)
                );

                vSector_MapTile(editor->level.sectors, tile);

                /*Update the x or y axis of the tile position*/
                if(axisChange != NULL)
                {
                    *axisChange = *axisChange + tileLength;
                }
            }

            text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Tile total: %d", editor->level.header.numTiles);

            break;

            case VLE_OBJECT_UNIT:

            /*Check if the unit should be tiled*/
            if(editor->tileObject == 1)
            {
                x = vLE_ObjectTile(&pX, &pY,
                                   ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->w, ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->h,
                                   &editor->tilePos, &tileLength, &axisChange);

            }
            else
            {
                x = 1;
                pX = uEditor->templateSupply.oX;
                pY = uEditor->templateSupply.oY;
            }

            for(x = x; x > 0; x --)
            {
                uEditor->templateSupply.oX = pX;
                uEditor->templateSupply.oY = pY;

                vLevel_AddSupply(&editor->level, vSupply_Clone(&uEditor->templateSupply));

                /*Update the x or y axis of the unit position*/
                if(axisChange != NULL)
                {
                    *axisChange = *axisChange + tileLength;
                }
            }

            break;

            case VLE_OBJECT_PLAYERSTART:

            editor->level.header.startLoc[TEAM_PLAYER].x = pX - ve_Surfaces.sideStart[TEAM_PLAYER]->w/2;
            editor->level.header.startLoc[TEAM_PLAYER].y = pY - ve_Surfaces.sideStart[TEAM_PLAYER]->h/2;

            break;

            case VLE_OBJECT_ENEMYSTART:

            editor->level.header.startLoc[TEAM_1].x = pX - ve_Surfaces.sideStart[TEAM_1]->w/2;
            editor->level.header.startLoc[TEAM_1].y = pY - ve_Surfaces.sideStart[TEAM_1]->h/2;

            break;

            case VLE_OBJECT_POSITION:

            graph_Add_Node(&editor->positionGraph, node_Create(graph_AssignIndex(&editor->positionGraph), pX, pY));

            if(editor->positionGraph.numNodes > 1)
            {
                graph_Add_Edge(&editor->positionGraph, edge_Create_Index(editor->positionGraph.nodes, graph_CurrentIndex(&editor->positionGraph) - 1, graph_CurrentIndex(&editor->positionGraph), 1));
            }

            /*If the max number of positions to record has been set, revert back the main menu*/
            if(editor->maxPositions == 0)
            {
                vLE_SetState(gState, VLE_STATE_BASE);
            }
            else
            {
                editor->maxPositions --;
            }

            break;

            case VLE_OBJECT_NODE:

            graph_Add_Node(&editor->level.pathGraph, node_Create(graph_AssignIndex(&editor->level.pathGraph), pX, pY));

            if(nEditor->linkPreviousNode == 1 && editor->positionGraph.numNodes > 1)
            {
                graph_Add_Edge(&editor->level.pathGraph, edge_Create_Index(editor->level.pathGraph.nodes, graph_PreviousIndex(&editor->level.pathGraph, graph_CurrentIndex(&editor->level.pathGraph)), graph_CurrentIndex(&editor->level.pathGraph), 1));
            }

            break;

            case VLE_OBJECT_EDGE:

            node = node_GetClosest(editor->level.pathGraph.nodes, pX, pY, &distanceSqr);

            if(node != NULL && distanceSqr < (5 * 5))
            {
                vLE_NodeEdit_AddRemove_Edge(nEditor, node, 1);
            }

            break;

        }
    }

    return;
}

void vLE_ControlsRemove(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    Vent_Tile *tile = NULL;
    Vent_Supply *supply = NULL;
    B_Node *node = NULL;
    B_Edge *edge = NULL;

    /*Get the absolute position of the pointer*/
    int pX = editor->cPntPos.x;
    int pY = editor->cPntPos.y;
    int distanceSqr = 0;

    vLE_Controls(info);

    if(editor->currentSector != NULL && (uiPointer_IsActivated(&ve_Menu.pointer, 250) == 1 || control_IsActivated(&ve_Controls.cConfirm) == 1))
    {
        switch(editor->objectType)
        {
            default:
            case VLE_OBJECT_TILE:
            tile = vSector_GetTile(editor->currentSector, 1, pX, pY, 5, 5);

            if(tile != NULL)
            {
                vLevel_RemoveTile(&editor->level, tile);

                vTile_Clean(tile);
                mem_Free(tile);

                text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 500, "Tiles: %d", editor->level.header.numTiles);
            }

            break;

            case VLE_OBJECT_UNIT:

            supply = vSupply_GetCollision(editor->level.unitSupply, pX, pY, 5, 5);

            if(supply != NULL)
            {
                vLevel_RemoveSupply(&editor->level, supply);

                vSupply_Clean(supply);
                mem_Free(supply);
            }

            break;

            case VLE_OBJECT_POSITION:

            node = node_GetClosest(editor->positionGraph.nodes, pX, pY, &distanceSqr);

            if(node != NULL && distanceSqr < (5 * 5))
            {
                /*If the node has an edge to more than one other node then try and link around this node*/
                if(node->degree > 1)
                {
                    graph_Add_EdgeAroundNode(&editor->positionGraph, node_GetEdge(node, 0), node_GetEdge(node, 1), 1);
                }

                graph_Remove_Node(&editor->positionGraph, node);
            }

            break;

            case VLE_OBJECT_NODE:

            node = node_GetClosest(editor->level.pathGraph.nodes, pX, pY, &distanceSqr);

            if(node != NULL && distanceSqr < (5 * 5))
            {
                graph_Remove_Node(&editor->level.pathGraph, node);
            }

            break;

            case VLE_OBJECT_EDGE:

            edge = Edge_GetClosest(editor->level.pathGraph.edges, (float)pX, (float)pY, &distanceSqr);

            if(edge != NULL && distanceSqr < (10 * 10))
            {
                graph_Remove_EdgesByNodes(&editor->level.pathGraph, edge->startNode, edge->endNode);
            }
            else
            {
                node = node_GetClosest(editor->level.pathGraph.nodes, pX, pY, &distanceSqr);

                if(node != NULL && distanceSqr < (5 * 5))
                {
                    vLE_NodeEdit_AddRemove_Edge(&editor->nodeEdit, node, 0);
                }
            }

            break;
        }
    }

    return;
}

void vLE_ControlsReplace(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    Vent_Tile *tile = NULL;

    /*Get the absolute position of the pointer*/
    int pX = editor->cPntPos.x;
    int pY = editor->cPntPos.y;
    int distanceSqr = 0;

    vLE_Controls(info);

    if(editor->currentSector != NULL && (uiPointer_IsActivated(&ve_Menu.pointer, 250) == 1 || control_IsActivated(&ve_Controls.cConfirm) == 1))
    {
        switch(editor->objectType)
        {
            case VLE_OBJECT_TILE:
            tile = vSector_GetTile(editor->currentSector, 1, pX, pY, 5, 5);

            if(tile != NULL)
            {
                vTile_Copy(0, tile, &editor->tileEdit.templateTile);

                text_Draw_Arg(25, 85, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 250, "Tile Edited", editor->level.header.numTiles);
                spriteMan_Push(gKer_DrawManager(), A_FREE, 800,  camera2D_RelativeX(&editor->camera, tile->base.position.x) + (tile->base.width/2) - (sprite_Width(&ve_Sprites.repair)/2), camera2D_RelativeY(&editor->camera, tile->base.position.y) + (tile->base.height/2) - (sprite_Height(&ve_Sprites.repair)/2), sprite_Copy(&ve_Sprites.repair, 0), ker_Screen());
            }

            break;
        }
    }

    return;
}

void vLE_ControlsCopy(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;
    Vent_Level_TileEditor *tEditor = &editor->tileEdit;
    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;

    Vent_Tile *tile = NULL;
    Vent_Supply *supply = NULL;
    Data_Struct *patrolAi = NULL;

    int x = 0;

    /*Get the absolute position of the pointer*/
    int pX = editor->cPntPos.x;
    int pY = editor->cPntPos.y;

    vLE_Controls(info);

    if(editor->currentSector != NULL && (uiPointer_IsActivated(&ve_Menu.pointer, 250) == 1 || control_IsActivated(&ve_Controls.cConfirm) == 1))
    {
        switch(editor->objectType)
        {
            default:
            case VLE_OBJECT_TILE:

            tile = vSector_GetTile(editor->currentSector, 1, pX, pY, 5, 5);

            if(tile != NULL)
            {
                /*Copy this tile*/
                vTile_Copy(1, &tEditor->templateTile, tile);

                vTile_Copy(1, tEditor->customTile, &tEditor->templateTile);

                tEditor->selectedTile = &tEditor->templateTile;

                /*Set the state to placing that new copied tile*/
                vLE_SetState(gState, VLE_STATE_PLACE);
            }

            break;

            case VLE_OBJECT_UNIT:

            supply = vSupply_GetCollision(editor->level.unitSupply, pX, pY, 5, 5);

            if(supply != NULL)
            {
                vSupply_Clean(&uEditor->templateSupply);
                vSupply_Copy(&uEditor->templateSupply, supply);

                if(uEditor->aiData[uEditor->templateSupply.aiState] != NULL)
                {
                    dataStruct_Delete(&uEditor->aiData[uEditor->templateSupply.aiState]);
                    uEditor->aiData[uEditor->templateSupply.aiState] = vSupply_CopyAi(&uEditor->templateSupply);

                    if(uEditor->templateSupply.aiState == SUPPLY_AI_PATROL)
                    {
                        patrolAi = uEditor->aiData[SUPPLY_AI_PATROL];

                        /*Remove the template patrol list*/
                        if(uEditor->patrolList != NULL)
                        {
                            list_ClearAll(&uEditor->patrolList);
                            uEditor->patrolList = NULL;
                        }

                        /*Insert the patrols from the template ai data into the template patrol list*/
                        for(x = 0; x < (patrolAi->amount - 1)/3; x++)
                        {
                            list_Stack(&uEditor->patrolList, aiPatrol_Create(*(int *)patrolAi->dataArray[(x*3) + 1], *(int *)patrolAi->dataArray[(x*3) + 2], *(int *)patrolAi->dataArray[(x*3) + 3]), 0);
                        }
                    }
                }
            }

            vLE_SetState(gState, VLE_STATE_PLACE);

            break;
        }
    }

    return;
}

void vLE_Logic(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    /*Calculate fps*/
    editor->fps = fps_Calc_Now(&editor->fpsTimer);

    timer_Start(&editor->fpsTimer);
    editor->deltaTime = 0.0320f;

    /*Make sure the level sector is correct*/
    if(editor->currentSector != NULL)
        editor->currentSector = vSector_Find(editor->currentSector, editor->cPntPos.x, editor->cPntPos.y);

    /*Update the ui*/
    uiSpine_Update(&editor->spine, editor->deltaTime);

    return;
}

void vLE_LogicPlace(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;
    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    int placePositionX = (int)editor->cPntPos.x;
    int placePositionY = (int)editor->cPntPos.y;

    vLE_Logic(info);

    switch(editor->objectType)
    {
        case VLE_OBJECT_TILE:

        if(tEditor->selectedTile != NULL)
        {
            tEditor->selectedTile->base.position.x = placePositionX - tEditor->selectedTile->base.width/2;
            tEditor->selectedTile->base.position.y = placePositionY - tEditor->selectedTile->base.height/2;
        }

        break;

        default:
        break;
    }

    return;
}

void vLE_Render(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    struct list *supplyList = editor->level.unitSupply;
    Vector2DInt *posVector = NULL;

    /*Render the main level*/
    vLevel_Draw(&editor->level, editor->currentSector, &editor->camera, ker_Screen());

    /*menu screen*/
    uiSpine_Draw(&editor->spine, ker_Screen());

    /*information text*/
    text_Draw_Arg(150, 0, ker_Screen(), font_Get(2, 11), &tColourBlack, VL_HUD, 0, "FPS %.2f", editor->fps);
    text_Draw_Arg(10, 25, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 0, "%s", editor->levelName);
    text_Draw_Arg(10, 45, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 0, "%s", leStateNames[editor->state]);
    text_Draw_Arg(10, 65, ker_Screen(), font_Get(2, 13), &tColourBlack, VL_HUD, 0, "%s", leObjectNames[editor->objectType]);
    text_Draw_Arg(0, 0, ker_Screen(), font_Get(2, 11), &tColourBlack, VL_HUD + 1000, 0, "%d %d", (int)editor->pntPos.x + editor->camera.iPosition.x, (int)editor->pntPos.y + editor->camera.iPosition.y);

    if(editor->currentSector != NULL)
        text_Draw_Arg(80, 0, ker_Screen(), font_Get(2, 11), &tColourBlack, VL_HUD, 0, "%d %d", editor->currentSector->position.x, editor->currentSector->position.y);

    /*Render the starting positions of the sides*/
    surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, camera2D_RelativeX(&editor->camera, editor->level.header.startLoc[TEAM_PLAYER].x), camera2D_RelativeY(&editor->camera, editor->level.header.startLoc[TEAM_PLAYER].y), ve_Surfaces.sideStart[TEAM_PLAYER], ker_Screen(), NULL);
    surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, camera2D_RelativeX(&editor->camera, editor->level.header.startLoc[TEAM_1].x), camera2D_RelativeY(&editor->camera, editor->level.header.startLoc[TEAM_1].y), ve_Surfaces.sideStart[TEAM_1], ker_Screen(), NULL);

    /*Draw the unit supply*/
    while(supplyList != NULL)
    {
        vSupply_Draw(supplyList->data, &editor->camera, ker_Screen());

        supplyList = supplyList->next;
    }

    if(editor->nodeEdit.showGlobalNodes == 1)
    {
        /*Draw the global level nodes*/
        graph_Draw(&editor->level.pathGraph, VL_HUD, font_Get(1, 11), ve_Surfaces.node[0], &editor->camera, ker_Screen());
    }

    /*Render the pointer*/
    uiPointer_Draw(&ve_Menu.pointer, ker_Screen());

    return;
}

void vLE_RenderPlace(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;
    Vent_Level_TileEditor *tEditor = &editor->tileEdit;
    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;

    vLE_Render(info);

    switch(editor->objectType)
    {
        default:
        case VLE_OBJECT_TILE:

        if(tEditor->selectedTile != NULL)
        {
            if(editor->tileObject == 1)
            {
                sprite_DrawAtPos(camera2D_RelativeX(&editor->camera, editor->tilePos.x - tEditor->selectedTile->base.width/2), camera2D_RelativeY(&editor->camera, editor->tilePos.y - tEditor->selectedTile->base.height/2), tEditor->selectedTile->sImage, ker_Screen());
            }

            vTile_Draw(tEditor->selectedTile, &editor->camera, ker_Screen());
        }

        break;

        case VLE_OBJECT_UNIT:

        if(editor->tileObject == 1)
        {
            uEditor->templateSupply.oX = editor->tilePos.x - ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->w/2;
            uEditor->templateSupply.oY = editor->tilePos.y - ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->h/2;

            vSupply_Draw(&uEditor->templateSupply, &editor->camera, ker_Screen());
        }

        uEditor->templateSupply.oX = editor->cPntPos.x - ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->w/2;
        uEditor->templateSupply.oY = editor->cPntPos.y - ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP]->h/2;

        vSupply_Draw(&uEditor->templateSupply, &editor->camera, ker_Screen());

        break;

        case VLE_OBJECT_PLAYERSTART:

        surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, (int)editor->pntPos.x - ve_Surfaces.sideStart[TEAM_PLAYER]->w/2, (int)editor->pntPos.y - ve_Surfaces.sideStart[TEAM_PLAYER]->h/2, ve_Surfaces.sideStart[TEAM_PLAYER], ker_Screen(), NULL);

        break;

        case VLE_OBJECT_ENEMYSTART:

        surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, (int)editor->pntPos.x - ve_Surfaces.sideStart[TEAM_1]->w/2, (int)editor->pntPos.y - ve_Surfaces.sideStart[TEAM_1]->h/2, ve_Surfaces.sideStart[TEAM_1], ker_Screen(), NULL);

        break;

        case VLE_OBJECT_POSITION:

        /*Draw mouse node*/
        surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, (int)editor->pntPos.x - ve_Surfaces.node[0]->w/2, (int)editor->pntPos.y - ve_Surfaces.node[0]->h/2, ve_Surfaces.node[0], ker_Screen(), NULL);

        /*Draw the position nodes*/
        graph_Draw(&editor->positionGraph, VL_HUD, font_Get(1, 11), ve_Surfaces.node[0], &editor->camera, ker_Screen());

        break;

        case VLE_OBJECT_NODE:
        case VLE_OBJECT_EDGE:

        if(editor->nodeEdit.showGlobalNodes == 0)
        {
            /*Draw the level nodes*/
            graph_Draw(&editor->level.pathGraph, VL_HUD, font_Get(1, 11), ve_Surfaces.node[0], &editor->camera, ker_Screen());
        }

        if(editor->nodeEdit.selectedNodeA != NULL)
        {
            node_Draw(editor->nodeEdit.selectedNodeA, VL_HUD, ve_Surfaces.node[1], &editor->camera, ker_Screen());
        }

        if(editor->nodeEdit.selectedNodeB != NULL)
        {
            node_Draw(editor->nodeEdit.selectedNodeB, VL_HUD, ve_Surfaces.node[1], &editor->camera, ker_Screen());
        }

        break;
    }

    return;
}

void vLE_RenderRemove(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    vLE_Render(info);

    switch(editor->objectType)
    {
        default:

        break;

        case VLE_OBJECT_POSITION:
        /*Draw the position nodes*/
        graph_Draw(&editor->positionGraph, VL_HUD, font_Get(1, 11), ve_Surfaces.node[0], &editor->camera, ker_Screen());
        break;

        case VLE_OBJECT_NODE:
        case VLE_OBJECT_EDGE:

        if(editor->nodeEdit.showGlobalNodes == 0)
        {
            /*Draw the level nodes*/
            graph_Draw(&editor->level.pathGraph, VL_HUD, font_Get(1, 11), ve_Surfaces.node[0], &editor->camera, ker_Screen());
        }


        if(editor->nodeEdit.selectedNodeA != NULL)
        {
            node_Draw(editor->nodeEdit.selectedNodeA, VL_HUD, ve_Surfaces.node[1], &editor->camera, ker_Screen());
        }

        if(editor->nodeEdit.selectedNodeB != NULL)
        {
            node_Draw(editor->nodeEdit.selectedNodeB, VL_HUD, ve_Surfaces.node[1], &editor->camera, ker_Screen());
        }

        break;
    }

    return;
}

void vLE_Exit(void *info)
{
    Base_State *gState = info;
    Vent_Level_Editor *editor = gState->info;

    /*Remove any graphics that should not be drawn*/
    gKer_FlushGraphics();

    /*Clean up test player*/
    vPlayer_Clean(&editor->testPlayer);

    /*Clear up ui*/
    uiSpine_Clean(&editor->spine);
    LE_ClearTileTemplates(editor);

    /*Clear up positon graph*/
    graph_Clean(&editor->positionGraph);

    /*Clear up sub menu variables*/
    vLE_TileEdit_Clean(&editor->tileEdit);
    vLE_LevelEdit_Clean(&editor->levelEdit);
    vLE_UnitEdit_Clean(&editor->unitEdit);
    vLE_NodeEdit_Clean(&editor->nodeEdit);
    vLE_MiscEdit_Clean(&editor->miscEdit);

    /*Clean up controls*/
    control_Clean(&editor->cRotateState);
    /*Reset the state of the curser*/
    uiPointer_SetFrame(&ve_Menu.pointer, 0);
    uiPointer_SetGrid(&ve_Menu.pointer, 0, 0, 0, 0);

    /*Clean up level*/
    vLevel_Clean(&editor->level);

    mem_Free(editor);

    return;
}
