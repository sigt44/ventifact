#include "UiShape.h"

void uiRect_Setup(Ui_Rect *rect, int xLoc, int yLoc, int width, int height)
{
    rect->x = xLoc;
    rect->y = yLoc;

    rect->w = width;
    rect->h = height;

    return;
}

void uiRect_SetPos(Ui_Rect *rect, int xLoc, int yLoc)
{
    rect->x = xLoc;
    rect->y = yLoc;

    return;
}

void uiRect_ChangePos(Ui_Rect *rect, int xChange, int yChange)
{
    rect->x += xChange;
    rect->y += yChange;

    return;
}

void uiRect_SetDim(Ui_Rect *rect, int width, int height)
{
    rect->w = width;
    rect->h = height;

    return;
}
