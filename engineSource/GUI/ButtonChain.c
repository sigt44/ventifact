#include "ButtonChain.h"
#include <stdarg.h>

#include "../Common/List.h"

Ui_ButtonChain *uiButtonChain_Create(unsigned int numButtons, ...)
{
    Ui_ButtonChain *newChain = (Ui_ButtonChain *)mem_Malloc(sizeof(Ui_ButtonChain), __LINE__, __FILE__);
    Ui_Button *button = NULL;
    va_list args;
    unsigned int x = 0;

    va_start(args, numButtons);

    button = va_arg( args, Ui_Button *);

    newChain->bList = NULL;

    newChain->currentButton = button;
    list_Stack(&newChain->bList, button, 0);

    for(x = 1; x < numButtons; x++)
    {
        button = va_arg( args, Ui_Button *);

        /*Put the buttons in the list*/
        list_Stack(&newChain->bList, button, 0);
    }

    newChain->currentList = newChain->bList;

    va_end(args); /*Always call this in the same function as va_start.*/

    return newChain;
}

/*Change the main activated button in the chain*/
void uiButtonChain_Target(Ui_ButtonChain *bChain, int offset, int activate, int disablePrev)
{
    int x = 0;
    if(disablePrev == 1)
        bChain->currentButton->enabled = 0;

    if(offset > 0)
    {
        for(x = 0; x < offset; x++)
        {
            bChain->currentList = bChain->currentList->next;
            if(bChain->currentList == NULL) /*Head the to start of the list*/
                bChain->currentList = bChain->bList;
        }
    }
    else
    {
        for(x = 0; x > offset; x--)
        {
            bChain->currentList = bChain->currentList->previous;
        }
    }

    bChain->currentButton = bChain->currentList->data;

    if(activate == 1)
        bChain->currentButton->enabled = 1;

    return;
}

/*Disable all the buttons in a chain*/
void uiButtonChain_Disable(Ui_ButtonChain *bChain)
{
    struct list *t = bChain->bList;
    Ui_Button *button = NULL;

    while(t != NULL)
    {
        button = t->data;
        button->enabled = 0;
        t = t->next;
    }

    return;
}

void uiButtonChain_Draw(Ui_ButtonChain *bChain, SDL_Surface *destination)
{
    struct list *t = bChain->bList;
    Ui_Button *button = NULL;

    while(t != NULL)
    {
        uiButton_Draw(t->data, NULL, destination);
        t = t->next;
    }

    return;
}

void uiButtonChain_Update(Ui_ButtonChain *bChain, Ui_Pointer *p)
{
    struct list *t = bChain->bList;
    Ui_Button *button = NULL;

    while(t != NULL)
    {
        uiButton_Update(t->data, p);
        t = t->next;
    }

    return;
}

void uiButtonChain_Clean(Ui_ButtonChain *bChain)
{
    bChain->currentList = NULL;
    bChain->currentButton = NULL;

    while(bChain->bList != NULL)
    {
        uiButton_Clean(bChain->bList->data);
        mem_Free(bChain->bList->data);

        list_Pop(&bChain->bList);
    }

    return;
}
