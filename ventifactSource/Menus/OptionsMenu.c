#include "OptionsMenu.h"

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "GUI/ButtonFunc.h"
#include "GUI/Spine.h"

#include "MenuAttributes.h"
#include "ControlMenu.h"

#include "../Images.h"
#include "../Defines.h"
#include "../Options.h"

struct menu_Options
{
    Ui_Spine spine;

    int updateOptions; /*If true then the options that have changed will be set in the program*/

    Timer timer;
};

static char *difficultyText[2] = {"Easy", "Standard"};

static void OM_ControlMenu(Ui_Button *button)
{
    struct menu_Options *menu = button->info->dataArray[0];
    Base_State *controlMenu = baseState_GetState(*ker_BaseStateList(), "Control menu");

    if(controlMenu != NULL)
    {
        baseState_ActivateList(ker_BaseStateList(), controlMenu);
    }
    else
    {
        baseState_Push(ker_BaseStateList(), baseState_Create("Control menu", Menu_Controls_Init, Menu_Controls_Controls, Menu_Controls_Logic, Menu_Controls_Render, Menu_Controls_Exit, NULL));
    }

    timer_StopSkip(&menu->timer, 0, 1);

    return;
}


static void OM_BoolButtonActivate(Ui_Button *button)
{
    Ui_Button *boolButton = (Ui_Button *)button->info->dataArray[0];

    boolButton->forceActivation = 1;

    return;
}

/*Update a bool variable and set the frame of the graphic to either On/Off surface*/
static void OM_BoolActivate(Ui_Button *button)
{
    int *boolOption = (int *)button->info->dataArray[0];

    BFunc_BoolInt(button);

    sprite_SetFrame(button->graphic, 1 + *boolOption);

    return;
}

static void BFA_ToggleMute(Ui_Button *button)
{
    BFunc_BoolInt(button);

    if(ve_Options.mute == 1)
        sound_Mute();
    else
        sound_Unmute();

    return;
}
/*
    Returns a button that should change an option variable.
*/
static Ui_Button *OM_CreateOption(struct menu_Options *menu, char *optionName, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info)
{
    Sprite *buttonSprite = NULL;
    Ui_Button *button = NULL;

    SDL_Surface *textSurface = text_Arg(ve_Menu.mainFont[FS_MEDIUM], &tColourBlack, optionName);

    buttonSprite = sprite_Setup(sprite_Create(),  1, VL_HUD + 1, &menu->timer, 3,
                              frame_CreateBasic(-1, textSurface, A_FREE),
                              frame_CreateBasic(0, NULL, M_FREE), /*frame for no hover state*/
                              frame_CreateBasic(25, veMenu_SurfaceHoverShade(textSurface->w + 1, textSurface->h, &colourWhite, 35, textSurface), A_FREE)
    );

    /*frame_CreateBasic(25, veMABFH_ButtonHover(textSurface->w + 8, textSurface->h, 4, 1, 2, &colourBlack, NULL), A_FREE)*/

    button = uiButton_Create(0, 0, sprite_Width(buttonSprite), sprite_Height(buttonSprite), onActivate, onHover, info, buttonSprite, 0, 250, &menu->timer);

    uiButton_CopyControl(button, &ve_Menu.c_Activate, BC_ALL);

    return button;
}

static Ui_Button *OM_CreateCheckBox(int x, int y, int layer, int *boolOption, Timer *srcTime)
{
    Ui_Button *button = NULL;

    Sprite *sCheckBox = sprite_Copy(&ve_Sprites.checkBox, 1);
    sCheckBox->layer = layer;

    button = uiButton_Create(x, y, sprite_Width(sCheckBox), sprite_Height(sCheckBox), &OM_BoolActivate, NULL, dataStruct_Create(1, boolOption), sCheckBox, 0, 250, srcTime);

    uiButton_CopyControl(button, &ve_Menu.c_Activate, BC_ALL);

    return button;
}


static void Menu_Options_SetupControls(void *info)
{
    Base_State *gState = info;
    struct menu_Options *menu = gState->info;

    veMenu_SetSpineControl(&menu->spine);
    uiSpine_MapEntity(&menu->spine);

    /*Add in the side menu buttons to the chooser*/

    /*Add in the gameplay option buttons*/

    /*Add in the sound option buttons*/

    /*Link the buttons together*/

    return;
}

void Menu_Options_Init(void *info)
{
    Base_State *gState = info;
    struct menu_Options *menu = (struct menu_Options *)mem_Malloc( sizeof(struct menu_Options), __LINE__, __FILE__);

    SDL_Surface *background = NULL;

    int x = 0;

    const int tButtons = 2;
    Ui_Button *button[2];

    Ui_ButtonScroll *scroll = NULL;

    Sprite *sBox = NULL;

    gState->info = menu;

    /*Setup timer*/
    menu->timer = timer_Setup(NULL, 0, 0, 1);

    /*Setup menu spine*/
    background = surf_Copy(ve_Surfaces.mainMenu);

    uiSpine_Setup(&menu->spine, VL_HUD + 1, ve_Menu.menuPosition.x, ve_Menu.menuPosition.y, frame_CreateBasic(0, background, A_FREE), &ve_Menu.pointer, 0, NULL, UI_SPINE_ALLFLAGS);

    /*Create buttons*/

    /*Control button*/
    button[0] = veMenu_ButtonSide(ve_Menu.buttonPosition.x, ve_Menu.buttonPosition.y, "CONTROLS", &menu->timer, &OM_ControlMenu, &MABFH_ButtonHover, dataStruct_Create(1, menu));

    /*Back button*/
    button[1] = veMenu_ButtonSide(ve_Menu.buttonPosition.x, ve_Menu.buttonPosition.y + 4 * (ve_Menu.buttonSpaceing + ve_Menu.buttonHeight), "BACK", &menu->timer, &MABFA_Quit, &MABFH_ButtonHover, dataStruct_Create(1, gState));

    uiSpine_AddButton(&menu->spine, button[1], "Button:Back");
    uiSpine_AddButton(&menu->spine, button[0], "Button:Controls");

    /*Sound options*/
    scroll = uiButtonScroll_Create(520, 150,
                                       SCR_V, 10, 5, 0, 0,
                                       NULL, NULL, 2,
                                       veMenu_ButtonOption("Mute: %si", dataStruct_Create(2, &boolText[0], &ve_Options.mute), font_Get(2, 13), &menu->timer, VL_HUD + 1, &BFA_ToggleMute, &MABFH_ButtonHover, dataStruct_Create(1, &ve_Options.mute)),
                                       veMenu_ButtonOption("Volume: %d", dataStruct_Create(1, &ve_Options.volume), font_Get(2, 13), &menu->timer, VL_HUD + 1, &BFunc_ChangeInt, &MABFH_ButtonHover, dataStruct_Create(2, &ve_Options.volume, 2))
    );

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);

    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Sound");

    /*Gameplay options*/

    scroll = uiButtonScroll_Create(225, 150,
                                   SCR_V, 10, 5, 0, 0,
                                   NULL, NULL, 4,
                                   veMenu_ButtonOption("Difficulty: %si", dataStruct_Create(2, &difficultyText[0], &ve_Options.difficulty), font_Get(2, 13), &menu->timer, VL_HUD + 1, &BFunc_BoolInt, &MABFH_ButtonHover, dataStruct_Create(1, &ve_Options.difficulty)),
                                   veMenu_ButtonOption("Team ai: %si", dataStruct_Create(2, &boolText[0], &ve_Options.teamAi), font_Get(2, 13), &menu->timer, VL_HUD + 1, &BFunc_BoolInt, &MABFH_ButtonHover, dataStruct_Create(1, &ve_Options.teamAi)),
                                   veMenu_ButtonOption("Cheat: %si", dataStruct_Create(2, &boolText[0], &ve_Options.cheat), font_Get(2, 13), &menu->timer, VL_HUD + 1, &BFunc_BoolInt, &MABFH_ButtonHover, dataStruct_Create(1, &ve_Options.cheat)),
                                   veMenu_ButtonOption("FPS counter: %si", dataStruct_Create(2, &boolText[0], &ve_Options.showFPS), font_Get(2, 13), &menu->timer, VL_HUD + 1, &BFunc_BoolInt, &MABFH_ButtonHover, dataStruct_Create(1, &ve_Options.showFPS))
                                   );

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);

    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Gameplay");

    Menu_Options_SetupControls(info);

    menu->updateOptions = 0;

    timer_Start(&menu->timer);

    return;
}

void Menu_Options_Controls(void *info)
{
    Base_State *gState = info;
    struct menu_Options *menu = gState->info;

    /*If the base menu is active and the back control is pressed*/
    if(menu->spine.state == SPINE_STATE_IDLE && control_IsActivated(&ve_Menu.c_Back) == 1)
    {
        uiButton_TempActive(uiSpine_GetEntity(&menu->spine, "Button:Back"), 1);
    }

    return;
}

void Menu_Options_Logic(void *info)
{
    Base_State *gState = info;
    struct menu_Options *menu = gState->info;

    timer_Calc(&menu->timer);

    uiSpine_Update(&menu->spine, 0);

    return;
}

void Menu_Options_Render(void *info)
{
    Base_State *gState = info;
    struct menu_Options *menu = gState->info;

    uiSpine_Draw(&menu->spine, ker_Screen());

    text_Draw_Arg(225, 120, ker_Screen(), ve_Menu.mainFont[FS_LARGE], &tColourBlack, VL_HUD + 1, 0, "Gameplay:");
    text_Draw_Arg(520, 120, ker_Screen(), ve_Menu.mainFont[FS_LARGE], &tColourBlack, VL_HUD + 1, 0, "Sound:");

    return;
}

void Menu_Options_Exit(void *info)
{
    Base_State *gState = info;
    struct menu_Options *menu = gState->info;

    /*Remove any graphics that should not be drawn*/
    gKer_FlushGraphics();

    /*Clear up ui*/
    uiSpine_Clean(&menu->spine);

    mem_Free(menu);

    return;
}
