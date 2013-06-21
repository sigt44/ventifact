#ifndef SUPPLYMENU_H
#define SUPPLYMENU_H

#include "LevelEditor.h"

void vLE_UnitEdit_Init(Vent_Level_UnitEditor *uEditor);
void vLE_UnitEdit_Clean(Vent_Level_UnitEditor *uEditor);

void vLE_SetupUnitEditMenu(Vent_Level_Editor *editor);

void vLE_ResetPatrolScroll(Ui_Spine *spine, Vent_Level_UnitEditor *uEditor);
void vLE_SetupAiSettingsMenu(Vent_Level_Editor *editor, Ui_Spine *ownerSpine);

#endif
