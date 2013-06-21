#include "LevelFile.h"

#include "Graphics/Surface.h"

#include "Level.h"
#include "Tiles.h"
#include "../Units/Supply.h"

/*The object save types*/
const char *levelTags[NUM_TAGS] = {"[HEADER]",
                                    "[TILE]",
                                    "[SUPPLY]",
                                    "[NODE]",
                                    "[EDGE]",
                                    "[END]",
                                    "[CUSTOMSURFACE]"};

const int levelTagVersions[NUM_TAGS] = {0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0};

enum
{
    TAG_HEADER = 0,
    TAG_TILE = 1,
    TAG_SUPPLY = 2,
    TAG_NODE = 3,
    TAG_EDGE = 4,
    TAG_END = 5,
    TAG_CUSTOMSURFACE = 6,
    TAG_ENDTYPE = 7

} LEVEL_TAG_TYPES;

/*Function pointers to different versions of loading in level objects*/
#define HEADER_VERSIONS 1
#define TILE_VERSIONS 1
#define SUPPLY_VERSIONS 1
#define NODE_VERSIONS 1
#define EDGE_VERSIONS 1
#define CUSTOMSURFACE_VERSIONS 1

void (*vLevel_LoadHeaderVer[HEADER_VERSIONS])(Vent_Level_Header *header, Level_Tag *tag, FILE *levelFile) =
    {
        &vLevel_LoadHeader_0
    };

void (*vLevel_LoadTileVer[TILE_VERSIONS])(Vent_Level *l, Level_Tag *tag, FILE *levelFile) =
    {
        &vLevel_LoadTile_0
    };

void (*vLevel_LoadSupplyVer[SUPPLY_VERSIONS])(Vent_Level *l, void *game, Level_Tag *tag, FILE *levelFile) =
    {
        &vLevel_LoadSupply_0
    };

void (*vLevel_LoadNodeVer[NODE_VERSIONS])(Vent_Level *l, Level_Tag *tag, FILE *levelFile) =
    {
        &vLevel_LoadNode_0
    };

void (*vLevel_LoadEdgeVer[EDGE_VERSIONS])(Vent_Level *l, Level_Tag *tag, FILE *levelFile) =
    {
        &vLevel_LoadEdge_0
    };

void (*vLevel_LoadCustomSurfaceVer[CUSTOMSURFACE_VERSIONS])(Vent_Level *l, Level_Tag *tag, FILE *levelFile) =
    {
        &vLevel_LoadCustomSurface_0
    };

/*
    Function: vLevel_SaveTag

    Description -
    Function that saves the tag that defines which type, version and amount of object is being saved below it.

    3 arguments:
    FILE *save - The save file that has been opened and can be written to.
    int amount - The amount of objects that are going to be saved below it.
    const int tagType - The type of object that is being saved.
*/
static void vLevel_SaveTag(FILE *save, int amount, const int tagType)
{
    file_Log(save, 0, "%s %d %d\n", levelTags[tagType], levelTagVersions[tagType], amount);

    return;
}

/*
    Function: vLevel_SaveHeader

    Description -
    Function that saves the level header to a file.

    2 arguments:
    Vent_Level_Header *header - The level header that contains the description.
    FILE *save - The save file that has been opened and can be written to.
*/
static void vLevel_SaveHeader(Vent_Level_Header *header, FILE *save)
{
    int x = 0;

    vLevel_SaveTag(save, 1, TAG_HEADER);

    /*Save, name, level width, height, number of sides, custom surfaces, forced unit type*/
    file_Log(save, 0, "%s %d %d %d %d %d\n",
    header->name,
    header->width,
    header->height,
    header->numSides,
    header->numCustomSurfaces,
    header->forcedPlayerUnit
    );

    /*Save the starting locations*/
    if(header->numSides > 0)
    {
        for(x = 0; x < header->numSides; x++)
            file_Log(save, 0, "%d %d ", header->startLoc[x].x, header->startLoc[x].y);

        file_Log(save, 0, "\n");
    }

    /*Save the enemy unit levels*/
    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        file_Log(save, 0, "%d ", header->enemyUnitLevel[x]);
    }

    file_Log(save, 0, "\n");

    /*Save the ground tile texture ID*/
    file_Log(save, 0, "%d\n", header->baseTileID);

    return;
}

/*
    Function: vLevel_SaveCustomSurface

    Description -
    Function that saves all of the locations of custom surfaces used by the level.

    2 arguments:
    Vent_Level *l - The level that contains the custom surfaces.
    FILE *save - The save file that has been opened and can be written to.
*/
static void vLevel_SaveCustomSurface(Vent_Level *l, FILE *save)
{
    int x = 0;
    struct list *i = NULL;
    struct list *rotated = NULL;
    Vent_LevelSurface *lvlSurface = NULL;

    l->header.numCustomSurfaces = vLevel_GetCustomSurfaces(l);

    vLevel_SaveTag(save, l->header.numCustomSurfaces, TAG_CUSTOMSURFACE);


    /*Save all the names of the custom surfaces to load and their ID*/
    i = l->surfacesLoaded;

    while(i != NULL)
    {
        lvlSurface = i->data;

        /*If this surface is a custom one*/
        if(i->info < 0)
        {
            rotated = lvlSurface->surfaceAngleList;

            file_Log(save, 0, "%s %d %d", lvlSurface->name, lvlSurface->ID, lvlSurface->numRotated);

            /*Save the angles to be preloaded of the rotated version of this surface*/
            if(lvlSurface->numRotated > 0)
            {
                while(rotated != NULL)
                {
                    file_Log(save, 0, " %d", rotated->info);
                    rotated = rotated->next;
                }
            }

            file_Log(save, 0, "\n");

            x++;
        }

        i = i->next;

        if(x > l->header.numCustomSurfaces)
        {
            printf("Error: Amount of custom surfaces does not match (%d vs %d)\n", x, l->header.numCustomSurfaces);
            break;
        }
    }

    return;
}

/*
    Function: vLevel_SaveTile

    Description -
    Function that saves all of the tiles in the level to a file.

    2 arguments:
    Vent_Level *l - The level that contains the tiles.
    FILE *save - The save file that has been opened and can be written to.
*/
static void vLevel_SaveTile(Vent_Level *l, FILE *save)
{
    struct list *i = l->tiles;

    vLevel_SaveTag(save, l->header.numTiles, TAG_TILE);

    while(i != NULL)
    {
        vTile_Save(i->data, l->surfacesLoaded, save);
        i = i->next;
    }

    return;
}

/*
    Function: vLevel_SaveEdge

    Description -
    Function that saves all of the supplies in the level to a file.

    2 arguments:
    Vent_Level *l - The level that contains the supplies.
    FILE *save - The save file that has been opened and can be written to.
*/
static void vLevel_SaveSupply(Vent_Level *l, FILE *save)
{
    struct list *i = l->unitSupply;

    vLevel_SaveTag(save, l->header.numSupply, TAG_SUPPLY);

    while(i != NULL)
    {
        vSupply_Save(i->data, save);
        i = i->next;
    }

    return;
}

/*
    Function: vLevel_SaveNode

    Description -
    Function that saves all of the nodes in the level to a file.

    2 arguments:
    Vent_Level *l - The level that contains the nodes.
    FILE *save - The save file that has been opened and can be written to.
*/
static void vLevel_SaveNode(Vent_Level *l, FILE *save)
{
    struct list *i = l->pathGraph.nodes;
    B_Node *node = NULL;

    vLevel_SaveTag(save, l->pathGraph.numNodes, TAG_NODE);

    while(i != NULL)
    {
        node = i->data;

        /*Save the node, index, x position, y position*/
        file_Log(save, 0, "%d %d %d\n", node->index, node->x, node->y);

        i = i->next;
    }

    return;
}

/*
    Function: vLevel_SaveEdge

    Description -
    Function that saves all of the edges in the level to a file.

    2 arguments:
    Vent_Level *l - The level that contains the edges.
    FILE *save - The save file that has been opened and can be written to.
*/
static void vLevel_SaveEdge(Vent_Level *l, FILE *save)
{
    struct list *i = l->pathGraph.edges;
    B_Edge *edge = NULL;

    vLevel_SaveTag(save, l->pathGraph.numEdges, TAG_EDGE);

    while(i != NULL)
    {
        edge = i->data;

        /*Save the edge, start index, end index, weight*/
        file_Log(save, 0, "%d %d %d\n", edge->startNode->index, edge->endNode->index, edge->weight);

        i = i->next;
    }

    return;
}

/*
    Function: vLevel_Save

    Description -
    Function that saves a level in the /Level/ folder.

    2 arguments:
    Vent_Level *l - the level to save
    char *name - the name to save the level as, .map will be appended to this
*/
void vLevel_Save(Vent_Level *l, char *name)
{
    char *saveFolder = "../Levels/";
    char *saveName = (char *)mem_Malloc(strlen(saveFolder) + strlen(name) + strlen(".map") + 1, __LINE__, __FILE__);

    int x = 0;

    FILE *save = NULL;

    strcpy(saveName, saveFolder);
    strcat(saveName, name);
    strcat(saveName, ".map");

    save = fopen(saveName, "w");

    if(save == NULL)
    {
        printf("Error could not save level %s\n", saveName);

        return;
    }

    l->header.numCustomSurfaces = vLevel_GetCustomSurfaces(l);

    /*Structure of the level in a file -
    First write the object tag - [TAGNAME] objectAmount tagVersion
    Then below that write all the object data.
    */
    vLevel_SaveHeader(&l->header, save);

    vLevel_SaveCustomSurface(l, save);

    vLevel_SaveTile(l, save);

    vLevel_SaveSupply(l, save);

    vLevel_SaveNode(l, save);

    vLevel_SaveEdge(l, save);

    vLevel_SaveTag(save, 0, TAG_END);

    fclose(save);

    mem_Free(saveName);

    return;
}

static int vLevel_TagVerify(Level_Tag *tag, FILE *levelFile)
{
    int x = 0;

    /*Load in the tag name*/
    fscanf(levelFile, "%s", tag->name);

    /*Find the tag type from the name*/
    tag->type = -1;

    for(x = 0; x < TAG_ENDTYPE; x++)
    {
        if(strcmp(tag->name, levelTags[x]) == 0)
        {
            tag->type = x;
        }
    }

    if(tag->type == -1)
    {
        printf("Error: unable to find tag name %s\n", tag->name);

        return 0;
    }

    /*Load in the amount of objects below the tag and the tag version*/
    fscanf(levelFile, "%d%d", &tag->version, &tag->amount);

    if(tag->version < 0)
    {
        printf("Error: tag version < 0 (%d)\n", tag->version);

        return 0;
    }

    return 1;
}

void vLevel_LoadHeader_0(Vent_Level_Header *header, Level_Tag *tag, FILE *levelFile)
{
    const int version = 0;
    const int numUnits = 4; /*Number of units known at this time*/

    char levelName[20];
    char surfacePathLimit[255];

    int ID = 0;

    int x = 0;

    /*Load in the level name, width, height, number of sides, number of custom surfaces*/
    fscanf(levelFile, "%s%d%d%d%d%d",
        levelName,
        &header->width,
        &header->height,
        &header->numSides,
        &header->numCustomSurfaces,
        &header->forcedPlayerUnit
        );

    /*make sure level name does not exceed limit*/
    strncpy(header->name, levelName, 20);

    /*Load the starting points for the sides*/
    for(x = 0; x < header->numSides; x++)
        fscanf(levelFile, "%d%d", &header->startLoc[x].x, &header->startLoc[x].y);

    /*Load the enemy unit levels*/
    for(x = 0; x < numUnits; x++)
    {
        fscanf(levelFile, "%d", &header->enemyUnitLevel[x]);
    }

    /*Load the ground surface ID*/
    fscanf(levelFile, "%d", &header->baseTileID);

    return;
}

void vLevel_LoadCustomSurface_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile)
{
    const int version = 0;

    char surfacePathLimit[255];

    int ID = 0;

    int numRotated = 0;
    int rotationDeg = 0;

    int x = 0;
    int y = 0;

    /*Load the custom surfaces*/
    for(x = 0; x < tag->amount; x++)
    {
        fscanf(levelFile, "%s%d%d", surfacePathLimit, &ID, &numRotated);

        file_Log(ker_Log(), P_OUT, "Loading custom surface(%d) from: %s\n", ID, surfacePathLimit);

        vLevel_SurfaceInsert(&l->surfacesLoaded, surfacePathLimit, ID);

        if(numRotated > 0)
        {
            //file_Log(ker_Log(), 1, "Creating %d rotations of surface(%d)\n", numRotated, ID);

            for(y = 0; y < numRotated; y++)
            {
                fscanf(levelFile, "%d", &rotationDeg);
                vLevel_SurfaceInsertRotation(l->surfacesLoaded, ID, rotationDeg);

                //file_Log(ker_Log(), 1, "\t%d rotation\n", rotationDeg);
            }
        }

        fscanf(levelFile, "\n");
    }

    return;
}

void vLevel_LoadTile_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile)
{
    const int version = 0;

    int x = 0;

    Vent_Tile *tile = NULL;

    l->header.numTiles = tag->amount;

    for(x = 0; x < l->header.numTiles; x++)
    {
        /*Load in the tile data*/
        tile = vTile_Load(&l->surfacesLoaded, l->srcTime, levelFile);

        /*Place the tile into the level*/
        list_Stack(&l->tiles, tile, 0);

        /*If the tile is important, add it in to another list so it can be tracked more easily.*/
        if(flag_Check(&tile->base.stateFlags, TILESTATE_IMPORTANT) == 1)
        {
            depWatcher_AddBoth(&tile->depWatcher, &l->importantTiles, 0);
        }

        //printf("Adding new tile at %d %d, health %d flags %d to level\n", tile->base.position.x, tile->base.position.y, tile->base.healthStarting, tile->base.stateFlags);

        if(flag_Check(&tile->base.stateFlags, TILESTATE_DESTROYABLE) == 1 && tile->base.healthStarting > 0)
        {
            //printf("Adding new tile at %d %d, health %d to list\n", tile->base.position.x, tile->base.position.y, tile->base.healthStarting);
            depWatcher_AddBoth(&tile->depWatcher, &l->destroyableTiles, 0);
        }

    }

    return;
}

void vLevel_LoadSupply_0(Vent_Level *l, void *game, Level_Tag *tag, FILE *levelFile)
{
    const int version = 0;

    int x = 0;

    l->header.numSupply = tag->amount;

    for(x = 0; x < l->header.numSupply; x++)
    {
        /*Load the supply data and directly place it into the level*/
        list_Push(&l->unitSupply, vSupply_Load(game, l->srcTime, levelFile), 0);
    }

    return;
}

void vLevel_LoadNode_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile)
{
    const int version = 0;

    int x = 0;
    int numNodes = 0;
    int nodeIndex = 0;
    int nodeX = 0;
    int nodeY = 0;

    numNodes = tag->amount;

    for(x = 0; x < numNodes; x++)
    {
        /*Obtain the node data, index, x and y position*/
        fscanf(levelFile, "%d%d%d", &nodeIndex, &nodeX, &nodeY);

        /*Create the node for the level*/
        graph_Add_Node(&l->pathGraph, node_Create(nodeIndex, nodeX, nodeY));
    }

    return;
}

void vLevel_LoadEdge_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile)
{
    const int version = 0;

    int x = 0;
    int numEdges = 0;
    int edgeFrom = 0;
    int edgeTo = 0;
    int edgeWeight = 0;

    numEdges = tag->amount;

    for(x = 0; x < numEdges; x++)
    {
        /*Obtain the edge data, node index from, node index to, the weight between those two indexes*/
        fscanf(levelFile, "%d%d%d", &edgeFrom, &edgeTo, &edgeWeight);

        /*Create the edge into the level graph*/
        graph_Add_Edge_DistanceWeight(&l->pathGraph, edge_Create_Index(l->pathGraph.nodes, edgeFrom, edgeTo, edgeWeight));
    }

    return;
}


/*
    Function: vLevel_Load

    Description -
    Loads a level from a file.

    4 arguments:
    Vent_Level *l - The level structure that the level will be loaded into.
    void *game - The Vent_Game structure.
    Timer *srcTime - The source timer the level will use.
    char *name - The file name of the level to load.
*/
void vLevel_Load(Vent_Level *l, void *game, Timer *srcTime, char *name)
{
    char *loadFolder = "../Levels/";
    char *loadName = (char *)mem_Malloc(strlen(loadFolder) + strlen(name) + 1, __LINE__, __FILE__);

    FILE *load = NULL;
    Level_Tag tag;

    int finishedLoading = 0;

    /*Setup the default values of the level*/
    vLevel_Setup(l, NULL, 0, 0);

    l->srcTime = srcTime;

    /*Find the level file*/
    strcpy(loadName, loadFolder);
    strcat(loadName, name);

    load = fopen(loadName, "r");

    if(load == NULL)
    {
        printf("Error: Could not load level file %s\n", loadName);

        return;
    }

    while(vLevel_TagVerify(&tag, load) == 1 && finishedLoading == 0)
    {
        switch(tag.type)
        {
            case TAG_HEADER:

            if(tag.version < HEADER_VERSIONS)
            {
                vLevel_LoadHeaderVer[tag.version](&l->header, &tag, load);
            }

            break;

            case TAG_CUSTOMSURFACE:

            if(tag.version < CUSTOMSURFACE_VERSIONS)
            {
                vLevel_LoadCustomSurfaceVer[tag.version](l, &tag, load);

            }

            break;

            case TAG_TILE:

            if(tag.version < TILE_VERSIONS)
                vLevel_LoadTileVer[tag.version](l, &tag, load);

            break;

            case TAG_SUPPLY:

            if(tag.version < SUPPLY_VERSIONS)
                vLevel_LoadSupplyVer[tag.version](l, game, &tag, load);

            break;

            case TAG_NODE:

            if(tag.version < NODE_VERSIONS)
                vLevel_LoadNodeVer[tag.version](l, &tag, load);

            break;

            case TAG_EDGE:

            if(tag.version < EDGE_VERSIONS)
                vLevel_LoadEdgeVer[tag.version](l, &tag, load);

            break;

            case TAG_END:
            file_Log(ker_Log(), 1, "Level finished loading: %s (%d %d)\n", l->header.name, l->header.width, l->header.height);
            finishedLoading = 1;
            break;

            default:

            printf("Error unknown tag type %s -> %d\n", tag.name, tag.type);

            break;
        }
    }

    fclose(load);

    mem_Free(loadName);

    return;
}

/*
    Function: vLevel_LoadHeader

    Description -
    Loads a level header from a file.

    2 arguments:
    Vent_Level_Header *header - The level header to be loaded into.
    char *name - The file name of the level to load.
*/
void vLevel_LoadHeader(Vent_Level_Header *header, char *name)
{
    char *loadFolder = "../Levels/";
    char *loadName = (char *)mem_Malloc(strlen(loadFolder) + strlen(name) + 1, __LINE__, __FILE__);

    int finishedLoading = 0;

    FILE *load = NULL;
    Level_Tag tag;

    /*Setup the default values of the header*/
    vLevel_SetupHeader(header, NULL, 0, 0);

    /*Find the level file*/
    strcpy(loadName, loadFolder);
    strcat(loadName, name);

    load = fopen(loadName, "r");

    if(load == NULL)
    {
        file_Log(ker_Log(), 1, "Error: Could not load level file for header. (%s)\n", loadName);

        return;
    }

    while(vLevel_TagVerify(&tag, load) == 1 && finishedLoading == 0)
    {
        switch(tag.type)
        {
            case TAG_HEADER:

            if(tag.version < HEADER_VERSIONS)
            {
                vLevel_LoadHeaderVer[tag.version](header, &tag, load);
                finishedLoading = 1;
            }

            break;

            default:

            file_Log(ker_Log(), 1, "Error unknown tag type (while searching for header) %s -> %d\n", tag.name, tag.type);

            break;
        }
    }

    fclose(load);

    mem_Free(loadName);

    return;
}

/*
    Function: vLevel_LoadOld

    Description -
    This function loads a level (Without full compatibility) using an out of date level structure

    4 arguments:
    Vent_Level *l - the structure to load the level into
    struct list **unitList - the head of a list that is used to store units
    Timer *srcTime - the timer the level will use
    char *name - the name of the level file (without the extension)
*/
void vLevel_LoadOld(Vent_Level *l, void *game, Timer *srcTime, char *name)
{
    char *saveFolder = "../LevelsOld/";
    char *saveName = (char *)mem_Malloc(strlen(saveFolder) + strlen(name) + 1, __LINE__, __FILE__);

    /*Old unit spawn structure varibales*/
    int spawnX = 0;
    int spawnY = 0;
    int spawnAmount = 0;
    int spawnTime = 0;
    int spawnTeam = 0;
    int spawnType = 0;
    int spawnAi = 0;
    int spawnDead = 0;

    /*Old tile variables*/
    int subType = 0;

    int read;
    int numTiles = 0;
    char textBuffer[255];
    char *floorTexture = NULL;

    int ID = 0;
    int important = 0;

    int graphicTypeArray[8] = {
        TILEG_NULL,
        TILEG_CUSTOM,
        TILEG_WALL_H,
        TILEG_DECOR,
        TILEG_CITADELBASE,
        TILEG_BUILDINGBASE,
        TILEG_ROAD_H,
        TILEG_WATER
        };
    char surfacePath[255];

    char *newCustomPath = "../Textures/Level/Custom/";
    char *texturePath = NULL;

    Vent_Tile *t = NULL;
    Sprite *sTile = NULL;

    FILE *load = NULL;
    Data_Struct *aiData = NULL;

    vLevel_Setup(l, NULL, 0, 0);

    strcpy(saveName, saveFolder);
    strcat(saveName, name);
    /*strcat(saveName, ".map");*/


    load = fopen(saveName, "r");

    if(load == NULL)
    {
        printf("Error cannot load old level %s\n", saveName);

        mem_Free(saveName);
        return;
    }

    /*ignore any comments at the top of the file*/
    fscanf(load, "%s", textBuffer);
    if(strcmp(textBuffer,"/") == 0)
    {
        fscanf(load, "%s", textBuffer);
        while(strcmp(textBuffer,"/") != 0)
        {
            fscanf(load, "%s", textBuffer);
        }
    }

    /*load in the name of the level*/
    fscanf(load, "%s", textBuffer);
    if(strcmp(textBuffer,"Level_Name:") != 0)
    {
        printf("Could not read level name marker, %s\n",textBuffer);
        return;
    }

    fscanf(load, "%s", l->header.name);

    /*load the amount of points awarded for winning the level*/
    fscanf(load, "%s",textBuffer);
    if(strcmp(textBuffer,"Win_Points:") != 0)
    {
        printf("Could not read win points marker, %s\n",textBuffer);
        return;
    }
    fscanf(load,"%d",&ID);

    /*load in the location of the graphic to be used as the ground*/
    fscanf(load,"%s", textBuffer);
    if(strcmp(textBuffer,"Base_Dir:") != 0)
    {
        printf("Could not read level base dir marker, %s\n",textBuffer);
        return;
    }
    fscanf(load,"%s", textBuffer);

    /*point floorTexture to the position after the '.' in a file name*/
    floorTexture = string_Target(textBuffer, '.');

    switch(*(floorTexture-5))
    {
        case 'w':
        l->header.baseTileID = TILEG_SNOW;
        break;

        case 'd':
        l->header.baseTileID = TILEG_SAND;
        break;

        default:
        l->header.baseTileID = TILEG_GRASS;
        break;
    }

    /*load in the level size, width then height*/
    fscanf(load,"%s", textBuffer);
    if(strcmp(textBuffer,"Level_Size:") != 0)
    {
        printf("Could not read level size marker, %s\n",textBuffer);
        return;
    }

    fscanf(load,"%d",&l->header.width);

    fscanf(load,"%d",&l->header.height);

    /*load in the position where the enemy unit start*/
    fscanf(load,"%s", textBuffer);
    if(strcmp(textBuffer,"Enemy_Starting_Cords:") != 0)
    {
        printf("Could not read enemy starting name marker, %s\n",textBuffer);
        return;
    }

    fscanf(load,"%d",&read);
    l->header.startLoc[TEAM_1].x = read;
    fscanf(load,"%d",&read);
    l->header.startLoc[TEAM_1].y = read;

    /*load in the position where the player unit start*/
    fscanf(load,"%s", textBuffer);
    if(strcmp(textBuffer,"Player_Starting_Cords:") != 0)
    {
        printf("Could not read player starting name marker, %s\n",textBuffer);
        return;
    }

    fscanf(load,"%d",&read);
    l->header.startLoc[TEAM_PLAYER].x = read;
    fscanf(load,"%d",&read);
    l->header.startLoc[TEAM_PLAYER].y = read;
    l->header.numSides = 2;

    /*start loading in the tiles*/
    fscanf(load,"%s", textBuffer);
    if(strcmp(textBuffer,"Map_Tile_Cords:") != 0)
    {
        printf("Could not read level tile starting name marker, %s\n",textBuffer);
        return;
    }

    while(fscanf(load, "%d", &read) > 0)
    {
        t = (Vent_Tile *)mem_Malloc(sizeof(Vent_Tile), __LINE__, __FILE__);
        sTile = sprite_Create();
        important = 0;

        numTiles ++;

        t->base.position.x = read;

        fscanf(load, "%d%d%d%d%d", &t->base.position.y, &t->base.type, &subType, &t->base.group, &t->base.collision);
        t->base.team = 0;

        if(t->base.type > 7)
        {
            printf("Warning tile type > 7 (%d) setting to 0\n", t->base.type);
            t->base.type = 0;
        }

        t->base.type = graphicTypeArray[t->base.type];
        t->base.stateFlags = TILESTATE_IDLE;
        t->citadel = NULL;
        t->base.rotationDeg = 0;

        //printf("Type = %d\n", t->type);

        switch(t->base.type)
        {
            case TILEG_CUSTOM:

                fscanf(load,"%d",&t->base.graphicType);

                if(t->base.graphicType > 0)
                    t->base.graphicType = -t->base.graphicType;

                fscanf(load,"%s", textBuffer);

                texturePath = string_Target(textBuffer, '.');

                while(*(texturePath) != '/')
                {
                    texturePath --;
                }

                texturePath ++;

                strcpy(surfacePath, newCustomPath);
                strcat(surfacePath, texturePath);

                if(vLevel_SurfaceRetrieve(l->surfacesLoaded, t->base.graphicType, 0) == NULL)
                    vLevel_SurfaceInsert(&l->surfacesLoaded, surfacePath, t->base.graphicType);

                t->base.type = TILE_CUSTOM;

            break;

            case TILEG_WALL_H:

                t->base.graphicType = TILEG_WALL_H;

                if(subType == 1)
                {
                    t->base.rotationDeg = 90;
                }

                t->base.type = TILE_DECOR;

            break;

            case TILEG_ROAD_H:

                switch(subType)
                {
                    case 0:
                        t->base.graphicType = TILEG_ROAD_H;
                    break;

                    case 1:
                        t->base.graphicType = TILEG_ROAD_H;
                        t->base.rotationDeg = 90;
                    break;

                    case 2:
                        t->base.graphicType = TILEG_ROAD_CUL;
                        t->base.rotationDeg = 270;
                    break;

                    case 3:
                        t->base.graphicType = TILEG_ROAD_CUL;
                    break;

                    case 4:
                        t->base.graphicType = TILEG_ROAD_CUL;
                        t->base.rotationDeg = 180;
                    break;

                    case 5:
                        t->base.graphicType = TILEG_ROAD_CUL;
                        t->base.rotationDeg = 90;
                    break;
                }

                t->base.type = TILE_DECOR;

            break;

            case TILEG_CITADELBASE:

                t->base.stateFlags = TILESTATE_BUILDABLE;
                t->base.graphicType = TILEG_CITADELBASE;

                t->base.type = TILE_CITADELBASE;

            break;

            case TILEG_BUILDINGBASE:

                t->base.stateFlags = TILESTATE_BUILDABLE;
                t->base.graphicType = TILEG_BUILDINGBASE;

                 t->base.type = TILE_BUILDINGBASE;

            break;

            case TILEG_WATER:

                t->base.graphicType = TILEG_WATER;

                t->base.type = TILE_WATER;
            break;

            default:

                t->base.graphicType = TILEG_NULL;
                t->base.type = TILE_NULL;

            break;
        }

        if(t->base.group != 0 && flag_Check(&t->base.stateFlags, TILESTATE_BUILDABLE) != 1)
        {
            file_Log(ker_Log(), 0, "Warning: Adding tile with bad group %d, correcting...\n", t->base.group);

            t->base.group = 0;
        }

        if(t->base.collision > 0)
            t->base.collision --;

        fscanf(load, "%d%d%d%d", &t->base.healthStarting, &t->base.team, &important, &t->base.layer);

        if(vLevel_SurfaceRetrieve(l->surfacesLoaded, t->base.graphicType, t->base.rotationDeg) == NULL)
        {
            printf("Error cant file required graphic for tile, type (%d) %d, rotation %d\n", t->base.type, t->base.graphicType, t->base.rotationDeg);
        }

        sprite_Setup(sTile, 1, t->base.layer, srcTime, 1, frame_CreateBasic(0, vLevel_SurfaceRetrieve(l->surfacesLoaded, t->base.graphicType, t->base.rotationDeg), M_FREE));

        t->base.width = sprite_Width(sTile);
        t->base.height = sprite_Height(sTile);

        t->sImage = sTile;
        vTile_Init(t);

        list_Stack(&l->tiles, t, 0);

        if(important == 1)
            flag_On(&t->base.stateFlags, TILESTATE_IMPORTANT);

        /*If the tile is important, add it in to another list so it can be tracked more easily.*/
        if(flag_Check(&t->base.stateFlags, TILESTATE_IMPORTANT) == 1)
        {
            depWatcher_AddBoth(&t->depWatcher, &l->importantTiles, 0);
        }

        if(t->base.healthStarting > 0)
        {
            depWatcher_AddBoth(&t->depWatcher, &l->destroyableTiles, 0);
        }

        if(t->base.healthStarting > 0)
        {
            /*Create the health bar 5 pixels above the tile*/
            t->healthBar = (Vent_HealthBar *)mem_Malloc(sizeof(Vent_HealthBar), __LINE__, __FILE__);
            vHealthBar_Setup(t->healthBar, NULL, t->base.position.x + (t->base.width/2), t->base.position.y - 10, 80, 8, 4, colourGreen);
        }
        else
            t->healthBar = NULL;


        l->header.numTiles ++;

        //file_Log(ker_Log(), 1, "(%d)Tile added, t%d g: %d, h:%d w:%d h:%d l:%d\n",numTiles, t->type, t->group, t->health, t->width, t->height, t->layer);
    }

    l->header.numCustomSurfaces = vLevel_GetCustomSurfaces(l);

    /*load in the unit spawns*/
    fscanf(load,"%s", textBuffer);
    if(strcmp(textBuffer,"Ressuply_Info:") != 0)
    {
        printf("Could not read ressuply info marker, %s\n",textBuffer);
         return;
    }


    while(fscanf(load, "%d", &read) > 0)
    {
        spawnX = read;

        fscanf(load, "%d %d %d %d %d %d %d", &spawnY, &spawnTeam, &spawnType, &spawnAmount, &spawnTime, &spawnAi, &spawnDead);

            if(spawnDead == 0)
                spawnDead = 1;
            else
                spawnDead = 0;

            /*Ai
            0 - Wander -> SUPPLY_AI_NONE
            1 - Attack citadel -> SUPPLY_AI_ATTACKBUILDING
            2 - Attack target -> SUPPLY_AI_ATTACKPLAYER
            3 - Attack important building -> SUPPLY_AI_ATTACKBUILDING
            */
            switch(spawnAi)
            {
                default:
                case 0:
                spawnAi = SUPPLY_AI_NONE;
                aiData = NULL;
                break;

                case 1:
                case 3:
                spawnAi = SUPPLY_AI_ATTACKBUILDING;
                aiData = vSupply_AiToData_AttackBuilding();
                break;

                case 2:
                spawnAi = SUPPLY_AI_ATTACKPLAYER;
                aiData = vSupply_AiToData_AttackPlayer();
                break;
            }

            list_Push(&l->unitSupply,
                vSupply_Create(game, srcTime, spawnAmount, spawnTime, 1, spawnDead, spawnTeam, spawnType, spawnX, spawnY, 0, spawnAi, aiData),
                0);
            l->header.numSupply ++;

            //file_Log(ker_Log(), 1, "Setting up a unit ressuply at %d %d, time: %d, amount:%d, dw: %d\n", spawnX, spawnY, spawnTime, spawnAmount, spawnDead);
    }

    fclose(load);

    vLevel_LoadOldGraph(l, l->header.name);

    mem_Free(saveName);

    file_Log(ker_Log(), 1, "Warning: Old level structure loaded - %s\n", l->header.name);

    return;
}

void vLevel_LoadOldGraph(Vent_Level *l, char *name)
{
    B_Graph *graph = &l->pathGraph;

    char *graphFolder = "../LevelsOld/";

    char *graphName = (char *)mem_Malloc(strlen(graphFolder) + strlen(name) + strlen("_Path.path") + 1, __LINE__, __FILE__);

    struct graphNode nodeLoad;
    struct graphEdge edgeLoad;

    int eCheck = 1;
    int totalNodes = 0;

    FILE *openGraph = NULL;

    strcpy(graphName, graphFolder);
    strcat(graphName, name);
    strcat(graphName, "_Path.path");

    openGraph = fopen(graphName,"rb");

    if(openGraph == NULL)
    {
        printf("Could not read a path for level %s -> %s\n", name, graphName);
        return;
    }

    //printf("Loading nodes\n");

    /*Obtain the total amount of nodes in the graph*/
    eCheck = fread(&totalNodes,sizeof(int),1,openGraph);

    /*Get the data for a graphNode structure until there should be no more nodes*/
    eCheck = fread(&nodeLoad, sizeof(struct graphNode), 1, openGraph);
    while(eCheck == 1 && totalNodes > 0)
    {
        /*Place it into the newer node structure*/
        //printf("Adding in node(%d) - %d %d\n", nodeLoad.index, nodeLoad.box.x, nodeLoad.box.y);
        graph_Add_Node(graph, node_Create(nodeLoad.index, nodeLoad.box.x, nodeLoad.box.y));

        eCheck = fread(&nodeLoad,sizeof(struct graphNode),1,openGraph);

        totalNodes--;
    }

    node_Report_List(graph->nodes);

    /*Get the data for graphEdge until there are no more*/
    eCheck = fread(&edgeLoad,sizeof(struct graphEdge),1,openGraph);
    while(eCheck == 1)
    {
        //printf("Adding in edge: %d %d cost %d\n", edgeLoad.from, edgeLoad.to, edgeLoad.cost);
        graph_Add_Edge_DistanceWeight(graph, edge_Create_Index(graph->nodes, edgeLoad.from, edgeLoad.to, edgeLoad.cost));
        //graph->pEdgeStart = create_pEdge(graph->pEdgeStart,get_node(graph->nodeStart,edgeLoad.from),get_node(graph->nodeStart,edgeLoad.to),edgeLoad.cost);
        //printf("Loading edge joins at %d %d and cost is %d\n",edgeLoad.from,edgeLoad.to,edgeLoad.cost);
        eCheck = fread(&edgeLoad,sizeof(struct graphEdge),1,openGraph);
    }

    fclose(openGraph);

    mem_Free(graphName);

    return;
}
