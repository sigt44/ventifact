#include "Bullets.h"

#include "Vector2D.h"
#include "Collision.h"

#include "Graphics/Sprite.h"
#include "Graphics/Graphics.h"
#include "Font.h"
#include "Maths.h"
#include "Kernel/Kernel.h"

#include "Defines.h"
#include "Units/Units.h"
#include "Bullet_Attributes.h"
#include "Buildings/Buildings.h"
#include "Game/VentGame.h"
#include "Images.h"
#include "Effects.h"
#include "Sounds.h"

void vBullet_GiveDirection(Vent_Bullet *b, int accurate)
{
    if(accurate == 0)
    {
        switch(b->direction)
        {
            case D_UP:
                b->velocity.x = 0.0f;
                b->velocity.y = -b->speed;
            break;

            case D_DOWN:
                b->velocity.x = 0.0f;
                b->velocity.y = b->speed;
            break;

            case D_LEFT:
                b->velocity.x = -b->speed;
                b->velocity.y = 0.0f;
            break;

            case D_RIGHT:
                b->velocity.x = b->speed;
                b->velocity.y = 0.0f;
            break;

            case D_UPRIGHT:
                b->velocity.x = 0.7071f * b->speed;
                b->velocity.y = -b->velocity.x;
            break;

            case D_UPLEFT:
                b->velocity.x = -0.7071f * b->speed;
                b->velocity.y = b->velocity.x;
            break;

            case D_DOWNLEFT:
                b->velocity.x = -0.7071f * b->speed;
                b->velocity.y = -b->velocity.x;
            break;

            case D_DOWNRIGHT:
                b->velocity.x = 0.7071f * b->speed;
                b->velocity.y = b->velocity.x;
            break;
        }
    }
    else
    {
        b->velocity.x = b->velocity.x * b->speed;
        b->velocity.y = b->velocity.y * b->speed;
    }

    return;
}

Vent_Bullet *vBullet_Create(int ID, int x, int y, Vent_Bullet_Attributes *bA, Vent_Sector *inSector, int team, void *direction, Timer *srcTime)
{
    Vent_Bullet *b = (Vent_Bullet *)mem_Malloc(sizeof(Vent_Bullet), __LINE__, __FILE__);

    vBullet_Setup(b, ID, x, y, bA, inSector, team, direction, srcTime);

    return b;
}

void vBullet_Setup(Vent_Bullet *b, int ID, int x, int y, Vent_Bullet_Attributes *bA, Vent_Sector *inSector, int team, void *direction, Timer *srcTime)
{
    Vent_WeaponDirection *wD = direction;

    b->ID = ID;

    b->direction = wD->direction;
    b->velocity = wD->moveScale;

    b->currentDistance = 0.0f;

    b->team = team;

    vBulletAttributes_GiveProp(b, bA);

    x = x - sprite_Width(b->sBullet)/2;
    y = y - sprite_Height(b->sBullet)/2;

    b->position.x = (float)x;
    b->dPosition.x = x;
    b->position.y = (float)y;
    b->dPosition.y = y;

    b->inSector = inSector;

    b->status = BULLET_NO_HIT;

    vBullet_GiveDirection(b, wD->accurate);

    timer_Start(&b->lifeTimer);

    if(b->strikethrough > 0)
    {
        timer_Start(&b->strikethroughTimer);
    }

    return;
}

int vBullet_Update(Vent_Bullet *b, void *game, struct list *buildings, float dt) //If returns 1 then bullet should be deleted
{
    Vector2D fV;
    Vent_Game *g = game;

    /*Return early if the bullet is already destroyed*/
    if(b->status == BULLET_DESTROYED)
    {
        return b->status;
    }

    /*update the position, distance travelled*/
    fV = vector_MultiplyBy(&b->velocity, dt);
    b->position = vector_Add(&b->position, &fV);

    b->currentDistance += b->speed * dt;

    b->dPosition.x = (int)b->position.x;
    b->dPosition.y = (int)b->position.y;

    /*Return early if the bullet has reached its max distance*/
    if((int)b->currentDistance >= b->maxDistance)
    {
        b->status = BULLET_DESTROYED;
        return b->status;
    }

    /*Find the sector the bullet is in*/
    vBullet_SetSector(b, g->level);

    /*Update the strikethrough timer*/
    if(b->strikethrough > 0)
    {
        timer_Calc(&b->strikethroughTimer);
    }

    /*Check for collision on buildings*/
    b->status = vBullet_UpdateBuildings(b, buildings);

    /*Check for collision on units*/
    if(b->status == BULLET_NO_HIT)
        b->status = vBullet_UpdateUnits(b, g);

    /*Check for collision on tiles*/
    if(b->status == BULLET_NO_HIT)
        b->status = vBullet_UpdateTiles(b, g->level->destroyableTiles);

    /*Create an effect if the bullet hit something*/
    if(b->status != BULLET_NO_HIT)
    {
        vBullet_DamageEffect(b, g);

        if(b->strikethroughStart == 0 || b->strikethroughStart == (b->strikethrough + 1))
        {
            if(g->playerUnit != NULL && b->ID == g->playerUnit->ID)
            {
                g->stats.bulletsHit ++;
            }
        }
    }

    /*Standard effects when the bullet is moving*/
    vBullet_UpdateEffect(b, g);

    /*If the bullet has a life time, check if it has ended*/
    if(b->lifeTimer.end_Time != 0)
    {
        timer_Calc(&b->lifeTimer);

        if(timer_Get_Remain(&b->lifeTimer) <= 0)
            b->status = BULLET_DESTROYED;
    }

    return b->status;
}

void vBullet_UpdateEffect(Vent_Bullet *b, void *game)
{
    switch(b->type)
    {
        default:
        break;

        case WEAPON_FLAME:

        sound_Call(ve_Sounds.bulletFlame, b->ID, SND_APPEND, vGame_SoundVolume_Distance(game, b->dPosition.x, b->dPosition.y), 0);

        break;
    }

    return;
}


void vBullet_DamageEffect(Vent_Bullet *b, void *game)
{
    Vent_Game *g = game;

    switch(b->type)
    {
        case WEAPON_REPAIR:

        vEffect_Insert(&g->effects,
           vEffect_Create(&ve_Sprites.heal, NULL, b->dPosition.x, b->dPosition.y,
                          400, &g->gTimer,
                          &vEffect_DrawStandard, dataStruct_Create(1, game),
                          ker_Screen())
        );

        break;

        case WEAPON_FLAME:

        vEffect_Insert(&g->effects,
           vEffect_Create(&ve_Sprites.flame, NULL, b->dPosition.x, b->dPosition.y,
                          400, &g->gTimer,
                          &vEffect_DrawStandard, dataStruct_Create(1, game),
                          ker_Screen())
        );

        break;

        case WEAPON_MINE:

        vEffect_Insert(&g->effects,
           vEffect_Create(&ve_Sprites.unitExplosion, ve_Sounds.bulletExplosion, b->dPosition.x + sprite_Width(b->sBullet)/2 - sprite_Width(&ve_Sprites.unitExplosion)/2, b->dPosition.y + sprite_Height(b->sBullet)/2 - sprite_Height(&ve_Sprites.unitExplosion)/2,
                          250, &g->gTimer,
                          &vEffect_DrawStandard, dataStruct_Create(1, game),
                          ker_Screen())
        );

        //sound_Call(ve_Sounds.bulletExplosion, -1, SND_DUPLICATE, vGame_SoundVolume_Distance(game, b->dPosition.x, b->dPosition.y), 0);


        break;

        case WEAPON_PLASMA:
        case WEAPON_PLASMA_WEAK:
        case WEAPON_MISSILE:
        case WEAPON_ROCKET:
        default:

        vEffect_Insert(&g->effects,
                       vEffect_Create(&ve_Sprites.bulletHit, NULL, b->dPosition.x, b->dPosition.y,
                                      150, &g->gTimer,
                                      &vEffect_DrawStandard, dataStruct_Create(1, game),
                                      ker_Screen())
        );

        break;
    }

    return;
}

void vBullet_SetSector(Vent_Bullet *b, Vent_Level *level)
{
    Vent_Sector *s = b->inSector;
    struct list *sectors = level->sectors;
    int foundSector = 0;
    int x = 0;

    if(b->dPosition.x < 0 || b->dPosition.y < 0 || b->dPosition.x >= level->header.width || b->position.y >= level->header.height)
    {
        return;
    }

	/*if the bullet is still in the same sector as last check, return early*/
    if(s != NULL &&
        collisionStatic_Rectangle2(
            b->dPosition.x, b->dPosition.y,
            sprite_Width(b->sBullet), sprite_Height(b->sBullet),
            s->position.x, s->position.y,
            s->width, s->height) == 1)
    {
        return;
    }

	/*search through the list of sectors*/
    while(sectors != NULL)
    {
        if(s != NULL)
            s = vSector_Find(s, b->dPosition.x, b->dPosition.y);
        else
            s = vSector_Find(sectors->data, b->dPosition.x, b->dPosition.y);

        if(collisionStatic_Rectangle2(
            b->dPosition.x, b->dPosition.y, sprite_Width(b->sBullet), sprite_Height(b->sBullet),
            s->position.x, s->position.y, s->width, s->height) == 1)
        {
            b->inSector = s;

            foundSector = 1;
            /*exit early, new sector has been found*/
            return;
        }

        sectors = sectors->next;
        x++;
    }

    /*The unit is not inside any sector on the level*/
    if(foundSector == 0)
    {
        //printf("Warning could not find sector for bullet at %d %d\n", b->dPosition.x, b->dPosition.y);
        if(b->inSector != NULL)
        {
            b->inSector = NULL;
        }
    }

    return;
}

int vBullet_UpdateBuildings(Vent_Bullet *b, struct list *buildings)
{
    int bHit = BULLET_NO_HIT;
    int didHit = BULLET_NO_HIT;

    int bWidth = sprite_Width(b->sBullet);
    int bHeight = sprite_Height(b->sBullet);

    Vent_Building *building = NULL;

    /*Go through all the buildings and perform collision detection*/
    while(buildings != NULL)
    {
        building = buildings->data;

        if(vBullet_TeamCheck(b, building->team) == 1 && collisionStatic_Rectangle2(
                building->position.x, building->position.y, building->width, building->height,
                b->dPosition.x, b->dPosition.y, bWidth, bHeight) == 1)
        {
            /*If the building has no citadel linked then do extra damage*/
            if(vBuilding_HasCitadel(building) == 0 && building->team != b->team)
            {
                b->damage *= 3;
                bHit = vBullet_Hit(b, &building->health, building->sBuilding.layer);
                b->damage /=3;
            }
            else
            {
                bHit = vBullet_Hit(b, &building->health, building->sBuilding.layer);
            }

            /*Exit early if the bullet is destroyed or the bullet has no more strikethrough*/
            if(bHit == BULLET_DESTROYED || bHit == BULLET_HIT_STRIKE)
                return bHit;

            if(bHit == BULLET_HIT)
                didHit = BULLET_HIT;
        }

        buildings = buildings->next;
    }

    return didHit;
}

int vBullet_UpdateTiles(Vent_Bullet *b, struct list *tiles)
{
    int bHit = BULLET_NO_HIT;
    int didHit = BULLET_NO_HIT;

    int bWidth = sprite_Width(b->sBullet);
    int bHeight = sprite_Height(b->sBullet);

    Vent_Tile *tile = NULL;

    while(tiles != NULL)
    {
        tile = tiles->data;

        if(tile->destroyed == 0 && vBullet_TeamCheck(b, tile->base.team) == 1 &&
                collisionStatic_Rectangle2(
                tile->base.position.x, tile->base.position.y, tile->base.width, tile->base.height,
                b->dPosition.x, b->dPosition.y, sprite_Width(b->sBullet), sprite_Height(b->sBullet)) == 1)
        {
            bHit = vBullet_Hit(b, &tile->health, tile->base.layer);

            /*Exit early if the bullet is destroyed or the bullet has no more strikethrough*/
            if(bHit == BULLET_DESTROYED || bHit == BULLET_HIT_STRIKE)
                return bHit;

            if(bHit == BULLET_HIT)
                didHit = BULLET_HIT;
        }

        tiles = tiles->next;
    }

    return didHit;
}

int vBullet_UpdateUnits(Vent_Bullet *b, void *game)
{
    int bHit = BULLET_NO_HIT;
    int didHit = BULLET_NO_HIT;

    int x = 0;
    Vent_Unit *u = NULL;

    struct list *units = NULL;

    if(b->inSector == NULL)
    {
        return BULLET_NO_HIT;
    }

    /*Search the units that are in the same sector as the bullet*/
    bHit = vBullet_HitUnits(b, b->inSector->units, game);

    /*Return early if the bullet hit something in that sector*/
    if(bHit != BULLET_NO_HIT)
    {
        return bHit;
    }

    for(x = 0; x < SECTOR_ADJACENT; x++)
    {
        if(b->inSector->adjacent[x] != NULL)
        {
            /*Search through the units in an adjacent sector*/
            bHit = vBullet_HitUnits(b, b->inSector->adjacent[x]->units, game);

            /*Return early if the bullet hit something in that sector*/
            if(bHit != BULLET_NO_HIT)
            {
                return bHit;
            }
        }
    }

    return BULLET_NO_HIT;
}

int vBullet_Hit(Vent_Bullet *b, int *targetHealth, int targetHeight)
{
    int bStatus = BULLET_NO_HIT;

    /*If the height of the bullet will not reach the targets height, return early*/
    if(b->maxHeight < targetHeight)
    {
        return BULLET_NO_HIT;
    }

    /*If the bullet can hit more than once*/
    if(b->strikethrough > 0)
    {
        /*Make sure that the interval between each hit is large enough*/
        if(timer_Get_Remain(&b->strikethroughTimer) <= 0)
        {
            b->strikethrough --;

            timer_Start(&b->strikethroughTimer);
        }
        else
        {
            return BULLET_HIT_STRIKE;
        }
    }

    switch(b->type)
    {
        case WEAPON_REPAIR:

        *targetHealth += b->damage;
        bStatus = BULLET_HIT;

        break;

        case WEAPON_PLASMA_WEAK:
        case WEAPON_PLASMA:
        case WEAPON_FLAME:
        case WEAPON_MINE:
        case WEAPON_MISSILE:
        case WEAPON_ROCKET:
        default:

        *targetHealth -= b->damage;
        bStatus = BULLET_HIT;

        break;

    }

    /*Since there is no more strikethrough and the bullet hit something, destroy it*/
    if(b->strikethrough == 0 && bStatus == BULLET_HIT)
    {
        bStatus = BULLET_DESTROYED;
    }

    return bStatus;
}

int vBullet_TeamCheck(Vent_Bullet *b, int team)
{
    switch(b->type)
    {
        default:

        if(team != b->team)
        {
            return 1;
        }

        break;

        case WEAPON_REPAIR:

        if(team == b->team)
        {
            return 1;
        }

        break;
    }

    return 0;
}


int vBullet_HitUnits(Vent_Bullet *b, struct list *units, void *game)
{
    Vent_Game *g = game;
    Vent_Unit *u = NULL;
    int bHit = BULLET_NO_HIT;
    int didHit = BULLET_NO_HIT;

    int bWidth = sprite_Width(b->sBullet);
    int bHeight = sprite_Height(b->sBullet);

    while(units != NULL)
    {
        u = units->data;

        /*If the bullet does collide with the unit*/
        if(u->invulnerability == 0 && vBullet_TeamCheck(b, u->team) == 1 &&
           collisionStatic_Rectangle2(
                    u->iPosition.x, u->iPosition.y, u->width, u->height,
                    b->dPosition.x, b->dPosition.y, bWidth, bHeight) == 1)
        {
            bHit = vBullet_Hit(b, &u->health, u->layer);

            /*Update stats if the player was hit*/
            if(bHit != BULLET_NO_HIT && g->playerUnit == u && b->team != u->team)
            {
                g->stats.healthLost += b->damage;
            }

            /*Exit early if the bullet is destroyed or the bullet has no more strikethrough*/
            if(bHit == BULLET_DESTROYED || bHit == BULLET_HIT_STRIKE)
                return bHit;

            if(bHit == BULLET_HIT)
            {
                didHit = BULLET_HIT;
            }
        }

        units = units->next;
    }

    return didHit;
}


void vBullet_Clean(Vent_Bullet *b)
{
    sprite_Clean(b->sBullet);

    mem_Free(b->sBullet);

    return;
}

void vBullet_Draw(Vent_Bullet *b, Camera_2D *c, SDL_Surface *dest)
{
    if(c != NULL)
        sprite_DrawAtPos(b->dPosition.x - c->iPosition.x, b->dPosition.y - c->iPosition.y, b->sBullet, dest);
    else
        sprite_DrawAtPos(b->dPosition.x, b->dPosition.y, b->sBullet, dest);

    return;
}
