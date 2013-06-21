#include "File.h"

/*
"r"	Open a file for reading. The file must exist.
"w"	Create an empty file for writing. If a file with the same name already exists its content is erased and the file is treated as a new empty file.
"a"	Append to a file. Writing operations append data at the end of the file. The file is created if it does not exist.
"r+"	Open a file for update both reading and writing. The file must exist.
"w+"	Create an empty file for both reading and writing. If a file with the same name already exists its content is erased and the file is treated as a new empty file.
"a+"	Open a file for reading and appending. All writing operations are performed at the end of the file, protecting the previous content to be overwritten. You can reposition (fseek, rewind) the internal pointer to anywhere in the file for reading, but writing operations will move it back to the end of file. The file is created if it does not exist.
*/

static struct file_Component PE_File;

int file_Init(const char *logName)
{
    PE_File.fileList = NULL;

    if(file_Open(logName, "w+") == NULL)
        return 1;

    return 0;
}

FILE *file_Open(const char *file_Name, const char *flag)
{
    FILE *file_Create = NULL;
    if((file_Create = fopen(file_Name, flag)) != NULL)
    {
        list_Push(&PE_File.fileList,file_Create,sizeof(*PE_File.fileList));
        return file_Create;
    }
    else
    {
        printf("FILE_MANAGER: Error could not create/find the file %s .\n",file_Name);
        abort();
    }

    return NULL;
}

int file_Log(FILE *log, unsigned int printOut, char *text, ...)
{
	char textBuffer[256];
    va_list args;

    va_start(args, text);

    if(printOut == P_OUT)
	{
        vprintf(text, args);

	#ifdef WINDOWS
			vsnprintf(textBuffer, 255, text, args);
			OutputDebugString(textBuffer);
	#endif

	}

    if(log == NULL)
    {
        va_end(args);
        return 0;
    }

    if(vfprintf(log,text,args))
    {
        /*#ifdef LINUX
        if(fsync)
           sync();
        #endif*/
        va_end(args);
        return 0;
    }
    else
    {
        va_end(args);

        printf("FILE_MANAGER: Error could not write to file - %s.\n", text);

        abort();
    }

    return 1;
}


/*
    Function: file_GetString

    Description -
    Reads in a string from a file up to maxChar characters or until the EOF or '\n' symbols are read.
    If '\n' has been read then the function replaces that with the '\0' symbol.

    3 arguments:
    char *strBuffer - The character buffer of size maxChar to store the string into.
    int maxChar - The amount of characters the buffer can hold (including the NULL end of string character)
    FILE *stream - The file to read the string from.
*/
void file_GetString(char *strBuffer, int maxChar, FILE *stream)
{
    int length = 0;
    fgets(strBuffer, maxChar, stream);

    if(strBuffer != NULL)
    {
        length = strlen(strBuffer);

        if(strBuffer[length - 1] == '\n')
        {
            strBuffer[length - 1] = '\0';
        }
    }

    return;
}

/*Find if there is already a file in the specified DIR with the same name*/
int file_Check_Name(char *DIR, char *name)
{
    char *fullPath = (char *)mem_Malloc(strlen(DIR) + strlen(name) + sizeof(char), __LINE__, __FILE__);
    FILE *fc = NULL;

    strcpy(fullPath, DIR);
    strcat(fullPath, name);

    fc = fopen(fullPath, "r");

    mem_Free(fullPath);

    if(fc == NULL)
        return 0;

    fclose(fc);

    return 1;
}

/*Find any file in the directory passed that has the same extension specified*/
struct dirent *file_Get_Extension(DIR *FDIR, char extension[32])
{
        struct dirent *file;

        while((file = readdir(FDIR)) != NULL)
        {
            if(strstr(file->d_name, extension) != NULL)
            {
                return file;
            }
        }

        rewinddir(FDIR);
        return NULL;
}

/*Find any files in the directory passed that have the same extension specified and return each file represented by a dirent structure,
note each structure has been allocated memory so they need to be released.*/
struct list *file_Get_Extension_List(DIR *FDIR, char extension[32])
{
    struct list *fileHead = NULL;
    struct dirent *file = NULL;
	struct dirent *fileCopy = NULL;
    char *extTest;

    while((file = readdir(FDIR)) != NULL)
    {
        extTest = strstr(file->d_name, extension);

        if(extTest != NULL && *(extTest + strlen(extension)) == '\0' )
        {
			fileCopy = (struct dirent *)mem_Malloc(sizeof(struct dirent), __LINE__, __FILE__);
			*fileCopy = *file;
            list_Push(&fileHead, fileCopy, 0);
        }
    }

    rewinddir(FDIR);

    return fileHead;
}

int string_Line(char *editString) /*Get rid of the enter symbol in a string*/
{
    int x = 0;

	for(x = strlen(editString), editString = editString + x; x > 0; x--, editString--)
	{
        if(*editString == '\n')
		{
            *editString = '\0';
			break;
		}
    }

    return x;
}

/*Remove file extension from a string*/
void string_RemoveExt(char *editString)
{
    int x = 0;

    while(*(editString+x) != '\0')
    {
        if(*(editString+x) == '.')
            *(editString+x) = '\0';
        else
            x++;
    }
}

/*
    Function: string_CatLimit

    Description -
    Concatanates one string onto another. If the original string
    is not large enough to hold the other string then only copy the
    amount it can hold.

    Returns the amount of characters that have been concatanated to the original string.

    3 arguments:
    char *editString - The original string which will have another string concatanated to it.
    char *catString - The string to add the the original.
    int *limit - The amount of characters left that editString can hold. This will be updated
    by the function.
*/
int string_CatLimit(char *editString, char *catString, int *limit)
{
    int lengthDelta = 0;

    lengthDelta = *limit - strlen(catString);

    /*If there is enough room to concatanate the string*/
    if(lengthDelta > 0)
        lengthDelta = -(*limit - lengthDelta);
    else /*Otherwise only copy the remaining (*limit) amount of characters*/
        lengthDelta = *limit;

    if(*limit > 0)
    {
        strncat(editString, catString, lengthDelta);
        *limit -= lengthDelta;

        return lengthDelta;
    }

    return 0;
}

/*Returns a pointer to the first found requested character in the string*/
char *string_Target(char *string, char targetChar)
{
    int l = strlen(string);
    char *targetPointer = string + l;

    while(*targetPointer != targetChar && l > 0)
    {
        l --;
        targetPointer --;
    }

    return targetPointer;
}

int string_ReplaceChar(char *string, char replace, char replaceWith)
{
    int x = 0;
    char *replaceChar = string;

    while(*replaceChar != '\0')
    {
        if(*replaceChar == replace)
        {
            *replaceChar = replaceWith;
        }

        replaceChar ++;
    }

    return x;
}

/*Check if string passed is the same as the string read by the current FILE pointer */
int file_Check_Line(char text[255], FILE *read)
{
    char textBuffer[255];
    fgets(textBuffer,255,read);

    string_Line(textBuffer);

    if(strcmp(textBuffer,text) != 0)
    {
        if(ker_Log() != NULL)
            fprintf(ker_Log(), "Could not read '%s', instead found '%s'\n", text, textBuffer);
        else
            printf("Could not read '%s', instead found '%s'\n", text, textBuffer);

        return 0;
    }

    return 1;
}

int file_Close(FILE *file_Name)
{
    struct list *fileList = PE_File.fileList;
    struct list *fileCurrent = NULL;
    FILE *data = NULL;

    while(fileList != NULL)
    {
        fileCurrent = fileList;
        data = list_Iterate(&fileList);

        if(data == file_Name)
        {
            list_Delete_Node(&PE_File.fileList,fileCurrent);
            if(fclose(data) == EOF)
            {
                printf("FILE_MANAGER: Error could not close a file properly\n");
                return 0;
            }
            return 1;
        }
    }

    return 0;
}

/*Get a string from a file until max_Length characters have been read or until
EOF, \n or terminatingChar have been found. Returns length of string or -1 on error*/
int file_GetSubString(char *stringBuffer, int max_Length, char terminatingChar, FILE *file)
{
    int x = 0;
    char character = '0';

    if(file != NULL)
    {
        for(x = 0; x < max_Length; x++)
        {
            character = (char)fgetc(file);
            if(character == terminatingChar || character == '\n' || character == EOF)
            {
                stringBuffer[x] = '\0';
                break;
            }
            else
            {
                stringBuffer[x] = character;
            }
        }
    }
    else
    {
        return -1;
    }

    return x;
}


void file_Clear(void)
{
    while(PE_File.fileList != NULL)
	{
	    file_Close(PE_File.fileList->data);
	}

	return;
}

void file_Quit(void)
{
    #ifdef GP2X
    sync();
    #endif

    #ifdef DINGOO
    sync();
    #endif

    file_Clear();

	return;
}
