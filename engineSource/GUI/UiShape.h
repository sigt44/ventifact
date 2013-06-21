#ifndef UISHAPE_H
#define UISHAPE_H

typedef struct ui_Rect
{
    int x;
    int y;
    int w;
    int h;
} Ui_Rect;

void uiRect_Setup(Ui_Rect *rect, int xLoc, int yLoc, int width, int height);

void uiRect_SetPos(Ui_Rect *rect, int xLoc, int yLoc);
void uiRect_ChangePos(Ui_Rect *rect, int xChange, int yChange);

void uiRect_SetDim(Ui_Rect *rect, int width, int height);

#endif
