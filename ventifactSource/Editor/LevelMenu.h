#ifndef LEVELMENU_H
#define LEVELMENU_H

#include "LevelEditor.h"

void vLE_LevelEdit_Init(Vent_Level_AttributesEditor *lEditor);

void vLE_LevelEdit_Clean(Vent_Level_AttributesEditor *lEditor);

void vLE_SetupLevelEditMenu(Vent_Level_Editor *editor);

void vLE_ResetLevelFileScroll(Vent_Level_Editor *editor, Ui_Spine *loadSpine);
Ui_ButtonScroll *vLE_SetupLevelFileScroll(Vent_Level_Editor *editor, Ui_Spine *spine);

void vLE_SetupLevelLoadMenu(Vent_Level_Editor *editor, Ui_Spine *baseSpine);

#endif
