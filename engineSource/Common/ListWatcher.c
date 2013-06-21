/*
The DependencyWatcher structure is designed to make sure anything that references an object is told when that object is to be deleted.
This could be by removing the object in a list, setting reference pointer to NULL, setting an integer to 1*/
#include <stdio.h>

#include "ListWatcher.h"

/*
	Function: depWatcher_Clean

	Description -
	Removes all lists the watcher holds.

	If removeData is 1 then it checks all the lists the watcher holds, if any of them have the object the watcher references to then it removes it from the list.

    Returns the amount of references that have been deleted or told the object has been deleted.

	1 argument:
	Dependency_Watcher *dW - the list watcher to clean
*/
int depWatcher_Clean(Dependency_Watcher *dW)
{
	Depwatch_StructType type;

	int refRemoved = 0;

    while(dW->watcher != NULL)
    {
        switch(dW->watcher->info)
        {
            case DW_LIST:
            /*Get the list that may have a reference to the object*/
            type.listHead = dW->watcher->data;

            /*Find the reference and delete it*/
            refRemoved += list_Delete_NodeFromData(type.listHead, dW->objectDep); /*if the object is not in this list, nothing will be done*/

            break;

            case DW_PNT:
            type.doublePointer = dW->watcher->data;

            if(*type.doublePointer != dW->objectDep)
            {
               printf("CL: Error objPnt(%p) != objectDep(%p) from list size %d, DW: %p\n", *type.doublePointer, dW->objectDep, list_Size(dW->watcher), dW);
            }
            else
            {
                /*Let it be known the object has been deleted by setting the reference to it to be NULL*/
                *type.doublePointer = NULL;
                refRemoved ++;
            }

            break;

            case DW_INT:
            type.intPointer = dW->watcher->data;

            /*Tell the reference to the depedency object that it is deleted by setting the integer to > 0*/
            *type.intPointer = *type.intPointer + 1;
            refRemoved ++;
            break;
        }

        list_Pop(&dW->watcher);
    }

	return refRemoved;
}

/*
	Function: depWatcher_Setup

	Description -
	This function sets up the Dependency_Watcher structure for later use.

	2 arguments:
	Dependency_Watcher *dW - the Dependency_Watcher needed to setup
	void *objectDep - points to the object that is contained in all lists to be handled by the list watcher
*/
void depWatcher_Setup(Dependency_Watcher *dW, void *objectDep)
{
	dW->objectDep = objectDep;

	dW->watcher = NULL;

	return;
}

/*
	Function: depWatcher_Add

	Description -
	Function to add a linked list to a Dependency_Watcher. The linked list is presumed to contain a node which points to the same
	object as the dependency object in the Dependency_Watcher.

	2 arguments:
	Dependency_Watcher *dW - the dependency watcher that a linked list is to be added to
	struct list **headList - this should point to the base address of the linked list that is to be added
*/
void depWatcher_Add(Dependency_Watcher *dW, struct list **headList)
{
	list_Push(&dW->watcher, headList, DW_LIST);

	return;
}

/*
	Function: depWatcher_AddPnt

	Description -
	Function that adds a double pointer to the Dependency_Watcher. The double pointer
	is the address of the pointer that is a reference to the dependency object.
	When the dependency object is deleted, this pointer will be assigned to NULL.

	2 arguments:
	Dependency_Watcher *dW - the dependency watcher that a object double pointer is to be added to
	void **objectPointer - this should point to the address of the object reference pointer.
*/
void depWatcher_AddPnt(Dependency_Watcher *dW, void **objectPointer)
{
	list_Push(&dW->watcher, objectPointer, DW_PNT);

	return;
}

/*
	Function: depWatcher_AddInt

	Description -
	Function that adds a pointer to an integer in the Dependency_Watcher.
	The integer should be used to let a reference know that the object it is pointing
	to has been deleted.

	When the dependency object is deleted the value of this integer will be set to 1.

	2 arguments:
	Dependency_Watcher *dW - the dependency watcher that a int pointer is to be added to
	int *intPointer - the pointer to the integer that will be set to '1' when the dependency object is deleted.
*/
void depWatcher_AddInt(Dependency_Watcher *dW, int *intPointer)
{
	list_Push(&dW->watcher, intPointer, DW_INT);

	/*Make sure that the integer has value 0*/
	*intPointer = 0;

	return;
}

/*
	Function: depWatcher_AddBoth

	Description -
	Function to add a linked list to the Dependency_Watcher. At the same time it creates a new node in the linked list,
	this node points to the object that is referenced to by the Dependency_Watcher.

	3 arguments -
	Dependency_Watcher *dW - the list watcher that a linked list is to be added to
	struct list **headList - this should point to the base address of the linked list that is to be added
	int info - input a number here that will be inserted into the new node of the list
*/
void depWatcher_AddBoth(Dependency_Watcher *dW, struct list **headList, int info)
{
	/*create new node in the list, set it to point to the Dependency_Watcher object reference*/
	list_Push(headList, dW->objectDep, info);

    depWatcher_Add(dW, headList);

	return;
}

/*
	Function - depWatcher_RemoveBoth

	Description -
	Removes a list from the Dependency_Watcher, also removes the node in the list that contains the Dependency_Watcher's object reference

	2 arguments:
	Dependency_Watcher *dW - the Dependency_Watcher to remove the list from
	struct list **headList - the head of the linked list that is to be removed. If the list contains a node that points to the
	Dependency_Watchers object reference then that is to be removed.
*/
void depWatcher_RemoveBoth(Dependency_Watcher *dW, struct list **headList)
{
    list_Delete_NodeFromData(headList, dW->objectDep);

    depWatcher_Remove(dW, headList);

    return;
}


/*
    The remove functions are to be called when a reference to the dependency object is
    deleted before the dependency object.
*/

/*
	Function - depWatcher_Remove

	Description -
	Removes a list/double pointer from the Dependency_Watcher.

	2 arguments:
	Dependency_Watcher *dW - the Dependency_Watcher to remove the list from
	void **watcherObject - the head of the linked list that is to be removed.
*/
int depWatcher_Remove(Dependency_Watcher *dW, struct list **watcherObject)
{
	return list_Delete_NodeFromData(&dW->watcher, watcherObject);;
}

int depWatcher_RemovePnt(Dependency_Watcher *dW, void **objectPointer)
{
    void **objPnt = objectPointer;

    if(*objPnt != dW->objectDep)
    {
        printf("RM: Error objPnt(%p) != objectDep(%p)\n", *objPnt, dW->objectDep);
    }


	return list_Delete_NodeFromData(&dW->watcher, objectPointer);
}

int depWatcher_RemoveInt(Dependency_Watcher *dW, int *intPointer)
{
	return list_Delete_NodeFromData(&dW->watcher, intPointer);
}
