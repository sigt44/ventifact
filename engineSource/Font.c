#include "Font.h"

#include <SDL/SDL_ttf.h>

#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Graphics/Graphics.h"
#include "Graphics/SurfaceManager.h"

struct font_Manager fontManager;

SDL_Colour tColourLightBlue = {0, 175, 255, 0};
SDL_Colour tColourBlack = {0,0,0,0};
SDL_Colour tColourWhite = {255, 255, 255, 0};
SDL_Colour tColourRed = {255, 0, 0, 0};
SDL_Colour tColourGreen = {0, 255, 0, 0};

int font_Init(const char *rootDIR)
{
    fontManager.fontDIR = (char *)mem_Malloc(256, __LINE__, __FILE__);
    strncpy(fontManager.fontDIR, rootDIR, 255);

    fontManager.fontList = NULL;

	return 0;
}

int font_Load(char *name, int size, int ID)
{
    Font_Info *fontInfo = (Font_Info *)mem_Malloc(sizeof(Font_Info), __LINE__, __FILE__);

    TTF_Font *font = NULL;
	char fontDIR[384];

	strncpy(fontDIR, fontManager.fontDIR, 255);
	strncat(fontDIR, name, 127);

	font = TTF_OpenFont(fontDIR,size);

	if(font == NULL)
	{
	    printf("Failed to load: %s\n", fontDIR);

	    mem_Free(fontInfo);

		return 1;
	}

	//TTF_SetFontKerning(font, 0);
	/*TTF_SetFontOutline(font, 0);*/

	//TTF_SetFontHinting(font, TTF_HINTING_NORMAL);

	fontInfo->name = name;
	fontInfo->font = font;
	fontInfo->ID = ID;
	fontInfo->size = size;

    if(font_Verify(name, ID) == 2)
    {
        printf("Warning: Clashing ID(%d) for fonts %s\n", ID, name);
    }

    list_Push(&fontManager.fontList, fontInfo, ID);

	return 0;
}

/*
    Function: font_Verify

    Description -
    Returns 1 if a specific font is in the list. 2 if the font ID is in the list, but
    not the name. Otherwise returns 0.

    2 arguments:
    char *name - The name that was given to the font when it was loaded.
    int ID - The ID of the font.
*/
int font_Verify(char *name, int ID)
{
    struct list *fontList = fontManager.fontList;
    Font_Info *fontInfo = NULL;

    while(fontList != NULL)
    {
        fontInfo = fontList->data;

        if(strcmp(name, fontInfo->name) == 0 && ID == fontInfo->ID)
        {
            return 1;
        }
        else if(strcmp(name, fontInfo->name) != 0 && ID == fontInfo->ID)
        {
            return 2;
        }

        fontList = fontList->next;
    }

    return 0;
}

/*
    Function: font_Get

    Description -
    Returns a specific font that has been loaded.

    2 arguments:
    int ID - The ID that was given to the font when it was loaded.
    int size - The size of the font. If -1 then any size.
*/
TTF_Font *font_Get(int ID, int size)
{
    struct list *fontList = fontManager.fontList;
    Font_Info *fontInfo = NULL;

    while(fontList != NULL)
    {
        fontInfo = fontList->data;

        if(fontInfo->ID == ID)
        {
            if(size == -1 || size == fontInfo->size)
                return fontInfo->font;
        }

        fontList = fontList->next;
    }

    printf("Warning: Unknown font with ID %d and size %d\n", ID, size);

    return NULL;
}

/*
    Function: font_GetName

    Description -
    Returns a specific font that has been loaded, by using the name of the font.

    2 arguments:
    char *name - The name that was given to the font when it was loaded.
    int size - The size of the font. If -1 then any size.
*/
TTF_Font *font_GetName(char *name, int size)
{
    struct list *fontList = fontManager.fontList;
    Font_Info *fontInfo = NULL;

    while(fontList != NULL)
    {
        fontInfo = fontList->data;

        if(strcmp(name, fontInfo->name) == 1)
        {
            if(size == -1 || size == fontInfo->size)
                return fontInfo->font;
        }

        fontList = fontList->next;
    }

    printf("Warning: Unknown font with name %s and size %d\n", name, size);

    return NULL;
}

int text_Draw(int x, int y,SDL_Surface *destination, TTF_Font *font, SDL_Color *colour, int layer, int time, char *text)
{
    SDL_Surface *tempText = TTF_RenderText_Solid(font,text, *colour);

	if(tempText == NULL)
		return 1;

    surfaceMan_Push(gKer_DrawManager(), A_FREE, layer, time, x, y, tempText, destination, NULL);

    /*surf_Blit(x,y,tempText,destination,NULL);*/

    /*SDL_FreeSurface(tempText);*/

    return 0;
}

SDL_Surface *text_Arg(TTF_Font *font, SDL_Color *colour, char *text, ...)
{
    char textBuffer[255];

    SDL_Surface *tempText = NULL;
    va_list args;

    va_start(args, text);

    vsnprintf(textBuffer, 254, text,args);

    va_end(args);

    tempText = TTF_RenderText_Solid(font, textBuffer, *colour);

	return tempText;
}

SDL_Surface *text_Blended_Arg(TTF_Font *font, SDL_Color *colour, char *text, ...)
{
    char textBuffer[255];

    SDL_Surface *tempText = NULL;
    va_list args;

    va_start(args, text);

    vsnprintf(textBuffer, 254, text, args);

    va_end(args);

    tempText = TTF_RenderText_Blended(font, textBuffer, *colour);

	return tempText;
}



int text_Draw_Arg(int x, int y, SDL_Surface *destination, TTF_Font *font, SDL_Color *colour, int layer, int time, char *text, ...)
{
    char textBuffer[255];

    SDL_Surface *tempText = NULL;
    va_list args;

    va_start(args, text); /* Get the type of arguments needed from reading the text passed to the function.*/

    vsnprintf(textBuffer,254, text, args);

    va_end(args); /*Always call this in the same function as va_start.*/

    tempText = TTF_RenderText_Solid(font, textBuffer,*colour);

	if(tempText == NULL)
		return 1;

    surfaceMan_Push(gKer_DrawManager(), A_FREE, layer, time, x, y, tempText, destination, NULL);
    /*surf_Blit(x,y,tempText,destination,NULL);*/

    /*SDL_FreeSurface(tempText);*/

    return 0;
}

int text_Draw_Blended_Arg(int x, int y,SDL_Surface *destination,TTF_Font *font,SDL_Color *colour, int layer, int time, char *text, ...)
{
    char textBuffer[255];

    SDL_Surface *tempText = NULL;
    va_list args;

    va_start(args, text); /* Get the type of arguments needed from reading the text passed to the function. ?*/

    vsnprintf(textBuffer,254,text,args);

    va_end(args); /*Always call this in the same function as va_start.*/

    tempText = TTF_RenderText_Blended(font,textBuffer,*colour);

	if(tempText == NULL)
		return 1;

    #ifdef DINGOO
    //x--;
    #endif

    #ifdef GP2X
    //y-=2;
    #endif

    surfaceMan_Push(gKer_DrawManager(), A_FREE, layer, time, x, y, tempText, destination, NULL);
    /*surf_Blit(x,y,tempText,destination,NULL);*/

    /*SDL_FreeSurface(tempText);*/

    return 0;
}

int font_Quit(void)
{
    Font_Info *fontInfo = NULL;

	while(fontManager.fontList != NULL)
	{
		fontInfo = fontManager.fontList->data;

		if(fontInfo->font != NULL)
			TTF_CloseFont(fontInfo->font);

        mem_Free(fontInfo);

		list_Pop(&fontManager.fontList);
	}

	mem_Free(fontManager.fontDIR);

	return 0;
}
