#include "Screenshot.h"
#include "File.h"

int screenshot_Take(SDL_Surface *surf, char *saveName)
{
	int nameLength = strlen(saveName) + 3;
    char *newName = (char *)mem_Malloc(nameLength + 1, __LINE__, __FILE__);
    char *ext = NULL;
    char numbers[3];
    int rn = 0;

    strncpy(newName, saveName, nameLength);

    /*Get the point of the extension and replace it with "00"*/
    ext = string_Target(newName, '.');
    *ext = '0';
    *(ext + 1) = '0';
    *(ext + 2) = '\0';

    /*Copy back the extension*/
    ext = string_Target(saveName, '.');
    strcat(newName, ext);

    while(file_Check_Name("./", newName) == 1)
    {
        ext = string_Target(newName, '.');
        numbers[1] = *(ext - 1);
        numbers[0] = *(ext - 2);

        /*Increase the file ending number*/
        rn = atoi(numbers);
        rn ++;

        if(rn < 10)
        {
            sprintf(numbers,"%d", 10 + rn);
            numbers[0] = '0';
        }
        else if(rn < 100)
            sprintf(numbers,"%d", rn);
        else
        {
            printf("Error: Too many files of similar name to %s\n", saveName);
            mem_Free(newName);
            return 1;
        }

        /*Correct the real string*/
        *(ext - 1) = numbers[1];
        *(ext - 2) = numbers[0];

    }

    SDL_SaveBMP(surf, newName);

    mem_Free(newName);

    return 0;
}


