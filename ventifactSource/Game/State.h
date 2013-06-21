#ifndef V_STATE_H
#define V_STATE_H

#include "VentGame.h"

#include "Time/Timer.h"
#include "Controls.h"
#include "Common/Fuzzy/FuzzySet.h"
#include "Common/Fuzzy/FuzzyLV.h"
#include "Camera.h"

#include "../Level/Sector.h"

typedef struct Vent_GameState
{
    SDL_Surface *black_Fill;

    Vent_Game game;

    Control_Event cUp;
    Control_Event cLeft;
    Control_Event cRight;
    Control_Event cDown;

    Control_Event cShootUp;
    Control_Event cShootDown;
    Control_Event cShootLeft;
    Control_Event cShootRight;
    Control_Event cShootUpRight;
    Control_Event cShootUpLeft;
    Control_Event cShootDownRight;
    Control_Event cShootDownLeft;

    Control_Event cExit;
    Control_Event cFPSCAP;
    Control_Event cHealth;
    Control_Event cTest;
    Control_Event cPause;
    Control_Event cChooseUnit;
    Control_Event cBuild;
    Control_Event cCycleWeapon;
    Control_Event cCall;

    Timer finishTimer; /*Timer to exit automatically after game has ended*/

    Timer fpsTimer;
    float fps;

} Vent_GameState;

void Game_Init(void *info);

void Game_Controls(void *info);

void Game_Logic(void *info);

void Game_LogicFinished(void *info);

void Game_RenderUnitSelect(void *info);

void Game_Render(void *info);

void Game_Exit(void *info);

#endif
