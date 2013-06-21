#include "Tiles.h"

#include "Kernel/Kernel_State.h"
#include "Graphics/Surface.h"
#include "Common/List.h"
#include "Collision.h"
#include "File.h"
#include "Maths.h"

#include "../Units/Units.h"
#include "../Game/VentGame.h"
#include "../Effects.h"

char *vTile_Names[TILE_ENDTYPE] = {"None", "Decor", "Citadel", "Building", "Water", "Grass", "Snow", "Sand", "Custom"};

Vent_Tile *vTile_Create(int x, int y, int type, int graphicType, int collision, int layer, int team, int health, int state, int group, int rotationDeg, Sprite *sprite)
{
    Vent_Tile *newTile = (Vent_Tile *)mem_Malloc(sizeof(Vent_Tile), __LINE__, __FILE__);

    vTile_Setup(newTile, x, y, type, graphicType, collision, layer, team, health, state, group, rotationDeg, sprite);

    return newTile;
}

void vTile_Setup(Vent_Tile *t, int x, int y, int type, int graphicType, int collision, int layer, int team, int health, int state, int group, int rotationDeg, Sprite *sprite)
{
    t->base.position.x = x;
    t->base.position.y = y;

    if(sprite != NULL)
    {
        t->base.width = sprite_Width(sprite);
        t->base.height = sprite_Height(sprite);
    }
    else
    {
        t->base.width = t->base.height = 0;
    }

    if(t->base.width > t->base.height)
        t->radius = t->base.width/2;
    else
        t->radius = t->base.height/2;

    t->base.type = type;
    t->base.graphicType = graphicType;
    t->base.layer = layer;
    t->base.team = team;
    t->base.group = group;
    t->base.healthStarting = health;
    t->base.stateFlags = state;
    t->base.collision = collision;
    t->base.rotationDeg = rotationDeg;

    t->rotation = rotationDeg;
    t->sImage = sprite;

    if(t->sImage != NULL)
        t->sImage->layer = layer;

    t->destroyed = 0;

    if(t->base.healthStarting > 0)
    {
        /*Create the health bar 10 pixels above the tile*/
        t->healthBar = (Vent_HealthBar *)mem_Malloc(sizeof(Vent_HealthBar), __LINE__, __FILE__);
        vHealthBar_Setup(t->healthBar, NULL, t->base.position.x + (t->base.width/2), t->base.position.y - 10, 80, 8, 4, colourGreen);
    }
    else
        t->healthBar = NULL;

    vTile_Init(t);

    return;
}

void vTile_Init(Vent_Tile *t)
{
    t->unitsOn = 0;

    t->groupList = NULL;
    t->inSectors = NULL;
    t->numSector = 0;

    t->drawCalled = 0;
    t->healthPrev = t->health = t->base.healthStarting;

    t->citadel = NULL;
    t->building = NULL;

    t->destroyed = 0;

    depWatcher_Setup(&t->depWatcher, t);

    if(t->base.group == 0 && flag_Check(&t->base.stateFlags, TILESTATE_BUILDABLE) == 1)
        file_Log(ker_Log(), 1, "Error: Invalid buildable tile using group 0\n");

    return;
}

void vTile_ChangeGraphic(Vent_Tile *t, Sprite *graphic)
{
    if(t->sImage != NULL && t->sImage->local == 1)
    {
        sprite_Clean(t->sImage);
        mem_Free(t->sImage);
    }

    t->sImage = graphic;

    if(graphic != NULL)
    {
        t->base.width = sprite_Width(graphic);
        t->base.height = sprite_Height(graphic);
        graphic->layer = t->base.layer;
    }
    else
    {
        t->base.width = t->base.height = 0;
    }

    if(t->base.width > t->base.height)
        t->radius = t->base.width/2;
    else
        t->radius = t->base.height/2;

    return;
}

void vTile_Copy(int cleanDest, Vent_Tile *tileDest, Vent_Tile *tileSource)
{
    if(tileDest == tileSource)
    {
        printf("Warning: Trying to copy the same tile to itself\n");
        return;
    }

    if(cleanDest > 0)
    {
        vTile_Clean(tileDest);

        vTile_Setup(tileDest, tileDest->base.position.x,
                             tileDest->base.position.x,
                             tileSource->base.type,
                             tileSource->base.graphicType,
                             tileSource->base.collision,
                             tileSource->base.layer,
                             tileSource->base.team,
                             tileSource->base.healthStarting,
                             tileSource->base.stateFlags,
                             tileSource->base.group,
                             tileSource->base.rotationDeg,
                             sprite_Copy(tileSource->sImage, 1)
        );
    }
    else
    {
        tileDest->base.type = tileSource->base.type;
        tileDest->base.graphicType = tileSource->base.graphicType;
        tileDest->base.layer = tileSource->base.layer;
        tileDest->base.team = tileSource->base.team;
        tileDest->base.group = tileSource->base.group;
        tileDest->base.healthStarting = tileSource->base.healthStarting;
        tileDest->base.stateFlags = tileSource->base.stateFlags;
        tileDest->base.collision = tileSource->base.collision;
        tileDest->base.rotationDeg = tileSource->base.rotationDeg;

        tileDest->rotation = tileSource->base.rotationDeg;
        vTile_ChangeGraphic(tileDest, sprite_Copy(tileSource->sImage, 1));
    }

    tileDest->base.width = tileSource->base.width;
    tileDest->base.height = tileSource->base.height;

    return;
}

void vTile_ChangeGraphicSurface(Vent_Tile *t, struct list *lvlSurfaces, int graphicID)
{
    Frame *updateFrame = NULL;
    SDL_Surface *tileSurface = NULL;

    t->base.graphicType = graphicID;

    if(t->sImage != NULL)
    {
        updateFrame = sprite_RetrieveFrame(t->sImage, 0);

        frame_Clean(updateFrame);

        tileSurface = vLevel_SurfaceRetrieve(lvlSurfaces, graphicID, t->base.rotationDeg);
        t->rotation = t->base.rotationDeg;

        if(tileSurface == NULL)
        {
            printf("Warning: Cant find new rotated surface of (%d) so trying to use non-rotated version\n", graphicID);
            tileSurface = vLevel_SurfaceRetrieve(lvlSurfaces, graphicID, 0);

            if(tileSurface == NULL)
            {
                printf("Error: Cant obtain new graphic (%d) for tile\n", graphicID);
                return;
            }
        }
        else
        {
            frame_Setup(updateFrame, 0, tileSurface, 0, 0, 0, 0, 0, 0, M_FREE);
        }
    }
    else
    {
        printf("Error: Trying to change tile graphic with sprite as NULL\n");
    }

    return;
}

void vTile_SetRotation(Vent_Tile *t, struct list *lvlSurfaces, int rotationDeg)
{
    int numRevolutions = abs(rotationDeg/360);

    if(rotationDeg < 0)
    {
        rotationDeg = (360 * (numRevolutions + 1)) + rotationDeg;
    }
    else
    {
        rotationDeg = rotationDeg - (360 * numRevolutions);
    }

    t->base.rotationDeg = rotationDeg;

    vTile_ChangeGraphicSurface(t, lvlSurfaces, t->base.graphicType);
    if(t->sImage != NULL)
    {
        t->base.width = sprite_Width(t->sImage);
        t->base.height = sprite_Height(t->sImage);
    }
    else
    {
        t->base.width = t->base.height = 0;
    }

    if(t->base.width > t->base.height)
        t->radius = t->base.width/2;
    else
        t->radius = t->base.height/2;

    return;
}

Sprite *vTile_CreateSprite(struct list *surfaceList, int layer, Timer *srcTime, int graphicID, int rotationDeg)
{
    Sprite *sTile = sprite_Create();
    SDL_Surface *tileSurface = NULL;

    int autoFree = M_FREE;

    /*Try and find the correct surface from the level*/
    tileSurface = vLevel_SurfaceRetrieve(surfaceList, graphicID, rotationDeg);

    if(tileSurface == NULL)
    {
        /*If the correct surface is not found then try to find the non rotated version*/
        tileSurface = vLevel_SurfaceRetrieve(surfaceList, graphicID, 0);

        if(tileSurface == NULL)
        {
            /*If the non rotated version is still not found there must be an error*/
            mem_Free(sTile);

            return NULL;
        }

        /*Since there is no preloaded rotated version of the surface then create one*
        tileSurface = surface_Rotate(tileSurface, PI * (rotationDeg/180.0f), 0);
        autoFree = A_FREE;*/
    }

    sprite_Setup(sTile, 1, layer, srcTime, 1, frame_CreateBasic(0, tileSurface, autoFree));

    return sTile;
}



void vTile_Reset(Vent_Tile *t)
{
    t->drawCalled = 0;
    t->unitsOn = 0;

    return;
}

void vTile_Draw(Vent_Tile *t, Camera_2D *c, SDL_Surface *dest)
{
    unsigned int *tilesDrawn = NULL;

    if(t->destroyed == 1)
        return;

    if(c != NULL)
        sprite_DrawAtPos(t->base.position.x - c->iPosition.x, t->base.position.y - c->iPosition.y, t->sImage, dest);
    else
        sprite_DrawAtPos(t->base.position.x, t->base.position.y, t->sImage, dest);

    if(t->healthBar != NULL)
    {
        vHealthBar_Draw(t->healthBar, c, dest);
    }

    //tilesDrawn = gKer_GetDebugVar(1,0);
    //*tilesDrawn = *tilesDrawn + 1;

    return;
}

int vTile_CheckCollision(Vent_Tile *t, Vent_Unit *u)
{
    SDL_Rect A, B;

    A.x = t->base.position.x;
    A.y = t->base.position.y;
    A.w = t->base.width;
    A.h = t->base.height;

    B.x = u->iPosition.x;
    B.y = u->iPosition.y;
    B.w = u->width;
    B.h = u->height;

    return collisionStatic_Rectangle(&A, &B);
}

/*
    Function: vTile_IsDestroyed

    Description -
    Returns 1 if a tile is destroyed, also created the destroyed effect of the tile.

    2 arguments:
    Vent_Tile *tile - The tile to check if its been destroyed.
    void *g - The current game. Vent_Game structure
*/
int vTile_IsDestroyed(Vent_Tile *tile, void *g)
{
    Vent_Game *game = g;
    static int scatterEffects = 5;
    static int scatterAmount = 20;
    static int scatterTimeDelta = 100;

    if(tile->health <= 0)
    {
        if((tile->base.width * tile->base.height) < 200)
        {
            vEffect_Insert(&game->effects,
                   vEffect_Create(&ve_Sprites.bulletHit, NULL, tile->base.position.x + (tile->base.width/2) - (sprite_Width(&ve_Sprites.bulletHit)/2), tile->base.position.y + (tile->base.height/2) - (sprite_Height(&ve_Sprites.bulletHit)/2),
                                  250, &game->gTimer,
                                  &vEffect_DrawScatter, dataStruct_Create(5, game, &game->effects, &scatterEffects, &scatterAmount, &scatterTimeDelta),
                                  ker_Screen())
            );
        }
        else
        {
            vEffect_Insert(&game->effects,
                   vEffect_Create(&ve_Sprites.unitExplosion, NULL, tile->base.position.x + (tile->base.width/2) - (sprite_Width(&ve_Sprites.unitExplosion)/2), tile->base.position.y + (tile->base.height/2) - (sprite_Height(&ve_Sprites.unitExplosion)/2),
                                  250, &game->gTimer,
                                  &vEffect_DrawScatter, dataStruct_Create(5, game, &game->effects, &scatterEffects, &scatterAmount, &scatterTimeDelta),
                                  ker_Screen())
            );

        }

        tile->destroyed = 1;
    }

    return tile->destroyed;
}

void vTile_Clean(Vent_Tile *t)
{
    if(t->sImage != NULL && t->sImage->local == 1)
    {
        sprite_Clean(t->sImage);
        mem_Free(t->sImage);
    }

    if(t->healthBar != NULL)
    {
        vHealthBar_Clean(t->healthBar);
        mem_Free(t->healthBar);
    }

    depWatcher_Clean(&t->depWatcher);

    list_Clear(&t->groupList);
    list_Clear(&t->inSectors);

    return;
}

void vTile_Save(Vent_Tile *t, struct list *lSurfaces, FILE *save)
{
    file_Log(save, 0, "%d %d %d %d %d %d %d %d %d %d %d %d %d\n", t->base.position.x,
                                                                  t->base.position.y,
                                                                  t->base.width,
                                                                  t->base.height,
                                                                  t->base.layer,
                                                                  t->base.team,
                                                                  t->base.collision,
                                                                  t->base.type,
                                                                  t->base.healthStarting,
                                                                  t->base.stateFlags,
                                                                  t->base.graphicType,
                                                                  t->base.group,
                                                                  t->base.rotationDeg);

    return;
}

Vent_Tile *vTile_Load(struct list **lSurfaces, Timer *srcTime,  FILE *save)
{
    Vent_Tile *t = (Vent_Tile *)mem_Malloc(sizeof(Vent_Tile), __LINE__, __FILE__);
    Sprite *sTile = NULL;
    int width = 0;
    int height = 0;

    fscanf(save, "%d%d%d%d%d%d%d%d%d%d%d%d%d", &t->base.position.x,
                                               &t->base.position.y,
                                               &width,
                                               &height,
                                               &t->base.layer,
                                               &t->base.team,
                                               &t->base.collision,
                                               &t->base.type,
                                               &t->base.healthStarting,
                                               &t->base.stateFlags,
                                               &t->base.graphicType,
                                               &t->base.group,
                                               &t->base.rotationDeg);

    if(t->base.graphicType < 0 && vLevel_SurfaceRetrieve(*lSurfaces, t->base.graphicType, t->base.rotationDeg) == NULL)
    {
        printf("Error cant find required graphic for tile, type %d with rotation %d\n", t->base.graphicType, t->base.rotationDeg);
    }

    sTile = vTile_CreateSprite(*lSurfaces, t->base.layer, srcTime, t->base.graphicType, t->base.rotationDeg);

    vTile_Setup(t, t->base.position.x, t->base.position.y, t->base.type, t->base.graphicType, t->base.collision, t->base.layer, t->base.team,  t->base.healthStarting, t->base.stateFlags, t->base.group, t->base.rotationDeg, sTile);

    t->base.width = width;
    t->base.height = height;

    vTile_Init(t);

    return t;
}

void vTile_GroupUp(struct list *tileList)
{
    struct list *tileSecondList = NULL;
    int numGrouped = 0;

    Vent_Tile *tileA = NULL;
    Vent_Tile *tileB = NULL;

    while(tileList != NULL)
    {
        tileSecondList = tileList->next;

        tileA = tileList->data;

        if(tileA->base.group != 0)
        {
            while(tileSecondList != NULL)
            {
                tileB = tileSecondList->data;

                if(tileA->base.group == tileB->base.group)
                {
                    list_Push(&tileA->groupList, tileB, 0);
                    list_Push(&tileB->groupList, tileA, 0);

                    numGrouped ++;
                }

                tileSecondList = tileSecondList->next;
            }
        }

        tileList = tileList->next;
    }

    //file_Log(ker_Log(), 1, "Tiles linked: %d\n", numGrouped);

    return;
}
