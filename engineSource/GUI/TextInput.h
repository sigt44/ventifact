#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "../Graphics/Sprite.h"
#include "../Common/ListWatcher.h"
#include "Button.h"
#include "Pointer.h"

enum
{
    UI_TEXTINPUT_BASE = 0,
    UI_TEXTINPUT_POLL = 1
} UI_TEXTINPUT_STATES;

enum
{
    UI_OUTPUT_STR = 0,
    UI_OUTPUT_INT = 1
} UI_OUTPUT_TYPES;

typedef struct ui_TextInput
{
    Ui_Rect rect;

    int state; /*Base state or Input state*/

    int outputType;
    void **outputVar; /*Points to where the input text should be copied to*/

    int textLength;
    int textWidth;
    int textPos; /*Index for the current place text is to be inserted*/
    char *inputText; /*The buffer for the input text*/

    Sprite sMarker; /*Marks where the current input letter is at*/

    Ui_Button button; /*The main body of the text input will be a button*/

    Timer inputTimer;

    Dependency_Watcher dW;

} Ui_TextInput;

Ui_TextInput *uiTextInput_Create(int x, int y, int layer, char *text, TTF_Font *font, int maxLength, int outputType, void *outputVar, Control_Event *cActivate, Ui_Pointer *pnt, Timer *srcTimer);

void uiTextInput_Setup(Ui_TextInput *textInput, int x, int y, int layer, char *text, TTF_Font *font, int maxLength, int outputType, void *outputVar, Control_Event *cActivate, Ui_Pointer *pnt, Timer *srcTimer);

void uiTextInput_Draw(Ui_TextInput *textInput, Camera_2D *camera, SDL_Surface *destination);

void uiTextInput_Update(Ui_TextInput *textInput, Ui_Pointer *pnt);

void uiTextInput_AssignInput(Ui_TextInput *textInput);

void uiTextInput_AssignOutput(Ui_TextInput *textInput);

void uiTextInput_SetOutputVar(Ui_TextInput *textInput, void *outputVar);

void uiTextInput_SetPos(Ui_TextInput *textInput, int x, int y);

Vector2DInt uiTextInput_GetPos(Ui_TextInput *textInput);

Ui_Rect *uiTextInput_GetRect(Ui_TextInput *textInput);

void uiTextInput_ChangePos(Ui_TextInput *textInput, int xDis, int yDis);

void uiTextInput_SetState(Ui_TextInput *textInput, Ui_Pointer *pnt, int state);

void uiTextInput_Clean(Ui_TextInput *textInput);


#endif
