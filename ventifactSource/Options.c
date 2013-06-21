#include "Options.h"

struct options_Holder ve_Options;

void options_SetDefaults(void)
{
    ve_Options.mute = 0;
    ve_Options.volume = 100;

    ve_Options.teamAi = 0;
    ve_Options.difficulty = 1;
    ve_Options.cheat = 0;

    ve_Options.showFPS = 0;

    ve_Options.lastSave[0] = '%';
    ve_Options.lastSave[1] = '\0';

    return;
}
