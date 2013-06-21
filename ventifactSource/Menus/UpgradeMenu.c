#include "UpgradeMenu.h"

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "GUI/ButtonFunc.h"
#include "GUI/Spine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Effects/Blink.h"

#include "MenuAttributes.h"

#include "../Player.h"
#include "../Images.h"
#include "../Defines.h"
#include "../Units/Attributes.h"

enum upgradeTypes
{
    UPGRADE_PLAYER = 0,
    UPGRADE_TANK = 1,
    UPGRADE_INFANTRY = 2,
    UPGRADE_AIR = 3,
    UPGRADE_TOWER = 4
};

static char *upgradeNames[5] = {"Player", "Tank", "Infantry", "Aircraft", "Tower"};
static int upgradeCosts[5] = {100, 200, 300, 400, 500};

struct menu_Upgrade
{
    Ui_Spine spine;

    int chosenUpgrade;
    char *upgradeName;
    char upgradeText[256];
    char upgradeTotalText[256];

    SDL_Surface *upgradeBox[2];

    Vent_Player *player;

    Timer timer;
};

static void MM_ForceScrollActive(Ui_Button *button)
{
    int x = 0;
    struct menu_Upgrade *menu = button->info->dataArray[0];

    Ui_ButtonScroll *scroll = uiSpine_GetEntity(&menu->spine, "Scroll:Upgrades");

    /*Make the upgrade button hover as normal*/
    MABFH_ButtonHover(button);

    return;
}

static void BFH_ScrollLine(Ui_Button *button)
{
    draw_Line(button->rect.x - 3, button->rect.y + 1, button->rect.x - 3, button->rect.y + button->rect.h - 1, 2, 0, &colourBlack, ker_Screen());

    return;
}

static Vent_Attributes *MM_GetAttributes(struct menu_Upgrade *menu, int **upgradeAmount)
{
    switch(menu->chosenUpgrade)
    {
        case UPGRADE_PLAYER:
        *upgradeAmount = &menu->player->campaign->playerLevel;

        if(**upgradeAmount < 5)
            return &playerAttributes[**upgradeAmount];

        break;

        case UPGRADE_TANK:
        *upgradeAmount = &menu->player->campaign->unitLevel[UNIT_TANK];

        if(**upgradeAmount < 5)
            return &unitAttributes[UNIT_TANK][**upgradeAmount];

        break;

        case UPGRADE_INFANTRY:
        *upgradeAmount = &menu->player->campaign->unitLevel[UNIT_INFANTRY];

        if(**upgradeAmount < 5)
            return &unitAttributes[UNIT_INFANTRY][**upgradeAmount];

        break;

        case UPGRADE_AIR:
        *upgradeAmount = &menu->player->campaign->unitLevel[UNIT_AIR];

        if(**upgradeAmount < 5)
            return &unitAttributes[UNIT_AIR][**upgradeAmount];

        break;

        case UPGRADE_TOWER:
        *upgradeAmount = &menu->player->campaign->unitLevel[UNIT_TOWER];

        if(**upgradeAmount < 5)
            return &unitAttributes[UNIT_TOWER][**upgradeAmount];
        break;
    }

    return NULL;
}

static int MM_GetSummedAttributes(struct menu_Upgrade *menu, Vent_Attributes *sumAttribute)
{
    int x = 0;
    int i = 0;
    int upgradeLvl = 0;
    Vent_Attributes *attributes = NULL;

    *sumAttribute = noAttributes;

    switch(menu->chosenUpgrade)
    {
        case UPGRADE_PLAYER:
        upgradeLvl = menu->player->campaign->playerLevel;
        attributes = &playerAttributes[0];
        break;

        case UPGRADE_TANK:
        upgradeLvl = menu->player->campaign->unitLevel[UNIT_TANK];
        attributes = &unitAttributes[UNIT_TANK][0];
        break;

        case UPGRADE_INFANTRY:
        upgradeLvl = menu->player->campaign->unitLevel[UNIT_INFANTRY];
        attributes = &unitAttributes[UNIT_INFANTRY][0];
        break;

        case UPGRADE_AIR:
        upgradeLvl = menu->player->campaign->unitLevel[UNIT_AIR];
        attributes = &unitAttributes[UNIT_AIR][0];
        break;

        case UPGRADE_TOWER:
        upgradeLvl = menu->player->campaign->unitLevel[UNIT_TOWER];
        attributes = &unitAttributes[UNIT_TOWER][0];
        break;
    }

    for(i = 0; i < upgradeLvl; i++)
    {
        if(i == 5)
            break;

        vAttributes_Add(sumAttribute, attributes);
        attributes ++;
    }

    return i;
}

static void MM_ChangeUpgradeText(struct menu_Upgrade *menu)
{
    Vent_Attributes *attribute = NULL;
    Vent_Attributes sumAttribute = noAttributes;
    int *upgradeAmount = NULL;
    int x = 0;

    Ui_TextBox *upgradeTotalText = uiSpine_GetEntity(&menu->spine, "TextBox:UpgradeTotal");
    Ui_TextBox *upgradeText = uiSpine_GetEntity(&menu->spine, "TextBox:Upgrade");

    attribute = MM_GetAttributes(menu, &upgradeAmount);

    if(attribute != NULL)
    {
        vAttributes_WriteDifference(menu->upgradeText, attribute);
        uiTextBox_ChangeText(upgradeText, 0, 0, menu->upgradeText, NULL);
    }
    else
    {
        uiTextBox_ChangeText(upgradeText, 0, 0, "No upgrade", NULL);
    }

    if(*upgradeAmount > 0)
    {
        x = MM_GetSummedAttributes(menu, &sumAttribute);

        vAttributes_WriteDifference(menu->upgradeTotalText, &sumAttribute);
        //printf("Total text: %s, numWritten %d\n", menu->upgradeTotalText, x);
        uiTextBox_ChangeText(upgradeTotalText, 0, 0, menu->upgradeTotalText, NULL);
    }
    else
    {
        uiTextBox_ChangeText(upgradeTotalText, 0, 0, "None purchased", NULL);
    }

    return;
}

static void MM_ChangeUpgrade(Ui_Button *button)
{
    struct menu_Upgrade *menu = button->info->dataArray[0];
    int upgradeType = (int)button->info->dataArray[1];

    menu->chosenUpgrade = upgradeType;

    MM_ChangeUpgradeText(menu);

    return;
}

static void MM_BuyUpgrade(Ui_Button *button)
{
    struct menu_Upgrade *menu = button->info->dataArray[0];
    Vent_Player *player = menu->player;
    int *upgradeAmount = NULL;

    Ui_TextBox *upgradeText = NULL;
    Vent_Attributes *attribute = NULL;

    attribute = MM_GetAttributes(menu, &upgradeAmount);

    if(*upgradeAmount >= 5)
    {
        veMenu_WindowOk(&menu->spine, "Spine:MaxUpgrade", "Can't upgrade further.", ker_Screen_Width()/2, ker_Screen_Height()/2, 240, 60, menu->spine.layer + 1);

        return;
    }
    else if(player->campaign->cash < upgradeCosts[*upgradeAmount])
    {
        veMenu_WindowOk(&menu->spine, "Spine:CostUpgrade", "Can't afford this upgrade.", ker_Screen_Width()/2, ker_Screen_Height()/2, 240, 60, menu->spine.layer + 1);

        return;
    }

    /*Upgrade bought*/
    player->campaign->cash -= upgradeCosts[*upgradeAmount];

    *upgradeAmount += 1;

    MM_ChangeUpgradeText(menu);

    return;
}

static void Menu_Upgrade_SetupControls(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = gState->info;

    Ui_ButtonScroll *scroll = NULL;

    /*Add in the controls to move the upgrade choice up and down*/
    scroll = uiSpine_GetEntity(&menu->spine, "Scroll:Upgrades");

    uiButton_CopyControl(scroll->traverseForwards, &ve_Menu.c_MoveDown, BC_ALL);
    uiButton_CopyControl(scroll->traverseBackwards, &ve_Menu.c_MoveUp, BC_ALL);

    veMenu_SetSpineControl(&menu->spine);
    uiSpine_MapEntity(&menu->spine);

    return;
}

void Menu_Upgrade_Init(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = (struct menu_Upgrade *)mem_Malloc( sizeof(struct menu_Upgrade), __LINE__, __FILE__);

    SDL_Surface *background = NULL;

    const int tButtons = 2;
    Ui_Button *button[2];

    Ui_ButtonScroll *scroll = NULL;

    menu->player = gState->info;

    gState->info = menu;

    menu->chosenUpgrade = UPGRADE_PLAYER;
    menu->upgradeName = upgradeNames[menu->chosenUpgrade];

    /*Setup timer*/
    menu->timer = timer_Setup(NULL, 0, 0, 1);

    /*Setup menu spine*/
    background = surf_Copy(ve_Surfaces.mainMenu);

    menu->upgradeBox[0] = surf_SimpleBox(15, 15, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);
    menu->upgradeBox[1] = surf_SimpleBox(13, 13, &colourYellow, &colourBlack, 1);
    SDL_SetAlpha(menu->upgradeBox[1], SDL_SRCALPHA | SDL_RLEACCEL, SDL_FAST_ALPHA);

    uiSpine_Setup(&menu->spine, VL_HUD + 1, ve_Menu.menuPosition.x, ve_Menu.menuPosition.y, frame_CreateBasic(0, background, A_FREE), &ve_Menu.pointer, 0, NULL, UI_SPINE_ALLFLAGS);

    /*Create buttons*/

    /*Back button*/
    button[0] = veMenu_ButtonSide(ve_Menu.buttonPosition.x, ve_Menu.buttonPosition.y + 4 * (ve_Menu.buttonSpaceing + ve_Menu.buttonHeight), "BACK", &menu->timer, &MABFA_Quit, &MABFH_ButtonHover, dataStruct_Create(1, gState));

    /*Buy button*/
    button[1] = veMenu_ButtonBasic(ve_Menu.menuSecondPosition.x - 20, 240, "UPGRADE", &menu->timer, VL_HUD + 1, &MM_BuyUpgrade, &MM_ForceScrollActive, dataStruct_Create(1, menu));

    uiSpine_AddButton(&menu->spine, button[0], "Button:Back");
    uiSpine_AddButton(&menu->spine, button[1], "Button:Buy");

    /*Upgrades scroll*/
    button[0] = veMenu_ButtonUnit(UNIT_TANK, VL_HUD + 1, &menu->timer, &MM_ChangeUpgrade, NULL, dataStruct_Create(2, menu, UPGRADE_PLAYER));
    scroll = uiButtonScroll_Create(ve_Menu.menuSecondPosition.x, 170,
                                       SCR_V, 10, 1, 1, 0,
                                       veMenu_ButtonSprite(ve_Menu.menuSecondPosition.x - 20, 190, sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1), &menu->timer, &BFA_ScrollSelectForwards, &MABFH_ButtonHover, NULL),
                                       veMenu_ButtonSprite(ve_Menu.menuSecondPosition.x - 20, 170, sprite_Copy(&ve_Sprites.scroll[D_UP], 1), &menu->timer, &BFA_ScrollSelectBackwards, &MABFH_ButtonHover, NULL),
                                       5,
                                       button[0],
                                       veMenu_ButtonUnit(UNIT_INFANTRY, VL_HUD + 1, &menu->timer, &MM_ChangeUpgrade, NULL, dataStruct_Create(2, menu, UPGRADE_INFANTRY)),
                                       veMenu_ButtonUnit(UNIT_AIR, VL_HUD + 1, &menu->timer, &MM_ChangeUpgrade, NULL, dataStruct_Create(2, menu, UPGRADE_AIR)),
                                       veMenu_ButtonUnit(UNIT_TOWER, VL_HUD + 1, &menu->timer, &MM_ChangeUpgrade, NULL, dataStruct_Create(2, menu, UPGRADE_TOWER)),
                                       veMenu_ButtonUnit(UNIT_TANK, VL_HUD + 1, &menu->timer, &MM_ChangeUpgrade, NULL, dataStruct_Create(2, menu, UPGRADE_TANK))
    );

    uiButtonScroll_SetHoverState(scroll, &BFH_ScrollLine);
    uiButtonScroll_ForceSelect(scroll, 0);
    uiButtonScroll_ActivateOnTraverse(scroll, 1);

    uiButton_TempActive(uiButtonScroll_GetButton(scroll, 0), 1);

    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Upgrades");

    /*Add in the main header text*/
    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(225, 120, VL_HUD + 1, 0, NULL, font_Get(2, 16), tColourBlack, &menu->timer),
                       "TextBox:Header");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:Header"),
                     0, 0, "Upgrade: %s", dataStruct_Create(1, &menu->upgradeName));

    /*Add in the upgrade header text*/
    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(450, 150, VL_HUD + 1, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:UpgradeTotalHeader");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:UpgradeTotalHeader"),
                     0, 0, "Total:", NULL);

    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(330, 150, VL_HUD + 1, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:UpgradeNextHeader");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:UpgradeNextHeader"),
                     0, 0, "Next:", NULL);

    /*Add in the upgrade details text*/
    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(330, 170, VL_HUD + 1, 0, NULL, font_Get(2, 13), tColourGreen, &menu->timer),
                       "TextBox:Upgrade");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:Upgrade"),
                     0, 0, "Bought", NULL);

    /*Add in the upgrade details text*/
    uiSpine_AddTextBox(&menu->spine,
                       uiTextBox_CreateBase(450, 170, VL_HUD + 1, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:UpgradeTotal");

    uiTextBox_AddText(uiSpine_GetEntity(&menu->spine,"TextBox:UpgradeTotal"),
                     0, 0, "Total Upgrade", NULL);

    Menu_Upgrade_SetupControls(info);

    baseState_SetOnActivate(gState, &Menu_Upgrade_Activate);

    timer_Start(&menu->timer);

    return;
}

void Menu_Upgrade_Activate(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = gState->info;

    MM_ChangeUpgradeText(menu);

    return;
}

void Menu_Upgrade_Controls(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = gState->info;

    /*If the base menu is active and the back control is pressed*/
    if(menu->spine.state == SPINE_STATE_IDLE && control_IsActivated(&ve_Menu.c_Back) == 1)
    {
        uiButton_TempActive(uiSpine_GetEntity(&menu->spine, "Button:Back"), 1);
    }

    return;
}

void Menu_Upgrade_Logic(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = gState->info;

    timer_Calc(&menu->timer);

    menu->upgradeName = upgradeNames[menu->chosenUpgrade];

    uiSpine_Update(&menu->spine, 0);

    return;
}

void Menu_Upgrade_Render(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = gState->info;

    int x = 0;
    int upgradeAmount = 0;

    uiSpine_Draw(&menu->spine, ker_Screen());

    switch(menu->chosenUpgrade)
    {
        case UPGRADE_PLAYER:
        upgradeAmount = menu->player->campaign->playerLevel;
        break;

        case UPGRADE_TANK:
        upgradeAmount = menu->player->campaign->unitLevel[UNIT_TANK];
        break;

        case UPGRADE_INFANTRY:
        upgradeAmount = menu->player->campaign->unitLevel[UNIT_INFANTRY];
        break;

        case UPGRADE_AIR:
        upgradeAmount = menu->player->campaign->unitLevel[UNIT_AIR];
        break;

        case UPGRADE_TOWER:
        upgradeAmount = menu->player->campaign->unitLevel[UNIT_TOWER];
        break;
    }

    text_Draw_Arg(494 - 15, 480 - 65, ker_Screen(), font_Get(2, 16), &tColourBlack, VL_HUD + 1, 0, "%s", menu->player->pName);

    text_Draw_Arg(494 - 37, 480 - 45, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "Money $%d", menu->player->campaign->cash);

    if(upgradeAmount < 5)
    {
        text_Draw_Arg(ve_Menu.menuSecondPosition.x + 10, 221, ker_Screen(), font_Get(2, 13), &ve_Menu.subTextColour, VL_HUD + 1, 0, "$%d", upgradeCosts[upgradeAmount]);
    }

    for(x = 0; x < 5; x++)
    {
        surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_HUD + 1, 0, ve_Menu.menuSecondPosition.x - 20 + (x * 20), 270, menu->upgradeBox[0], ker_Screen(), NULL);

        if(upgradeAmount > x)
        {
            surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_HUD + 1, 0, ve_Menu.menuSecondPosition.x - 19 + (x * 20), 271, menu->upgradeBox[1], ker_Screen(), NULL);
        }
    }

    return;
}

void Menu_Upgrade_Exit(void *info)
{
    Base_State *gState = info;
    struct menu_Upgrade *menu = gState->info;

    /*Remove any graphics that should not be drawn*/
    gKer_FlushGraphics();

    /*Clear up ui*/

    SDL_FreeSurface(menu->upgradeBox[0]);
    SDL_FreeSurface(menu->upgradeBox[1]);

    uiSpine_Clean(&menu->spine);

    mem_Free(menu);

    return;
}
