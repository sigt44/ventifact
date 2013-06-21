#ifndef NODEMENU_H
#define NODEMENU_H

#include "LevelEditor.h"

void vLE_NodeEdit_Init(Vent_Level_NodeEditor *nEditor, B_Graph *levelGraph);

void vLE_NodeEdit_AddRemove_Edge(Vent_Level_NodeEditor *nEditor, B_Node *selectedNode, int add);

void vLE_NodeEdit_Clean(Vent_Level_NodeEditor *nEditor);

void vLE_SetupNodeEditMenu(Vent_Level_Editor *editor);

#endif
