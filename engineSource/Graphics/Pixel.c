#include "Pixel.h"

#ifdef SCREEN_BIT_32
Uint32 colourWhite = 0x0;
Uint32 colourBlack = 0x0;
Uint32 colourLightBlue = 0x0;
Uint32 colourLightGreen = 0x0;
Uint32 colourYellow = 0x0;
Uint32 colourRed = 0x0;
Uint32 colourGreen = 0x0;
Uint32 colourGrey = 0x0;
Uint32 colourPinkT = 0x0;
#else
Uint16 colourWhite = 0x0;
Uint16 colourBlack = 0x0;
Uint16 colourLightBlue = 0x0;
Uint16 colourLightGreen = 0x0;
Uint16 colourYellow = 0x0;
Uint16 colourRed = 0x0;
Uint16 colourGreen = 0x0;
Uint16 colourGrey = 0x0;
Uint16 colourPinkT = 0x0;
#endif

/*
    Function: pixel_Get

    Description -
    Returns the pixel colour data at position (x, y) of a surface.

    3 arguments:
    int x - The x-axis location of the pixel.
    int y - The y-axis location of the pixel.
    SDL_Surface *surface - The surface to get the pixel from.
*/
Uint16P *pixel_Get(int x, int y, SDL_Surface *surface)
{
	Uint16P *pixels = (Uint16P *)((char *)surface->pixels + (y * surface->pitch) + (x << BYTES_SHIFT_PIXEL));

	return pixels;
}

/*
    Function: pixel_Fill

    Descrition -
    Fills the pixels of a surface with a certain colour.

    6 arguments:
    int x - The starting x-axis location of the pixels to fill.
    int y - The starting y-axis location of the pixels to fill.
    int width - The width of the pixels to fill.
    int height - The height of the pixels to fill.
    Uint16P colour - The colour to fill the pixels.
    SDL_Surface *surface - The surface containing the pixels.
*/
void pixel_Fill(int x, int y, int width, int height, Uint16P colour, SDL_Surface *surface)
{
    Uint16P *pixels = NULL;

    int xWidth = x + width;
    int yHeight = y + height;
    int startX = x;
    int yLoc = 0;

    /*Iterate through all the pixels and fill them*/
    for(y = y; y < yHeight; y++)
    {
        yLoc = y * surface->pitch;

        for(x = startX; x < xWidth; x++)
        {
            pixels = (Uint16P *)((char *)surface->pixels + yLoc + (x << BYTES_SHIFT_PIXEL));
            *pixels = colour;
        }
    }

    return;
}

/*
    Function: pixel_Line

    Descrition -
    Fills a line of pixels of a surface with a certain colour.

    8 arguments:
    int sX - The starting x-axis point of the line segment.
    int sY - The starting y-axis point of the line segment.
    int eX - The ending x-axis point of the line segment.
    int eY - The ending y-axis point of the line segment.
    int thickness - The thickness of the line.
    int quality - If > 0 the line will redraw every pixel along the line.
    Uint16P colour - The colour to fill the pixels.
    SDL_Surface *surface - The surface containing the pixels.
*/
void pixel_Line(int sX, int sY, int eX, int eY, int thickness, int quality, Uint16P *colour, SDL_Surface *surface)
{
    int dX, dY, xDirection, yDirection, i;
    int errorerm = 0;

    int pX, pY, pW, pH;

    int sizeAmount = 1;

    int overFillX = 0;
    int overFillY = 0;
    int dX2 = 0;
    int dY2 = 0;

    if(quality > 0)
        sizeAmount = 1;
    else
        sizeAmount = thickness;


    xDirection = dX = eX - sX; /*Get the distance from the x axis*/
    if (dX < 0)
    {
        dX = -dX;
    }

    yDirection = dY = eY - sY; /*Get the distance from the y axis*/

    if (dY < 0)
    {
        dY = -dY;
    }

    /*the horizontal direction of the line (moving left or right?)*/
    if (xDirection < 0)
    {
        xDirection = -sizeAmount;        /* Moving left */
    }
    else if (xDirection > 0)
    {
        xDirection = sizeAmount;         /* Moving right */
    }
    else
    {
        xDirection = 0;         /* vertical line! So no need to move*/
    }

    /*  the vertical direction of the line (moving up or down?) */
    if (yDirection < 0)
    {
        yDirection = -sizeAmount;      /* Subtract height to move up the line */
    }
    else if (yDirection > 0)
    {
        yDirection = sizeAmount;    /* Add the height to move down the line */
    }
    else
    {

        yDirection = 0;
    }

    pX = sX;
    pY = sY;
    pW = pH = thickness;

    dX2 = dX+dX;
    dY2 = dY+dY;

    if(dX > dY)
    {
        errorerm = dY2 - dX;
        /* Line is more horizontal than vertical (x>y) */
        for (i = 0; i < dX; i+= sizeAmount)
        {
            /* Draw this pixel*/
            overFillX = surface->w - (pX + pW);
            overFillY = surface->h - (pY + pH);

            if(overFillX < 0)
            {
                pW += overFillX;
            }
            if(overFillY < 0)
            {
                pH += overFillY;
            }

            if(pY >= 0 && pX >= 0 && pW > 0 && pH > 0)
            {
                pixel_Fill(pX, pY, pW, pH, *colour, surface);
            }

            pH = pW = thickness;

            /* Move the pixelOffset 1 pixel left or right. (x)*/
            pX += xDirection;
            /* Time to move pixelOffset up or down? (y)*/

            if (errorerm >= 0)
            {
                /* Yes. Reset the error term */
                errorerm -= dX2;
                /* And move the pixelOffset 1 pixel up or down */
               pY += yDirection;
            }
            errorerm += dY2;
        }
    }
    else
    {
        /* Line is more vertical than horizontal: (y>x).*/
        errorerm = dX2 - dY;
        for (i = 0; i < dY; i+= sizeAmount)
        {
            /* Draw this pixel*/
            overFillX = surface->w - (pX + pW);
            overFillY = surface->h - (pY + pH);

            if(overFillX < 0)
            {
                pW += overFillX;
            }
            if(overFillY < 0)
            {
                pH += overFillY;
            }

            if(pY >= 0 && pX >= 0 && pW > 0 && pH > 0)
            {
                pixel_Fill(pX, pY, pW, pH, *colour, surface);
            }

            pH = pW = thickness;

            /*Move pixelOffset up or down. (y)*/
            pY += yDirection;
            /*Time to move pixelOffset left or right? (x)*/

            if (errorerm >= 0)
            {
                /* Yes. Reset the error term */
                errorerm -= dY2;
                /* And move the pixelOffset 1 pixel left or right */
                pX += xDirection;
            }

            errorerm += dX2;
        }

    }

    return;
}

void pixel_Box(int x, int y, int width, int height, int thickness, Uint16P *colour, SDL_Surface *surface)
{
    pixel_Line(x, y, x, y + height, thickness, 0, colour, surface); /* Draw line downwards*/
    pixel_Line(x, y, x+width,y, thickness, 0, colour, surface); /* Across*/
    pixel_Line(x+width,y, x+width, y+height, thickness, 0, colour, surface); /* Right down*/
    pixel_Line(x, y+height, x+width+thickness, y+height, thickness, 0, colour, surface); /* Down across*/

    return;
}

void pixel_Triangle(int fX, int fY, int sX, int sY, int tX, int tY, int thickness, Uint16P *colour, SDL_Surface *surface)
{
    pixel_Line(fX, fY, sX, sY, thickness, 0, colour, surface); /*First point to second point*/
    pixel_Line(sX, sY, tX, tY, thickness, 0, colour, surface); /*Second point to third point*/
    pixel_Line(tX, tY, fX, fY, thickness, 0, colour, surface); /*Third point to first point*/

    return;
}


