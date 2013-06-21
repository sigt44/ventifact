#ifndef MENUATTRIBUTES_H
#define MENUATTRIBUTES_H

#include <SDL/SDL.h>

#include "Controls.h"
#include "Graphics/Pixel.h"
#include "Vector2DInt.h"
#include "GUI/Spine.h"
#include "GUI/Button.h"
#include "GUI/Pointer.h"
#include "Font.h"

struct ve_MenuAttributes
{
    int textIndentX; /*Indent amount for the text that is placed in the main menu buttons*/
    int textIndentY;
    int textSize; /*Text size for the main menu buttons*/
    int buttonWidth;
    int buttonHeight;
    int buttonSpaceing;

    Vector2DInt menuPosition; /*The position of the menus background*/
    Vector2DInt menuSecondPosition; /*The position of the second menu*/
    Vector2DInt buttonPosition; /*The starting position for the first of the 4 menu buttons, relative to the main menu background*/

    TTF_Font *mainFont[3]; /*The main font to use, Small Medium and Large*/

    SDL_Color subTextColour; /*Colour of the sub text in the menus*/

    Uint16P colourBackgroundBasic; /*Basic colour of a background*/

    Ui_Pointer pointer; /*Main mouse pointer*/

    /*Controls to navigate the menu*/
    Control_Event c_MoveRight;
    Control_Event c_MoveLeft;
    Control_Event c_MoveUp;
    Control_Event c_MoveDown;

    Control_Event c_Activate;
    Control_Event c_PointerActivate;
    Control_Event c_Back;

};

extern struct ve_MenuAttributes ve_Menu;

extern char *boolText[2];

void menuAttributes_Setup(void);

void menuAttributes_ResetControls(void);

void menuAttributes_Clean(void);

void veMenu_SetMapControl(Ui_Map *map, int all);
void veMenu_SetSpineControl(Ui_Spine *spine);

Ui_Button *veMenu_ButtonSide(int x, int y, char *buttonText, Timer *srcTimer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);

Ui_Button *veMenu_ButtonOption(char *optionText, Data_Struct *textVars, TTF_Font *font, Timer *srcTimer, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);

Ui_Button *veMenu_ButtonBasic(int x, int y, char *text, Timer *srcTimer, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);

Ui_Button *veMenu_ButtonText(int x, int y, int w, int h, TTF_Font *font, char *text, Timer *srcTimer, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);

Ui_Button *veMenu_ButtonCheckBox(char *text, TTF_Font *font, int x, int y, int layer, Timer *srcTime, void (*onActivate)(Ui_Button *button), void (*onUpdate)(Ui_Button *button), Data_Struct *info);

Ui_Button *veMenu_ButtonSprite(int x, int y, Sprite *buttonSprite, Timer *srcTimer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);

Ui_Button *veMenu_ButtonEmpty(Timer *srcTimer);

Ui_Button *veMenu_ButtonUnit(int unitType, int layer, Timer *srcTimer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);

SDL_Surface *veMenu_SurfaceHover(int buttonWidth, int buttonHeight, int barWidth, int barTotal, int barHeightReduce, Uint16P *colour, SDL_Surface *dest);

SDL_Surface *veMenu_SurfaceHoverShade(int buttonWidth, int buttonHeight, Uint16P *shadeColour, int shadeAmount, SDL_Surface *dest);

Ui_Spine *veMenu_WindowOk(Ui_Spine *baseWindow, char *name, char *text, int x, int y, int width, int height, int layer);

Ui_Spine *veMenu_WindowBool(Ui_Spine *baseWindow, char *name, char *text, Data_Struct *textVars, int x, int y, Ui_Button *buttonCall, int *boolChoice);

Ui_ButtonScroll *veMenu_ScrollFile(char *DIRName, char *extension, struct list **nameList, int x, int y, int layer, Timer *srcTimer);

Ui_ButtonScroll *veMenu_ScrollEmpty(int direction, int aX, int aY, int bX, int bY, int layer, Timer *srcTimer);

void MABFH_ButtonHover(Ui_Button *button);

void MABFA_Quit(Ui_Button *button);

void MABFA_ExitWindow(Ui_Button *button);
void MABFA_ExitBoolWindow(Ui_Button *button);

void MABFA_CheckBoxActivate(Ui_Button *button);
void MABFU_CheckBoxEnsureActivate(Ui_Button *button);
void MABFA_CheckBoxToggleFlag(Ui_Button *button);
void MABFU_CheckBoxEnsureFlag(Ui_Button *button);

void MABFH_HoverLine(Ui_Button *button);
void MABFH_HoverLineThin(Ui_Button *button);

void MABFH_ScrollHoverLine(Ui_Button *button);

#endif
