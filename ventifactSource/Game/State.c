#include "State.h"

#include "Kernel/Kernel_State.h"
#include "BaseState.h"
#include "Time/Fps.h"
#include "Font.h"
#include "Graphics/Pixel.h"
#include "Graphics/Graphics.h"
#include "Graphics/SurfaceManager.h"

#include "../Ai/Ai_State.h"
#include "../Ai/Units/Ai_UnitAttack.h"
#include "../Ai/Units/Ai_UnitMove.h"
#include "../Ai/Units/Ai_UnitFunction.h"
#include "../Images.h"
#include "../Buildings/Buildings.h"
#include "../Units/Supply.h"
#include "../Weapon.h"
#include "../Bullets.h"
#include "../Level/Level.h"
#include "../Level/Tiles.h"
#include "../Options.h"
#include "HUD.h"
#include "../ControlMap.h"

static void Game_SetControls(Vent_GameState *vgs)
{
    control_CopyKey(&vgs->cPause, &ve_Controls.cPause);
    control_CopyKey(&vgs->cBuild, &ve_Controls.cBuy);
    control_CopyKey(&vgs->cCycleWeapon, &ve_Controls.cCycleWeapon);
    control_CopyKey(&vgs->cCall, &ve_Controls.cCall);

    control_CopyKey(&vgs->cDown, &ve_Controls.cMoveDown);
    control_CopyKey(&vgs->cUp, &ve_Controls.cMoveUp);
    control_CopyKey(&vgs->cLeft, &ve_Controls.cMoveLeft);
    control_CopyKey(&vgs->cRight, &ve_Controls.cMoveRight);

    control_CopyKey(&vgs->cShootRight, &ve_Controls.cShootRight);
    control_CopyKey(&vgs->cShootLeft, &ve_Controls.cShootLeft);
    control_CopyKey(&vgs->cShootUp, &ve_Controls.cShootUp);
    control_CopyKey(&vgs->cShootUpLeft, &ve_Controls.cShootUpLeft);
    control_CopyKey(&vgs->cShootUpRight, &ve_Controls.cShootUpRight);
    control_CopyKey(&vgs->cShootDown, &ve_Controls.cShootDown);
    control_CopyKey(&vgs->cShootDownLeft, &ve_Controls.cShootDownLeft);
    control_CopyKey(&vgs->cShootDownRight, &ve_Controls.cShootDownRight);

    #if defined (LINUX) || defined (WINDOWS)
    control_AddKey(&vgs->cExit, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_q);
    control_AddKey(&vgs->cTest, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_c);
    control_AddKey(&vgs->cFPSCAP, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_f);
    control_AddKey(&vgs->cHealth, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_h);
    #endif

    #ifdef PANDORA
    control_AddKey(&vgs->cExit, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_q);
    control_AddKey(&vgs->cFPSCAP, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_f);
    control_AddKey(&vgs->cHealth, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_h);
    control_AddKey(&vgs->cTest, C_KEYBOARD, C_BUTTON_DOWN, 1, SDLK_c);
    #endif

    return;
}

void Game_DrawBanner(Vent_GameState *vgs, int time, char *text)
{
    SDL_Surface *textSurface = NULL;

    /*Draw the background banner*/
    surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_HUD + 2, time, 0, (ker_Screen_Height()/2) - (vgs->black_Fill->h/2), vgs->black_Fill, ker_Screen(), NULL);

    /*Get the text in the banner*/
    textSurface = text_Arg(font_Get(2, 22), &tColourWhite, text);

    /*Draw text in the middle of the screen*/
    surfaceMan_Push(gKer_DrawManager(), A_FREE, VL_HUD + 2, time, (ker_Screen_Width()/2) - (textSurface->w/2), (ker_Screen_Height()/2) - (textSurface->h/2), textSurface, ker_Screen(), NULL);
    return;
}

void Game_Init(void *info)
{
    Base_State *bs = info;
    Vent_Player *p = bs->info;
    Vent_GameState *vgs = (Vent_GameState *)mem_Malloc(sizeof(Vent_GameState), __LINE__, __FILE__);

    struct list *temp = NULL;

    int x = 0;
    Vent_Unit *u = NULL;

	bs->info = vgs;

    vGame_Setup(&vgs->game, p);

    /*string_RemoveExt(p->levelChosen);*/
    vGame_SetupLevel(&vgs->game, p->levelChosen);

    /*temp = NULL;
    list_Stack(&temp, aiPatrol_Create(200, 200, 1000), 0);
    list_Stack(&temp, aiPatrol_Create(180, 200, 1000), 0);
    list_Stack(&temp, aiPatrol_Create(160, 200, 1000), 0);
    list_Stack(&temp, aiPatrol_Create(140, 200, 1000), 0);
    list_Stack(&temp, aiPatrol_Create(120, 200, 1000), 0);
    list_Stack(&temp, aiPatrol_Create(100, 200, 1000), 0);

    vUnit_GiveAi(vgs->game.playerUnit,
                    aiState_UnitPatrol_Setup(aiState_Create(),
                        AI_GLOBAL_STATE + 1,
                        1,
                        AI_COMPLETE_SINGLE,
                        vgs->game.playerUnit,
                        temp,
                        2,
                        0,
                        &vgs->game.gTimer
                        )
                 );*/

    #ifdef dontdef
    vUnit_GiveAi(vgs->game.playerUnit,
        aiState_UnitSelectWeapon_Setup(aiState_Create(),
        AI_GLOBAL_STATE,    /*group*/
        5,                  /*priority*/
        AI_COMPLETE_SINGLE, /*complete type*/
        vgs->game.playerUnit,                  /*entity*/
        1000,               /*secondary fire length*/
        2000,              /*update time*/
        &vgs->game.gTimer       /*source timer*/
        )
    );

    vUnit_GiveAi(vgs->game.playerUnit,
        aiState_UnitObtainTargets_Setup(aiState_Create(),
        AI_GLOBAL_STATE,    /*group*/
        5,                  /*priority*/
        AI_COMPLETE_SINGLE, /*complete type*/
        vgs->game.playerUnit,                  /*entity*/
        175,
        5000,
        &vgs->game
        )
    );
    #endif

    temp = NULL;
    /*vUnit_GiveDefaultAi(vgs->game.playerUnit, &vgs->game);*/

    control_Setup(&vgs->cUp, "Up", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cDown, "Down", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cLeft, "Left", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cRight, "Right", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootLeft, "Shoot left", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootRight, "Shoot right", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootUp, "Shoot up", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootDown, "Shoot down", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootDownRight, "Shoot down left", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootDownLeft, "Shoot down right", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootUpLeft, "Shoot up left", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cShootUpRight, "Shoot up right", &vgs->game.gTimer, 0);

    control_Setup(&vgs->cExit, "Exit game", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cTest, "Test", &vgs->game.gTimer, 1000);
    control_Setup(&vgs->cFPSCAP, "FPS Cap", &vgs->game.gTimer, 200);
    control_Setup(&vgs->cHealth, "Health", &vgs->game.gTimer, 200);
    control_Setup(&vgs->cPause, "Pause", NULL, 1000);
    control_Setup(&vgs->cChooseUnit, "Choose unit", &vgs->game.gTimer, 1000);
    control_Setup(&vgs->cBuild, "Build", &vgs->game.gTimer, 1000);
    control_Setup(&vgs->cCycleWeapon, "Cycle Weapon", &vgs->game.gTimer, 0);
    control_Setup(&vgs->cCall, "Call units", &vgs->game.gTimer, 0);

    Game_SetControls(vgs);

    vgs->fpsTimer = timer_Setup(NULL, 0, 0, 1);
    vgs->finishTimer = timer_Setup(NULL, 0, 20000, 0);

    vgs->fps = 0;

    /*Setup the win/lose background*/
    vgs->black_Fill = surf_Create(ker_Screen_Width(), ker_Screen_Height()/5);

    fill_Rect(0, 0, vgs->black_Fill->w, vgs->black_Fill->h, &colourBlack, vgs->black_Fill);
    SDL_SetAlpha(vgs->black_Fill, SDL_SRCALPHA | SDL_RLEACCEL, SDL_FAST_ALPHA);

    pixel_Line(0, 0, vgs->black_Fill->w, 0, 2, 0, &colourWhite, vgs->black_Fill);
    pixel_Line(0, vgs->black_Fill->h - 2, vgs->black_Fill->w, vgs->black_Fill->h - 2, 2, 0, &colourWhite, vgs->black_Fill);

    #if defined (LINUX) || defined (WINDOWS)
    gKer_SetFPS(30);
    #else
    gKer_SetFPS(30);
    #endif

    return;
}

void Game_Controls(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;
    static int fps = 1;
    int x = 0;

    Vent_Unit *unit = NULL;

    int shootDirection = -1;

    if(control_IsActivated(&vgs->cExit) == 1)
    {
        baseState_CallQuit(bs, 1);
        return;
    }

    if(control_IsActivated(&vgs->cFPSCAP) == 1)
    {
        if(fps == 0)
        {
            fps = 1;
            gKer_SetFPS(30);
        }
        else
        {
            fps = 0;
            gKer_SetFPS(-1);
        }
    }

    if(ve_Options.cheat == 1 && control_IsActivated(&vgs->cHealth) == 1)
    {
        if(vgs->game.playerUnit != NULL)
        {
            vgs->game.playerUnit->health = vgs->game.playerUnit->healthStarting;



            #ifdef DONTDEF
            vStats_Update(&vgs->game.stats, &vgs->game);
            vStats_Report(&vgs->game.stats);

            unit = vGame_AddUnit(&vgs->game, vUnit_Create(vgs->game.playerUnit->middlePosition.x, vgs->game.playerUnit->middlePosition.y, TEAM_1, rand() % UNIT_ENDTYPE, 0, 1, NULL, &vgs->game.gTimer));

            vUnit_GiveAi(unit,
                aiState_UnitReportAi_Setup(aiState_Create(),
                AI_GLOBAL_STATE + 2,    /*group*/
                2,                  /*priority*/
                AI_COMPLETE_SINGLE, /*complete type*/
                unit,                  /*entity*/
                NULL,         /*ai type tracking*/
                &vgs->game.side[unit->team],       /*side of the unit*/
                0             /*exit as soon as it is run*/
                )
            );
            #endif
        }
    }

    if(control_IsActivated(&vgs->cPause) == 1)
    {
        if(timer_Get_Stopped(&vgs->game.gTimer) == 0)
            timer_Pause(&vgs->game.gTimer);
        else
            timer_Resume(&vgs->game.gTimer);
    }

    if(control_IsActivated(&vgs->cCall) == 1)
    {
        aiUnit_CallIn(vgs->game.playerUnit, vgs->game.side[vgs->game.playerUnit->team].units);

        sprite_DrawAtPos(camera2D_RelativeX(&vgs->game.camera, vUnit_MiddleX(vgs->game.playerUnit)) - (sprite_Width(&ve_Sprites.call)/2),
                         camera2D_RelativeY(&vgs->game.camera, vUnit_MiddleY(vgs->game.playerUnit)) - (sprite_Height(&ve_Sprites.call)/2),
                         &ve_Sprites.call, ker_Screen());
    }

    if(control_IsActivated(&vgs->cCycleWeapon) == 1)
    {
        vWeapon_CycleBullet(&vgs->game.playerUnit->weapon, 1);
    }

    if(control_IsActivated(&vgs->cBuild) == 1)
    {
        timer_Pause(&vgs->game.gTimer);
        timer_Skip(&vgs->game.gTimer, 1);

        timer_Stop(&vgs->game.HUD.timer, 1000);
        control_Stop(&vgs->cShootRight, 1000);
        control_Stop(&vgs->cShootDown, 1000);

        /*If a building can be built, buy buildings*/
        if(vBuilding_CanBuild(vgs->game.playerUnit, &vgs->game) == 1)
        {
            vBuildingBar_Start(&vgs->game.HUD.buildBar, bs, vgs->game.buildTile);
        }
        else /*Otherwise buy units*/
        {
            vUnitMenu_Init(&vgs->game.HUD.unitMenu, bs);
        }
    }

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        if(control_IsActivated(&ve_Controls.cBuyUnit[x]) == 1)
        {
            /*Purchase a unit that will be spawned in the middle of the players unit.*/
            vUnitMenu_PurchaseShortcut(&vgs->game.HUD.unitMenu, x, vUnit_MiddleX(vgs->game.playerUnit), vUnit_MiddleY(vgs->game.playerUnit));

            break;
        }
    }

    if(control_IsActivated(&vgs->cDown) == 1)
    {
        vgs->game.playerUnit->targetForce.y = vgs->game.playerUnit->steeringForce;
    }
    else if(control_IsActivated(&vgs->cUp) == 1)
    {
        vgs->game.playerUnit->targetForce.y = -vgs->game.playerUnit->steeringForce;
    }

    if(control_IsActivated(&vgs->cLeft) == 1)
    {
        vgs->game.playerUnit->targetForce.x = -vgs->game.playerUnit->steeringForce;
    }
    else if(control_IsActivated(&vgs->cRight) == 1)
    {
        vgs->game.playerUnit->targetForce.x = vgs->game.playerUnit->steeringForce;
    }

    if(control_IsActivated(&vgs->cShootDownRight) == 1)
    {
        shootDirection = D_DOWNRIGHT;
    }
    else if(control_IsActivated(&vgs->cShootDownLeft) == 1)
    {
        shootDirection = D_DOWNLEFT;
    }
    else if(control_IsActivated(&vgs->cShootUpLeft) == 1)
    {
        shootDirection = D_UPLEFT;
    }
    else if(control_IsActivated(&vgs->cShootUpRight) == 1)
    {
        shootDirection = D_UPRIGHT;
    }
    else if(control_IsActivated(&vgs->cShootRight))
    {
        shootDirection = D_RIGHT;
    }
    else if(control_IsActivated(&vgs->cShootLeft) == 1)
    {
        shootDirection = D_LEFT;
    }
    else if(control_IsActivated(&vgs->cShootDown) == 1)
    {
        shootDirection = D_DOWN;
    }
    else if(control_IsActivated(&vgs->cShootUp) == 1)
    {
        shootDirection = D_UP;
    }

    if(shootDirection != -1)
    {
        switch(vWeapon_Fire(&vgs->game.playerUnit->weapon,
                            &vgs->game,
                            (int)vgs->game.playerUnit->position.x,
                            (int)vgs->game.playerUnit->position.y,
                            vWeapon_DirectionPreset(shootDirection, 0, 0, 0))
              )
        {
            case WEAPON_FIRED:
            vgs->game.stats.bulletsFired ++;
            break;

            case WEAPON_NOAMMO:
            /*puts("No ammo");*/
            break;

            case WEAPON_REFIRE:
            /*puts("Reloading...");*/
            break;
        }
    }

    return;
}

void Game_Logic(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;
    float dt = 0.0f;

    int x = 0;

    /*Force constant time step*/
    vgs->game.staticTimer.current_Time += 32;

    timer_Calc(&vgs->game.gTimer);

    vgs->fps = fps_Calc_Now(&vgs->fpsTimer);
    timer_Start(&vgs->fpsTimer);

    if(vgs->fps < 10.0f)
    {
        //file_Log(ker_Log(), 0, "[%d]: Warning frame rate is low %.2f\n", timer_Get_Passed(gKer_Timer(), 0), vgs->fps);
    }

    if(!timer_Get_Paused(&vgs->game.gTimer))
    {
        vgs->game.deltaTime = dt = 0.0320f;

        vLevel_Update(vgs->game.level, vgs->game.units);

        vGame_UpdateSides(&vgs->game);

        vGame_UpdateUnits(&vgs->game, dt);

        vGame_UpdateBuildings(&vgs->game, dt);

        vGame_UpdateTiles(&vgs->game, dt);

        vGame_UpdateSupply(&vgs->game);

        vGame_UpdateBullets(&vgs->game, dt);

        if(vRules_Update(&vgs->game.rules, &vgs->game) == VR_GAME_FINISHED && bs->logic != Game_LogicFinished)
        {
            bs->logic = Game_LogicFinished;

            /*Update player and campaign stats*/
            vStats_Update(&vgs->game.stats, &vgs->game);
            vRules_UpdateStats(&vgs->game.rules, &vgs->game, vgs->game.player);

            if(vgs->game.rules.teamWon == TEAM_PLAYER)
            {
                Game_DrawBanner(vgs, 10000, "Mission Successful!");
            }
            else
            {
                Game_DrawBanner(vgs, 10000, "Mission Failed!");
            }

            timer_Start(&vgs->finishTimer);
        }
        else
        {
            if(vgs->game.playerUnit != NULL)
            {
                camera2D_SetPosition(&vgs->game.camera, vgs->game.playerUnit->iPosition.x, vgs->game.playerUnit->iPosition.y);
            }
        }
    }

    return;
}

void Game_LogicFinished(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;
    Vent_Rules *rules = &vgs->game.rules;

    /*Perform main game logic*/
    Game_Logic(info);

    camera2D_SetPosition(&vgs->game.camera, rules->keyPosition.x, rules->keyPosition.y);

    timer_Calc(&vgs->finishTimer);

    /*If the timer is up exit the game*/
    if(timer_Get_Remain(&vgs->finishTimer) <= 0)
    {
        baseState_CallQuit(bs, 1);
    }

    return;
}

void Game_Render(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    struct list *units = vgs->game.units;
    struct list *bullets = vgs->game.bullets;
    struct list *buildings = vgs->game.buildings;

    /*surfaceMan_Push(gKer_DrawManager(), M_FREE, 0, 0, 0, 0, vgs->black_Fill, ker_Screen(), NULL);*/

    vLevel_Draw(vgs->game.level, vgs->game.playerUnit->inSector, &vgs->game.camera, ker_Screen());

    while(bullets != NULL)
    {
        vBullet_Draw(bullets->data, &vgs->game.camera, ker_Screen());

        bullets = bullets->next;
    }

    while(buildings != NULL)
    {
        vBuilding_Draw(buildings->data, &vgs->game.camera, ker_Screen());

        buildings = buildings->next;
    }

    vGame_DrawEffects(&vgs->game);

    vHUD_Draw(&vgs->game.HUD, &vgs->game, ker_Screen());

    if(ve_Options.showFPS == 1)
    {
       text_Draw_Arg(vgs->game.camera.width - 30, 10, ker_Screen(), font_Get(1, 11), &tColourWhite, VL_HUD, 0, "%.2f", vgs->fps, 1000/vgs->fps);
    }
    //graph_Draw(&vgs->game.level->pathGraph, -vgs->game.camera.iPosition.x, -vgs->game.camera.iPosition.y, ker_Screen());

    /*text_Draw_Arg(280, 70, ker_Screen(), fontArial[FS_MEDIUM], &tColourWhite, VL_HUD, 0, "%d", surfaceMan_ReportDrawn(gKer_DrawManager()));
    /*printf("Surfaces %d\n", surfaceMan_ReportDrawn(gKer_DrawManager()));
    text_Draw_Arg(280, 90, ker_Screen(), fontArial[MEDIUM], &tColourWhite, 10, 0, "%d Units Drawn", *gKer_GetDebugVar(0, 0));
    text_Draw_Arg(280, 120, ker_Screen(), fontArial[MEDIUM], &tColourWhite, 10, 0, "%d Tiles Drawn", *gKer_GetDebugVar(1, 0))*/;
    //text_Draw_Arg(280, 150, ker_Screen(), fontArial[MEDIUM], &tColourWhite, 10, 0, "A %d E %d", vgs->game.side[TEAM_PLAYER].totalUnits[UNIT_TANK], vgs->game.side[TEAM_1].totalUnits[UNIT_TANK]);
    /*if(vgs->game.playerUnit != NULL)
    text_Draw_Arg(280, 180, ker_Screen(), font_Get(1, 13), &tColourWhite, 10, 0, "(%d %d)", (int)vgs->game.playerUnit->position.x, (int)vgs->game.playerUnit->position.y);*/
    /*text_Draw_Arg(100, 50, ker_Screen(), fontArial[MEDIUM], &tColourWhite, 1, 0, "%.2f %.2f", vgs->game.playerUnit->position.x, vgs->game.playerUnit->position.y);
    text_Draw_Arg(100, 70, ker_Screen(), fontArial[MEDIUM], &tColourWhite, 1, 0, "%d %d", vgs->game.camera.iPosition.x, vgs->game.camera.iPosition.y);*/

    return;
}

void Game_Exit(void *info)
{
    Base_State *bs = info;
    Vent_GameState *vgs = bs->info;

    SDL_FreeSurface(vgs->black_Fill);

    control_Clean(&vgs->cUp);
    control_Clean(&vgs->cDown);
    control_Clean(&vgs->cLeft);
    control_Clean(&vgs->cRight);
    control_Clean(&vgs->cShootRight);
    control_Clean(&vgs->cShootLeft);
    control_Clean(&vgs->cShootUp);
    control_Clean(&vgs->cShootUpLeft);
    control_Clean(&vgs->cShootUpRight);
    control_Clean(&vgs->cShootDown);
    control_Clean(&vgs->cShootDownLeft);
    control_Clean(&vgs->cShootDownRight);
    control_Clean(&vgs->cExit);
    control_Clean(&vgs->cFPSCAP);
    control_Clean(&vgs->cHealth);
    control_Clean(&vgs->cTest);
    control_Clean(&vgs->cPause);
    control_Clean(&vgs->cChooseUnit);
    control_Clean(&vgs->cBuild);
    control_Clean(&vgs->cCycleWeapon);
    control_Clean(&vgs->cCall);

    vGame_Clean(&vgs->game);

    mem_Free(vgs);

    gKer_FlushGraphics();

    return;
}
