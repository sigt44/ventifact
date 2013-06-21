#include <string.h>
#include <math.h>
#include "SDL/SDL_image.h"
#include "Surface.h"
#include "../Maths.h"

#include "Graphics.h"
#include "Pixel.h"
#include "../Memory.h"
#include "../Kernel/Kernel.h"
#include "../Common/List.h"

static struct surface_Pool surf_C;

int surf_Init(const char *rootDIR)
{
    surf_C.surfacesDIR = (char *)mem_Malloc(256, __LINE__, __FILE__);
    strncpy(surf_C.surfacesDIR, rootDIR, 255);

    surf_C.surfaceList = NULL;
    surf_C.totalSurfacesInList = 0;

    return 0;
}

int surf_Quit(void)
{
    surf_Free_All();
    mem_Free(surf_C.surfacesDIR);

    return 0;
}

SDL_Surface *surf_Copy(SDL_Surface *source)
{
    SDL_Surface *returnSurface = NULL;

    if(source == NULL)
    {
        file_Log(ker_Log(), 1, "\nError - trying to copy NULL surfce\n");
        return NULL;
    }

    returnSurface = SDL_ConvertSurface(source, source->format, source->flags);

    if(returnSurface == NULL)
    {
        file_Log(ker_Log(), 1, "\nError - could not copy a surface. (%s)\n",SDL_GetError());
    }

    return returnSurface;
}

SDL_Surface *surf_Create(unsigned int width, unsigned int height)
{
    SDL_Surface *returnSurface = SDL_CreateRGBSurface(0, width, height, ker_Screen_BPP(), 0,0,0,0);

    if(returnSurface == NULL)
    {
        printf("\nError - could not create a surface. (%s)\n",SDL_GetError());
        file_Log(ker_Log(),1,"\nError - could not create a surface. (%s)\n",SDL_GetError());
    }

    SDL_SetColorKey(returnSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, *ker_colourKey());

    return returnSurface;
}

/*
    Function: surf_SimpleBox

    Description -
    Returns a surface filled with a certain colour and a border.

    5 arguments:
    int width - width of the new surface.
    int height - height of the new surface.
    Uint16P *baseColour - main colour of the surface.
    Uint16P *borderColour - border colour of the surface.
    int borderThickness - the thickness of the border.
*/
SDL_Surface *surf_SimpleBox(int width, int height, Uint16P *baseColour, Uint16P *borderColour, int borderThickness)
{
    SDL_Surface *box = surf_Create(width, height);

    if(borderThickness != 0)
    {
        /*Fill in the border colour*/
        fill_Rect(0, 0, width, height, borderColour, box);

        /*Fill in the main colour*/

        fill_Rect(borderThickness, borderThickness, width - (borderThickness*2), height - (borderThickness*2), baseColour, box);
    }
    else
    {
        /*Just fill in the main colour*/
        fill_Rect(0, 0, width, height, baseColour, box);
    }

    return box;
}

/*
    Function: surf_Offset

    Description:
    Creates a new surface that contains the source surface offseted by a specified amount.

    4 arguments:
    unsigned int x - x offset of the source surface.
    unsigned int y - y offset of the source surface.
    SDL_Surface *source - source surface to offset and blit onto the new one.
    int releaseSource - if 1 then the source surface will be deleted.
*/
SDL_Surface *surf_Offset(unsigned int x, unsigned int y, SDL_Surface *source, int releaseSource)
{
    /*Create the new surface with enough room for the offset*/
    SDL_Surface *offsetSurface = surf_Create(x + source->w, y + source->h);

    /*Make it transparent*/
    fill_Rect(0, 0, offsetSurface->w, offsetSurface->h, (Uint16P *)ker_colourKey(), offsetSurface);

    /*Blit the surface to be offseted to the new one*/
    surf_Blit(x, y, source, offsetSurface, NULL);

    /*If the original surface should be released*/
    if(releaseSource == 1)
    {
        SDL_FreeSurface(source);
    }

    return offsetSurface;
}

SDL_Surface *surf_LoadNew(char *name)
{
	SDL_Surface *newSurface = NULL;

	surf_Load(&newSurface, name, M_FREE);

	return newSurface;
}

int surf_Load(SDL_Surface **image, char *name, int autoFreeID)
{
    char *imageLoc;
	SDL_Surface *optimizedImage = NULL;

    imageLoc = (char *)mem_Malloc(strlen(surf_C.surfacesDIR) + strlen(name) + 1, __LINE__, __FILE__);

    strcpy(imageLoc, surf_C.surfacesDIR);
    strcat(imageLoc, name);

    *image = IMG_Load(imageLoc);

	mem_Free(imageLoc);

	if(*image == NULL)
	{
	    printf("\nError - could not load image\n\t%s. (%s)\n", name, SDL_GetError());
        file_Log(ker_Log(), 0, "\nError - could not load image\n\t%s. (%s)\n", name, SDL_GetError());
		return 1;
	}

	optimizedImage = SDL_DisplayFormat(*image);

	SDL_FreeSurface(*image);

	if(optimizedImage == NULL)
		return 2;

	SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY | SDL_RLEACCEL, *ker_colourKey());

	*image = optimizedImage;

	if(autoFreeID == A_FREE)
		list_Push(&surf_C.surfaceList, *image, autoFreeID);

	return 0;
}


void surf_Blit(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_BlitSurface(source, clip, destination, &dest);
	return;
}

void surf_Blit_Free(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_BlitSurface(source, clip, destination, &dest);

	SDL_FreeSurface(source);
	return;
}

int surf_Free_All(void)
{
	SDL_Surface *freeImage = NULL;

	while(surf_C.surfaceList != NULL)
	{
        freeImage = surf_C.surfaceList->data;

        if(freeImage != NULL)
            SDL_FreeSurface(freeImage);

        freeImage = NULL;

		list_Pop(&surf_C.surfaceList);
	}

	return 0;
}

int surf_Free_ID(int ID)
{
    struct list *surfaceList = surf_C.surfaceList;
    struct list *surfaceListTemp;
	SDL_Surface *freeImage = NULL;

	while(surfaceList != NULL)
	{
	    surfaceListTemp = surfaceList->next;
		if(surfaceList->info == ID)
		{
			freeImage = surfaceList->data;

			if(freeImage != NULL)
				SDL_FreeSurface(freeImage);

			freeImage = NULL;

			list_Delete_Node(&surf_C.surfaceList,surfaceList);

		}

        surfaceList = surfaceListTemp;
	}

	return 0;
}

SDL_Surface *surface_Rotate(SDL_Surface *source, float angle, int autoFree)
{
    SDL_Surface *rotatedSurface = NULL;

    int x = 0;
    int y = 0;

    float sOriginX, sOriginY;
    float distance;
    float xDis, yDis;
    float dX, dY;
    float odX, odY;

    float newAngle;
    float cosA, sinA;

    float newWidth;
    float newHeight;

    float rOriginX;
    float rOriginY;

    Uint16P *sPixel;
    Uint16P *dPixel;

    /*Fix the angle to judge the size of the new rotated image*/
    newAngle = (float)fabs(angle);

    if(newAngle > PI_2)
    {
        /*If the angle is larger than 360 then reduce it to keep it in that domain*/
        if(newAngle > PI2)
        {
            newAngle = mth_Segment(newAngle, PI2);
        }

        /*Keep the angle within 90 domain*/
        switch(mth_GetQuadrant_Angle(newAngle))
        {
            case 1:
            /*Make the new origin to be from the negative x-axis and positive y-axis*/
            newAngle = PI - newAngle;
            break;

            case 2:
            /*make the new angle be from the negative x-axis and y-axis*/
            newAngle = (float)fabs(PI - newAngle);
            break;

            case 3:
            /*make the new angle be from the positive axis and negative y-axis*/
            newAngle = PI2 - newAngle;
            break;
        }
        /*The code above in this branch is the same as doing
        newAngle = fabs(atan(tan(newAngle)));*/
    }

    /*Obtain the new width and height using trigonomentry*/
    newWidth = (source->w * cosf(newAngle)) + (source->h * cosf((PI/2) - newAngle));

    distance = sqrtf((source->w * source->w) + (source->h * source->h));
    newHeight = distance * sinf(atanf((float)source->h/(float)source->w) + newAngle);

    newWidth = (float)floor(newWidth + 0.5f);
    newHeight = (float)floor(newHeight + 0.5f);

    /*Finally create the new surface*/
    rotatedSurface = surf_Create((int)newWidth, (int)newHeight);

    pixel_Fill(0, 0, rotatedSurface->w, rotatedSurface->h, *ker_colourKey(), rotatedSurface);
    //pixel_Fill(0, 0, rotatedSurface->w, rotatedSurface->h, colourGreen, rotatedSurface);

    /*printf("Created surface (%d %d)\n", rotatedSurface->w, rotatedSurface->h);*/

    /*Obtain the middle position of the source image*/
    sOriginX = (source->w - 1)/2.0f;
    sOriginY = (source->h - 1)/2.0f;

    rOriginX = (rotatedSurface->w - 1)/2.0f;
    rOriginY = (rotatedSurface->h - 1)/2.0f;

    /*Lock the surfaces so their pixels can be written and read*/
    SDL_LockSurface(source);
    SDL_LockSurface(rotatedSurface);

    //printf("Rotating image (%d %d)(%.2f %.2f) into (%d %d)(%.2f %.2f)\n", source->w, source->h, sOriginX, sOriginY, rotatedSurface->w, rotatedSurface->h, rOriginX, rOriginY);
    cosA = cosf(angle);
    sinA = sinf(angle);

    /*For all the pixels in the rotated surface*/
    for(x = 0; x < rotatedSurface->w; x++)
    {
        for(y = 0; y < rotatedSurface->h; y++)
        {
            /*Obtain the pixel to rotate back to the original image*/
            dPixel = pixel_Get(x, y, rotatedSurface);

            /*Obtain the distance from the middle of the surface to the current pixel*/
            xDis = (float)x - rOriginX;
            yDis = (float)y - rOriginY;

            /*Update the new x and y locations of the rotated pixel that would map to this
            pixel in the rotated surface*/
            dX = sOriginX + xDis * cosA - yDis * sinA;
            dY = sOriginY + xDis * sinA + yDis * cosA;

            odX = (float)floor(dX + 0.5f);
            odY = (float)floor(dY + 0.5f);

            /*Ignore pixels that map back outside the original surfaces canvas,
            these pixels have been created to just enlarge the rotated
            surfaces canvas*/
            if(odX < 0 || odY < 0)
            {
                /*printf("ErrorUnder:(%d %d): %f %f\n", x, y, odX, odY);
                printf("Extra info: cosA %.2f, sinA %.2f, dX %.2f, dY %.2f\n", cosA, sinA, dX, dY);*/
            }
            else if(odX >= source->w || odY >= source->h)
            {
                /*printf("ErrorOver:(%d %d): %f %f\n", x, y, odX, odY);
                printf("Extra info: cosA %.2f, sinA %.2f, dX %.2f, dY %.2f\n", cosA, sinA, dX, dY);*/
            }
            else
            {
                /*Obtain the pixel at the original surface*/
                sPixel = pixel_Get((int)odX, (int)odY, source);

                /*Give the colour of this pixel to the colour of the corresponding pixel that is
                mapped onto the rotated surface*/
                *dPixel = *sPixel;
            }
        }
    }

    SDL_UnlockSurface(source);
    SDL_UnlockSurface(rotatedSurface);
    SDL_SetColorKey(rotatedSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(ker_Screen()->format, 255, 0, 255));

    if(autoFree == A_FREE)
		list_Push(&surf_C.surfaceList, rotatedSurface, autoFree);

    return rotatedSurface;
}

