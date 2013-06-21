#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include <SDL/SDL.h>

#include "../Common/List.h"
#include "../Time/Timer.h"

#define DM_NO_TYPE -1

typedef struct draw_Object
{
    struct draw_Type *type; /*Draw_Type of the object*/
    SDL_Surface *destSurface; /*Surface to draw the shape on*/

    int lifeTime; /*Timer for how long the object should be drawn*/

    void *object; /*Points to a structure that contains extra infomation on the object, based on Draw_Type*/

} Draw_Object;

typedef struct draw_Type
{
    int ID;
    void (*drawFunction)(Draw_Object *drawObj); /*Function used to draw the object*/
    void (*cleanFunction)(Draw_Object *drawObj); /*Function used to clean the object*/
    int objSize; /*Memory size of the object structure*/

    int *totalObjectLocations; /*Points to the draw managers count of object locations so this layer can modify it*/

    int totalFree; /*Number of nodes in unusedObjectList*/
    struct list *unusedObjectList; /*List of objects with specific type that are ready to be reused*/

} Draw_Type;

typedef struct draw_Layer
{
    struct list *objectList; /*Points to the list of objects to draw for the layer*/

    int totalNodes; /*The total number of nodes that should be drawn on this layer*/

    int *totalObjectLocations; /*Points to the draw managers count of object locations so this layer can modify it*/

    int layer;

} Draw_Layer;

typedef struct draw_Manager
{
    int topLayer;
    int maxToHold; /*Maximum number of objects the manager should hold*/
    int totalObjectLocations; /*Total number of locations there are currently to place an object*/
    int maxTotalObjectLocations;
    int reachedMax; /*The number of times the maximum amount of objects have been drawn*/

    struct list *layers; /*List where each node contains a draw_Layer structure*/

    struct list *currentLayerList; /*The list that contains the current layer to draw*/
    Draw_Layer *currentLayer; /*The current layer of the objects to be drawn*/

    int numTypes;
    Draw_Type **drawTypes; /*Dynamic array for information on what to do with each type of object*/

    Timer srcTimer; /*Source timer to judge objects lifetime*/

} Draw_Manager;

Draw_Type *drawType_Create(int ID, void (*drawFunction)(Draw_Object *drawObj), void (*cleanFunction)(Draw_Object *drawObj), int objSize, int *totalObjectLocations);
int drawType_ReleaseExcess(Draw_Type *dT, int amount);
void drawType_Clean(Draw_Type *drawType);

Draw_Object *drawObject_Create(Draw_Type *type, int lifeTime, SDL_Surface *destSurface);
void drawObject_Edit(Draw_Object *drawObject, int lifeTime, SDL_Surface *destSurface);
void drawObject_Clean(Draw_Object *drawObject);

Draw_Layer *drawLayer_Create(int layer, int *totalGlobalLocations);
void drawLayer_Clean(Draw_Layer *layer);

struct list *drawLayer_Push(Draw_Layer *dL, Draw_Type *dT);
void drawLayer_Release(Draw_Layer *dL, struct list *node, int remove);

struct list *drawLayer_GetFirstUsed(Draw_Layer *dL);


void drawManager_Setup(Draw_Manager *dM, Timer *srcTimer, int maxToHold);

void drawManager_AddType(Draw_Manager *dM, Draw_Type *dT);
Draw_Type *drawManager_GetType(Draw_Manager *dM, int typeID);

int drawManager_ReleaseExcess(Draw_Manager *dM, int amount);

void *drawManager_Push(Draw_Manager *dM, int layer, int typeID, int lifeTime, SDL_Surface *destSurface);

void drawManager_Draw(Draw_Manager *dM);

void drawManager_Clean(Draw_Manager *dM);
void drawManager_Reset(Draw_Manager *dM);

void drawManager_Report(Draw_Manager *dM);

void drawManager_InitLayer(Draw_Manager *dM);
void drawManager_FindUsedLayer(Draw_Manager *dM);
void drawManager_NextLayer(Draw_Manager *dM);





#endif
