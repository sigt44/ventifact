#include "VentGame.h"

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Sound.h"

#include "../Units/Supply.h"
#include "../Bullets.h"
#include "../Level/LevelFile.h"
#include "../Buildings/Buildings.h"
#include "../Ai/Units/Ai_UnitAttack.h"
#include "../Ai/Units/Ai_UnitFunction.h"
#include "../Ai/Units/Ai_UnitMove.h"
#include "../Effects.h"
#include "../Options.h"
#include "../Sounds.h"

void vGame_Setup(Vent_Game *g, Vent_Player *p)
{
    /*Setup the player side*/
    vSide_Setup(&g->side[TEAM_PLAYER], 2500, TEAM_PLAYER);

            /*vSide_SetupAttributes(&g->side[TEAM_PLAYER],
                              4,
                              4,
                              4,
                              4,
                              4
        );*/

    if(p->playCampaign == 1 && p->campaign != NULL)
    {
        vSide_SetupAttributes(&g->side[TEAM_PLAYER],
                              p->campaign->playerLevel,
                              p->campaign->unitLevel[UNIT_TANK],
                              p->campaign->unitLevel[UNIT_INFANTRY],
                              p->campaign->unitLevel[UNIT_AIR],
                              p->campaign->unitLevel[UNIT_TOWER]
        );
    }

    if(ve_Options.teamAi == 1)
        vSide_SetupAi(&g->side[TEAM_PLAYER], AI_DIFF_NONE, g);

    /*Setup enemy side*/
    vSide_Setup(&g->side[TEAM_1], 20000, TEAM_1);
    vSide_SetupAi(&g->side[TEAM_1], AI_DIFF_NONE, g);

    vSide_SetEnemy(&g->side[TEAM_1], &g->side[TEAM_PLAYER]);
    vSide_SetEnemy(&g->side[TEAM_PLAYER], &g->side[TEAM_1]);

    vStats_Setup(&g->stats);

    g->totalTeams = 2;

    g->player = p;

    g->units = NULL;
    g->buildings = NULL;
    g->bullets = NULL;
    g->effects = NULL;

    g->level = NULL;

    g->totalCitadels = 0;

    g->deltaTime = 0;

    /*Setup the main game timer*/
    g->staticTimer = timer_Setup(gKer_Timer(), 0, 0, 1);
    g->gTimer = timer_Setup(&g->staticTimer, 0, 0, 1);

    g->buildTile = NULL;

    /*Setup the camera*/
    camera2D_Setup(&g->camera, 0, 0,
        ker_Screen_Width(), ker_Screen_Height(), 0, 0, -ker_Screen_Width()/2 + 7, -ker_Screen_Height()/2 + 7);

    vHUD_Setup(&g->HUD, g, &g->side[TEAM_PLAYER], &g->gTimer);

    /*Create the player unit*/
    g->playerUnit = vUnit_Create(0, 0, TEAM_PLAYER, p->startingUnit, 1, 0, &g->side[TEAM_PLAYER].playerAttributes, &g->gTimer);
    g->playerInCamera = 0;

    return;
}

void vGame_SetupLevel(Vent_Game *g, char *levelName)
{
    B_Graph *graph = NULL;

    g->level = (Vent_Level *)mem_Malloc(sizeof(Vent_Level), __LINE__, __FILE__);

    //vLevel_Load(g->level, g, &g->gTimer, "Level_TenTest.map");
    vLevel_Load(g->level, g, &g->gTimer, g->player->levelDirectory, levelName);
    //vLevel_LoadOld(g->level, g, &g->gTimer, levelName);
    //vLevel_Setup(g->level, "TestMap", 800, 800, 1);
    /*void vTile_Setup(Vent_Tile *t, int x, int y, int type, int graphicType, int collision, int layer, int health, int state, int group, Sprite *sprite)*/
    //vLevel_AddTile(g->level, 400, 400, TILE_CITADELBASE, TILE_CITADELBASE, 0, VL_VEHICLE, 0, TILESTATE_BUILDABLE, 0);
    //vLevel_AddTile(g->level, 400, 450, TILE_BUILDINGBASE, TILE_BUILDINGBASE, 0, VL_VEHICLE, 0, TILESTATE_BUILDABLE, 0);
    //vLevel_AddTile(g->level, 400, 500, TILE_BUILDINGBASE, TILE_BUILDINGBASE, 0, VL_VEHICLE, 0, TILESTATE_BUILDABLE, 0);
    /*vLevel_SurfaceInsert(&g->level->surfacesLoaded, "../Textures/Level/Custom/tree1.png", -10);
    vLevel_SurfaceInsert(&g->level->surfacesLoaded, "../Textures/Level/Custom/tree2.png", -11);
    vLevel_SurfaceInsert(&g->level->surfacesLoaded, "../Textures/Level/Custom/largePower.png", -12);
    vLevel_AddTile(g->level, 200, 550, TILE_DECOR, -10, 0, VL_VEHICLE, 0, TILESTATE_IDLE, 0);
    vLevel_AddTile(g->level, 220, 550, TILE_DECOR, -11, 1, VL_VEHICLE, 0, TILESTATE_IDLE, 0);
    vLevel_AddTile(g->level, 600, 550, TILE_DECOR, -12, 1, VL_VEHICLE, 0, TILESTATE_IDLE, 0);
    vLevel_AddTile(g->level, 50, 100, TILE_CITADELBASE, TILE_CITADELBASE, 0, VL_VEHICLE, 50, TILESTATE_BUILDABLE, 2);
    vLevel_AddTile(g->level, 250, 300, TILE_BUILDINGBASE, -10, 0, VL_VEHICLE, 50, TILESTATE_BUILDABLE, 2);
    vLevel_AddTile(g->level, 50, 300, TILE_CITADELBASE, TILE_CITADELBASE, 0, VL_VEHICLE, 50, TILESTATE_BUILDABLE, 3);
    vLevel_AddTile(g->level, 50, 400, TILE_CITADELBASE, TILE_CITADELBASE, 0, VL_VEHICLE, 50, TILESTATE_BUILDABLE, 4);
    vLevel_AddTile(g->level, 50, 500, TILE_CITADELBASE, TILE_CITADELBASE, 0, VL_VEHICLE, 50, TILESTATE_BUILDABLE, 5);

    g->level->baseTileID = TILE_GRASS;
*/

    Dijkstra_SetupNodes(&g->level->pathSearch, &g->level->pathGraph);

    vTile_GroupUp(g->level->tiles);

    vLevel_SetupGround(g->level, vLevel_SurfaceRetrieve(g->level->surfacesLoaded, g->level->header.baseTileID, 0));

    vLevel_SetupSectors(g->level);

    g->totalCitadels = vLevel_NumTileType(g->level, TILE_CITADELBASE);

    camera2D_SetConstraints(&g->camera, 0, 0, g->level->header.width, g->level->header.height);

    /*Add the player unit to the level*/
    if(g->playerUnit != NULL)
    {
        g->playerUnit->spawnPosition = g->playerUnit->iPosition = g->level->header.startLoc[TEAM_PLAYER];
        g->playerUnit->position.x = (float)g->level->header.startLoc[TEAM_PLAYER].x;
        g->playerUnit->position.y = (float)g->level->header.startLoc[TEAM_PLAYER].y;

        vGame_AddUnit(g, g->playerUnit);
    }

    /*Setup the enemy unit level based on the values of the level*/
    vSide_SetupAttributes(&g->side[TEAM_PLAYER],
                        0,
                        g->level->header.enemyUnitLevel[UNIT_TANK],
                        g->level->header.enemyUnitLevel[UNIT_INFANTRY],
                        g->level->header.enemyUnitLevel[UNIT_AIR],
                        g->level->header.enemyUnitLevel[UNIT_TOWER]
    );

    vRules_Setup(&g->rules, g);

    return;
}

Vent_Unit *vGame_AddUnit(Vent_Game *g, Vent_Unit *u)
{
    depWatcher_AddBoth(&u->depWatcher, &g->side[u->team].units, 0);

    list_Push(&g->units, u, 0);

    if(u->isPlayer == 0)
        vUnit_GiveDefaultAi(u, g);

	if(u->team != g->playerUnit->team)
	{
		/*Lower the unit health for easy difficulty*/
		if(ve_Options.difficulty == 0)
		{
			u->health = u->healthStarting = u->healthStarting >> 1;
		}
	}

    /*Create the spawn effect*/
    vEffect_Insert(&g->effects,
       vEffect_Create(&ve_Sprites.spawn, NULL, u->middlePosition.x, u->middlePosition.y,
                      200, &g->gTimer,
                      &vEffect_DrawMiddle, dataStruct_Create(1, g),
                      ker_Screen())
    );

    /*Create the spawn sound if the unit is within range of the camera*/
    if(camera2D_Collision(&g->camera, u->iPosition.x, u->iPosition.y, u->width, u->height) == 1)
    {
        sound_Call(ve_Sounds.unitSpawn, -1, SND_DUPLICATE, vGame_SoundVolume_Distance(g, u->iPosition.x, u->iPosition.y), 0);
    }

    if(u->trackStat == 1) /*If it hasnt been created by level*/
    {
        g->side[u->team].totalUnits[u->type] ++;
    }

    return u;
}

/*
    Function: vGame_SoundVolume_Distance

    Description -
    Returns the volume a sound should be based on the distance between the origin of
    the sound and the player unit.

    3 arguments:
    Vent_Game *g - The current game the sound should be in.
    int xOrigin - x-axis origin of the sound.
    int yOrigin - y-axis origin of the sound.
*/
int vGame_SoundVolume_Distance(Vent_Game *g, int xOrigin, int yOrigin)
{
    const int maxHearingDistance = 500;
    const float volumeRatio = -MIX_MAX_VOLUME/(float)maxHearingDistance;
    int distance = 0;
    int xDis = 0;
    int yDis = 0;

    if(g->playerUnit == NULL || g->playerInCamera == 0)
    {
        xDis = abs(g->camera.iPosition.x + (g->camera.width/2)  - xOrigin);
        yDis = abs(g->camera.iPosition.y + (g->camera.height/2) - yOrigin);
    }
    else
    {
        xDis = abs(g->playerUnit->iPosition.x - xOrigin);
        yDis = abs(g->playerUnit->iPosition.y - yOrigin);
    }

    distance = xDis + yDis;

    if(distance >= maxHearingDistance)
    {
        return 0;
    }

    //printf("Volume: %d, distance %d\n", (int)((volumeRatio * distance) + MIX_MAX_VOLUME), distance);
    /*volume will be in linear proportion to the distance, y = mx + c*/
    return (int)((volumeRatio * distance) + MIX_MAX_VOLUME);
}

void *vGame_AddBuilding(Vent_Game *g, void *building)
{
    Vent_Building *b = building;

    depWatcher_AddBoth(&b->depWatcher, &g->side[b->team].buildings, 0);

    list_Push_Sort(&g->buildings, building, b->position.y);

    if(b->type == BUILDING_CITADEL)
    {
        g->side[b->team].captures ++;
        depWatcher_AddBoth(&b->depWatcher, &g->side[b->team].citadels, 0);
    }

    g->side[b->team].totalBuildings[b->type] ++;

    vBuilding_GiveAi(building, g);

    return building;
}

/*
    Function: vGame_UpdateUnits

    Description -
    This function updates all the units in the game.

    2 arguments:
    Vent_Game *g - The game the units are in.
    float deltaTime - The time passed since the last call of the function.
*/
void vGame_UpdateUnits(Vent_Game *g, float deltaTime)
{
    struct list *units = g->units;

    /*For all the units in the game*/
    while(units != NULL)
    {
        /*Set the sector that the unit is in*/
        vUnit_SetSector(units->data, g->level->sectors);

        /*Update the ai states*/
        vUnit_Ai(units->data);

        /*Move the unit*/
        vUnit_Move(units->data, g, deltaTime);

        /*Check for collisions*/
        vUnit_Collision(units->data, g->level);

        /*Check if its been damaged*/
        vUnit_DamagedCheck(units->data);

        /*Check if the unit is destroyed*/
        vUnit_IsDestroyed(units->data);

        units = units->next;
    }

    if(g->playerUnit != NULL && camera2D_Collision(&g->camera, g->playerUnit->iPosition.x, g->playerUnit->iPosition.y, g->playerUnit->width, g->playerUnit->height) == 1)
    {
        g->playerInCamera = 1;
    }
    else
    {
        g->playerInCamera = 0;
    }

    /*Remove any units that are destroyed*/
    vUnit_DestroyList(&g->units, g);

    return;
}

/*
    Function: vGame_UpdateSupply

    Description -
    Updates all the unit supplies in the game level.

    1 argument:
    Vent_Game *g - The game that the unit supplies are in.
*/
void vGame_UpdateSupply(Vent_Game *g)
{
    struct list *supplyList = g->level->unitSupply;
    struct list *next = NULL;

    Vent_Supply *s = NULL;

    /*For all the unit supplys in the level*/
    while(supplyList != NULL)
    {
        next = supplyList->next;

        /*Update the supply*/
        vSupply_Update(supplyList->data);

        /*If the supply has finished*/
        if(vSupply_CheckFinished(supplyList->data, 1) == 1)
        {
            /*Remove the supply from the level*/
            list_Delete_Node(&g->level->unitSupply, supplyList);
        }

        supplyList = next;
    }

    return;
}

/*
    Function: vGame_UpdateBullets

    Description -
    Updates all the bullets in the game.

    2 arguments:
    Vent_Game *g - The game that contains the bullets to update.
    float deltaTime - The time since the function was last called.
*/
void vGame_UpdateBullets(Vent_Game *g, float deltaTime)
{
    struct list *bulletList = g->bullets;
    struct list *next = NULL;
    int bHit = 0;

    /*For all the bullets in the game*/
    while(bulletList != NULL)
    {
        next = bulletList->next;

        /*Update the bullet*/
        bHit = vBullet_Update(bulletList->data, g, g->buildings, deltaTime);

        if(bHit == BULLET_DESTROYED)
        {
            /*If the bullet is destroyed*/
            vBullet_Clean(bulletList->data);
            mem_Free(bulletList->data);

            list_Delete_Node(&g->bullets, bulletList);
        }

        bulletList = next;
    }

    return;
}

/*
    Function: vGame_UpdateBuildings

    Description -
    Updates all the buildings in the game.

    2 arguments:
    Vent_Game *g - The game that contains the buildings to update.
    float deltaTime - The time since the function was last called.
*/
void vGame_UpdateBuildings(Vent_Game *g, float deltaTime)
{
    struct list *buildingList = g->buildings;
    struct list *next = NULL;

    Vent_Building *b = NULL;

    /*For all the buildings in the game*/
    while(buildingList != NULL)
    {
        next = buildingList->next;
        b = buildingList->data;

        /*Update the building*/
        if(vBuilding_IsDestroyed(buildingList->data, g) == 1)
        {
            /*If the building is destroyed*/
            vBuilding_Clean(buildingList->data, g);
            mem_Free(buildingList->data);

            list_Delete_Node(&g->buildings, buildingList);
        }
        else
        {
            aiModule_Update(&b->ai);

            vBuilding_IsDamaged(b, 1);
        }
        buildingList = next;
    }

    return;
}

/*
    Function: vGame_UpdateTiles

    Description -
    Updates all the tiles in the game.

    2 arguments:
    Vent_Game *g - The game that contains the tiles to update.
    float deltaTime - The time since the function was last called.
*/
void vGame_UpdateTiles(Vent_Game *g, float deltaTime)
{
    struct list *tileList = g->level->destroyableTiles;
    struct list *next = NULL;

    Vent_Tile *tile = NULL;

    /*For all the tiles in the game*/
    while(tileList != NULL)
    {
        next = tileList->next;
        tile = tileList->data;

        /*Update the tile*/
        if(tile->destroyed == 0)
        {
            //printf("Tile: %d %d - %d %d\n", tile->base.position.x, tile->base.position.y, tile->base.team, tile->health);

            /*update the health bar is tile has been damaged*/
            if(tile->health != tile->healthPrev)
            {
                vHealthBar_Display(tile->healthBar, (float)tile->health/tile->base.healthStarting, 200);
                tile->healthPrev = tile->health;
            }

            vTile_IsDestroyed(tile, g);
        }

        tileList = next;
    }

    return;
}

/*
    Function: vGame_UpdateSides

    Description -
    Updates all the sides in the game.

    1 argument:
    Vent_Game *g - The game that contains the sides to update.
*/
void vGame_UpdateSides(Vent_Game *g)
{
    int x = 0;

    /*Update the ai of the sides*/
    for(x = 0; x < g->totalTeams; x++)
    {
        aiModule_Update(&g->side[x].ai);
    }

    return;
}

/*
    Function: vGame_DrawEffects

    Description -
    Draws all the effects in the game.

    1 argument:
    Vent_Game *g - The game that contains the effects to draw.
*/
void vGame_DrawEffects(Vent_Game *g)
{
    struct list *effectList = g->effects;
    struct list *temp = NULL;
    Vent_Effect *effect = NULL;

    while(effectList != NULL)
    {
        temp = effectList->next;

        effect = effectList->data;

        if(vEffect_IsComplete(effect) == 1)
        {
            vEffect_Clean(effect);
            mem_Free(effect);

            list_Delete_Node(&g->effects, effectList);
        }
        else
        {
            effect->draw(effect, &g->camera);
        }

        effectList = temp;
    }

    return;
}

void vGame_Clean(Vent_Game *g)
{
    int x = 0;

    while(g->bullets != NULL)
    {
        vBullet_Clean(g->bullets->data);

        mem_Free(g->bullets->data);
        list_Pop(&g->bullets);
    }

    while(g->units != NULL)
    {
        vUnit_Clean(g->units->data);
        mem_Free(g->units->data);

        list_Pop(&g->units);
    }

    while(g->buildings != NULL)
    {
        vBuilding_Clean(g->buildings->data, g);
        mem_Free(g->buildings->data);

        list_Pop(&g->buildings);
    }

    /*Clean up effects*/
    while(g->effects != NULL)
    {
        vEffect_Clean(g->effects->data);
        mem_Free(g->effects->data);

        list_Pop(&g->effects);
    }

    for(x = 0; x < g->totalTeams; x++)
    {
        vSide_Clean(&g->side[x]);
    }

    if(g->level != NULL)
    {
        vLevel_Clean(g->level);
        mem_Free(g->level);
    }

    vHUD_Clean(&g->HUD);

    return;
}
