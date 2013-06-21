#include "NodeMenu.h"

#include "LevelEditor.h"
#include "../Menus/MenuAttributes.h"


void vLE_NodeEdit_Init(Vent_Level_NodeEditor *nEditor, B_Graph *levelGraph)
{
    nEditor->showGlobalNodes = 0;
    nEditor->linkPreviousNode = 0;

    nEditor->selectedNodeA = NULL;
    nEditor->selectedNodeB = NULL;

    nEditor->levelGraph = levelGraph;

    return;
}

void vLE_NodeEdit_AddRemove_Edge(Vent_Level_NodeEditor *nEditor, B_Node *selectedNode, int add)
{
    if(selectedNode == nEditor->selectedNodeA)
    {
        nEditor->selectedNodeA = NULL;
        return;
    }
    else if(selectedNode == nEditor->selectedNodeB)
    {
        nEditor->selectedNodeB = NULL;
        return;
    }

    if(nEditor->selectedNodeA == NULL && nEditor->selectedNodeB == NULL)
    {
        nEditor->selectedNodeA = selectedNode;
        return;
    }
    else if(nEditor->selectedNodeB == NULL)
    {
        nEditor->selectedNodeB = selectedNode;
    }
    else
    {
        nEditor->selectedNodeA = nEditor->selectedNodeB;
        nEditor->selectedNodeB = selectedNode;
    }

    if(add == 1)
    {
        graph_Add_Edge(nEditor->levelGraph,  edge_Create_Index(nEditor->levelGraph->nodes, nEditor->selectedNodeA->index, nEditor->selectedNodeB->index, 1));
    }
    else if(add == 0)
    {
        graph_Remove_EdgesByNodes(nEditor->levelGraph, nEditor->selectedNodeA, nEditor->selectedNodeB);
    }

    return;
}


void vLE_NodeEdit_Clean(Vent_Level_NodeEditor *nEditor)
{
    nEditor->selectedNodeA = NULL;
    nEditor->selectedNodeB = NULL;

    nEditor->levelGraph = NULL;

    return;
}

/*Close the node menu*/
static void BFA_CloseNodeMenu(Ui_Button *button)
{
    Vent_Level_Editor *editor = button->info->dataArray[0];
    int *closeType = (int *)button->info->dataArray[1];
    int *objectType = (int *)button->info->dataArray[2];

    Vent_Level_NodeEditor *nEditor = &editor->nodeEdit;

    switch(*closeType)
    {
        default:
        break;

        /*Remove*/
        case VLE_STATE_REMOVE:

        vLE_SetObjectType(editor, *objectType);

        vLE_SetState(editor->bState, VLE_STATE_REMOVE);

        break;

        /*Place*/
        case VLE_STATE_PLACE:

        vLE_SetObjectType(editor, *objectType);

        vLE_SetState(editor->bState, VLE_STATE_PLACE);

        break;

        /*Cancel*/
        case VLE_STATE_BASE:

        uiSpine_Close(uiSpine_GetEntity(&editor->spine, "Spine:NodeEdit"));

        break;
    }

    return;
}

static void BFA_ClearNodes(Ui_Button *button)
{
    Ui_Spine *nodeSpine = button->info->dataArray[0];
    Vent_Level_NodeEditor *nEditor = button->info->dataArray[1];
    int *state = (int *)button->info->dataArray[2];

    if(*state == -1)
    {
        veMenu_WindowBool(nodeSpine, "Spine:ConfirmClearNodes",
                          "Are you sure you want to remove all nodes?", NULL, ker_Screen_Width()/2, ker_Screen_Height()/2, button, state);
    }
    else if(*state == 0)
    {
    }
    else if(*state == 1)
    {
        graph_Clear_Nodes(nEditor->levelGraph);
    }

    return;
}

static void BFA_ClearEdges(Ui_Button *button)
{
    Ui_Spine *nodeSpine = button->info->dataArray[0];
    Vent_Level_NodeEditor *nEditor = button->info->dataArray[1];
    int *state = (int *)button->info->dataArray[2];

    if(*state == -1)
    {
        veMenu_WindowBool(nodeSpine, "Spine:ConfirmClearEdges",
                          "Are you sure you want to remove all edges?", NULL, ker_Screen_Width()/2, ker_Screen_Height()/2, button, state);
    }
    else if(*state == 0)
    {
    }
    else if(*state == 1)
    {
        graph_Clear_Edges(nEditor->levelGraph);
    }

    return;
}

void vLE_SetupNodeEditMenu(Vent_Level_Editor *editor)
{
    Ui_Spine *nodeSpine = NULL;
    SDL_Surface *background = NULL;
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *scrollButton[2] = {NULL, NULL};
    Ui_Button *button = NULL;
    Sprite *sprite = NULL;

    Vent_Level_NodeEditor *nEditor = &editor->nodeEdit;

    /*Setup the main window*/
    background = surf_SimpleBox(300, 240, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, editor->spine.layer + 1, 0, "Node menu:");

    nodeSpine = uiSpine_Create(editor->spine.layer + 1, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(&editor->spine, nodeSpine, 0, "Spine:NodeEdit");

    /*Add in the edge buttons*/
    text_Draw_Arg(20, 80, background, font_Get(2, 13), &tColourBlack, editor->spine.layer + 1, 0, "Edges:");

    scroll = uiButtonScroll_Create(20, 100,
                           SCR_H, 10, 5, 0, 0,
                           NULL, NULL, 3,
                           veMenu_ButtonBasic(0, 0, "Add", &nodeSpine->sTimer, nodeSpine->layer, &BFA_CloseNodeMenu, &MABFH_ButtonHover, dataStruct_CreateType("pdd", editor, VLE_STATE_PLACE, VLE_OBJECT_EDGE)),
                           veMenu_ButtonBasic(0, 0, "Remove", &nodeSpine->sTimer, nodeSpine->layer, &BFA_CloseNodeMenu, &MABFH_ButtonHover, dataStruct_CreateType("pdd", editor, VLE_STATE_REMOVE, VLE_OBJECT_EDGE)),
                           veMenu_ButtonBasic(0, 0, "Remove(All)", &nodeSpine->sTimer, nodeSpine->layer, &BFA_ClearEdges, &MABFH_ButtonHover, dataStruct_CreateType("ppd", nodeSpine, nEditor, -1))
                           );

    uiButtonScroll_SetHoverState(scroll, MABFH_HoverLine);
    uiSpine_AddScroll(nodeSpine, scroll, "Scroll:Edge");

    /*Add in the node buttons*/
    text_Draw_Arg(20, 140, background, font_Get(2, 13), &tColourBlack, editor->spine.layer + 1, 0, "Nodes:");

    scroll = uiButtonScroll_Create(20, 160,
                           SCR_H, 10, 5, 0, 0,
                           NULL, NULL, 3,
                           veMenu_ButtonBasic(0, 0, "Add", &nodeSpine->sTimer, nodeSpine->layer, &BFA_CloseNodeMenu, &MABFH_ButtonHover, dataStruct_CreateType("pdd", editor, VLE_STATE_PLACE, VLE_OBJECT_NODE)),
                           veMenu_ButtonBasic(0, 0, "Remove", &nodeSpine->sTimer, nodeSpine->layer, &BFA_CloseNodeMenu, &MABFH_ButtonHover, dataStruct_CreateType("pdd", editor, VLE_STATE_REMOVE, VLE_OBJECT_NODE)),
                           veMenu_ButtonBasic(0, 0, "Remove(All)", &nodeSpine->sTimer, nodeSpine->layer, &BFA_ClearNodes, &MABFH_ButtonHover, dataStruct_CreateType("ppd", nodeSpine, nEditor, -1))
                           );

    uiButtonScroll_SetHoverState(scroll, MABFH_HoverLine);
    uiSpine_AddScroll(nodeSpine, scroll, "Scroll:Node");

    /*Add in the global node viewing checkbox*/
    uiSpine_AddButton(nodeSpine,
                      vLE_ButtonCheckBox("Always view graph:", nodeSpine->layer, &MABFA_CheckBoxActivate, &MABFU_CheckBoxEnsureActivate, dataStruct_Create(1, &nEditor->showGlobalNodes)),
                      "Button:GlobalGraph");

    uiSpine_SetEntityPos(nodeSpine, "Button:GlobalGraph", 140, 50);

    /*Add in the close button*/
    uiSpine_AddButton(nodeSpine, vLE_CloseButton(&BFA_CloseNodeMenu, dataStruct_CreateType("pdd", editor, VLE_STATE_BASE, VLE_OBJECT_NODE)), "Button:Close");

    /*Add in the main buttons*/
    uiSpine_AddButton(nodeSpine,
                      veMenu_ButtonBasic(20, 210, "Ok", &nodeSpine->sTimer, nodeSpine->layer, &BFA_CloseNodeMenu, &MABFH_ButtonHover, dataStruct_CreateType("pdd", editor, VLE_STATE_BASE, VLE_OBJECT_NODE)),
                      "Button:Ok");

    /*Setup button control mapping*/
    veMenu_SetSpineControl(nodeSpine);
    uiSpine_MapEntity(nodeSpine);

    uiMap_SetDefaultPath(&nodeSpine->map, uiSpine_GetEntityBase(nodeSpine, "Button:Ok"));

    return;
}
