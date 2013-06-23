#include "Pointer.h"
#include "../Graphics/Animation.h"
#include "../Graphics/Surface.h"
#include "../Graphics/Sprite.h"
#include "../Vector2D.h"

Ui_Pointer *uiPointer_Create(Sprite *graphic, int layer)
{
    Ui_Pointer *pnt = (Ui_Pointer *)mem_Malloc(sizeof(Ui_Pointer), __LINE__, __FILE__);

    uiPointer_Setup(pnt, graphic, layer);

    return pnt;
}

void uiPointer_Setup(Ui_Pointer *pnt, Sprite *graphic, int layer)
{
    int mouseX = 0;
    int mouseY = 0;

    SDL_GetMouseState(&mouseX, &mouseY);
    pnt->lastMousePosition.x = (float)mouseX;
    pnt->lastMousePosition.y = (float)mouseY;
    pnt->mousePosition = pnt->lastMousePosition;

    vector_Clear(&pnt->positionChanged);
    vector_Clear(&pnt->lastPosition);
    vector_Clear(&pnt->clampPosition);
    vector_Clear(&pnt->position);
    vector_Clear(&pnt->gridPosition);
    vector_Clear(&pnt->boundry);

    pnt->clamped = 0;
    pnt->moved = 0;
    pnt->collidedX = PNT_COLLIDED_NONE;
    pnt->collidedY = PNT_COLLIDED_NONE;

    pnt->buttonSpeed = 0.0f;

    pnt->graphic = graphic;
    pnt->draw = 1;
    pnt->nudge = 0;

    pnt->enabled = 1;

    if(graphic == NULL)
    {
        pnt->halfWidth = 0.0f;
        pnt->halfHeight = 0.0f;
    }
    else
    {
        pnt->halfWidth = (float)sprite_Width(pnt->graphic)/2.0f;
        pnt->halfHeight = (float)sprite_Height(pnt->graphic)/2.0f;

        pnt->graphic->layer = layer;
    }

    control_Setup(&pnt->cUp, "Point Up", NULL, 0);
    control_Setup(&pnt->cDown, "Point Down", NULL, 0);
    control_Setup(&pnt->cLeft, "Point Left", NULL, 0);
    control_Setup(&pnt->cRight, "Point Right", NULL, 0);

    control_Setup(&pnt->cActivate, "Activate", NULL, 0);

    uiPointer_SetGrid(pnt, 0, 0, 0, 0);
    //sprite_SetAlpha(&pnt->graphic, SDL_ALPHA_OPAQUE/2);

    return;
}

void uiPointer_SetGrid(Ui_Pointer *pnt, int oX, int oY, int gridW, int gridH)
{
    pnt->gridOrigin.x = oX;
    pnt->gridOrigin.y = oY;
    pnt->gridW = gridW;
    pnt->gridH = gridH;

    uiPointer_UpdateGrid(pnt);

    return;
}

int uiPointer_SetControls(Ui_Pointer *pnt, float speed,
                                            Control_Event *cpyUp,
                                            Control_Event *cpyDown,
                                            Control_Event *cpyLeft,
                                            Control_Event *cpyRight)
{
    uiPointer_SetSpeed(pnt, speed);

    if(cpyUp != NULL)
    {
        control_CopyKey(&pnt->cUp, cpyUp);
    }

    if(cpyDown != NULL)
    {
        control_CopyKey(&pnt->cDown, cpyDown);
    }

    if(cpyLeft != NULL)
    {
        control_CopyKey(&pnt->cLeft, cpyLeft);
    }

    if(cpyRight != NULL)
    {
        control_CopyKey(&pnt->cRight, cpyRight);
    }

    return 0;
}

void uiPointer_SetActivate(Ui_Pointer *pnt, int repeater, Control_Event *cpyActivate)
{
    control_SetRepeat(&pnt->cActivate, repeater);

    if(cpyActivate != NULL)
    {
        control_CopyKey(&pnt->cActivate, cpyActivate);
    }

    return;
}

void uiPointer_SetSpeed(Ui_Pointer *pnt, float speed)
{
    pnt->buttonSpeed = speed;

    return;
}

void uiPointer_SetNudge(Ui_Pointer *pnt, int nudgeAmount)
{
    pnt->nudge = nudgeAmount;

    return;
}

int uiPointer_Update(Ui_Pointer *pnt, float deltaTime)
{
    int mouseX = 0;
    int mouseY = 0;
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;
    float buttonMove = 0.0f;

    pnt->moved = 0;

    pnt->lastPosition = pnt->position;

    /*Sort out the mouse position*/
    pnt->lastMousePosition = pnt->mousePosition;

    if(pnt->clamped == 1)
        return 0;

    SDL_GetMouseState(&mouseX, &mouseY);
    pnt->mousePosition.x = (float)mouseX;
    pnt->mousePosition.y = (float)mouseY;

    /*pnt->position.x += (float)(pnt->mousePosition.x - pnt->lastMousePosition.x);
    pnt->position.y += (float)(pnt->mousePosition.y - pnt->lastMousePosition.y);*/
    if(pnt->lastMousePosition.x != pnt->mousePosition.x || pnt->lastMousePosition.y != pnt->mousePosition.y)
        pnt->position = pnt->mousePosition;

    /*Sort out the button position*/
    if(pnt->nudge <= 0)
    {
        up = control_IsActivated(&pnt->cUp);
        down = control_IsActivated(&pnt->cDown);
        left = control_IsActivated(&pnt->cLeft);
        right = control_IsActivated(&pnt->cRight);

        buttonMove = pnt->buttonSpeed * deltaTime;
    }
    else
    {
        up = control_IsActivatedType(&pnt->cUp, C_BUTTON_DOWN);
        down = control_IsActivatedType(&pnt->cDown, C_BUTTON_DOWN);
        left = control_IsActivatedType(&pnt->cLeft, C_BUTTON_DOWN);
        right = control_IsActivatedType(&pnt->cRight, C_BUTTON_DOWN);

        buttonMove = (float)pnt->nudge;
    }

    if(up == 1)
    {
        /*Up right*/
        if(right == 1)
        {
            pnt->position.x += buttonMove * 0.7071f;
            pnt->position.y += -buttonMove * 0.7071f;
        }
        else if(left == 1) /*Up left*/
        {
            pnt->position.x += -buttonMove * 0.7071f;
            pnt->position.y += -buttonMove * 0.7071f;
        }
        else /*Up*/
        {
            pnt->position.y += -buttonMove;
        }
    }
    else if(down == 1)
    {
        /*Down right*/
        if(right == 1)
        {
            pnt->position.x += buttonMove * 0.7071f;
            pnt->position.y += buttonMove * 0.7071f;
        }
        else if(left == 1) /*Down left*/
        {
            pnt->position.x += -buttonMove * 0.7071f;
            pnt->position.y += buttonMove * 0.7071f;
        }
        else /*Down*/
        {
            pnt->position.y += buttonMove;
        }
    }
    else if(left == 1)
    {
        pnt->position.x += -buttonMove;
    }
    else if(right == 1)
    {
        pnt->position.x += buttonMove;
    }

    uiPointer_UpdateGrid(pnt);
    uiPointer_UpdateContraints(pnt);

    /*If the pointer has moved, return 1*/
    if(pnt->position.x != pnt->lastPosition.x || pnt->position.y != pnt->lastPosition.y)
    {
        pnt->positionChanged = vector_Subtract(&pnt->position, &pnt->lastPosition);
        pnt->moved = 1;

        if(pnt->enabled == 0)
        {
            uiPointer_Enable(pnt);
        }

        return 1;
    }

    return 0;
}

void uiPointer_UpdateGrid(Ui_Pointer *pnt)
{
    float gridMulti = 0.0f;
    float remainder = 0.0f;

    /*Make sure the pointer lies on the grid*/
    if(pnt->gridW > 0)
    {
        gridMulti = (pnt->position.x - pnt->gridOrigin.x)/pnt->gridW;
        remainder = gridMulti - (int)gridMulti;

        if(remainder >= 0.5f)
        {
            pnt->gridPosition.x = (float)(pnt->gridOrigin.x + (pnt->gridW * (int)(gridMulti + 1.00f)));
        }
        else
        {
            pnt->gridPosition.x = (float)(pnt->gridOrigin.x + (pnt->gridW * (int)(gridMulti)));
        }
    }

    if(pnt->gridH > 0)
    {
        gridMulti = (pnt->position.y - pnt->gridOrigin.y)/pnt->gridH;
        remainder = gridMulti - (int)gridMulti;

        if(remainder >= 0.5f)
        {
            pnt->gridPosition.y = (float)(pnt->gridOrigin.y + (pnt->gridH * (int)(gridMulti + 1.00f)));
        }
        else
        {
            pnt->gridPosition.y = (float)(pnt->gridOrigin.y + (pnt->gridH * (int)(gridMulti)));
        }
    }

    return;
}


int uiPointer_UpdateContraints(Ui_Pointer *pnt)
{
    pnt->collidedX = PNT_COLLIDED_NONE;
    pnt->collidedY = PNT_COLLIDED_NONE;

    /*Make sure the pointer is inside the boundry*/
    if(pnt->boundry.x > 0.0f)
    {
        if(pnt->position.x + pnt->halfWidth > pnt->boundry.x)
        {
            pnt->position.x = pnt->boundry.x - pnt->halfWidth;
            pnt->collidedX = PNT_COLLIDED_RIGHT;
        }
        else if(pnt->position.x - pnt->halfWidth < 0.0f)
        {
            pnt->position.x = pnt->halfWidth;
            pnt->collidedX = PNT_COLLIDED_LEFT;
        }
    }

    if(pnt->boundry.y > 0.0f)
    {
        if(pnt->position.y + pnt->halfHeight > pnt->boundry.y)
        {
            pnt->position.y = pnt->boundry.y - pnt->halfHeight;
            pnt->collidedY = PNT_COLLIDED_DOWN;
        }
        else if(pnt->position.y - pnt->halfHeight < 0.0f)
        {
            pnt->position.y = pnt->halfHeight;
            pnt->collidedY = PNT_COLLIDED_UP;
        }
    }

    return uiPointer_HasCollided(pnt);
}

int uiPointer_SetBoundry(Ui_Pointer *pnt, float x, float y)
{
    pnt->boundry.x = (float)x;
    pnt->boundry.y = (float)y;

    return 0;
}

int uiPointer_SetPos(Ui_Pointer *pnt, int warpMouse, float x, float y)
{
    int xMouse = (int)x;
    int yMouse = (int)y;
    pnt->lastPosition = pnt->position;
    pnt->lastMousePosition = pnt->mousePosition;

    pnt->position.x = x;
    pnt->position.y = y;

    if(warpMouse == 1)
    {
        SDL_WarpMouse(xMouse, yMouse);
        SDL_GetMouseState(&xMouse, &yMouse);
    }
    pnt->mousePosition.x = (float)xMouse;
    pnt->mousePosition.y = (float)yMouse;

    return uiPointer_HasMoved(pnt);
}

/*
    Function: uiPointer_GetPos

    Description -
    Returns a position vector that should be the required position of the pointer.
    If the pointer is clamped it will return the clamp position.
    If the pointer is fixed on a grid it will make sure to return the grid position.
    Otherwise it will return the main position, based on mouse and key movements.

    1 argument:
    Ui_Pointer *pnt - The pointer that the position will be from.
*/
Vector2D uiPointer_GetPos(Ui_Pointer *pnt)
{
    Vector2D position = {0.0f, 0.0f};

    if(pnt->clamped > 0)
    {
        position = pnt->clampPosition;
        return position;
    }


    if(pnt->gridW > 0)
    {
        position.x = pnt->gridPosition.x;
    }
    else
    {
        position.x = pnt->position.x;
    }

    if(pnt->gridH > 0)
    {
        position.y = pnt->gridPosition.y;
    }
    else
    {
        position.y = pnt->position.y;
    }

    return position;
}

/*
    Function: uiPointer_SetFrame

    Description -
    This sets the pointers graphic sprite to be at a certain frame.
    It also updates the half width and height values of the pointer,

    2 arguments:
    Ui_Pointer *pnt - The pointer which has the graphic frame to set.
    int frameIndex - The frame index to set the graphic to.
*/
void uiPointer_SetFrame(Ui_Pointer *pnt, int frameIndex)
{
    if(pnt->graphic == NULL)
        return;

    sprite_SetFrame(pnt->graphic, frameIndex);

    pnt->halfWidth = (float)sprite_Width(pnt->graphic)/2.0f;
    pnt->halfHeight = (float)sprite_Height(pnt->graphic)/2.0f;

    return;
}

void uiPointer_Clamp(Ui_Pointer *pnt)
{
    pnt->clamped = 1;
    pnt->clampPosition = pnt->position;

    return;
}

void uiPointer_Unclamp(Ui_Pointer *pnt)
{
    pnt->clamped = 0;

    return;
}

void uiPointer_Enable(Ui_Pointer *pnt)
{
    uiPointer_SetDraw(pnt, 1);
    pnt->enabled = 1;

    return;
}

void uiPointer_Disable(Ui_Pointer *pnt)
{
    uiPointer_SetDraw(pnt, 0);
    pnt->enabled = 0;

    return;
}

int uiPointer_IsEnabled(Ui_Pointer *pnt)
{
    if(pnt->enabled > 0)
        return 1;

    return 0;
}


void uiPointer_SetDraw(Ui_Pointer *pnt, int draw)
{
    pnt->draw = draw;

    return;
}

void uiPointer_Draw(Ui_Pointer *pnt, SDL_Surface *surface)
{
    Vector2D position = {0.0f, 0.0f};
    if(pnt->graphic == NULL || pnt->draw == 0)
    {
        return;
    }

    position = uiPointer_GetPos(pnt);

    if(sprite_CurrentSurface(pnt->graphic) == NULL)
    {
        sprite_Draw(pnt->graphic, surface);
    }
    else
    {
        sprite_DrawAtPos((int)position.x - (int)pnt->halfWidth, (int)position.y - (int)pnt->halfHeight, pnt->graphic, surface);
    }

    return;
}

int uiPointer_IsActivated(Ui_Pointer *pnt, int pauseTime)
{
    if(pnt->enabled == 1 && control_IsActivated(&pnt->cActivate) == 1)
    {
        if(pauseTime > 0)
        {
            control_Stop(&pnt->cActivate, pauseTime);
        }

        return 1;
    }

    return 0;
}

void uiPointer_Clean(Ui_Pointer *pnt)
{
    //frame_Clean(&pnt->graphic.frameList);

    control_Clean(&pnt->cUp);
    control_Clean(&pnt->cDown);
    control_Clean(&pnt->cLeft);
    control_Clean(&pnt->cRight);

    control_Clean(&pnt->cActivate);

    if(pnt->graphic != NULL)
    {
        sprite_Clean(pnt->graphic);
        mem_Free(pnt->graphic);
    }

    return;
}

int uiPointer_HasMoved(Ui_Pointer *pnt)
{
    return pnt->moved;
}

int uiPointer_HasCollided(Ui_Pointer *pnt)
{
    if(pnt->collidedX != PNT_COLLIDED_NONE || pnt->collidedY != PNT_COLLIDED_NONE)
    {
        return 1;
    }

    return 0;
}
