#include "MenuAttributes.h"

#include "Graphics/Graphics.h"
#include "Kernel/Kernel.h"
#include "Controls.h"
#include "GUI/Button.h"
#include "GUI/Spine.h"
#include "GUI/ButtonScroll.h"
#include "GUI/ButtonPaths.h"
#include "GUI/UI_Defines.h"
#include "GUI/ButtonFunc.h"
#include "Time/Timer.h"
#include "Font.h"

#include "../Images.h"
#include "../Sounds.h"
#include "../Defines.h"
#include "../ControlMap.h"

struct ve_MenuAttributes ve_Menu;

char *boolText[2] = {"Off", "On"};

/*
    Function: menuAttributes_Setup

    Description -
    Used to set the default values to help build the main menus of the game.

    0 arguments:
*/
void menuAttributes_Setup(void)
{
    Sprite *curserSprite = NULL;

    control_Setup(&ve_Menu.c_MoveRight, "Right", NULL, 250);
    control_Setup(&ve_Menu.c_MoveLeft, "Left", NULL, 250);
    control_Setup(&ve_Menu.c_MoveUp, "Up", NULL, 250);
    control_Setup(&ve_Menu.c_MoveDown, "Down", NULL, 250);
    control_Setup(&ve_Menu.c_Activate, "Activate", NULL, 250);
    control_Setup(&ve_Menu.c_PointerActivate, "PointerActivate", NULL, 500);
    control_Setup(&ve_Menu.c_Back, "Back", NULL, 250);

    control_CopyKey(&ve_Menu.c_Back, &ve_Controls.cBack);
    control_CopyKey(&ve_Menu.c_Activate, &ve_Controls.cConfirm);

    control_CopyKey(&ve_Menu.c_PointerActivate, &ve_Controls.cPointerConfirm);

    control_CopyKey(&ve_Menu.c_MoveLeft, &ve_Controls.cLeft);
    control_CopyKey(&ve_Menu.c_MoveRight, &ve_Controls.cRight);
    control_CopyKey(&ve_Menu.c_MoveUp, &ve_Controls.cUp);
    control_CopyKey(&ve_Menu.c_MoveDown, &ve_Controls.cDown);


    font_Load("Arial.ttf", 11, 2);
    font_Load("Arial.ttf", 13, 2);
    font_Load("Arial.ttf", 16, 2);
    font_Load("Arial.ttf", 22, 2);

    ve_Menu.mainFont[FS_SMALL] = font_Get(2, 11);
    ve_Menu.mainFont[FS_MEDIUM] = font_Get(2, 13);
    ve_Menu.mainFont[FS_LARGE] = font_Get(2, 22);

    ve_Menu.subTextColour.r = 40;
    ve_Menu.subTextColour.g = 40;
    ve_Menu.subTextColour.b = 40;

    ve_Menu.colourBackgroundBasic = (Uint16P)SDL_MapRGB(ker_Screen()->format, 140, 150, 140);

    curserSprite = sprite_Setup(sprite_Create(), 1, VL_HUD + 100, NULL, 2,
                                frame_CreateBasic(0, ve_Surfaces.curser, M_FREE),
                                frame_CreateBasic(0, ve_Surfaces.editorCurser, M_FREE)
    );

    uiPointer_Setup(&ve_Menu.pointer, curserSprite, VL_HUD + 100);

    uiPointer_SetControls(&ve_Menu.pointer, 175.0f,
                                &ve_Controls.cMoveUp,
                                &ve_Controls.cMoveDown,
                                &ve_Controls.cMoveLeft,
                                &ve_Controls.cMoveRight);

    uiPointer_SetActivate(&ve_Menu.pointer, 250, &ve_Menu.c_PointerActivate);

    /*These should perhaps be loaded from a file*/
    #if defined (LINUX) || defined (WINDOWS)
    ve_Menu.textIndentX = 20;
    ve_Menu.textIndentY = 12;
    ve_Menu.textSize = FS_LARGE;

    ve_Menu.menuPosition.x = (ker_Screen_Width()/2) - (ve_Surfaces.mainMenu->w/2);
    ve_Menu.menuPosition.y = (ker_Screen_Height()/2) - (ve_Surfaces.mainMenu->h/2);

    /*Relative to menu position*/
    ve_Menu.buttonPosition.x = 23;
    ve_Menu.buttonPosition.y = 102;
    ve_Menu.buttonWidth = 158;
    ve_Menu.buttonHeight = 50;
    ve_Menu.buttonSpaceing = 26;

    ve_Menu.menuSecondPosition.x = 240;
    ve_Menu.menuSecondPosition.y = 145;
    #endif

    #ifdef PANDORA
    ve_Menu.textIndentX = 20;
    ve_Menu.textIndentY = 12;
    ve_Menu.textSize = FS_LARGE;

    ve_Menu.menuPosition.x = (ker_Screen_Width()/2) - (ve_Surfaces.mainMenu->w/2);
    ve_Menu.menuPosition.y = (ker_Screen_Height()/2) - (ve_Surfaces.mainMenu->h/2);

    ve_Menu.buttonPosition.x = 22;
    ve_Menu.buttonPosition.y = 101;
    ve_Menu.buttonWidth = 160;
    ve_Menu.buttonHeight = 52;
    ve_Menu.buttonSpaceing = 24;

    ve_Menu.menuSecondPosition.x = 240;
    ve_Menu.menuSecondPosition.y = 145;
    #endif

    #ifdef DINGOO
    ve_Menu.textIndentX = 20;
    ve_Menu.textIndentY = 11;
    ve_Menu.textSize = FS_SMALL;

    ve_Menu.menuPosition.x = (ker_Screen_Width()/2) - (ve_Surfaces.mainMenu->w/2);
    ve_Menu.menuPosition.y = (ker_Screen_Height()/2) - (ve_Surfaces.mainMenu->h/2);

    ve_Menu.buttonPosition.x = 21;
    ve_Menu.buttonPosition.y = 50;
    ve_Menu.buttonWidth = 50;
    ve_Menu.buttonHeight = 25;
    ve_Menu.buttonSpaceing = 20;

    ve_Menu.menuSecondPosition.x = 240;
    ve_Menu.menuSecondPosition.y = 145;

    control_AddKey(&ve_Menu.c_Back, C_KEYBOARD, 1, DINKEY_Y);

    control_AddKey(&ve_Menu.c_Activate, C_KEYBOARD, 1, DINKEY_A);

    control_AddKey(&ve_Menu.c_MoveLeft, C_KEYBOARD, 1, DINKEY_LEFT);
    control_AddKey(&ve_Menu.c_MoveRight, C_KEYBOARD, 1, DINKEY_RIGHT);
    control_AddKey(&ve_Menu.c_MoveUp, C_KEYBOARD, 1, DINKEY_UP);
    control_AddKey(&ve_Menu.c_MoveDown, C_KEYBOARD, 1, DINKEY_DOWN);
    #endif

    return;
}

void menuAttributes_ResetControls(void)
{
    control_Clear(&ve_Menu.c_Back);
    control_Clear(&ve_Menu.c_Activate);
    control_Clear(&ve_Menu.c_PointerActivate);

    control_Clear(&ve_Menu.c_MoveLeft);
    control_Clear(&ve_Menu.c_MoveRight);
    control_Clear(&ve_Menu.c_MoveUp);
    control_Clear(&ve_Menu.c_MoveDown);

    control_CopyKey(&ve_Menu.c_Back, &ve_Controls.cBack);
    control_CopyKey(&ve_Menu.c_Activate, &ve_Controls.cConfirm);
    control_CopyKey(&ve_Menu.c_PointerActivate, &ve_Controls.cPointerConfirm);

    control_CopyKey(&ve_Menu.c_MoveLeft, &ve_Controls.cLeft);
    control_CopyKey(&ve_Menu.c_MoveRight, &ve_Controls.cRight);
    control_CopyKey(&ve_Menu.c_MoveUp, &ve_Controls.cUp);
    control_CopyKey(&ve_Menu.c_MoveDown, &ve_Controls.cDown);

    return;
}


void menuAttributes_Clean(void)
{
    uiPointer_Clean(&ve_Menu.pointer);


    control_Clean(&ve_Menu.c_MoveLeft);
    control_Clean(&ve_Menu.c_MoveRight);
    control_Clean(&ve_Menu.c_MoveUp);
    control_Clean(&ve_Menu.c_MoveDown);
    control_Clean(&ve_Menu.c_Back);
    control_Clean(&ve_Menu.c_Activate);
    control_Clean(&ve_Menu.c_PointerActivate);

    return;
}

void veMenu_SetMapControl(Ui_Map *map, int all)
{
    uiMap_SetControl(map, UI_UP, &ve_Menu.c_MoveUp);
    uiMap_SetControl(map, UI_DOWN, &ve_Menu.c_MoveDown);
    uiMap_SetControl(map, UI_LEFT, &ve_Menu.c_MoveLeft);
    uiMap_SetControl(map, UI_RIGHT, &ve_Menu.c_MoveRight);

    uiMap_SetControl(map, UI_UPLEFT, &ve_Controls.cUpLeft);
    uiMap_SetControl(map, UI_UPRIGHT, &ve_Controls.cUpRight);
    uiMap_SetControl(map, UI_DOWNRIGHT, &ve_Controls.cDownRight);
    uiMap_SetControl(map, UI_DOWNLEFT, &ve_Controls.cDownLeft);

    if(all == 1)
    {
        uiMap_SetControl(map, UI_ENTER, &ve_Menu.c_Activate);
        uiMap_SetControl(map, UI_EXIT, &ve_Menu.c_Back);
    }

    return;
}
void veMenu_SetSpineControl(Ui_Spine *spine)
{
    uiMap_SetControl(&spine->map, UI_UP, &ve_Menu.c_MoveUp);
    uiMap_SetControl(&spine->map, UI_DOWN, &ve_Menu.c_MoveDown);
    uiMap_SetControl(&spine->map, UI_LEFT, &ve_Menu.c_MoveLeft);
    uiMap_SetControl(&spine->map, UI_RIGHT, &ve_Menu.c_MoveRight);
    uiMap_SetControl(&spine->map, UI_ENTER, &ve_Menu.c_Activate);
    uiMap_SetControl(&spine->map, UI_EXIT, &ve_Menu.c_Back);

    uiMap_SetControl(&spine->map, UI_UPLEFT, &ve_Controls.cUpLeft);
    uiMap_SetControl(&spine->map, UI_UPRIGHT, &ve_Controls.cUpRight);
    uiMap_SetControl(&spine->map, UI_DOWNRIGHT, &ve_Controls.cDownRight);
    uiMap_SetControl(&spine->map, UI_DOWNLEFT, &ve_Controls.cDownLeft);

    return;
}

/*
    Function: veMenu_ButtonSide

    Description -
    Creates a button that is ment for the side menu.

    7 arguments:
    int x - x location of the button.
    int y - y location of the button.
    char *buttonText - The text that appears on the button.
    Timer *srcTimer - Which timer the button with use.
    void (*onActivate)(Ui_Button *button) - function to perform when the button is activated.
    void (*onHover)(Ui_Button *button) - function to perform when the button is selected.
    Ui_Info *info - Structure containing any values that the above functions need.
*/
Ui_Button *veMenu_ButtonSide(int x, int y, char *buttonText, Timer *srcTimer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Sprite *buttonSprite = sprite_Setup(sprite_Create(), 1, VL_HUD + 1, srcTimer, 3,
                 frame_CreateBasic(-1, surf_Offset(ve_Menu.textIndentX, ve_Menu.textIndentY, text_Arg(ve_Menu.mainFont[FS_LARGE], &tColourBlack, buttonText), 1), A_FREE),
                 frame_CreateBasic(0, NULL, M_FREE),
                 frame_CreateBasic(25, veMenu_SurfaceHover(ve_Menu.buttonWidth, ve_Menu.buttonHeight, 4, 1, 2, &colourBlack, NULL), A_FREE)
                 );

    Ui_Button *newButton = uiButton_Create(x, y,
                                           ve_Menu.buttonWidth,
                                           ve_Menu.buttonHeight,
                                           onActivate, onHover,
                                           info, buttonSprite,
                                           0,
                                           250,
                                           srcTimer
                                          );

    /*Copy the activate button from the default menu*/
    uiButton_CopyControl(newButton, &ve_Menu.c_Activate, BC_ALL);

    uiButton_SetSound(newButton, ve_Sounds.menuButtonSelect, ve_Sounds.menuButtonChange);

    return newButton;
}

/*
    Function: veMenu_ButtonBasic

    Description -
    Returns a basic boxed button.

    8 arguments:
    int x - x axis location of button.
    int y - y axis location of button.
    char *text - The text of the button.
    Timer *srcTimer - Timer for the button.
    int layer - Layer of the button.
    void (*onActivate)(Ui_Button *button) - Function to call once button is activated.
    void (*onHover)(Ui_Button *button) - Function to call once button is on hover state.
    Data_Struct *info - Data struct holding variable to change + other info needed, depending on the activate function.
*/
Ui_Button *veMenu_ButtonBasic(int x, int y, char *text, Timer *srcTimer, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Ui_Button *button = NULL;

    button = veMenu_ButtonText(x, y, -1, -1, ve_Menu.mainFont[FS_MEDIUM], text, srcTimer, layer, onActivate, onHover, info);

    return button;
}

/*
    Function: veMenu_ButtonText

    Description -
    Returns a basic boxed button using a specific font and box dimension.

    11 arguments:
    int x - x axis location of button.
    int y - y axis location of button.
    int w - width of the button, if <=0 then will use the text to decide.
    int w - height of the button, if <=0 then will use the text to decide.
    Font *font - The font of the button.
    char *text - The text of the button.
    Timer *srcTimer - Timer for the button.
    int layer - Layer of the button.
    void (*onActivate)(Ui_Button *button) - Function to call once button is activated.
    void (*onHover)(Ui_Button *button) - Function to call once button is on hover state.
    Data_Struct *info - Data struct holding variable to change + other info needed, depending on the activate function.
*/
Ui_Button *veMenu_ButtonText(int x, int y, int w, int h, TTF_Font *font, char *text, Timer *srcTimer, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Sprite *buttonSprite = NULL;
    Ui_Button *button = NULL;

    SDL_Surface *textSurface = text_Arg(font, &tColourBlack, text);
    SDL_Surface *boxSurface = NULL;

    if(w <= 0)
        w = textSurface->w + 8;
    if(h <= 0)
        h = textSurface->h + 4;

    boxSurface = surf_SimpleBox(w, h, (Uint16P *)ker_colourKey(), &colourBlack, 1);

    surf_Blit_Free((boxSurface->w/2) - (textSurface->w/2), (boxSurface->h/2) - (textSurface->h/2), textSurface, boxSurface, NULL);

    buttonSprite = sprite_Setup(sprite_Create(),  1, layer, srcTimer, 3,
                              frame_CreateBasic(-1, boxSurface, A_FREE),
                              frame_CreateBasic(0, NULL, M_FREE), /*frame for no hover state*/
                              frame_CreateBasic(25, veMenu_SurfaceHover(boxSurface->w, boxSurface->h, 2, 1, 2, &colourBlack, NULL), A_FREE)
    );

    button = uiButton_Create(x, y, w, h, onActivate, onHover, info, buttonSprite, 0, 250, srcTimer);

    uiButton_CopyControl(button, &ve_Menu.c_Activate, BC_ALL);

    return button;
}

/*
    Function: veMenu_ButtonOption

    Description -
    Returns a button that can be used to change a variable.

    8 arguments:
    char *optionText - The text of the option, formated similar to printf.
    Data_Struct *textVars - Holds the variables to display as text.
    TTF_Font *font - The font of the text.
    Timer *srcTimer - Timer for the button.
    int layer - Layer of the button.
    void (*onActivate)(Ui_Button *button) - Function to call once button is activated.
    void (*onHover)(Ui_Button *button) - Function to call once button is on hover state.
    Data_Struct *info - Structure containing any values that the above functions need.
*/
Ui_Button *veMenu_ButtonOption(char *optionText, Data_Struct *textVars, TTF_Font *font, Timer *srcTimer, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Sprite *buttonSprite = NULL;
    Ui_Button *button = NULL;

    Ui_TextBox *text = NULL;

    char *highlightText = NULL;

    /*Point to the character in the options text that is before a variable sign*/
    char *upToVariable = NULL;

    int height = 0;
    int width = 0;
	unsigned int textLength = sizeof(char) * (strlen(optionText) + 1);

	highlightText = (char *)mem_Malloc(textLength, __LINE__, __FILE__);

	upToVariable = highlightText;

    text = uiTextBox_CreateBase(0, 0, layer, 0, NULL, font, tColourBlack, srcTimer);
    uiTextBox_AddText(text,
                     0, 0, optionText, textVars);

    /*Find the variable sign '%' and cut the text to that point, this
    text will be highlighted when the button is hovered*/
    strcpy(highlightText, optionText);

    upToVariable = string_Target(highlightText, '%');

	/*upToVariable should now point to the location of memory where '%' resides, if it dosnt exist do not edit highlight text*/
	if(upToVariable != highlightText && *(upToVariable - 1) == ' ') /*If there is a space before the variable sign cut that off as well*/
		*(upToVariable - 1)= '\0';

    TTF_SizeText(font, highlightText, &width, &height);

    mem_Free(highlightText);

    buttonSprite = sprite_Setup(sprite_Create(),  1, layer, srcTimer, 3,
                              frame_CreateBasic(-1, NULL, M_FREE),
                              frame_CreateBasic(0, NULL, M_FREE), /*frame for no hover state*/
                              frame_CreateBasic(25, veMenu_SurfaceHoverShade(width, height, &colourWhite, 35, NULL), A_FREE)
    );

    button = uiButton_Create(0, 0, width, height, onActivate, onHover, info, buttonSprite, 0, 250, srcTimer);

    uiButton_AddText(button, text);

    uiButton_CopyControl(button, &ve_Menu.c_Activate, BC_ALL);

    return button;
}

/*
    Function: veMenu_ButtonCheckBox

    Description -
    Returns a button that should act like a check box.

    9 arguments:
    char *text - The text to go before the text box.
    TTF_Font *font - The font of the text.
    int x - x-axis position of the check box. (Not the text)
    int y - y-axis position of the check box.
    int layer - layer of the button.
    Timer *srcTime - Source timer for the button.
    void (*onActivate)(Ui_Button *button) - Activate function such as MABFA_CheckBox_Activate
    void (*onHover)(Ui_Button *button) - Hover function.
    Data_Struct *info - Data containing the variable to change.
*/
Ui_Button *veMenu_ButtonCheckBox(char *text, TTF_Font *font, int x, int y, int layer, Timer *srcTime, void (*onActivate)(Ui_Button *button), void (*onUpdate)(Ui_Button *button), Data_Struct *info)
{
    Ui_Button *button = NULL;

    Ui_TextBox *textBox = NULL;

    Sprite *sCheckBox = sprite_Copy(&ve_Sprites.checkBox, 1);
    sCheckBox->layer = layer;

    button = uiButton_Create(x, y, sprite_Width(sCheckBox), sprite_Height(sCheckBox), onActivate, NULL, info, sCheckBox, 0, 250, srcTime);

    uiButton_SetUpdate(button, onUpdate, 0);

    if(text != NULL)
    {
        textBox = uiTextBox_CreateBase(0, 0, layer, 0, NULL, font, tColourBlack, srcTime);
        uiTextBox_AddText(textBox,
                         0, 0, text, NULL);

        uiTextBox_SetPos(textBox, -(int)sprite_Width(textBox->graphic) - 5, -2);

        uiButton_AddText(button, textBox);
    }

    uiButton_SetPosText(button, x, y);

    uiButton_CopyControl(button, &ve_Menu.c_Activate, BC_ALL);

    return button;
}

/*
    Function: veMenu_ButtonSprite

    Description -
    Returns a button created with display as the supplied sprite.

    7 arguments:
    int x - x location of the button.
    int y - y location of the button.
    Sprite *buttonSprite - Sprite of the button.
    Timer *srcTimer - Timer for the button.
    void (*onActivate)(Ui_Button *button) - Function to call once button is activated.
    void (*onHover)(Ui_Button *button) - Function to call once button is on hover state.
    Data_Struct *info - Data struct holding variable to change + other info needed, depending on the activate function.
*/
Ui_Button *veMenu_ButtonSprite(int x, int y, Sprite *buttonSprite, Timer *srcTimer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Ui_Button *button = NULL;
    int width = 0;
    int height = 0;

    if(buttonSprite != NULL)
    {
        width = sprite_Width(buttonSprite);
        height = sprite_Height(buttonSprite);
    }
    button = uiButton_Create(x, y, width, height, onActivate, onHover, info, buttonSprite, 0, 250, srcTimer);

    uiButton_CopyControl(button, &ve_Menu.c_Activate, BC_ALL);

    return button;
}

/*
    Function: veMenu_ButtonEmpty

    Description -
    Returns an empty button

    1 arguments:
    Timer *srcTimer - Timer for the button.
*/
Ui_Button *veMenu_ButtonEmpty(Timer *srcTimer)
{
    Ui_Button *button = NULL;

    button = uiButton_Create(0, 0, 0, 0, NULL, NULL, NULL, NULL, 0, 250, srcTimer);

    return button;
}

/*  Function: veMenu_ButtonUnit

    Description -
    Returns a button that has the form of a unit inside of a box.

    7 arguments:
    int unitType - The type of unit the button is based on.
    int layer - The layer of the button.
    Timer *srcTimer - Timer for the button.
    void (*onActivate)(Ui_Button *button) - Function to call once button is activated.
    void (*onHover)(Ui_Button *button) - Function to call once button is on hover state.
    Data_Struct *info - Data struct holding data needed for the activate function.
*/
Ui_Button *veMenu_ButtonUnit(int unitType, int layer, Timer *srcTimer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Sprite *unitSprite = NULL;
    SDL_Surface *unitSurface = NULL;
    SDL_Surface *boxSurface = NULL;
    SDL_Rect boxClip = {0, 0, 48, 48};

    Ui_Button *unitButton = NULL;

    /*Create the background box*/
    boxSurface = surf_SimpleBox(50, 50, (Uint16P *)ker_colourKey(), &colourBlack, 1);

    switch(unitType)
    {
        case UNIT_TANK:

        unitSurface = ve_Surfaces.tank[TEAM_PLAYER][D_UP];

        break;

        case UNIT_AIR:

        unitSurface = ve_Surfaces.air[TEAM_PLAYER][D_UP];

        break;

        case UNIT_INFANTRY:

        unitSurface = ve_Surfaces.infantry[TEAM_PLAYER][D_UP];

        break;

        case UNIT_TOWER:

        unitSurface = ve_Surfaces.tower[TEAM_PLAYER][D_UP];

        break;
    }

    /*Blit the unit surface in the middle of the box*/
    surf_Blit(boxSurface->w/2 - unitSurface->w/2, boxSurface->h/2 - unitSurface->h/2, unitSurface, boxSurface, &boxClip);

    unitSprite = sprite_Setup(sprite_Create(), 1, layer, srcTimer, 1,
                         frame_CreateBasic(0, boxSurface, A_FREE));

    unitButton = uiButton_Create(0, 0, sprite_Width(unitSprite), sprite_Height(unitSprite), onActivate, onHover, info, unitSprite, 0, 250, srcTimer);

    uiButton_CopyControl(unitButton, &ve_Menu.c_Activate, BC_ALL);

    return unitButton;
}

/*
    Function: veMenu_SurfaceHover

    Description -
    Returns a surface that looks as if it highlights a certain area.
    If dest is not NULL then it will not create a surface, instead it will
    blit onto that one.

    7 arguments:
    int buttonWidth - The width of the highlight surface.
    int buttonHeight - The height of the highlight surface.
    int barTotal - The amount of bars to display on the highlight surface.
    int barHeightReduce - By how much should each bar decrease in size.
    Uint16P *colour - The colour of the bar.
    SDL_Surface *dest - If NULL then a new surface will be created, otherwise will blit to this surface.
*/
SDL_Surface *veMenu_SurfaceHover(int buttonWidth, int buttonHeight, int barWidth, int barTotal, int barHeightReduce, Uint16P *colour, SDL_Surface *dest)
{
    SDL_Surface *hoverSurface = dest;

    int x = 0;
    int startX = 0;
    int startY = 0;
    int rightX = buttonWidth;
    int endY = buttonHeight;

    if(dest == NULL)
    {
        hoverSurface = surf_Create(buttonWidth, buttonHeight);
        fill_Rect(0, 0, hoverSurface->w, hoverSurface->h, (Uint16P *)ker_colourKey(), hoverSurface);
    }

    for(x = 0; x < barTotal; x++)
    {
        startX = (x * barWidth);
        startY = x * barHeightReduce;

        pixel_Line(startX, startY, startX, endY - startY, barWidth, 0, colour, hoverSurface);
        pixel_Line((rightX - startX - barWidth), startY, (rightX - startX - barWidth), endY - startY, barWidth, 0, colour, hoverSurface);
    }

    return hoverSurface;
}

/*
    Function: veMenu_SurfaceHoverShade

    Description -
    Returns a surface that looks as if it highlights a certain area, by having a
    transparent layer of certain colour.
    If shadeInto is not NULL then it will blit that surface onto the new highlighted one.
    This makes it blended in.

    5 arguments:
    int buttonWidth - The width of the highlight surface.
    int buttonHeight - The height of the highlight surface.
    Uint16P *shadeColour - The colour of the transparent rect.
    int shadeAmount - The alpha value of the button highlight.
    SDL_Surface *shadeInto - If not NULL then this surface is blitted into the new highlight surface.
*/
SDL_Surface *veMenu_SurfaceHoverShade(int buttonWidth, int buttonHeight, Uint16P *shadeColour, int shadeAmount, SDL_Surface *shadeInto)
{
    SDL_Surface *newSurface = NULL;

    newSurface = surf_Create(buttonWidth, buttonHeight);
    fill_Rect(0, 0, newSurface->w, newSurface->h, shadeColour, newSurface);

    SDL_SetAlpha(newSurface, SDL_SRCALPHA | SDL_RLEACCEL, shadeAmount);

    if(shadeInto != NULL)
        surf_Blit(0, 0, shadeInto, newSurface, NULL);

    return newSurface;
}


/*
    Function: veMenu_WindowOk

    Description -
    Creates a window with basic text and an ok button.

    8 arguments:
    Ui_Spine *baseWindow - The base ui spine that this window will be placed into.
    char *name -  The name of the window.
    char *text - The text inside of the window.
    int x - x-axis location of the window.
    int y - y-axis location of the window.
    int width - width of the window. If <=0 let the function decide based on text width.
    int height - height of the window. If <=0 let the function decide based on text height.
    int layer - layer of the window.
*/
Ui_Spine *veMenu_WindowOk(Ui_Spine *baseWindow, char *name, char *text, int x, int y, int width, int height, int layer)
{
    int flags = 0;
    Ui_Spine *okWindow = NULL;

    SDL_Surface *background = NULL;

    Ui_Button *okButton = NULL;
    Ui_TextBox *message = NULL;

    /*Create the message to display*/
    message = uiTextBox_CreateBase(15, 7, layer, 0, NULL, font_Get(2, 13), tColourBlack, NULL);
    uiTextBox_AddText(message,
                     0, 0, text, NULL);

    /*Create the simple box background*/
    if(width <= 0)
        width = 30 + sprite_Width(message->graphic);
    if(height <=0)
        height = 50 + (sprite_Height(message->graphic) * message->numLines);

    background = surf_SimpleBox(width, height, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Setup the window*/

    if(flag_Check(&baseWindow->flags, UI_SPINE_UPDATESPINES) == 1)
        flags = UI_SPINE_ALLFLAGS;
    else
        flags = UI_SPINE_ALLFLAGS  - UI_SPINE_UPDATESPINES;

    okWindow = uiSpine_Create(layer, x - (background->w/2), y - (background->h/2), frame_CreateBasic(0, background, A_FREE), baseWindow->pnt, 0, NULL, flags);

    uiSpine_AddSpine(baseWindow, okWindow, 0, name);

    /*Add in the text*/
    uiSpine_AddTextBox(okWindow,
                       message,
                       "TextBox:Header");

    /*Add in the 'OK' button*/
    okButton = veMenu_ButtonBasic(15, height - 30, "Ok", &okWindow->sTimer, layer, &MABFA_ExitWindow, &MABFH_ButtonHover, dataStruct_Create(3, baseWindow, okWindow, name));

    uiSpine_AddButton(okWindow, okButton, "Button:Ok");

    veMenu_SetSpineControl(okWindow);
    uiSpine_MapEntity(okWindow);

    uiMap_SetDefaultPath(&okWindow->map, uiSpine_GetEntityBase(okWindow, "Button:Ok"));


    /*Make this current window the only active one by disabling the base spine*/
    uiSpine_Open(okWindow, baseWindow);

    return okWindow;
}

/*
    Function: veMenu_WindowBool

    Description -
    Creates a window with custom text and yes,no buttons.

    8 arguments:
    Ui_Spine *baseWindow - The base ui spine that this window will be placed into.
    char *name -  The name of the window.
    char *text - The custom text inside of the window.
    Data_Struct *textVars - Data structure containing the variables needed by the custom text.
    int x - x-axis location of the window.
    int y - y-axis location of the window.
    Ui_Button *buttonCall - Button to activate once the yes or no button is pressed.
    int *boolChoice - Sets the value of the int to 1 if yes was selected otherwise sets it to 0.
*/
Ui_Spine *veMenu_WindowBool(Ui_Spine *baseWindow, char *name, char *text, Data_Struct *textVars, int x, int y, Ui_Button *buttonCall, int *boolChoice)
{
    int width = 0;
    int height = 0;
    Ui_Spine *boolWindow = NULL;

    SDL_Surface *background = NULL;

    Ui_Button *yesButton = NULL;
    Ui_Button *noButton = NULL;
    Ui_TextBox *message = NULL;

    /*Create the message to display*/
    message = uiTextBox_CreateBase(15, 7, baseWindow->layer + 1, 0, NULL, font_Get(2, 13), tColourBlack, NULL);
    uiTextBox_AddText(message,
                     0, 0, text, textVars);

    /*Create the simple box background*/
    width = 30 + sprite_Width(message->graphic);
    height = 50 + (sprite_Height(message->graphic) * message->numLines);

    background = surf_SimpleBox(width, height, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Setup the window*/
    boolWindow = uiSpine_Create(baseWindow->layer + 1, x - (background->w/2), y - (background->h/2), frame_CreateBasic(0, background, A_FREE), baseWindow->pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(baseWindow, boolWindow, 0, name);

    /*Add in the text*/
    uiSpine_AddTextBox(boolWindow,
                       message,
                       "TextBox:Header");

    /*Add in the 'Yes' and 'No' buttons*/
    yesButton = veMenu_ButtonBasic(15, height - 30, "Yes", &boolWindow->sTimer, boolWindow->layer, &MABFA_ExitBoolWindow, &MABFH_ButtonHover, dataStruct_Create(5, baseWindow, boolWindow, 1, boolChoice, buttonCall));
    noButton = veMenu_ButtonBasic(60, height - 30, "No", &boolWindow->sTimer, boolWindow->layer, &MABFA_ExitBoolWindow, &MABFH_ButtonHover, dataStruct_Create(5, baseWindow, boolWindow, 0, boolChoice, buttonCall));

    uiSpine_AddButton(boolWindow, yesButton, "Button:Yes");
    uiSpine_AddButton(boolWindow, noButton, "Button:No");

    veMenu_SetSpineControl(boolWindow);
    uiSpine_MapEntity(boolWindow);
    uiMap_SetDefaultPath(&boolWindow->map, uiSpine_GetEntityBase(boolWindow, "Button:Yes"));

    /*Make this current window the only active one by disabling the base spine*/
    uiSpine_Open(boolWindow, baseWindow);

    return boolWindow;
}

/*
    Returns a ui scroll populated with files of a certain extension
*/
Ui_ButtonScroll *veMenu_ScrollFile(char *DIRName, char *extension, struct list **nameList, int x, int y, int layer, Timer *srcTimer)
{
    DIR *fileDIR = opendir(DIRName);
    struct list *fileList = NULL;
    struct list *currentList = NULL;

    struct dirent *fileDesc;

    char *fileName = NULL;

    int totalFiles = 0;

    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    Sprite *scrollSprite[2] = {NULL, NULL};

    if(fileDIR == NULL)
    {
        printf("Error cannot open directory (%s) to create file scroll\n", DIRName);

        return NULL;
    }

    scrollSprite[0] = sprite_Copy(&ve_Sprites.scroll[D_UP], 1);
    scrollSprite[0]->layer = layer;

    scrollSprite[1] = sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1);
    scrollSprite[1]->layer = layer;

    scroll = uiButtonScroll_Create(x, y, SCR_V, 7, 5, 1, 0,
                                   veMenu_ButtonSprite(x - 25, y + 20, scrollSprite[1], srcTimer, NULL, &MABFH_ButtonHover, NULL),
                                   veMenu_ButtonSprite(x - 25, y, scrollSprite[0], srcTimer, NULL, &MABFH_ButtonHover, NULL),
                                  0);

    fileList = file_Get_Extension_List(fileDIR, extension);
    currentList = fileList;

    /*Find all the files with extension and create a button for each*/
    while(currentList != NULL)
    {
        fileDesc = (struct dirent *)currentList->data;

        fileName = (char *)mem_Malloc(strlen(fileDesc->d_name) + 1, __LINE__, __FILE__);

        strncpy(fileName, fileDesc->d_name, strlen(fileDesc->d_name));
        fileName[strlen(fileDesc->d_name)] = '\0';

        list_Stack(nameList, fileName, totalFiles);

        button = veMenu_ButtonOption(fileName, NULL, font_Get(2, 13), srcTimer, layer, NULL, &MABFH_ButtonHover, NULL);

        uiButtonScroll_AddButton(scroll, button, 0);

        totalFiles ++;

        currentList = currentList->next;
    }

    list_ClearAll(&fileList);

    closedir(fileDIR);

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);

    return scroll;
}

Ui_ButtonScroll *veMenu_ScrollEmpty(int direction, int aX, int aY, int bX, int bY, int layer, Timer *srcTimer)
{
    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    Sprite *scrollSprite[2] = {NULL, NULL};

    if(direction == SCR_V)
        scrollSprite[0] = sprite_Copy(&ve_Sprites.scroll[D_UP], 1);
    else
        scrollSprite[0] = sprite_Copy(&ve_Sprites.scroll[D_LEFT], 1);

    scrollSprite[0]->layer = layer;

    if(direction == SCR_V)
        scrollSprite[1] = sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1);
    else
        scrollSprite[1] = sprite_Copy(&ve_Sprites.scroll[D_RIGHT], 1);

    scrollSprite[1]->layer = layer;

    scroll = uiButtonScroll_Create(0, 0, direction, 7, 5, 1, 0,
                                   veMenu_ButtonSprite(aX, aY, scrollSprite[1], srcTimer, NULL, &MABFH_ButtonHover, NULL),
                                   veMenu_ButtonSprite(bX, bY, scrollSprite[0], srcTimer, NULL, &MABFH_ButtonHover, NULL),
                                  0);
    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);

    return scroll;
}

/*Standard button functions*/

/*Force buttons sprite to the hover frame*/
void MABFH_ButtonHover(Ui_Button *button)
{
    sprite_ForceFrame(button->graphic, 2, 1);

    return;
}

/*
    Exit the menu state.
    Data struct requires [0] -> Base_State * - Base state that the menu is in.
*/
void MABFA_Quit(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];

    baseState_CallQuit(gState, 0);

    return;
}

/*
    Delete a window in a spine.
    Data struct requires
    [0] -> Ui_Spine *baseSpine,
    [1] -> Ui_Spine *window,
*/
void MABFA_ExitWindow(Ui_Button *button)
{
    Ui_Spine *baseSpine = (Ui_Spine *)button->info->dataArray[0];
    Ui_Spine *window = (Ui_Spine *)button->info->dataArray[1];

    uiSpine_Close(window);

    uiSpine_SetEntityDelete2(baseSpine, window);

    return;
}

/*
    Exit a bool window.
*/
void MABFA_ExitBoolWindow(Ui_Button *button)
{
    Ui_Spine *baseSpine = (Ui_Spine *)button->info->dataArray[0];
    Ui_Spine *window = (Ui_Spine *)button->info->dataArray[1];
    int boolValue = (int)button->info->dataArray[2];
    int *boolChoice = (int *)button->info->dataArray[3];
    Ui_Button *buttonCall = (Ui_Button *)button->info->dataArray[4];

    *boolChoice = boolValue;
    uiButton_TempActive(buttonCall, 1);

    MABFA_ExitWindow(button);

    return;
}

void MABFA_CheckBoxActivate(Ui_Button *button)
{
    int *boolOption = (int *)button->info->dataArray[0];

    BFunc_BoolInt(button);

    sprite_SetFrame(button->graphic, 1 + *boolOption);

    return;
}

void MABFA_CheckBoxToggleFlag(Ui_Button *button)
{
    unsigned int *flags = (unsigned int *)button->info->dataArray[0];
    unsigned int toggle = (unsigned int)button->info->dataArray[1];

    BFunc_ToggleFlag(button);

    sprite_SetFrame(button->graphic, 1 + flag_Check(flags, toggle));

    return;
}

void MABFU_CheckBoxEnsureFlag(Ui_Button *button)
{
    unsigned int *flags = (unsigned int *)button->info->dataArray[0];
    unsigned int toggle = (unsigned int)button->info->dataArray[1];

    sprite_SetFrame(button->graphic, 1 + flag_Check(flags, toggle));

    return;
}

void MABFU_CheckBoxEnsureActivate(Ui_Button *button)
{
    int *boolOption = (int *)button->info->dataArray[0];

    sprite_SetFrame(button->graphic, 1 + *boolOption);
}

/*Draw a line to the left of the button*/
void MABFH_HoverLine(Ui_Button *button)
{
    draw_Line(button->rect.x - 7, button->rect.y + 2, button->rect.x - 7, button->rect.y + button->rect.h - 2, 3, 0, &colourBlack, ker_Screen());

    return;
}

void MABFH_HoverLineThin(Ui_Button *button)
{
    draw_Line(button->rect.x - 5, button->rect.y + 1, button->rect.x - 5, button->rect.y + button->rect.h - 1, 2, 0, &colourBlack, ker_Screen());

    return;
}

void MABFH_ScrollHoverLine(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];

    draw_Line(button->rect.x - 10, button->rect.y + 2, button->rect.x - 10, button->rect.y + button->rect.h - 2, 3, 0, &colourBlack, ker_Screen());

    uiButtonScroll_SetHoverTraverse(scroll, 1);

    return;
}




