#ifndef POINTER_H
#define POINTER_H

#include "../Vector2D.h"
#include "../Graphics/Sprite.h"
#include "../Controls.h"

enum PNT_COLLIDE_STATES
{
    PNT_COLLIDED_NONE = 0,
    PNT_COLLIDED_LEFT = 1,
    PNT_COLLIDED_RIGHT = 2,
    PNT_COLLIDED_UP = 3,
    PNT_COLLIDED_DOWN = 4
};

typedef struct ui_Pointer
{
    Vector2D clampPosition;
    Vector2D lastPosition;

    Vector2D mousePosition;
    Vector2D lastMousePosition;

    Vector2D positionChanged;
    Vector2D position;

    Vector2D gridPosition;

    Vector2D boundry;

    Sprite *graphic;
    float halfWidth;
    float halfHeight;

    unsigned int clamped;
    int draw;
    int nudge;

    int enabled; /*Should the pointer update*/

    float buttonSpeed;

    Control_Event cUp;
    Control_Event cDown;
    Control_Event cLeft;
    Control_Event cRight;

    Control_Event cActivate;

    int moved; /*Has the pointer move from last update*/
    int collidedX; /*Has the pointer collided with boundry from last update*/
    int collidedY;

    int gridW; /*Pointer will only move to locations on the grid*/
    int gridH;
    Vector2DInt gridOrigin;

} Ui_Pointer;

Ui_Pointer *uiPointer_Create(Sprite *graphic, int layer);

void uiPointer_Setup(Ui_Pointer *pnt, Sprite *graphic, int layer);

void uiPointer_SetGrid(Ui_Pointer *pnt, int oX, int oY, int gridW, int gridH);

int uiPointer_SetControls(Ui_Pointer *pnt, float speed,
                                            Control_Event *cpyUp,
                                            Control_Event *cpyDown,
                                            Control_Event *cpyLeft,
                                            Control_Event *cpyRight);

void uiPointer_SetActivate(Ui_Pointer *pnt, int repeater, Control_Event *cpyActivate);

int uiPointer_SetPos(Ui_Pointer *pnt, int warpMouse, float x, float y);

Vector2D uiPointer_GetPos(Ui_Pointer *pnt);

void uiPointer_SetDraw(Ui_Pointer *pnt, int draw);

int uiPointer_SetBoundry(Ui_Pointer *pnt, float x, float y);

void uiPointer_SetFrame(Ui_Pointer *pnt, int frameIndex);

void uiPointer_SetSpeed(Ui_Pointer *pnt, float speed);

void uiPointer_SetNudge(Ui_Pointer *pnt, int nudgeAmount);

void uiPointer_Enable(Ui_Pointer *pnt);

void uiPointer_Disable(Ui_Pointer *pnt);

int uiPointer_IsEnabled(Ui_Pointer *pnt);

int uiPointer_Update(Ui_Pointer *pnt, float deltaTime);

void uiPointer_UpdateGrid(Ui_Pointer *pnt);

int uiPointer_UpdateContraints(Ui_Pointer *pnt);

void uiPointer_Draw(Ui_Pointer *pnt, SDL_Surface *surface);

void uiPointer_Clean(Ui_Pointer *pnt);

int uiPointer_HasMoved(Ui_Pointer *pnt);

int uiPointer_HasCollided(Ui_Pointer *pnt);

void uiPointer_Unclamp(Ui_Pointer *pnt);

void uiPointer_Clamp(Ui_Pointer *pnt);

int uiPointer_IsActivated(Ui_Pointer *pnt, int pauseTime);


#endif
