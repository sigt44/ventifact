#include "HashTable.h"

#include <stdio.h>

#include "../Memory.h"

int hashTable_HashModulus(Hash_Table *hTable, int ID)
{
    return (ID % hTable->numElements);
}

Hash_Table *hashTable_Create(int numElements)
{
    Hash_Table *newTable = (Hash_Table *)mem_Alloc(sizeof(Hash_Table));

    if(hashTable_Setup(newTable, numElements) == 1)
    {
        mem_Free(newTable);
        return NULL;
    }

    return newTable;
}

int hashTable_Setup(Hash_Table *hTable, int numElements)
{
    if(numElements <= 0)
    {
        printf("Invalid hash table with <= 0 elements\n");
        return 1;
    }

    hTable->numElements = numElements;

    hTable->tableArray = (struct list **)mem_Alloc(numElements * sizeof(struct list *));

    return 0;
}

int hashTable_Insert(Hash_Table *hTable, void *object, int ID)
{
    int elementLocation = hashTable_HashModulus(hTable, ID);
    printf("Hashed %d to %d\n", ID, elementLocation);

    list_Push(&hTable->tableArray[elementLocation], object, ID);

    return elementLocation;
}

void *hashTable_Retrieve(Hash_Table *hTable, int ID, int remove)
{
    int elementLocation = hashTable_HashModulus(hTable, ID);

    struct list *objectList = hTable->tableArray[elementLocation];
    void *object = NULL;

    while(objectList != NULL)
    {
        if(objectList->info == ID)
        {
            object = objectList->data;

            if(remove > 0)
            {
                list_Delete_Node(hTable->tableArray, objectList);
            }

            break;
        }

        objectList = objectList->next;
    }

    return object;
}

void hashTable_Clear(Hash_Table *hTable, int deleteNodes)
{
    int x = 0;

    for(x = 0; x < hTable->numElements; x++)
    {
        if(hTable->tableArray[x] != NULL)
        {
            if(deleteNodes > 0)
            {
                list_ClearAll(&hTable->tableArray[x]);
            }
            else
            {
                list_Clear(&hTable->tableArray[x]);
            }
        }
    }

    return;
}


void hashTable_Clean(Hash_Table *hTable, int deleteNodes)
{
    struct list *nodeList = NULL;

    hashTable_Clear(hTable, deleteNodes);

    mem_Free(hTable->tableArray);

    hTable->tableArray = NULL;

    return;
}
