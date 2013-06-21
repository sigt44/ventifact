#ifndef CONTROLMAP_H
#define CONTROLMAP_H

#include "Controls.h"
#include "Defines.h"

#define CONTROL_CHANGE 1
#define CONTROL_LOCKED 0

struct Ve_Controls
{
    Control_Event cLeft;
    Control_Event cRight;
    Control_Event cUp;
    Control_Event cDown;
    Control_Event cUpRight;
    Control_Event cUpLeft;
    Control_Event cDownRight;
    Control_Event cDownLeft;

    Control_Event cMenuForward;
    Control_Event cMenuBackward;

    Control_Event cMoveLeft;
    Control_Event cMoveRight;
    Control_Event cMoveUp;
    Control_Event cMoveDown;
    Control_Event cMoveUpRight;
    Control_Event cMoveUpLeft;
    Control_Event cMoveDownRight;
    Control_Event cMoveDownLeft;

    Control_Event cShootLeft;
    Control_Event cShootRight;
    Control_Event cShootUp;
    Control_Event cShootDown;
    Control_Event cShootUpRight;
    Control_Event cShootUpLeft;
    Control_Event cShootDownRight;
    Control_Event cShootDownLeft;

    Control_Event cBuyUnit[UNIT_ENDTYPE];

    Control_Event cConfirm;
    Control_Event cPointerConfirm;
    Control_Event cBack;
    Control_Event cMouseHold;

    Control_Event cBuy;
    Control_Event cCall;
    Control_Event cPause;
    Control_Event cCycleWeapon;

    Control_Event cMoveCameraLeft;
    Control_Event cMoveCameraRight;
    Control_Event cMoveCameraUp;
    Control_Event cMoveCameraDown;
    Control_Event cMoveCameraUpRight;
    Control_Event cMoveCameraUpLeft;
    Control_Event cMoveCameraDownRight;
    Control_Event cMoveCameraDownLeft;

    Control_Event cTileObjects;
    Control_Event cGridObjects;
    Control_Event cNudgePointer;
    Control_Event cRotateObject;
    Control_Event cEditorSave;
    Control_Event cEditorHelp;

    struct list *controlList;
    int numControls;
    int numControlsChangeable;
};

extern struct Ve_Controls ve_Controls;

int vControl_Setup(Control_Event *controlEvent, char *name, Timer *srcTimer, unsigned int repeatDelay, int allowChange);
Control_Event *vControl_Find(char *controlName);

void vControls_Init(void);
void vControls_SetDefaults(void);
void vControls_Clean(void);

#endif
