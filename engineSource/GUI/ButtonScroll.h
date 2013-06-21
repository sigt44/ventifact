#ifndef BUTTONSCROLL_H
#define BUTTONSCROLL_H

#include <stdarg.h>
#include "Button.h"
#include "UI_Defines.h"

enum SCROLL_EXCESS
{
    SCR_BEFORE = 0, /*The index of the array that counts the amount of buttons there are before the origin*/
    SCR_AFTER = 1
};

typedef struct ui_ButtonScroll
{
    int updateEnabled; /*If 1 then the scroll will update*/

    Ui_Button scrollButton; /*The scroll represented as a button*/

    struct list *buttons; /*list of buttons to scroll through*/

    Ui_Button *traverseForwards; /*Two buttons to scroll forwards and backwards*/
    Ui_Button *traverseBackwards;

    struct list *originButtonList; /*Points to the list node that contains the button at the origin of scrolling*/

    int excessButtons[2]; /*Holds the amount of buttons that are behind the orginButton and after originButton+maxShowButtons*/

    int totalButtons; /*Total buttons to scroll through*/

    int forceHover; /*The relative position of the scroll where the button is to be always hovering*/
    int activateOnTraverse; /*Activate the forced hover button if the the scroll is traversed*/
    Ui_Button *currentForced;

    int padding; /*The distance between each button*/

    int maxShowButtons; /*The maximum amount of buttons to show*/

    int fadeTransition; /*Should there be fading when a button scrolls in/out of view*/

    int rotate; /*Should the scoll rotate back to the start/end when it reaches end/start*/

    int topX; /*The starting x-axis position for the list of buttons*/
    int topY; /*The starting y-axis position for the list of buttons*/

    int direction; /*The direction to scroll in, verticle or horizontal*/
    int bX; /*Used to hold the positions of where to place the next button*/
    int bY;
    int largestAxis;

    Dependency_Watcher dW;

} Ui_ButtonScroll;

void BFA_ScrollTraverseBackwards(Ui_Button *button);
void BFA_ScrollTraverseForwards(Ui_Button *button);
void BFA_ScrollSelectForwards(Ui_Button *button);
void BFA_ScrollSelectBackwards(Ui_Button *button);


Ui_ButtonScroll *uiButtonScroll_Create(int tX, int tY, int scrollDirection, int padding, int maxShowButtons, int rotate, int fadeTransition, Ui_Button *travF, Ui_Button *travB, int numButtons, ...);

void uiButtonScroll_CorrectPositions(Ui_ButtonScroll *scroll);

void uiButtonScroll_Update(Ui_ButtonScroll *scroll, Ui_Pointer *p);

void uiButtonScroll_Draw(Ui_ButtonScroll *scroll, Camera_2D *camera, SDL_Surface *surface);

void uiButtonScroll_Clean(Ui_ButtonScroll *scroll);

void uiButtonScroll_SetHoverState(Ui_ButtonScroll *scroll, void (*onHover)(struct ui_Button *button));

void uiButtonScroll_SetHovered(Ui_ButtonScroll *scroll);

void uiButtonScroll_SetHoverTraverse(Ui_ButtonScroll *scroll, int hover);

void uiButtonScroll_ForceSelect(Ui_ButtonScroll *scroll, int buttonPos);
void uiButtonScroll_ActivateOnTraverse(Ui_ButtonScroll *scroll, int activateOnTraverse);

void uiButtonScroll_AddButton(Ui_ButtonScroll *scroll, Ui_Button *button, int position);

void uiButtonScroll_RemoveButton(Ui_ButtonScroll *scroll, Ui_Button *button);

Ui_Button *uiButtonScroll_GetButton(Ui_ButtonScroll *scroll, int buttonPos);

int uiButtonScroll_GetButtonPos(Ui_ButtonScroll *scroll, Ui_Button *button);

void uiButtonScroll_TraverseForwards(Ui_ButtonScroll *scroll);

void uiButtonScroll_TraverseBackwards(Ui_ButtonScroll *scroll);

void uiButtonScroll_IncreaseSelected(Ui_ButtonScroll *scroll);
void uiButtonScroll_DecreaseSelected(Ui_ButtonScroll *scroll);
int uiButtonScroll_MoveSelected(Ui_ButtonScroll *scroll, int direction);


void uiButtonScroll_SetPosition(Ui_ButtonScroll *scroll, int x, int y);

void uiButtonScroll_ChangePosition(Ui_ButtonScroll *scroll, int xDis, int yDis);

Vector2DInt uiButtonScroll_GetPos(Ui_ButtonScroll *scroll);
Ui_Rect *uiButtonScroll_GetRect(Ui_ButtonScroll *scroll);

int uiButtonScroll_InView(Ui_ButtonScroll *scroll, Ui_Button *button);

void uiButtonScroll_SetOrigin(Ui_ButtonScroll *scroll, int buttonPos);


void BFA_ScrollSetHover(Ui_Button *button);
void BFA_UiButtonScroll_SetHover(Ui_Button *button);

#endif
