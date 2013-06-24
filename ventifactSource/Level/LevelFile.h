#ifndef LEVELFILE_H
#define LEVELFILE_H

#include "Level.h"

#define NUM_TAGS 7

extern const char *levelTags[NUM_TAGS];

extern const int levelTagVersions[NUM_TAGS];

typedef struct levelTag
{
    char name[16];

    int amount;
    int version;
    int type;

} Level_Tag;

/*Old structures
------------------*/
struct graphNode
{
    int index;
    SDL_Rect box;
    struct graphNode *next;
};

struct graphEdge
{
    int to;
    int from;

    int cost;
    struct graphEdge *next;
};
/*
-----------------------*/

void vLevel_Save(Vent_Level *l, char *directory, char *name);

void vLevel_Load(Vent_Level *l, void *game, Timer *srcTime, char *name);

void vLevel_LoadHeader(Vent_Level_Header *header, char *name);

void vLevel_LoadHeader_0(Vent_Level_Header *header, Level_Tag *tag, FILE *levelFile);

void vLevel_LoadTile_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile);

void vLevel_LoadCustomSurface_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile);

void vLevel_LoadSupply_0(Vent_Level *l, void *game, Level_Tag *tag, FILE *levelFile);

void vLevel_LoadNode_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile);

void vLevel_LoadEdge_0(Vent_Level *l, Level_Tag *tag, FILE *levelFile);

void vLevel_LoadOld(Vent_Level *l, void *game, Timer *srcTime, char *name);
void vLevel_LoadOldGraph(Vent_Level *l, char *name);

#endif
