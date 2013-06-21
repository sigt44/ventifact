#include "Units.h"

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Graphics/Surface.h"
#include "Graphics/Graphics.h"
#include "Graphics/SpriteManager.h"
#include "Graphics/SurfaceManager.h"
#include "Collision.h"
#include "Sound.h"
#include <math.h>

#include "../Defines.h"
#include "../Images.h"
#include "../Sounds.h"
#include "../Options.h"
#include "../Effects.h"
#include "../Game/VentGame.h"
#include "../Level/Tiles.h"

#include "../Ai/Ai_State.h"
#include "../Ai/AiScan.h"
#include "../Ai/Units/Ai_UnitAttack.h"
#include "../Ai/Units/Ai_UnitMove.h"
#include "../Ai/Units/Ai_UnitFunction.h"

#include "Font.h"

char *vUnitNames[UNIT_ENDTYPE] = {"Tank", "Infantry", "Aircraft", "Tower"};

/*
    Function: vUnit_MiddleX

    Description -
    Returns the location of the x position at the middle of a unit

    1 argument:
    Vent_Unit *u - the unit to get the position from
*/
int vUnit_MiddleX(Vent_Unit *u)
{
    return (int)(u->position.x + (u->width/2));
}

int vUnit_MiddleY(Vent_Unit *u)
{
    return (int)(u->position.y + (u->height/2));
}

/*
    Function: vUnit_SetDirection

    Description -
    This function attempts to set the units direction to the correct heading,
    based upon the units current velocity.

    1 argument:
    Vent_Unit *u - the unit to set the direction of
*/
static void vUnit_SetDirection(Vent_Unit *u)
{
    const float minChange = u->changeSpeed;
    float posX = fabs(u->velocity.x);
    float posY = fabs(u->velocity.y);
    int dUp = 0;
    int dDown = 1;
    int dLeft = 2;
    int dRight = 3;

    /*set the current units heading to its previous*/
    u->pGraphicTurn = u->graphicTurn;

    if(posX <= posY)
    {
        if(u->velocity.y > minChange)
        {
            u->graphicTurn = dDown;
        }
        else if(u->velocity.y < -minChange)
        {
            u->graphicTurn = dUp;
        }
    }
    else if((posX - posY) > minChange)
    {
        if(u->velocity.x > minChange)
        {
            u->graphicTurn = dRight;
        }
        else if(u->velocity.x < -minChange)
        {
            u->graphicTurn = dLeft;
        }
    }

    /*If the new direction is different to the old one*/
    if(u->pGraphicTurn != u->graphicTurn)
    {
        /*update the graphic sprite*/
        sprite_SetFrame(u->sUnit, u->graphicTurn);

        /*set the new width and height*/
        u->width = sprite_Width(u->sUnit);
        u->height = sprite_Height(u->sUnit);

        /*if the unit does not hit a tile because of this direction change*/
        if(vUnit_TileScan(u) == 0)
            vWeapon_SetOffset(&u->weapon, u->width/2, u->height/2);
        else
        {
            /*if the unit does collide with a tile after the direction change we need to revert it*/
            u->graphicTurn = u->pGraphicTurn;
            sprite_SetFrame(u->sUnit, u->pGraphicTurn);

            //sprite_DrawAtPos(0, 0, u->sUnit, ker_Screen());

            u->width = sprite_Width(u->sUnit);
            u->height = sprite_Height(u->sUnit);
        }
    }

    return;
}

/*
    Function: vUnit_DamagedCheck

    Description -
    Returns 1 if the unit has been damaged.

    2 arguments:
    Vent_Unit *u - The unit to check if its been damaged.
    int update - Whether to update the previous health of the unit. 1 if it should.
*/
int vUnit_IsDamaged(Vent_Unit *u, int update)
{
    int damaged = 0;

    if(u->healthPrev > u->health)
        damaged = 1;

    if(update == 1)
        u->healthPrev = u->health;

    return damaged;
}

/*
    Function: vUnit_DamagedCheck

    Description -
    If the unit has been damaged it temporarily changes the sprite to the damaged version.
    Otherwise it sets the units sprite back to its base.

    1 argument:
    Vent_Unit *u - The unit that is checked for damage.
*/
void vUnit_DamagedCheck(Vent_Unit *u)
{
    /*if(u->damageAnimationLength != 0)
        timer_Calc(&u->damageAnimationTimer);*/

    /*If the previous check of the units health is larger than the units current health*/
    if(vUnit_IsDamaged(u, 1))
    {
        /*u->damageAnimationLength = u->damageAnimationTimer.end_Time;

        sprite_SetFrame(&u->sUnitDamaged, u->graphicTurn);
        //u->sUnit = &u->sUnitDamaged;

        timer_Start(&u->damageAnimationTimer);*/
    }
    /*else if(timer_Get_Remain(&u->damageAnimationTimer) <= 0)
    {
        if(u->sUnit == &u->sUnitDamaged)
        {
            sprite_SetFrame(&u->sUnitBase, u->graphicTurn);
            //u->sUnit = &u->sUnitBase;
            u->damageAnimationLength = 0;
        }
    }*/

    return;
}

/*
    Function: vUnit_HasMoved

    Description -
    Returns 1 if the unit has moved since its last update

    1 argument:
    Vent_Unit *u - The unit that is checked for whether it has moved.
*/
int vUnit_HasMoved(Vent_Unit *u)
{
    if(u->prevPosition.x != u->position.x)
        return 1;
    if(u->prevPosition.y != u->position.y)
        return 1;

    return 0;
}

/*
    Function: vUnit_SteeringForce

    Description -
    Returns the sum force to give a unit.

    1 argument:
    Vent_Unit *u - The unit that gets its forces summed up.
*/
static Vector2D vUnit_SteeringForce(Vent_Unit *u)
{
    Vector2D netForce = {0.0f, 0.0f};
    Vector2D frictionForce = {0.0f, 0.0f};

    /*Scale the forces*/
    u->seperationForce = vector_MultiplyBy(&u->seperationForce, u->steeringForce * u->seperationWeight);
    u->targetForce = vector_MultiplyBy(&u->targetForce, u->steeringForce  * u->targetWeight);

    /*Sum all the forces, priority based on order*/
    vAi_SteerAccumulate(&netForce, &u->targetForce, u->steeringForce);
    vAi_SteerAccumulate(&netForce, &u->seperationForce, u->steeringForce);

    /*Reset the forces*/
    vector_Clear(&u->targetForce);
    vector_Clear(&u->collisionForce);
    vector_Clear(&u->seperationForce);

    /*Setup the friction force*/
    frictionForce = vector_MultiplyBy(&u->velocity, -u->friction);

    if(u->prevForce.x != netForce.x)
    {
        u->prevForce.x = netForce.x;

        u->forceMaxSpeedX = fabs(netForce.x/u->friction);
        u->steerFixX.x = u->forceMaxSpeedX/u->maxSpeed;
        u->steerFixX.y = 1/u->steerFixX.x;
    }

    if(u->forceMaxSpeedX != u->maxSpeed && netForce.x != 0.0f )
    {
        if(u->forceMaxSpeedX < u->maxSpeed)
        {
            frictionForce.x *= u->steerFixX.x;
        }
        else
            frictionForce.x *= u->steerFixX.y;
    }

    if(u->prevForce.y != netForce.y)
    {
        u->prevForce.y = netForce.y;

        u->forceMaxSpeedY = fabs(netForce.y/u->friction);
        u->steerFixY.x = u->forceMaxSpeedY/u->maxSpeed;
        u->steerFixY.y = 1/u->steerFixY.x;
    }

    if(u->forceMaxSpeedY != u->maxSpeed && netForce.y != 0.0f )
    {
        if(u->forceMaxSpeedY < u->maxSpeed)
        {
            frictionForce.y *= u->steerFixY.x;
        }
        else
            frictionForce.y *= u->steerFixY.y;
    }

    netForce = vector_Add(&netForce, &frictionForce);

    return netForce;
}

/*
    Function: vUnit_Move

    Description -
    Moves a unit in the game world.

    3 arguments:
    Vent_Unit *u - the unit to move
    void *vg - this should point to the game world structure
    float dt - this is the time progressed in the game world since the
    last function call.
*/
int vUnit_Move(Vent_Unit *u, void *vg, float dt)
{
    Vector2D netForce = {0.0f, 0.0f};

    /*Set the previous unit position as the current one*/
    u->prevPosition = u->position;

    u->position.x += u->velocity.x  * dt;
    u->position.y += u->velocity.y  * dt;

    u->iPosition.x = (int)u->position.x;
    u->iPosition.y = (int)u->position.y;

    if(u->isPlayer == 1)
    {
        //printf("Position %d %d -- %.2f %.2f\n", u->iPosition.x, u->iPosition.y, u->position.x, u->position.y);
    }
    u->middlePosition.x = vUnit_MiddleX(u);
    u->middlePosition.y = vUnit_MiddleY(u);

    netForce = vUnit_SteeringForce(u);

    u->velocity.x += netForce.x * dt;
    u->velocity.y += netForce.y * dt;

    /*Keep at max speed*/
    if(u->velocity.x > u->maxSpeed)
        u->velocity.x = u->maxSpeed;
    else if(u->velocity.x < -u->maxSpeed)
        u->velocity.x = -u->maxSpeed;

    if(u->velocity.y > u->maxSpeed)
        u->velocity.y = u->maxSpeed;
    else if(u->velocity.y < -u->maxSpeed)
        u->velocity.y = -u->maxSpeed;

    /*if(u->isPlayer)
        printf("UnitVel = %.2f %.2f\n", u->velocity.x, u->velocity.y);*/

    vUnit_SetDirection(u);

    return 0;
}

/*
    Function: vUnit_Ai

    Description -
    Updates all the ai states of the unit.

    1 argument:
    Vent_Unit *u - The unit with the ai states to update.
*/
void vUnit_Ai(Vent_Unit *u)
{
    aiModule_Update(&u->ai);
}

/*
    Function: vUnit_TileCollision

    Description -
    Given a valid unit and a tile, this function will check if they
    collide in the game world. If so and the tile is a 'physical object'
    it will attempt to move the unit adjacent to the point of contact.
    Any collision with a tile that is not 'physical' will update a variable (onTile)
    in the unit structure to point to the collided tile.

    2 arguments:
    Vent_Unit *u - the unit to check the collision
    void *t - the tile to check the collision (type Vent_Tile)
*/
void vUnit_TileCollision(Vent_Unit *u, void *t)
{
    Vector2DInt prevPos;
    Vent_Tile *tile = t;

    int prevInY = 0;
    int prevInX = 0;

    /*If the tile has no collision or is destroyed*/
    if(tile->destroyed == 1 || (tile->base.collision == 0 && tile->base.type == TILE_DECOR))
        return;

    if(vTile_CheckCollision(tile, u) == 1)
    {
        /*There is a collision, but where?*/
        if(tile->base.collision == 1 && u->layer != VL_AIR)
        {
            prevPos.x = (int)u->prevPosition.x;
            prevPos.y = (int)u->prevPosition.y;
            //holdPos = u->iPosition;


            if(prevPos.y > tile->base.position.y && (prevPos.y - tile->base.position.y) < (int)tile->base.height)
                prevInY = 1;
            else if(prevPos.y <= tile->base.position.y && (prevPos.y + u->height) > tile->base.position.y)
                prevInY = 1;
            else
                prevInY = 0;

            if(prevPos.x > tile->base.position.x && (prevPos.x - tile->base.position.x) < (int)tile->base.width)
                prevInX = 1;
            else if(prevPos.x <= tile->base.position.x && (prevPos.x + u->width) > tile->base.position.x)
                prevInX = 1;
            else
                prevInX = 0;

            if((prevInY == 0 && prevInX == 1) || prevInY == prevInX)
            {
                /*Y axis collision*/
                if(u->iPosition.y < tile->base.position.y)
                    u->iPosition.y = tile->base.position.y - u->height;
                else
                    u->iPosition.y = tile->base.position.y + tile->base.height;

                u->position.y = u->iPosition.y;
            }

            /*X axis collision*/
            if((prevInY == 1 && prevInX == 0) || prevInY == prevInX)
            {
                if(u->iPosition.x < tile->base.position.x)
                    u->iPosition.x = tile->base.position.x - u->width;
                else
                {
                    u->iPosition.x = tile->base.position.x + tile->base.width;
                }

                u->position.x = u->iPosition.x;
            }


        }
        else
        {
            //u->onTile = tile; /*onTile now holds the tile that the unit is positioned on in the game world*/
            list_Push(&u->onTiles, tile, 0);
            tile->unitsOn ++;
            //printf("Added tile %d %d\n", tile->position.x, tile->position.y);
        }

    }

    return;
}

/*
    Function: vUnit_TileCollisionList

    Description -
    Given a valid list of tiles and a unit, this function will
    call the function vUnit_TileCollision for each tile in the list.

    2 arguments:
    Vent_Unit *u - the unit to check for tile collisions
    struct list *tiles - a list holding valid tiles
*/
static void vUnit_TileCollisionList(Vent_Unit *u, struct list *tiles)
{
    while(tiles != NULL)
    {
        vUnit_TileCollision(u, tiles->data);

        tiles = tiles->next;
    }

    return;
}

/*
    Function: vUnit_Collision

    Description -
    This function is used to perform all the collision based actions on a unit.

    2 argument2:
    Vent_Unit *u - the unit to perform collision checks and responces on
    Vent_Level *l - the current level that the unit is in
*/
void vUnit_Collision(Vent_Unit *u, Vent_Level *l)
{
    Vent_Sector *s = u->inSector;

    int x;

    list_Clear(&u->onTiles);

    /*Tile collisions - */
    if(s != NULL)
    {
        /*Perform tile collision for the units current sector*/
        vUnit_TileCollisionList(u, s->collisionTiles);

        /*Also perform tile collisions for the units adjacent sectors, so long as the unit overlaps the sector*/
        for(x = 0; x < SECTOR_ADJACENT; x++)
        {
            if(s->adjacent[x] != NULL &&
               collisionStatic_Rectangle2(
                u->iPosition.x,
                u->iPosition.y,
                u->width, u->height,
                s->adjacent[x]->position.x,
                s->adjacent[x]->position.y,
                s->adjacent[x]->width,
                s->adjacent[x]->height) == 1)
            {
                vUnit_TileCollisionList(u, s->adjacent[x]->collisionTiles);
            }/*
            if(s->adjacent[x] != NULL)
            {
                vUnit_TileCollisionList(u, s->adjacent[x]->tiles);
            }*/
        }
    }

    /*Make sure unit is in simple boundaries of level*/
    if( u->position.x < 0 )
    {
        u->position.x = 0.0f;
        u->iPosition.x = 0;
    }
    else if((u->position.x + u->width) > l->header.width)
    {
        u->position.x = u->iPosition.x = l->header.width - u->width;
    }

    if( u->position.y < 0 )
    {
        u->position.y = 0.0f;
        u->iPosition.y = 0;
    }
    else if((u->position.y + u->height) > l->header.height)
    {
        u->position.y = u->iPosition.y = l->header.height - u->height;
    }

    return;
}

/*
    Function: vUnit_TileScan

    Description -
    Checks if the unit is colliding with any tiles in its current map sector

    1 argument:
    Vent_Unit *u - the unit to check for collisions
*/
int vUnit_TileScan(Vent_Unit *u)
{
    Vent_Sector *s = u->inSector;
    int x = 0;

    struct list *i = NULL;
    Vent_Tile *t = NULL;

    /*If the unit is actually in a sector*/
    if(s == NULL)
        return 0;

    i = s->collisionTiles;

    /*Go through all the tiles in the sector*/
    while(i != NULL)
    {
        t = i->data;
        /*Check if there is a collision between the tile and the unit*/
        if(t->destroyed == 0 && t->base.collision == 1 && vTile_CheckCollision(i->data, u) == 1)
            return 1;

        i = i->next;
    }

    /*Also perform tile collisions for the units adjacent sectors, so long as the unit overlaps the sector*/
    for(x = 0; x < SECTOR_ADJACENT; x++)
    {
        if(s->adjacent[x] != NULL &&
         collisionStatic_Rectangle2(
            (int)u->position.x,
            (int)u->position.y,
            u->width, u->height,
            s->adjacent[x]->position.x,
            s->adjacent[x]->position.y,
            s->adjacent[x]->width,
            s->adjacent[x]->height) == 1)
        {
            i = s->adjacent[x]->collisionTiles;

            /*Go through all the tiles in the sector*/
            while(i != NULL)
            {
                t = i->data;

                /*Check if there is a collision between the tile and the unit*/
                if(t->destroyed == 0 && t->base.collision == 1 && vTile_CheckCollision(i->data, u) == 1)
                    return 1;

                i = i->next;
            }
        }
    }

    /*There have been no collisions*/
    return 0;
}

/*
    Function: vUnit_SetSector

    Description -
    This function makes the unit be reported in the correct sector for the game world.

    2 arguments:
    Vent_Unit *u - the unit that is to be set in the correct sector
    struct list *sectors - a list of sectors that are in the game world
*/
void vUnit_SetSector(Vent_Unit *u, struct list *sectors)
{
    Vent_Sector *s = u->inSector;
    int foundSector = 0;

	/*if the unit is still in the same sector as last check, return early*/
    if(s != NULL &&
       (!vUnit_HasMoved(u) ||
        collisionStatic_Rectangle2(
            u->iPosition.x, u->iPosition.y,
            u->width, u->height,
            s->position.x, s->position.y,
            s->width, s->height) == 1) )
    {
        return;
    }

	/*search through the list of sectors*/
    while(sectors != NULL)
    {
        if(s != NULL)
        {
            s = vSector_Find(s, u->iPosition.x, u->iPosition.y);
        }
        else
            s = vSector_Find(sectors->data, u->iPosition.x, u->iPosition.y);

        if(collisionStatic_Rectangle2(
            u->iPosition.x, u->iPosition.y, u->width, u->height,
            s->position.x, s->position.y, s->width, s->height) == 1)
        {
            if(u->inSector != NULL)
            {
				/*remove the reference to the unit that the sector contains*/
                depWatcher_RemoveBoth(&u->depWatcher, &u->inSector->units);
            }

			/*add in the reference to unit for the new sector*/
            depWatcher_AddBoth(&u->depWatcher, &s->units, 0);

            u->inSector = s;

            foundSector = 1;
            /*exit early, new sector has been found*/
            return;
        }

        sectors = sectors->next;
    }

    /*The unit is not inside any sector on the level*/
    if(foundSector == 0)
    {
        printf("Warning could not find sector for unit at %d %d\n", (int)u->position.x, (int)u->position.y);
        if(u->inSector != NULL)
        {
            /*remove the reference to the unit that the sector contains*/
            depWatcher_RemoveBoth(&u->depWatcher, &u->inSector->units);

            u->inSector = NULL;
        }
    }

    return;
}

/*
    Function: vUnit_GiveDefaultAi

    Description -
    Gives a unit some default ai states. Attack others, wander if idle, seperate from others.

    2 arguments:
    Vent_Unit *u - The unit to give the ai states to.
    void *ventGame - Points to the game, type Vent_Game.
*/
void vUnit_GiveDefaultAi(Vent_Unit *u, void *ventGame)
{
    Vent_Game *game = ventGame;
    int seperateDistance = 0;
    int wanderDistance = 0;
    int fireRange = vWeapon_GetRange(&u->weapon);

    /*Setup the unit dependant values*/
    switch(u->type)
    {
        case UNIT_TANK:
        default:

        seperateDistance = 25;
        wanderDistance = 50;

        break;

        case UNIT_INFANTRY:

        seperateDistance = 15;
        wanderDistance = 25;

        break;

        case UNIT_AIR:

        seperateDistance = 35;
        wanderDistance = 150;

        /*Phase ai*/
        vUnit_GiveAi(u,
                     aiState_UnitPhase_Setup(aiState_Create(),
                        AI_GLOBAL_STATE,     /*group*/
                        4,                   /*priority*/
                        AI_COMPLETE_SINGLE,       /*complete type*/
                        u,                  /*entity*/
                        5000,             /*Invulnerability time*/
                        10000,            /*Cooldown time*/
                        &game->gTimer          /*Source timer*/
                    )
        );

        break;
    }

    /*Unit seperation ai*/
    vUnit_GiveAi(u,
        aiState_UnitRepel_Setup(aiState_Create(),
            AI_GLOBAL_STATE,    /*group*/
            2,                  /*priority*/
            AI_COMPLETE_SINGLE, /*complete type*/
            u,                  /*entity*/
            seperateDistance * seperateDistance,            /*repel distance squared*/
            &game->camera,   /*game camera*/
            &game->gTimer   /*source timer*/
            )
    );

    if(vWeapon_GetRange(&u->weapon) > u->viewDistance)
    {
        fireRange = u->viewDistance;
    }

    /*Unit obtain targets ai*/
    vUnit_GiveAi(u,
        aiState_UnitObtainTargets_Setup(aiState_Create(),
        AI_GLOBAL_STATE,    /*group*/
        4,                  /*priority*/
        AI_COMPLETE_SINGLE, /*complete type*/
        u,                  /*entity*/
        u->viewDistance,       /*distance he unit can see targets*/
        1000,                  /*Time between each check*/
        game                   /*game*/
        )
    );

    /*Unit attack buildings/units ai*/
    vUnit_GiveAi(u,
        aiState_UnitAttack_Setup(aiState_Create(),
        AI_GLOBAL_STATE,    /*group*/
        5,                  /*priority*/
        AI_COMPLETE_SINGLE, /*complete type*/
        u,                  /*entity*/
        game,         /*game*/
        fireRange,                /*distance to start checking*/
        0                   /*priority of building(1) vs unit(0)*/
        )
    );

    /*Unit select weapon ai*/
    vUnit_GiveAi(u,
        aiState_UnitSelectWeapon_Setup(aiState_Create(),
        AI_GLOBAL_STATE,    /*group*/
        5,                  /*priority*/
        AI_COMPLETE_SINGLE, /*complete type*/
        u,                  /*entity*/
        1000,               /*secondary fire length*/
        2000,              /*update time*/
        &game->gTimer       /*source timer*/
        )
    );

    /*Unit wander ai*/
    vUnit_GiveAi(u,
        aiState_UnitWander_Setup(aiState_Create(),
        AI_GLOBAL_STATE - 1,    /*group*/
        10,                      /*priority*/
        AI_COMPLETE_SINGLE,     /*complete type*/
        u,                      /*entity*/
        (float)wanderDistance,                  /*wander distance*/
        (float)u->position.x,   /*wander origin x*/
        (float)u->position.y,   /*wander origin y*/
        5000,                   /*pause time at each point*/
        &game->gTimer,      /*game timer*/
        0,                      /*use level nodes(1), dont use level nodes(0)*/
        game->level         /*level*/
        )
    );

    return;
}

/*
    Function: vUnit_GiveTankProp

    Description -
    This function sets the properties of a unit to be that of a tank

    1 argument -
    Vent_Unit *u - the unit to give properties to
*/
void vUnit_GiveTankProp(Vent_Unit *u)
{
    int accurate = 0;

    u->viewDistance = 175;

    u->friction = 1.5f;

    u->layer = VL_TREE; /* layer in the game world, used for drawing and some logic*/

    /*setup the graphics of the unit*/
    sprite_Setup(&u->sUnitBase, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.tank[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tank[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tank[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tank[u->team][D_RIGHT], M_FREE)
        );

    /*setup the graphics of the unit when it has been damaged*/
    sprite_Setup(&u->sUnitDamaged, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.tankDamaged[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tankDamaged[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tankDamaged[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tankDamaged[u->team][D_RIGHT], M_FREE)
        );

    if(u->isPlayer)
    {
        if(ve_Options.cheat == 1)
        {
            u->health = u->healthStarting = 30000;
        }
        else
        {
             u->health = u->healthStarting = 300;
        }
        u->maxSpeed = 75;
        u->changeSpeed = 20;
        u->steeringForce = 200.0f;

        /*setup the weapon*/
        vWeapon_Setup(&u->weapon, u->ID, 20, sprite_CurrentSurface(&u->sUnitBase)->w/2, sprite_CurrentSurface(&u->sUnitBase)->h/2, WEAPON_PLASMA, u->team, accurate, u->isPlayer, u->srcTimer);

        u->weapon.ammo[WEAPON_PLASMA] = AMMO_UNLIMITED;
        u->weapon.ammo[WEAPON_MISSILE] = 2;
        u->weapon.ammo[WEAPON_MINE] = 5;
        if(ve_Options.cheat == 1)
        {
            u->weapon.ammo[WEAPON_ROCKET] = AMMO_UNLIMITED;
        }
    }
    else
    {
        u->health = 200;
        u->healthStarting = 200;
        u->maxSpeed = 60;
        u->changeSpeed = 25;
        u->steeringForce = 100.0f;

        /*setup the weapon*/
        vWeapon_Setup(&u->weapon, u->ID, 20, sprite_CurrentSurface(&u->sUnitBase)->w/2, sprite_CurrentSurface(&u->sUnitBase)->h/2, WEAPON_PLASMA_WEAK, u->team, accurate, u->isPlayer, u->srcTimer);

        u->weapon.ammo[WEAPON_PLASMA_WEAK] = AMMO_UNLIMITED;
        u->weapon.ammo[WEAPON_MISSILE] = 2;
    }

    return;
}

void vUnit_GiveInfantryProp(Vent_Unit *u)
{
    int accurate = 1;

    u->health = 60;
    u->healthStarting = 60;
    u->maxSpeed = 50;

    u->viewDistance = 150;
    u->layer = VL_VEHICLE;

    u->friction = 4.0f;
    u->steeringForce = 80.0f;
    u->changeSpeed = 5;

    sprite_Setup(&u->sUnitBase, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.infantry[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.infantry[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.infantry[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.infantry[u->team][D_RIGHT], M_FREE)
        );

    sprite_Setup(&u->sUnitDamaged, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.infantryDamaged[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.infantryDamaged[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.infantryDamaged[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.infantryDamaged[u->team][D_RIGHT], M_FREE)
        );

    vWeapon_Setup(&u->weapon, u->ID, 20, sprite_CurrentSurface(&u->sUnitBase)->w/2, sprite_CurrentSurface(&u->sUnitBase)->h/2, WEAPON_PLASMA_WEAK, u->team, accurate, u->isPlayer, u->srcTimer);
    u->weapon.ammo[WEAPON_PLASMA_WEAK] = AMMO_UNLIMITED;
    u->weapon.ammo[WEAPON_MINE] = 1;

    if(u->isPlayer)
    {
        u->weapon.ammo[WEAPON_REPAIR] = AMMO_UNLIMITED;
    }
    else
    {
        u->weapon.ammo[WEAPON_REPAIR] = 50;
    }

    return;
}

void vUnit_GiveTowerProp(Vent_Unit *u)
{
    int accurate = 1;

    u->health = 700;
    u->healthStarting = 700;
    u->maxSpeed = 0;
    u->viewDistance = 200;
    u->layer = VL_VEHICLE;

    u->friction = 0.0f;
    u->steeringForce = 0.0f;
    u->changeSpeed = 0;

    sprite_Setup(&u->sUnitBase, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.tower[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tower[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tower[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.tower[u->team][D_RIGHT], M_FREE)
        );

    sprite_Setup(&u->sUnitDamaged, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.towerDamaged[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.towerDamaged[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.towerDamaged[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.towerDamaged[u->team][D_RIGHT], M_FREE)
        );

    u->maxForce = 0;

    vWeapon_Setup(&u->weapon, u->ID, 20, sprite_CurrentSurface(&u->sUnitBase)->w/2, sprite_CurrentSurface(&u->sUnitBase)->h/2, WEAPON_PLASMA_WEAK, u->team, accurate, u->isPlayer, u->srcTimer);
    u->weapon.ammo[WEAPON_PLASMA_WEAK] = AMMO_UNLIMITED;
    u->weapon.ammo[WEAPON_MINE] = 5;
    u->weapon.ammo[WEAPON_MISSILE] = 20;
    //u->weapon.ammo[WEAPON_REPAIR] = 50;

    return;
}

void vUnit_GiveAirProp(Vent_Unit *u)
{
    int accurate = 0;

    u->health = 150;
    u->healthStarting = 150;
    u->maxSpeed = 150;
    u->viewDistance = 300;
    u->layer = VL_AIR;

    u->friction = 0.75f;
    u->steeringForce = 300.0f;
    u->changeSpeed = 50;

    sprite_Setup(&u->sUnitBase, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, surf_Copy(ve_Surfaces.air[u->team][D_UP]), A_FREE),
        frame_CreateBasic(0, surf_Copy(ve_Surfaces.air[u->team][D_DOWN]), A_FREE),
        frame_CreateBasic(0, surf_Copy(ve_Surfaces.air[u->team][D_LEFT]), A_FREE),
        frame_CreateBasic(0, surf_Copy(ve_Surfaces.air[u->team][D_RIGHT]), A_FREE)
        );

    sprite_Setup(&u->sUnitDamaged, 0, u->layer, u->srcTimer, 4,
        frame_CreateBasic(0, ve_Surfaces.airDamaged[u->team][D_UP], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.airDamaged[u->team][D_DOWN], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.airDamaged[u->team][D_LEFT], M_FREE),
        frame_CreateBasic(0, ve_Surfaces.airDamaged[u->team][D_RIGHT], M_FREE)
        );

    vWeapon_Setup(&u->weapon, u->ID, 20, sprite_CurrentSurface(&u->sUnitBase)->w/2, sprite_CurrentSurface(&u->sUnitBase)->h/2, WEAPON_PLASMA, u->team, accurate, u->isPlayer, u->srcTimer);
    u->weapon.ammo[WEAPON_PLASMA] = AMMO_UNLIMITED;

    /*Tweak the plasma bullet*/
    vBulletAttributes_AddProp(&u->weapon.attributes[WEAPON_PLASMA],
                               "Plasma (aircraft)",
                               0,
                               0,
                               0,
                               70, /*Extra max distance*/
                               0,
                               0,
                               0,
                               100 /*Slightly slower refire*/
                               );

    vWeapon_AssignBullet(&u->weapon, WEAPON_PLASMA);

    return;
}

/*
    Function: vUnit_Create

    Description -
    A function that creates a new unit in the game world.

    8 arguments:
    unsigned int x - x axis location of the unit to be placed in the game world
    unsigned int y - y axis location
    unsigned int team - the team of the unit
    unsigned int type - the type of unit (tank, infantry...)
    unsigned int isPlayer - 1 if the unit is a player, 0 if not
    unsigned int trackStat - 1 if the unit is to be included in its sides stats, 0 if not
    Vent_Attributes *extraAttributes - the upgraded attributes of the unit
    Timer *srcTime - timer the unit will use for logic, could be the game world timer
*/
Vent_Unit *vUnit_Create(unsigned int x, unsigned int y,
                        unsigned int team, unsigned int type,
                        unsigned int isPlayer, unsigned int trackStat,
                        Vent_Attributes *extraAttributes, Timer *srcTime)
{
    Vent_Unit *unitNew = (Vent_Unit *)mem_Malloc(sizeof(Vent_Unit), __LINE__, __FILE__);

    vUnit_Setup(unitNew, x, y, team, type, isPlayer, trackStat, extraAttributes, srcTime);

    return unitNew;
}

void vUnit_Setup(Vent_Unit *u,
                unsigned int x, unsigned int y,
                unsigned int team, unsigned int type,
                unsigned int isPlayer, unsigned int trackStat,
                Vent_Attributes *extraAttributes, Timer *srcTime)
{
    int i = 0;

    u->ID = (int)u;

    u->spawnPosition.y = y;
    u->spawnPosition.x = x;

    u->position.x = (float)x;
    u->position.y = (float)y;
    u->iPosition.x = x;
    u->iPosition.y = y;

    u->prevPosition = u->position;

    u->team = team;
    u->type = type;

    u->onTiles = NULL;

    u->pGraphicTurn = u->graphicTurn = 0;

    u->srcTimer = srcTime;

    u->inSector = NULL;

    u->attributes = extraAttributes;

    u->forceMaxSpeedX = 0.0f;
    u->forceMaxSpeedY = 0.0f;
    vector_Clear(&u->prevForce);
    vector_Clear(&u->targetForce);
    vector_Clear(&u->collisionForce);
    vector_Clear(&u->seperationForce);
    u->seperationWeight = 1.0f;
    u->targetWeight = 1.0f;

    u->isPlayer = isPlayer;

    u->invulnerability = 0;
    u->collide = 0;

    u->trackStat = trackStat;

    depWatcher_Setup(&u->depWatcher, u);

    vTarget_SetupDefaultsArray(&u->targetEnemy[0], UNIT_MAX_TARGETS);
    vTarget_SetupDefaultsArray(&u->targetAlly[0], UNIT_MAX_TARGETS);

    aiModule_Setup(&u->ai);

    u->direction = D_UP;

    vUnit_GiveProperties(u, type);

    if(u->attributes != NULL)
        vAttributes_Set(u, u->attributes);

    u->healthPrev = u->health;

    u->velocity.x = 0;
    u->velocity.y = 0;

    if(isPlayer == 1)
    {
        snprintf(u->name, 32, "(Team %d)Player", team);
    }
    else
    {
        snprintf(u->name, 32, "(Team %d)Unit %d", team, (int)u);
    }

    /*u->damageAnimationLength = 0;
    u->damageAnimationTimer = timer_Setup(srcTime, 0, 200, 0);*/

    u->destroyed = UNIT_ALIVE;

    u->sUnit = &u->sUnitBase;
    u->width = sprite_CurrentSurface(&u->sUnitBase)->w;
    u->height = sprite_CurrentSurface(&u->sUnitBase)->h;

    u->middlePosition.x = vUnit_MiddleX(u);
    u->middlePosition.y = vUnit_MiddleY(u);

    u->heading = u->velocity;

	/*if(options.sound && check_collision(&camera,&u->box) == 0)
		sound_Call(s_UnitSpawn,-1,0,1);*/

    return;
}

/*
    Function: vUnit_Draw

    Description -
    Function that draws a unit on screen.

    3 arguments:
    Vent_Unit *u - the unit to draw
    Camera_2D *c - this is used to draw the unit relative to the game camera
    SDL_Surface *destination - the SDL surface to draw the unit on
*/
void vUnit_Draw(Vent_Unit *u, Camera_2D *c, SDL_Surface *destination)
{
    int *unitDrawTrack;
    if(c != NULL)
    {
        sprite_DrawAtPos(u->iPosition.x - c->iPosition.x, u->iPosition.y - c->iPosition.y, u->sUnit, destination);
        /*draw_Box((int)u->position.x - c->iPosition.x, (int)u->position.y - c->iPosition.y, u->width, u->height, 1, &colourWhite, destination);*/
    }
    else
        sprite_DrawAtPos((int)u->position.x, (int)u->position.y, u->sUnit, destination);

    unitDrawTrack = gKer_GetDebugVar(0, 0);
    *unitDrawTrack = *unitDrawTrack + 1;

    return;
}

/*
    Function: vUnit_GiveProperties

    Description -
    This function sets the properties of a unit based upon the type it is given.

    2 arguments:
    Vent_Unit *u - the unit to set properties to
    int type - the type of unit, used to determine which properties are given
*/
void vUnit_GiveProperties(Vent_Unit *u, int type)
{
    switch(type)
    {
        case UNIT_TANK:

        vUnit_GiveTankProp(u);

        break;

        case UNIT_INFANTRY:

        vUnit_GiveInfantryProp(u);

        break;

        case UNIT_TOWER:

        vUnit_GiveTowerProp(u);

        break;

        case UNIT_AIR:

        vUnit_GiveAirProp(u);

        break;

        default:

        printf("Error: wrong unit type given %d\n", type);

        break;
    }

    return;
}

/*
    Function: vUnit_IsDestroyed

    Description -
    This function checks and determines if a unit is destroyed.
    Returns 1 if it is destroyed otherwise returns 0.

    1 argument:
    Vent_Unit *u -the unit that is to be checked
*/
int vUnit_IsDestroyed(Vent_Unit *u)
{
    if(u->destroyed == UNIT_ALIVE)
    {
        if(u->health > 0)
            return 0;
        else if(u->isPlayer == 1)
        {
            /*if the unit is the players unit, dont set it to be destroyed*/
            u->health = 0;
            u->maxSpeed = 0; /*freeze the players unit*/
            u->destroyed = UNIT_DEATH;
        }
        else
        {
            /*on the next call to this function the unit will
            have a state changed so that it is deleted from the game*/
            u->destroyed = UNIT_DEATH;
            return 1;
        }
    }
    else if(u->destroyed == UNIT_DEATH && u->isPlayer != 1)
        u->destroyed = UNIT_DELETE; /*change state to delete the unit from game*/

    return 1;
}

/*
    Function: vUnit_Buy

    Description -
    This function creates a unit in a game world.
    It also corrects the statistics of a side in the game to track the number of units.
    Returns a pointer to the new unit.

    7 arguments -
    int x - x axis location to spawn the unit
    int y - y axis location of unit
    unsigned int type - type of unit to create
    unsigned int team - the team of the unit
    unsigned int isPlayer - 1 if the unit should be the player, 0 otherwise
    Vent_Side *side - a pointer to the side that is buying the unit
    Timer *srcTime - a timer that the unit will perform with
*/
Vent_Unit *vUnit_Buy(int x, int y,
                    unsigned int type, unsigned int team,
                    unsigned int isPlayer,
                    Vent_Side *side, Timer *srcTime)
{
    Vent_Attributes *attributes = NULL;
    Vent_Unit *unitReturn = NULL;
    Vent_Target target;

    if(side->credits < side->unitDesc[type].price)
    {
        return NULL; /*not enough credits*/
    }

    switch(type)
    {
        case UNIT_TANK:

        /*Set position to be where the middle of the unit is*/
        x -= ve_Surfaces.tank[0][D_UP]->w/2;
        y -= ve_Surfaces.tank[0][D_UP]->h/2;
        break;

        case UNIT_INFANTRY:
        x -= ve_Surfaces.infantry[0][D_UP]->w/2;
        y -= ve_Surfaces.infantry[0][D_UP]->h/2;
        break;

        case UNIT_AIR:
        x -= ve_Surfaces.air[0][D_UP]->w/2;
        y -= ve_Surfaces.air[0][D_UP]->h/2;
        break;

        case UNIT_TOWER:

        /*Make sure to keep the maximum of towers at twice the currently owned citadel*/
        if((side->captures * 2) <= side->totalUnits[type])
        {
            printf("Not enough captures to build tower %d vs %d\n",side->captures, side->totalUnits[type]);
            return NULL;
        }

        /*Make sure the tower is being bought close enough to a citadel*/
        target = vAi_ScanBuildings(side->citadels,
                         x,
                         y,
                         250 * 250,
                         1);

        if(target.entity == NULL)
        {
            printf("No citadel %p in range from (%d, %d) to build tower\n", side->citadels, x, y);
            return NULL;
        }

        x -= ve_Surfaces.tower[0][D_UP]->w/2;
        y -= ve_Surfaces.tower[0][D_UP]->h/2;

        break;
    }


    if(isPlayer == 1)
    {
        attributes = &side->playerAttributes;
    }
    else
    {
        attributes = &side->unitAttributes[type];
    }

    unitReturn = vUnit_Create(x, y, team, type, isPlayer, 1, attributes, srcTime);


    /*update the stats on the units side*/
    side->readyUnits[type] --;
    side->credits -= side->unitDesc[type].price;
    side->creditsSpent += side->unitDesc[type].price;

    return unitReturn;
}

/*
    Function: vUnit_GiveAi

    Description -

    Gives an ai state to a unit.

    2 arguments:
    Vent_Unit *u - The unit the ai is ment for.
    Ai_State *ai - The ai state to give the unit.
*/
Ai_State *vUnit_GiveAi(Vent_Unit *u, Ai_State *ai)
{
    aiModule_PushState(&u->ai, ai);

    return ai;
}

/*
    Function: vUnit_NeedsDelete

    Description -
    Checks if a unit needs to be deleted.
    If so then it corrects game statistics and creates destroyed effects in the game world.
    Returns 1 if the unit needs to be deleted, 0 otherwise.

    2 arguments:
    Vent_Unit *u - the unit to check
    void *vg - the game world, the type is a Vent_Game structure
*/
int vUnit_NeedsDelete(Vent_Unit *u, void *vg)
{
    Vent_Game *vGame = vg;
    Vent_Effect *effect = NULL;

    static int scatterEffects = 6;
    static int scatterAmount = 10;
    static int scatterTimeDelta = 200;

    if(u->destroyed == UNIT_DELETE)
    {
        if(u->trackStat == 1) /*If it hasnt been created by level*/
        {
            vGame->side[u->team].totalUnits[u->type] --;
            vGame->side[u->team].lostUnits[u->type] ++;
        }

        if(vGame->side[u->team].enemy != NULL)
        {
            vGame->side[u->team].enemy->killedUnits[u->type] ++;
        }

        /*Create the destroyed sound if the unit is inside of the camera*/
        if(camera2D_Collision(&vGame->camera, u->iPosition.x, u->iPosition.y, u->width, u->height) == 1)
        {
            sound_Call(ve_Sounds.unitDestroyed[u->type], -1, SND_DUPLICATE, vGame_SoundVolume_Distance(vGame, u->iPosition.x, u->iPosition.y), 0);
        }

        switch(u->type)
        {
            case UNIT_TANK:
            /*Create the explosion effect*/
            effect = vEffect_Create(&ve_Sprites.unitExplosion, NULL, u->middlePosition.x - (sprite_Width(&ve_Sprites.unitExplosion)/2), u->middlePosition.y - (sprite_Height(&ve_Sprites.unitExplosion)/2),
                              250, &vGame->gTimer,
                              &vEffect_DrawStandard, dataStruct_Create(1, vg),
                              ker_Screen());

            vEffect_Insert(&vGame->effects, effect);

            /*Create the tank debris effect*/
            effect = vEffect_Create(&ve_Sprites.tankDestroyed, NULL, u->iPosition.x, u->iPosition.y,
                  2000, &vGame->gTimer,
                  &vEffect_DrawStandard, dataStruct_Create(1, vg),
                  ker_Screen());

            vEffect_Insert(&vGame->effects, effect);
            sprite_SetFrame(effect->sEffect, u->graphicTurn);

            /*if(options.sound && check_collision(&camera,&u->box) == 0)
                sound_Call(s_TankDestroyed, -1, 0, 1);*/

            break;

            case UNIT_INFANTRY:

            /*Create the unit debris effect*/
            effect = vEffect_Create(&ve_Sprites.infantryDestroyed, NULL, u->iPosition.x, u->iPosition.y,
                  2000, &vGame->gTimer,
                  &vEffect_DrawStandard, dataStruct_Create(1, vg),
                  ker_Screen());

            vEffect_Insert(&vGame->effects, effect);
            sprite_SetFrame(effect->sEffect, u->graphicTurn);

            break;

            case UNIT_AIR:

            /*Create aircraft debris effect*/
            effect = vEffect_Create(&ve_Sprites.airDestroyed, NULL, u->iPosition.x, u->iPosition.y,
                  2000, &vGame->gTimer,
                  &vEffect_DrawStandard, dataStruct_Create(1, vg),
                  ker_Screen());

            vEffect_Insert(&vGame->effects, effect);
            sprite_SetFrame(effect->sEffect, u->graphicTurn);

            /*Create flame effect*/
            effect = vEffect_Create(&ve_Sprites.flame, NULL, u->middlePosition.x, u->middlePosition.y,
                  400, &vGame->gTimer,
                  &vEffect_DrawScatter, dataStruct_Create(5, vg, &vGame->effects, &scatterEffects, &scatterAmount, &scatterTimeDelta),
                  ker_Screen());

            vEffect_Insert(&vGame->effects, effect);

            break;

            case UNIT_TOWER:

            /*if(options.sound && check_collision(&camera,&u->box) == 0)
                sound_Call(s_TankDestroyed,-1,0,1);

            nExplosion(unit->box.x,u->box.y,1,1000);*/

            break;
        }

        u->destroyed = UNIT_FINISHED;

        return 1;
    }
    else
        return 0;
}

/*
    Function: vUnit_DestroyList

    Description -
    This function deletes the units that need to be removed from the game world.

    2 arguments:
    struct list **units - should point to the head of the list of units
    void *vg - the game world structure, type is Vent_Game
*/
void vUnit_DestroyList(struct list **units, void *vg)
{
    struct list *currentUnit = *units;
    struct list *tempUnit = *units;
    Vent_Unit *unit = NULL;

    while (currentUnit != NULL)
    {
        unit = currentUnit->data;
        tempUnit = currentUnit->next;

        if(vUnit_NeedsDelete(unit, vg) == 1)
        {
            list_Delete_Node(units, currentUnit);
            vUnit_Clean(unit);
            mem_Free(unit);
        }

        currentUnit = tempUnit;
    }

    return;
}

/*
    Function: vUnit_Clean

    Description -
    Function that releases any dynamically allocated memory the unit structure contains.

    1 argument:
    Vent_Unit *u - the unit this function performs on
*/
void vUnit_Clean(Vent_Unit *u)
{
    /*remove ai make sure this is at the top*/
    aiModule_Clean(&u->ai);

    /*Clean up the old targets*/
    vTarget_ClearArray(&u->targetEnemy[0], UNIT_MAX_TARGETS);
    vTarget_ClearArray(&u->targetAlly[0], UNIT_MAX_TARGETS);

    sprite_Clean(&u->sUnitBase);
    sprite_Clean(&u->sUnitDamaged);

    list_Clear(&u->onTiles);

    depWatcher_Clean(&u->depWatcher);

    vWeapon_Clean(&u->weapon);

    return;
}
