#ifndef SURFACE_H
#define SURFACE_H

#include "SDL/SDL.h"

#include "../Common/List.h"
#include "Pixel.h"
#include "../Kernel/Kernel.h"

struct surface_Pool
{
    char *surfacesDIR;

    struct list *surfaceList;
    unsigned int totalSurfacesInList;
};

int surf_Init(const char *rootDIR);

int surf_Quit(void);

SDL_Surface *surf_Copy(SDL_Surface *source);

SDL_Surface *surf_Create(unsigned int width,unsigned int height);

SDL_Surface *surf_LoadNew(char *name);

SDL_Surface *surf_Offset(unsigned int x, unsigned int y, SDL_Surface *source, int releaseSource);

SDL_Surface *surf_SimpleBox(int width, int height, Uint16P *baseColour, Uint16P *borderColour, int borderThickness);

int surf_LoadDIR(SDL_Surface **image, char *directory, char *name, int autoFreeID);
int surf_Load(SDL_Surface **image,char *name,int autoFreeID);

void surf_Blit(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip);

void surf_Blit_Free(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip);

int surf_Free_All(void);

int surf_Free_ID(int ID);

SDL_Surface *surface_Rotate(SDL_Surface *source, float angle, int autoFree);

#endif
