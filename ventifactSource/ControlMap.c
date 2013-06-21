#include "ControlMap.h"

struct Ve_Controls ve_Controls;

int vControl_Setup(Control_Event *controlEvent, char *name, Timer *srcTimer, unsigned int repeatDelay, int allowChange)
{
    control_Setup(controlEvent, name, srcTimer, repeatDelay);

    list_Stack(&ve_Controls.controlList, controlEvent, allowChange);

    if(allowChange > 0)
    {
        ve_Controls.numControlsChangeable ++;
    }

    ve_Controls.numControls ++;

    return 0;
}

Control_Event *vControl_Find(char *controlName)
{
    struct list *controlList = ve_Controls.controlList;
    Control_Event *cEvent = NULL;

    while(controlList != NULL)
    {
        cEvent = controlList->data;

        if(strcmp(cEvent->name, controlName) == 0)
        {
            return cEvent;
        }

        controlList = controlList->next;
    }

    return NULL;
}

void vControls_Init(void)
{
    int x = 0;
    char *controlName = NULL;
    ve_Controls.controlList = NULL;
    ve_Controls.numControls = 0;
    ve_Controls.numControlsChangeable = 0;

    vControl_Setup(&ve_Controls.cRight, "Right", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cLeft, "Left", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cUp, "Up", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cDown, "Down", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cUpRight, "Up right", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cUpLeft, "Up left", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cDownLeft, "Down left", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cDownRight, "Down right", NULL, 0, CONTROL_LOCKED);

    vControl_Setup(&ve_Controls.cMenuForward, "Menu forward", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cMenuBackward, "Menu backward", NULL, 0, CONTROL_LOCKED);

    vControl_Setup(&ve_Controls.cMoveLeft, "Move left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveRight, "Move right", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveUp, "Move up", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveDown, "Move down", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveUpRight, "Move up right", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveUpLeft, "Move up left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveDownLeft, "Move down left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveDownRight, "Move down right", NULL, 0, CONTROL_CHANGE);

    vControl_Setup(&ve_Controls.cShootLeft, "Shoot left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootRight, "Shoot right", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootUp, "Shoot up", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootDown, "Shoot down", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootUpRight, "Shoot up right", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootUpLeft, "Shoot up left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootDownLeft, "Shoot down left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cShootDownRight, "Shoot down right", NULL, 0, CONTROL_CHANGE);

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        switch(x)
        {
            default: controlName = "Buy unknown unit"; break;
            case UNIT_TANK: controlName = "Buy tank"; break;
            case UNIT_INFANTRY: controlName = "Buy infantry"; break;
            case UNIT_AIR: controlName = "Buy aircraft"; break;
            case UNIT_TOWER: controlName = "Buy tower"; break;
        }

        vControl_Setup(&ve_Controls.cBuyUnit[x], controlName, NULL, 0, CONTROL_CHANGE);
    }

    vControl_Setup(&ve_Controls.cConfirm, "Confirm", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cPointerConfirm, "Pointer Confirm", NULL, 0, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cBack, "Back", NULL, 500, CONTROL_LOCKED);
    vControl_Setup(&ve_Controls.cMouseHold, "Left mouse hold", NULL, 0, CONTROL_LOCKED);

    vControl_Setup(&ve_Controls.cBuy, "Buy", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cCall, "Call units", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cPause, "Pause", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cCycleWeapon, "Cycle weapon", NULL, 0, CONTROL_CHANGE);

    vControl_Setup(&ve_Controls.cMoveCameraLeft, "Camera left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraRight, "Camera right", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraUp, "Camera up", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraDown, "Camera down", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraUpRight, "Camera up right", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraUpLeft, "Camera up left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraDownLeft, "Camera down left", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cMoveCameraDownRight, "Camera down right", NULL, 0, CONTROL_CHANGE);

    vControl_Setup(&ve_Controls.cTileObjects, "Editor tile objects", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cGridObjects, "Editor grid objects", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cNudgePointer, "Editor nudge pointer", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cRotateObject, "Editor rotate tile", NULL, 0, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cEditorSave, "Editor save", NULL, 1000, CONTROL_CHANGE);
    vControl_Setup(&ve_Controls.cEditorHelp, "Editor help", NULL, 0, CONTROL_LOCKED);

    return;
}

void vControls_Clean(void)
{
    int x = 0;

    while(ve_Controls.controlList != NULL)
    {
        control_Clean(ve_Controls.controlList->data);

        if(ve_Controls.controlList->info == CONTROL_CHANGE)
        {
            ve_Controls.numControlsChangeable --;
        }

        ve_Controls.numControls --;

        list_Pop(&ve_Controls.controlList);
    }


    return;
}

#if defined (LINUX) || defined (WINDOWS)
void vControls_SetDefaults(void)
{
    control_AddKey(&ve_Controls.cLeft, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_LEFT);
    control_AddKey(&ve_Controls.cRight, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cUp, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_UP);
    control_AddKey(&ve_Controls.cDown, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_DOWN);
    control_AddKey(&ve_Controls.cUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_UP, SDLK_LEFT);
    control_AddKey(&ve_Controls.cUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_UP, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_DOWN, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_DOWN, SDLK_LEFT);

    control_AddKey(&ve_Controls.cMenuForward, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_e);
    control_AddKey(&ve_Controls.cMenuBackward, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_q);

    control_AddKey(&ve_Controls.cMoveLeft, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_a);
    control_AddKey(&ve_Controls.cMoveRight, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_d);
    control_AddKey(&ve_Controls.cMoveUp, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_w);
    control_AddKey(&ve_Controls.cMoveDown, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_s);
    control_AddKey(&ve_Controls.cMoveUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_w, SDLK_d);
    control_AddKey(&ve_Controls.cMoveUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_w, SDLK_a);
    control_AddKey(&ve_Controls.cMoveDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_s, SDLK_a);
    control_AddKey(&ve_Controls.cMoveDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_s, SDLK_d);

    control_AddKey(&ve_Controls.cShootLeft, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_LEFT);
    control_AddKey(&ve_Controls.cShootRight, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cShootUp, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_UP);
    control_AddKey(&ve_Controls.cShootDown, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_DOWN);
    control_AddKey(&ve_Controls.cShootUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_UP, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cShootUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_UP, SDLK_LEFT);
    control_AddKey(&ve_Controls.cShootDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_DOWN, SDLK_LEFT);
    control_AddKey(&ve_Controls.cShootDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_DOWN, SDLK_RIGHT);

    control_AddKey(&ve_Controls.cBuyUnit[UNIT_TANK], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_1);
    control_AddKey(&ve_Controls.cBuyUnit[UNIT_INFANTRY], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_2);
    control_AddKey(&ve_Controls.cBuyUnit[UNIT_AIR], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_3);
    control_AddKey(&ve_Controls.cBuyUnit[UNIT_TOWER], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_4);

    control_AddKey(&ve_Controls.cConfirm, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_RETURN);
    control_AddKey(&ve_Controls.cPointerConfirm, C_MOUSE, C_BUTTON_STATES, 1, C_MOUSELEFT);

    control_AddKey(&ve_Controls.cBack, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_ESCAPE);
    control_AddKey(&ve_Controls.cBack, C_MOUSE, SDL_MOUSEBUTTONDOWN, 1, C_MOUSERIGHT);

    control_AddKey(&ve_Controls.cMouseHold, C_MOUSE, C_BUTTON_STATES, 1, C_MOUSELEFT);

    control_AddKey(&ve_Controls.cBuy, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_SPACE);
    control_AddKey(&ve_Controls.cPause, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_p);
    control_AddKey(&ve_Controls.cCall, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_h);
    control_AddKey(&ve_Controls.cCycleWeapon, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_v);

    control_AddKey(&ve_Controls.cMoveCameraLeft, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_LEFT);
    control_AddKey(&ve_Controls.cMoveCameraRight, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cMoveCameraUp, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_UP);
    control_AddKey(&ve_Controls.cMoveCameraDown, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_DOWN);
    control_AddKey(&ve_Controls.cMoveCameraUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_UP, SDLK_RIGHT);
    control_AddKey(&ve_Controls.cMoveCameraUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_UP, SDLK_LEFT);
    control_AddKey(&ve_Controls.cMoveCameraDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_DOWN, SDLK_LEFT);
    control_AddKey(&ve_Controls.cMoveCameraDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, SDLK_DOWN, SDLK_RIGHT);

    control_AddKey(&ve_Controls.cTileObjects, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_LSHIFT);
    control_AddKey(&ve_Controls.cTileObjects, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_RSHIFT);

    control_AddKey(&ve_Controls.cGridObjects, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_z);

    control_AddKey(&ve_Controls.cNudgePointer, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_n);
    control_AddKey(&ve_Controls.cRotateObject, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_r);
    control_AddKey(&ve_Controls.cEditorSave, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_F5);
    control_AddKey(&ve_Controls.cEditorHelp, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_h);

    return;
}
#endif

#ifdef PANDORA
void vControls_SetDefaults(void)
{
    control_AddKey(&ve_Controls.cLeft, C_KEYBOARD, C_BUTTON_STATES, 1, PND_LEFT);
    control_AddKey(&ve_Controls.cRight, C_KEYBOARD, C_BUTTON_STATES, 1, PND_RIGHT);
    control_AddKey(&ve_Controls.cUp, C_KEYBOARD, C_BUTTON_STATES, 1, PND_UP);
    control_AddKey(&ve_Controls.cDown, C_KEYBOARD, C_BUTTON_STATES, 1, PND_DOWN);
    control_AddKey(&ve_Controls.cUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_UP, PND_LEFT);
    control_AddKey(&ve_Controls.cUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_UP, PND_RIGHT);
    control_AddKey(&ve_Controls.cDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_DOWN, PND_RIGHT);
    control_AddKey(&ve_Controls.cDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_DOWN, PND_LEFT);


    control_AddKey(&ve_Controls.cMenuForward, C_KEYBOARD, C_BUTTON_STATES, 1, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMenuBackward, C_KEYBOARD, C_BUTTON_STATES, 1, PND_LSHOULDER);

    control_AddKey(&ve_Controls.cMoveLeft, C_KEYBOARD, C_BUTTON_STATES, 1, PND_LEFT);
    control_AddKey(&ve_Controls.cMoveRight, C_KEYBOARD, C_BUTTON_STATES, 1, PND_RIGHT);
    control_AddKey(&ve_Controls.cMoveUp, C_KEYBOARD, C_BUTTON_STATES, 1, PND_UP);
    control_AddKey(&ve_Controls.cMoveDown, C_KEYBOARD, C_BUTTON_STATES, 1, PND_DOWN);
    control_AddKey(&ve_Controls.cMoveUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_UP, PND_RIGHT);
    control_AddKey(&ve_Controls.cMoveUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_UP, PND_LEFT);
    control_AddKey(&ve_Controls.cMoveDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_DOWN, PND_LEFT);
    control_AddKey(&ve_Controls.cMoveDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_DOWN, PND_RIGHT);

    control_AddKey(&ve_Controls.cShootLeft, C_KEYBOARD, C_BUTTON_STATES, 1, PND_A);
    control_AddKey(&ve_Controls.cShootRight, C_KEYBOARD, C_BUTTON_STATES, 1, PND_B);
    control_AddKey(&ve_Controls.cShootUp, C_KEYBOARD, C_BUTTON_STATES, 1, PND_Y);
    control_AddKey(&ve_Controls.cShootDown, C_KEYBOARD, C_BUTTON_STATES, 1, PND_X);
    control_AddKey(&ve_Controls.cShootUpRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_Y, PND_B);
    control_AddKey(&ve_Controls.cShootUpLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_Y, PND_A);
    control_AddKey(&ve_Controls.cShootDownLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_X, PND_A);
    control_AddKey(&ve_Controls.cShootDownRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_X, PND_B);

    control_AddKey(&ve_Controls.cBuyUnit[UNIT_TANK], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_1);
    control_AddKey(&ve_Controls.cBuyUnit[UNIT_INFANTRY], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_2);
    control_AddKey(&ve_Controls.cBuyUnit[UNIT_AIR], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_3);
    control_AddKey(&ve_Controls.cBuyUnit[UNIT_TOWER], C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_4);

    control_AddKey(&ve_Controls.cConfirm, C_KEYBOARD, SDL_KEYDOWN, 1, PND_B);
    control_AddKey(&ve_Controls.cConfirm, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_1);
    //control_AddKey(&ve_Controls.cPointerConfirm, C_KEYBOARD, SDL_KEYDOWN, 1, PND_B);
    control_AddKey(&ve_Controls.cPointerConfirm, C_MOUSE, C_BUTTON_STATES, 1, C_MOUSELEFT);

    control_AddKey(&ve_Controls.cBack, C_KEYBOARD, SDL_KEYDOWN, 1, PND_X);
    control_AddKey(&ve_Controls.cBack, C_MOUSE, SDL_MOUSEBUTTONDOWN, 1, C_MOUSERIGHT);

    control_AddKey(&ve_Controls.cMouseHold, C_MOUSE, C_BUTTON_STATES, 1, C_MOUSELEFT);

    control_AddKey(&ve_Controls.cBuy, C_KEYBOARD, SDL_KEYDOWN, 1, SDLK_SPACE);
    control_AddKey(&ve_Controls.cBuy, C_KEYBOARD, SDL_KEYDOWN, 1, PND_SELECT);
    control_AddKey(&ve_Controls.cPause, C_KEYBOARD,  SDL_KEYDOWN,1, PND_START);
    control_AddKey(&ve_Controls.cCall, C_KEYBOARD, C_BUTTON_STATES, 1, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cCycleWeapon, C_KEYBOARD, SDL_KEYDOWN, 1, PND_LSHOULDER);

    control_AddKey(&ve_Controls.cMoveCameraLeft, C_KEYBOARD, C_BUTTON_STATES, 2, PND_LEFT, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraRight, C_KEYBOARD, C_BUTTON_STATES, 2, PND_RIGHT, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraUp, C_KEYBOARD, C_BUTTON_STATES, 2, PND_UP, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraDown, C_KEYBOARD, C_BUTTON_STATES, 2, PND_DOWN, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraUpRight, C_KEYBOARD, C_BUTTON_STATES, 3, PND_UP, PND_RIGHT, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraUpLeft, C_KEYBOARD, C_BUTTON_STATES, 3, PND_UP, PND_LEFT, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraDownLeft, C_KEYBOARD, C_BUTTON_STATES, 3, PND_DOWN, PND_LEFT, PND_RSHOULDER);
    control_AddKey(&ve_Controls.cMoveCameraDownRight, C_KEYBOARD, C_BUTTON_STATES, 3, PND_DOWN, PND_RIGHT, PND_RSHOULDER);

    control_AddKey(&ve_Controls.cTileObjects, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_LSHIFT);
    control_AddKey(&ve_Controls.cTileObjects, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_RSHIFT);
    control_AddKey(&ve_Controls.cTileObjects, C_KEYBOARD, C_BUTTON_STATES, 1, PND_Y);

    control_AddKey(&ve_Controls.cGridObjects, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_z);
    control_AddKey(&ve_Controls.cGridObjects, C_KEYBOARD, C_BUTTON_STATES, 1, PND_A);

    control_AddKey(&ve_Controls.cNudgePointer, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_2);
    control_AddKey(&ve_Controls.cRotateObject, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_3);
    control_AddKey(&ve_Controls.cEditorSave, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_F5);
    control_AddKey(&ve_Controls.cEditorHelp, C_KEYBOARD, C_BUTTON_STATES, 1, SDLK_h);


    return;
}
#endif
