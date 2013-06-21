#ifndef BUTTONCHAIN_H
#define BUTTONCHAIN_H

#include "Button.h"

typedef struct ui_ButtonChain
{
    struct list *bList;
    struct list *currentList;
    Ui_Button *currentButton;
} Ui_ButtonChain;

Ui_ButtonChain *uiButtonChain_Create(unsigned int numButtons, ...);

void uiButtonChain_Target(Ui_ButtonChain *bChain, int offset, int activate, int disablePrev);

void uiButtonChain_Disable(Ui_ButtonChain *bChain);

void uiButtonChain_Draw(Ui_ButtonChain *bChain, SDL_Surface *destination);

void uiButtonChain_Update(Ui_ButtonChain *bChain, Ui_Pointer *p);

void uiButtonChain_Clean(Ui_ButtonChain *bChain);

#endif
