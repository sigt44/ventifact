#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <stdarg.h>
#include "Common/List.h"

typedef struct data_Struct_Record
{
    int position;
    char type;
} Data_Struct_Record;

typedef struct data_Struct
{
    int amount;
    void **dataArray;

    struct list *dataInfo;

} Data_Struct;

Data_Struct_Record *dataStructType_Create(int position, char type);

Data_Struct *dataStruct_Create(int amount, ...);

Data_Struct *dataStruct_CreateType(char *dataTypes, ...);

Data_Struct *dataStruct_CreateEmpty(int amount);

int dataStruct_InsertData(Data_Struct *dataStruct, int *atPos, int amount, ...);
int dataStruct_InsertDataType(Data_Struct *dataStruct, int *atPos, char *dataTypes, ...);
int dataStruct_CopyDataType(Data_Struct *dataStruct, int *atPos, char *dataTypes, ...);

Data_Struct *dataStruct_Copy(Data_Struct *source);

void dataStruct_Report(Data_Struct *dataStruct);

void dataStruct_Clean(Data_Struct *dataStruct);

void dataStruct_Delete(Data_Struct **dataStruct);

#endif
