#include "Button.h"

#include "../DataStruct.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/SurfaceManager.h"
#include "../Graphics/Pixel.h"
#include "../Kernel/Kernel_State.h"
#include "../Collision.h"

/*
    Function: uiButton_Create

    Description -
    Returns a customised GUI button.

    12 arguments:
    int x - relative x-axis position of the button. (To screen/spine/button scroll)
    int y - relative y-axis position of the button.
    int w - pointer collision width.
    int h - pointer collision height.
    void (*onActivate)(Ui_Button *button) - function to call when the button is activated
    void (*onHover)(Ui_Button *button) - function to call when the button is being highlighted.
    Data_Struct *info - Holds the variables needed for the function calls.
    Sprite *graphic - Graphic of the button.
    int forceHover - The button is always highlighted if this is 1
    int updateFreq - How often should the button be updated.
    Timer *srcTimer - Source timer for the button to use.
*/
Ui_Button *uiButton_Create(int x, int y,
                           int w, int h,
                           void (*onActivate)(Ui_Button *button),
                           void (*onHover)(Ui_Button *button),
                           Data_Struct *info,
                           Sprite *graphic,
                           int forceHover, int updateFreq,
                           Timer *srcTimer)
{
    Ui_Button *newButton = (Ui_Button *)mem_Malloc(sizeof(Ui_Button), __LINE__, __FILE__);

    uiButton_Setup(newButton, x, y, w, h, onActivate, onHover, info, graphic, forceHover, updateFreq, srcTimer);

    return newButton;
}

void uiButton_Setup(Ui_Button *b, int x, int y, int w, int h,
    void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info,
    Sprite *graphic, int forceHover, int updateFreq, Timer *srcTimer)
{
    uiRect_Setup(&b->rect, x, y, w, h);

    b->onActivate = onActivate;
    b->onHover = onHover;
    b->onUpdate = NULL;

    b->forceHover = forceHover;
    b->tempHover = 0;

    b->enabled = 1;
    b->info = info;

    b->forceActivation = 0;
	b->state = BUTTON_STATE_IDLE;

    b->updateFreq = timer_Setup(srcTimer, 0 , 0, 1);

    b->graphic = graphic;
    b->text = NULL;

    b->selectSound = NULL;
    b->onHoverSound = NULL;

    control_Setup(&b->cActivate[BC_POINTER], "ButtonPntActivate", srcTimer, updateFreq);
    control_Setup(&b->cActivate[BC_KEY], "ButtonKeyActivate", srcTimer, updateFreq);

    control_AddTimerLink(&b->cActivate[BC_POINTER], &b->cActivate[BC_KEY], 1);

    depWatcher_Setup(&b->dW, b);

    return;
}

void uiButton_SetSound(Ui_Button *b, Mix_Chunk *selectSound, Mix_Chunk *onHoverSound)
{
    b->selectSound = selectSound;
    b->onHoverSound = onHoverSound;

    return;
}

/*Set the button to being always highlighted(1) or not(0)*/
void uiButton_ForceHover(Ui_Button *b, int hover)
{
    b->forceHover = hover;

    return;
}

/*Set the button to being highlighted for a set amount of frames*/
void uiButton_TempHover(Ui_Button *b, int hoverFrames)
{
    b->tempHover += hoverFrames;

    return;
}

/*Set the button to being activated for a set amount of frames*/
void uiButton_TempActive(Ui_Button *b, int activeFrames)
{
    b->forceActivation += activeFrames;

    return;
}

/*
    Function: uiButton_CopyControl

    Description -
    Copy the key events from a control structure to the control structure of the button.

    3 arguments:
    Ui_Button *b - The button to copy keys into.
    Control_Event *cpyActivate - The controls to be copied.
    int controlType - The type of event that the controls are to be used for,
    BC_POINTER for when the pointer is on the button,
    BC_KEY controls for when the button is in its hover state.
    BC_ALL controls for both of the above.
*/
void uiButton_CopyControl(Ui_Button *b, Control_Event *cpyActivate, int controlType)
{
    if(controlType != BC_ALL)
    {
        control_CopyKey(&b->cActivate[controlType], cpyActivate);
        control_AddTimerLink(cpyActivate, &b->cActivate[controlType], 1);
    }
    else
    {
        control_CopyKey(&b->cActivate[BC_POINTER], cpyActivate);
        control_AddTimerLink(cpyActivate, &b->cActivate[BC_POINTER], 1);

        control_CopyKey(&b->cActivate[BC_KEY], cpyActivate);
        control_AddTimerLink(cpyActivate, &b->cActivate[BC_KEY], 1);
    }

    return;
}

/*Set the function which is called on each update of the button*/
void uiButton_SetUpdate(Ui_Button *b, void (*onUpdate)(Ui_Button *button), int updateNow)
{
    b->onUpdate = onUpdate;

    if(updateNow == 1)
        b->onUpdate(b);

    return;
}

/*
    Function: uiButton_SetPos

    Description -
    Changes the position of a button, makes sure that the text box
    linked to it is also moved.

    3 arguments:
    Ui_Button *b - button to change position.
    int x - relative x-axis position of the button. (To screen/spine/button scroll)
    int y - relative y-axis position of the button.
*/
void uiButton_SetPos(Ui_Button *b, int x, int y)
{
    int changeX = x - b->rect.x;
    int changeY = y - b->rect.y;

    uiRect_SetPos(&b->rect, x, y);

    /*if(b->graphic != NULL)
    {
        sprite_SetPos(b->graphic, x, y);
    }*/

    if(b->text != NULL)
    {
        uiTextBox_SetPos(b->text, b->text->rect.x + changeX, b->text->rect.y + changeY);
    }

    return;
}

/*
    Function: uiButton_SetPosText

    Description -
    Changes the position of a button, makes sure that the text box
    linked to it is also moved. The origin of the button is made to be at the
    beggining of the text linked to the button.

    3 arguments:
    Ui_Button *b - button to change position.
    int x - relative x-axis position to set the button text. (To screen/spine/button scroll)
    int y - relative y-axis position to set the button text.
*/
void uiButton_SetPosText(Ui_Button *b, int x, int y)
{
    Vector2DInt textPos;
    int xDis = 0;
    int yDis = 0;

    if(b->text != NULL)
    {
        textPos = uiTextBox_GetPos(b->text);
        xDis = b->rect.x - textPos.x;
        yDis = b->rect.y - textPos.y;

        uiTextBox_SetPos(b->text, x, y);

        uiRect_SetPos(&b->rect, x + xDis, y + yDis);
    }
    else
    {
        uiRect_SetPos(&b->rect, x, y);
    }

    return;
}

/*
    Function: uiButton_ChangePos

    Description -
    Changes the position of a button, makes sure that the text box
    linked to it is also moved.

    3 arguments:
    Ui_Button *b - button to change position.
    int xDis - change in x-axis.
    int yDis - change in y-axis.
*/
void uiButton_ChangePos(Ui_Button *b, int xDis, int yDis)
{
    uiRect_ChangePos(&b->rect, xDis, yDis);

    /*if(b->graphic != NULL)
    {
        sprite_ChangePos(b->graphic, xDis, yDis);
    }*/

    if(b->text != NULL)
    {
        uiTextBox_ChangePos(b->text, xDis, yDis);
    }

    return;
}

/*
    Function: uiButton_GetPos

    Description -
    Returns an int vector as the position of the button.

    1 arguments:
    Ui_Button *b - button to get the position from.
*/
Vector2DInt uiButton_GetPos(Ui_Button *b)
{
    Vector2DInt pos = {b->rect.x, b->rect.y};

    return pos;
}

Vector2DInt uiButton_GetPosText(Ui_Button *b)
{
    if(b->text != NULL)
    {
        return uiTextBox_GetPos(b->text);
    }

    return uiButton_GetPos(b);
}

/*
    Function: uiButton_GetRect

    Description -
    Returns the rectangle that represents the button.

    1 arguments:
    Ui_Button *b - button to get the rectangle from.
*/
Ui_Rect *uiButton_GetRect(Ui_Button *b)
{
    return &b->rect;
}

/*
    Function: uiButton_AddText

    Description -
    Links a Ui_TextBox to a button so that when the button moves
    so does the text.

    2 arguments:
    Ui_Button *b - Button to link in the text.
    Ui_TextBox *text - The text to add to the button.
*/
void uiButton_AddText(Ui_Button *b, Ui_TextBox *text)
{
    if(b->text != NULL)
    {
        printf("Warning replacing text from button\n");
        uiTextBox_Clean(b->text);

        mem_Free(b->text);
    }

    b->text = text;

    uiTextBox_SetPos(b->text, b->text->rect.x + b->rect.x, b->text->rect.y + b->rect.y);

    return;
}

/*
    Function: uiButton_Update

    Description -
    Main update function of the button.
    Sets the state of the button. Makes sure that the button
    only updates a certain amount of frames.
    Checks if conditions for the hover, update and activate function are met, if so calls those functions.

    2 arguments:
    Ui_Button *b - Button to update.
    Ui_Pointer *p - The pointer that can activate and hover over the button.
*/
void uiButton_Update(Ui_Button *b, Ui_Pointer *p)
{
    int activate = 0;
    int hover = 0;

    Vector2D pntPos = {0.0f, 0.0f};

    //b->state = BUTTON_STATE_IDLE;

    if(b->enabled < 1)
        return;

    /*If the button has been stopped for a certain amount of time*/
    if(timer_Get_Stopped(&b->updateFreq) > 0)
    {
        timer_Calc(&b->updateFreq);
        return;
    }

    /*If the button is to update only a set amount of times a frame*/
    if(b->updateFreq.end_Time != 0)
    {
        timer_Calc(&b->updateFreq);

        if(timer_Get_Remain(&b->updateFreq) > 0)
        {
            return;
        }

        timer_Start(&b->updateFreq);
    }

    /*If text is linked make sure that text is being updated*/
    if(b->text != NULL)
        uiTextBox_UpdateParse(b->text);

    /*Perform the general update function if there is one*/
    if(b->onUpdate != NULL)
        b->onUpdate(b);

    if(b->forceActivation != 0)
    {
        /*Reduce the variable by 1 every frame*/
		if(b->forceActivation > 0)
			b->forceActivation --;

        /*Activate the button*/
        activate ++;
    }
    else if(b->forceHover < 0) /*If hovering has been forced off*/
    {
        b->forceHover ++;
    }
    else
    {
        if(p!= NULL && uiPointer_IsEnabled(p) == 1)
        {
            pntPos = uiPointer_GetPos(p);

            /*If the pointer is on the button*/
            if(collisionStatic_Rectangle2(b->rect.x, b->rect.y, b->rect.w, b->rect.h, (int)pntPos.x, (int)pntPos.y, 1, 1) == 1)
            {
                hover++;

                if(uiPointer_IsActivated(p, 0) == 1 || control_IsActivated(&b->cActivate[BC_POINTER]) == 1)
                {
                    activate++;
                }
            }
        }

        if(b->tempHover > 0 || b->forceHover > 0)
        {
            hover++;

            if(control_IsActivated(&b->cActivate[BC_KEY]) == 1)
            {
                activate++;
            }

            if(b->tempHover > 0)
                b->tempHover --;
        }
    }

    if(hover > 0)
    {
        uiButton_Hover(b);
    }
    else
    {
        flag_Off(&b->state, BUTTON_STATE_HOVER);
    }

    if(activate > 0)
    {
        uiButton_Activate(b);
    }
    else
    {
        flag_Off(&b->state, BUTTON_STATE_ACTIVATED);
    }



    return;
}

void uiButton_Activate(Ui_Button *b)
{

    if(flag_Check(&b->state, BUTTON_STATE_ACTIVATED) == 0)
    {
        if(b->selectSound != NULL)
            sound_Call(b->selectSound, -1, SND_DUPLICATE, -1, 0);

        flag_On(&b->state, BUTTON_STATE_ACTIVATED);
    }

    if(b->onActivate != NULL)
    {
        b->onActivate(b);
    }

    return;
}

void uiButton_Hover(Ui_Button *b)
{
    /*If the button is just entering the hover state*/
    if(flag_Check(&b->state, BUTTON_STATE_HOVER) == 0)
    {
        if(b->onHoverSound != NULL)
            sound_Call(b->onHoverSound, -1, SND_DUPLICATE, -1, 0);

        flag_On(&b->state, BUTTON_STATE_HOVER);
    }

    if(b->onHover != NULL)
    {
        b->onHover(b);
    }

    return;
}

int uiButton_IsActivated(Ui_Button *b)
{
    if(flag_Check(&b->state, BUTTON_STATE_ACTIVATED) == 1)
    {
        return 1;
    }

    return 0;
}

/*
    Function: uiButton_Draw

    Description -
    Draws the button and if linked, the text.

    3 arguments:
    Ui_Button *b - Button to draw.
    Camera_2D *camera - Draw relative to the camera, if NULL then draw at the set position.
    SDL_Surface *surface - Draw the button onto this surface
*/
void uiButton_Draw(Ui_Button *b, Camera_2D *camera, SDL_Surface *surface)
{
    if(b->enabled < 1)
        return;

    if(b->graphic != NULL)
    {
        if(camera == NULL)
        {
            sprite_DrawAtPos(b->rect.x, b->rect.y, b->graphic, surface);
        }
        else
        {
            sprite_DrawAtPosFit(camera2D_RelativeX(camera, b->rect.x), camera2D_RelativeY(camera, b->rect.y), &camera->originRect, b->graphic, surface);
        }
    }

    if(b->text != NULL)
        uiTextBox_Draw(b->text, camera, surface);

    return;
}

/*Draw a collision box around the button*/
void uiButton_DrawCollision(Ui_Button *b, SDL_Surface *surface)
{
    draw_Box(b->rect.x, b->rect.y, b->rect.w, b->rect.h, 1, &colourLightBlue, surface);

    return;
}

/*Enable (1) / disable (0) a button*/
void uiButton_Enable(Ui_Button *b, int toggle)
{
    b->enabled = toggle;

    return;
}

/*
    Function: uiButton_Clean

    Description -
    Cleans up a button.

    1 argument:
    Ui_Button *b - Button to clean up.
*/
void uiButton_Clean(Ui_Button *b)
{
    depWatcher_Clean(&b->dW);

    control_Clean(&b->cActivate[BC_POINTER]);
    control_Clean(&b->cActivate[BC_KEY]);

    if(b->graphic != NULL && b->graphic->local == 1)
    {
        sprite_Clean(b->graphic);
        mem_Free(b->graphic);
        b->graphic = NULL;
    }

    if(b->info != NULL)
    {
        dataStruct_Clean(b->info);
        mem_Free(b->info);
    }

    if(b->text != NULL)
    {
        uiTextBox_Clean(b->text);

        mem_Free(b->text);
    }

    return;
}
