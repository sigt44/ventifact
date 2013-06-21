#ifndef FONT_H
#define FONT_H

#include <stdarg.h>
#include <string.h>

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "Common/List.h"
#include "Graphics/Surface.h"

enum fontSizes
{
    FS_SMALL = 0,
    FS_MEDIUM = 1,
    FS_LARGE = 2
};

typedef struct font_Info
{
    char *name;
    int size;

    int ID;

    TTF_Font *font;
} Font_Info;

struct font_Manager
{
    char *fontDIR;

    struct list *fontList; /*List of font_Info structures*/
};

extern SDL_Colour tColourLightBlue;
extern SDL_Colour tColourBlack;
extern SDL_Colour tColourWhite;
extern SDL_Colour tColourRed;
extern SDL_Colour tColourGreen;

int font_Init(const char *rootDIR);

int font_Load(char *name, int size, int ID);

TTF_Font *font_Get(int ID, int size);

int font_Verify(char *name, int ID);

int text_Draw(int x, int y,SDL_Surface *destination,TTF_Font *font,SDL_Color *colour, int layer, int time, char *text);

SDL_Surface *text_Arg(TTF_Font *font, SDL_Color *colour, char *text, ...);

SDL_Surface *text_Blended_Arg(TTF_Font *font, SDL_Color *colour, char *text, ...);

int text_Draw_Arg(int x, int y, SDL_Surface *destination, TTF_Font *font, SDL_Color *colour, int layer, int time, char *text, ...);

int text_Draw_Blended_Arg(int x, int y, SDL_Surface *destination,TTF_Font *font,SDL_Color *colour, int layer, int time, char *text, ...);

int font_Quit(void);
#endif




