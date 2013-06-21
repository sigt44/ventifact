#ifndef SOUNDS_H
#define SOUNDS_H

#include <SDL/SDL_mixer.h>
#include "Sound.h"

#include "Defines.h"

typedef struct ve_Sounds
{
    Mix_Chunk *bulletFire[VENT_TOTALWEAPONS];
    Mix_Chunk *bulletFlame;
    Mix_Chunk *bulletExplosion;
    Mix_Chunk *unitDestroyed[UNIT_ENDTYPE];
    Mix_Chunk *unitSpawn;

    Mix_Chunk *buildingDestroyed;

    Mix_Chunk *menuButtonChange;
    Mix_Chunk *menuButtonSelect;

    Mix_Chunk *gameWon;
    Mix_Chunk *gameLost;

    Mix_Music *testMusic;
} Ve_Sounds;

extern Ve_Sounds ve_Sounds;

int vSounds_Load(void);

#endif
