#include "TileMenu.h"

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Graphics/Graphics.h"
#include "Graphics/PixelManager.h"
#include "Graphics/Surface.h"
#include "GUI/ButtonFunc.h"
#include "Font.h"

#include "../Menus/MenuAttributes.h"
#include "../ControlMap.h"

/*Close the tile menu and move to the next state of the editor, depending on
which button was pressed.*/
static void BFA_CloseTileMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int closeType = (int)button->info->dataArray[1];

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    /*Obtain the custom tile template*/

    /*Copy the generic template tile contents to the custom tile template contents*/
    vTile_Copy(1, tEditor->customTile, &tEditor->templateTile);

    vLE_SetObjectType(editor, VLE_OBJECT_TILE);

    switch(closeType)
    {
        default:
        break;

        case VLE_STATE_REMOVE:
        vLE_SetState(editor->bState, VLE_STATE_REMOVE);

        break;

        case VLE_STATE_PLACE:

        tEditor->selectedTile = &tEditor->templateTile;

        vLE_SetState(editor->bState, VLE_STATE_PLACE);

        break;

        case VLE_STATE_REPLACE:

        tEditor->selectedTile = &tEditor->templateTile;

        vLE_SetState(editor->bState, VLE_STATE_REPLACE);

        break;

        case VLE_STATE_COPY:

        vLE_SetState(editor->bState, VLE_STATE_COPY);

        break;

        /*Cancel*/
        case VLE_STATE_BASE:

        uiSpine_Close(uiSpine_GetEntity(&editor->spine, "Spine:TileEdit"));

        break;
    }

    return;
}

/*Make sure that the current tile graphic being shown is the correct one*/
static void BFU_UpdateTileGraphic(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];
    Ui_Spine *tileSpine = (Ui_Spine *)button->info->dataArray[1];
    Vent_Tile *templateTile = &editor->tileEdit.templateTile;

    Sprite *tSprite = uiSpine_GetEntity(tileSpine, "Sprite:Tile");

    Frame *frame = NULL;

    frame = sprite_RetrieveFrame(tSprite, 1);

    frame->graphic = vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, templateTile->base.graphicType, templateTile->base.rotationDeg);

    if(templateTile->rotation != templateTile->base.rotationDeg)
    {
        vTile_ChangeGraphicSurface(templateTile, editor->level.surfacesLoaded, templateTile->base.graphicType);
        templateTile->base.width = sprite_Width(templateTile->sImage);
        templateTile->base.height = sprite_Height(templateTile->sImage);
    }

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

/*Make sure that the health input box is only showing if destroyable is flagged*/
static void BFU_CheckDestroyable(Ui_Button *button)
{
    unsigned int *flags = (unsigned int *)button->info->dataArray[0];
    unsigned int destroyable = (unsigned int)button->info->dataArray[1];
    Ui_Spine *tileSpine = (Ui_Spine *)button->info->dataArray[2];

    MABFU_CheckBoxEnsureFlag(button);

    if(flag_Check(flags, destroyable) == 1)
    {
        uiSpine_SetEntityUpdate(tileSpine, "TextInput:Health", 1, 1);
    }
    else
    {
        uiSpine_SetEntityUpdate(tileSpine, "TextInput:Health", 0, 0);
    }

    return;
}

/*Make sure that the buildable flag is not checked if the collision flag is*/
static void BFU_CheckCollision(Ui_Button *button)
{
    int *collision = (int *)button->info->dataArray[0];
    unsigned int *flags = (unsigned int *)button->info->dataArray[1];

    MABFU_CheckBoxEnsureActivate(button);

    if(*collision == 1 && flag_Check(flags, TILESTATE_BUILDABLE) == 1)
    {
        flag_Off(flags, TILESTATE_BUILDABLE);
    }

    return;
}

/*Make sure that the destroyable box is checked in if important is flagged*/
static void BFU_CheckImportant(Ui_Button *button)
{
    unsigned int *flags = (unsigned int *)button->info->dataArray[0];
    unsigned int important = (unsigned int)button->info->dataArray[1];

    MABFU_CheckBoxEnsureFlag(button);

    if(flag_Check(flags, important) == 1 && flag_Check(flags, TILESTATE_DESTROYABLE) == 0)
    {
        flag_On(flags, TILESTATE_DESTROYABLE);
    }

    return;
}

/*Make sure that the group input is displayed if the tile is buildable*/
static void BFU_CheckGroup(Ui_Button *button)
{
    unsigned int *flags = (unsigned int *)button->info->dataArray[0];
    Ui_Spine *tileSpine = (Ui_Spine *)button->info->dataArray[2];

    MABFU_CheckBoxEnsureFlag(button);

    if(flag_Check(flags, TILESTATE_BUILDABLE) == 1)
    {
        uiSpine_SetEntityUpdate(tileSpine, "TextInput:Group", 1, 1);
    }
    else
    {
        uiSpine_SetEntityUpdate(tileSpine, "TextInput:Group", 0, 0);
    }

    return;
}

/*Change the template tile to a different one*/
static void BFA_ChangeTile(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];
    int direction = (int)button->info->dataArray[1];

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;
    struct list *tilePos = list_MatchCheck(tEditor->loadedTiles, tEditor->selectedTile);

    if(tilePos == NULL)
    {
        tilePos = tEditor->loadedTiles;

        while(tilePos != NULL)
        {
            if(tilePos->info == tEditor->tileTemplateType)
                break;

            tilePos = tilePos->next;
        }

        if(tilePos == NULL)
        {
            printf("Error cannot find the next tile template from %p %d\n", tEditor->selectedTile, tEditor->tileTemplateType);

            return;
        }
    }

    if(direction == D_UP)
    {
        tilePos = tilePos->next;
        if(tilePos == NULL)
            tilePos = tEditor->loadedTiles;

        tEditor->selectedTile = tilePos->data;
    }
    else if(direction == D_DOWN)
    {
        tilePos = tilePos->previous;
        tEditor->selectedTile = tilePos->data;
    }

    tEditor->tileTemplateType = tilePos->info;

    vTile_Copy(1, &tEditor->templateTile, tEditor->selectedTile);

    return;
}

/*Enable the tile graphic menu*/
static void BFA_OpenTileGraphicMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *tileSpine = uiSpine_GetEntity(&editor->spine, "Spine:TileEdit");

    uiSpine_Open(uiSpine_GetEntity(&editor->spine, "Spine:GraphicID"), tileSpine);

    editor->graphicID = &editor->tileEdit.templateTile.base.graphicType;
    editor->tileEdit.chosenGraphicID = editor->tileEdit.templateTile.base.graphicType;
    editor->tileEdit.customGraphicID = vLevel_SurfaceNewID(editor->level.surfacesLoaded);

    return;
}

void vLE_TileEdit_Init(Vent_Level_TileEditor *tEditor)
{
    tEditor->loadedTiles = NULL;
    tEditor->selectedTile = NULL;
    tEditor->customTile = NULL;
    tEditor->graphicNames = NULL;

    vTile_Setup(&tEditor->templateTile, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);

    tEditor->chosenGraphicID = 0;
    tEditor->customGraphicID = 0;
    tEditor->customGraphicName = "None";
    tEditor->tileTemplateType = 0;

    return;
}

void vLE_TileEdit_Clean(Vent_Level_TileEditor *tEditor)
{
    vTile_Clean(&tEditor->templateTile);

    if(tEditor->graphicNames != NULL)
    {
        list_ClearAll(&tEditor->graphicNames);
    }

    return;
}

void vLE_SetupTileEditMenu(Vent_Level_Editor *editor)
{
    Ui_Spine *tileSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};
    Ui_Button *button = NULL;
    Ui_TextBox *textBox = NULL;
    Sprite *sprite = NULL;

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    Vent_Tile *tile = &tEditor->templateTile;
    //printf("Template %p, selected %p\n", &tEditor->templateTile, tEditor->selectedTile);
    /*Setup the template tile first so that the buttons can take values from it*/
    vTile_Copy(0, &tEditor->templateTile, tEditor->selectedTile);

    /*Setup the main window*/
    background = surf_SimpleBox(400, 250, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, VL_HUD + 2, 0, "Tile menu:");

    tileSpine = uiSpine_Create(VL_HUD + 2, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(&editor->spine, tileSpine, 0, "Spine:TileEdit");

    /*Add in the close button*/
    uiSpine_AddButton(tileSpine, vLE_CloseButton(&BFA_CloseTileMenu, dataStruct_Create(2, editor, VLE_STATE_BASE)), "Button:Close");

    /*Add in the main buttons*/
    uiSpine_AddButton(tileSpine, veMenu_ButtonBasic(0, 0, "Ok", &tileSpine->sTimer, VL_HUD + 2, &BFA_CloseTileMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_BASE)), "Button:Ok");
    uiSpine_AddButton(tileSpine, veMenu_ButtonBasic(0, 0, "Place", &tileSpine->sTimer, VL_HUD + 2, &BFA_CloseTileMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_PLACE)), "Button:Place");
    uiSpine_AddButton(tileSpine, veMenu_ButtonBasic(0, 0, "Replace", &tileSpine->sTimer, VL_HUD + 2, &BFA_CloseTileMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_REPLACE)), "Button:Replace");
    uiSpine_AddButton(tileSpine, veMenu_ButtonBasic(0, 0, "Copy", &tileSpine->sTimer, VL_HUD + 2, &BFA_CloseTileMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_COPY)), "Button:Copy");
    uiSpine_AddButton(tileSpine, veMenu_ButtonBasic(0, 0, "Remove", &tileSpine->sTimer, VL_HUD + 2, &BFA_CloseTileMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_REMOVE)), "Button:Remove");

    uiSpine_AlignEntities(tileSpine, 15, 210, 10, SCR_H, 5, "Button:Ok", "Button:Place", "Button:Replace", "Button:Copy", "Button:Remove");

    /*Add in tile options*/

    uiSpine_AddButton(tileSpine,
                      vLE_ButtonOption("Team: %si", dataStruct_Create(2, &leTeamNames[0], &tile->base.team), tileSpine->layer, &BFunc_RotateInt, dataStruct_Create(3, &tile->base.team, 3, D_UP)),
                      "Button:Team");

    uiSpine_AddButton(tileSpine,
                      vLE_ButtonOption("Type: %si", dataStruct_Create(2, &vTile_Names[0], &tile->base.type), tileSpine->layer, &BFunc_RotateInt, dataStruct_Create(3, &tile->base.type, TILE_ENDTYPE, D_UP)),
                      "Button:Type");

    uiSpine_SetEntityPos(tileSpine, "Button:Team", 20, 50);
    uiSpine_SetEntityPos(tileSpine, "Button:Type", 20, 75);

    /*Health input*/
    uiSpine_AddTextInput(tileSpine,
                         uiTextInput_Create(130, 150, tileSpine->layer, "Health:", font_Get(2, 13), 5, UI_OUTPUT_INT, &tile->base.healthStarting, &ve_Menu.c_Activate, tileSpine->pnt, &tileSpine->sTimer),
                         "TextInput:Health");

    /*Width input*/
    uiSpine_AddTextInput(tileSpine,
                         uiTextInput_Create(290, 150, tileSpine->layer, "Width:", font_Get(2, 13), 4, UI_OUTPUT_INT, &tile->base.width, &ve_Menu.c_Activate, tileSpine->pnt, &tileSpine->sTimer),
                         "TextInput:Width");

    /*Height input*/
    uiSpine_AddTextInput(tileSpine,
                         uiTextInput_Create(290, 170, tileSpine->layer, "Height:", font_Get(2, 13), 4, UI_OUTPUT_INT, &tile->base.height, &ve_Menu.c_Activate, tileSpine->pnt, &tileSpine->sTimer),
                         "TextInput:Height");

    /*Rotation input*/
    uiSpine_AddTextInput(tileSpine,
                         uiTextInput_Create(290, 190, tileSpine->layer, "Rotation:", font_Get(2, 13), 4, UI_OUTPUT_INT, &tile->base.rotationDeg, &ve_Menu.c_Activate, tileSpine->pnt, &tileSpine->sTimer),
                         "TextInput:Rotation");

    /*Add in the check box options*/
    scroll = uiButtonScroll_Create(130, 50,
                               SCR_V, 10, 5, 0, 0,
                               NULL, NULL, 4,
                               vLE_ButtonCheckBox("Collision:", tileSpine->layer, &MABFA_CheckBoxActivate, &BFU_CheckCollision, dataStruct_Create(2, &tile->base.collision, &tile->base.stateFlags)),
                               vLE_ButtonCheckBox("Buildable:", tileSpine->layer, &MABFA_CheckBoxToggleFlag, &BFU_CheckGroup, dataStruct_Create(3, &tile->base.stateFlags, TILESTATE_BUILDABLE, tileSpine)),
                               vLE_ButtonCheckBox("Important:", tileSpine->layer, &MABFA_CheckBoxToggleFlag, &BFU_CheckImportant, dataStruct_Create(2, &tile->base.stateFlags, TILESTATE_IMPORTANT)),
                               vLE_ButtonCheckBox("Destroyable:", tileSpine->layer, &MABFA_CheckBoxToggleFlag, &BFU_CheckDestroyable, dataStruct_Create(3, &tile->base.stateFlags, TILESTATE_DESTROYABLE, tileSpine))
                               );

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);
    uiSpine_AddScroll(tileSpine, scroll, "Scroll:TileCheckbox");

    /*Group input*/
    uiSpine_AddTextInput(tileSpine,
                     uiTextInput_Create(20, 105, tileSpine->layer, "Group:", font_Get(2, 13), 3, UI_OUTPUT_INT, &tile->base.group, &ve_Menu.c_Activate, tileSpine->pnt, &tileSpine->sTimer),
                     "TextInput:Group");

    /*Layer scroll*/
    scrollButton[0] = veMenu_ButtonSprite(20, 170, sprite_Copy(&ve_Sprites.scroll[D_LEFT], 1), &tileSpine->sTimer, &BFunc_MoveInt, &MABFH_ButtonHover, dataStruct_Create(3, &tile->base.layer, 0, D_DOWN));
    scrollButton[0]->graphic->layer = tileSpine->layer;

    uiSpine_AddButton(tileSpine, scrollButton[0], "Button:LayerLeft");

    scrollButton[1] = veMenu_ButtonSprite(40, 170, sprite_Copy(&ve_Sprites.scroll[D_RIGHT], 1), &tileSpine->sTimer, &BFunc_MoveInt, &MABFH_ButtonHover,  dataStruct_Create(3, &tile->base.layer, 5, D_UP));
    scrollButton[1]->graphic->layer = tileSpine->layer;

    uiSpine_AddButton(tileSpine, scrollButton[1], "Button:LayerRight");

    textBox = uiTextBox_CreateBase(20, 150, tileSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &tileSpine->sTimer);

    uiTextBox_AddText(textBox, 0, 0, "Layer: %si", dataStruct_Create(2, &leLayerNames[0], &tile->base.layer));

    uiSpine_AddTextBox(tileSpine, textBox, "TextBox:Layer");

    /*Template scroll*/
    scrollButton[0] = veMenu_ButtonSprite(270, 70, sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1), &tileSpine->sTimer, &BFA_ChangeTile, &MABFH_ButtonHover, dataStruct_Create(2, editor, D_DOWN));
    scrollButton[0]->graphic->layer = tileSpine->layer;

    uiSpine_AddButton(tileSpine, scrollButton[0], "Button:TemplateDown");

    scrollButton[1] = veMenu_ButtonSprite(270, 50, sprite_Copy(&ve_Sprites.scroll[D_UP], 1), &tileSpine->sTimer, &BFA_ChangeTile, &MABFH_ButtonHover, dataStruct_Create(2, editor, D_UP));
    scrollButton[1]->graphic->layer = tileSpine->layer;

    uiSpine_AddButton(tileSpine, scrollButton[1], "Button:TemplateUp");

    textBox = uiTextBox_CreateBase(235, 15, tileSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &tileSpine->sTimer);

    uiTextBox_AddText(textBox, 0, 0, "Template: %si", dataStruct_Create(2, &leTileTemplateNames[0], &tEditor->tileTemplateType));

    uiSpine_AddTextBox(tileSpine, textBox, "TextBox:Template");


    /*Add in the tile graphic sprite*/
    sprite = sprite_Setup(sprite_Create(), 1, tileSpine->layer, &tileSpine->sTimer, 2,
                         frame_CreateBasic(-1, surf_SimpleBox(51, 51, (Uint16P *)ker_colourKey(), &colourBlack, 1), A_FREE),
                         frame_Create(0, NULL, 1, 1, 0, 0, 49, 49, M_FREE)
    );

    sprite_SetPos(sprite, 300, 50);

    uiSpine_AddSprite(tileSpine, sprite, A_FREE, "Sprite:Tile");

    /*Add in the change graphic button*/
    button = veMenu_ButtonBasic(270, 105, "Change Graphic", &tileSpine->sTimer, tileSpine->layer, &BFA_OpenTileGraphicMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, tileSpine));
    uiButton_SetUpdate(button, &BFU_UpdateTileGraphic, 1);

    uiSpine_AddButton(tileSpine, button, "Button:ChangeGraphic");


    /*Setup the button control map*/
    veMenu_SetSpineControl(tileSpine);
    uiSpine_MapEntity(tileSpine);

    /*Setup the tile graphic menu*/
    //vLE_SetupTileGraphicMenu(editor, tileSpine, &tEditor->templateTile.base.graphicType);

    return;
}

/*Close the tile graphic menu, if select was pressed then set the
graphic type to the template tile.*/
static void BFA_CloseTileGraphicMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *graphicIDSpine = button->info->dataArray[1];
    int **graphicID = (int **)button->info->dataArray[2];
    int closeType = (int)button->info->dataArray[3];

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    uiSpine_Close(graphicIDSpine);

    editor->level.header.numCustomSurfaces = vLevel_GetCustomSurfaces(&editor->level);

    switch(closeType)
    {
        default:
        break;

        /*Cancel*/
        case 0:

        break;

        /*Select*/
        case 1:

        **graphicID = tEditor->chosenGraphicID;

        if(*graphicID == &tEditor->templateTile.base.graphicType)
        {
            vTile_ChangeGraphic(&tEditor->templateTile,
                                sprite_Setup(sprite_Create(), 1, VL_VEHICLE, &editor->spine.sTimer, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, tEditor->templateTile.base.graphicType, 0), M_FREE)));

            tEditor->templateTile.base.width = sprite_Width(tEditor->templateTile.sImage);
            tEditor->templateTile.base.height = sprite_Height(tEditor->templateTile.sImage);
            tEditor->templateTile.base.rotationDeg = 0;
        }

        break;
    }

    return;
}

static void BFA_SelectTileGraphic(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int graphicType = (int)button->info->dataArray[1];

    /*Set the selected frame*/
    sprite_SetFrame(button->graphic, 3);

    //printf("Setting chosen type to %d\n", graphicType);
    editor->tileEdit.chosenGraphicID = graphicType;
    editor->tileEdit.customGraphicID = graphicType;

    return;
}

static void BFU_CheckTileGraphic(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int graphicType = (int)button->info->dataArray[1];

    if(button->state == BUTTON_STATE_IDLE && graphicType != editor->tileEdit.chosenGraphicID)
    {
        /*Revert back to the original frame*/
        sprite_SetFrame(button->graphic, 2);
    }
    else if(graphicType == editor->tileEdit.chosenGraphicID && sprite_GetFrameIndex(button->graphic) != 3)
    {
        sprite_SetFrame(button->graphic, 3);
    }


    return;
}

static void BFH_HoverTileGraphic(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int graphicType = (int)button->info->dataArray[1];

    if(graphicType != editor->tileEdit.chosenGraphicID)
    {
        /*Set the hover frame*/
        sprite_SetFrame(button->graphic, 4);
    }

    return;
}

static void BFA_ScrollTileCam(Ui_Button *button)
{
    Camera_2D *camera = button->info->dataArray[0];
    int direction = (int)button->info->dataArray[1];
    float *deltaTime = (float *)button->info->dataArray[2];

    if(direction == D_UP)
    {
        camera2D_ChangePosition(camera, 0, (int)(-200.0f * *deltaTime));
    }
    else
    {
        camera2D_ChangePosition(camera, 0, (int)(200.0f * *deltaTime));
    }

    return;
}

static void BFA_AddRemoveImage(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    Ui_Spine *tGraphicSpine = button->info->dataArray[1];
    int add = (int)button->info->dataArray[2];
    int *graphicID = (int *)button->info->dataArray[3];

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;
    int originalID = *graphicID;

    char fullPath[255];
	char *path;

    /*Get the path to the image to load*/

    strcpy(fullPath, kernel_GetPath("PTH_VentCustomTextures"));
    strcat(fullPath, editor->tileEdit.customGraphicName);

    /*Make sure the custom graphic ID is < 0 to show it is custom*/
    if(*graphicID >= 0)
    {
        veMenu_WindowOk(tGraphicSpine, "Spine:IDWarning", "ID must be below 0.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, tGraphicSpine->layer + 2);
        return;
    }

    if(add == 1) /*Add the image*/
    {
        vLevel_SurfaceInsert(&editor->level.surfacesLoaded, fullPath, *graphicID);

        /*Find a new valid custom graphic ID*/
        tEditor->customGraphicID = vLevel_SurfaceNewID(editor->level.surfacesLoaded);
    }
    else if(add == 0) /*Remove the image*/
    {
        /*If the custom graphic is the unknown surface, then try to remove the surface from the level completely*/
        if(ve_Surfaces.unknown == vLevel_SurfaceRetrieve(editor->level.surfacesLoaded, *graphicID, 0))
        {
            /*The custom graphic must not be used by any tiles in the level*/
            if(vLevel_CheckTileGraphic(&editor->level, *graphicID) > 0)
            {
                veMenu_WindowOk(tGraphicSpine, "Spine:RemainingTilesWarning", "There are still tiles with this graphic in the level.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, tGraphicSpine->layer + 2);
            }
            else
            {
                vLevel_SurfaceRemove(&editor->level.surfacesLoaded, *graphicID);

                /*If the template tile is using this graphic, need to fix it*/
                if(tEditor->templateTile.base.graphicType == *graphicID)
                {
                    vTile_ChangeGraphicSurface(&tEditor->templateTile, editor->level.surfacesLoaded, 0);
                    vTile_Copy(1, tEditor->customTile, &tEditor->templateTile);
                }

                *graphicID = 0;
            }
        }
        else
        {
            /*This sets the custom graphic to be the unknown surface*/
            vLevel_SurfaceDisable(&editor->level.surfacesLoaded, *graphicID);

            /*If the template tile is using this graphic, need to fix it*/
            if(tEditor->templateTile.base.graphicType == *graphicID)
            {
                vTile_ChangeGraphicSurface(&tEditor->templateTile, editor->level.surfacesLoaded, tEditor->chosenGraphicID);
                vTile_Copy(1, tEditor->customTile, &tEditor->templateTile);
            }
        }
    }

    vLE_ResetTileGraphicButtons(editor, tGraphicSpine);

    /*Update any tiles placed in the level that have been affected by the change in images*/
    vLevel_ChangeTileGraphic(&editor->level, originalID, originalID);

    return;
}

void vLE_ResetTileGraphicButtons(Vent_Level_Editor *editor, Ui_Spine *tGraphicSpine)
{
    Ui_ButtonScroll *scroll = NULL;

    /*Delete the current graphic buttons*/
    uiSpine_DeleteEntity(tGraphicSpine, uiSpine_GetEntityBase(tGraphicSpine, "Spine:TileScroll"));
    uiSpine_DeleteEntity(tGraphicSpine, uiSpine_GetEntityBase(tGraphicSpine, "Button:TileScrollUp"));
    uiSpine_DeleteEntity(tGraphicSpine, uiSpine_GetEntityBase(tGraphicSpine, "Button:TileScrollDown"));

    /*Create a new one*/
    vLE_SetupTileGraphicButtons(editor, tGraphicSpine);

    return;
}

void vLE_SetupTileGraphicButtons(Vent_Level_Editor *editor, Ui_Spine *tGraphicSpine)
{
    Ui_Spine *mainWindow = NULL;

    Ui_Button *button = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};

    Sprite *sprite = NULL;
    Frame *tileFrame = NULL;
    Frame *tileHoverFrame = NULL;
    Frame *tileEnabledFrame = NULL;
    Vent_LevelSurface *levelSurface = NULL;

    SDL_Surface *background = NULL;

    int rotateX = 0;
    int yLevel = 0;
    int xPadding = 10;
    int yPadding = 10;
    int buttonNum = 0;
    char buttonName[255];
    struct list *surfacesLoaded = editor->level.surfacesLoaded;

    background = surf_SimpleBox(244, 184, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*First setup a new window inside of the main tile chooser window*/
    mainWindow = uiSpine_Create(tGraphicSpine->layer + 1, 20, 40, frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);
    uiSpine_AddSpine(tGraphicSpine, mainWindow, 1, "Spine:TileScroll");

    uiSpine_Open(mainWindow, NULL);

    /*Setup the camera that will pan through that window, acting as a scrollbar*/
    uiSpine_SetupCamera(mainWindow, camera2D_Create(), 244, 184, mainWindow->rect.x + 244, 9999);
    camera2D_SetOrigin(mainWindow->camera, mainWindow->rect.x, mainWindow->rect.y);

    while(surfacesLoaded != NULL)
    {
        levelSurface = surfacesLoaded->data;

        /*Create the tile graphic frame*/
        tileFrame = frame_Create(-1, levelSurface->surface, 0, 0, 0, 0, 0, 0, M_FREE);
        tileHoverFrame = frame_CreateBasic(1, surf_SimpleBox(51, 51, (Uint16P *)ker_colourKey(), &colourBlack, 2), A_FREE);
        tileEnabledFrame = frame_CreateBasic(0, surf_SimpleBox(51, 51, (Uint16P *)ker_colourKey(), &colourBlack, 4), A_FREE);

        /*Make sure the graphic lies in the middle of the frame*/
        frame_SetMiddle(tileFrame, 49, 49);
        tileFrame->offsetX += 1;
        tileFrame->offsetY += 1;

        /*Create the sprite*/
        sprite = sprite_Setup(sprite_Create(), 1, mainWindow->layer, &mainWindow->sTimer, 5,
                             frame_CreateBasic(-1, surf_SimpleBox(51, 51, (Uint16P *)ker_colourKey(), &colourBlack, 1), A_FREE),
                             tileFrame,
                             frame_CreateEmpty(),
                             tileEnabledFrame,
                             tileHoverFrame
        );

        /*Create the button*/
        button = veMenu_ButtonSprite(5 + (rotateX * (51 + xPadding)), 5 + (yLevel * (51 + yPadding)),
                                     sprite,
                                     &mainWindow->sTimer,
                                     &BFA_SelectTileGraphic,
                                     &BFH_HoverTileGraphic,
                                     dataStruct_Create(2, editor, levelSurface->ID));

        uiButton_SetUpdate(button, BFU_CheckTileGraphic, 0);

        snprintf(buttonName, 254, "Button:Tile[%d]", buttonNum);

        uiSpine_AddButton(mainWindow, button, buttonName);

        rotateX = (rotateX + 1) % 4;
        if(rotateX == 0)
            yLevel ++;

        buttonNum++;

        surfacesLoaded = surfacesLoaded->next;
    }

    scrollButton[0] = uiButton_Create(275, 50, sprite_Width(&ve_Sprites.scroll[D_UP]), sprite_Height(&ve_Sprites.scroll[D_UP]), &BFA_ScrollTileCam, &MABFH_ButtonHover, dataStruct_Create(3, mainWindow->camera, D_UP, &editor->deltaTime), sprite_Copy(&ve_Sprites.scroll[D_UP], 1), 0, 0, &tGraphicSpine->sTimer);
    scrollButton[0]->graphic->layer = tGraphicSpine->layer;
    uiButton_CopyControl(scrollButton[0], &ve_Controls.cMouseHold, BC_POINTER);

    uiSpine_AddButton(tGraphicSpine, scrollButton[0], "Button:TileScrollUp");

    scrollButton[1] = uiButton_Create(275, 70, sprite_Width(&ve_Sprites.scroll[D_DOWN]), sprite_Height(&ve_Sprites.scroll[D_DOWN]), &BFA_ScrollTileCam, &MABFH_ButtonHover, dataStruct_Create(3, mainWindow->camera, D_DOWN, &editor->deltaTime), sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1), 0, 0, &tGraphicSpine->sTimer);
    scrollButton[1]->graphic->layer = tGraphicSpine->layer;
    uiButton_CopyControl(scrollButton[1], &ve_Controls.cMouseHold, BC_POINTER);

    uiSpine_AddButton(tGraphicSpine, scrollButton[1], "Button:TileScrollDown");

    camera2D_SetConstraints(mainWindow->camera, mainWindow->camera->minX, mainWindow->camera->minY, mainWindow->camera->maxX, mainWindow->camera->originRect.y + 2 + (yLevel + 1) * (51 + yPadding));

    //uiSpine_MapEntity(tGraphicSpine);

    /*Setup the button control map*/
    //veMenu_SetSpineControl(mainWindow);
    //uiSpine_MapEntity(mainWindow);

    return;
}

static Ui_ButtonScroll *vLE_SetupImageScroll(Vent_Level_Editor *editor, Ui_Spine *spine)
{
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    char fileDIR[254];

    int x = 0;
    struct list *graphicNames = NULL;
	char *path;

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    strncpy(fileDIR, kernel_GetPath("PTH_Textures"), 239);
    strcat(fileDIR, "Level/Custom/");

    if(tEditor->graphicNames != NULL)
    {
        list_ClearAll(&tEditor->graphicNames);
    }

    scroll = veMenu_ScrollFile(fileDIR, ".png", &tEditor->graphicNames, 340, 50, spine->layer, &spine->sTimer);
    scroll->rotate = 0;
    scroll->maxShowButtons = 8;

    for(x = 0, graphicNames = tEditor->graphicNames; x < scroll->totalButtons; x++, graphicNames = graphicNames->next)
    {
        button = uiButtonScroll_GetButton(scroll, x);
        button->onActivate = BFunc_SetString;
        button->info = dataStruct_Create(2, graphicNames->data, &tEditor->customGraphicName);
    }

    return scroll;
}

void vLE_ResetTileImageScroll(Vent_Level_Editor *editor, Ui_Spine *tGraphicSpine)
{
    Ui_ButtonScroll *scroll = NULL;

    /*Delete the current scroll of image names*/
    uiSpine_DeleteEntity(tGraphicSpine, uiSpine_GetEntityBase(tGraphicSpine, "Scroll:Images"));

    /*Create a new one, with up to date names*/
    scroll = vLE_SetupImageScroll(editor, tGraphicSpine);
    uiSpine_AddScroll(tGraphicSpine, scroll, "Scroll:Images");

    editor->tileEdit.customGraphicName = "None";

    return;
}

void vLE_SetupTileGraphicMenu(Vent_Level_Editor *editor, Ui_Spine *ownerSpine)
{
    int mainLayer = VL_HUD + 3;

    Ui_Spine *tGraphicSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_TextBox *textBox = NULL;

    Vent_Level_TileEditor *tEditor = &editor->tileEdit;

    /*Setup the main window*/
    background = surf_SimpleBox(500, 275, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, mainLayer, 0, "Images:");

    text_Draw_Arg(300 + 10, 15, background, font_Get(2, 16), &tColourBlack, mainLayer, 0, "Custom:");

    /*Add divider line*/
    pixelMan_PushLine(gKer_DrawManager(), 0, 300, 0, 300, background->h, 2, colourBlack, background);

    tGraphicSpine = uiSpine_Create(mainLayer, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(ownerSpine, tGraphicSpine, 0, "Spine:GraphicID");

    /*Add in the close button*/
    uiSpine_AddButton(tGraphicSpine, vLE_CloseButton(&BFA_CloseTileGraphicMenu, dataStruct_Create(4, editor, tGraphicSpine, &editor->graphicID, 1)), "Button:Close");


    /*Add in the main buttons*/
    scroll = uiButtonScroll_Create(15, 235,
                           SCR_H, 10, 5, 0, 0,
                           NULL, NULL, 3,
                           veMenu_ButtonBasic(0, 0, "Select", &tGraphicSpine->sTimer, mainLayer, &BFA_CloseTileGraphicMenu, &MABFH_ButtonHover, dataStruct_Create(4, editor, tGraphicSpine, &editor->graphicID, 1)),
                           veMenu_ButtonBasic(0, 0, "Cancel", &tGraphicSpine->sTimer, mainLayer, &BFA_CloseTileGraphicMenu, &MABFH_ButtonHover, dataStruct_Create(4, editor, tGraphicSpine, &editor->graphicID, 0)),
                           veMenu_ButtonBasic(0, 0, "Remove", &tGraphicSpine->sTimer, mainLayer, &BFA_AddRemoveImage, &MABFH_ButtonHover, dataStruct_Create(4, editor, tGraphicSpine, 0, &tEditor->chosenGraphicID))
                           );

    uiSpine_AddScroll(tGraphicSpine, scroll, "Scroll:Main");

    /*Add in selected ID text*/
    textBox = uiTextBox_CreateBase(190, 237, tGraphicSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &tGraphicSpine->sTimer);

    uiTextBox_AddText(textBox, 0, 0, "ID: %d", dataStruct_Create(1, &tEditor->chosenGraphicID));

    uiSpine_AddTextBox(tGraphicSpine, textBox, "TextBox:SelectedID");

    /*Add in the image finder scroll*/
    scroll = vLE_SetupImageScroll(editor, tGraphicSpine);
    uiSpine_AddScroll(tGraphicSpine, scroll, "Scroll:Images");

    /*Add in the image finder 'add' button*/
    uiSpine_AddButton(tGraphicSpine,
                      veMenu_ButtonBasic(320, 235, "Add", &tGraphicSpine->sTimer, mainLayer, &BFA_AddRemoveImage, &MABFH_ButtonHover, dataStruct_Create(4, editor, tGraphicSpine, 1, &tEditor->customGraphicID)),
                      "Button:Add");

    /*Add ID input*/
    uiSpine_AddTextInput(tGraphicSpine,
                         uiTextInput_Create(380, 237, mainLayer, "ID:", font_Get(2, 13), 4, UI_OUTPUT_INT, &tEditor->customGraphicID, &ve_Menu.c_Activate, tGraphicSpine->pnt, &tGraphicSpine->sTimer),
                         "TextInput:AddID");

    /*Remove button
    uiSpine_AddButton(tGraphicSpine,
                      veMenu_ButtonBasic(0, 0, "Remove", &tGraphicSpine->sTimer, mainLayer, &BFA_AddRemoveImage, &MABFH_ButtonHover, dataStruct_Create(3, editor, tGraphicSpine, 0)),
                      "Button:Remove");*/

    /*Remove ID input
    uiSpine_AddTextInput(tGraphicSpine,
                         uiTextInput_Create(425, 231, mainLayer, "ID:", font_Get(2, 13), 4, UI_OUTPUT_INT, &tEditor->customGraphicID, &ve_Menu.c_Activate, tGraphicSpine->pnt, &tGraphicSpine->sTimer),
                         "TextInput:RemoveID");*/

    /*Chosen image name*/
    uiSpine_AddTextBox(tGraphicSpine,
                       uiTextBox_CreateBase(380, 16, mainLayer, 0, NULL, font_Get(2, 13), tColourBlack, &tGraphicSpine->sTimer),
                       "TextBox:CustomGraphicName");

    uiTextBox_AddText(uiSpine_GetEntity(tGraphicSpine, "TextBox:CustomGraphicName"),
                     0, 0, "%s", dataStruct_Create(1, &tEditor->customGraphicName));

    /*Setup the button control map*/
    //veMenu_SetSpineControl(tGraphicSpine);
    //uiSpine_MapEntity(tGraphicSpine);

    vLE_SetupTileGraphicButtons(editor, tGraphicSpine);

    /*Disable the spine*/
    uiSpine_SetEntityUpdate(ownerSpine, "Spine:GraphicID", 0, 0);

    return;
}
