#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "BaseState.h"
#include "GUI/ButtonScroll.h"

Ui_ButtonScroll *MM_CreateLevelScroll(Base_State *gState);

void MM_RepopulateLevels(Base_State *gState);

void Menu_Game_Init(void *info);

void Menu_Game_Activate(void *info);

void Menu_Game_Controls(void *info);

void Menu_Game_Logic(void *info);

void Menu_Game_Render(void *info);

void Menu_Game_Exit(void *info);

#endif
