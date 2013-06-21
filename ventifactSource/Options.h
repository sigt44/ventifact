#ifndef OPTIONS_H
#define OPTIONS_H

struct options_Holder
{
    /*Sound*/
    int mute;
    int volume;

    /*Gameplay*/
    int teamAi;
    int difficulty;
    int cheat;

    /*Visual*/
    int showFPS;

    char lastSave[4];
};

extern struct options_Holder ve_Options;

void options_SetDefaults(void);

#endif
