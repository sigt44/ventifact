#include "MiscMenu.h"

#include "LevelEditor.h"
#include "../Menus/MenuAttributes.h"

void vLE_MiscEdit_Init(Vent_Level_MiscEditor *mEditor)
{
    mEditor->useObjectGrid = 0;
    mEditor->useGrid = 0;

    mEditor->gridWidth = 0;
    mEditor->gridHeight = 0;
    mEditor->objGridHeight = 0;
    mEditor->objGridWidth = 0;

    mEditor->cameraSpeed = 300;
    return;
}

void vLE_MiscEdit_Clean(Vent_Level_MiscEditor *mEditor)
{

    return;
}

/*Close the misc menu*/
static void BFA_CloseMiscMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];

    Vent_Level_MiscEditor *mEditor = &editor->miscEdit;

    uiSpine_Close(uiSpine_GetEntity(&editor->spine, "Spine:MiscEdit"));

    return;
}

/*Make sure the grid input regions only display if the use grid box is checked in*/
static void BFU_CheckGrid(Ui_Button *button)
{

    Vent_Level_MiscEditor *mEditor = (Vent_Level_MiscEditor *)button->info->dataArray[1];
    Ui_Spine *miscSpine = (Ui_Spine *)button->info->dataArray[2];

    MABFU_CheckBoxEnsureActivate(button);

    if(mEditor->useGrid == 1)
    {
        uiSpine_SetEntityUpdate(miscSpine, "Button:UseObjectGrid", 1, 1);

        if(mEditor->useObjectGrid == 0)
        {
            uiSpine_SetEntityUpdate(miscSpine, "TextInput:GridWidth", 1, 1);
            uiSpine_SetEntityUpdate(miscSpine, "TextInput:GridHeight", 1, 1);
        }
    }

    if(mEditor->useGrid == 0 || mEditor->useObjectGrid == 1)
    {
        uiSpine_SetEntityUpdate(miscSpine, "TextInput:GridWidth", 0, 0);
        uiSpine_SetEntityUpdate(miscSpine, "TextInput:GridHeight", 0, 0);
    }

    if(mEditor->useGrid == 0)
    {
        uiSpine_SetEntityUpdate(miscSpine, "Button:UseObjectGrid", 0, 0);
    }

    return;
}

void vLE_SetupMiscEditMenu(Vent_Level_Editor *editor)
{
    Ui_Spine *miscSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};
    Ui_Button *button = NULL;
    Sprite *sprite = NULL;

    Vent_Level_MiscEditor *mEditor = &editor->miscEdit;

    /*Setup the main window*/
    background = surf_SimpleBox(300, 240, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, editor->spine.layer + 1, 0, "Misc menu:");

    miscSpine = uiSpine_Create(editor->spine.layer + 1, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(&editor->spine, miscSpine, 0, "Spine:MiscEdit");

    /*Add in the close button*/
    uiSpine_AddButton(miscSpine, vLE_CloseButton(&BFA_CloseMiscMenu, dataStruct_CreateType("p", editor)), "Button:Close");

    /*Add in the use grid checkbox*/
    uiSpine_AddButton(miscSpine,
                      vLE_ButtonCheckBox("Use grid:", miscSpine->layer, &MABFA_CheckBoxActivate, &BFU_CheckGrid, dataStruct_Create(3, &mEditor->useGrid, mEditor, miscSpine)),
                      "Button:UseGrid");

    uiSpine_SetEntityPos(miscSpine, "Button:UseGrid", 80, 50);

    /*Add in the object grid size checkbox*/
    uiSpine_AddButton(miscSpine,
                      vLE_ButtonCheckBox("Use object grid size:", miscSpine->layer, &MABFA_CheckBoxActivate, &MABFU_CheckBoxEnsureActivate, dataStruct_Create(1, &mEditor->useObjectGrid)),
                      "Button:UseObjectGrid");

    uiSpine_SetEntityPos(miscSpine, "Button:UseObjectGrid", 150, 80);

    /*Pointer grid width*/
    uiSpine_AddTextInput(miscSpine,
         uiTextInput_Create(95, 100, miscSpine->layer, " Grid width:", font_Get(2, 13), 6, UI_OUTPUT_INT, &mEditor->gridWidth, &ve_Menu.c_Activate, miscSpine->pnt, &miscSpine->sTimer),
         "TextInput:GridWidth");

    /*Pointer grid height*/
    uiSpine_AddTextInput(miscSpine,
         uiTextInput_Create(95, 120, miscSpine->layer, "Grid height:", font_Get(2, 13), 6, UI_OUTPUT_INT, &mEditor->gridHeight, &ve_Menu.c_Activate, miscSpine->pnt, &miscSpine->sTimer),
         "TextInput:GridHeight");

    /*Camera speed*/
    uiSpine_AddTextInput(miscSpine,
         uiTextInput_Create(20, 150, miscSpine->layer, "Camera speed:", font_Get(2, 13), 4, UI_OUTPUT_INT, &mEditor->cameraSpeed, &ve_Menu.c_Activate, miscSpine->pnt, &miscSpine->sTimer),
         "TextInput:CameraSpeed");


    /*Add in the main buttons*/
    uiSpine_AddButton(miscSpine, veMenu_ButtonBasic(20, 210, "Ok", &miscSpine->sTimer, miscSpine->layer, &BFA_CloseMiscMenu, &MABFH_ButtonHover, dataStruct_CreateType("p", editor)), "Button:Ok");

    /*Setup button control mapping*/
    veMenu_SetSpineControl(miscSpine);
    uiSpine_MapEntity(miscSpine);

    uiMap_SetDefaultPath(&miscSpine->map, uiSpine_GetEntityBase(miscSpine, "Button:Ok"));

    return;
}
