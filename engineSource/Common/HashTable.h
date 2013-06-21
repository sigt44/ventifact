#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "List.h"

typedef struct hash_Table
{
	int numElements;

	struct list **tableArray;

} Hash_Table;

int hashTable_HashModulus(Hash_Table *hTable, int ID);

Hash_Table *hashTable_Create(int numElements);

int hashTable_Setup(Hash_Table *hTable, int numElements);

int hashTable_Insert(Hash_Table *hTable, void *object, int ID);

void *hashTable_Retrieve(Hash_Table *hTable, int ID, int remove);

void hashTable_Clear(Hash_Table *hTable, int deleteNodes);

void hashTable_Clean(Hash_Table *hTable, int deleteNodes);

#endif
