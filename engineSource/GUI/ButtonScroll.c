#include "ButtonScroll.h"

#include "../Graphics/SpriteEffects.h"
#include "../Graphics/Effects/FadeIn.h"
#include "ButtonFunc.h"

void BFA_ScrollTraverseBackwards(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];

    uiButtonScroll_TraverseBackwards(scroll);

    return;
}

void BFA_ScrollTraverseForwards(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];

    uiButtonScroll_TraverseForwards(scroll);

    return;
}

void BFA_ScrollSelectForwards(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];

     uiButtonScroll_IncreaseSelected(scroll);

    return;
}

void BFA_ScrollSelectBackwards(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];

     uiButtonScroll_DecreaseSelected(scroll);

    return;
}

static void BFU_ButtonScrollUpdate(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];

    button->rect.w = scroll->bX - scroll->topX;
    if(button->rect.w == 0)
        button->rect.w = scroll->largestAxis;
    else
        button->rect.w -= scroll->padding;

    button->rect.h = scroll->bY - scroll->topY;
    if(button->rect.h == 0)
        button->rect.h = scroll->largestAxis;
    else
        button->rect.h -= scroll->padding;

    return;
}



void BFA_ScrollSetHover(Ui_Button *button)
{
    Ui_ButtonScroll *scroll = button->info->dataArray[0];
    int *activateOnTraverse = (int *)button->info->dataArray[1];
    /*Get the relative position of this button from the origin*/
    int position = uiButtonScroll_GetButtonPos(scroll, button) - scroll->excessButtons[SCR_BEFORE];

    uiButtonScroll_ForceSelect(scroll, position);

    return;
}

void uiButtonScroll_TraverseBackwards(Ui_ButtonScroll *scroll)
{
    struct list *buttons = scroll->buttons;

    Ui_Button *b = NULL;

    /*If there is just one/none button to scroll then return early*/
    if(buttons == NULL || buttons->next == NULL)
    {
        return;
    }

    /*If the total buttons is smaller than the maximum allowed to show, return early*/
    if(scroll->totalButtons <= scroll->maxShowButtons)
        return;

    if(scroll->rotate == 0)
    {
       /*If there are no more buttons to scroll backwards then return early*/
       if(scroll->excessButtons[SCR_BEFORE] == 0)
            return;
       else
       {
           scroll->excessButtons[SCR_BEFORE] --;
           scroll->excessButtons[SCR_AFTER] ++;
       }
    }
    else if(scroll->rotate == 1)
    {
        if(scroll->originButtonList == scroll->buttons)
        {
            scroll->excessButtons[SCR_BEFORE] = scroll->excessButtons[SCR_AFTER];
            scroll->excessButtons[SCR_AFTER] = 0;
        }
        else
        {
           scroll->excessButtons[SCR_BEFORE] --;
           scroll->excessButtons[SCR_AFTER] ++;
        }
    }

    //printf("SCROLL POS: BEFORE %d AFTER %d ROTATE %d\n", scroll->excessButtons[SCR_BEFORE], scroll->excessButtons[SCR_AFTER], scroll->rotate);
    scroll->originButtonList = scroll->originButtonList->previous;

    if(scroll->fadeTransition > 0)
    {
        b = scroll->originButtonList->data;

        sprite_ClearEffects(b->graphic);
        spriteSE_FadeIn(b->graphic, scroll->fadeTransition, 1);
    }

    uiButtonScroll_CorrectPositions(scroll);

    return;
}

int uiButtonScroll_GetButtonsAfterOrigin(Ui_ButtonScroll *scroll)
{
    int numButtons = 0;
    struct list *originList = scroll->originButtonList;

    while(originList != NULL)
    {
        numButtons++;
        originList = originList->next;
    }

    return numButtons;
}

void uiButtonScroll_TraverseForwards(Ui_ButtonScroll *scroll)
{
    struct list *buttons = scroll->buttons;

    Ui_Button *b = NULL;

    int x = 0;

    /*If the total buttons is smaller than the maximum allowed to show, return early*/
    if(scroll->totalButtons <= scroll->maxShowButtons)
        return;

    if(scroll->rotate == 0)
    {
        /*If there are no more buttons to scroll to then return early*/
        if(scroll->excessButtons[SCR_AFTER] <= scroll->maxShowButtons)
            return;
    }

    scroll->excessButtons[SCR_BEFORE] ++;
    scroll->excessButtons[SCR_AFTER] --;

    scroll->originButtonList = scroll->originButtonList->next;

    if(scroll->originButtonList == NULL && scroll->rotate == 1)
    {
        scroll->originButtonList = scroll->buttons;
        scroll->excessButtons[SCR_AFTER] = scroll->excessButtons[SCR_BEFORE];
        scroll->excessButtons[SCR_BEFORE] = 0;
    }
    else if(scroll->originButtonList == NULL)
    {
        puts("Error: Origin button list is NULL");
        return;
    }

    //printf("SCROLL POS: BEFORE %d AFTER %d ROTATE %d\n", scroll->excessButtons[SCR_BEFORE], scroll->excessButtons[SCR_AFTER], scroll->rotate);


    if(scroll->fadeTransition > 0)
    {
        buttons = scroll->originButtonList;
        for(x = 1; x < scroll->maxShowButtons; x++)
        {
            if(buttons->next == NULL)
            {
                if(scroll->rotate == 1)
                {
                    if(scroll->maxShowButtons < scroll->totalButtons)
                        buttons = scroll->buttons;
                }
                else
                    break;
            }
            else
                buttons = buttons->next;
        }
        b = buttons->data;

        sprite_ClearEffects(b->graphic);
        spriteSE_FadeIn(b->graphic, scroll->fadeTransition, 1);
    }

    uiButtonScroll_CorrectPositions(scroll);

    return;
}

static void uiButtonScroll_ButtonSetPos(Ui_ButtonScroll *scroll, Ui_Button *button)
{
    switch(scroll->direction)
    {
        default:
        case SCR_V:

        /*printf("Setting button at %d %d\n", scroll->bX, scroll->bY);*/
        uiButton_SetPosText(button, scroll->bX, scroll->bY);

        scroll->bY += button->rect.h + scroll->padding;

        if(button->rect.w > scroll->largestAxis)
        {
            scroll->largestAxis = button->rect.w;
        }

        break;

        case SCR_H:

        uiButton_SetPosText(button, scroll->bX, scroll->bY);

        scroll->bX += button->rect.w + scroll->padding;

        if(button->rect.h > scroll->largestAxis)
        {
            scroll->largestAxis = button->rect.h;
        }

        break;

        case SCR_N:
        /*Leave positions alone*/

        break;
    }

    return;
}

/*
    Function: uiButtonScroll_CorrectPositions

    Description -
    Corrects the positions of all the viewed buttons in the scroll.
*/
void uiButtonScroll_CorrectPositions(Ui_ButtonScroll *scroll)
{
    int x = 0;
    struct list *origin = NULL;
    Ui_Button *temp = NULL;

    scroll->bX = scroll->topX;
    scroll->bY = scroll->topY;

    scroll->largestAxis = 0;

    if(scroll->originButtonList == NULL)
        scroll->originButtonList = scroll->buttons;

    origin = scroll->originButtonList;

    while(origin != NULL && x < scroll->maxShowButtons)
    {
        x++;
        uiButtonScroll_ButtonSetPos(scroll, origin->data);

        origin = origin->next;

        /*Check if the scroll is to rotate*/
        if(origin == NULL && scroll->rotate == 1 && scroll->totalButtons > scroll->maxShowButtons)
        {
            origin = scroll->buttons;
        }
    }

    return;
}

/*
    Function: uiButtonScroll_Create

    Description -
    Creates a scroll of buttons.

    10* arguments:
    int tX - The top x-axis location of the scroll of buttons.
    int tY - The top x-axis location of the scroll of buttons.
    int scrollDirection - Whether the scroll goes in a verticle or horizontal direction (SCR_V or SCR_H)
    int padding - The distance between each button.
    int maxShowButtons - The maximum number of buttons to show.
    int rotate - If true the button list will loop back to the start/finish.
    int fadeTransition - If true buttons will fade in and out of view.
    Ui_Button *travF - The button that is used to scroll forwards.
    Ui_Button *travB - The button that is used to scroll backwards.
    int numButtons - The number of pointers to (Ui_Button *) that are added to the end of the function.
*/
Ui_ButtonScroll *uiButtonScroll_Create(int tX, int tY, int scrollDirection, int padding, int maxShowButtons, int rotate, int fadeTransition, Ui_Button *travF, Ui_Button *travB, int numButtons, ...)
{
    Ui_ButtonScroll *scroll = (Ui_ButtonScroll *)mem_Malloc(sizeof(Ui_ButtonScroll), __LINE__, __FILE__);
    va_list args;

    int x = 0;
    int prevPos = 0;

    Ui_Button *button = NULL;

    /*Setup the general scroll button*/
    uiButton_Setup(&scroll->scrollButton, tX, tY, 0, 0,
                NULL, /*onActivate*/
                NULL, /*onHover*/
                dataStruct_Create(1, scroll),
                NULL, /*Sprite*/
                0, 0,
                NULL /*Timer*/
    );

    uiButton_SetUpdate(&scroll->scrollButton, &BFU_ButtonScrollUpdate, 0);

    scroll->updateEnabled = 1;

    depWatcher_Setup(&scroll->dW, scroll);

    scroll->bY = scroll->topY = tY;
    scroll->bX = scroll->topX = tX;
    scroll->largestAxis = 0;

    scroll->direction = scrollDirection;

    scroll->padding = padding;
    scroll->rotate = rotate;
    scroll->fadeTransition = fadeTransition;

    scroll->maxShowButtons = maxShowButtons;

    scroll->traverseForwards = travF;
    scroll->traverseBackwards = travB;

    scroll->buttons = NULL;

    scroll->forceHover = -1;
    scroll->activateOnTraverse = 0;
    scroll->currentForced = NULL;

    scroll->excessButtons[SCR_BEFORE] = 0;
    scroll->excessButtons[SCR_AFTER] = 0;

    scroll->totalButtons = 0;

    /*Push in all the given buttons*/
    va_start(args, numButtons);

    for(x = 0; x < numButtons; x++)
    {
        button = va_arg( args, Ui_Button *);
        uiButtonScroll_AddButton(scroll, button, 0);
    }

    va_end(args);

    //scroll->excessButtons[SCR_AFTER] = scroll->totalButtons;

    /*Set the current origin button to be the one at the head of the list*/
    scroll->originButtonList = scroll->buttons;

    if(travF != NULL)
    {
        if(travF->onActivate == NULL)
            travF->onActivate = BFA_ScrollTraverseForwards;

        if(travF->info == NULL && (travF->onActivate == BFA_ScrollTraverseForwards || travF->onActivate == BFA_ScrollSelectForwards))
            travF->info = dataStruct_Create(1, scroll);
    }

    if(travB != NULL)
    {
        if(travB->onActivate == NULL)
            travB->onActivate = BFA_ScrollTraverseBackwards;

        if(travB->info == NULL && (travB->onActivate == BFA_ScrollTraverseBackwards || travB->onActivate == BFA_ScrollSelectBackwards))
            travB->info = dataStruct_Create(1, scroll);
    }

    return scroll;
}

void uiButtonScroll_SetHoverState(Ui_ButtonScroll *scroll, void (*onHover)(struct ui_Button *button))
{
    scroll->scrollButton.onHover = onHover;

    return;
}

/*
    Function: uiButtonScroll_ForceSelect

    Description -
    Sets a button that is in a relative position in the scroll to be always
    hovering.

    2 arguments:
    Ui_ButtonScroll *scroll - The scroll to set a button to be force hovering.
    int buttonPos - The relative position of the button that is to be always hovering. If -1 then no buttons.
*/
void uiButtonScroll_ForceSelect(Ui_ButtonScroll *scroll, int buttonPos)
{
    scroll->forceHover = buttonPos;

    uiButtonScroll_SetHovered(scroll);

    return;
}
/*
    Function: uiButtonScroll_ActivateOnTraverse

    Description -
    If activateOnTraverse is > 0 then sets the scrolls behaviour to activate the button that has just been selected.

    2 arguments:
    Ui_ButtonScroll *scroll - The scroll to set traverse activation on.
    int activateOnTraverse - If > 0 then the button will be activated when the scroll is traversed.
*/
void uiButtonScroll_ActivateOnTraverse(Ui_ButtonScroll *scroll, int activateOnTraverse)
{
    scroll->activateOnTraverse = activateOnTraverse;

    return;
}


/*
    Function: uiButtonScroll_SetHoverTraverse

    Description -
    Temporarily sets the traverse buttons in the scroll to be hovering.

    2 arguments:
    Ui_ButtonScroll *scroll - The scroll to set the traverse buttons to be hovering.
    int hover - How many frames should the buttons be forced to hover.
*/
void uiButtonScroll_SetHoverTraverse(Ui_ButtonScroll *scroll, int hover)
{
    if(scroll->traverseForwards != NULL)
    {
        uiButton_TempHover(scroll->traverseForwards, hover);
    }

    if(scroll->traverseBackwards != NULL)
    {
        uiButton_TempHover(scroll->traverseBackwards, hover);
    }

    return;
}

void uiButtonScroll_SetOrigin(Ui_ButtonScroll *scroll, int buttonPos)
{
    int x = 0;
    int fade = scroll->fadeTransition;

    scroll->fadeTransition = 0;

    scroll->excessButtons[SCR_BEFORE] = 0;
    scroll->excessButtons[SCR_AFTER] = scroll->totalButtons;

    scroll->originButtonList = scroll->buttons;

    uiButtonScroll_CorrectPositions(scroll);

    for(x = 0; x < buttonPos; x++)
    {
        uiButtonScroll_TraverseForwards(scroll);
    }

    scroll->fadeTransition = fade;

    return;
}

void uiButtonScroll_AddButton(Ui_ButtonScroll *scroll, Ui_Button *button, int position)
{
    if(position == 0)
    {
        list_Stack(&scroll->buttons, button, 0);
    }
    else
    {
        list_Push(&scroll->buttons, button, 0);
    }

    scroll->originButtonList = scroll->buttons;

    uiButtonScroll_ButtonSetPos(scroll, button);

    scroll->totalButtons ++;

    scroll->excessButtons[SCR_AFTER] ++;

    return;
}

void uiButtonScroll_RemoveButton(Ui_ButtonScroll *scroll, Ui_Button *button)
{
    unsigned int deleted = 0;
    int originPos = uiButtonScroll_GetButtonPos(scroll, scroll->originButtonList->data);

    /*Remove the button from the scroll list*/
    deleted = list_Delete_NodeFromData(&scroll->buttons, button);

    /*If successful then clean up the button and reset the scroll*/
    if(deleted > 0)
    {
        scroll->totalButtons --;
        uiButtonScroll_SetOrigin(scroll, originPos);

        uiButton_Clean(button);
        mem_Free(button);
    }

    return;
}

/*
    Function: uiButtonScroll_SetHovered

    Description -
    Find the button relative to the origin and force it to hover.
    The button will be relative by the amount of scroll->forceHover.

    1 argument:
    Ui_ButtonScroll *scroll - The scroll to set the button to be active.
*/
void uiButtonScroll_SetHovered(Ui_ButtonScroll *scroll)
{
    struct list *originList = scroll->originButtonList;

    int x = 0;

    if(scroll->forceHover < 0 || scroll->forceHover >= scroll->maxShowButtons)
    {
        return;
    }

    for(x = 0; x < scroll->forceHover; x++)
    {
        originList = originList->next;

        if(originList == NULL && scroll->rotate == 1)
        {
            if(scroll->maxShowButtons < scroll->totalButtons)
                originList = scroll->buttons;
        }
    }

    if(originList != NULL)
    {
        if(scroll->currentForced != originList->data)
        {
            if(scroll->currentForced != NULL)
            {
                uiButton_TempHover(scroll->currentForced, -scroll->currentForced->tempHover);
            }

            scroll->currentForced = originList->data;

            if(scroll->activateOnTraverse == 1)
            {
                uiButton_TempActive(scroll->currentForced, 1);
            }
        }

        uiButton_TempHover(scroll->currentForced, 1);
    }
    else /*Turn off force hover*/
    {
        scroll->forceHover = -1;
        //printf("Error: origin moved to null when trying to set active scroll button %d\n", scroll->forceHover);
    }

    return;
}

void uiButtonScroll_IncreaseSelected(Ui_ButtonScroll *scroll)
{
    //printf("Increase Scroll: Scroll->forceHover == %d\n", scroll->forceHover);

    if(scroll->forceHover < 0)
        scroll->forceHover = -1;
    else if(scroll->forceHover >= (scroll->maxShowButtons - 1))
    {
        scroll->forceHover = scroll->maxShowButtons - 2;
        uiButtonScroll_TraverseForwards(scroll);
    }
    else if(scroll->forceHover >= (scroll->totalButtons - 1))
    {
        scroll->forceHover = scroll->totalButtons - 2;
    }

    uiButtonScroll_ForceSelect(scroll, scroll->forceHover + 1);

    //printf("SELECTING POS %d in scroll, MAX SHOW %d, TOTAL %d\n", scroll->forceHover, scroll->maxShowButtons, scroll->totalButtons);

    return;
}

void uiButtonScroll_DecreaseSelected(Ui_ButtonScroll *scroll)
{
    if(scroll->forceHover <= 0)
    {
        scroll->forceHover = 1;
        uiButtonScroll_TraverseBackwards(scroll);
    }
    else if(scroll->forceHover >= scroll->maxShowButtons)
    {
        scroll->forceHover = scroll->maxShowButtons - 1;
    }

    uiButtonScroll_ForceSelect(scroll, scroll->forceHover - 1);

    return;
}

int uiButtonScroll_MoveSelected(Ui_ButtonScroll *scroll, int direction)
{
    if(scroll->direction == SCR_V)
    {
        if(direction == UI_UP)
        {
            uiButtonScroll_DecreaseSelected(scroll);

            return 2;
        }
        else if(direction == UI_DOWN)
        {
            uiButtonScroll_IncreaseSelected(scroll);

            return 2;
        }
    }
    else if(scroll->direction == SCR_H)
    {
        if(direction == UI_LEFT)
        {
            uiButtonScroll_DecreaseSelected(scroll);

            return 2;
        }
        else if(direction == UI_RIGHT)
        {
            uiButtonScroll_IncreaseSelected(scroll);

            return 2;
        }
    }

    return 0;
}



void uiButtonScroll_Update(Ui_ButtonScroll *scroll, Ui_Pointer *p)
{
    int x;
    struct list *origin = NULL;

    if(scroll->updateEnabled != 1)
    {
        return;
    }

    if(scroll->traverseForwards != NULL)
        uiButton_Update(scroll->traverseForwards, p);
    if(scroll->traverseBackwards != NULL)
        uiButton_Update(scroll->traverseBackwards, p);

    /*Since the traverse buttons above may change the order of the origin, it is now safe to assign it*/
    origin = scroll->originButtonList;

    /*See if a button needs to be forced hovering*/
    uiButtonScroll_SetHovered(scroll);

    for(x = 0; x < scroll->maxShowButtons; x++)
    {
        if(origin != NULL)
        {
            uiButton_Update((Ui_Button *)origin->data, p);
            origin = origin->next;

            if(origin == NULL && scroll->rotate == 1)
            {
                if(scroll->maxShowButtons < scroll->totalButtons)
                    origin = scroll->buttons;
            }
        }
        else if(scroll->rotate == 0)
        {
            break;
        }
    }

    uiButton_Update(&scroll->scrollButton, p);


    return;
}

void uiButtonScroll_Draw(Ui_ButtonScroll *scroll, Camera_2D *camera, SDL_Surface *surface)
{
    int x = 0;
    struct list *origin = scroll->originButtonList;

    Ui_Button *b = NULL;

    /*Draw the 2 scrolling buttons*/
    if(scroll->traverseForwards != NULL)
        uiButton_Draw(scroll->traverseForwards, camera, surface);
    if(scroll->traverseBackwards != NULL)
        uiButton_Draw(scroll->traverseBackwards, camera, surface);

    /*Draw the list of buttons to scroll through*/
    for(x = 0; x < scroll->maxShowButtons; x++)
    {
        if(origin != NULL)
        {
            uiButton_Draw(origin->data, camera,  surface);
            origin = origin->next;

            if(origin == NULL && scroll->rotate == 1)
            {
                if(scroll->maxShowButtons < scroll->totalButtons)
                    origin = scroll->buttons;
            }
        }
        else if(scroll->rotate == 0) /*Exit early the amount of buttons to show is greater than the actual amount of buttons*/
        {
            break;
        }
    }

    return;
}

void uiButtonScroll_SetPosition(Ui_ButtonScroll *scroll, int x, int y)
{
    /*Keep relative distance*/
    if(scroll->traverseForwards != NULL)
    {
        uiButton_SetPos(scroll->traverseForwards, (scroll->traverseForwards->rect.x - scroll->topX) + x, (scroll->traverseForwards->rect.y - scroll->topY) + y);
    }

    if(scroll->traverseBackwards != NULL)
    {
        uiButton_SetPos(scroll->traverseBackwards, (scroll->traverseBackwards->rect.x - scroll->topX) + x, (scroll->traverseBackwards->rect.y - scroll->topY) + y);
    }

    uiButton_SetPos(&scroll->scrollButton, (scroll->scrollButton.rect.x - scroll->topX) + x, (scroll->scrollButton.rect.y - scroll->topY) + y);


    scroll->bX = scroll->topX = x;

    scroll->bY = scroll->topY = y;

    uiButtonScroll_CorrectPositions(scroll);

    return;
}

void uiButtonScroll_ChangePosition(Ui_ButtonScroll *scroll, int xDis, int yDis)
{
    /*Keep relative distance*/
    if(scroll->traverseForwards != NULL)
    {
        uiButton_ChangePos(scroll->traverseForwards, xDis, yDis);
    }

    if(scroll->traverseBackwards != NULL)
    {
        uiButton_ChangePos(scroll->traverseBackwards, xDis, yDis);
    }

    uiButton_ChangePos(&scroll->scrollButton, xDis, yDis);


    scroll->bX = scroll->topX += xDis;

    scroll->bY = scroll->topY += yDis;

    uiButtonScroll_CorrectPositions(scroll);

    return;
}

/*
    Function: uiButtonScroll_GetPos

    Description -
    Returns a 2D int vector that contains the position from where the scroll starts.

    1 arguments:
    Ui_ButtonScroll *scroll - The scroll to get the position from.
*/
Vector2DInt uiButtonScroll_GetPos(Ui_ButtonScroll *scroll)
{
    return uiButton_GetPos(&scroll->scrollButton);
}

/*
    Function: uiButtonScroll_GetPos

    Description -
    Returns the rectangle that represents the shape of the scroll.

    1 arguments:
    Ui_ButtonScroll *scroll - The scroll to get the position from.
*/
Ui_Rect *uiButtonScroll_GetRect(Ui_ButtonScroll *scroll)
{
    BFU_ButtonScrollUpdate(&scroll->scrollButton);

    return &scroll->scrollButton.rect;
}

/*
    Function: uiButtonScroll_GetButton

    Description -
    Returns the button in the scroll that is in position of buttonPos.

    2 arguments:
    Ui_ButtonScroll *scroll - The scroll the button is in
    int buttonPos - The position the button is in the scroll, starting from 0
*/
Ui_Button *uiButtonScroll_GetButton(Ui_ButtonScroll *scroll, int buttonPos)
{
    int x = 0;
    struct list *buttons = scroll->buttons;

    for(x = 0; x < buttonPos; x++)
    {
        if(buttons->next != NULL)
            buttons = buttons->next;
        else
            printf("Error: button position %d outside range of scroll\n", buttonPos);
    }

    return buttons->data;
}

/*
    Function: uiButtonScroll_GetButtonPos

    Description -
    Returns the position (starting from 0) of a button in the scroll. If the button is not
    found in the scroll then the function will return -1.

    2 arguments:
    Ui_ButtonScroll *scroll - The scroll the button is in
    Ui_Button *button - The button to find the position of.
*/
int uiButtonScroll_GetButtonPos(Ui_ButtonScroll *scroll, Ui_Button *button)
{
    int position = 0;
    struct list *buttons = scroll->buttons;

    while(buttons != NULL)
    {
        if(buttons->data == button)
        {
            return position;
        }

        position ++;
        buttons = buttons->next;
    }

    return -1;
}

int uiButtonScroll_InView(Ui_ButtonScroll *scroll, Ui_Button *button)
{
    int buttonPos = uiButtonScroll_GetButtonPos(scroll, button);
    int originPos = 0;

    if(buttonPos < 0 || scroll->originButtonList == NULL)
        return -1;

    originPos = uiButtonScroll_GetButtonPos(scroll, scroll->originButtonList->data);

    if(scroll->rotate == 1 && (scroll->totalButtons - originPos + buttonPos) < scroll->maxShowButtons)
    {
        return 1;
    }

    if(buttonPos < originPos || (buttonPos >= originPos + scroll->maxShowButtons))
    {
        return 0;
    }

    return 1;
}

void uiButtonScroll_Clean(Ui_ButtonScroll *scroll)
{
    depWatcher_Clean(&scroll->dW);

    uiButton_Clean(&scroll->scrollButton);

    while(scroll->buttons != NULL)
    {
        uiButton_Clean(scroll->buttons->data);
        mem_Free(scroll->buttons->data);

        list_Pop(&scroll->buttons);
    }

    if(scroll->traverseForwards != NULL)
    {
        uiButton_Clean(scroll->traverseForwards);
        mem_Free(scroll->traverseForwards);
    }

    if(scroll->traverseBackwards != NULL)
    {
        uiButton_Clean(scroll->traverseBackwards);
        mem_Free(scroll->traverseBackwards);
    }

    return;
}
