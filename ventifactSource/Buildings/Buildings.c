#include "Buildings.h"

#include <SDL/SDL.h>
#include "Flag.h"
#include "Graphics/Sprite.h"
#include "Graphics/Graphics.h"
#include "Graphics/Surface.h"
#include "Graphics/Pixel.h"
#include "Sound.h"

#include "../Images.h"
#include "../Sounds.h"
#include "../Game/VentGame.h"
#include "../Effects.h"
#include "../Ai/Ai_Building.h"

char *ventBuilding_Names[BUILDING_TOTAL] =
{
    "Citadel",
    "Power",
    "Tank",
    "Aircraft",
    "Infantry"
};

int ventBuilding_Prices[BUILDING_TOTAL] =
{
    500,
    250,
    400,
    700,
    200
};

void vBuilding_GiveCitadelProp(Vent_Building *b)
{
    struct list *tileGroup = b->tileList;
    Vent_Tile *tile = NULL;

    b->health = b->healthStarting = 2500;

    sprite_Setup(&b->sBuilding, 0, VL_BUILDING, b->srcTime, 1, frame_CreateBasic(0, surf_Copy(ve_Surfaces.bCitadel[b->team]), A_FREE));

    /*Set all same grouped tiles to link to this citadel*/
    if(b->onTile != NULL)
    {
        b->onTile->citadel = b;

        while(tileGroup != NULL)
        {
            tile = tileGroup->data;

            if(tile->base.group == b->onTile->base.group)
            {
                tile->citadel = b;
            }

            tileGroup = tileGroup->next;
        }
    }

    return;
}

void vBuilding_GivePowerProp(Vent_Building *b)
{
    b->health = b->healthStarting = 700;

    sprite_Setup(&b->sBuilding, 0, VL_BUILDING, b->srcTime, 1, frame_CreateBasic(0, surf_Copy(ve_Surfaces.bPower[b->team]), A_FREE));

    return;
}

void vBuilding_GiveTankProp(Vent_Building *b)
{
    b->health = b->healthStarting = 1250;

    sprite_Setup(&b->sBuilding, 0, VL_BUILDING, b->srcTime, 1, frame_CreateBasic(0, surf_Copy(ve_Surfaces.bTank[b->team]), A_FREE));

    return;
}

void vBuilding_GiveInfantryProp(Vent_Building *b)
{
    b->health = b->healthStarting = 1000;

    sprite_Setup(&b->sBuilding, 0, VL_BUILDING, b->srcTime, 1, frame_CreateBasic(0, surf_Copy(ve_Surfaces.bInfantry[b->team]), A_FREE));

    return;
}

void vBuilding_GiveAirProp(Vent_Building *b)
{
    b->health = b->healthStarting = 1250;

    sprite_Setup(&b->sBuilding, 0, VL_BUILDING, b->srcTime, 1, frame_CreateBasic(0, surf_Copy(ve_Surfaces.bAir[b->team]), A_FREE));

    return;
}

void vBuilding_GiveAi(Vent_Building *b, Vent_Game *game)
{
    int *attribute = NULL; /*Which attribute will the building change?*/
    int attributeChange = 0; /*How much will it change it by*/
    int attributeMax = 0; /*What is the max that the attribute can go to, -1 if unlimited*/
    int updateTime = 0; /*What is the time between the attribute change*/

    switch(b->type)
    {
        case BUILDING_CITADEL:

        attribute = &game->side[b->team].readyUnits[UNIT_TOWER];
        attributeChange = 1;
        attributeMax = 2;
        updateTime = 30000;

        /*Add in healing pulse*/
        aiModule_PushState(&b->ai,
                  aiState_BuildingHealPulse_Setup(aiState_Create(),
                    AI_GLOBAL_STATE,    /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_SINGLE,     /*complete type*/
                    b,                      /*entity*/
                    150,               /*heal radius*/
                    0.2f,        /*how much % to heal each unit*/
                    10,            /*max units to be healed each pulse*/
                    5000,          /*time between each pulse*/
                    game         /*current game*/
                 )
        );

        /*Add in the small credit increase*/
        aiModule_PushState(&b->ai,
                  aiState_BuildingAttribute_Setup(aiState_Create(),
                        AI_GLOBAL_STATE + 1,    /*group*/
                        1,                      /*priority*/
                        AI_COMPLETE_SINGLE,     /*complete type*/
                        b,                      /*entity*/
                        &game->side[b->team].credits,    /*attribute to change*/
                        50,    /*amount to change*/
                        -1,       /*max the attribute can be*/
                        30000,          /*time between each change*/
                        &game->gTimer          /*timer which the update is based on*/
                   )
        );

        break;

        case BUILDING_POWER:

        attribute = &game->side[b->team].credits;
        attributeChange = 100;
        attributeMax = -1;
        updateTime = 30000;

        break;

        case BUILDING_AIR:

        attribute = &game->side[b->team].readyUnits[UNIT_AIR];
        attributeChange = 1;
        attributeMax = 4;
        updateTime = 60000;

        break;

        case BUILDING_TANK:

        attribute = &game->side[b->team].readyUnits[UNIT_TANK];
        attributeChange = 1;
        attributeMax = 4;
        updateTime = 45000;

        break;

        case BUILDING_INFANTRY:

        attribute = &game->side[b->team].readyUnits[UNIT_INFANTRY];
        attributeChange = 1;
        attributeMax = 4;
        updateTime = 20000;

        break;

        default:

        printf("Warning: unknown building type %d\n", b->type);
        attribute = &game->side[b->team].credits;
        attributeChange = 10;
        attributeMax = -1;
        updateTime = 30000;

        break;
    }

    /*Add in the introduction state*/
    aiModule_PushState(&b->ai,
              aiState_BuildingIntro_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 2,    /*group*/
                    0,                      /*priority*/
                    AI_COMPLETE_SINGLE,     /*complete type*/
                    b,                      /*entity*/
                    2500,        /*Length of transparency*/
                    0,           /*Should units effect the length of transparency, 1 if not*/
                    &game->gTimer /*Source timer for the transparency time*/

               )
    );

    /*Add in the main state*/
    aiModule_PushState(&b->ai,
              aiState_BuildingAttribute_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 1,    /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_SINGLE,     /*complete type*/
                    b,                      /*entity*/
                    attribute,    /*attribute to change*/
                    attributeChange,    /*amount to change*/
                    attributeMax,       /*max the attribute can be*/
                    updateTime,          /*time between each change*/
                    &game->gTimer          /*timer which the update is based on*/
               )
    );

    /*Add in repair state*/
    aiModule_PushState(&b->ai,
              aiState_BuildingRepair_Setup(aiState_Create(),
                    AI_GLOBAL_STATE,    /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_SINGLE,     /*complete type*/
                    b,                      /*entity*/
                    30,          /*min health %*/
                    80,          /*max health to heal to %*/
                    150,       /*repair amount each time tick*/
                    2500,         /*delay between each repair*/
                    15000,         /*time between each check for repair*/
                    game         /*current game*/
               )
    );

    return;
}


void vBuilding_GiveProperties(Vent_Building *b, int type)
{
    switch(type)
    {
        case BUILDING_CITADEL:

        vBuilding_GiveCitadelProp(b);

        break;

        case BUILDING_POWER:

        vBuilding_GivePowerProp(b);

        break;

        case BUILDING_AIR:

        vBuilding_GiveAirProp(b);

        break;

        case BUILDING_TANK:

        vBuilding_GiveTankProp(b);

        break;

        case BUILDING_INFANTRY:

        vBuilding_GiveInfantryProp(b);

        break;

        default:

        printf("Error: wrong building type given %d\n", type);

        break;
    }

    return;
}

Vent_Building *vBuilding_Create(int x, int y, Timer *srcTime, Vent_Tile *onTile, int type, int team, struct list *tileList)
{
    Vent_Building *newBuilding = (Vent_Building *)mem_Malloc(sizeof(Vent_Building), __LINE__, __FILE__);

    vBuilding_Setup(newBuilding, x, y, srcTime, onTile, type, team, tileList);

    return newBuilding;
}

void vBuilding_Setup(Vent_Building *b, int x, int y, Timer *srcTime, Vent_Tile *onTile, int type, int team, struct list *tileList)
{
    Uint16P barColour = (Uint16P)SDL_MapRGB(ker_Screen()->format, 0, 255, 0);

    b->position.x = x;
    b->position.y = y;

    b->type = type;
    b->team = team;

    b->onTile = onTile;
    b->tileList = tileList;

    depWatcher_Setup(&b->depWatcher, b);

    aiModule_Setup(&b->ai);

    b->srcTime = srcTime;

    vBuilding_GiveProperties(b, type);

    b->width = sprite_Width(&b->sBuilding);
    b->height = sprite_Height(&b->sBuilding);

    if(b->width > b->height)
    {
        b->radius = b->width/2;
    }
    else
    {
        b->radius = b->height/2;
    }

    sprite_SetAlpha(&b->sBuilding, SDL_ALPHA_OPAQUE);

    if(b->onTile != NULL)
    {
        b->position.x += b->onTile->base.position.x + (int)(sprite_Width(b->onTile->sImage) - sprite_Width(&b->sBuilding))/2;
        b->position.y += b->onTile->base.position.y - (int)(sprite_Height(&b->sBuilding) - sprite_Height(b->onTile->sImage));

        flag_Off(&b->onTile->base.stateFlags, TILESTATE_BUILDABLE);
        b->onTile->building = b;
    }

    b->middlePosition.x = b->position.x + b->width/2;
    b->middlePosition.y = b->position.y + b->height/2;

    b->healthPrevious = b->health;
    b->destroyed = 0;

    /*Setting up the health meter*/
    vHealthBar_Setup(&b->healthBar, srcTime, b->middlePosition.x, b->position.y - 5, 80, 8, 4, barColour);

    return;
}

int vBuilding_CanBuild(Vent_Unit *u, Vent_Game *g)
{
    struct list *i = u->onTiles;
    Vent_Tile *t = NULL;
    Vent_Building *building = NULL;

    while(i != NULL)
    {
        t = i->data;

        /*printf("Tile buy check %d %d : %d\n", t->position.x, t->position.y, flag_Check(&t->stateFlags, TILESTATE_BUILDABLE));*/
        building = t->building;

        /*If the tile the unit is on can be built upon*/
        if(flag_Check(&t->base.stateFlags, TILESTATE_BUILDABLE) == 1)
        {
            g->buildTile = t;
            return 1;
        }
        /*If the tile the unit is on can have its building replaced*/
        else if(building != NULL && building->type != BUILDING_CITADEL && building->team == u->team)
        {
            g->buildTile = t;
            return 1;
        }

        i = i->next;
    }

    g->buildTile = NULL;

    return 0;
}

/*
    Function: vBuilding_CheckBuy

    Description -
    Checks if the supplied building is able to be built on the supplied tile.
    The conditions are if the tile contains a linked citadel, if the tile is the correct type,
    and if the side buying the building has enough credits.

    4 arguments:
    int chosenBuilding - The building type to build on the tile.
    int team - The team of the side thats buying the building.
    Vent_Game *game - The current game.
    Vent_Tile *t - The tile the building is to be built on.
*/
int vBuilding_CheckBuy(int chosenBuilding, int team, Vent_Game *game, Vent_Tile *t)
{
    Vent_Building *citadel = t->citadel;
    int canBuy = 1; /*This should remain 1 if all the requirements are met to buy the building*/

    if(chosenBuilding == BUILDING_CITADEL)
    {
        if(t->base.type == TILE_BUILDINGBASE && chosenBuilding == BUILDING_CITADEL)
        {
            printf("You can't build a citadel here, only standard buildings allowed.\n");
            canBuy --;
        }
    }
    else
    {
        if(t->base.type == TILE_CITADELBASE)
        {
            printf("Only a citadel can be built here.\n");
            canBuy --;
        }
        else if((citadel == NULL || team != citadel->team))
        {
            /*If there is no citadel linked up or there is, but it is on the wrong team.*/
            printf("No friendly citadel is linked.\n");
            canBuy --;
        }
    }

    if(canBuy == 1 && !vBuilding_Buy(t, chosenBuilding, &game->side[team]))
    {
        printf("Not enough cash!\n");
        canBuy --;
    }

    return canBuy;
}

int vBuilding_Buy(Vent_Tile *buildTile, int buildingType, Vent_Side *side)
{
    float healthRatio = 1.0f;
    Vent_Building *building = NULL;

    /*If a building is being replaced with the same type, reduce the price*/
    if(buildTile != NULL && buildTile->building != NULL)
    {
        building = buildTile->building;

        /*Price is reduced to the ratio of the remaining health of the building
        40% health left = 40% of the price to buy*/
        if(building->type == buildingType)
            healthRatio = 1.0f - ((float)building->health/(float)building->healthStarting);


    }

    if(side->credits < (int)(ventBuilding_Prices[buildingType] * healthRatio))
        return 0;

    side->credits -= (int)(ventBuilding_Prices[buildingType] * healthRatio);
    side->creditsSpent += (int)(ventBuilding_Prices[buildingType] * healthRatio);

    return 1;
}

/*
    Function: vBuilding_IsDamaged

    Description -
    Returns 1 if the building has been damaged this frame, 0 if not. Updates
    the building health bar if update == 1.

    2 arguments -
    Vent_Building *building - Building to check if its been damaged.
    int update - if 1 then it will update the previous health to current health and update the health bar.
*/
int vBuilding_IsDamaged(Vent_Building *building, int update)
{
    int bPercentage = 0;

    if(building->healthPrevious != building->health)
    {
        if(update == 1)
        {
            building->healthPrevious = building->health;

            /*Display the health bar for 2 seconds*/
            if(building->health != 0)
                vHealthBar_Display(&building->healthBar, (float)building->health/building->healthStarting, 2000);
        }

        return 1;
    }
    else
    {
        return 0;
    }
}

/*
    Function: vBuilding_IsDestroyed

    Description -
    Returns 1 if a building is destroyed, also created the destroyed effect of the building.

    2 arguments:
    Vent_Building *building - The building to check if its been destroyed.
    Vent_Game *gamme - The current game.
*/
int vBuilding_IsDestroyed(Vent_Building *building, Vent_Game *game)
{
    static int scatterEffects = 10;
    static int scatterAmount = 30;
    static int scatterTimeDelta = 100;

    if(building->destroyed > 1)
    {
        building->destroyed--;

        return building->destroyed;
    }

    if(building->health <= 0)
    {
        /*Create the explosion effects*/
        vEffect_Insert(&game->effects,
           vEffect_Create(&ve_Sprites.unitExplosion, ve_Sounds.bulletExplosion, building->middlePosition.x - (sprite_Width(&ve_Sprites.unitExplosion)/2), building->middlePosition.y - (sprite_Height(&ve_Sprites.unitExplosion)/2),
                          250, &game->gTimer,
                          &vEffect_DrawScatter, dataStruct_Create(5, game, &game->effects, &scatterEffects, &scatterAmount, &scatterTimeDelta),
                          ker_Screen())
        );

        if(camera2D_Collision(&game->camera, building->position.x, building->position.y, building->width, building->height) == 1)
        {
            sound_Call(ve_Sounds.buildingDestroyed, -1, SND_DUPLICATE, vGame_SoundVolume_Distance(game, building->position.x, building->position.y), 0);
        }

        /*Keep alive for 1 more frame, just to notify other objects that are pointing to the building*/
        building->destroyed = 2;
    }

    return building->destroyed;
}

/*
    Function: vBuilding_HasCitadel

    Description -
    Returns 1 if the building has a citadel linked to it.

    1 argument:
    Vent_Building *building - The building to check if it has a citadel linked to it.
*/
int vBuilding_HasCitadel(Vent_Building *building)
{
    Vent_Building *citadel = NULL;

    /*Check if the tile the building is on has a citadel linked to it, and make
    sure it is the same team*/
    if(building->onTile != NULL)
    {
        if(building->onTile->citadel != NULL)
        {
            citadel = building->onTile->citadel;

            if(citadel->team == building->team)
            {
                return 1;
            }
        }
    }

    return 0;
}
/*
    Function: vBuilding_GetFreeTile

    Description -
    Returns a tile that is linked to the given citadel and can be built upon.

    1 argument:
    Vent_Building *citadel - A citadel building whose tiles linked to it will be checked.
*/
Vent_Tile *vBuilding_GetFreeTile(Vent_Building *citadel)
{
    Vent_Tile *tile = NULL;

    /*Get the list of tiles with the same group as the tile the citadel is on*/
    struct list *tileList = citadel->onTile->groupList;

    while(tileList != NULL)
    {
        tile = tileList->data;

        /*If the tile does not have a building on it*/
        if(flag_Check(&tile->base.stateFlags, TILESTATE_BUILDABLE) == 1)
        {
            return tile;
        }

        tileList = tileList->next;
    }

    return NULL;
}

int vBuilding_Build(int team, int buildingType, Vent_Tile *buildTile, Vent_Game *g)
{
    Vent_Building *building = NULL;
    Vent_Tile *t = buildTile;

    int x = 0;
    int y = -2;

    if(t == NULL)
    {
        printf("Warning building without a base\n");

        x = 0;
        y = 0;
    }

    /*If a building is being replaced*/
    if(buildTile->building != NULL)
    {
        building = buildTile->building;
        building->destroyed = 2;
    }

    building = vBuilding_Create(x, y, &g->gTimer, t, buildingType, team, g->level->tiles);

    if(building != NULL)
    {
        vGame_AddBuilding(g, building);
    }

    return 0;
}

void vBuilding_Draw(Vent_Building *b, Camera_2D *c, SDL_Surface *destination)
{
    sprite_DrawAtPos(b->position.x - c->iPosition.x, b->position.y - c->iPosition.y, &b->sBuilding, destination);

    /*Draw the health meter if it has been recently damaged*/
    vHealthBar_Draw(&b->healthBar, c, destination);

    return;
}

void vBuilding_Clean(Vent_Building *b, Vent_Game *g)
{
    Vent_Tile *tile = NULL;

    /*Clear up the ai state*/
    aiModule_Clean(&b->ai);

    sprite_Clean(&b->sBuilding);

    /*Clean up health bar*/
    vHealthBar_Clean(&b->healthBar);

    g->side[b->team].totalBuildings[b->type] --;

    switch(b->type)
    {
        case BUILDING_CITADEL:

        if(b->onTile != NULL)
        {
            b->onTile->citadel = NULL;

            while(b->tileList != NULL)
            {
                tile = b->tileList->data;

                if(tile->base.group == b->onTile->base.group)
                {
                    tile->citadel = NULL;
                }

                b->tileList = b->tileList->next;
            }
        }

        g->side[b->team].captures --;

        break;
    }

    /*If the tile is definitely pointing to this building then set it as buildable again*/
    /*The onTile->building part may have been changed when a player decided to replace the building*/
    if(b->onTile != NULL && b->onTile->building == b)
    {
        b->onTile->building = NULL;
        flag_On(&b->onTile->base.stateFlags, TILESTATE_BUILDABLE);
    }

    depWatcher_Clean(&b->depWatcher);

    return;
}
