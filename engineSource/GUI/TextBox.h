#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <SDL/SDL.h>
#include "SDL/SDL_ttf.h"

#include "../Time/Timer.h"
#include "../Graphics/Sprite.h"
#include "../Graphics/Pixel.h"
#include "../DataStruct.h"
#include "../Camera.h"
#include "UiShape.h"

typedef struct ui_TextBox
{
    Ui_Rect rect;

    int indentX;
    int indentY;

    int lineSpacing;

    Timer lifeTime;

    unsigned int canMove;

    unsigned int alpha;

    int layer;
    int numLines;

    SDL_Colour textColour;

    TTF_Font *font;
    char *text; /*The original supplied text*/

    struct list *textList; /*Each node is a line that contains the corrected text that will actually be drawn*/

    Sprite *graphic;

    int state;

    Data_Struct *variables; /*Holds pointers to the variables which change, so that the text is updated*/

} Ui_TextBox;

Ui_TextBox *uiTextBox_CreateBase(int x, int y, int layer, int lineSpacing, SDL_Surface *background, TTF_Font *font, SDL_Color textColour, Timer *srcTime);

int uiTextBox_ParseText(Ui_TextBox *textBox);

Ui_TextBox *uiTextBox_UpdateParse(Ui_TextBox *textBox);

void uiTextBox_UpdateGraphic(Ui_TextBox *t);

void uiTextBox_AddText(Ui_TextBox *t, int indentX, int indentY, char *text, Data_Struct *variables);

void uiTextBox_ChangeText(Ui_TextBox *t, int indentX, int indentY, char *text, Data_Struct *variables);

void uiTextBox_Draw(Ui_TextBox *t, Camera_2D *camera, SDL_Surface *destination);

void uiTextBox_SetPos(Ui_TextBox *t, int x, int y);

void uiTextBox_ChangePos(Ui_TextBox *t, int xDis, int yDis);

Vector2DInt uiTextBox_GetPos(Ui_TextBox *t);

Ui_Rect *uiTextBox_GetRect(Ui_TextBox *t);

void uiTextBox_Clean(Ui_TextBox *t);


#endif
