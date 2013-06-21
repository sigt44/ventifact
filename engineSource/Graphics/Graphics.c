#include "Graphics.h"

#include "../Kernel/Kernel.h"
#include "../Kernel/Kernel_State.h"
#include "Surface.h"
#include "SurfaceManager.h"
#include "Pixel.h"
#include "PixelManager.h"

void fill_Rect(int x, int y, int w, int h, Uint16P *colour, SDL_Surface *dest)
{
	SDL_Rect fill;
	fill.x = x;
	fill.y = y;
	fill.w = w;
	fill.h = h;

	SDL_FillRect(dest, &fill, *colour);

	return;
}

void fill_Rect32(int x, int y, int w, int h,Uint32 *colour,SDL_Surface *dest)
{
	SDL_Rect fill;
	fill.x = x;
	fill.y = y;
	fill.w = w;
	fill.h = h;

	SDL_FillRect(dest,&fill,*colour);
	return;
}

void draw_Line(int sX, int sY, int eX, int eY, int size, int quality, Uint16P *colour, SDL_Surface *surface)
{
    int dX, dY, xDirection, yDirection, i;
    int errorerm = 0;
    /*int pixelOffset;*/
    SDL_Rect pixelFill = {0,0,0,0};
    Uint16P *pixelDraw = NULL;

    int sizeAmount = 1;

    int screenCapW = ker_Screen_Width() - size;
    int screenCapH = ker_Screen_Height()- size;
    int dX2 = 0;
    int dY2 = 0;

    int free = 0;
    int x = 0;
    int y = 0;

    pixelFill.h = pixelFill.w = size;

    if(quality > 0)
        sizeAmount = 1;
    else
        sizeAmount = size;


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

    pixelFill.x = sX;
    pixelFill.y = sY;
    dX2 = dX+dX;
    dY2 = dY+dY;

    if(dX > dY)
    {
        errorerm = dY2 - dX;
        /* Line is more horizontal than vertical (x>y) */
        for (i = 0; i < dX; i+= sizeAmount)
        {
            /* Draw this pixel*/
            if(pixelFill.x >= 0 && pixelFill.x <= screenCapW)
            {
                //pixel_Fill(pixelFill.x, pixelFill.y, pixelFill.w, pixelFill.h, *colour, surface);

                //pixelMan_Push(gKer_DrawManager(), 10, pixelFill.x, pixelFill.y, *colour, surface);

                pixelMan_Push(gKer_DrawManager(), 10, pixelFill.x, pixelFill.y, pixelFill.w, pixelFill.h, *colour, surface);
            }

            /* Move the pixelOffset 1 pixel left or right. (x)*/
            pixelFill.x += xDirection;
            /* Time to move pixelOffset up or down? (y)*/

            if (errorerm >= 0)
            {
                /* Yes. Reset the error term */
                errorerm -= dX2;
                /* And move the pixelOffset 1 pixel up or down */
               pixelFill.y += yDirection;
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
            /*Draw this pixel*/
            if(pixelFill.y >= 0 && pixelFill.y <= screenCapH)
            {
                pixelMan_Push(gKer_DrawManager(), 10, pixelFill.x, pixelFill.y, pixelFill.w, pixelFill.h, *colour, surface);
            }

            /*Move pixelOffset up or down. (y)*/
            pixelFill.y += yDirection;
            /*Time to move pixelOffset left or right? (x)*/

            if (errorerm >= 0)
            {
                /* Yes. Reset the error term */
                errorerm -= dY2;
                /* And move the pixelOffset 1 pixel left or right */
                pixelFill.x += xDirection;
            }

            errorerm += dX2;
        }

    }

    return;
}

void draw_Line_Vector(Vector2D *A, Vector2D *B, int size, int quality, Uint16P *colour, SDL_Surface *surface)
{
    draw_Line((int)A->x, (int)A->y, (int)B->x, (int)B->y, size, quality, colour, surface);
    return;
}

void draw_Box(int x, int y, int width, int height, int lineWidth, Uint16P *colour, SDL_Surface *surface)
{
    pixelMan_PushLine(gKer_DrawManager(), 100, x, y, x, y + height, lineWidth, *colour, surface); /* Draw left line*/
    pixelMan_PushLine(gKer_DrawManager(), 100, x, y, x+width, y, lineWidth, *colour, surface); /* Upper line*/
    pixelMan_PushLine(gKer_DrawManager(), 100, x+width, y, x+width, y+height, lineWidth, *colour, surface); /* Right line*/
    pixelMan_PushLine(gKer_DrawManager(), 100, x, y+height, x+width+lineWidth, y+height, lineWidth, *colour, surface); /* Lower line*/

    return;
}

void draw_Triangle(int fX, int fY, int sX, int sY, int tX, int tY, int lineWidth, Uint16P *colour, SDL_Surface *surface)
{
    draw_Line(fX, fY, sX, sY, lineWidth, 0, colour, surface); /*First point to second point*/
    draw_Line(sX, sY, tX, tY, lineWidth, 0, colour, surface); /*Second point to third point*/
    draw_Line(tX, tY, fX, fY, lineWidth, 0, colour, surface); /*Third point to first point*/

    return;
}



