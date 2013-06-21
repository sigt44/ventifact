#ifndef KEYTEST_H
#define KEYTEST_H
#include "Controls.h"

struct KT_data
{
    int keyValue;
    SDL_Event *checkEvent;
    struct control_Event c_Quit;
    struct control_Event c_TimerTest;
};

void KT_Init(void *info);

void KT_Render(void *info);

void KT_Controls(void *info);

void KT_Exit(void *info);

#endif
