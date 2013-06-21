#ifndef FILE_H
#define FILE_H

#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#include "Common/List.h"
#include "Memory.h"
#include "Kernel/Kernel.h"

#ifdef WINDOWS
#include <Windows.h>

#define snprintf _snprintf

#endif

#define P_OUT 1

struct file_Component
{
    struct list *fileList;

};

int file_Init(const char *logName);

FILE *file_Open(const char *file_Name, const char *flag);

int file_Log(FILE *log, unsigned int printOut, char *text, ...);

/*Find any file in the directory passed that has the same extension specified*/
struct dirent *file_Get_Extension(DIR *FDIR, char extension[32]);

/*Find any files in the directory passed that have the same extension specified and return a list of dirent*/
struct list *file_Get_Extension_List(DIR *FDIR, char extension[32]);

/*Check if string passed is the same as the string read by the current FILE pointer */
int file_Check_Line(char text[255], FILE *read);

/*Find if there is already a file in the specified DIR with the same name*/
int file_Check_Name(char *DIR, char *name);

int file_Close(FILE *file_Name);

void file_Clear(void);

void file_Quit(void);

void file_GetString(char *strBuffer, int maxChar, FILE *stream);

/*Get rid of the enter symbol in a string*/
int string_Line(char *editString);

void string_RemoveExt(char *editString);

char *string_Target(char *string, char targetChar);

int string_ReplaceChar(char *string, char replace, char replaceWith);

int string_CatLimit(char *editString, char *catString, int *limit);

/*Get a string from a file until max_Length characters have been read or until
EOF, \n or terminatingChar have been found. Returns length of string or -1 on error*/
int file_GetSubString(char *stringBuffer, int max_Length, char terminatingChar, FILE *file);

#endif
