#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL.h"

#include "../Vector2D.h"
#include "Pixel.h"

#define SDL_FAST_ALPHA 128

void fill_Rect(int x, int y, int w, int h, Uint16P *colour, SDL_Surface *dest);
void fill_Rect32(int x, int y, int w, int h,Uint32 *colour,SDL_Surface *dest);

void draw_Line(int sX, int sY, int eX, int eY, int size, int quality, Uint16P *colour, SDL_Surface *surface);
void draw_Line_Vector(Vector2D *A, Vector2D *B, int size, int quality, Uint16P *colour, SDL_Surface *surface);
void draw_Box(int x, int y, int width, int height, int lineWidth, Uint16P *colour, SDL_Surface *surface);
void draw_Triangle(int fX, int fY, int sX, int sY, int tX, int tY, int lineWidth, Uint16P *colour, SDL_Surface *surface);

#endif
