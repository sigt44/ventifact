#ifndef TILEMENU_H
#define TILEMENU_H

#include "LevelEditor.h"

void vLE_TileEdit_Init(Vent_Level_TileEditor *tEditor);
void vLE_TileEdit_Clean(Vent_Level_TileEditor *tEditor);

void vLE_CopyTile(int cleanSet, Vent_Tile *tileSet, Vent_Tile *tileCopy);

void vLE_SetupTileEditMenu(Vent_Level_Editor *editor);

void vLE_SetupTileGraphicButtons(Vent_Level_Editor *editor, Ui_Spine *tGraphicSpine);

void vLE_SetupTileGraphicMenu(Vent_Level_Editor *editor, Ui_Spine *ownerSpine);

void vLE_ResetTileImageScroll(Vent_Level_Editor *editor, Ui_Spine *tGraphicSpine);

void vLE_ResetTileGraphicButtons(Vent_Level_Editor *editor, Ui_Spine *tGraphicSpine);

#endif
