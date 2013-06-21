#include <stdio.h>

#include "../Memory.h"
#include "DrawManager.h"
#include "../Common/List.h"
#include "../Kernel/Kernel.h"
#include "../File.h"

Draw_Type *drawType_Create(int ID, void (*drawFunction)(Draw_Object *drawObj), void (*cleanFunction)(Draw_Object *drawObj), int objSize, int *totalObjectLocations)
{
    Draw_Type *drawType = (Draw_Type *)mem_Alloc(sizeof(Draw_Type));

    drawType->ID = ID;
    drawType->drawFunction = drawFunction;
    drawType->cleanFunction = cleanFunction;
    drawType->objSize = objSize;

    drawType->totalFree = 0;
    drawType->unusedObjectList = NULL;

    drawType->totalObjectLocations = totalObjectLocations;

    return drawType;
}

/*
    Function: drawType_ReleaseExcess

    Description -
    If there are any unused nodes with a specific type then it will delete them.
    The maximum total layers that will be deleted is specified by 'amount'.
    Returns the amount of nodes that have been deleted.

    2 arguments:
    Draw_Layer *dT - The type of unused nodes to remove.
    int amount - The maximum amount of unused nodes to remove, if < 0 then all will be removed.
*/
int drawType_ReleaseExcess(Draw_Type *dT, int amount)
{
    int x = 0;

    if(amount < 0)
        amount = dT->totalFree;

    if(dT->totalFree > 0)
    {
        for(x = 0; x < amount; x++)
        {
            /*Delete all the unused nodes*/
            drawObject_Clean(dT->unusedObjectList->data);
            mem_Free(dT->unusedObjectList->data);

            list_Pop(&dT->unusedObjectList);

            dT->totalFree --;

            if(dT->totalFree == 0)
            {
                x ++;

                break;
            }
        }

        *dT->totalObjectLocations = *dT->totalObjectLocations - x;

        return x;
    }

    return 0;
}

void drawType_Clean(Draw_Type *drawType)
{
    drawType_ReleaseExcess(drawType, -1);

    return;
}

/*
    Function: drawObject_Create

    Description -
    Creates and returns a new object for the the draw manager.

    4 arguments:
    void (*drawFunction)(void *object) - The draw function of this object.
    int objSize - The size of the objects data. This is to be filled in by a seperate function.
*/
Draw_Object *drawObject_Create(Draw_Type *type, int lifeTime, SDL_Surface *destSurface)
{
    Draw_Object *drawObject = (Draw_Object *)mem_Alloc(sizeof(Draw_Object));

    drawObject->object = (void *)mem_Alloc(type->objSize);

    drawObject->type = type;
    drawObject->lifeTime = lifeTime;
    drawObject->destSurface = destSurface;

    return drawObject;
}

void drawObject_Edit(Draw_Object *drawObject, int lifeTime, SDL_Surface *destSurface)
{
    drawObject->lifeTime = lifeTime;
    drawObject->destSurface = destSurface;

    return;
}


/*
    Function: drawObject_Clean

    Description -
    Cleans up a draw object.

    1 argument:
    Draw_Object *drawObject - The draw object to clean up.
*/
void drawObject_Clean(Draw_Object *drawObject)
{
    mem_Free(drawObject->object);
    drawObject->type = NULL;

    return;
}

/*
    Function: drawLayer_Create

    Description -
    Creates and returns a new layer for the draw manager.

    2 arguments:
    int layer - The 'height' of the layer.
    int *totalObjectLocations - Should point to the draw managers variable that holds the total amount of space that has been created for objects to draw.
*/
Draw_Layer *drawLayer_Create(int layer, int *totalObjectLocations)
{
    Draw_Layer *drawLayer = (Draw_Layer *)mem_Malloc(sizeof(Draw_Layer), __LINE__, __FILE__);

    drawLayer->objectList = NULL;

    drawLayer->totalNodes = 0;

    drawLayer->totalObjectLocations = totalObjectLocations;
    drawLayer->layer = layer;

    return drawLayer;
}

/*
    Function: drawLayer_Clean

    Description -
    Removes and cleans up all objects in the layer.

    1 argument:
    Draw_Layer *layer - The layer to clean up.
*/
void drawLayer_Clean(Draw_Layer *layer)
{
    Draw_Object *drawObj = NULL;

    while(layer->objectList != NULL)
    {
        drawObj = layer->objectList->data;

        if(drawObj->type->cleanFunction != NULL)
        {
            drawObj->type->cleanFunction(drawObj);
        }

        drawObject_Clean(drawObj);
        mem_Free(drawObj);

        *layer->totalObjectLocations = *layer->totalObjectLocations - 1;

        list_Pop(&layer->objectList);
    }

    return;
}

/*
    Function: drawLayer_Push

    Description -
    Returns a list node that is free to contain an object to draw.
    If there is an empty node it will use that otherwise it will create a new one.

    2 arguments:
    Draw_Layer *dL - Should point to the layer that a new object should be inserted to.
    int objSize - The object size that is needed.
*/
struct list *drawLayer_Push(Draw_Layer *dL, Draw_Type *dT)
{
    struct list *listNode = NULL;
    int x = 0;

    /*Search the unused object list*/

    //printf("Layer(INS): %d, Total %d, Used %d, Free %d, Inserting %d\n", dL->layer, dL->totalNodes, dL->totalUsed, dL->totalFree, objSize);
    /*Find a free node with the correct size*/

    /*If there is a node in the list that is free to be used with the correct size*/
    if(dT->totalFree > 0)
    {
        /*Remove the unused node*/
        listNode = dT->unusedObjectList;

        list_Unlink_Node(&dT->unusedObjectList, listNode);

        /*Place it onto the end of the list*/
        list_Stack_Node(&dL->objectList, listNode, listNode->data, dT->objSize);

        dT->totalFree --;
        dL->totalNodes ++;
    }
    else /*Otherwise create a new node in the list*/
    {
        list_Stack(&dL->objectList, NULL, dT->objSize);

        /*Return the last node in the list*/
        if(dL->objectList->previous != NULL)
            listNode = dL->objectList->previous;
        else
            listNode = dL->objectList;

        dL->totalNodes ++;

        *dL->totalObjectLocations = *dL->totalObjectLocations + 1;
    }

    return listNode;
}

/*
    Function: drawLayer_Release

    Description -
    This function either removes or sets a node in the list as inactive.
    Inactive nodes can be reused later to save there being another call to malloc.

    3 arguments:
    Draw_Layer *dL - The layer that the node to remove is located.
    struct list *node - The node that is to be removed/set inactive.
    int remove - If 1 then the node will be deleted, otherwise it can be reused.
*/
void drawLayer_Release(Draw_Layer *dL, struct list *node, int remove)
{
    Draw_Object *drawObj = node->data;

   //printf("Layer(REM): %d, Total %d, Used %d, Free %d\n", dL->layer, dL->totalNodes, dL->totalUsed, dL->totalFree);

    if(remove == 1)
    {
        drawObject_Clean(node->data);
        mem_Free(node->data);
        list_Delete_Node(&dL->objectList, node);

        dL->totalNodes --;

        *dL->totalObjectLocations = *dL->totalObjectLocations - 1;
    }
    else
    {
        /*Remove the node from the list*/
        list_Unlink_Node(&dL->objectList, node);

        /*Place it at the head of the unused list*/
        list_Push_Node(&drawObj->type->unusedObjectList, node, node->data, node->info);

        drawObj->type->totalFree ++;
    }

    return;
}

/*
    Function: drawLayer_GetFirstUsed

    Description -
    Returns the starting point in the list of objects the layer contains, of objects that should be drawn.

    1 argument:
    Draw_Layer *dL - The draw layer the objects are in.
*/
struct list *drawLayer_GetFirstUsed(Draw_Layer *dL)
{
    return dL->objectList;
}

/*
    Function: drawManager_Setup

    Description -
    Sets up the variable values for a draw manager.

    3 arguments:
    Draw_Manager *dM - The draw manager to setup.
    Timer *srcTimer - The timer for the time that objects are drawn.
    int maxToHold - The maximum number of objects that the draw manager can draw.
*/
void drawManager_Setup(Draw_Manager *dM, Timer *srcTimer, int maxToHold)
{
    dM->topLayer = 0;
    dM->maxToHold = maxToHold;
    dM->totalObjectLocations = 0;
    dM->maxTotalObjectLocations = 0;
    dM->reachedMax = 0;

    dM->layers = NULL;

    dM->currentLayer = NULL;
    dM->currentLayerList = NULL;

    dM->srcTimer = timer_Setup(srcTimer, 0, 0, 1);

    dM->numTypes = 0;
    dM->drawTypes = NULL;

    return;
}

void drawManager_AddType(Draw_Manager *dM, Draw_Type *dT)
{
    int x = 0;
    Draw_Type **newArray = NULL;

    dM->numTypes ++;

    /*Create the new array to hold the draw types*/
    newArray = (Draw_Type **)mem_Alloc(dM->numTypes * sizeof(Draw_Type *));

    for(x = 0; x < (dM->numTypes - 1); x++)
    {
        newArray[x] = dM->drawTypes[x];
    }

    newArray[x] = dT;

    /*Release the array in the draw manager and replace it with the new one*/
    if(dM->drawTypes != NULL)
    {
        mem_Free(dM->drawTypes);
    }

    dM->drawTypes = newArray;

    return;
}

Draw_Type *drawManager_GetType(Draw_Manager *dM, int typeID)
{
    int x = 0;

    for(x = 0; x < dM->numTypes; x++)
    {
        if(dM->drawTypes[x]->ID == typeID)
        {
            return dM->drawTypes[x];
        }
    }

    return NULL;
}

/*
    Function: drawManager_ReleaseExcess

    Description -
    Removes any unused nodes of any type up to 'amount'.
    If amount is < 0 then all unused nodes from all layers will be deleted.
    Returns the number of unused nodes deleted.

    2 arguments:
    Draw_Manager *dM - The draw manager to remove unused nodes from.
    int amount - The maximum amount of unused nodes to remove, if < 0 then remove all.
*/
int drawManager_ReleaseExcess(Draw_Manager *dM, int amount)
{
    int x = 0;
    int numReleased = 0;

    for(x = 0; x < dM->numTypes; x++)
    {
        numReleased += drawType_ReleaseExcess(dM->drawTypes[x], amount);

        if(amount > 0 && numReleased >= amount)
        {
            break;
        }
    }

    return numReleased;
}

/*
    Function: drawManager_Push

    Description -
    This function will create space for a new object to draw and then return the memory that
    points to it. The function that calls this will then assign the values of the object to draw.

    4 arguments:
    Draw_Manager *dM - The draw manager that an object is to be created into.
    int layer - The layer the draw object should be on.
    void (*drawFunction)(void *object) - The draw function of the object.
    int sizeofObject - The sizeof(Object structure) to draw.
*/
void *drawManager_Push(Draw_Manager *dM, int layer, int typeID, int lifeTime, SDL_Surface *destSurface)
{
    struct list *layerList = dM->layers;
    struct draw_Layer *drawLayer = NULL;
    Draw_Object *drawObj = NULL;

    Draw_Type *dT = drawManager_GetType(dM, typeID);

    int x = 0;

    struct list *listNode = NULL;

    if(dT == NULL)
    {
        file_Log(ker_Log(), P_OUT, "Error: Invalid draw type of (%d)\n", typeID);
        return NULL;
    }

    if(layer < 0)
    {
        layer = dM->topLayer;
    }

    /*Find the required layer*/
    while(layerList != NULL)
    {
        if(layerList->info == layer)
        {
            drawLayer = layerList->data;
            break;
        }

        layerList = layerList->next;
    }

    /*If the layer required has not been inserted yet*/
    if(drawLayer == NULL)
    {
        /*Create a new one*/
        drawLayer = drawLayer_Create(layer, &dM->totalObjectLocations);

        list_Push_Sort(&dM->layers, drawLayer, layer);
    }

    /*Check if there is enough room to create space for the object*/
    if(dM->totalObjectLocations >= dM->maxToHold && dT->totalFree == 0)
    {
        x = drawManager_ReleaseExcess(dM, 1);

        //printf("Released %d excess nodes\n", x);

        /*The number of objects to draw has already reached its maximum*/
        if(x == 0)
        {
            //printf("Rejected: %d %d %d\n", dM->totalObjectLocations, dM->maxToHold, dT->totalFree);
            dM->reachedMax ++;
            return NULL;
        }
    }

    /*Insert the object to be drawn in the correct layer*/
    listNode = drawLayer_Push(drawLayer, dT);

    /*If the list has not previously contained an object then create the memory for it*/
    if(listNode->data == NULL)
    {
        drawObj = drawObject_Create(dT, lifeTime, destSurface);
        listNode->data = drawObj;
    }
    else
    {
        /*Otherwise use the memory created when the list was originally made*/
        drawObj = listNode->data;
        drawObject_Edit(drawObj, lifeTime, destSurface);
    }

    /*Update the stats*/
    if(dM->totalObjectLocations > dM->maxTotalObjectLocations)
            dM->maxTotalObjectLocations = dM->totalObjectLocations;
    if(layer > dM->topLayer)
        dM->topLayer = layer;

    /*Return the space created for the object to draw*/
    return drawObj->object;
}

/*
    Function: drawManager_InitLayer

    Description -
    Sets the current layer the draw manager is on to be the first layer.

    1 arguments:
    Draw_Manager *dM - The draw manager to set the current layer.
*/
void drawManager_InitLayer(Draw_Manager *dM)
{
    dM->currentLayerList = dM->layers;
    dM->currentLayer = NULL;

    drawManager_FindUsedLayer(dM);

    return;
}

void drawManager_FindUsedLayer(Draw_Manager *dM)
{
    while(dM->currentLayerList != NULL)
    {
        dM->currentLayer = dM->currentLayerList->data;

        /*If this layer has objects to draw*/
        if(dM->currentLayer->totalNodes > 0)
        {
            return;
        }

        dM->currentLayerList = dM->currentLayerList->next;
    }

    dM->currentLayer = NULL;

    return;
}


/*
    Function: drawManager_InitLayer

    Description -
    Sets the current layer of the draw manager to be the next layer in its list that has
    objects in it.

    1 arguments:
    Draw_Manager *dM - The draw manager to set the current layer.
*/
void drawManager_NextLayer(Draw_Manager *dM)
{
    dM->currentLayerList = dM->currentLayerList->next;

    drawManager_FindUsedLayer(dM);

    return;
}

/*
    Function: drawManager_Draw

    Description -
    Draws all the objects in the draw manager.

    1 argument:
    Pixel_Manager *pM - The draw manager that will draw the objects.
*/
void drawManager_Draw(Draw_Manager *dM)
{
    struct list *objectList = NULL;
    struct list *temp = NULL;
    Draw_Object *drawObj = NULL;

    int x = 0;
    static int frame = 0;

    int deltaTime = timer_Calc(&dM->srcTimer);

    /*Init the starting layer*/
    drawManager_InitLayer(dM);

    while(dM->currentLayer != NULL)
    {
        /*Get the objects to draw from the next layer*/
        objectList = dM->currentLayer->objectList;

        /*Draw all the objects in the layer*/
        while(objectList != NULL)
        {
            temp = objectList->next;

            drawObj = objectList->data;

            drawObj->type->drawFunction(drawObj);

            if(drawObj->lifeTime < 1)
            {
                if(drawObj->type->cleanFunction != NULL)
                {
                    drawObj->type->cleanFunction(drawObj);
                }

                drawLayer_Release(dM->currentLayer, objectList, 0);
            }
            else
            {
                drawObj->lifeTime -= deltaTime;
            }


            objectList = temp;
        }

        /*Find the next layer with objects to draw*/
        drawManager_NextLayer(dM);
    }

    return;
}

void drawManager_CleanLayers(Draw_Manager *dM)
{
    while(dM->layers != NULL)
    {
        drawLayer_Clean(dM->layers->data);
        mem_Free(dM->layers->data);

        list_Pop(&dM->layers);
    }

    return;
}

/*
    Function: drawManager_Clean

    Description -
    Deletes all the layers in the draw manager. Also removes any remaining draw objects in the manager.

    1 argument:
    Draw_Manager *dM - The draw manager to clean.
*/
void drawManager_Clean(Draw_Manager *dM)
{
    /*Delete all the layers and the objects inside of them*/
    drawManager_CleanLayers(dM);

    /*Delete all the draw types*/
    for(dM->numTypes = dM->numTypes; dM->numTypes > 0; dM->numTypes --)
    {
        drawType_Clean(dM->drawTypes[dM->numTypes - 1]);
        mem_Free(dM->drawTypes[dM->numTypes - 1]);
    }

    if(dM->drawTypes != NULL)
    {
        mem_Free(dM->drawTypes);
    }

    return;
}

void drawManager_Reset(Draw_Manager *dM)
{
    drawManager_CleanLayers(dM);

    dM->topLayer = 0;
    dM->maxTotalObjectLocations = 0;
    dM->reachedMax = 0;

    dM->currentLayer = NULL;
    dM->currentLayerList = NULL;

    return;
}

void drawManager_Report(Draw_Manager *dM)
{
    int x = 0;
    file_Log(ker_Log(), P_OUT, "Draw manager:\n Obj:%d\n Reached Max: %d\n", dM->totalObjectLocations, dM->reachedMax);

    file_Log(ker_Log(), P_OUT, "Draw Types:\n");

    for(x = 0; x < dM->numTypes; x++)
    {
        file_Log(ker_Log(), P_OUT, "%d: total unused %d\n", dM->drawTypes[x]->ID, dM->drawTypes[x]->totalFree);
    }

    return;
}
