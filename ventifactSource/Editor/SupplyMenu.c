#include "SupplyMenu.h"

#include "Kernel/Kernel.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "GUI/ButtonFunc.h"
#include "Font.h"

#include "../Menus/MenuAttributes.h"
#include "../ControlMap.h"
#include "../Ai/Units/Ai_UnitMove.h"
#include "LevelEditor.h"

void vLE_UnitEdit_Init(Vent_Level_UnitEditor *uEditor)
{
    int x = 0;

    vSupply_Setup(&uEditor->templateSupply, NULL, NULL, 1, 0, 1, 1, TEAM_PLAYER, UNIT_TANK, 0, 0, 0, SUPPLY_AI_NONE, NULL);

    for(x = 0; x < SUPPLY_AI_TOTAL; x++)
    {
        uEditor->aiData[x] = vSupply_GetDefaultAiData(x);
    }

    uEditor->patrolList = NULL;

    return;
}

void vLE_UnitEdit_Clean(Vent_Level_UnitEditor *uEditor)
{
    int x = 0;

    vSupply_Clean(&uEditor->templateSupply);

    for(x = 0; x < SUPPLY_AI_TOTAL; x++)
    {
        if(uEditor->aiData[x] != NULL)
        {
            dataStruct_Clean(uEditor->aiData[x]);
            mem_Free(uEditor->aiData[x]);
        }
    }

    list_ClearAll(&uEditor->patrolList);

    return;
}

/*Close the unit supply menu*/
static void BFA_CloseUnitMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int closeType = (int)button->info->dataArray[1];

    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;

    switch(closeType)
    {
        default:
        break;

        /*Remove*/
        case VLE_STATE_REMOVE:

        vLE_SetObjectType(editor, VLE_OBJECT_UNIT);

        vLE_SetState(editor->bState, VLE_STATE_REMOVE);

        break;

        /*Place*/
        case VLE_STATE_PLACE:

        vLE_SetObjectType(editor, VLE_OBJECT_UNIT);

        vLE_SetState(editor->bState, VLE_STATE_PLACE);

        break;

        /*Copy*/
        case VLE_STATE_COPY:

        vLE_SetObjectType(editor, VLE_OBJECT_UNIT);

        vLE_SetState(editor->bState, VLE_STATE_COPY);

        break;

        /*Cancel*/
        case VLE_STATE_BASE:

        uiSpine_Close(uiSpine_GetEntity(&editor->spine, "Spine:UnitEdit"));

        break;
    }

    return;
}

static void BFU_UpdateUnitGraphic(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];
    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;

    Frame *frame = NULL;

    frame = sprite_RetrieveFrame(button->graphic, 1);

    frame->graphic = ve_Surfaces.units[uEditor->templateSupply.type][uEditor->templateSupply.team][D_UP];

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

/*Change the ai type*/
static void BFA_ChangeAiType(Ui_Button *button)
{
    Vent_Level_UnitEditor *uEditor = (Vent_Level_UnitEditor *)button->info->dataArray[3];
    Vent_Supply *supply = &uEditor->templateSupply;

    int currentAi = supply->aiState;

    BFunc_MoveInt(button);

    if(currentAi != supply->aiState)
    {
        if(supply->aiData != NULL)
        {
            dataStruct_Delete(&supply->aiData);
        }

        if(uEditor->aiData[supply->aiState] != NULL)
        {
            supply->aiData = dataStruct_Copy(uEditor->aiData[supply->aiState]);
            printf("Supply ai: %p\n", supply->aiData);
        }
    }

    return;
}

/*Open the ai settings menu*/
static void BFA_OpenAiSettings(Ui_Button *button)
{
    Ui_Spine *unitSpine = (Ui_Spine *)button->info->dataArray[0];
    Vent_Level_UnitEditor *uEditor = (Vent_Level_UnitEditor *)button->info->dataArray[1];

    Vent_Supply *supply = &uEditor->templateSupply;
    int x = 0;

    Ui_Spine *aiSpine = uiSpine_GetEntity(unitSpine, "Spine:AiSettings");
    Ui_Button *b = NULL;

    uiSpine_Open(aiSpine, unitSpine);

    switch(uEditor->templateSupply.aiState)
    {
        default:
        uiSpine_OpenTemplate(aiSpine, "None", SELECT_ON);

        return;

        break;

        case SUPPLY_AI_PATROL:

        vLE_ResetPatrolScroll(aiSpine, uEditor);

        b = uiSpine_GetEntity(aiSpine, "Button:PatrolLoopType");
        b->info->dataArray[0] = uEditor->aiData[SUPPLY_AI_PATROL]->dataArray[0];

        uiTextInput_SetOutputVar(uiSpine_GetEntity(aiSpine, "TextInput:PatrolPauseTime"), NULL);

        break;

        case SUPPLY_AI_HOLD:

        uiTextInput_SetOutputVar(uiSpine_GetEntity(aiSpine, "TextInput:HoldXOrigin"), uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[0]);
        uiTextInput_SetOutputVar(uiSpine_GetEntity(aiSpine, "TextInput:HoldYOrigin"), uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[1]);
        uiTextInput_SetOutputVar(uiSpine_GetEntity(aiSpine, "TextInput:HoldRadius"), uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[2]);

        break;
    }

    uiSpine_OpenTemplate2(aiSpine, uEditor->templateSupply.aiState, SELECT_ON);

    return;
}

void vLE_SetupUnitEditMenu(Vent_Level_Editor *editor)
{
    Ui_Spine *unitSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};
    Ui_Button *button = NULL;
    Sprite *sprite = NULL;

    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;
    Vent_Supply *supply = &uEditor->templateSupply;

    /*Setup the main window*/
    background = surf_SimpleBox(300, 240, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, editor->spine.layer + 1, 0, "Unit menu:");

    unitSpine = uiSpine_Create(editor->spine.layer + 1, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(&editor->spine, unitSpine, 0, "Spine:UnitEdit");

    /*Add in the close button*/
    uiSpine_AddButton(unitSpine, vLE_CloseButton(&BFA_CloseUnitMenu, dataStruct_Create(2, editor, VLE_STATE_BASE)), "Button:Close");

    /*Add in the main buttons*/
    uiSpine_AddButton(unitSpine, veMenu_ButtonBasic(0, 0, "Ok", &unitSpine->sTimer, unitSpine->layer, &BFA_CloseUnitMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_BASE)), "Button:Ok");
    uiSpine_AddButton(unitSpine, veMenu_ButtonBasic(0, 0, "Place", &unitSpine->sTimer, unitSpine->layer, &BFA_CloseUnitMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_PLACE)), "Button:Place");
    uiSpine_AddButton(unitSpine, veMenu_ButtonBasic(0, 0, "Copy", &unitSpine->sTimer, unitSpine->layer, &BFA_CloseUnitMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_COPY)), "Button:Copy");
    uiSpine_AddButton(unitSpine, veMenu_ButtonBasic(0, 0, "Remove", &unitSpine->sTimer, unitSpine->layer, &BFA_CloseUnitMenu, &MABFH_ButtonHover, dataStruct_Create(2, editor, VLE_STATE_REMOVE)), "Button:Remove");

    uiSpine_AlignEntities(unitSpine, 15, 210, 10, SCR_H, 4,
                          "Button:Ok",
                          "Button:Place",
                          "Button:Copy",
                          "Button:Remove");

    /*Supply amount*/
    uiSpine_AddTextInput(unitSpine,
                     uiTextInput_Create(20, 100, unitSpine->layer, "Amount      :", font_Get(2, 13), 5, UI_OUTPUT_INT, &supply->respawnAmount, &ve_Menu.c_Activate, unitSpine->pnt, &unitSpine->sTimer),
                     "TextInput:Amount");

    /*Respawn time*/
    uiSpine_AddTextInput(unitSpine,
                     uiTextInput_Create(20, 120, unitSpine->layer, "Respawn time:", font_Get(2, 13), 7, UI_OUTPUT_INT, &supply->supplyTime.end_Time, &ve_Menu.c_Activate, unitSpine->pnt, &unitSpine->sTimer),
                     "TextInput:RespawnTime");

    /*Add in the check box options*/
    scroll = uiButtonScroll_Create(20, 55,
                               SCR_V, 10, 5, 0, 0,
                               NULL, NULL, 2,
                               vLE_ButtonCheckBox("Start Spawned   :", unitSpine->layer, &MABFA_CheckBoxActivate, &MABFU_CheckBoxEnsureActivate, dataStruct_Create(1, &supply->startSpawned)),
                               vLE_ButtonCheckBox("Respawn on death:", unitSpine->layer, &MABFA_CheckBoxActivate, &MABFU_CheckBoxEnsureActivate, dataStruct_Create(1, &supply->deathWait))
                               );

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);
    uiSpine_AddScroll(unitSpine, scroll, "Scroll:UnitCheckbox");


    /*Add in the unit type graphic button*/
    sprite = sprite_Setup(sprite_Create(), 1, unitSpine->layer, &unitSpine->sTimer, 2,
                         frame_CreateBasic(-1, surf_SimpleBox(51, 51, (Uint16P *)ker_colourKey(), &colourBlack, 1), A_FREE),
                         frame_Create(0, NULL, 1, 1, 0, 0, 49, 49, M_FREE)
    );

    button = veMenu_ButtonSprite(195, 50, sprite, &unitSpine->sTimer, NULL, NULL, dataStruct_Create(1, editor));
    uiButton_SetUpdate(button, &BFU_UpdateUnitGraphic, 1);

    uiSpine_AddButton(unitSpine,
                      button,
                      "Button:UnitGraphic");


    sprite = sprite_Copy(&ve_Sprites.scroll[D_LEFT], 1);
    sprite->layer = unitSpine->layer;

    uiSpine_AddButton(unitSpine,
                      veMenu_ButtonSprite(195, 105, sprite, &unitSpine->sTimer, &BFunc_RotateInt, &MABFH_ButtonHover, dataStruct_Create(3, &supply->type, UNIT_ENDTYPE, 1)),
                      "Button:UnitTypeLeft");

    sprite = sprite_Copy(&ve_Sprites.scroll[D_RIGHT], 1);
    sprite->layer = unitSpine->layer;

    uiSpine_AddButton(unitSpine,
                  veMenu_ButtonSprite(230, 105, sprite, &unitSpine->sTimer, &BFunc_RotateInt, &MABFH_ButtonHover, dataStruct_Create(3, &supply->type, UNIT_ENDTYPE, 0)),
                  "Button:UnitTypeRight");

    /*Add in the unit team button*/
    uiSpine_AddButton(unitSpine,
                  vLE_ButtonOption("Team:\n%si", dataStruct_Create(2, &leTeamNames[0], &supply->team), unitSpine->layer, &BFunc_RotateInt, dataStruct_Create(3, &supply->team, 2, D_UP)),
                  "Button:UnitTeam");

    uiSpine_SetEntityPos(unitSpine, "Button:UnitTeam", 195, 130);

    /*Add in the unit ai settings button*/
    uiSpine_AddButton(unitSpine,
                  veMenu_ButtonBasic(20, 180, "Ai settings", &unitSpine->sTimer, unitSpine->layer, BFA_OpenAiSettings, &MABFH_ButtonHover, dataStruct_Create(2, unitSpine, uEditor)),
                  "Button:AiSettings"
    );

    /*Add in the ai state scroll*/
    scrollButton[0] = veMenu_ButtonSprite(0, 20, sprite_Copy(&ve_Sprites.scroll[D_LEFT], 1), &unitSpine->sTimer, &BFA_ChangeAiType, &MABFH_ButtonHover, dataStruct_Create(4, &supply->aiState, 0, D_DOWN, uEditor));
    scrollButton[0]->graphic->layer = unitSpine->layer;

    scrollButton[1] = veMenu_ButtonSprite(20, 20, sprite_Copy(&ve_Sprites.scroll[D_RIGHT], 1), &unitSpine->sTimer, &BFA_ChangeAiType, &MABFH_ButtonHover,  dataStruct_Create(4, &supply->aiState, 6, D_UP, uEditor));
    scrollButton[1]->graphic->layer = unitSpine->layer;

    scroll = uiButtonScroll_Create(0, 0,
                               SCR_V, 10, 5, 0, 0,
                               scrollButton[0], scrollButton[1], 1,
                               vLE_ButtonOption("Ai state: %si", dataStruct_Create(2, &leAiNames[0], &supply->aiState), unitSpine->layer, NULL, NULL)
                               );

    uiButtonScroll_SetHoverState(scroll, &MABFH_ScrollHoverLine);
    uiButtonScroll_SetPosition(scroll, 20, 140);

    uiSpine_AddScroll(unitSpine, scroll, "Scroll:AiState");

    veMenu_SetSpineControl(unitSpine);
    uiSpine_MapEntity(unitSpine);
    uiMap_SetDefaultPath(&unitSpine->map, uiSpine_GetEntityBase(unitSpine, "Button:Ok"));


    /*Add in the ai settings spine*/
    vLE_SetupAiSettingsMenu(editor, unitSpine);

    return;
}

/*Close the ai settings menu*/
static void BFA_CloseAiSettingsMenu(Ui_Button *button)
{
    Ui_Spine *aiSpine = (Ui_Spine *)button->info->dataArray[0];
    Vent_Level_UnitEditor *uEditor = (Vent_Level_UnitEditor *)button->info->dataArray[1];
    Vent_Supply *supply = &uEditor->templateSupply;

    Data_Struct *oldAi = uEditor->aiData[supply->aiState];

    int save = (int)button->info->dataArray[2];

    switch(supply->aiState)
    {
        default:
        break;

        case SUPPLY_AI_PATROL:

        uEditor->aiData[supply->aiState] = vSupply_AiToData_Patrol(*(int *)oldAi->dataArray[0], uEditor->patrolList);

        dataStruct_Delete(&oldAi);

        break;
    }

    if(save == 1)
    {
        if(supply->aiData != NULL)
        {
            dataStruct_Delete(&supply->aiData);
        }

        if(uEditor->aiData[supply->aiState] != NULL)
        {
            supply->aiData = dataStruct_Copy(uEditor->aiData[supply->aiState]);
        }
    }

    uiSpine_Close(aiSpine);

    return;
}

/*Set the state of the level editor to choosing a new position for the hold position ai*/
static void BFA_SetHoldPos(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];
    int *state = (int *)button->info->dataArray[1];

    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;

    B_Node *holdPos = NULL;

    if(*state == 0)
    {
        vLE_SetObjectType(editor, VLE_OBJECT_POSITION);
        editor->maxPositions = 0;

        vLE_SetState(editor->bState, VLE_STATE_PLACE);

        *state = 1;

        /*Make sure the position graph is empty*/
        graph_Reset(&editor->positionGraph);

        /*Call back to this function with a different state*/
        uiButton_TempActive(button, 1);
    }
    else
    {
        *state = 0;

        /*Assign the first position that was chosen to the hold ai origin position*/
        if(editor->positionGraph.nodes != NULL)
        {
            holdPos = editor->positionGraph.nodes->data;

            *(int *)uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[0] = holdPos->x;
            *(int *)uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[1] = holdPos->y;

            graph_Clean(&editor->positionGraph);
        }

    }

    return;
}

/*Set the state of the level editor to choosing new positions for the patrol ai*/
static void BFA_SetPatrolPos(Ui_Button *button)
{
    Vent_Level_Editor *editor = (Vent_Level_Editor *)button->info->dataArray[0];
    Ui_Spine *aiSpine = (Ui_Spine *)button->info->dataArray[1];
    int *addRemove = (int *)button->info->dataArray[2];
    int *state = (int *)button->info->dataArray[3];

    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;

    AiUnitPatrol *aiPatrolPos = NULL;
    B_Node *patrolPos = NULL;
    struct list *pList = NULL;

    if(*state == 0)
    {
        vLE_SetObjectType(editor, VLE_OBJECT_POSITION);
        editor->maxPositions = 100;

        if(*addRemove == 1)
        {
            vLE_SetState(editor->bState, VLE_STATE_PLACE);
        }
        else
        {
            vLE_SetState(editor->bState, VLE_STATE_REMOVE);
        }

        *state = 1;

        /*Make sure the position graph is empty*/
        graph_Reset(&editor->positionGraph);

        /*Copy the patrol points already in the ai state to the position list*/
        while(uEditor->patrolList != NULL)
        {
            aiPatrolPos = uEditor->patrolList->data;

            graph_Add_Node(&editor->positionGraph, node_Create(graph_AssignIndex(&editor->positionGraph), aiPatrolPos->positionX, aiPatrolPos->positionY));

            if(editor->positionGraph.numNodes > 1)
            {
                graph_Add_Edge(&editor->positionGraph, edge_Create_Index(editor->positionGraph.nodes, graph_CurrentIndex(&editor->positionGraph) - 1, graph_CurrentIndex(&editor->positionGraph), 1));
            }

            mem_Free(aiPatrolPos);
            list_Pop(&uEditor->patrolList);
        }

        /*Call back to this function with a different state*/
        uiButton_TempActive(button, 1);
    }
    else
    {
        *state = 0;

        /*Assign the positions that were chosen back to the patrol list*/
        pList = editor->positionGraph.nodes;

        while(pList != NULL)
        {
            patrolPos = pList->data;

            list_Stack(&uEditor->patrolList, aiPatrol_Create(patrolPos->x, patrolPos->y, 0), 0);

            pList = pList->next;
        }

        graph_Reset(&editor->positionGraph);

        vLE_ResetPatrolScroll(aiSpine, uEditor);
    }

    return;
}

static void BFA_SelectPatrolNode(Ui_Button *button)
{
    Ui_TextInput *textInput = (Ui_TextInput *)button->info->dataArray[2];
    AiUnitPatrol *patrol = (AiUnitPatrol *)button->info->dataArray[3];

    if(textInput->state == UI_TEXTINPUT_BASE)
    {
        BFA_ScrollSetHover(button);

        uiTextInput_SetOutputVar(textInput, &patrol->pauseTime);
    }

    return;
}

static void BFA_RemovePatrolNode(Ui_Button *button)
{
    Vent_Level_UnitEditor *uEditor = (Vent_Level_UnitEditor *)button->info->dataArray[0];
    Ui_Spine *aiSpine = (Ui_Spine *)button->info->dataArray[1];
    Ui_ButtonScroll *scroll = uiSpine_GetEntity(aiSpine, "Scroll:PatrolNodes");

    /*Data variables for the button to be removed*/
    AiUnitPatrol *patrol = NULL;
    Ui_TextInput *textInput = NULL;

    /*Remove the button that is currently selected in the patrol scroll*/
    if(scroll->forceHover >= 0)
    {
        /*Also remove the patrol in associated with this button*/
        patrol = (AiUnitPatrol *)scroll->currentForced->info->dataArray[3];

        list_Delete_NodeFromData(&uEditor->patrolList, patrol);
        mem_Free(patrol);

        /*Unlink the connection to the patrol pause time text input box*/
        textInput = (Ui_TextInput *)scroll->currentForced->info->dataArray[2];
        uiTextInput_SetOutputVar(textInput, NULL);

        uiButtonScroll_RemoveButton(scroll, scroll->currentForced);
    }

    return;
}

static Ui_ButtonScroll *vLE_SetupPatrolScroll(Ui_Spine *spine, Vent_Level_UnitEditor *uEditor)
{
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *patrolButton = NULL;

    char buttonText[16];

    struct list *patrolList = uEditor->patrolList;
    AiUnitPatrol *patrol = NULL;

    int x = 0;

    scroll = veMenu_ScrollEmpty(SCR_V, -20, 20, -20, 0, spine->layer, &spine->sTimer);
    scroll->rotate = 0;

    while(patrolList != NULL)
    {
        patrol = patrolList->data;

        patrolButton = vLE_ButtonOption("%d: [%d, %d] %d", dataStruct_CreateType("dppp", x, &patrol->positionX, &patrol->positionY, &patrol->pauseTime), spine->layer, &BFA_SelectPatrolNode, dataStruct_CreateType("pdpp", scroll, 1, uiSpine_GetEntity(spine, "TextInput:PatrolPauseTime"), patrol));

        uiButtonScroll_AddButton(scroll, patrolButton, 0);

        x++;
        patrolList = patrolList->next;
    }

    return scroll;
}

void vLE_ResetPatrolScroll(Ui_Spine *spine, Vent_Level_UnitEditor *uEditor)
{
    uiSpine_DeleteEntity(spine, uiSpine_GetEntityBase(spine, "Scroll:PatrolNodes"));

    uiSpine_AddScroll(spine, vLE_SetupPatrolScroll(spine, uEditor), "Scroll:PatrolNodes");

    uiSpine_SetEntityPos(spine, "Scroll:PatrolNodes", 170, 40);

    uiTextInput_SetOutputVar(uiSpine_GetEntity(spine, "TextInput:PatrolPauseTime"), NULL);

    return;
}

void vLE_SetupAiSettingsMenu(Vent_Level_Editor *editor, Ui_Spine *ownerSpine)
{
    int mainLayer = VL_HUD + 3;

    Ui_Spine *aiSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_TextBox *textBox = NULL;

    Vent_Level_UnitEditor *uEditor = &editor->unitEdit;
    Vent_Supply *supply = &uEditor->templateSupply;

    /*Setup the main window*/
    background = surf_SimpleBox(300, 200, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, mainLayer, 0, "Ai settings:");

    aiSpine = uiSpine_Create(mainLayer, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(ownerSpine, aiSpine, 0, "Spine:AiSettings");

    /*Add in the close button*/
    uiSpine_AddButton(aiSpine, vLE_CloseButton(&BFA_CloseAiSettingsMenu, dataStruct_CreateType("ppd", aiSpine, editor, 1)), "Button:Close");

    /*Add in the main buttons*/
    scroll = uiButtonScroll_Create(15, 165,
                           SCR_H, 10, 5, 0, 0,
                           NULL, NULL, 2,
                           veMenu_ButtonBasic(0, 0, "Ok", &aiSpine->sTimer, mainLayer, &BFA_CloseAiSettingsMenu, &MABFH_ButtonHover, dataStruct_Create(3, aiSpine, uEditor, 1)),
                           veMenu_ButtonBasic(0, 0, "Cancel", &aiSpine->sTimer, mainLayer, &BFA_CloseAiSettingsMenu, &MABFH_ButtonHover, dataStruct_Create(3, aiSpine, uEditor, 0))
                           );

    uiSpine_AddScroll(aiSpine, scroll, "Scroll:Main");

    /*Add in current ai name*/
    textBox = uiTextBox_CreateBase(100, 18, aiSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &aiSpine->sTimer);

    uiTextBox_AddText(textBox, 0, 0, "%si", dataStruct_Create(2, &leAiNames[0], &supply->aiState));

    uiSpine_AddTextBox(aiSpine, textBox, "TextBox:AiName");

    /*Add in no ai settings text*/
    textBox = uiTextBox_CreateBase(20, 60, aiSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &aiSpine->sTimer);

    uiTextBox_AddText(textBox, 0, 0, "No ai settings for %si.", dataStruct_Create(2, &leAiNames[0], &supply->aiState));

    uiSpine_AddTextBox(aiSpine, textBox, "TextBox:NoAiSettings");

    /*Create the template for when no ai options are needed*/
    uiSpine_CreateTemplate(aiSpine, SUPPLY_AI_NONE, "None", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER, 4,
                                    uiSpine_CreateTemplateEntity("Button:Close", UIS_ENTITY_UPDATE, 1),
                                    uiSpine_CreateTemplateEntity("Scroll:Main", UIS_ENTITY_UPDATE, 1),
                                    uiSpine_CreateTemplateEntity("TextBox:AiName", UIS_ENTITY_UPDATE, 1),
                                    uiSpine_CreateTemplateEntity("TextBox:NoAiSettings", UIS_ENTITY_UPDATE, 1)
    );

    /*Add in hold area ai state options*/
    uiSpine_AddTextInput(aiSpine,
                 uiTextInput_Create(25, 40, aiSpine->layer, "x-axis origin:", font_Get(2, 13), 7, UI_OUTPUT_INT, uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[0], &ve_Menu.c_Activate, aiSpine->pnt, &aiSpine->sTimer),
                 "TextInput:HoldXOrigin");

    uiSpine_AddTextInput(aiSpine,
                 uiTextInput_Create(25, 60, aiSpine->layer, "y-axis origin:", font_Get(2, 13), 7, UI_OUTPUT_INT, uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[1], &ve_Menu.c_Activate, aiSpine->pnt, &aiSpine->sTimer),
                 "TextInput:HoldYOrigin");

    uiSpine_AddTextInput(aiSpine,
                 uiTextInput_Create(25, 125, aiSpine->layer, " guard radius:", font_Get(2, 13), 7, UI_OUTPUT_INT, uEditor->aiData[SUPPLY_AI_HOLD]->dataArray[2], &ve_Menu.c_Activate, aiSpine->pnt, &aiSpine->sTimer),
                 "TextInput:HoldRadius");

    uiSpine_AddButton(aiSpine,
                      veMenu_ButtonBasic(25, 80, "Set Position", &aiSpine->sTimer, aiSpine->layer, &BFA_SetHoldPos, &MABFH_ButtonHover, dataStruct_CreateType("pd", editor, 0)),
                      "Button:HoldSetPos");

    uiSpine_CreateTemplate(aiSpine, SUPPLY_AI_HOLD, "Hold ai", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER, 7,
                           uiSpine_CreateTemplateEntity("Button:Close", UIS_ENTITY_UPDATE, 1),
                           uiSpine_CreateTemplateEntity("Scroll:Main", UIS_ENTITY_UPDATE, 1),
                           uiSpine_CreateTemplateEntity("TextBox:AiName", UIS_ENTITY_UPDATE, 1),
                           uiSpine_CreateTemplateEntity("TextInput:HoldXOrigin", UIS_ENTITY_UPDATE, 1),
                           uiSpine_CreateTemplateEntity("TextInput:HoldYOrigin", UIS_ENTITY_UPDATE, 1),
                           uiSpine_CreateTemplateEntity("TextInput:HoldRadius", UIS_ENTITY_UPDATE, 1),
                           uiSpine_CreateTemplateEntity("Button:HoldSetPos", UIS_ENTITY_UPDATE, 1)
    );

    /*Add in the options for the patrol ai state*/
    uiSpine_AddButton(aiSpine,
                      vLE_ButtonCheckBox("Loop endpoints:", aiSpine->layer, &MABFA_CheckBoxActivate, &MABFU_CheckBoxEnsureActivate, dataStruct_Create(1, uEditor->aiData[SUPPLY_AI_PATROL]->dataArray[0])),
                      "Button:PatrolLoopType");

    uiSpine_SetEntityPos(aiSpine, "Button:PatrolLoopType", 117, 80);

    uiSpine_AddTextInput(aiSpine,
             uiTextInput_Create(20, 50, aiSpine->layer, "Pause time:", font_Get(2, 13), 6, UI_OUTPUT_INT, NULL, &ve_Menu.c_Activate, aiSpine->pnt, &aiSpine->sTimer),
             "TextInput:PatrolPauseTime");

    /*uiSpine_AddButton(aiSpine,
            veMenu_ButtonBasic(15, 40, "Remove point", &aiSpine->sTimer, aiSpine->layer, &BFA_RemovePatrolNode, &MABFH_ButtonHover, dataStruct_CreateType("pp", uEditor, aiSpine)),
            "Button:PatrolRemovePoint");*/

    uiSpine_AddScroll(aiSpine,
                      vLE_SetupPatrolScroll(aiSpine, uEditor),
                      "Scroll:PatrolNodes");

    uiSpine_SetEntityPos(aiSpine, "Scroll:PatrolNodes", 170, 40);

    uiSpine_AddButton(aiSpine,
                  veMenu_ButtonBasic(150, 165, "Add", &aiSpine->sTimer, aiSpine->layer, &BFA_SetPatrolPos, &MABFH_ButtonHover, dataStruct_CreateType("ppdd", editor, aiSpine, 1, 0)),
                  "Button:PatrolAddPos");

    uiSpine_AddButton(aiSpine,
              veMenu_ButtonBasic(195, 165, "Remove", &aiSpine->sTimer, aiSpine->layer, &BFA_SetPatrolPos, &MABFH_ButtonHover, dataStruct_CreateType("ppdd", editor, aiSpine, 0, 0)),
              "Button:PatrolRemovePos");

    uiSpine_CreateTemplate(aiSpine, SUPPLY_AI_PATROL, "Patrol ai", UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER, 8,
                       uiSpine_CreateTemplateEntity("Button:Close", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("Scroll:Main", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("TextBox:AiName", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("Button:PatrolLoopType", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("Scroll:PatrolNodes", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("Button:PatrolAddPos", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("Button:PatrolRemovePos", UIS_ENTITY_UPDATE, 1),
                       uiSpine_CreateTemplateEntity("TextInput:PatrolPauseTime", UIS_ENTITY_UPDATE, 1)
    );

    uiSpine_OpenTemplate(aiSpine, "None", SELECT_ON);


    return;
}
