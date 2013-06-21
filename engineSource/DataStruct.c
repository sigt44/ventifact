#include "DataStruct.h"

#include <stdarg.h>
#include <string.h>

#include "Memory.h"

/*
    Function: dataStructType_Create

    Description -
    This creates a record of a variable in a
    data structure that has been allocated memory especially for it.

    The record is used to clean up the memory once the data structure is no longer
    being used.

    2 arguments:
    int position - The position of the variable in the data structure that has been allocated memory.
    char type - The type of variable in the data structure.
        'd' - To say an integer has been created.
        'f' - To say a float has been created.
*/
Data_Struct_Record *dataStructType_Create(int position, char type)
{
    Data_Struct_Record *dataType = (Data_Struct_Record *)mem_Malloc(sizeof(Data_Struct_Record), __LINE__, __FILE__);

    dataType->position = position;
    dataType->type = type;

    return dataType;
}

/*
    Function: dataStruct_Create

    Description -
    This creates a generic data structure. Each variable
    is located in an array.

    2 arguments:
    int amount - The amount of variables being assigned to this data structure.
    Each variable is assumed to be a generic pointer. You can just assign a specific
    number instead of an address as a value, making the pointer act as an integer.
    But this may not be portable so you should use dataStruct_CreateType for that.

    ... - The variable values should be inserted here.
*/
Data_Struct *dataStruct_Create(int amount, ...)
{
    Data_Struct *nData = (Data_Struct *)mem_Malloc(sizeof(Data_Struct), __LINE__, __FILE__);
    va_list args;
    int x;

    nData->amount = amount;
    nData->dataInfo = NULL;

    if(amount > 0)
    {
        nData->dataArray = (void *)mem_Malloc(sizeof(void *) * amount, __LINE__, __FILE__);

        va_start(args, amount);

        for(x = 0; x < amount; x++)
        {
            nData->dataArray[x] = va_arg(args, void *);
        }

        va_end(args);
    }
    else
        nData->dataArray = NULL;

    return nData;
}

/*
    Function: dataStruct_CreateType

    Description -
    This creates a generic data structure. Each variable
    is located in an array.

    2 arguments:
    char *dataTypes - This should be a string consisting of a series of letters,
    each of them representing a data type of the variables to be created such as int, float or pointer. Note that memory
    will be allocated to create the space for an integer or float.
    int = 'd'
    float  = 'f'
    pointer = 'p'

    ... - The variable values should be inserted here (in order of dataTypes letters).
*/
Data_Struct *dataStruct_CreateType(char *dataTypes, ...)
{
    Data_Struct *nData = (Data_Struct *)mem_Malloc(sizeof(Data_Struct), __LINE__, __FILE__);
    va_list args;

    int amount = strlen(dataTypes);
    int x = 0;
    char *dataType = dataTypes;
	int data = 0;

    nData->amount = amount;
    nData->dataInfo = NULL;

    if(amount > 0)
    {
        nData->dataArray = (void **)mem_Malloc(sizeof(void *) * amount, __LINE__, __FILE__);

        va_start(args, dataTypes);

        for(x = 0; x < amount; x++)
        {
            switch(*dataType)
            {
                default:
                printf("Unknown data type '%c' ignoring variable\n", *dataType);
                break;

                case 'd':

                nData->dataArray[x] = mem_Malloc(sizeof(int), __LINE__, __FILE__);

                *(int *)nData->dataArray[x] = va_arg(args, int);
                list_Push_Sort(&nData->dataInfo, dataStructType_Create(x, *dataType), x);

                break;

                case 'f':

                nData->dataArray[x] = mem_Malloc(sizeof(double), __LINE__, __FILE__);

                *(double *)nData->dataArray[x] = va_arg(args, double);

                list_Push_Sort(&nData->dataInfo, dataStructType_Create(x, *dataType), x);

                break;

                case 'p':

                nData->dataArray[x] = va_arg(args, void *);

                break;
            }

            dataType ++;
        }

        va_end(args);
    }
    else
        nData->dataArray = NULL;

    return nData;
}

/*
    Function: dataStruct_CreateEmpty

    Description -
    This creates a generic empty data structure. The data array will
    be able to hold a specific amount of variables determined by the parameter.
    Each variable is set to be NULL. It is expected to fill them using
    dataStruct_InsertData or dataStruct_InsertDataType.

    1 argument:
    int amount - The amount of variables the data structure can hold.
*/
Data_Struct *dataStruct_CreateEmpty(int amount)
{
    int x = 0;

    Data_Struct *nData = (Data_Struct *)mem_Malloc(sizeof(Data_Struct), __LINE__, __FILE__);

    nData->amount = amount;
    nData->dataInfo = NULL;

    if(amount > 0)
    {
        nData->dataArray = (void *)mem_Malloc(sizeof(void *) * amount, __LINE__, __FILE__);

        for(x = 0; x < amount; x++)
        {
            nData->dataArray[x] = NULL;
        }
    }
    else
        nData->dataArray = NULL;

    return nData;
}

/*
    Function: dataStruct_InsertData

    Description -
    This function allows you to insert variables into an already created
    data structure.

    4 arguments:
    Data_Struct *dataStruct - The data structure to insert variables.
    int *atPos - The starting position to insert the variables into the array.
    Note that this value gets incremented by the amount of data inserted.
    int amount - The amount of variables being inserted to this data structure.
    Each variable is assumed to be a generic pointer. You can just assign a specific
    number instead of an address as a value, making the pointer act as an integer.
    But this may not be portable so you should use dataStruct_InsertType for that.

    ... - The variable values should be inserted here.
*/
int dataStruct_InsertData(Data_Struct *dataStruct, int *atPos, int amount, ...)
{
    int x = 0;
    int endPos = *atPos + amount;

    va_list args;

    if(endPos > dataStruct->amount)
    {
        printf("Error trying to place too much data into data array, %d vs %d\n", dataStruct->amount, endPos);
        return 1;
    }

    va_start(args, amount);

    for(x = *atPos; x < endPos; x++)
    {
        dataStruct->dataArray[x] = va_arg(args, void *);
    }

    va_end(args);

    *atPos = *atPos + amount;

    return 0;
}

/*
    Function: dataStruct_InsertDataType

    Description -
    This function allows you to insert variables into an already created
    data structure.

    4 arguments:
    Data_Struct *dataStruct - The data structure to insert variables.
    int *atPos - The starting position to insert the variables into the array.
    Note that this value gets incremented by the amount of data inserted.
    char *dataTypes - This should be a string consisting of a series of letters,
    each of them representing a data type of the variables to be inserted such as int, float or pointer. Note that memory
    will be allocated to create the space for an integer or float.
    int = 'd'
    float  = 'f'
    pointer = 'p'

    ... - The variable values should be inserted here (in order of dataTypes letters).
*/
int dataStruct_InsertDataType(Data_Struct *dataStruct, int *atPos, char *dataTypes, ...)
{
    int amount = strlen(dataTypes);
    int x = 0;
    int endPos = *atPos + amount;
    char *dataType = dataTypes;

    va_list args;

    if(endPos > dataStruct->amount)
    {
        printf("Error trying to place too much data into data array, %d vs %d\n", dataStruct->amount, endPos);
        return 1;
    }

    va_start(args, dataTypes);

    for(x = *atPos; x < endPos; x++)
    {
        switch(*dataType)
        {
            default:
            printf("Unknown data type '%c' ignoring variable\n", *dataType);
            break;

            case 'd':

            dataStruct->dataArray[x] = mem_Malloc(sizeof(int), __LINE__, __FILE__);

            *(int *)dataStruct->dataArray[x] = va_arg(args, int);

            list_Push_Sort(&dataStruct->dataInfo, dataStructType_Create(x, *dataType), x);

            break;

            case 'f':

            dataStruct->dataArray[x] = mem_Malloc(sizeof(double), __LINE__, __FILE__);

            *(double *)dataStruct->dataArray[x] = va_arg(args, double);

            list_Push_Sort(&dataStruct->dataInfo, dataStructType_Create(x, *dataType), x);

            break;

            case 'p':

            dataStruct->dataArray[x] = va_arg(args, void *);

            break;
        }

        dataType ++;
    }

    va_end(args);

    *atPos = *atPos + amount;

    return 0;
}

/*
    Function: dataStruct_CopyDataType

    Description -
    This function allows you to copy variables from one data structure into an already created
    data structure.

    4 arguments:
    Data_Struct *dataStruct - The data structure to insert variables.
    int *atPos - The starting position to insert the variables into the array.
    Note that this value gets incremented by the amount of data inserted.
    char *dataTypes - This should be a string consisting of a series of letters,
    each of them representing a data type of the variables to be inserted such as int, float or pointer. Note that memory
    will be allocated to create the space for an integer or float.
    int = 'd'
    float  = 'f'
    pointer = 'p'

    ... - The variables of a data structure should be inserted here (in order of dataTypes letters).
*/
int dataStruct_CopyDataType(Data_Struct *dataStruct, int *atPos, char *dataTypes, ...)
{
    int amount = strlen(dataTypes);
    int x = 0;
    int endPos = *atPos + amount;
    char *dataType = dataTypes;

    va_list args;

    if(endPos > dataStruct->amount)
    {
        printf("Error trying to place too much data into data array, %d vs %d\n", dataStruct->amount, endPos);
        return 1;
    }

    va_start(args, dataTypes);

    for(x = *atPos; x < endPos; x++)
    {
        switch(*dataType)
        {
            default:
            printf("Unknown data type '%c' ignoring variable\n", *dataType);
            break;

            case 'd':

            dataStruct->dataArray[x] = mem_Malloc(sizeof(int), __LINE__, __FILE__);

            *(int *)dataStruct->dataArray[x] = *(int *)va_arg(args, int *);

            list_Push_Sort(&dataStruct->dataInfo, dataStructType_Create(x, *dataType), x);

            break;

            case 'f':

            dataStruct->dataArray[x] = mem_Malloc(sizeof(double), __LINE__, __FILE__);

            *(double *)dataStruct->dataArray[x] = *(double *)va_arg(args, double *);

            list_Push_Sort(&dataStruct->dataInfo, dataStructType_Create(x, *dataType), x);

            break;

            case 'p':

            dataStruct->dataArray[x] = va_arg(args, void *);

            break;
        }

        dataType ++;
    }

    va_end(args);

    *atPos = *atPos + amount;

    return 0;
}

/*
    Function: dataStruct_Copy

    Description -
    Returns a new data structure that is a copy of the one in the parameter.

    Memory is not shared between each data structure so if that source
    data structure contained a malloced integer, so would the copied structure.
    (Instead of the copied structure simply pointing to sources malloced integer)

    1 argument:
    Data_Struct *source - The source data structure that is to be copied.
*/
Data_Struct *dataStruct_Copy(Data_Struct *source)
{
    Data_Struct *nData = dataStruct_CreateEmpty(source->amount);

    int x = 0;
    int insertPos = 0;
    struct list *dataInfo = source->dataInfo;
    Data_Struct_Record *sourceType = NULL;
    char typeString[2] = {'0', '\0'};

    /*Obtain the records of any variables in the source data structure that have been
    allocated memory*/
    while(dataInfo != NULL)
    {
        sourceType = dataInfo->data;

        /*Fill in the variables that are simple pointers, up to the position of the
        variable that owns the address it is pointing to*/
        for(x = insertPos; x < sourceType->position; x++)
        {
            nData->dataArray[x] = source->dataArray[x];
            //printf("Simple copy [%d] [%p] [%p]\n", x, nData->dataArray[x], source->dataArray[x]);
        }

        insertPos = sourceType->position;

        typeString[0] = sourceType->type;

        /*Make a copy of the variable and insert it into the new data structure*/
        dataStruct_CopyDataType(nData, &insertPos, typeString, source->dataArray[insertPos]);
        //printf("Main copy [%d] [%p] [%p]\n", sourceType->position, nData->dataArray[sourceType->position], source->dataArray[sourceType->position]);

        dataInfo = dataInfo->next;
    }

    /*Fill in the remainder of simple pointer variables*/
    for(x = insertPos; x < source->amount; x++)
    {
        nData->dataArray[x] = source->dataArray[x];
        //printf("Simple copy [%d] [%p] [%p]\n", x, nData->dataArray[x], source->dataArray[x]);
    }

    return nData;
}

void dataStruct_Report(Data_Struct *dataStruct)
{
    struct list *recordList = dataStruct->dataInfo;
    Data_Struct_Record *record = NULL;

    int x = 0;

    printf("Data structure report: %p\n", dataStruct);

    printf("Total variables: %d\n", dataStruct->amount);

    printf("Local variables -\n");
    while(recordList != NULL)
    {
        record = recordList->data;

        switch(record->type)
        {
            default:
            printf("[%d] unknown type [%c]: %p\n", record->position, record->type, dataStruct->dataArray[record->position]);
            break;

            case 'd':
            printf("[%d] type [%c]: %d\n", record->position, record->type, *(int *)dataStruct->dataArray[record->position]);
            break;

            case 'f':
            printf("[%d] type [%c]: %d\n", record->position, record->type, *(int *)dataStruct->dataArray[record->position]);
            break;

        }

        recordList = recordList->next;
    }

    printf("Pointer array - \n");
    for(x = 0; x < dataStruct->amount; x++)
    {
        printf("[%d]: %p\n", x, dataStruct->dataArray[x]);
    }

    return;
}

/*
    Function: dataStruct_CleanType

    Description -
    Cleans up a record of a local variable in the data struct and
    releases the memory created for that variable.

    2 arguments:
    Data_Struct *dataStruct - The data structure that the variable (and record) is in.
    Data_Struct_Record *dataType - The record of the local variable.
*/
void dataStruct_CleanType(Data_Struct *dataStruct, Data_Struct_Record *dataType)
{
    mem_Free(dataStruct->dataArray[dataType->position]);

    return;
}

/*
    Function: dataStruct_Clean

    Description -
    Cleans up data structure, releases all memory that is local to it.

    1 argument:
    Data_Struct *dataStruct - The data structure to clean.
*/
void dataStruct_Clean(Data_Struct *dataStruct)
{
    /*Clean up all the variables that were allocated memory (ones that were not pointers)*/
    while(dataStruct->dataInfo != NULL)
    {
        /*Clean up the variable allocated memory*/
        dataStruct_CleanType(dataStruct, dataStruct->dataInfo->data);

        /*Delete the record of this*/
        mem_Free(dataStruct->dataInfo->data);

        list_Pop(&dataStruct->dataInfo);
    }

    /*Delete the array of pointers that have been assigned to each variable*/
    if(dataStruct->dataArray != NULL)
        mem_Free(dataStruct->dataArray);

    return;
}

/*
    Function: dataStruct_Delete

    Description -
    Cleans up data structure, releases all memory that is local to it.
    Also deletes the data structure and sets the pointer to it to be NULL.

    1 argument:
    Data_Struct *dataStruct - The data structure to delete.
*/
void dataStruct_Delete(Data_Struct **dataStruct)
{
    dataStruct_Clean(*dataStruct);
    mem_Free(*dataStruct);

    *dataStruct = NULL;

    return;
}
