#include "Sector.h"

#include "Collision.h"
#include "Kernel/Kernel.h"
#include "File.h"
#include "Graphics/Graphics.h"

#include "Tiles.h"
#include "../Units/Units.h"

void vSector_Report(Vent_Sector *s)
{
    file_Log(ker_Log(), 0, "Sector (%d, %d) (%d, %d)\n", s->position.x, s->position.y, s->width, s->height);

    return;
}

struct list *vSector_Tile(int sWidth, int sHeight, int gWidth, int gHeight)
{
    float remainX = (float)gWidth/sWidth;
    float remainY = (float)gHeight/sHeight;

    int numX = (int)remainX;
    int numY = (int)remainY;
    int x, y;
    int eW, eH;

    struct list *sectors = NULL;

    //file_Log(ker_Log(), 1, "Sectors setup: (%d %d) %d %d (%f %f)\n", sWidth, sHeight, numX, numY, remainX, remainY);

    for(x = 0; x < numX; x++)
    {
        for(y = 0; y < numY; y++)
        {
            /*file_Log(ker_Log(), 0, "[%d %d] %d %d %d %d\n", x, y,sWidth * x, sHeight * y, sWidth, sHeight);*/
            list_Stack(&sectors, vSector_Create(sWidth * x, sHeight * y, sWidth, sHeight), 0);
        }
    }

    if( (eW = (gWidth - (sWidth * numX))) != 0)
    {
        for(y = 0; y < numY; y++)
            list_Stack(&sectors, vSector_Create(sWidth * numX, sHeight * y, sWidth, sHeight), 0);
    }

    if( (eH = (gHeight - (sHeight * numY))) != 0)
    {
        for(x = 0; x < numX; x++)
            list_Stack(&sectors, vSector_Create(sWidth * x, sHeight * numY, sWidth, sHeight), 0);
    }

    if(eH != 0 && eW != 0)
        list_Stack(&sectors, vSector_Create(sWidth * numX, sHeight * numY, sWidth, sHeight), 0);

    return sectors;
}

Vent_Sector *vSector_Create(int x, int y, int w, int h)
{
    Vent_Sector *newSector = (Vent_Sector *)mem_Malloc(sizeof(Vent_Sector), __LINE__, __FILE__);

    vSector_Setup(newSector, x, y, w, h);

    return newSector;
}

void vSector_Setup(Vent_Sector *s, int x, int y, int w, int h)
{
    int z = 0;

    s->position.x = x;
    s->position.y = y;
    s->width = w;
    s->height = h;

    s->tiles = NULL;
    s->collisionTiles = NULL;
    s->units = NULL;

    for(z = 0; z < SECTOR_ADJACENT; z++)
        s->adjacent[z] = NULL;

    //printf("\tSector up: (%d %d) (%d, %d)\n", s->position.x, s->position.y, s->width, s->height);
    return;
}

Vent_Sector *vSector_Find(Vent_Sector *startSector, int x, int y)
{
    int s = 0;
    int xDis = (x - startSector->position.x);
    int yDis = (y - startSector->position.y);
    int widthIteration = (xDis/startSector->width);
    int heightIteration = (yDis/startSector->height);
    int directionX = D_RIGHT;
    int directionY = D_DOWN;

    Vent_Sector *targetSector;

    //printf("Trying to find sector. (%d, %d) (%d, %d)(%d, %d) - (%d, %d)\n", x, y, startSector->position.x, startSector->position.y, startSector->width, startSector->height, widthIteration, heightIteration);
    if(xDis < 0)
    {
        widthIteration --;
    }
    if(yDis < 0)
    {
        heightIteration --;
    }

    if(widthIteration < 0)
    {
        directionX = D_LEFT;
        widthIteration = -widthIteration;
    }

    if(heightIteration < 0)
    {
        heightIteration = -heightIteration;
        directionY = D_UP;
    }

    targetSector = startSector;
    for(s = 0; s < widthIteration; s++)
    {
        if(targetSector->adjacent[directionX] != NULL)
        {
            targetSector = targetSector->adjacent[directionX];
        }
        else if(targetSector->position.x + targetSector->width == x)
        {
            /*puts("Sector X is on the edge");*/
            s = widthIteration;
        }
        else
        {
            printf("Warning sector adjacentX is NULL [%d] %d At (%d %d)(%d %d)\n",s, widthIteration, targetSector->position.x, targetSector->position.y, targetSector->width, targetSector->height);
            printf("\tTrying to find sector. (%d, %d) (%d, %d)(%d, %d) - (%d, %d)\n", x, y, startSector->position.x, startSector->position.y, startSector->width, startSector->height, widthIteration, heightIteration);
            s = widthIteration;
        }
    }

    for(s = 0; s < heightIteration; s++)
    {
        if(targetSector->adjacent[directionY] != NULL)
        {
            targetSector = targetSector->adjacent[directionY];
        }
        else if(targetSector->position.y + targetSector->height == y)
        {
            /*puts("Sector Y is on the edge");*/
            s = heightIteration;
        }
        else
        {
            printf("Warning sector adjacentY is NULL [%d] %d At (%d %d)(%d %d)\n",s, heightIteration, targetSector->position.x, targetSector->position.y, targetSector->width, targetSector->height);
            printf("Trying to find sector. (%d, %d) (%d, %d)(%d, %d) - (%d, %d)\n", x, y, startSector->position.x, startSector->position.y, startSector->width, startSector->height, widthIteration, heightIteration);
            s = heightIteration;
        }
    }

    return targetSector;
}

int vSector_Draw(Vent_Sector *s, Camera_2D *c, SDL_Surface *dest)
{
    struct list *i = NULL;
    Vent_Tile *t = NULL;

    //if(c != NULL && camera2D_Collision(c, s->position.x, s->position.y, s->width, s->height) == 0)
      //  return 0;

    //draw_Box(s->position.x - c->iPosition.x, s->position.y - c->iPosition.y, s->width, s->height, 1, &colourWhite, ker_Screen());
    //draw_Box(s->position.x - c->iPosition.x, s->position.y - c->iPosition.y, 10, 10, 1, &colourBlack, ker_Screen());

    i = s->tiles;

    while(i != NULL)
    {
        t = i->data;
        t->drawCalled ++;

        i = i->next;
    }

    i = s->units;

    while(i != NULL)
    {
        vUnit_Draw(i->data, c, dest);
        i = i->next;
    }

    return 1;
}

void vSector_AssignAdjacent(Vent_Sector *p, Vent_Sector *c)
{
    SDL_Rect A;
    SDL_Rect B;

    A.x = p->position.x;
    A.y = p->position.y;
    A.w = p->width;
    A.h = p->height;

    B.x = c->position.x;
    B.y = c->position.y;
    B.w = c->width;
    B.h = c->height;

    /*Check left*/
    A.x -= 1;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_RIGHT] = p;
        p->adjacent[D_LEFT] = c;
        return;
    }

    /*Check right*/
    A.x += 2;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_LEFT] = p;
        p->adjacent[D_RIGHT] = c;
        return;
    }

    /*Check down*/
    A.x -= 1;
    A.y += 1;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_UP] = p;
        p->adjacent[D_DOWN] = c;
        return;
    }

    /*Check up*/
    A.y -= 2;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_DOWN] = p;
        p->adjacent[D_UP] = c;
        return;
    }

    /*Check up left*/
    A.x -= 1;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_DOWNRIGHT] = p;
        p->adjacent[D_UPLEFT] = c;
        return;
    }

    /*Check up right*/
    A.x += 2;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_DOWNLEFT] = p;
        p->adjacent[D_UPRIGHT] = c;
        return;
    }

    /*Check down right*/
    A.y += 2;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_UPLEFT] = p;
        p->adjacent[D_DOWNRIGHT] = c;
        return;
    }

    /*Check down left*/
    A.x -= 2;
    if(collisionStatic_Rectangle(&A, &B) == 1)
    {
        c->adjacent[D_UPRIGHT] = p;
        p->adjacent[D_DOWNLEFT] = c;
        return;
    }


    return;
}

void vSector_Map(struct list *sectorList)
{
    struct list *i = NULL;

    while(sectorList != NULL)
    {
        i = sectorList->next;

        while(i != NULL)
        {
            vSector_AssignAdjacent(sectorList->data, i->data);

            i = i->next;
        }

        sectorList = sectorList->next;
    }

    return;
}

int vSector_MapTileInsert(Vent_Sector *s, void *tile)
{
    Vent_Tile *t = tile;
    int drawHeight = 0;
    SDL_Rect A, B;

    A.x = s->position.x;
    A.y = s->position.y;
    A.w = s->width;
    A.h = s->height;

    B.x = t->base.position.x;
    B.y = t->base.position.y;
    B.w = t->base.width;
    B.h = t->base.height;
    drawHeight = B.y + B.h;

    if(collisionStatic_Rectangle(&A, &B) == 1 && list_MatchCheck(t->inSectors, s) == NULL)
    {
        list_Push_Sort(&s->tiles, t, drawHeight);
        depWatcher_Add(&t->depWatcher, &s->tiles);

        list_Stack(&t->inSectors, s, 0);
        t->numSector ++;

        /*If the tile will need some sort of collision detection*/
        if(t->base.collision > 0 ||
           flag_Check(&t->base.stateFlags, TILESTATE_BUILDABLE) == 1 ||
           t->base.type == TILE_WATER)
        {
            /*Push it into another list so that they can be found easier*/
            list_Stack(&s->collisionTiles, t, 0);
            depWatcher_Add(&t->depWatcher, &s->collisionTiles);
        }

        return 1;
    }

    return 0;
}

int vSector_MapTile(struct list *sectors, void *tile)
{
    int mapped = 0;

    while(sectors != NULL)
    {
        mapped += vSector_MapTileInsert(sectors->data, tile);

        sectors = sectors->next;
    }

    return mapped;
}

void vSector_MapTiles(struct list *sectors, struct list *tiles)
{
    struct list *sortTiles = tiles;
    struct list *sort = NULL;

    while(sectors != NULL)
    {
        sort = sortTiles;

        while(sort != NULL)
        {
            vSector_MapTileInsert(sectors->data, sort->data);

            sort = sort->next;
        }

        sectors = sectors->next;
    }

    return;
}

void *vSector_GetTile(Vent_Sector *s, int closest, int posX, int posY, int w, int h)
{
    struct list *tiles = s->tiles;
    Vent_Tile *tile = NULL;
    Vent_Tile *tileClosest = NULL;
    int closestDistanceSqr = -1;
    int distanceSqr = -1;
    int highestLayer = -1;

    SDL_Rect A, B;
    int xDis = 0;
    int yDis = 0;

    A.x = posX;
    A.y = posY;
    A.w = w;
    A.h = h;

    /*Start from the end of the list*/
    if(tiles != NULL && tiles->previous != NULL)
        tiles = tiles->previous;

    while(tiles != NULL)
    {
        tile = tiles->data;

        B.x = tile->base.position.x;
        B.y = tile->base.position.y;
        B.w = tile->base.width;
        B.h = tile->base.height;

        if(collisionStatic_Rectangle(&A, &B) == 1)
        {
            if(closest == 0)
                return tile;
            else
            {
                xDis = (B.x + B.w/2) - A.x;
                yDis = (B.y + B.h/2) - A.y;
                distanceSqr = (xDis * xDis) + (yDis * yDis);
                if(closestDistanceSqr == -1 || (tile->base.layer > highestLayer) || ((distanceSqr < closestDistanceSqr) && (tile->base.layer >= highestLayer)))
                {
                    highestLayer = tile->base.layer;
                    closestDistanceSqr = distanceSqr;
                    tileClosest = tile;
                }
            }
        }

        tiles = tiles->previous;

        if(tiles == NULL || tiles->next == NULL)
        {
            break;
        }
    }

    return tileClosest;
}

void vSector_Update(Vent_Sector *s, struct list *units)
{
    SDL_Rect A, B;
    Vent_Unit *u = NULL;

    list_Clear(&s->units);

    A.x = s->position.x;
    A.y = s->position.y;
    A.w = s->width;
    A.h = s->height;

    while(units != NULL)
    {
        u = units->data;

        B.x = u->iPosition.x;
        B.y = u->iPosition.y;
        B.w = u->width;
        B.h = u->height;

        if(collisionStatic_Rectangle(&A, &B) == 1)
        {
            if(u->inSector != s)
            {
                if(u->inSector != NULL)
                    list_Delete_NodeFromData(&u->inSector->units, u);

                u->inSector = s;
            }

            list_Push(&s->units, u, 0);
        }

        units = units->next;
    }

    return;
}

void vSector_Clean(Vent_Sector *s)
{
    Vent_Tile *tile = NULL;
    struct list *tileList = s->tiles;

    list_Clear(&s->units);

    while(tileList != NULL)
    {
        tile = tileList->data;

        list_Delete_NodeFromData(&tile->inSectors, s);
        depWatcher_Remove(&tile->depWatcher, &s->tiles);
        depWatcher_Remove(&tile->depWatcher, &s->collisionTiles);

        tileList = tileList->next;
    }

    list_Clear(&s->tiles);

    list_Clear(&s->collisionTiles);

    return;
}
