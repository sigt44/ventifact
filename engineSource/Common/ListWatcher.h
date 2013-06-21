#ifndef LISTWATCHER_H
#define LISTWATCHER_H

#include "List.h"

enum watchTypes
{
    DW_LIST = 0,
    DW_PNT = 1,
    DW_INT = 2,
};

typedef union depwatch_UnionType
{
    struct list **listHead;
    void **doublePointer;
    int *intPointer;
} Depwatch_UnionType;

typedef struct depwatch_StructType
{
    struct list **listHead;
    void **doublePointer;
    int *intPointer;
} Depwatch_StructType;

typedef struct dependency_Watcher
{
    /*The dependency object, the one all other objects reference to*/
	void *objectDep;

    /*List will contain the references to this object, or the variables that can be used to let the reference know
    that the object has been deleted*/
	struct list *watcher;

} Dependency_Watcher;

int depWatcher_Clean(Dependency_Watcher *dW);

void depWatcher_Setup(Dependency_Watcher *dW, void *objectRef);

void depWatcher_Add(Dependency_Watcher *dW, struct list **headList);

void depWatcher_AddPnt(Dependency_Watcher *dW, void **objectPointer);

void depWatcher_AddInt(Dependency_Watcher *dW, int *intPointer);

void depWatcher_AddBoth(Dependency_Watcher *dW, struct list **headList, int info);

int depWatcher_Remove(Dependency_Watcher *dW, struct list **headList);

int depWatcher_RemovePnt(Dependency_Watcher *dW, void **objectPoiner);

void depWatcher_RemoveBoth(Dependency_Watcher *dW, struct list **headList);

int depWatcher_RemoveInt(Dependency_Watcher *dW, int *intPointer);

#endif
