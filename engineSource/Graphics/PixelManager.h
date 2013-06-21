#ifndef PIXELMANAGER_H
#define PIXELMANAGER_H
#include <SDL/SDL.h>

#include "Pixel.h"
#include "DrawManager.h"

#define MAX_PIXELS 5000

enum
{
    PM_RECT = 0,
    PM_LINE = 1,
    PM_CIRCLE = 2,
    PM_BOX = 3,
    PM_TRIANGLE = 4
} PM_SHAPETYPES;

typedef struct
{
    int shapeType; /*Type of shape*/
    Uint16P colour; /*Colour of the shape to draw*/

    int pixelThickness; /*How thichk the lines are in the shape*/

    union
    {
        struct
        {
            int x; /*x-axis location of the pixel to draw on the destSurface*/
            int y; /*y-axis location of the pixel to draw on the destSurface*/
            int w; /*number of pixels to draw horizontally*/
            int h; /*number of pixels to draw vertically*/
        } rect;

        struct
        {
            int sX; /*Start points of the line segment*/
            int sY;

            int eX; /*End points of the line segment*/
            int eY;
        } line;

        struct
        {
            int cX; /*position of the centre of the circle*/
            int cY;
            int radius; /*radius of the circle*/
        } circle;

    } shape;

} Pixel_DrawInfo;

typedef struct
{
    Draw_Manager dM;

} Pixel_Manager;

void pixelMan_Setup(Draw_Manager *dM);

void pixelMan_Push(Draw_Manager *dM, int layer, int x, int y, int w, int h, Uint16P colour, SDL_Surface *destSurface);

void pixelMan_PushLine(Draw_Manager *dM, int layer, int sX, int sY, int eX, int eY, int thickness, Uint16P colour, SDL_Surface *destSurface);

void pixelMan_DrawObject(Draw_Object *drawObj);

#endif
