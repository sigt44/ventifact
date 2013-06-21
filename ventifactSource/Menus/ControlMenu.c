#include "ControlMenu.h"

#include "OptionsMenu.h"

#include "Kernel/Kernel_State.h"
#include "Graphics/PixelManager.h"
#include "GUI/ButtonFunc.h"
#include "GUI/Spine.h"
#include "Flag.h"

#include "MenuAttributes.h"
#include "../Images.h"
#include "../Defines.h"
#include "../ControlMap.h"

struct menu_Controls
{
    Ui_Spine spine;

    Control_Event *selectedEvent;
    char *controlName;

    Timer timer;
};

/*Button functions*/
static void CM_DrawControlInfo(Ui_Button *button)
{
    Control_Event *control = button->info->dataArray[0];
    Control_Keys *cKeys = NULL;
    int *keyIndex = NULL;
    struct list *keyList = NULL;
    int x = 0;
    int *y = 0;

    int xValue = 0;

    int yGlobal = ve_Menu.menuSecondPosition.y + 5;
    int yValueKey = yGlobal - 40;
    int yValueJoy = yGlobal - 40;
    int yValueMouse = yGlobal - 40;

    int yDraw = 0;
    char keyName[128];

    MABFH_ButtonHover(button);

    y = 0;
    keyList = control->eventKeys;
    while(keyList != NULL)
    {
        cKeys = keyList->data;
        keyIndex = cKeys->keys;

        switch(cKeys->pollType)
        {
            case C_KEYBOARD:
            xValue = 450;
            yValueKey += 40;
            y = &yValueKey;
            break;

            case C_JOYSTICK:
            xValue = 550;
            yValueJoy += 40;
            y = &yValueJoy;
            break;

            case C_MOUSE:
            xValue = 650;
            yValueMouse += 40;
            y = &yValueMouse;
            break;
        }

        for(x = 0; x < keyList->info; x++)
        {
            if(cKeys->pollType == C_KEYBOARD)
                strncpy(keyName, SDL_GetKeyName(keyIndex[x]), 127);
            else
                snprintf(keyName, 127, "%d", keyIndex[x]);

            yDraw = (20 * x) + *y;

            text_Draw_Arg(xValue, yDraw, ker_Screen(), ve_Menu.mainFont[FS_MEDIUM], &tColourBlack, VL_HUD + 2, 0, "%s", keyName);
        }

        pixelMan_PushLine(gKer_DrawManager(), VL_HUD + 2, xValue, yDraw + 20, xValue + 50, yDraw + 20, 2, colourBlack, ker_Screen());
        *y = yDraw;

        keyList = keyList->next;
    }

    return;
}

static void CM_SetPollMenu(Ui_Button *button)
{
    Control_Event *control = button->info->dataArray[0];
    struct menu_Controls *menu = button->info->dataArray[1];

    menu->selectedEvent = control;
    menu->controlName = (char *)control->name;

    /*Toggle updating of main components in the base menu*/
    uiSpine_Open(uiSpine_GetEntity(&menu->spine, "Spine:PollKey"), &menu->spine);

    return;
}

static void CM_ExitPollKey(Ui_Button *button)
{
    struct menu_Controls *menu = button->info->dataArray[0];

    /*Disable the waiting for keys message and the spine*/
    uiSpine_SetEntityUpdate(uiSpine_GetEntity(&menu->spine, "Spine:PollKey"), "TextBox:Key", 0, 0);

    uiSpine_Close(uiSpine_GetEntity(&menu->spine, "Spine:PollKey"));

    return;
}

static void CM_PollControl(Ui_Button *button)
{
    struct menu_Controls *menu = button->info->dataArray[0];
    int replace = (int)button->info->dataArray[1];
    struct list *eventList = NULL;
    struct list *eventTrav = NULL;
    struct list *keyList = NULL;

    int keyType = 0;
    int *key = 0;

    int eventType = C_BUTTON_DOWN;
    Control_Keys *cKey = NULL;

    SDL_Event *event = NULL;

    Ui_Spine *pollKey = uiSpine_GetEntity(&menu->spine, "Spine:PollKey");
    Ui_Spine_Entity *textEntity = uiSpine_GetEntityBase(pollKey, "TextBox:Key");

    /*First make sure that the status message is being displayed*/
    if(textEntity->draw == 0)
    {
        /*Set the status message to be drawn*/
        uiSpine_SetEntityUpdate(pollKey, "TextBox:Key", 0, 1);

        /*Force this function to be called again*/
        button->forceActivation ++;

        return;
    }

    /*Wait until a button has been pressed*/
    do
    {
        control_Update();
    }
    while(control_GetEventType(C_BUTTON_DOWN) == NULL);

    /*Grab all the buttons pressed for a short time*/
    eventList = control_TakeEvents(C_BUTTON_DOWN, 500);

    /*Get the event type of one of the keys in the control*/
    if(menu->selectedEvent->eventKeys != NULL)
    {
        cKey = menu->selectedEvent->eventKeys->data;
        eventType = cKey->eventType;
    }

    /*Remove all the keys for the control, if required*/
    if(replace == 1)
        control_Clear(menu->selectedEvent);

    /*Filter the buttons*/
    /*Only push in the buttons of the first known poll type*/
    if(eventList != NULL)
    {
        keyType = eventList->info;
    }

    eventTrav = eventList;
    while(eventTrav != NULL)
    {
        event = eventTrav->data;

        if(eventTrav->info == keyType)
        {
            key = (int *)mem_Malloc(sizeof(int), __LINE__, __FILE__);

            *key = control_GetKeyValue(event);

            list_Stack(&keyList, key, 0);
        }

        eventTrav = eventTrav->next;
    }

    /*If the previous key linked to the control required keystates then use it for these new keys also*/
    if(eventType == C_BUTTON_STATES)
    {
        control_AddKeyList(menu->selectedEvent, control_GetPollType(keyType), C_BUTTON_STATES, keyList);
    }
    else
    {
        control_AddKeyList(menu->selectedEvent, control_GetPollType(keyType), keyType, keyList);
    }

    list_ClearAll(&eventList);
    list_ClearAll(&keyList);

    CM_ExitPollKey(button);

    return;
}

static Ui_Button *CM_ButtonControlOption(char *name, Control_Event *control, struct menu_Controls *menu)
{
    return veMenu_ButtonOption((char *)name, NULL, font_Get(2, 13), &menu->timer, VL_HUD + 1, &CM_SetPollMenu, &CM_DrawControlInfo, dataStruct_Create(2, control, menu));
}

static void CM_PollKeySetup(Base_State *gState)
{
    struct menu_Controls *menu = gState->info;

    SDL_Surface *background = NULL;
    Ui_Spine *pollKey = NULL;

    Ui_Button *addButton = NULL;
    Ui_Button *replaceButton = NULL;
    Ui_Button *cancelButton = NULL;

    /*Create the simple box background*/
    background = surf_SimpleBox(200, 80, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Set the spine to be in the middle of the screen*/
    pollKey = uiSpine_Create(VL_HUD + 2, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), menu->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS);

    uiSpine_AddSpine(&menu->spine, pollKey, 0, "Spine:PollKey");

    /*Add in the header text*/
    uiSpine_AddTextBox(pollKey,
                       uiTextBox_CreateBase(15, 7, VL_HUD + 2, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:Header");

    uiTextBox_AddText(uiSpine_GetEntity(pollKey, "TextBox:Header"),
                     0, 0, "Control: %s", dataStruct_Create(1, &menu->controlName));

    /*Add in the key checking text*/
    uiSpine_AddTextBox(pollKey,
                       uiTextBox_CreateBase(15, 27, VL_HUD + 2, 0, NULL, font_Get(2, 13), tColourBlack, &menu->timer),
                       "TextBox:Key");

    uiTextBox_AddText(uiSpine_GetEntity(pollKey, "TextBox:Key"),
                     0, 0, "Please press new key/s...", dataStruct_Create(1, &menu->controlName));

    uiSpine_SetEntityUpdate(pollKey, "TextBox:Key", 0, 0);

    /*Add in the 'Add' button*/
    addButton = veMenu_ButtonBasic(15, 50, "Add", &menu->timer, VL_HUD + 2, &CM_PollControl, &MABFH_ButtonHover, dataStruct_Create(2, menu, 0));

    /*Add in the 'Replace' button*/
    replaceButton = veMenu_ButtonBasic(60, 50, "Replace", &menu->timer, VL_HUD + 2, &CM_PollControl, &MABFH_ButtonHover, dataStruct_Create(2, menu, 1));

    /*Add in the 'Cancel' button*/
    cancelButton = veMenu_ButtonBasic(135, 50, "Cancel", &menu->timer, VL_HUD + 2, &CM_ExitPollKey, &MABFH_ButtonHover, dataStruct_Create(1, menu));

    uiSpine_AddButton(pollKey, cancelButton, "Button:Cancel");
    uiSpine_AddButton(pollKey, replaceButton, "Button:Replace");
    uiSpine_AddButton(pollKey, addButton, "Button:Add");

    veMenu_SetSpineControl(pollKey);
    uiSpine_MapEntity(pollKey);


    uiSpine_SetEntityUpdate(&menu->spine, "Spine:PollKey", UIS_ENTITY_NO_UPDATE, 0);

    return;
}


static void Menu_Controls_SetupControls(void *info)
{
    Base_State *gState = info;
    struct menu_Controls *menu = gState->info;

    veMenu_SetSpineControl(&menu->spine);
    uiSpine_MapEntity(&menu->spine);

    return;
}

void Menu_Controls_Init(void *info)
{
    Base_State *gState = info;
    struct menu_Controls *menu = (struct menu_Controls *)mem_Malloc( sizeof(struct menu_Controls), __LINE__, __FILE__);

    SDL_Surface *background = NULL;

    int x = 0;
    struct list *controlList = ve_Controls.controlList;
    Control_Event *cEvent = NULL;

    const int tButtons = 1;
    Ui_Button *button[1];

    Ui_ButtonScroll *scroll = NULL;

    gState->info = menu;

    /*Setup timer*/
    menu->timer = timer_Setup(NULL, 0, 0, 1);

    /*Setup menu spine*/
    background = surf_Copy(ve_Surfaces.mainMenu);

    uiSpine_Setup(&menu->spine, VL_HUD + 1, ve_Menu.menuPosition.x, ve_Menu.menuPosition.y, frame_CreateBasic(0, background, A_FREE), &ve_Menu.pointer, 0, NULL, UI_SPINE_ALLFLAGS);

    /*Create buttons*/

    /*Back button*/
    button[0] = veMenu_ButtonSide(ve_Menu.buttonPosition.x, ve_Menu.buttonPosition.y + 4 * (ve_Menu.buttonSpaceing + ve_Menu.buttonHeight),  "BACK", &menu->timer, &MABFA_Quit, &MABFH_ButtonHover, dataStruct_Create(1, gState));

    uiSpine_AddButton(&menu->spine, button[0], "Button:Back");

    /*Control options*/
    scroll = uiButtonScroll_Create(ve_Menu.menuSecondPosition.x, ve_Menu.menuSecondPosition.y + 5,
                                       SCR_V, 10, 7, 1, 0,
                                       veMenu_ButtonSprite(ve_Menu.menuSecondPosition.x + 100, ve_Menu.menuSecondPosition.y - 20, sprite_Copy(&ve_Sprites.scroll[D_DOWN], 1), &menu->timer, NULL, &MABFH_ButtonHover, NULL),
                                       veMenu_ButtonSprite(ve_Menu.menuSecondPosition.x + 120, ve_Menu.menuSecondPosition.y - 20, sprite_Copy(&ve_Sprites.scroll[D_UP], 1), &menu->timer, NULL, &MABFH_ButtonHover, NULL),
                                       0

    );

    while(controlList != NULL)
    {
        if(controlList->info == CONTROL_CHANGE)
        {
            cEvent = controlList->data;
            uiButtonScroll_AddButton(scroll, CM_ButtonControlOption((char *)cEvent->name, cEvent, menu), 0);
        }

        controlList = controlList->next;
    }

    uiButtonScroll_SetHoverState(scroll, &MABFH_HoverLine);

    uiSpine_AddScroll(&menu->spine, scroll, "Scroll:Controls");

    menu->selectedEvent = NULL;
    menu->controlName = "None";

    /*Setup the poll key menu*/
    CM_PollKeySetup(gState);

    Menu_Controls_SetupControls(info);

    timer_Start(&menu->timer);

    return;
}

void Menu_Controls_Controls(void *info)
{
    Base_State *gState = info;
    struct menu_Controls *menu = gState->info;

    /*If the base menu is active and the back control is pressed*/
    if(menu->spine.state == SPINE_STATE_IDLE && control_IsActivated(&ve_Menu.c_Back) == 1)
    {
        uiButton_TempActive(uiSpine_GetEntity(&menu->spine, "Button:Back"), 1);
    }

    return;
}

void Menu_Controls_Logic(void *info)
{
    Base_State *gState = info;
    struct menu_Controls *menu = gState->info;

    timer_Calc(&menu->timer);

    uiSpine_Update(&menu->spine, 0);

    return;
}

void Menu_Controls_Render(void *info)
{
    Base_State *gState = info;
    struct menu_Controls *menu = gState->info;

    uiSpine_Draw(&menu->spine, ker_Screen());

    text_Draw_Arg(ve_Menu.menuSecondPosition.x - 15, ve_Menu.menuSecondPosition.y - 25, ker_Screen(), ve_Menu.mainFont[FS_LARGE], &tColourBlack, VL_HUD + 1, 0, "Controls:");
    text_Draw_Arg(450, ve_Menu.menuSecondPosition.y - 15, ker_Screen(), ve_Menu.mainFont[FS_MEDIUM], &tColourBlack, VL_HUD + 1, 0, "Keyboard");
    text_Draw_Arg(550, ve_Menu.menuSecondPosition.y - 15, ker_Screen(), ve_Menu.mainFont[FS_MEDIUM], &tColourBlack, VL_HUD + 1, 0, "Joystick");
    text_Draw_Arg(650, ve_Menu.menuSecondPosition.y - 15, ker_Screen(), ve_Menu.mainFont[FS_MEDIUM], &tColourBlack, VL_HUD + 1, 0, "Mouse");
}

void Menu_Controls_Exit(void *info)
{
    Base_State *gState = info;
    struct menu_Controls *menu = gState->info;

    /*Remove any graphics that should not be drawn*/
    gKer_FlushGraphics();

    /*Clear up ui*/
    uiSpine_Clean(&menu->spine);

    mem_Free(menu);

    return;
}
