#include "GameMenu.h"
#include <SDL/SDL.h>

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "BaseState.h"
#include "GUI/Button.h"
#include "GUI/ButtonScroll.h"
#include "GUI/ButtonFunc.h"
#include "GUI/ButtonChooser.h"
#include "GUI/Spine.h"
#include "DataStruct.h"
#include "Controls.h"
#include "Graphics/Sprite.h"
#include "Graphics/SpriteEffects.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "Graphics/Effects/FadeOut.h"
#include "Graphics/Effects/Slide.h"
#include "Font.h"

#include "../Images.h"
#include "../Game/State.h"
#include "../Player.h"
#include "MenuAttributes.h"
#include "UpgradeMenu.h"
#include "../Level/Level.h"
#include "../Level/LevelFile.h"
#include "../CampaignScores.h"

struct menu_Game
{
    char *selectedCampaign;

    Ui_Spine spine;

    struct list *nameList;
    struct list *campaignNameList;

    Timer timer;
    float prevTime; /*store the time of the previous loop, to use for delta time*/

    Vent_Player *player;

    Vent_Level_Header levelHeader;
};

/*
    Button activation functions
    ----------------------------
*/

static void MM_Quit(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];

    /*Quit the game menu state*/
    baseState_CallQuit(gState, 0);

    return;
}

static void MM_StartGame(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];
    struct menu_Game *menu = gState->info;

    if(strcmp(menu->player->levelChosen, "None") != 0)
        baseState_Push(ker_BaseStateList(), baseState_Create("Game state", Game_Init, Game_Controls, Game_Logic, Game_Render, Game_Exit, menu->player));
    else
    {
        printf("Error no map chosen! (%s)\n", menu->player->levelChosen);
        veMenu_WindowOk(&menu->spine, "Spine:MapWarningBox", "No map chosen.", ker_Screen_Width()/2, ker_Screen_Height()/2, 150, 60, menu->spine.layer + 1);
    }

    timer_StopSkip(&menu->timer, 0, 1);

    return;
}

static void MM_Upgrade(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];
    struct menu_Game *menu = gState->info;

    Base_State *upgradeMenu = NULL;

    if(menu->player->playCampaign == 0)
    {
        veMenu_WindowOk(&menu->spine, "Spine:UpgradeWarningBox", "No campaign selected.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, menu->spine.layer + 1);
        return;
    }

    if((upgradeMenu = baseState_GetState(*ker_BaseStateList(), "Upgrade menu")) != NULL)
    {
        baseState_ActivateList(ker_BaseStateList(), upgradeMenu);
    }
    else
    {
        baseState_Push(ker_BaseStateList(), baseState_Create("Upgrade menu", Menu_Upgrade_Init, Menu_Upgrade_Controls, Menu_Upgrade_Logic, Menu_Upgrade_Render, Menu_Upgrade_Exit, menu->player));
    }

    timer_StopSkip(&menu->timer, 0, 1);

    return;
}

static void MM_ButtonHover(Ui_Button *button)
{
    sprite_SetFrame(button->graphic, 2);

    return;
}

static void MM_ChooseCampaign(Ui_Button *button)
{
    struct menu_Game *menu = button->info->dataArray[0];
    char *campaignName = button->info->dataArray[1];

    /*obtain the campaign window*/
    Ui_Spine *campaignSpine = uiSpine_GetEntity(&menu->spine, "Spine:Campaign");

    /*assign the campaign name*/
    menu->selectedCampaign = campaignName;

    return;
}

/*
    Returns a ui scroll populated with campaigns
*/
static Ui_ButtonScroll *MM_CreateCampaignScroll(Base_State *gState)
{
    struct menu_Game *menu = gState->info;

    struct list *currentList = NULL;

    Ui_Button *button = NULL;
    Ui_ButtonScroll *scroll = NULL;

    int x = 0;

    scroll = veMenu_ScrollFile("../Campaigns", ".cam", &menu->campaignNameList, 40, 40, VL_HUD + 2, &menu->timer);

    currentList = menu->campaignNameList;

    /*Adjust the scroll buttons*/
    while(currentList != NULL)
    {
        button = uiButtonScroll_GetButton(scroll, x);

        button->onActivate = MM_ChooseCampaign;
        button->info = dataStruct_Create(2, menu, currentList->data);

        x++;
        currentList = currentList->next;
    }

    /*Insert the none campaign into the list*/
    button = veMenu_ButtonOption("None", NULL, font_Get(2, 13), &menu->timer, VL_HUD + 2, &MM_ChooseCampaign, &MABFH_ButtonHover, dataStruct_Create(2, menu, "None"));

    uiButtonScroll_AddButton(scroll, button, 1);

    return scroll;
}

static void MM_SelectCampaign(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];

    struct menu_Game *menu = gState->info;

    /*Disable updating of main spine*/
    //uiSpine_DisableBase(&menu->spine);

    /*Enable updating of campaign spine*/
    //uiSpine_SetEntityUpdate(&menu->spine, "Spine:Campaign", UIS_ENTITY_UPDATE, 1);

    uiSpine_Open(uiSpine_GetEntity(&menu->spine, "Spine:Campaign"), &menu->spine);

    //timer_StopSkip(&campaignSpine->sTimer, 0, 1);

    return;
}

void MM_RepopulateLevels(Base_State *gState)
{
    struct menu_Game *menu = gState->info;

    Ui_ButtonScroll *scroll = NULL;

    /*Clean up the current level scroll*/
    scroll = uiSpine_RemoveEntity(&menu->spine, "Scroll:Levels");

    uiButtonScroll_Clean(scroll);
    mem_Free(scroll);

    /*Add in the new one*/
    scroll =  MM_CreateLevelScroll(gState);

    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Levels");

    menu->player->levelChosen = "None";
    vLevel_SetupHeader(&menu->levelHeader, "None", 0, 0);

    uiSpine_MapEntity(&menu->spine);
    return;
}

static void MM_QuitCampaign(Ui_Button *button)
{
    Base_State *gState = button->info->dataArray[0];
    int accepted = (int)button->info->dataArray[1];

    struct menu_Game *menu = gState->info;

    char *campaignName = (char *)mem_Malloc(strlen(menu->selectedCampaign) + 1, __LINE__, __FILE__);

    strcpy(campaignName, menu->selectedCampaign);

    /*Fix the campaign name*/
    string_RemoveExt(campaignName);

    /*Revert to the main ui section*/
    uiSpine_Close(uiSpine_GetEntity(&menu->spine, "Spine:Campaign"));

    /*If the campaign chosen was accepted*/
    if(accepted == 1)
    {
        vPlayer_LoadCampaign(menu->player, campaignName);

        if(menu->player->playCampaign == 1 && menu->player->campaign != NULL)
        {
            menu->selectedCampaign = menu->player->campaign->name;
        }

        /*Repopulate the level scroll*/
        MM_RepopulateLevels(gState);
    }

    timer_StopSkip(&menu->spine.sTimer, 0, 1);
    control_Pause(500);

    mem_Free(campaignName);

    return;
}

static void MM_SaveGame(Ui_Button *button)
{
    struct menu_Game *menu = button->info->dataArray[0];

    vPlayer_Save(menu->player);

    /*Pause the control of this button for a while*/
    timer_StopSkip(&button->updateFreq, 500, 1);

    /*Add in a saved finished OK box*/
    veMenu_WindowOk(&menu->spine, "Spine:SaveBox", "Game saved.", ker_Screen_Width()/2, ker_Screen_Height()/2, -1, -1, menu->spine.layer + 1);

    return;
}

static void MM_ChooseLevel(Ui_Button *button)
{
    struct menu_Game *menu = button->info->dataArray[0];

    menu->player->levelChosen = button->info->dataArray[1];

    vLevel_LoadHeader(&menu->levelHeader, menu->player->levelChosen);

    return;
}

static Ui_Button *GM_CreateUnitButton(int unitType, int *unitChoice, int layer, Timer *srcTimer)
{
    Ui_Button *unitButton = NULL;
    unitButton = veMenu_ButtonUnit(unitType, layer, srcTimer, &BFunc_SetInt, &BFunc_SetInt, dataStruct_Create(2, unitChoice, unitType));

    return unitButton;
}


/*
    Returns a ui scroll populated with levels, either all of them or just
    the ones in a campaign.
*/
Ui_ButtonScroll *MM_CreateLevelScroll(Base_State *gState)
{
    struct menu_Game *menu = gState->info;

    DIR *levelDIR = opendir("../Levels");
    //DIR *levelDIR = opendir("../LevelsOld");
    struct list *levelList = NULL;
    struct list *currentList = NULL;
    struct dirent *level;

    char *levelName = NULL;

    int totalLevels = 0;
    int x = 0;
    Vent_CaLvl *cl = NULL;

    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    Sprite *sB = NULL;
    SDL_Surface *lNameSurface[2];

    scroll = uiButtonScroll_Create(ve_Menu.menuSecondPosition.x, 150, SCR_V, 7, 5, 1, 0,
                                   veMenu_ButtonSprite(ve_Menu.menuSecondPosition.x, 270, sprite_Copy(&ve_Sprites.scroll[D_UP], 1), &menu->timer, NULL, &MABFH_ButtonHover, NULL),
                                   veMenu_ButtonSprite(ve_Menu.menuSecondPosition.x + 20, 270, sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1), &menu->timer, NULL, &MABFH_ButtonHover, NULL),
                                  0);

    list_ClearAll(&menu->nameList);

    menu->nameList = NULL;

    if(menu->player->playCampaign == 0)
    {
        if(levelDIR == NULL)
        {
            printf("Error cannot open level directory\n");
        }

        levelList = file_Get_Extension_List(levelDIR, ".map");
        currentList = levelList;

        /*Find all the files with the .map extension and insert the names into the menu*/
        while(currentList != NULL)
        {
            totalLevels ++;

            level = currentList->data;

            levelName = (char *)mem_Malloc(strlen(level->d_name) + 1, __LINE__, __FILE__);
            strcpy(levelName, level->d_name);

            list_Push(&menu->nameList, levelName, 0);

            //printf("Level name: %s\n", levelName);

            button = veMenu_ButtonOption(levelName, NULL, font_Get(2, 13), &menu->timer, VL_HUD + 1, &MM_ChooseLevel, &MABFH_ButtonHover, dataStruct_Create(2, menu, levelName));

            uiButtonScroll_AddButton(scroll, button, 0);

            currentList = currentList->next;
        }

        list_ClearAll(&levelList);
    }
    else
    {
        /*Build scroll with level names in campaign*/
        currentList = menu->player->campaign->lvlInfo;

        x = 0;

        while(currentList != NULL && x <= menu->player->campaign->totalLevels && x <= menu->player->campaign->upToLevel)
        {
            x++;
            cl = currentList->data;

            printf("Level name: %s\n", cl->name);

            button = veMenu_ButtonOption(cl->name, NULL, font_Get(2, 13), &menu->timer, VL_HUD + 1, &MM_ChooseLevel, &MABFH_ButtonHover, dataStruct_Create(2, menu, cl->fileName));

            uiButtonScroll_AddButton(scroll, button, 0);

            currentList = currentList->next;
        }
    }

    closedir(levelDIR);

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);

    return scroll;
}


static void MM_CampaignMenuSetup(Base_State *gState)
{
    struct menu_Game *menu = gState->info;

    Ui_Spine *spineCampaign = NULL;

    SDL_Surface *buttonSurface = NULL;
    SDL_Surface *background = NULL;

    Ui_Button *acceptButton = NULL;
    Ui_Button *cancelButton = NULL;

    Sprite *bSprite = NULL;

    /*Setup the main spine for the campaign window*/
    background = surf_SimpleBox(300, 200, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    spineCampaign = uiSpine_Create(VL_HUD + 2, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), menu->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS);

    uiSpine_AddSpine(&menu->spine, spineCampaign, 0, "Spine:Campaign");

    /*Add in the header text*/
    uiSpine_AddTextBox(spineCampaign,
                       uiTextBox_CreateBase(15, 10, VL_HUD + 2, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:Header");

    uiTextBox_AddText(uiSpine_GetEntity(spineCampaign,"TextBox:Header"),
                     0, 0, "Campaign: %s", dataStruct_Create(1, &menu->selectedCampaign));

    /*Add in the accept and cancel button*/
    acceptButton = veMenu_ButtonBasic(15, 160, "Accept", &spineCampaign->sTimer, VL_HUD + 2, &MM_QuitCampaign, &MABFH_ButtonHover, dataStruct_Create(2, gState, 1));

    cancelButton = veMenu_ButtonBasic(100, 160, "Cancel", &spineCampaign->sTimer, VL_HUD + 2, &MM_QuitCampaign, &MABFH_ButtonHover, dataStruct_Create(2, gState, 0));

    uiSpine_AddButton(spineCampaign, acceptButton, "Button:Accept");
    uiSpine_AddButton(spineCampaign, cancelButton, "Button:Cancel");

    /*Find all the campaigns and put them in a scroll list*/
    uiSpine_AddScroll(spineCampaign, MM_CreateCampaignScroll(gState), "Scroll:Campaigns");

    /*Setup the button chooser */
    veMenu_SetSpineControl(spineCampaign);
    uiSpine_MapEntity(spineCampaign);

    uiSpine_SetEntityUpdate(&menu->spine, "Spine:Campaign", UIS_ENTITY_NO_UPDATE, 0);

    return;
}

void Menu_Game_Init(void *info)
{
    Base_State *gState = info;

    struct menu_Game *menu = (struct menu_Game *)mem_Malloc( sizeof(struct menu_Game), __LINE__, __FILE__);

    int x = 0;

    const int tButtons = 5;
    char **test = NULL;
    Ui_Button *button[5];
    Sprite *bSprite[5];

    Ui_ButtonScroll *mainScroll = NULL;
    Ui_ButtonScroll *unitScroll = NULL;
    SDL_Surface *background = NULL;

    menu->player = gState->info;

    menu->nameList = NULL;
    menu->campaignNameList = NULL;

    if(menu->player->campaign != NULL)
    {
        menu->selectedCampaign = menu->player->campaign->name;
    }
    else
        menu->selectedCampaign = "None";

    vLevel_SetupHeader(&menu->levelHeader, "None", 0, 0);

    gState->info = menu;

    /*Setup timer*/
    menu->timer = timer_Setup(NULL, 0, 0, 1);

    /*Setup menu spine*/
    background = surf_Copy(ve_Surfaces.mainMenu);

    pixelMan_PushLine(gKer_DrawManager(), 0, 475, 101, 475, background->h - 101, 2, colourBlack, background);


    uiSpine_Setup(&menu->spine, VL_HUD + 1, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), &ve_Menu.pointer, 0, NULL, UI_SPINE_ALLFLAGS);

    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(225, 120, VL_HUD + 1, 0, NULL, font_Get(2, 16), tColourBlack, &menu->timer),
                       "TextBox:Campaign");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:Campaign"),
                     0, 0, "Campaign: %s", dataStruct_Create(1, &menu->selectedCampaign));

    /*Create buttons*/
    button[0] = veMenu_ButtonSide(0, 0, "PLAY", &menu->spine.sTimer, &MM_StartGame, &MM_ButtonHover, dataStruct_Create(1, gState));

    button[1] = veMenu_ButtonSide(0, 0, "CAMPAIGN", &menu->spine.sTimer, &MM_SelectCampaign, &MM_ButtonHover, dataStruct_Create(1, gState));

    button[2] = veMenu_ButtonSide(0, 0, "UPGRADE", &menu->spine.sTimer, &MM_Upgrade, &MM_ButtonHover, dataStruct_Create(1, gState));

    button[3] = veMenu_ButtonSide(0, 0, "SAVE", &menu->spine.sTimer, &MM_SaveGame, &MM_ButtonHover, dataStruct_Create(1, menu));

    button[4] = veMenu_ButtonSide(0, 0, "BACK", &menu->spine.sTimer, &MM_Quit, &MM_ButtonHover, dataStruct_Create(1, gState));

    uiSpine_AddButton(&menu->spine, button[0], "Button:Play");
    uiSpine_AddButton(&menu->spine, button[1], "Button:Campaign");
    uiSpine_AddButton(&menu->spine, button[2], "Button:Upgrade");
    uiSpine_AddButton(&menu->spine, button[3], "Button:Save");
    uiSpine_AddButton(&menu->spine, button[4], "Button:Back");

    uiSpine_AlignEntities(&menu->spine, ve_Menu.buttonPosition.x, ve_Menu.buttonPosition.y, ve_Menu.buttonSpaceing, SCR_V, 5,
                          "Button:Play",
                          "Button:Campaign",
                          "Button:Upgrade",
                          "Button:Save",
                          "Button:Back");

    /*Create the list of levels to be selected and displayed*/
    uiSpine_AddScroll(&menu->spine, MM_CreateLevelScroll(gState), "Scroll:Levels");

    /*Add in the play unit scroll*/
    unitScroll = uiButtonScroll_Create(515, 305,
                                   SCR_H, ve_Menu.buttonSpaceing, 1, 1, 0,
                                   veMenu_ButtonSprite(550, 360, sprite_Copy(&ve_Sprites.scroll[D_RIGHT], 1), &menu->spine.sTimer, &BFA_ScrollSelectForwards, &MABFH_ButtonHover, NULL),
                                   veMenu_ButtonSprite(515, 360, sprite_Copy(&ve_Sprites.scroll[D_LEFT], 1), &menu->spine.sTimer, &BFA_ScrollSelectBackwards, &MABFH_ButtonHover, NULL),
                                   4,
                                   GM_CreateUnitButton(UNIT_TANK, &menu->player->startingUnit, VL_HUD + 1, &menu->timer),
                                   GM_CreateUnitButton(UNIT_INFANTRY, &menu->player->startingUnit, VL_HUD + 1, &menu->timer),
                                   GM_CreateUnitButton(UNIT_AIR, &menu->player->startingUnit, VL_HUD + 1, &menu->timer),
                                   GM_CreateUnitButton(UNIT_TOWER, &menu->player->startingUnit, VL_HUD + 1, &menu->timer)
                                   );

    uiButtonScroll_SetHoverState(unitScroll, &MABFH_HoverLine);

    uiSpine_AddScroll(&menu->spine, unitScroll, "Scroll:Unit");

    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(505, 280, VL_HUD + 1, 0, NULL, font_Get(2, 16), tColourBlack, &menu->timer),
                       "TextBox:UnitSelect");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:UnitSelect"),
                     0, 0, "Unit selected:", NULL);

    /*Setup the button chooser in the spine so that the menu can be traversed with buttons*/
    veMenu_SetSpineControl(&menu->spine);
    uiSpine_MapEntity(&menu->spine);


    /*Setup campaign select sub-menu*/
    MM_CampaignMenuSetup(gState);

    timer_Start(&menu->timer);

    baseState_SetOnActivate(gState, &Menu_Game_Activate);

    menu->prevTime = (float)timer_Get_Passed(&menu->timer, 0);

    return;
}

void Menu_Game_Activate(void *info)
{
    Base_State *gState = info;
    struct menu_Game *menu = gState->info;

    if(menu->player->campaign != NULL && menu->player->playCampaign == 1)
    {
        menu->selectedCampaign = menu->player->campaign->name;
    }
    else
        menu->selectedCampaign = "None";

    MM_RepopulateLevels(gState);

    return;
}

void Menu_Game_Controls(void *info)
{
    Base_State *gState = info;
    struct menu_Game *menu = gState->info;

    /*If the base menu is active and the back control is pressed*/
    if(menu->spine.state == SPINE_STATE_IDLE && control_IsActivated(&ve_Menu.c_Back) == 1)
    {
        uiButton_TempActive(uiSpine_GetEntity(&menu->spine, "Button:Back"), 1);
    }

    return;
}

void Menu_Game_Logic(void *info)
{
    Base_State *gState = info;
    struct menu_Game *menu = gState->info;

    menu->prevTime = (float)timer_Get_Passed(&menu->timer, 0);
    timer_Calc(&menu->timer);

    if(menu->player->playedGame > 0)
    {
        /*If the player is playing a campaign and just won a level*/
        if(menu->player->playCampaign == 1 && menu->player->playedGame == 2)
        {
            /*Reset the level scroll so that any new levels can be displayed*/
            MM_RepopulateLevels(gState);
        }

        menu->player->playedGame = 0;
    }

    /*If there is a level selected*/
    if(strcmp(menu->levelHeader.name, "None") != 0)
    {
        /*Make sure to force the unit the level requires the player to use*/
        if(menu->levelHeader.forcedPlayerUnit != VLEVEL_FORCE_ANY)
        {
            uiButtonScroll_SetOrigin(uiSpine_GetEntity(&menu->spine, "Scroll:Unit"), menu->levelHeader.forcedPlayerUnit);
        }
    }

    uiSpine_Update(&menu->spine, 0);

    return;
}

void Menu_Game_Render(void *info)
{
    Base_State *gState = info;
    struct menu_Game *menu = gState->info;

    Vent_CaLvl *caLvl = NULL;

    uiSpine_Draw(&menu->spine, ker_Screen());

    if(menu->player->playCampaign == 1)
    {
        //text_Draw_Arg(225, 100, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "Campaign: %s %p", menu->player->campaign->name, menu->player->campaign->name);
        text_Draw_Arg(225, 300, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Level progress: %d/%d", menu->player->campaign->upToLevel, menu->player->campaign->totalLevels);
        text_Draw_Arg(225, 320, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Time: %d minutes", vCampaign_GetTimePlayed(menu->player->campaign)/(1000 *60));
        text_Draw_Arg(225, 340, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Score: %d", vCampaign_GetScore(menu->player->campaign));

        caLvl = vCampaign_GetLvl(menu->player->campaign, menu->player->levelChosen);

        if(caLvl != NULL)
        {
            text_Draw_Arg(505, 120, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "%s", caLvl->name);

            text_Draw_Arg(520, 150, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Won: %d Lost: %d", caLvl->timesWon, caLvl->timesPlayed - caLvl->timesWon);
            //text_Draw_Arg(520, 170, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Completed: %d", caLvl->hasCompleted);
            text_Draw_Arg(520, 170, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Time: %d minutes", caLvl->playDuration/(1000 * 60));
            text_Draw_Arg(520, 190, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Reward: $%d", caLvl->moneyReward);

            if(caLvl->hasCompleted > 0)
            {
                text_Draw_Arg(520, 210, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Quickest Time: %d:%d", (caLvl->quickestDuration/(1000 *60)), ((caLvl->quickestDuration/1000) % 60));
                text_Draw_Arg(520, 230, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Least Health Lost: %d", caLvl->leastHealthLost);
                text_Draw_Arg(520, 250, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Best score: %d", caLvl->score);

                surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_HUD + 1, 0, 710, 120, ve_Surfaces.badge, ker_Screen(), NULL);

            }
        }

        text_Draw_Arg(494 - 37, 480 - 45, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Credits $%d", menu->player->campaign->cash);
    }
    else
    {
        //text_Draw_Arg(225, 120, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "Campaign: %s", "None");
        text_Draw_Arg(505, 120, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "%s", menu->levelHeader.name);
    }

    text_Draw_Arg(494 - 15, 480 - 65, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "%s", menu->player->pName);

    return;
}

void Menu_Game_Exit(void *info)
{
    Base_State *gState = info;
    struct menu_Game *menu = gState->info;

    while(menu->nameList != NULL)
    {
        mem_Free(menu->nameList->data);
        list_Pop(&menu->nameList);
    }

    while(menu->campaignNameList != NULL)
    {
        mem_Free(menu->campaignNameList->data);
        list_Pop(&menu->campaignNameList);
    }

    /*Clear up ui*/
    uiSpine_Clean(&menu->spine);

    /*Remove any graphics that should not be drawn*/
    gKer_FlushGraphics();

    mem_Free(menu);

    return;
}
