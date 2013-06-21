#include "MainMenu.h"
#include "GameMenu.h"
#include <SDL/SDL.h>

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "BaseState.h"
#include "GUI/Button.h"
#include "GUI/ButtonScroll.h"
#include "GUI/ButtonChooser.h"
#include "GUI/ButtonFunc.h"
#include "GUI/Spine.h"
#include "Graphics/Pixel.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "Graphics/SurfaceManager.h"
#include "Graphics/SpriteEffects.h"
#include "Graphics/Effects/FadeOut.h"
#include "Font.h"
#include "Maths.h"

#include "../Player.h"
#include "../Campaign.h"

#include "../Images.h"
#include "../Config.h"
#include "../Options.h"
#include "MenuAttributes.h"
#include "OptionsMenu.h"
#include "../Editor/LevelEditor.h"

struct menu_Main
{
    Ui_Spine spine;

    struct list *saveList;
    char *selectedSave;
    int playerLoaded;

    char saveLetters[4];

    Vent_Player player;

    Timer timer;
};

/*Quit this state*/
static void MM_Quit(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];
    struct menu_Main *menu = gState->info;

    baseState_CallQuit(ker_BaseState(), 1);

    return;
}

static void MM_SetupDefaultCampains(void)
{
    Vent_Campaign *c = NULL;

    c = vCampaign_Create("TestCampaign", 0, 0, 0, 10000,5,
                vCaLvl_Create("Level One", "Level_One.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Two", "Level_Two.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Three", "Level_Three.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Four", "Level_Four.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Seven", "Level_Seven.map", 0, 0, 0, 1)
                );

        vCampaign_SaveBase(c);
        vCampaign_Clean(c);
        mem_Free(c);

        c = vCampaign_Create("TestCampaign_2", 0, 0, 0, -5000, 8,
                vCaLvl_Create("Level One", "Level_One.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Two", "Level_Two.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Three", "Level_Three.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Four", "Level_Four.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Seven", "Level_Seven.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Seven", "Level_Seven.map", 0, 0, 0, 1),
                vCaLvl_Create("Level Seven", "Level_Seven.map", 0, 0, 0, 1),
                vCaLvl_Create("An invalid level", "Level_Invalid.map", 0, 0, 0, 1)
                );

        vCampaign_SaveBase(c);
                vCampaign_Clean(c);
        mem_Free(c);


        c = vCampaign_Create("GP2X", 0, 0, 0, 0, 10,
                vCaLvl_Create("Level One", "Level_One.map", 0, 0, 0, 50),
                vCaLvl_Create("Level Two", "Level_Two.map", 0, 0, 0, 50),
                vCaLvl_Create("Level Three", "Level_Three.map", 0, 0, 0, 200),
                vCaLvl_Create("Level Four", "Level_Four.map", 0, 0, 0, 200),
                vCaLvl_Create("Level Five", "Level_Five.map", 0, 0, 0, 200),
                vCaLvl_Create("Level Six", "Level_Six.map", 0, 0, 0, 500),
                vCaLvl_Create("Level Seven", "Level_Seven.map", 0, 0, 0, 500),
                vCaLvl_Create("Level Eight", "Level_Eight.map", 0, 0, 0, 1500),
                vCaLvl_Create("Level Nine", "Level_Nine.map", 0, 0, 0, 1500),
                vCaLvl_Create("Level Ten", "Level_Ten.map", 0, 0, 0, 2800)
                );

        vCampaign_SaveBase(c);


        //vPlayer_Setup(p, "LAW");

        //vPlayer_AddCampaign(p, c, 1);

      return;
}

static void MM_GameMenu(Ui_Button *button)
{
    struct menu_Main *menu = button->info->dataArray[0];
    Vent_Player *p = NULL;

    Base_State *gameMenu = NULL;

    Vent_Campaign *c = NULL;

            c = vCampaign_Create("GP2X", 0, 0, 0, 0, 10,
                vCaLvl_Create("Level One", "Level_One.map", 0, 0, 0, 50),
                vCaLvl_Create("Level Two", "Level_Two.map", 0, 0, 0, 50),
                vCaLvl_Create("Level Three", "Level_Three.map", 0, 0, 0, 200),
                vCaLvl_Create("Level Four", "Level_Four.map", 0, 0, 0, 200),
                vCaLvl_Create("Level Five", "Level_Five.map", 0, 0, 0, 200),
                vCaLvl_Create("Level Six", "Level_Six.map", 0, 0, 0, 500),
                vCaLvl_Create("Level Seven", "Level_Seven.map", 0, 0, 0, 500),
                vCaLvl_Create("Level Eight", "Level_Eight.map", 0, 0, 0, 1500),
                vCaLvl_Create("Level Nine", "Level_Nine.map", 0, 0, 0, 1500),
                vCaLvl_Create("Level Ten", "Level_Ten.map", 0, 0, 0, 2800)
                );

        vCampaign_SaveBase(c);
        vCampaign_Clean(c);
        mem_Free(c);

    if(menu->playerLoaded == 0)
    {
        /*Since no player is loaded, start up the new game window*/
        uiSpine_Open(uiSpine_GetEntity(&menu->spine, "Spine:New"), &menu->spine);

        return;
    }

    if((gameMenu = baseState_GetState(*ker_BaseStateList(), "Game menu")) != NULL)
    {
        baseState_ActivateList(ker_BaseStateList(), gameMenu);
    }
    else
    {
        baseState_Push(ker_BaseStateList(), baseState_Create("Game menu", Menu_Game_Init, Menu_Game_Controls, Menu_Game_Logic, Menu_Game_Render, Menu_Game_Exit, &menu->player));
    }

    timer_StopSkip(&menu->timer, 0, 1);

    return;
}

static void MM_LevelEditor(Ui_Button *button)
{
    struct menu_Main *menu = button->info->dataArray[0];

    Base_State *levelEditor = NULL;

    if((levelEditor = baseState_GetState(*ker_BaseStateList(), "Level editor")) != NULL)
    {
        baseState_ActivateList(ker_BaseStateList(), levelEditor);
    }
    else
    {
        baseState_Push(ker_BaseStateList(), baseState_Create("Level editor", vLE_Init, vLE_Controls, vLE_Logic, vLE_Render, vLE_Exit, NULL));
    }

    timer_StopSkip(&menu->timer, 0, 1);

    return;
}

static void MM_ChooseSave(Ui_Button *button)
{
    struct menu_Main *menu = button->info->dataArray[0];
    char *saveName = button->info->dataArray[1];

    menu->selectedSave = saveName;

    return;
}

static Ui_ButtonScroll *MM_CreateSaveScroll(Base_State *gState)
{
    struct menu_Main *menu = gState->info;

    struct list *currentList = NULL;
    struct list *nameList = NULL;

    Ui_Button *button = NULL;
    Ui_ButtonScroll *scroll = NULL;

    int x = 0;

	list_ClearAll(&menu->saveList);
    scroll = veMenu_ScrollFile("../Saves", ".save", &menu->saveList, 40, 40, VL_HUD + 2, &menu->timer);

    scroll->rotate = 0;
    scroll->maxShowButtons = 4;

    currentList = menu->saveList;

    /*Adjust the scroll buttons*/
    while(currentList != NULL)
    {
        button = uiButtonScroll_GetButton(scroll, x);

        /*Give correct activate function and info*/
        button->onActivate = MM_ChooseSave;
        button->info = dataStruct_Create(2, menu, currentList->data);

        x++;
        currentList = currentList->next;
    }

    return scroll;
}

static void MM_StartLoad(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];

    struct menu_Main *menu = gState->info;
	Ui_Spine *spineLoad = (Ui_Spine *)uiSpine_GetEntity(&menu->spine, "Spine:Load");

	/*Reload the save scroll*/
	uiSpine_DeleteEntity(spineLoad, uiSpine_GetEntityBase(spineLoad, "Scroll:Saves"));
	uiSpine_AddScroll(spineLoad, MM_CreateSaveScroll(gState), "Scroll:Saves");
	uiSpine_MapEntity(spineLoad);
	
    uiSpine_Open(spineLoad, &menu->spine);


    timer_StopSkip(&menu->timer, 0, 1);

    return;
}

static void MM_QuitLoad(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];
    struct menu_Main *menu = gState->info;

    int load = (int)button->info->dataArray[1];

    char saveName[32];

    /*Disable the load menu*/
    uiSpine_Close(uiSpine_GetEntity(&menu->spine, "Spine:Load"));

    /*If the accept button was pressed and a save has been selected*/
    if(load == 1 && strcmp(menu->selectedSave, "") != 0)
    {
        /*If a player is already loaded*/
        if(menu->playerLoaded == 1)
        {
            vPlayer_Clean(&menu->player);
        }

        strncpy(saveName, menu->selectedSave, 31);

        string_RemoveExt(saveName);

        printf("Loading %s save\n", saveName);

        vPlayer_Load(&menu->player, saveName);

        /*Set the current campaign to GP2X*/
        vPlayer_LoadCampaign(&menu->player, menu->player.lastPlayedCampaign);


        menu->playerLoaded = 1;

        /*Enable the base menu*/
        uiSpine_Enable(&menu->spine);
    }
    else if(load == 0)
    {
        /*Enable the base menu*/
        //uiSpine_Enable(&menu->spine);
    }
    else if(load == 2) /*New game selected*/
    {
        uiSpine_Open(uiSpine_GetEntity(&menu->spine, "Spine:New"), &menu->spine);
    }

    return;
}

static void MM_Options(Ui_Button *button)
{
    struct menu_Main *menu = button->info->dataArray[0];
    Base_State *optionsMenu = baseState_GetState(*ker_BaseStateList(), "Options menu");

    timer_StopSkip(&menu->timer, 500, 1);

    if(optionsMenu != NULL)
    {
        baseState_ActivateList(ker_BaseStateList(), optionsMenu);
    }
    else
    {
        baseState_Push(ker_BaseStateList(), baseState_Create("Options menu", Menu_Options_Init, Menu_Options_Controls, Menu_Options_Logic, Menu_Options_Render, Menu_Options_Exit, NULL));
    }

    return;
}

static void MM_SetupLoadMenu(Base_State *gState)
{
    struct menu_Main *menu = gState->info;

    Ui_Spine *spineLoad = NULL;

    SDL_Surface *buttonSurface = NULL;
    SDL_Surface *background = NULL;

    Ui_Button *acceptButton = NULL;
    Ui_Button *cancelButton = NULL;
    Ui_Button *newButton = NULL;

    menu->saveList = NULL;
    menu->selectedSave = "";

    /*Setup the main spine for the load window*/
    background = surf_SimpleBox(300, 200, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    spineLoad = uiSpine_Create(VL_HUD + 2, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), menu->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS);

    uiSpine_AddSpine(&menu->spine, spineLoad, 0, "Spine:Load");

    /*Add in the header text*/
    uiSpine_AddTextBox(spineLoad,
                       uiTextBox_CreateBase(15, 10, VL_HUD + 2, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:Header");

    uiTextBox_AddText(uiSpine_GetEntity(spineLoad,"TextBox:Header"),
                     0, 0, "Load: %s", dataStruct_Create(1, &menu->selectedSave));

    /*Add in the accept and cancel button*/
    acceptButton = veMenu_ButtonBasic(15, 160, "Accept", &spineLoad->sTimer, VL_HUD + 2, &MM_QuitLoad, &MABFH_ButtonHover, dataStruct_Create(2, gState, 1));

    cancelButton = veMenu_ButtonBasic(100, 160, "Cancel", &spineLoad->sTimer, VL_HUD + 2, &MM_QuitLoad, &MABFH_ButtonHover, dataStruct_Create(2, gState, 0));

    newButton = veMenu_ButtonBasic(185, 160, "New", &spineLoad->sTimer, VL_HUD + 2, &MM_QuitLoad, &MABFH_ButtonHover, dataStruct_Create(2, gState, 2));

    uiSpine_AddButton(spineLoad, newButton, "Button:New");
    uiSpine_AddButton(spineLoad, cancelButton, "Button:Cancel");
    uiSpine_AddButton(spineLoad, acceptButton, "Button:Accept");


    /*Find all the saves and put them in a scroll list*/
    uiSpine_AddScroll(spineLoad, MM_CreateSaveScroll(gState), "Scroll:Saves");

    /*Setup the button chooser */
    veMenu_SetSpineControl(spineLoad);
    uiSpine_MapEntity(spineLoad);


    /*Start off disabled*/
    uiSpine_Close(uiSpine_GetEntity(&menu->spine, "Spine:Load"));

    return;
}

static void MM_ChangeSaveLetter(Ui_Button *button)
{
    struct menu_Main *menu = button->info->dataArray[0];
    int index = (int)button->info->dataArray[1];
    int direction = (int)button->info->dataArray[2];

    /*Rotate from A - Z - 0 - 9*/

    if(direction == D_DOWN)
    {
        menu->saveLetters[index] = menu->saveLetters[index] + 1;

        if(menu->saveLetters[index] > 'Z')
        {
            menu->saveLetters[index] = '0';
        }
        else if(menu->saveLetters[index] > '9' && menu->saveLetters[index] < 'A')
        {
            menu->saveLetters[index] = 'A';
        }
    }
    else if(direction == D_UP)
    {
        menu->saveLetters[index] = menu->saveLetters[index] - 1;

        if(menu->saveLetters[index] < 'A' && menu->saveLetters[index] > '9')
        {
            menu->saveLetters[index] = '9';
        }
        else if(menu->saveLetters[index] < '0')
        {
            menu->saveLetters[index] = 'Z';
        }
    }

    return;
}

static void MM_ForceActive(Ui_Button *button)
{
    int x = 0;
    Ui_Button *buttonForce = NULL;

    for(x = 0; x < button->info->amount; x++)
    {
        buttonForce = button->info->dataArray[x];
        uiButton_TempHover(buttonForce, 1);
    }

    return;
}

static void MM_QuitNew(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];
    struct menu_Main *menu = gState->info;

    int accept = (int)button->info->dataArray[1];

    /*Disable the new game menu*/
    uiSpine_Close(uiSpine_GetEntity(&menu->spine, "Spine:New"));

    /*If the create button was pressed*/
    if(accept == 1)
    {
        /*If there is already a player with this name*/
        if(vPlayer_CheckSaved(menu->saveLetters) == 1)
        {
            veMenu_WindowOk(&menu->spine, "Spine:AlreadySave", "Already a player with that name.", ker_Screen_Width()/2, ker_Screen_Height()/2, 240, 60, menu->spine.layer + 1);
        }
        else
        {
            /*If a player is already loaded*/
            if(menu->playerLoaded == 1)
            {
                vPlayer_Clean(&menu->player);
            }

            printf("Creating %s save\n", menu->saveLetters);

            vPlayer_Setup(&menu->player, menu->saveLetters);
            vPlayer_Save(&menu->player);

            vPlayer_LoadCampaign(&menu->player, "GP2X");

            menu->playerLoaded = 1;
        }
    }

    return;
}

static Ui_ButtonScroll *MM_CreateLetterScroll(struct menu_Main *menu, int x, int y, int layer, int index)
{
    Ui_ButtonScroll *scroll = NULL;
    Sprite *scrollSprite[2] = {sprite_Copy(&ve_Sprites.scroll[D_UP], 1), sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1)};

    Ui_Button *scrollButton[2] = {NULL, NULL};

    scrollSprite[1]->layer = scrollSprite[0]->layer = layer;


    scrollButton[0] = veMenu_ButtonSprite(x - sprite_Width(scrollSprite[0])/2, y - 20, scrollSprite[0], &menu->spine.sTimer, &MM_ChangeSaveLetter, &MABFH_ButtonHover, dataStruct_Create(3, menu, index, D_UP));
    uiButton_CopyControl(scrollButton[0], &ve_Menu.c_MoveUp, BC_ALL);

    scrollButton[1] = veMenu_ButtonSprite(x - sprite_Width(scrollSprite[1])/2, y + 20, scrollSprite[1], &menu->spine.sTimer, &MM_ChangeSaveLetter, &MABFH_ButtonHover, dataStruct_Create(3, menu, index, D_DOWN));
    uiButton_CopyControl(scrollButton[1], &ve_Menu.c_MoveDown, BC_ALL);

    scroll = uiButtonScroll_Create(x, y,
                       SCR_V, ve_Menu.buttonSpaceing, 1, 1, 0,
                       scrollButton[0],
                       scrollButton[1],
                       1,
                       veMenu_ButtonOption("  ", NULL, font_Get(2, 13), &menu->spine.sTimer, layer, NULL, &MM_ForceActive, dataStruct_Create(2, scrollButton[0], scrollButton[1]))
                       );
    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLineThin);

    return scroll;
}

static Ui_TextBox *MM_CreateSaveLetter(struct menu_Main *menu, int x, int y, int index)
{
    Ui_TextBox *textBox = NULL;

    textBox = uiTextBox_CreateBase(x, y, VL_HUD + 2, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer);

    uiTextBox_AddText(textBox,
                     0, 0, "%c", dataStruct_Create(1, &menu->saveLetters[index]));

    return textBox;
}

static void MM_SetupNewMenu(Base_State *gState)
{
    struct menu_Main *menu = gState->info;

    Ui_Spine *spineNew = NULL;

    SDL_Surface *buttonSurface = NULL;
    SDL_Surface *background = NULL;

    Ui_Button *acceptButton = NULL;
    Ui_Button *cancelButton = NULL;

    Ui_ButtonScroll *letterScroll[3] = {NULL, NULL, NULL};

    menu->saveLetters[0] = 'A';
    menu->saveLetters[1] = 'A';
    menu->saveLetters[2] = 'A';
    menu->saveLetters[3] = '\0';

    /*Setup the main spine for the load window*/
    background = surf_SimpleBox(170, 140, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    spineNew = uiSpine_Create(VL_HUD + 2, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), menu->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS);

    uiSpine_AddSpine(&menu->spine, spineNew, 0, "Spine:New");

    /*Add in the header text*/
    uiSpine_AddTextBox(spineNew,
                       uiTextBox_CreateBase(15, 10, VL_HUD + 2, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:Header");

    uiTextBox_AddText(uiSpine_GetEntity(spineNew,"TextBox:Header"),
                     0, 0, "Please enter a name:", NULL);

    /*Add in the accept and cancel button*/
    acceptButton = veMenu_ButtonBasic(15, 110, "Create", &spineNew->sTimer, VL_HUD + 2, &MM_QuitNew, &MABFH_ButtonHover, dataStruct_Create(2, gState, 1));

    cancelButton = veMenu_ButtonBasic(100, 110, "Cancel", &spineNew->sTimer, VL_HUD + 2, &MM_QuitNew, &MABFH_ButtonHover, dataStruct_Create(2, gState, 0));

    uiSpine_AddButton(spineNew, acceptButton, "Button:Accept");
    uiSpine_AddButton(spineNew, cancelButton, "Button:Cancel");

    /*Setup the letter scrolls*/
    uiSpine_AddTextBox(spineNew, MM_CreateSaveLetter(menu, 40 - 2, 60, 0), "TextBox:Letter1");
    uiSpine_AddTextBox(spineNew, MM_CreateSaveLetter(menu, 75 - 2, 60, 1), "TextBox:Letter2");
    uiSpine_AddTextBox(spineNew, MM_CreateSaveLetter(menu, 110 - 2, 60, 2), "TextBox:Letter3");
    letterScroll[0] = MM_CreateLetterScroll(menu, 40, 60, VL_HUD + 2, 0);
    letterScroll[1] = MM_CreateLetterScroll(menu, 75, 60, VL_HUD + 2, 1);
    letterScroll[2] = MM_CreateLetterScroll(menu, 110, 60, VL_HUD + 2, 2);
	
    uiSpine_AddScroll(spineNew, letterScroll[0], "Scroll:Letter1");
    uiSpine_AddScroll(spineNew, letterScroll[1], "Scroll:Letter2");
    uiSpine_AddScroll(spineNew, letterScroll[2], "Scroll:Letter3");
				
    /*Setup the button chooser */
    veMenu_SetSpineControl(spineNew);
    uiSpine_MapEntity(spineNew);

    uiSpine_Close(uiSpine_GetEntity(&menu->spine, "Spine:New"));

    return;
}

void Menu_Main_Init(void *info)
{
    Base_State *gState = info;
    struct menu_Main *menu = (struct menu_Main *)mem_Malloc( sizeof(struct menu_Main), __LINE__, __FILE__);

    SDL_Surface *background = NULL;

    int x = 0;

    const int tButtons = 5;
    Ui_Button *button[5];

    Ui_TextBox *textBox = NULL;

    Ui_ButtonScroll *mainScroll = NULL;

    gState->info = menu;

    if(strcmp(ve_Options.lastSave, "%") != 0)
    {
        menu->playerLoaded = vPlayer_Load(&menu->player, ve_Options.lastSave);
    }
    else
    {
        menu->playerLoaded = 0;
    }

    /*Setup timer*/
    menu->timer = timer_Setup(NULL, 0, 0, 1);

    /*Setup menu spine*/
    background = surf_Copy(ve_Surfaces.mainMenu);

    uiSpine_Setup(&menu->spine, VL_HUD + 1, ve_Menu.menuPosition.x, ve_Menu.menuPosition.y, frame_CreateBasic(0, background, A_FREE), &ve_Menu.pointer, 0, NULL, UI_SPINE_ALLFLAGS);

    /*Create buttons*/
    button[0] = veMenu_ButtonSide(0, 0, "START", &menu->timer, &MM_GameMenu, &MABFH_ButtonHover, dataStruct_Create(1, menu));

    button[1] = veMenu_ButtonSide(0, 0, "LOAD", &menu->timer, &MM_StartLoad, &MABFH_ButtonHover, dataStruct_Create(1, gState));

    button[2] = veMenu_ButtonSide(0, 0, "OPTIONS", &menu->timer, &MM_Options, &MABFH_ButtonHover, dataStruct_Create(1, menu));

    button[3] = veMenu_ButtonSide(0, 0, "EDITOR", &menu->timer, &MM_LevelEditor, &MABFH_ButtonHover, dataStruct_Create(1, menu));

    button[4] = veMenu_ButtonSide(0, 0, "EXIT", &menu->timer, &MM_Quit, &MABFH_ButtonHover, dataStruct_Create(1, gState));

    uiSpine_AddButton(&menu->spine, button[0], "Button:Start");
    uiSpine_AddButton(&menu->spine, button[1], "Button:Load");
    uiSpine_AddButton(&menu->spine, button[2], "Button:Options");
    uiSpine_AddButton(&menu->spine, button[3], "Button:Editor");
    uiSpine_AddButton(&menu->spine, button[4], "Button:Exit");

    uiSpine_AlignEntities(&menu->spine, ve_Menu.buttonPosition.x, ve_Menu.buttonPosition.y, ve_Menu.buttonSpaceing, SCR_V, 5,
                          "Button:Start",
                          "Button:Load",
                          "Button:Options",
                          "Button:Editor",
                          "Button:Exit");
		
    MM_SetupNewMenu(gState);
    MM_SetupLoadMenu(gState);
	
    veMenu_SetSpineControl(&menu->spine);
    uiSpine_MapEntity(&menu->spine);


    timer_Start(&menu->timer);

    return;
}

void Menu_Main_Controls(void *info)
{
    Base_State *gState = info;
    struct menu_Main *menu = gState->info;
    /*If the base menu is active and the back control is pressed*/
    if(menu->spine.state == SPINE_STATE_IDLE && control_IsActivated(&ve_Menu.c_Back) == 1)
    {
        uiButton_TempActive(uiSpine_GetEntity(&menu->spine, "Button:Exit"), 1);
    }

    return;
}

void Menu_Main_Logic(void *info)
{
    Base_State *gState = info;
    struct menu_Main *menu = gState->info;

    timer_Calc(&menu->timer);

    uiSpine_Update(&menu->spine, 0);

    return;
}

void Menu_Main_Render(void *info)
{
    Base_State *gState = info;
    struct menu_Main *menu = gState->info;

    uiSpine_Draw(&menu->spine, ker_Screen());

    text_Draw_Arg(260, 60, ker_Screen(), font_Get(2, 11), &tColourBlack, VL_HUD + 1, 0, "%s", VENT_VERSION);

    if(menu->playerLoaded == 1)
    {
        text_Draw_Arg(494 - 15, 480 - 65, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "%s", menu->player.pName);
    }

    return;
}

void Menu_Main_Exit(void *info)
{
    Base_State *gState = info;
    struct menu_Main *menu = gState->info;

    /*Clear player*/
    if(menu->playerLoaded == 1)
    {
        vPlayer_Save(&menu->player);
        vPlayer_Clean(&menu->player);
    }

    /*Save config*/
    vConfig_Save();

    list_ClearAll(&menu->saveList);

    /*Clear up ui*/
    uiSpine_Clean(&menu->spine);

    /*Remove any graphics that should not be drawn*/
    gKer_FlushGraphics();

    mem_Free(menu);

    return;
}
