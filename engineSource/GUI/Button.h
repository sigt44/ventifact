#ifndef BUTTON_H
#define BUTTON_H

#include "../Sound.h"
#include "../Controls.h"
#include "../Common/ListWatcher.h"
#include "Pointer.h"
#include "../Time/Timer.h"
#include "../Graphics/Sprite.h"
#include "../DataStruct.h"
#include "../Camera.h"
#include "TextBox.h"

#define SELF NULL

#define BUTTON_STATE_IDLE 0
#define BUTTON_STATE_ACTIVATED 1
#define BUTTON_STATE_HOVER 2

enum
{
    BC_POINTER = 0,
    BC_KEY = 1,
    BC_ALL = 2
} BUTTON_CONTROL_TYPES;

typedef struct ui_Button
{
    Ui_Rect rect;

    int layer;

    Control_Event cActivate[2];

    void (*onActivate)(struct ui_Button *button);
    void (*onHover)(struct ui_Button *button);
    void (*onHoverGraphic)(struct ui_Button *button);
    void (*onUpdate)(struct ui_Button *button);

    Data_Struct *info;

    Timer updateFreq;

    Sprite *graphic;
    Ui_TextBox *text; /*To display variable text*/

    Mix_Chunk *selectSound;
    Mix_Chunk *onHoverSound;

    unsigned int enabled;

    int tempHover;
    int forceHover;
    int forceActivation;
	unsigned int state;

	Dependency_Watcher dW;

} Ui_Button;

Ui_Button *uiButton_CreateChain(unsigned int numButtons, ...);

Ui_Button *uiButton_Create(int x, int y, int w, int h,
    void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info,
    Sprite *graphic, int forceHover, int updateFreq, Timer *srcTimer);

void uiButton_Setup(Ui_Button *b, int x, int y, int w, int h,
    void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info,
    Sprite *graphic, int forceHover, int updateFreq, Timer *srcTimer);

void uiButton_SetSound(Ui_Button *b, Mix_Chunk *selectSound, Mix_Chunk *onHoverSound);

void uiButton_SetPos(Ui_Button *b, int x, int y);

void uiButton_SetPosText(Ui_Button *b, int x, int y);

void uiButton_ChangePos(Ui_Button *b, int xDis, int yDis);

Vector2DInt uiButton_GetPos(Ui_Button *b);
Vector2DInt uiButton_GetPosText(Ui_Button *b);

Ui_Rect *uiButton_GetRect(Ui_Button *b);

void uiButton_SetUpdate(Ui_Button *b, void (*onUpdate)(Ui_Button *button), int updateNow);

void uiButton_AddText(Ui_Button *b, Ui_TextBox *text);

void uiButton_CopyControl(Ui_Button *b, Control_Event *cpyActivate, int controlType);

void uiButton_ForceHover(Ui_Button *b, int hover);

void uiButton_TempHover(Ui_Button *b, int hoverFrames);

void uiButton_TempActive(Ui_Button *b, int activeFrames);

void uiButton_Update(Ui_Button *b, Ui_Pointer *p);

void uiButton_Activate(Ui_Button *b);

void uiButton_Hover(Ui_Button *b);

int uiButton_IsActivated(Ui_Button *b);

void uiButton_Draw(Ui_Button *b, Camera_2D *camera, SDL_Surface *surface);

void uiButton_DrawCollision(Ui_Button *b, SDL_Surface *surface);

void uiButton_Enable(Ui_Button *b, int toggle);

void uiButton_Clean(Ui_Button *b);



#endif
