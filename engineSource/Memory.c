
/* Memory allocation
    When it needs to allocate memory for data it also allocates memory for a linked list.
    This allows it to unlink the list easily when freeing memory since the list will
    be - sizeof(linked list) from the head of memory*/

#include <stdlib.h>
#include "Memory.h"

#include "Common/List.h"

static struct mem_Component PE_Memory;

void mem_Reset(void)
{
    PE_Memory.memList = NULL;
    PE_Memory.currentBytesAllocated = 0;
    PE_Memory.maxBytesAllocated = 0;
    PE_Memory.memoryLeaked = 0;
    PE_Memory.log = NULL;

    return;
}

int mem_Init(void)
{
    mem_Reset();

    #ifdef MEM_TRACK
    #ifdef MEM_FILE
    PE_Memory.log = fopen("../Log/memory.txt","w");

    if(PE_Memory.log == NULL)
    {
        printf("Error could not create memory log, tried \"../Log/memory.txt\"\n");
        return 1;
    }

    fprintf(PE_Memory.log,"Memory log:\n\n");
    fprintf(PE_Memory.log,"Enabled.\n");

    #ifdef MEM_PRINTF
    printf("Memory tracking enabled.\n");
    #endif
    #endif
    #endif

    return 0;
}

void *mem_Free(void *memory)
{
    struct list *listPoint = NULL;
    struct mem_Info *infoPoint = NULL;
    char *data = (char *)memory;

    if(memory == NULL)
    {
        #ifdef MEM_PRINTF
        printf("MEM_MANAGER: Error trying to free NULL pointer\n");
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER: Error trying to free NULL pointer\n");
        #endif

        abort();
        return NULL;
    }

    #ifndef MEM_TRACK
    free(data);

    return NULL;
    #endif

    data = data - sizeof(struct mem_Info); /*Data now points to info block*/

    infoPoint = (struct mem_Info *)data;

    data = data - sizeof(struct list); /*Now data points to the start of the block*/

    listPoint = (struct list*)data;

    PE_Memory.currentBytesAllocated -= infoPoint->bytes;

    #ifdef MEM_FILE_V
    fprintf(PE_Memory.log,"MEM_MANAGER(%s)(%d): Freeing %d bytes from %p\n",infoPoint->file,infoPoint->line,infoPoint->bytes,data);
    #elif defined (MEM_FILE)
    if(PE_Memory.memoryLeaked == 1)
    {
        fprintf(PE_Memory.log,"MEM_MANAGER(%s)(%d): Freeing %d bytes from %p\n",infoPoint->file,infoPoint->line,infoPoint->bytes,data);
    }
    #endif

    #ifdef MEM_PRINTF_V
    printf("MEM_MANAGER(%s)(%d): Freeing %d bytes from %p\n",infoPoint->file,infoPoint->line,infoPoint->bytes,data);
    #elif defined (MEM_PRINTF)
    if(PE_Memory.memoryLeaked == 1)
    {
        printf("MEM_MANAGER(%s)(%d): Freeing %d bytes from %p\n",infoPoint->file,infoPoint->line,infoPoint->bytes,data);
    }
    #endif

    if(listPoint->data != data)
    {
        #ifdef MEM_PRINTF
        printf("\nMEM_MANAGER(%s)(%d): Error (%p,%d,%d)Head addresses are not the same!: %p != %p\n",infoPoint->file, infoPoint->line, PE_Memory.memList, infoPoint->bytes, sizeof(*PE_Memory.memList), listPoint->data, data);
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER(%s)(%d): Error Head addresses are not the same!: %p != %p\n",infoPoint->file,infoPoint->line,listPoint->data,data);
        #endif
        abort();
    }
    if( (unsigned int)listPoint->info != infoPoint->bytes)
    {
        #ifdef MEM_PRINTF
        printf("MEM_MANAGER(%s)(%d): Error bytes do not match! %d != %d\n",infoPoint->file,infoPoint->line,listPoint->info,infoPoint->bytes);
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER(%s)(%d): Error bytes do not match! %d != %d\n",infoPoint->file,infoPoint->line,listPoint->info,infoPoint->bytes);
        #endif

        abort();
    }

    /* Unlink the list that points to this memory */
    list_Unlink_Node(&PE_Memory.memList, listPoint);

    /*Then delete the memory chunk that contains the linked list, debug info and data to free */
    free(data);

    return NULL;
}

void *mem_Malloc(unsigned int bytes,const int line, const char *file)
{
    char *memory;
    char *memPoint;
    struct list *listAdd;
    struct mem_Info *info;

    #ifndef MEM_TRACK
    memory = malloc(bytes);

    return memory;
    #endif

    memPoint = memory = malloc(bytes + sizeof(struct list) + sizeof(struct mem_Info)); /*Create both the data and the linked list to track the data. Plus extra debug info */

    #ifdef MEM_PRINTF_V
    printf("MEM_MANAGER(%s)(%d): Allocating %d + %d bytes at %p\n\n",file,line,bytes,sizeof(struct list),memory);
    #endif

    #ifdef MEM_FILE_V
    fprintf(PE_Memory.log,"MEM_MANAGER(%s)(%d): Allocating %d + %d bytes at %p\n\n",file,line,bytes,sizeof(struct list),memory);
    #endif

    if(memory != NULL)
    {
        PE_Memory.currentBytesAllocated+= bytes;
        if(PE_Memory.currentBytesAllocated > PE_Memory.maxBytesAllocated)
            PE_Memory.maxBytesAllocated = PE_Memory.currentBytesAllocated;

        listAdd = (struct list*)memory;

        list_Push_Node(&PE_Memory.memList, listAdd, memory, bytes); /*Dont allocate any memory for the list just link it in */

        info = (struct mem_Info *)(memPoint + sizeof(struct list)); /*Find and then fill info memory block*/

        info->bytes = bytes;
        info->file = file;
        info->line = line;
    }
    else
    {
        #ifdef MEM_PRINTF
        printf("MEM_MANAGER(%s)(%d): Error - Failed to allocate %d + %d bytes.\n",file,line,bytes,sizeof(*PE_Memory.memList));
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER(%s)(%d): Error - Failed to allocate %d + %d bytes.\n",file,line,bytes,sizeof(*PE_Memory.memList));
        #endif

        mem_Report();
    }

    memPoint = (char *)(memory + sizeof(struct list) + sizeof(struct mem_Info));

    return memPoint;
}

void mem_Clear(void)
{
    while(PE_Memory.memList != NULL)
	{
        mem_Free((char *)PE_Memory.memList->data + sizeof(struct list) + sizeof(struct mem_Info));
	}

	return;
}

int mem_Report(void)
{
    if(PE_Memory.currentBytesAllocated)
    {
        #ifdef MEM_PRINTF
        printf("MEM_MANAGER: There is still %ld bytes allocated.\n",PE_Memory.currentBytesAllocated);
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER: There is still %ld bytes allocated.\n",PE_Memory.currentBytesAllocated);
        #endif

        PE_Memory.memoryLeaked = 1;

        return 1;
    }
    else if(PE_Memory.currentBytesAllocated < 0)
    {
        #ifdef MEM_PRINTF
        printf("MEM_MANAGER: ERROR - There is %ld memory allocated which is below 0.\n",PE_Memory.currentBytesAllocated);
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER: ERROR - There is %ld memory allocated which is below 0.\n",PE_Memory.currentBytesAllocated);
        #endif
        return 1;
    }
    else
    {

        #ifdef MEM_PRINTF
        printf("MEM_MANAGER: There is no memory currently allocated.\n");
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER: There is no memory currently allocated.\n");
        #endif
    }

    return 0;
}

int mem_Quit(void)
{
    #ifdef MEM_TRACK
    if(mem_Report())
    {
        #ifdef MEM_PRINTF
        printf("MEM_MANAGER: ERROR - Memory should of been released, freeing memory now...\n");
        #endif

        #ifdef MEM_FILE
        fprintf(PE_Memory.log,"MEM_MANAGER: ERROR - Memory should of been released, freeing memory now...\n");
        #endif

        /*#ifdef LINUX
        sync();
        #endif*/
        mem_Clear();

        if(mem_Report())
            return 1;
        else
        {
            #ifdef MEM_PRINTF
            printf("MEM_MANAGER: Warning memory has been released.\n");
            #endif

            #ifdef MEM_FILE
            fprintf(PE_Memory.log,"MEM_MANAGER: Warning memory has been released.\n");
            #endif
        }
    }
    #ifdef MEM_PRINTF
    printf("MEM_MANAGER: Peak memory usage %ld bytes (%ld kb)\n", PE_Memory.maxBytesAllocated, PE_Memory.maxBytesAllocated/1024);
    #endif

    #ifdef MEM_FILE
    fprintf(PE_Memory.log, "MEM_MANAGER: Peak memory usage %ld bytes (%ld kb)\n", PE_Memory.maxBytesAllocated, PE_Memory.maxBytesAllocated/1024);
    fclose(PE_Memory.log);
    #endif

    /*#ifdef LINUX
    sync();
    #endif*/

    #endif

    return 0;
}

