#ifndef PIXEL_H
#define PIXEL_H

#include <SDL/SDL.h>

#ifdef SCREEN_BIT_32
#define Uint16P Uint32
#define BYTES_PER_PIXEL 4
#define BYTES_SHIFT_PIXEL 2

extern Uint32 colourWhite;
extern Uint32 colourBlack;
extern Uint32 colourLightBlue;
extern Uint32 colourLightGreen;
extern Uint32 colourYellow;
extern Uint32 colourRed;
extern Uint32 colourGreen;
extern Uint32 colourGrey;
#else
#define Uint16P Uint16
#define BYTES_PER_PIXEL 2
#define BYTES_SHIFT_PIXEL 1

extern Uint16 colourWhite;
extern Uint16 colourBlack;
extern Uint16 colourLightBlue;
extern Uint16 colourLightGreen;
extern Uint16 colourYellow;
extern Uint16 colourRed;
extern Uint16 colourGreen;
extern Uint16 colourGrey;
#endif

Uint16P *pixel_Get(int x, int y, SDL_Surface *surface);
void pixel_Fill(int x, int y, int width, int height, Uint16P colour, SDL_Surface *surface);

void pixel_Line(int sX, int sY, int eX, int eY, int thickness, int quality, Uint16P *colour, SDL_Surface *surface);
void pixel_Box(int x, int y, int width, int height, int thickness, Uint16P *colour, SDL_Surface *surface);
void pixel_Triangle(int fX, int fY, int sX, int sY, int tX, int tY, int thickness, Uint16P *colour, SDL_Surface *surface);

#endif
