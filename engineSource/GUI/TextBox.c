#include "TextBox.h"

#include "../Font.h"
#include "../Graphics/Surface.h"
#include "../Kernel/Kernel.h"
#include "../DataStruct.h"

Ui_TextBox *uiTextBox_CreateBase(int x, int y, int layer, int lineSpacing, SDL_Surface *background, TTF_Font *font, SDL_Color textColour, Timer *srcTime)
{
    Ui_TextBox *textBox = (Ui_TextBox *)mem_Malloc(sizeof(Ui_TextBox), __LINE__, __FILE__);

    uiRect_Setup(&textBox->rect, x, y, 0, 0);

    textBox->indentX = 0;
    textBox->indentY = 0;

    textBox->graphic = sprite_Create();

    textBox->layer = layer;
    textBox->numLines = 0;

    textBox->text = NULL;
    textBox->textList = NULL;

    textBox->font = font;
    textBox->textColour = textColour;

    textBox->lineSpacing = lineSpacing + TTF_FontHeight(font);

    textBox->variables = NULL;

    if(background != NULL)
    {
        sprite_Setup(textBox->graphic, 0, layer, srcTime, 1,
                     frame_CreateBasic(-1, background, A_FREE)
        );
    }
    else
    {
        sprite_Setup(textBox->graphic, 0, layer, srcTime, 0);
    }

    sprite_SetPos(textBox->graphic, textBox->rect.x, textBox->rect.y);

    return textBox;
}

int uiTextBox_ReplaceText(void **nodeData, char *replaceText, int length)
{
    char *baseText = (char *)*nodeData;

    if(baseText == NULL)
    {
        baseText = (char *)mem_Malloc((length + 1) * sizeof(char), __LINE__, __FILE__);

        strncpy(baseText, replaceText, length);

        *nodeData = baseText;

        return 1;
    }
    else if(strcmp(baseText, replaceText) != 0)
    {
        mem_Free(baseText);

        baseText = (char *)mem_Malloc((length + 1) * sizeof(char), __LINE__, __FILE__);

        strncpy(baseText, replaceText, length);

        *nodeData = baseText;

        return 2;
    }

    return 0;
}

int uiTextBox_CreateLine(struct list **textListHead, struct list **textList, char *textBuffer, int textLength)
{
    /*If textList is NULL then a new line should be created*/
    if(*textList == NULL)
    {
        list_Stack(textListHead, NULL, 1);

        /*If textList was not at the head of the list*/
        if((*textListHead)->previous != NULL)
            *textList = (*textListHead)->previous; /*Set it to the end of the list*/
        else
            *textList = *textListHead; /*There was no previous new lines so just set it back to the head of the list*/
    }

    /*try and place the text into the line*/
    (*textList)->info = uiTextBox_ReplaceText(&(*textList)->data, textBuffer, textLength);

    return (*textList)->info;
}

int uiTextBox_ConvertText(char *textBuffer, char **textMarker, int markPos, int *varPos, int maxParse, Data_Struct *variables)
{
    char printfMark[3] = {'%', '%', '\0'}; /*This is to be placed into sprintf function*/
    int varLength = 0;

    int isArray = 0;
    int *arrayValue = 0;

    int *intValue = NULL;
    float *floatValue = NULL;
    void *pointerValue = NULL;
    void **pointerArrayValue = NULL;
    char **charValue = NULL;
    char *charArrayValue = NULL;

    char *varMarker = *textMarker;

    /*Obtain the next character for more infomation*/
    printfMark[1] = *(varMarker + 1);

    if(printfMark[1] != '\0')
    {
        /*Needed to check if the variable is an array that also requires an index to find
        its element*/
        if( *(varMarker + 2) == 'i')
        {
            isArray = 1;
        }
    }

    //file_Log(NULL, P_OUT, "\tMark found[%d + 1] = %c -> %s, using array %d\n", markPos,  *(varMarker + 1), printfMark, isArray);

    switch(printfMark[1])
    {
        case '%':
        /*Treat as a single % character and move forwards*/
        textBuffer[markPos] = *varMarker;
        *textMarker++;

        break;

        case '\0':
        /*Do nothing, this will be caught in the loop*/
        break;

        case 'p':

        pointerValue = variables->dataArray[*varPos];

        if(isArray == 1)
        {
            arrayValue = (int *)variables->dataArray[*varPos + 1];
            pointerArrayValue = pointerValue;
            pointerValue = pointerArrayValue[*arrayValue];
            *varPos = *varPos + 2;
            *textMarker = *textMarker + 2;
        }
        else
        {
            *varPos = *varPos + 1;
            *textMarker = *textMarker + 1;
        }

        varLength = snprintf(&textBuffer[markPos], maxParse - markPos, printfMark, pointerValue) - 1;

        break;

        case 's':
        case 'c':

        charValue = (char **)variables->dataArray[*varPos];

        if(isArray == 1)
        {
            arrayValue = (int *)variables->dataArray[*varPos + 1];

            charValue = &charValue[*arrayValue];
            *varPos = *varPos + 2;
            *textMarker = *textMarker + 2;
        }
        else
        {
            *varPos = *varPos + 1;
            *textMarker = *textMarker + 1;
        }

        varLength = snprintf(&textBuffer[markPos], maxParse - markPos, printfMark, *charValue) - 1;

        break;

        case 'a':

        charArrayValue = (char *)variables->dataArray[*varPos];

        if(isArray == 1)
        {
            arrayValue = (int *)variables->dataArray[*varPos + 1];

            charArrayValue = &charArrayValue[*arrayValue];
            *varPos = *varPos + 2;
            *textMarker = *textMarker + 2;
        }
        else
        {
            *varPos = *varPos + 1;
            *textMarker = *textMarker + 1;
        }

        varLength = snprintf(&textBuffer[markPos], maxParse - markPos, "%s", charArrayValue) - 1;

        break;

        case 'd':

        intValue = (int *)variables->dataArray[*varPos];

        if(isArray == 1)
        {
            arrayValue = (int *)variables->dataArray[*varPos + 1];
            intValue = &intValue[*arrayValue];
            *varPos = *varPos + 2;
            *textMarker = *textMarker + 2;
        }
        else
        {
            *varPos = *varPos + 1;
            *textMarker = *textMarker + 1;
        }

        varLength = snprintf(&textBuffer[markPos], maxParse - markPos, printfMark, *intValue) - 1;

        break;

        case 'f':

        floatValue = (float *)variables->dataArray[*varPos];

        if(isArray == 1)
        {
            arrayValue = (int *)variables->dataArray[*varPos + 1];
            floatValue = &floatValue[*arrayValue];
            *varPos = *varPos + 2;
            *textMarker = *textMarker + 2;
        }
        else
        {
            *varPos = *varPos + 1;
            *textMarker = *textMarker + 1;
        }

        varLength = snprintf(&textBuffer[markPos], maxParse - markPos, printfMark, *floatValue) - 1;
        break;

        default:

        printf("Warning unknown tag (%s) for text parse\n", printfMark);

        break;
    }


    //printf("[%d]printed var(%c, %s) of length %d\n", markPos, printfMark[1], printfMark, varLength);
    return varLength;
}

int uiTextBox_ParseText(Ui_TextBox *textBox)
{
    const int maxParse = 255;

    char textBuffer[256];

    int x = 0;
    int varPos = 0;
    int textChanged = 0;

    struct list *textList = textBox->textList;

    char *parseText = textBox->text;

    Data_Struct *variables = textBox->variables;

    textBox->numLines = 0;

    /*Go through all the characters in the base text*/
    while(*parseText != '\0' && x < maxParse)
    {
        //printf("\t[%d] = %c\n", x, *parseText);

        /*If the character might be a variable sign*/
        if(*parseText == '%')
        {
            x += uiTextBox_ConvertText(&textBuffer[0], &parseText, x, &varPos, maxParse, variables);
        }
        else if(*parseText == '\n') /*If the character is to set a new line*/
        {
            /*end the current text buffer*/
            textBuffer[x] = '\0';

            /*Create the line and enter the text into it*/
            if(uiTextBox_CreateLine(&textBox->textList, &textList, textBuffer, x + 1) > 0)
            {
                textChanged ++;
            }

            textBox->numLines ++;

            textList = textList->next;
            x = -1;
        }
        else
        {
           // printf("\t[%d] = %c\n", x, *parseText);
            textBuffer[x] = *parseText;
        }

        *parseText ++;

        x++;
    }

    //printf("Setting end of string at %d\n", x);

    if(x >= maxParse)
    {
        printf("Warning text length is too long for text box\n");

        return textChanged;
    }

    /*end the final line*/
    textBuffer[x] = '\0';
    //printf("Text is %s\n", textBuffer);

    if(x != 0) /*If there was actually a final line*/
    {
        if(uiTextBox_CreateLine(&textBox->textList, &textList, textBuffer, x + 1) > 0)
        {
            textChanged ++;
            textBox->numLines ++;
        }

        textList = textList->next;
    }

    /*If there are still lines remaining from the last update, set them to be deleted*/
    while(textList != NULL)
    {
        if(textList->info != -2)
        {
            textList->info = -1;

            textChanged ++;
        }

        textList = textList->next;
    }

    return textChanged;
}

Ui_TextBox *uiTextBox_UpdateParse(Ui_TextBox *textBox)
{
    /*Check if the value of the variables have changed, update the graphic if so*/
    if(textBox->variables != NULL && uiTextBox_ParseText(textBox) > 0)
    {
         uiTextBox_UpdateGraphic(textBox);
    }

    return textBox;
}

void uiTextBox_UpdateGraphic(Ui_TextBox *t)
{
    int x = 0;
    SDL_Surface *textSurface = NULL;

    struct list *textList = t->textList;

    /*Go through all the lines*/
    while(textList != NULL)
    {
        /*If this line needs to be redrawn*/
        if(textList->info > 0)
        {
            textSurface = text_Arg(t->font, &t->textColour, textList->data);

            /*If this line is replacing a frame*/
            if(textList->info == 2)
            {
                sprite_ReplaceFrame(t->graphic, frame_CreateOffset(-1, textSurface, t->indentX, t->indentY + (t->lineSpacing * x), A_FREE), x);
            }
            else
            {
                /*Otherwise add in a new frame*/
                sprite_AddFrame(t->graphic, frame_CreateOffset(-1, textSurface, t->indentX, t->indentY + (t->lineSpacing * x), A_FREE));
            }

            textList->info = 0;
        }
        else if(textList->info == -1) /*Line needs to be removed*/
        {
            sprite_RemoveFrame(t->graphic, x);
            x--;
            textList->info = -2;

            mem_Free(textList->data);
            textList->data = NULL;
        }

        x++;
        textList = textList->next;
    }

    uiRect_SetDim(&t->rect, sprite_Width(t->graphic), sprite_Height(t->graphic));

    return;
}

void uiTextBox_AddText(Ui_TextBox *t, int indentX, int indentY, char *text, Data_Struct *variables)
{
    t->text = text;
    t->variables = variables;

    t->indentX = indentX;
    t->indentY = indentY;

    uiTextBox_ParseText(t);

    uiTextBox_UpdateGraphic(t);

    return;
}

void uiTextBox_ChangeText(Ui_TextBox *t, int indentX, int indentY, char *text, Data_Struct *variables)
{
    if(t->variables != variables && t->variables != NULL)
    {
        dataStruct_Clean(t->variables);
        mem_Free(t->variables);
    }

    uiTextBox_AddText(t, indentX, indentY, text, variables);

    return;
}

void uiTextBox_Draw(Ui_TextBox *t, Camera_2D *camera, SDL_Surface *destination)
{
    if(camera == NULL)
    {
        sprite_Draw(t->graphic, destination);
    }
    else if(camera2D_Collision(camera, t->graphic->x, t->graphic->y, sprite_Width(t->graphic), sprite_Height(t->graphic)) == 1)
    {
        sprite_DrawAtPosFit(camera2D_RelativeX(camera, t->graphic->x), camera2D_RelativeY(camera, t->graphic->y), &camera->originRect, t->graphic, destination);
    }

    return;
}

void uiTextBox_SetPos(Ui_TextBox *t, int x, int y)
{
    uiRect_SetPos(&t->rect, x, y);

    sprite_SetPos(t->graphic, x, y);

    return;
}

void uiTextBox_ChangePos(Ui_TextBox *t, int xDis, int yDis)
{
    uiRect_ChangePos(&t->rect, xDis, yDis);

    sprite_ChangePos(t->graphic, xDis, yDis);

    return;
}

Vector2DInt uiTextBox_GetPos(Ui_TextBox *t)
{
    Vector2DInt pos = {t->rect.x, t->rect.y};

    return pos;
}

Ui_Rect *uiTextBox_GetRect(Ui_TextBox *t)
{
    return &t->rect;
}

void uiTextBox_Clean(Ui_TextBox *t)
{
    while(t->textList != NULL)
    {
        if(t->textList->data != NULL)
        {
            mem_Free(t->textList->data);
        }

        list_Pop(&t->textList);
    }

    if(t->variables != NULL)
    {
        dataStruct_Clean(t->variables);
        mem_Free(t->variables);
    }

    sprite_Clean(t->graphic);

    mem_Free(t->graphic);

    return;
}
