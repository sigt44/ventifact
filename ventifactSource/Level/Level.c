#include "Level.h"

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Common/List.h"
#include "Graphics/Sprite.h"
#include "Graphics/Surface.h"
#include "Graphics/SurfaceManager.h"
#include "Graphs/Search/DijkstraSearch.h"
#include "Maths.h"

#include "LevelFile.h"
#include "Tiles.h"
#include "../Units/Supply.h"
#include "Sector.h"

/*
    Function: vLevelSurface_Create

    Description -
    Function to create a structure that holds a surface and the path it was loaded from.
    Returns the struct, of type Vent_LevelSurface.

    3 arguments:
    char *fullPath - the path name that the surface was loaded from
    SDL_Surface *surface - pointer to the surface
    int ID - ID of the surface
*/
Vent_LevelSurface *vLevelSurface_Create(char *fullPath, SDL_Surface *surface, int ID)
{
    Vent_LevelSurface *lvlSurface = (Vent_LevelSurface *)mem_Malloc(sizeof(Vent_LevelSurface),__LINE__, __FILE__);

    if(fullPath != NULL)
        strncpy(lvlSurface->name, fullPath, 254);
    else
    {
        lvlSurface->name[0] = '-';
        lvlSurface->name[1] = '\0';
    }

    lvlSurface->surfaceAngleList = NULL;
    lvlSurface->numRotated = 0;

    lvlSurface->surface = surface;

    lvlSurface->ID = ID;

    return lvlSurface;
}

/*
    Function: vLevel_GetCustomSurfaces

    Description -
    A function to return the number of custom surfaces in the level

    1 argument:
    Vent_Level *l - the level to count the custom surfaces from
*/
int vLevel_GetCustomSurfaces(Vent_Level *l)
{
    struct list *surfaceList = l->surfacesLoaded;
    int customSurfaces = 0;

    while(surfaceList != NULL)
    {
        if(surfaceList->info < 0)
            customSurfaces ++;

        surfaceList = surfaceList->next;
    }

    return customSurfaces;
}

/*
    Function: vLevel_SurfaceRetrieve

    Description -
    This function finds a surface in a list, using a predefined ID number.
    It returns the surface it finds.

    3 arguments:
    struct list *s - the list of SDL surfaces
    int type - the ID of the surface
    int rotationDeg - the rotation of the surface in degrees.
*/
SDL_Surface *vLevel_SurfaceRetrieve(struct list *s, int type, int rotationDeg)
{
    Vent_LevelSurface *lvlSurface = NULL;
    SDL_Surface *returnSurface = NULL;

    struct list *surfaceList = s;

    while(s != NULL)
    {
        if(s->info == type)
        {
            lvlSurface = s->data;

            if(rotationDeg == 0)
            {
                /*Obtain the original surface since there is no rotation*/
                returnSurface = lvlSurface->surface;
            }
            else
            {
                /*See if the rotated surface has already been preloaded*/
                returnSurface = vLevel_SurfaceRetrieveRotation(lvlSurface, rotationDeg);

                if(returnSurface == NULL)
                {
                    /*Create the preloaded version of the surface*/
                    vLevel_SurfaceInsertRotation(surfaceList, type, rotationDeg);

                    /*Obtain that new surface*/
                    returnSurface = vLevel_SurfaceRetrieveRotation(lvlSurface, rotationDeg);
                }
            }

            return returnSurface;
        }

        s = s->next;
    }

    //printf("Unable to find level surface of type %d\n", type);

    return NULL;
}

SDL_Surface *vLevel_SurfaceRetrieveRotation(Vent_LevelSurface *lvlSurface, int rotationDeg)
{
    struct list *rotationSurfaces = lvlSurface->surfaceAngleList;

    rotationDeg = mth_FitAngleBoundry_Deg(rotationDeg);

    if(rotationDeg == 0)
        return lvlSurface->surface;

    while(rotationSurfaces != NULL)
    {
        if(rotationSurfaces->info == rotationDeg)
            return rotationSurfaces->data;

        rotationSurfaces = rotationSurfaces->next;
    }

    //printf("Unable to find rotated(%d) surface of graphic ID %d\n", rotationDeg, lvlSurface->ID);

    return NULL;
}

/*
    Function: vLevel_SurfaceNewID

    Description -
    This function returns a new valid ID to use for a custom surface.

    1 argument:
    struct list *s - the list of SDL surfaces in the level already created.
*/
int vLevel_SurfaceNewID(struct list *s)
{
    int newID = 0;

    while(s != NULL)
    {
        /*Obtain the lowest ID found so far*/
        if(newID > s->info)
        {
            newID = s->info;
        }

        s = s->next;
    }

    /*Make the newest ID the lowest of them all*/
    newID --;

    return newID;
}

/*
    Function: vLevel_SurfaceRemove

    Description -
    This function finds a surface in a list, using the graphics ID.
    It removes the list node that contains the surface and frees the surface.

    2 arguments:
    struct list **surfaceList - pointer to the list of SDL surfaces
    int graphicType - the ID of the surface
*/
int vLevel_SurfaceRemove(struct list **surfaceList, int graphicType)
{
    Vent_LevelSurface *lvlSurface = NULL;
    struct list *s = *surfaceList;

    while(s != NULL)
    {
        if(s->info == graphicType)
        {
            lvlSurface = s->data;

            vLevel_SurfaceClean(lvlSurface);

            mem_Free(lvlSurface);

            list_Delete_Node(surfaceList, s);

            return 1;
        }

        s = s->next;
    }

    printf("Unable to find and delete surface with ID %d\n", graphicType);

    return 0;
}

/*
    Function: vLevel_SurfaceDisable

    Description -
    This function finds a surface in a list, using the graphics ID.
    It disables the surface by releasing the graphic if its custom, then
    settings the graphic to the special 'unknown' surface.

    2 arguments:
    struct list **surfaceList - pointer to the list of SDL surfaces
    int graphicType - the ID of the surface
*/
int vLevel_SurfaceDisable(struct list **surfaceList, int graphicType)
{
    Vent_LevelSurface *lvlSurface = NULL;
    struct list *s = *surfaceList;

    while(s != NULL)
    {
        if(s->info == graphicType)
        {
            lvlSurface = s->data;

            vLevel_SurfaceClean(lvlSurface);

            lvlSurface->surface = ve_Surfaces.unknown;

            return 1;
        }

        s = s->next;
    }

    printf("Warning: Unable to find and disable surface with ID %d\n", graphicType);

    return 0;
}

/*
    Function: vLevel_SurfaceInsert

    Description -
    Function used to load a surface from file and then insert it into a linked list.
    User must also specify an ID for the surface in the list.

    3 arguments:
    struct list **s - the head of the linked list that the surface is to be inserted
    char *fullPath - the path of the surface file, should not exceed 254 characters
    int graphicType - the ID to give the surface
*/
void vLevel_SurfaceInsert(struct list **s, char *fullPath, int graphicType)
{
    char pBuffer[255];

    SDL_Surface *newSurface = NULL;

    strncpy(pBuffer, fullPath, 254);

    if(strcmp(fullPath, "None") != 0)
        surf_Load(&newSurface, pBuffer, M_FREE);

    /*if unable to load the custom surface from a file, use the error surface*/
    if(newSurface == NULL)
        newSurface = ve_Surfaces.unknown;

    if(vLevel_SurfaceRetrieve(*s, graphicType, 0) != NULL)
    {
        file_Log(ker_Log(), 1, "Replacing surface with ID %d\n", graphicType);
        vLevel_SurfaceRemove(s, graphicType);
    }

    list_Push_Sort(s, vLevelSurface_Create(fullPath, newSurface, graphicType), graphicType);

    return;
}

void vLevel_SurfaceInsertRotation(struct list *s, int graphicID, int rotationDeg)
{
    Vent_LevelSurface *lvlSurface = NULL;

    /*Make sure rotation is within boundaries, 0 to 360 degrees*/
    rotationDeg = mth_FitAngleBoundry_Deg(rotationDeg);
    //printf("Rotation deg = %d\n", rotationDeg);

    while(s != NULL)
    {
        if(s->info == graphicID)
        {
            lvlSurface = s->data;

            if(vLevel_SurfaceRetrieveRotation(lvlSurface, rotationDeg) != NULL)
            {
                printf("Warning: Rotated surface (%d) already found in graphic ID of %d\n", rotationDeg, graphicID);
                return;
            }

            lvlSurface->numRotated ++;

            list_Push_Sort(&lvlSurface->surfaceAngleList, surface_Rotate(lvlSurface->surface, PI * (rotationDeg/180.0f), 0), rotationDeg);

            return;
        }

        s = s->next;
    }

    printf("Warning: Could not find graphic ID %d to place rotation in\n", graphicID);

    return;
}


/*
    Function: vLevel_SurfaceClean

    Description -
    Free's the surface if it is a custom ID.

    1 argument:
    Vent_LevelSurface *lvlSurface - The level surface to clean.
*/
void vLevel_SurfaceClean(Vent_LevelSurface *lvlSurface)
{
    if(lvlSurface->ID < 0 && lvlSurface->surface != ve_Surfaces.unknown)
    {
        SDL_FreeSurface(lvlSurface->surface);
    }

    /*Remove the rotated surfaces*/
    while(lvlSurface->surfaceAngleList != NULL)
    {
        SDL_FreeSurface(lvlSurface->surfaceAngleList->data);

        list_Pop(&lvlSurface->surfaceAngleList);
    }

    strcpy(lvlSurface->name, "None");

    return;
}

/*
    Function: vLevel_AddTile

    Description -
    Function to add a tile to the level. If no sprite is already set to the tile
    the function will create one for it.

    2 arguments:
    Vent_Level *l - the level the tile is added to
    Vent_Tile *tile - The tile to add to the level.
*/
void *vLevel_AddTile(Vent_Level *l, void *tile)
{
    Vent_Tile *t = tile;

    /*create a new sprite if the current one is NULL*/
    if(t->sImage == NULL)
    {
        Sprite *sNew = vTile_CreateSprite(l->surfacesLoaded, t->base.layer, NULL, t->base.graphicType, t->base.rotationDeg);

        vTile_ChangeGraphic(t, sNew);
    }

    list_Stack(&l->tiles, t, 0);

    /*update the number of tile count*/
    l->header.numTiles ++;

    return t;
}

/*
    Function: vLevel_RemoveTile

    Description -
    Function to remove a tile from a level.

    2 arguments:
    Vent_Level *l - the level the tile is to be removed from
    void *t - the tile to remove from the level, should be a Vent_Tile structure
*/
void vLevel_RemoveTile(Vent_Level *l, void *t)
{
    /*search for the tile in the levels tile list*/
    struct list *node = list_MatchCheck(l->tiles, t);

    if(node != NULL) /*if the tile has been found*/
    {
        l->header.numTiles --; /*update tile count*/

        list_Delete_Node(&l->tiles, node);
    }

    return;
}

/*
    Function: vLevel_AddSupply

    Description -
    Function to add a unit supply to the level.

    2 arguments:
    Vent_Level *l - the level to add the supply to
    void *s - a pointer to the structure of the supply, should be type Vent_Supply
*/
void vLevel_AddSupply(Vent_Level *l, void *s)
{
   list_Push(&l->unitSupply, s, 0);

    /*update total supplies in the level*/
   l->header.numSupply ++;

   return;
}

/*
    Function: vLevel_RemoveSupply

    Description -
    Removes a supply from a level.

    2 arguments:
    Vent_Level *l - the level to remove the supply from
    void *s - a pointer to the supply to remove, should be type Vent_Supply
*/
void vLevel_RemoveSupply(Vent_Level *l, void *s)
{
    /*find the supply in a list*/
    struct list *node = list_MatchCheck(l->unitSupply, s);

    if(node != NULL)
    {
        l->header.numSupply --;

        list_Delete_Node(&l->unitSupply, node);
    }

    return;
}

/*
    Function: vLevel_SetupHeader

    Description -
    Function used to setup a level header to its default properties.

    4 arguments:
    Vent_Level_Header *header - The header to setup.
    char *name - the name of the level, must be 20 characters or less
    int width - the width of the level in pixels
    int height - the height of the level in pixels
*/
void vLevel_SetupHeader(Vent_Level_Header *header, char *name, int width, int height)
{
    int x = 0;

    if(name != NULL)
        strncpy(header->name, name, 20);
    else
        strcpy(header->name, "NULL");

    header->width = width;
    header->height = height;

    header->numSides = 2;
    for(x = 0; x < header->numSides; x++)
    {
        header->startLoc[x].x = 0;
        header->startLoc[x].y = 0;
    }

    header->numTiles = 0;
    header->numSupply = 0;
    header->numCustomSurfaces = 0;

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        header->enemyUnitLevel[x] = 0;
    }

    header->forcedPlayerUnit = VLEVEL_FORCE_ANY;

    header->baseTileID = TILEG_GRASS;

   return;
}


/*
    Function: vLevel_Setup

    Description -
    Function used to setup a level to its default properties.

    4 arguments:
    Vent_Level *l - the level to setup
    char *name - the name of the level, must be 20 characters or less
    int width - the width of the level in pixels
    int height - the height of the level in pixels
*/
void vLevel_Setup(Vent_Level *l, char *name, int width, int height)
{
    vLevel_SetupHeader(&l->header, name, width, height);

    l->ground = NULL;

    l->tiles = NULL;
    l->importantTiles = NULL;
    l->destroyableTiles = NULL;
    l->sectors = NULL;
    l->unitSupply = NULL;
    l->surfacesLoaded = NULL;

    graph_Setup(&l->pathGraph);

    Dijkstra_Setup(&l->pathSearch);

    /*insert surfaces to the level, giving them an ID so they can be easily associated with tiles*/
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.unknown, TILEG_NULL), TILEG_NULL);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileBuilding, TILEG_BUILDINGBASE), TILEG_BUILDINGBASE);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileCitadel, TILEG_CITADELBASE), TILEG_CITADELBASE);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileWater, TILEG_WATER), TILEG_WATER);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileWall, TILEG_WALL_H), TILEG_WALL_H);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileRoad[0], TILEG_ROAD_H), TILEG_ROAD_H);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileRoad[1], TILEG_ROAD_CUL), TILEG_ROAD_CUL);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileGrass, TILEG_GRASS), TILEG_GRASS);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileSnow, TILEG_SNOW), TILEG_SNOW);
    list_Push(&l->surfacesLoaded, vLevelSurface_Create(NULL, ve_Surfaces.tileSand, TILEG_SAND), TILEG_SAND);

    l->srcTime = NULL;

    return;
}

void vLevel_SetupSectors(Vent_Level *l)
{
    int divX, divY;
    int width;
    int height;

    /*if(l->header.width > 1000)
        divX = 10;
    else
        divX = 4;

    if(l->header.height > 1000)
        divY = 10;
    else
        divY = 4;

    width = l->header.width/divX;
    height = l->header.height/divY;*/
    width = 200;
    height = 200;

    l->sectors = vSector_Tile(width, height, l->header.width, l->header.height);

    vSector_Map(l->sectors);

    vSector_MapTiles(l->sectors, l->tiles);

    return;
}

void vLevel_SetupGround(Vent_Level *l, SDL_Surface *ground)
{
    int x, y;
    SDL_Surface *screen = ker_Screen();

    float xAmount = (float)ker_Screen_Width() / ground->w;
    float yAmount = (float)ker_Screen_Height() / ground->h;

    if(xAmount - (int)xAmount == 0.0f)
        x = (int)xAmount;
    else
        x = (int)xAmount + 1;

    if(yAmount - (int)yAmount == 0.0f)
        y = (int)yAmount;
    else
        y = (int)yAmount + 1;

    l->ground = SDL_CreateRGBSurface( ker_Video_Flags(), x * ground->w, y * ground->h, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask );

    for(x = 0; x < l->ground->w; x = x + ground->w)
    {
        for(y = 0; y < l->ground->h; y = y + ground->h)
            surf_Blit(x, y, ground, l->ground, NULL);
    }

    return;
}

void vLevel_DrawGround(Vent_Level *l, Camera_2D *c, SDL_Surface *dest)
{
    SDL_Rect clip;
    SDL_Rect clipSecond;

    int overY;
    int overX;
    int posX = 0;
    int posY = 0;

    if(l->ground != NULL)
    {
        if(c != NULL)
        {
            clip.x = c->iPosition.x % l->ground->w;
            clip.y = c->iPosition.y % l->ground->h;
            clip.w = c->width;
            clip.h = c->height;

            surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_GROUND, 0, 0, 0, l->ground, dest, &clip);

            overX = (clip.x + c->width) - l->ground->w;
            overY = (clip.y + c->height) - l->ground->h;

            if(overY > 0)
            {
                clipSecond.y = 0;
                clipSecond.x = clip.x;
                clipSecond.w = clip.w;
                clipSecond.h = overY;

                posY = c->height - clipSecond.h;
                surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_GROUND, 0, 0, posY, l->ground, dest, &clipSecond);
            }

            if(overX > 0)
            {
                clipSecond.y = clip.y;
                clipSecond.x = 0;
                clipSecond.w = overX;
                clipSecond.h = clip.h;

                posX = c->width - clipSecond.w;
                surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_GROUND, 0, posX, 0, l->ground, dest, &clipSecond);

                if(overY > 0)
                {
                    clipSecond.y = 0;
                    clipSecond.h = overY;

                    surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_GROUND, 0, posX, posY, l->ground, dest, &clipSecond);
                }
            }

        }
        else
            surfaceMan_Push(gKer_DrawManager(), M_FREE, VL_GROUND, 0, 0, 0, l->ground, dest, NULL);
    }

    return;
}

void vLevel_Draw(Vent_Level *l, Vent_Sector *sectorOrigin, Camera_2D *c, SDL_Surface *dest)
{
    struct list *i = NULL;
    Vent_Sector *drawSector = NULL;
    Vent_Sector *adjSector = NULL;
    Vent_Tile *tile = NULL;

    /*Draw the ground tiles*/
    vLevel_DrawGround(l, c, dest);

    /*Draw all the level sectors in range of the camera*/
    /*Find the sector in the lower right of the camera and start from there*/
    if(sectorOrigin != NULL)
        drawSector = vSector_Find(sectorOrigin, c->iPosition.x + c->width, c->iPosition.y + c->height);

    if(drawSector != NULL)
    {
        /*Increase the sector draw area by 1  to deal with units that overlap a sector, thus
        not getting drawn*/
        if(drawSector->adjacent[D_DOWNRIGHT] != NULL)
        {
            drawSector = drawSector->adjacent[D_DOWNRIGHT];
        }
        else if(drawSector->adjacent[D_DOWN] != NULL)
        {
            drawSector = drawSector->adjacent[D_DOWN];
        }
    }
    else
    {
        if(sectorOrigin != NULL)
        {
            printf("Unable to find sector to draw [%d %d] [%d %d]\n", sectorOrigin->position.x, sectorOrigin->position.y, c->iPosition.x + c->width, c->iPosition.y + c->height);
        }
    }

    while(drawSector != NULL)
    {
        /*Draw the sectors starting from the lower right of the camera*/

        /*Draw the rest of the sectors that are visible to the players camera*/
        adjSector = drawSector;
        while(adjSector != NULL)
        {
            /*Keep going to the sector above until it is outside range of the camera*/
            /*Make sure to draw the first sector that is out of range incase of unit overlap*/
            vSector_Draw(adjSector, c, ker_Screen());

            if(adjSector != drawSector && camera2D_Collision(c, adjSector->position.x-1, adjSector->position.y-1, adjSector->width+1, adjSector->height+1) == 0)
            {
                break;
            }
            else
            {
                /*Change to the sector above*/
                adjSector = adjSector->adjacent[D_UP];
            }
        }

        /*Change to the sector towards the left*/
        drawSector = drawSector->adjacent[D_LEFT];
    }

    i = l->tiles;

    /*Make sure all tiles are only drawn once*/
    while(i != NULL)
    {
        tile = i->data;

        if(tile->drawCalled > 0)
        {
            vTile_Draw(i->data, c, ker_Screen());
        }

        vTile_Reset(i->data);
        i = i->next;
    }

    /*graph_Draw(&l->pathGraph, -c->iPosition.x, -c->iPosition.y, dest);*/

    return;
}

void vLevel_Update(Vent_Level *l, struct list *units)
{


    return;
}

void vLevel_ClearSectors(Vent_Level *l)
{
    while(l->sectors != NULL)
    {
        vSector_Clean(l->sectors->data);
        mem_Free(l->sectors->data);

        list_Pop(&l->sectors);
    }

    return;
}

void vLevel_CleanGround(Vent_Level *l)
{
    if(l->ground != NULL)
        SDL_FreeSurface(l->ground);

    l->ground = NULL;

    return;
}

void vLevel_Clean(Vent_Level *l)
{
    Vent_LevelSurface *lvlSurface = NULL;

    vLevel_CleanGround(l);

    while(l->tiles != NULL)
    {
        vTile_Clean(l->tiles->data);
        mem_Free(l->tiles->data);

        list_Pop(&l->tiles);
    }

    vLevel_ClearSectors(l);

    while(l->unitSupply != NULL)
    {
        vSupply_Clean(l->unitSupply->data);
        mem_Free(l->unitSupply->data);

        list_Pop(&l->unitSupply);
    }

    while(l->surfacesLoaded != NULL)
    {
        lvlSurface = l->surfacesLoaded->data;

        vLevel_SurfaceClean(lvlSurface);

        mem_Free(lvlSurface);

        list_Pop(&l->surfacesLoaded);
    }

    Dijkstra_Clean(&l->pathSearch);

    graph_Clean(&l->pathGraph);

    return;
}

int vLevel_NumTileType(Vent_Level *l, int tileType)
{
    struct list *tiles = l->tiles;
    Vent_Tile *t = NULL;

    int numTile = 0;

    while(tiles != NULL)
    {
        t = tiles->data;

        if(t->base.type == tileType)
            numTile ++;

        tiles = tiles->next;
    }

    return numTile;
}

int vLevel_ChangeTileGraphic(Vent_Level *l, int oldGraphicType, int newGraphicType)
{
    struct list *tileList = l->tiles;
    Vent_Tile *tile = NULL;

    int tilesChanged = 0;

    while(tileList != NULL)
    {
        tile = tileList->data;

        if(tile->base.graphicType == oldGraphicType)
        {
            vTile_ChangeGraphicSurface(tile, l->surfacesLoaded, newGraphicType);

            tilesChanged ++;
        }

        tileList = tileList->next;
    }

    return tilesChanged;
}

int vLevel_CheckTileGraphic(Vent_Level *l, int graphicType)
{
    struct list *tileList = l->tiles;
    Vent_Tile *tile = NULL;

    int tilesChecked = 0;

    while(tileList != NULL)
    {
        tile = tileList->data;

        if(tile->base.graphicType == graphicType)
        {
            tilesChecked ++;
        }

        tileList = tileList->next;
    }

    return tilesChecked;
}
