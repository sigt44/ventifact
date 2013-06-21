#include "TextInput.h"
#include <ctype.h>


#include "../Kernel/Kernel_State.h"
#include "../Graphics/Surface.h"
#include "../Graphics/DrawManager.h"
#include "../Graphics/SurfaceManager.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Pixel.h"
#include "../Font.h"
#include "../Controls.h"

static void BFA_EnterPoll(Ui_Button *button)
{
    Ui_TextInput *textInput = (Ui_TextInput *)button->info->dataArray[0];
    Ui_Pointer *pnt = (Ui_Pointer *)button->info->dataArray[1];

    uiTextInput_SetState(textInput, pnt, UI_TEXTINPUT_POLL);

    button->state = BUTTON_STATE_IDLE;

    return;
}

static void BFA_Hover(Ui_Button *button)
{
    draw_Line(button->rect.x - 1, button->rect.y + button->rect.h  + 2, button->rect.x + button->rect.w + 1, button->rect.y + button->rect.h + 2, 1, 0, &colourBlack, ker_Screen());

    return;
}

/*
    Function: uiTextInput_Create

    Description -

    Allocates memory and returns a Ui_TextInput structure defining a text input region.

    11 arguments:
    int x - The x axis location of the text input region.
    int y - The y axis location of the text input region.
    int layer - Layer of the graphics.
    char *text - The text that will be displayed before the input region.
    TTF_Font *font - Font of the text displayed by the input region.
    int maxLength - Max amount of characters allowed to be in the region.
    int outputType - Whether the output variable is an integer(UI_OUTPUT_INT) or string(UI_OUTPUT_STR).
    void *outputVar - The address of the output variable, this variable receives the value of the text input once finished. Can be NULL and then set later.
    Control_Event *cActivate - The control to be used to enter and exit the input state.
    Ui_Pointer *pnt - The pointer that will be used to activate the text region.
    Timer *srcTimer - Source timer used for input delay.
*/
Ui_TextInput *uiTextInput_Create(int x, int y, int layer, char *text, TTF_Font *font, int maxLength, int outputType, void *outputVar, Control_Event *cActivate, Ui_Pointer *pnt, Timer *srcTimer)
{
    Ui_TextInput *textInput = mem_Malloc(sizeof(Ui_TextInput), __LINE__, __FILE__);

    uiTextInput_Setup(textInput, x, y, layer, text, font, maxLength, outputType, outputVar, cActivate, pnt, srcTimer);

    return textInput;
}

void uiTextInput_Setup(Ui_TextInput *textInput, int x, int y, int layer, char *text, TTF_Font *font, int maxLength, int outputType, void *outputVar, Control_Event *cActivate, Ui_Pointer *pnt, Timer *srcTimer)
{
    int i = 0;
    int buttonWidth = 0;
    int buttonHeight = 0;
    Sprite *buttonSprite = NULL;
    Ui_TextBox *textBox = NULL;

    /*Setup the input buffer*/
    textInput->textLength = maxLength;

    textInput->inputText = (char *)mem_Malloc((maxLength + 1) * sizeof(char), __LINE__, __FILE__);
    for(i = 0; i < maxLength + 1; i++)
        textInput->inputText[i] = '\0';

    /*Setup output variable*/
    textInput->outputType = outputType;
    textInput->outputVar = outputVar;

    /*Setup the letter marking graphic*/
    sprite_Setup(&textInput->sMarker, 0, layer, srcTimer, 1,
                 frame_CreateBasic(0, surf_SimpleBox(2, 2, &colourBlack, &colourBlack, 1), A_FREE));


    /*Create the text box that is to be linked to the button*/
    textBox = uiTextBox_CreateBase(0, 0, layer, 0, NULL, font, tColourBlack, srcTimer);
    uiTextBox_AddText(textBox,
                     0, 0, text, NULL);

    /*Make sure the text starts to the left of the button*/
    uiTextBox_SetPos(textBox, -(int)sprite_Width(textBox->graphic) - 5, 0);

    buttonSprite = sprite_Setup(sprite_Create(),  1, layer, srcTimer, 1,
                              frame_CreateBasic(0, NULL, M_FREE)
    );

    /*Approximate the needed button width by using the '0' character width then
    multiplying it by the maximum text length*/
    TTF_SizeText(font,"0", &buttonWidth, &buttonHeight);
    buttonWidth *= maxLength;

    uiRect_SetDim(&textInput->rect, buttonWidth, buttonHeight);

    uiButton_Setup(&textInput->button, x, y, buttonWidth, buttonHeight, BFA_EnterPoll, BFA_Hover, dataStruct_Create(2, textInput, pnt), buttonSprite, 0, 250, srcTimer);

    uiButton_AddText(&textInput->button, textBox);

    uiButton_CopyControl(&textInput->button, cActivate, BC_POINTER);
    uiButton_CopyControl(&textInput->button, cActivate, BC_KEY);

    uiTextInput_SetPos(textInput, x, y);
    uiTextInput_SetState(textInput, pnt, UI_TEXTINPUT_BASE);
    uiTextInput_AssignInput(textInput);

    depWatcher_Setup(&textInput->dW, textInput);

    return;
}

/*
    Function: uiTextInput_Draw

    Description -

    Draws the input text and button text, also draws
    the character marker if in the poll state.

    3 arguments:
    Ui_TextInput *textInput -
    Camera_2D *camera - The camera to draw relative to.
    SDL_Surface *destination -
*/
void uiTextInput_Draw(Ui_TextInput *textInput, Camera_2D *camera, SDL_Surface *destination)
{
    Ui_Button *button = &textInput->button;
    int markerPos = 0;
    int textHeight = 0;
    char temp = 0;
    SDL_Surface *textSurface = NULL;
    Ui_Rect *rect = NULL;

    uiButton_Draw(button, camera, destination);

    textSurface = text_Arg(button->text->font, &tColourBlack, textInput->inputText);
    rect = uiTextBox_GetRect(button->text);
    if(textSurface != NULL)
        uiRect_SetDim(&textInput->rect, rect->w + textSurface->w, textInput->rect.h);
    else
        uiRect_SetDim(&textInput->rect, rect->w, textInput->rect.h);


    if(camera == NULL)
    {
        surfaceMan_Push(gKer_DrawManager(), A_FREE, button->text->layer, 0, button->rect.x, button->rect.y, textSurface, destination, NULL);
    }
    else if(camera2D_Collision(camera, button->rect.x, button->rect.y, textSurface->w, textSurface->h) == 1)
    {
        surfaceMan_Push(gKer_DrawManager(), A_FREE, button->text->layer, 0, camera2D_RelativeX(camera, button->rect.x), camera2D_RelativeY(camera, button->rect.y), textSurface, destination, NULL);
    }

    switch(textInput->state)
    {
        default:
        case UI_TEXTINPUT_BASE:

        break;

        case UI_TEXTINPUT_POLL:
        /*Obtain the x and y position of the marker dot, based on the text width (so far) and height
        defined by the text character position*/

        temp = textInput->inputText[textInput->textPos];
        textInput->inputText[textInput->textPos] = '\0';

        TTF_SizeText(button->text->font, textInput->inputText, &markerPos, &textHeight);

        textInput->inputText[textInput->textPos] = temp;

        markerPos = markerPos + button->rect.x;

        if(camera == NULL)
        {
            sprite_DrawAtPos(markerPos, button->rect.y + textHeight, &textInput->sMarker, destination);
        }
        else if(camera2D_Collision(camera, button->rect.x, button->rect.y, sprite_Width(&textInput->sMarker), sprite_Height(&textInput->sMarker)) == 1)
        {
            sprite_DrawAtPosFit(markerPos - camera->iPosition.x, button->rect.y + textHeight - camera->iPosition.y, &camera->originRect, &textInput->sMarker, destination);
        }

        break;
    }

    return;
}

/*
    Function: uiTextInput_IsKeyInRange

    Description -

    Checks if the value of the keyboard key supplied is within the correct
    range. The range is determined from the output type.

    Returns 1 if the key is in the correct range. Otherwise 0.

    2 arguments:
    int outputType - Type of variable the output is to be assigned to.
    int key - The keyboard key value that is checked.
*/
int uiTextInput_IsKeyInRange(int outputType, int key)
{
    if(outputType == UI_OUTPUT_STR)
    {
        /*Make sure that the key is only the letters of the alphabet or numbers*/
        if((key >= 'a' && key <= 'z') || (key >= '0' && key <= '9'))
        {
            return 1;
        }

        if(key == SDLK_SPACE)
            return 1;
    }
    else if(outputType == UI_OUTPUT_INT)
    {
        if(key >= '0' && key <= '9')
        {
            return 1;
        }
    }

    return 0;
}

/*
    Function: uiTextInput_AddKey

    Description -

    Adds an ASCI key to the input text. Makes sure that
    the text does not exceed the maximum allowed characters.

    Returns 1 if a key has been added. Otherwise 0.

    2 arguments:
    Ui_TextInput *textInput - The text input structure to add a key to.
    char key - ASCI key to be added.
*/
int uiTextInput_AddKey(Ui_TextInput *textInput, char key)
{
    int x = 0;

    if(textInput->textLength > textInput->textPos)
    {
        for(x = (textInput->textLength - 1); x > textInput->textPos; x--)
        {
            textInput->inputText[x] = textInput->inputText[x - 1];
        }

        textInput->inputText[textInput->textPos] = key;
        textInput->textPos ++;

        return 1;
    }

    return 0;
}

/*
    Function: uiTextInput_RemoveKey

    Description -

    Removes a key from the input text. The location of the key
    is at the character position of the text input.

    Returns 1 if a key has been removed. Otherwise 0.

    1 argument:
    Ui_TextInput *textInput - The text input structure to remove a key from.
*/
int uiTextInput_RemoveKey(Ui_TextInput *textInput)
{
    int x = 0;

    if(textInput->textPos >= 0)
    {
        if(textInput->textPos > 0)
            textInput->textPos --;

        for(x = textInput->textPos; x < textInput->textLength; x++)
        {
            textInput->inputText[x] = textInput->inputText[x + 1];
        }

        return 1;
    }

    return 0;
}

/*
    Function: uiTextInput_UpdateInput

    Description -

    The main polling function for the input region.
    Checks if any keyboard keys have been pressed, decides what
    should be done to those keys.

    Returns 1 if the text input should exit its polling state. Otherwise 0.

    1 argument:
    Ui_TextInput *textInput - The text input structure that is being polled.
*/
int uiTextInput_UpdateInput(Ui_TextInput *textInput)
{
    int addKey = 0;
    SDLMod capitals;
    SDL_Event *keyEvent = NULL;

    /*First check if the exit key has been pressed*/
    if(uiButton_IsActivated(&textInput->button) == 1)
        return 1;

    /*Otherwise grab a keyboard key*/
    keyEvent = control_GetEventType(SDL_KEYDOWN);

    if(keyEvent != NULL)
    {
        addKey = control_GetKeyValue(keyEvent);

        /*Check if the key is in the correct range, from 0-9 or a-z*/
        if(uiTextInput_IsKeyInRange(textInput->outputType, addKey) == 1)
        {
            /*Check if they key should be a capital*/
            capitals = SDL_GetModState();

            if((capitals & KMOD_SHIFT) > 0 || (capitals & KMOD_CAPS) > 0)
            {
                addKey = toupper(addKey);
            }

            uiTextInput_AddKey(textInput, (char)addKey);
        }
        else
        {
            switch(addKey)
            {
                case SDLK_BACKSPACE:

                uiTextInput_RemoveKey(textInput);

                break;

                case SDLK_MINUS:

                /*Only input the minus if the sign is at the start of the text, and the text does not already contain one.*/
                if(textInput->textPos == 0 && string_ReplaceChar(textInput->inputText, '-', '-') == 0)
                {
                    uiTextInput_AddKey(textInput, (char)addKey);
                }

                break;

                case SDLK_RETURN:

                return 1;

                break;

                case SDLK_LEFT:

                if(textInput->textPos > 0)
                    textInput->textPos --;

                break;

                case SDLK_RIGHT:

                if(textInput->textPos < textInput->textLength && textInput->inputText[textInput->textPos] != '\0')
                    textInput->textPos ++;

                break;


                default:
                break;
            }
        }

    }

    return 0;
}

/*
    Function: uiTextInput_AssignInput

    Description -

    Copies the value of the output variable to the input variable.

    1 argument:
    Ui_TextInput *textInput - The text input structure to have its output variable copied.
*/
void uiTextInput_AssignInput(Ui_TextInput *textInput)
{
    int *intVar = NULL;
    char *stringVar = NULL;

    if(textInput->outputVar == NULL)
    {
        strcpy(textInput->inputText, "");

        return;
    }

    switch(textInput->outputType)
    {
        case UI_OUTPUT_STR:

        stringVar = (char *)textInput->outputVar;

        strncpy(textInput->inputText, stringVar, textInput->textLength);

        break;

        case UI_OUTPUT_INT:

        intVar = (int *)textInput->outputVar;

        snprintf(textInput->inputText, (textInput->textLength + 1), "%d", *intVar);

        break;

        default:

        printf("Error: unknown output type %d\n", textInput->outputType);

        break;
    }

    return;
}

/*
    Function: uiTextInput_AssignOutput

    Description -

    Copies the value of the input variable to the output variable.

    1 argument:
    Ui_TextInput *textInput - The text input structure to have its input variable copied.
*/
void uiTextInput_AssignOutput(Ui_TextInput *textInput)
{
    int *intVar = NULL;
    char *stringVar = NULL;

    if(textInput->outputVar == NULL)
        return;

    switch(textInput->outputType)
    {
        case UI_OUTPUT_STR:

        stringVar = (char *)textInput->outputVar;

        strncpy(stringVar, textInput->inputText, textInput->textLength);

        break;

        case UI_OUTPUT_INT:

        intVar = (int *)textInput->outputVar;

        if(textInput->inputText[0] == '\0')
        {
            *intVar = 0;
        }
        else
        {
            *intVar = atoi(textInput->inputText);
        }

        break;

        default:

        printf("Error: unknown output type %d\n", textInput->outputType);

        break;
    }

    return;
}

/*
    Function: uiTextInput_SetOutputVar

    Description -

    Sets a new output variable for the text input to edit.

    2 arguments:
    Ui_TextInput *textInput - The text input structure to assign the output variable to.
    void *outputVar - The output variable which is to be edited.
*/
void uiTextInput_SetOutputVar(Ui_TextInput *textInput, void *outputVar)
{
    textInput->outputVar = outputVar;

    uiTextInput_AssignInput(textInput);

    return;
}


/*
    Function: uiTextInput_Update

    Description -

    Updates the text input structure.

    If the state is UI_TEXTINPUT_BASE then keep checking if the text region is to be
    entered and make sure the input variable matches the output one.

    If the state is UI_TEXTINPUT_POLL then check for keyboard presses and update
    the input variable.

    2 arguments:
    Ui_TextInput *textInput - The text input structure to update.
    Ui_Pointer *pnt - Pointer used to decide whether to enter the text region.
*/
void uiTextInput_Update(Ui_TextInput *textInput, Ui_Pointer *pnt)
{
    switch(textInput->state)
    {
        default:
        case UI_TEXTINPUT_BASE:

        uiTextInput_AssignInput(textInput);
        uiButton_Update(&textInput->button, pnt);

        break;

        case UI_TEXTINPUT_POLL:

        if(uiTextInput_UpdateInput(textInput) == 1)
        {
            /*Input polling has finished so assign this input to the output variable
            and then change state*/
            uiTextInput_AssignOutput(textInput);

            uiTextInput_SetState(textInput, pnt, UI_TEXTINPUT_BASE);
        }

        break;
    }

    return;
}

/*
    Function: uiTextInput_SetPos

    Description -

    Set the position of the text input.

    3 arguments:
    Ui_TextInput *textInput - The text input structure to set the position of.
    int x - The x-axis location.
    int y - The y-axis location.
*/
void uiTextInput_SetPos(Ui_TextInput *textInput, int x, int y)
{
    Vector2DInt labelTextPos = uiTextBox_GetPos(textInput->button.text);
    Vector2DInt buttonPos = uiButton_GetPos(&textInput->button);

    int xDis = buttonPos.x - labelTextPos.x;
    int yDis = buttonPos.y - labelTextPos.y;

    uiButton_SetPos(&textInput->button, x + xDis, y + yDis);

    uiRect_SetPos(&textInput->rect, x, y);

    return;
}

/*
    Function: uiTextInput_ChangePos

    Description -

    Changes the position of the text input.

    3 arguments:
    Ui_TextInput *textInput - The text input structure to change the position of.
    int xDis - The x-axis change.
    int yDis - The y-axis change.
*/
void uiTextInput_ChangePos(Ui_TextInput *textInput, int xDis, int yDis)
{
    uiRect_ChangePos(&textInput->rect, xDis, yDis);

    uiButton_ChangePos(&textInput->button, xDis, yDis);

    return;
}

/*
    Function: uiTextInput_GetPos

    Description -

    Gets the position of the text input, int the form of a 2D integer vector.

    1 argument:
    Ui_TextInput *textInput - The text input to get the position from.
*/
Vector2DInt uiTextInput_GetPos(Ui_TextInput *textInput)
{
    Vector2DInt pos = {textInput->rect.x, textInput->rect.y};

    return pos;
}

/*
    Function: uiTextInput_GetRect

    Description -

    Gets the rectangle representation of the text input structure.

    1 argument:
    Ui_TextInput *textInput - The text input to get the rectangle from.
*/
Ui_Rect *uiTextInput_GetRect(Ui_TextInput *textInput)
{
    return &textInput->rect;
}

/*
    Function: uiTextInput_SetState

    Description -

    Changes the state of the text input structure.

    3 arguments:
    Ui_TextInput *textInput - The text input structure to set the position of.
    Ui_Pointer *pnt - The pointer that is used to check whether to enter the text region.
    int state - The state the text input should change to. (UI_TEXTINPUT_BASE or UI_TEXTINPUT_POLL)
*/
void uiTextInput_SetState(Ui_TextInput *textInput, Ui_Pointer *pnt, int state)
{
    int x = 0;

    switch(state)
    {
        case UI_TEXTINPUT_BASE:

        if(pnt != NULL)
        {
            /*Unhide the pointer*/
            uiPointer_Unclamp(pnt);
            uiPointer_SetDraw(pnt, 1);
        }

        break;

        case UI_TEXTINPUT_POLL:

        if(pnt != NULL)
        {
            /*Hide the pointer*/
            uiPointer_Clamp(pnt);
            uiPointer_SetDraw(pnt, 0);
        }

        /*Make sure that the character position is at the end of the input text*/
        for(x = 0; x < textInput->textLength; x++)
        {
            if(textInput->inputText[x] == '\0')
            {
                break;
            }
        }

        textInput->textPos = x;

        break;

        default:

        printf("Error: unknown text input state %d\n", state);

        return;

        break;
    }

    textInput->state = state;

    return;
}

/*
    Function: uiTextInput_Clean

    Description -

    Clean up any allocated memory created by the text input structure.

    1 argument:
    Ui_TextInput *textInput - The text input structure to clean up
*/
void uiTextInput_Clean(Ui_TextInput *textInput)
{
    uiButton_Clean(&textInput->button);

    mem_Free(textInput->inputText);

    sprite_Clean(&textInput->sMarker);

    depWatcher_Clean(&textInput->dW);

    return;
}
