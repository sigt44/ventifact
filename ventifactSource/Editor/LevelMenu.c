#include "LevelMenu.h"

#include "Kernel/Kernel.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "GUI/ButtonFunc.h"
#include "Font.h"

#include "../Menus/MenuAttributes.h"
#include "../ControlMap.h"
#include "../Level/LevelFile.h"
#include "../Level/Level.h"
#include "../Game/State.h"
#include "LevelEditor.h"
#include "TileMenu.h"

void vLE_LevelEdit_Init(Vent_Level_AttributesEditor *lEditor)
{
    lEditor->playerUnitName = "Any";

    lEditor->levelNames = NULL;
    lEditor->selectedLevel = "None";

    return;
}

void vLE_LevelEdit_Clean(Vent_Level_AttributesEditor *lEditor)
{
    list_ClearAll(&lEditor->levelNames);

    return;
}

/*Close the level menu*/
static void BFA_CloseLevelMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int closeType = (int)button->info->dataArray[1];

    Vent_Level_AttributesEditor *lEditor = &editor->levelEdit;

    uiSpine_Close(uiSpine_GetEntity(&editor->spine, "Spine:LevelEdit"));

    /*If the level width or height has changed*/
    if(editor->level.header.width != lEditor->previousHeader.width || editor->level.header.height != lEditor->previousHeader.height)
    {
        vLevel_ClearSectors(&editor->level);
        vLE_InitLevelSectors(editor);
    }

    /*Reset the ground if the ground ID has changed*/
    if(editor->level.header.baseTileID != lEditor->previousHeader.baseTileID)
    {
        vLevel_CleanGround(&editor->level);
        vLevel_SetupGround(&editor->level, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, editor->level.header.baseTileID, 0));
    }

    switch(closeType)
    {
        default:
        break;

        /*Ok button*/
        case VLE_STATE_BASE:

        break;
    }

    return;
}

/*Start the state to place the level side starting positions*/
static void BFA_StartSidePosition(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int side = (int)button->info->dataArray[1];

    vLE_SetObjectType(editor, side);

    vLE_SetState(editor->bState, VLE_STATE_PLACE);

    return;
}

static void BFA_ChangePlayerUnit(Ui_Button *button)
{
    Vent_Level_AttributesEditor *lEditor = (Vent_Level_AttributesEditor *)button->info->dataArray[0];
    int *playerUnitType = (int *)button->info->dataArray[1];

    if(*playerUnitType == VLEVEL_FORCE_ANY)
    {
        *playerUnitType = VLEVEL_FORCE_TANK;
        lEditor->playerUnitName = vUnitNames[*playerUnitType];
    }
    else if(*playerUnitType == VLEVEL_FORCE_TOWER)
    {
        *playerUnitType = VLEVEL_FORCE_TANK;
        lEditor->playerUnitName = "Any";
    }
    else
    {
        *playerUnitType = *playerUnitType + 1;
        lEditor->playerUnitName = vUnitNames[*playerUnitType];
    }

    return;
}

static void vLE_NewLevel(Vent_Level_Editor *editor)
{
    vLevel_Clean(&editor->level);
    vLevel_Setup(&editor->level, NULL, 1000, 1000);

    vLE_InitLevel(editor);

    vLE_ResetTileGraphicButtons(editor, uiSpine_GetEntity(&editor->spine, "Spine:GraphicID"));

    return;
}

static void BFA_NewLevel(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];

    vLE_NewLevel(editor);

    return;
}

static void BFA_TestLevel(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];
    Ui_Spine *levelSpine = uiSpine_GetEntity(&editor->spine, "Spine:LevelEdit");

    char *testMapName = "_EditorTestMap";
    char *testMapNameExt = "_EditorTestMap.map";

    vLevel_Save(&editor->level, kernel_GetPath("PTH_VentCustomLevels"), testMapName);

    /*Make sure the correct player unit is tested in the map*/
    if(editor->level.header.forcedPlayerUnit == VLEVEL_FORCE_ANY)
    {
        editor->testPlayer.startingUnit = UNIT_TANK;
    }
    else
    {
        editor->testPlayer.startingUnit = editor->level.header.forcedPlayerUnit;
    }

    editor->testPlayer.levelChosen = testMapNameExt;

    printf("Testing %s\n", editor->testPlayer.levelChosen);
    baseState_Push(ker_BaseStateList(), baseState_Create("Game test state", Game_Init, Game_Controls, Game_Logic, Game_Render, Game_Exit, &editor->testPlayer));

    return;
}

static void BFA_SaveLevel(Ui_Button *button)
{
    int *state = (int *)button->info->dataArray[0];
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[1];
    Ui_Spine *levelSpine = uiSpine_GetEntity(&editor->spine, "Spine:LevelEdit");

    if(*state == -1) /*Ask if sure*/
    {
        veMenu_WindowBool(levelSpine, "Spine:LevelSaveConfirm",
                          "Saving as %a.map, this may overwrite any file already named as that.\nAre you sure you want to save?", dataStruct_Create(1, editor->level.header.name), ker_Screen_Width()/2, ker_Screen_Height()/2,
                          button, (int *)button->info->dataArray[0]);
    }
    else if(*state == 0) /*No*/
    {

    }
    else if(*state == 1) /*Yes*/
    {
        vLevel_Save(&editor->level, kernel_GetPath("PTH_VentCustomLevels"), editor->level.header.name);

        veMenu_WindowOk(levelSpine, "Spine:SaveConfirmed", "Save complete.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, levelSpine->layer + 1);
    }

    /*Reset the state*/
    *state = -1;

    return;
}

/*Enable the tile graphic menu to change ground ID*/
static void BFA_OpenTileGraphicMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *levelSpine = uiSpine_GetEntity(&editor->spine, "Spine:LevelEdit");

    uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:GraphicID"), levelSpine);

    editor->graphicID = &editor->level.header.baseTileID;
    editor->tileEdit.chosenGraphicID = editor->level.header.baseTileID;
    editor->tileEdit.customGraphicID = vLevel_SurfaceNewID(editor->level.surfacesLoaded);

    return;
}

/*Enable the load level menu*/
static void BFA_OpenLoadMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *levelSpine = uiSpine_GetEntity(&editor->spine, "Spine:LevelEdit");
    Ui_Spine *loadSpine = uiSpine_GetEntity(levelSpine, "Spine:LevelLoad");

    vLE_ResetLevelFileScroll(editor, loadSpine);

    uiSpine_Open(loadSpine, levelSpine);

    return;
}

static void BFU_UpdateGroundGraphic(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];

    Frame *frame = NULL;

    frame = sprite_RetrieveFrame(button->graphic, 1);

    frame->graphic = vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, editor->level.header.baseTileID, 0);

    if(frame->graphic != NULL)
    {
        frame->enabled = 1;
        frame_SetClipToGraphic(frame);

        frame_SetMiddle(frame, 49, 49);

        frame->offsetX += 1;
        frame->offsetY += 1;
    }

    return;
}

void vLE_SetupLevelEditMenu(Vent_Level_Editor *editor)
{
    Ui_Spine *levelSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};
    Ui_Button *button = NULL;
    Sprite *sprite = NULL;

    Vent_Level_AttributesEditor *lEditor = &editor->levelEdit;

    /*Setup the forced unit variable*/
    if(editor->level.header.forcedPlayerUnit == VLEVEL_FORCE_ANY)
        lEditor->playerUnitName = "Any";
    else
        lEditor->playerUnitName = vUnitNames[editor->level.header.forcedPlayerUnit];

    /*Setup the main window*/
    background = surf_SimpleBox(300, 240, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, editor->spine.layer + 1, 0, "Level menu:");

    levelSpine = uiSpine_Create(editor->spine.layer + 1, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(&editor->spine, levelSpine, 0, "Spine:LevelEdit");

    /*Add in the close button*/
    uiSpine_AddButton(levelSpine, vLE_CloseButton(&BFA_CloseLevelMenu, dataStruct_Create(2, editor, VLE_STATE_BASE)), "Button:Close");

    /*Add in the main buttons*/
    uiSpine_AddButton(levelSpine, veMenu_ButtonBasic(0, 0, "Ok", &levelSpine->sTimer, levelSpine->layer, &BFA_CloseLevelMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_BASE)), "Button:Ok");
    uiSpine_AddButton(levelSpine, veMenu_ButtonBasic(0, 0, "Test", &levelSpine->sTimer, levelSpine->layer, &BFA_TestLevel, &MABFH_ButtonHover, dataStruct_CreateType("p", editor)), "Button:Test");
    uiSpine_AddButton(levelSpine, veMenu_ButtonBasic(0, 0, "Save", &levelSpine->sTimer, levelSpine->layer, &BFA_SaveLevel, &MABFH_ButtonHover, dataStruct_CreateType("dp", -1, editor)), "Button:Save");
    uiSpine_AddButton(levelSpine, veMenu_ButtonBasic(0, 0, "Load", &levelSpine->sTimer, levelSpine->layer, &BFA_OpenLoadMenu, &MABFH_ButtonHover, dataStruct_Create(1, editor)), "Button:Load");
    uiSpine_AddButton(levelSpine, veMenu_ButtonBasic(0, 0, "New", &levelSpine->sTimer, levelSpine->layer, &BFA_NewLevel, &MABFH_ButtonHover, dataStruct_Create(1, editor)), "Button:New");

    uiSpine_AlignEntities(levelSpine, 15, 210, 10, SCR_H, 5, "Button:Ok", "Button:Test", "Button:Save", "Button:Load", "Button:New");

    /*Name input*/
    uiSpine_AddTextInput(levelSpine,
                         uiTextInput_Create(20, 50, levelSpine->layer, "Name:", font_Get(2, 13), 16, UI_OUTPUT_STR, &editor->level.header.name, &ve_Menu.c_Activate, levelSpine->pnt, &levelSpine->sTimer),
                         "TextInput:Name");

    /*Width input*/
    uiSpine_AddTextInput(levelSpine,
                         uiTextInput_Create(20, 70, levelSpine->layer, "Width:", font_Get(2, 13), 5, UI_OUTPUT_INT, &editor->level.header.width, &ve_Menu.c_Activate, levelSpine->pnt, &levelSpine->sTimer),
                         "TextInput:Width");

    /*Height input*/
    uiSpine_AddTextInput(levelSpine,
                         uiTextInput_Create(20, 90, levelSpine->layer, "Height:", font_Get(2, 13), 5, UI_OUTPUT_INT, &editor->level.header.height, &ve_Menu.c_Activate, levelSpine->pnt, &levelSpine->sTimer),
                         "TextInput:Height");

    /*Force unit type*/
    uiSpine_AddButton(levelSpine,
                      vLE_ButtonOption("Player Unit: %s", dataStruct_Create(1, &lEditor->playerUnitName), levelSpine->layer, &BFA_ChangePlayerUnit, dataStruct_Create(2, lEditor, &editor->level.header.forcedPlayerUnit)),
                      "Button:ForceUnit");
    uiSpine_SetEntityPos(levelSpine, "Button:ForceUnit", 20, 110);

    /*Add in the player starting position button*/
    uiSpine_AddButton(levelSpine,
                      veMenu_ButtonBasic(20, 140, "Player Position", &levelSpine->sTimer, levelSpine->layer, &BFA_StartSidePosition, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_OBJECT_PLAYERSTART)),
                      "Button:PlayerStartingPosition"
    );

    /*Add in the enemy starting position button*/
    uiSpine_AddButton(levelSpine,
                      veMenu_ButtonBasic(20, 165, "Enemy Position", &levelSpine->sTimer, levelSpine->layer, &BFA_StartSidePosition, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_OBJECT_ENEMYSTART)),
                      "Button:EnemyStartingPosition"
    );

    /*Add in the ground tile graphic button*/
    sprite = sprite_Setup(sprite_Create(), 1, levelSpine->layer, &levelSpine->sTimer, 2,
                         frame_CreateBasic(-1, surf_SimpleBox(51, 51, (Uint16P *)ker_colourKey(), &colourBlack, 1), A_FREE),
                         frame_Create(0, NULL, 1, 1, 0, 0, 49, 49, M_FREE)
    );

    button = veMenu_ButtonSprite(195, 50, sprite, &levelSpine->sTimer, NULL, NULL, dataStruct_Create(1, editor));
    uiButton_SetUpdate(button, &BFU_UpdateGroundGraphic, 1);

    uiSpine_AddButton(levelSpine,
                      button,
                      "Button:GroundGraphic");

    /*Add in the change ground button*/
    uiSpine_AddButton(levelSpine,
    veMenu_ButtonBasic(170, 105, "Change Ground", &levelSpine->sTimer, levelSpine->layer, &BFA_OpenTileGraphicMenu, &MABFH_ButtonHover, dataStruct_Create(1, editor)),
    "Button:ChangeGround");

    /*Add in the stats*/
    uiSpine_AddTextBox(levelSpine,
       uiTextBox_CreateBase(170, 135, levelSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &levelSpine->sTimer),
       "TextBox:Stats");

    uiTextBox_AddText(uiSpine_GetEntity(levelSpine,"TextBox:Stats"),
                     0, 0, "Tiles: %d\nUnits: %d\nCustom images: %d", dataStruct_Create(3, &editor->level.header.numTiles, &editor->level.header.numSupply, &editor->level.header.numCustomSurfaces));


    /*Setup the button control map*/
    veMenu_SetSpineControl(levelSpine);
    uiSpine_MapEntity(levelSpine);

    /*Setup the load menu*/
    vLE_SetupLevelLoadMenu(editor, levelSpine);

    return;
}

/*Toggle the level directory between the game levels and custom levels*/
static void BFA_ToggleLevelDirectory(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *loadSpine = button->info->dataArray[1];
	int *toggle = (int *)button->info->dataArray[2];

	if(*toggle == 0)
	{
		*toggle = 1;
		uiSpine_SetEntityUpdate(loadSpine, "TextBox:CustomDIR", UIS_ENTITY_UPDATE, 1);
		uiSpine_SetEntityUpdate(loadSpine, "Scroll:CustomLevel", UIS_ENTITY_UPDATE, 1);
		uiSpine_SetEntityUpdate(loadSpine, "TextBox:GameDIR", UIS_ENTITY_NO_UPDATE, 0);
		uiSpine_SetEntityUpdate(loadSpine, "Scroll:MainLevel", UIS_ENTITY_NO_UPDATE, 0);
	}
	else
	{
		*toggle = 0;
		uiSpine_SetEntityUpdate(loadSpine, "TextBox:CustomDIR", UIS_ENTITY_NO_UPDATE, 0);
		uiSpine_SetEntityUpdate(loadSpine, "Scroll:CustomLevel", UIS_ENTITY_NO_UPDATE, 0);
		uiSpine_SetEntityUpdate(loadSpine, "TextBox:GameDIR", UIS_ENTITY_UPDATE, 1);
		uiSpine_SetEntityUpdate(loadSpine, "Scroll:MainLevel", UIS_ENTITY_UPDATE, 1);
	}

	return;
}

/*Close the load level menu*/
static void BFA_CloseLoadLevelMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *loadSpine = button->info->dataArray[1];
    int *closeType = (int *)button->info->dataArray[2];

    Vent_Level_AttributesEditor *lEditor = &editor->levelEdit;

    switch(*closeType)
    {
        default:
        break;

        /*New button*/
        case 2:

        vLE_NewLevel(editor);

        uiSpine_Close(loadSpine);

        break;

        /*Load button*/
        case 1:

        if(strcmp(lEditor->selectedLevel, "None") != 0)
        {
            vLevel_Clean(&editor->level);
            vLevel_Load(&editor->level, NULL, &editor->spine.sTimer, lEditor->selectedLevel);
            //vLevel_LoadOld(&editor->level, NULL, &editor->spine.sTimer, lEditor->selectedLevel);

            vLE_InitLevel(editor);

            vLE_ResetTileGraphicButtons(editor, uiSpine_GetEntity(&editor->spine, "Spine:GraphicID"));

            uiSpine_Close(loadSpine);
        }
        else
        {
            veMenu_WindowOk(loadSpine, "Spine:InvalidLevelLoad", "Please select a valid level.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, loadSpine->layer + 1);
        }

        break;

        /*Cancel button*/
        case 0:

        uiSpine_Close(loadSpine);

        break;
    }

    return;
}

void vLE_ResetLevelFileScroll(Vent_Level_Editor *editor, Ui_Spine *loadSpine)
{
    uiSpine_DeleteEntity(loadSpine, uiSpine_GetEntityBase(loadSpine, "Scroll:MainLevel"));
	uiSpine_DeleteEntity(loadSpine, uiSpine_GetEntityBase(loadSpine, "Scroll:CustomLevel"));


    uiSpine_AddScroll(loadSpine,
                      vLE_SetupLevelFileScroll(editor, loadSpine, kernel_GetPath("PTH_VentLevels")),
                      "Scroll:MainLevel");

	uiSpine_AddScroll(loadSpine,
                      vLE_SetupLevelFileScroll(editor, loadSpine,  kernel_GetPath("PTH_VentCustomLevels")),
                      "Scroll:CustomLevel");

    editor->levelEdit.selectedLevel = "None";

    uiSpine_MapEntity(loadSpine);

    return;
}

Ui_ButtonScroll *vLE_SetupLevelFileScroll(Vent_Level_Editor *editor, Ui_Spine *spine, char *directory)
{
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    char fileDIR[256];

    int x = 0;
    struct list *levelNames = NULL;

    Vent_Level_AttributesEditor *lEditor = &editor->levelEdit;

    strcpy(fileDIR, "../Levels/");

    if(lEditor->levelNames != NULL)
    {
        /*list_ClearAll(&lEditor->levelNames);*/
    }

    scroll = veMenu_ScrollFile(directory, ".map", &lEditor->levelNames, 40, 50, spine->layer, &spine->sTimer);
    scroll->rotate = 0;

    for(x = 0, levelNames = lEditor->levelNames; x < scroll->totalButtons; x++, levelNames = levelNames->next)
    {
        button = uiButtonScroll_GetButton(scroll, x);
        button->onActivate = BFunc_SetString;
        button->info = dataStruct_Create(2, levelNames->data, &lEditor->selectedLevel);
    }

    return scroll;
}

void vLE_SetupLevelLoadMenu(Vent_Level_Editor *editor, Ui_Spine *baseSpine)
{
    Ui_Spine *loadSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};
    Ui_Button *button = NULL;
    Sprite *sprite = NULL;

    Vent_Level_AttributesEditor *lEditor = &editor->levelEdit;

    /*Setup the main window*/
    background = surf_SimpleBox(300, 240, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, editor->spine.layer + 1, 0, "Load level:");

    loadSpine = uiSpine_Create(baseSpine->layer + 1, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(baseSpine, loadSpine, 0, "Spine:LevelLoad");

    /*Add in the selected level name*/
    uiSpine_AddTextBox(loadSpine,
       uiTextBox_CreateBase(105, 18, loadSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &loadSpine->sTimer),
       "TextBox:SelectedLevel");

    uiTextBox_AddText(uiSpine_GetEntity(loadSpine,"TextBox:SelectedLevel"),
                     0, 0, "%s", dataStruct_Create(1, &lEditor->selectedLevel));
	/*Add in the level directory*/
	uiSpine_AddTextBox(loadSpine,
       uiTextBox_CreateBase(205, 18, loadSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &loadSpine->sTimer),
       "TextBox:GameDIR");
	uiTextBox_AddText(uiSpine_GetEntity(loadSpine,"TextBox:GameDIR"),
                     0, 0, "GAME", NULL);

	uiSpine_AddTextBox(loadSpine,
       uiTextBox_CreateBase(205, 18, loadSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &loadSpine->sTimer),
       "TextBox:CustomDIR");
	uiTextBox_AddText(uiSpine_GetEntity(loadSpine,"TextBox:CustomDIR"),
                     0, 0, "CUSTOM", NULL);


    /*Add in the close button*/
    uiSpine_AddButton(loadSpine, vLE_CloseButton(&BFA_CloseLoadLevelMenu, dataStruct_CreateType("ppd", editor, loadSpine, 0)), "Button:Close");

    /*Add in the main buttons*/
    uiSpine_AddButton(loadSpine, veMenu_ButtonBasic(0, 0, "Load", &loadSpine->sTimer, loadSpine->layer, &BFA_CloseLoadLevelMenu, &MABFH_ButtonHover, dataStruct_CreateType("ppd", editor, loadSpine, 1)), "Button:Load");
    uiSpine_AddButton(loadSpine, veMenu_ButtonBasic(0, 0, "New", &loadSpine->sTimer, loadSpine->layer, &BFA_CloseLoadLevelMenu, &MABFH_ButtonHover, dataStruct_CreateType("ppd", editor, loadSpine, 2)), "Button:New");
    uiSpine_AddButton(loadSpine, veMenu_ButtonBasic(15, 185, "Switch DIR", &loadSpine->sTimer, loadSpine->layer, &BFA_ToggleLevelDirectory, &MABFH_ButtonHover, dataStruct_CreateType("ppd", editor, loadSpine, 0)), "Button:Switch");
    uiSpine_AddButton(loadSpine, veMenu_ButtonBasic(0, 0, "Cancel", &loadSpine->sTimer, loadSpine->layer, &BFA_CloseLoadLevelMenu, &MABFH_ButtonHover, dataStruct_CreateType("ppd", editor, loadSpine, 0)), "Button:Cancel");

    uiSpine_AlignEntities(loadSpine, 15, 210, 10, SCR_H, 3, "Button:Load", "Button:New", "Button:Cancel");

    uiSpine_AddScroll(loadSpine,
                      vLE_SetupLevelFileScroll(editor, loadSpine, kernel_GetPath("PTH_VentLevels")),
                      "Scroll:MainLevel");

	uiSpine_AddScroll(loadSpine,
                      vLE_SetupLevelFileScroll(editor, loadSpine,  kernel_GetPath("PTH_VentCustomLevels")),
                      "Scroll:CustomLevel");

    /*Setup the button control map*/
    veMenu_SetSpineControl(loadSpine);
    uiSpine_MapEntity(loadSpine);

	uiSpine_SetEntityUpdate(loadSpine, "TextBox:CustomDIR", UIS_ENTITY_NO_UPDATE, 0);
	uiSpine_SetEntityUpdate(loadSpine, "Scroll:CustomLevel", UIS_ENTITY_NO_UPDATE, 0);

    return;
}
