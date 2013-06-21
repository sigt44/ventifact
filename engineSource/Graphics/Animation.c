#include <stdarg.h>
#include "Animation.h"

#include "Surface.h"
#include "../File.h"
#include "../Kernel/Kernel.h"
#include "../Common/List.h"

void frame_Next(struct list **frame, struct list *frameStart)
{
    *frame = (*frame)->next;

    if(*frame == NULL)
        *frame = frameStart;

}

void frame_Previous(struct list **frame, struct list *frameStart)
{
    *frame = (*frame)->previous;
}


void frame_Reset(struct list **frame, struct list *frameStart)
{
    *frame = frameStart;
}

Frame *frame_CreateEmpty(void)
{
    Frame *newFrame = frame_Create(0, NULL, 0, 0, 0, 0, 0, 0, M_FREE);

    return newFrame;
}

Frame *frame_CreateBasic(unsigned int holdTime, SDL_Surface *frameSurface, unsigned int freeSurface)
{
    Frame *newFrame = frame_Create(holdTime, frameSurface, 0, 0, 0, 0, 0, 0, freeSurface);

    return newFrame;
}

Frame *frame_CreateClip(unsigned int holdTime, SDL_Surface *frameSurface, int oX, int oY, int oW, int oH, unsigned int freeSurface)
{
    Frame *newFrame = frame_Create(holdTime, frameSurface, 0, 0, oX, oY, oW, oH, freeSurface);

    return newFrame;
}

Frame *frame_CreateOffset(unsigned int holdTime, SDL_Surface *frameSurface, int ofX, int ofY, unsigned int freeSurface)
{
    Frame *newFrame = frame_Create(holdTime, frameSurface, ofX, ofY, 0, 0, 0, 0, freeSurface);

    return newFrame;
}

Frame *frame_Create(unsigned int holdTime, SDL_Surface *frameSurface, int ofX, int ofY, int oX, int oY, int oW, int oH, unsigned int freeSurface)
{
    Frame *newFrame = (Frame *)mem_Malloc(sizeof(Frame), __LINE__, __FILE__);
    frame_Setup(newFrame, holdTime, frameSurface, ofX, ofY, oX, oY, oW, oH, freeSurface);

    return newFrame;
}

void frame_Report(struct list *frameStart)
{
    int x = 0;
    Frame *frameCurrent;

    file_Log(ker_Log(),0,"\nFrame report\n");

    while(frameStart != NULL)
    {
        frameCurrent = frameStart->data;

        file_Log(ker_Log(),0,"Frame %d: time %d, surface address %p. Clip = (%d %d %d %d)\n", x, frameCurrent->deltaTime, frameCurrent->graphic, frameCurrent->clip.x, frameCurrent->clip.y, frameCurrent->clip.w, frameCurrent->clip.h);
        x++;
        frameStart = frameStart->next;
    }

    return;
}

void frame_Setup(Frame *frame, unsigned int holdTime, SDL_Surface *frameSurface, int ofX, int ofY, int oX, int oY, int oW, int oH, unsigned int freeSurface)
{
    frame->deltaTime = holdTime;
    frame->graphic = frameSurface;
    frame->freeSurface = freeSurface;
    frame->updateCount = 0;

    frame->offsetX = ofX;
    frame->offsetY = ofY;

    frame->clip.w = oW;
    frame->clip.h = oH;
    frame->clip.x = oX;
    frame->clip.y = oY;

    if(frame->graphic != NULL)
    {
        frame->enabled = 1;
        frame_CorrectClip(frame);
    }
    else
    {
        if(frame->freeSurface == A_FREE)
        {
            printf("Warning NULL frame is set as auto free, changing to manual");
            frame->freeSurface = M_FREE;
        }

        frame->enabled = 0;
    }

    return;
}

void frame_CleanList(struct list **frameStart)
{
    while(*frameStart != NULL)
    {
        frame_Clean((*frameStart)->data);

        mem_Free((*frameStart)->data);

        list_Pop(frameStart);
    }

    return;
}

void frame_Clean(Frame *frame)
{
    if(frame->freeSurface == A_FREE)
        SDL_FreeSurface(frame->graphic);

    return;
}

unsigned int frame_Width(Frame *frame)
{
    if(frame->graphic != NULL)
        return frame->clip.w;
    else
        return 0;
}

unsigned int frame_Height(Frame *frame)
{
    if(frame->graphic != NULL)
        return frame->clip.h;
    else
        return 0;
}

unsigned int frame_MaxWidth(Frame *frame)
{
    if(frame->graphic != NULL)
        return frame->graphic->w;
    else
        return 0;
}

unsigned int frame_MaxHeight(Frame *frame)
{
    if(frame->graphic != NULL)
        return frame->graphic->h;
    else
        return 0;
}

void frame_CorrectClip(Frame *frame)
{
    if(frame->graphic != NULL)
    {
        if(frame->clip.w == 0)
            frame->clip.w = frame->graphic->w - frame->clip.x;
        else if(frame->clip.w > (frame->graphic->w - frame->clip.x))
        {
            //printf("Fixing width clip of %d to %d\n", frame->clip.w, (frame->graphic->w - frame->clip.x));
            frame->clip.w = (frame->graphic->w - frame->clip.x);
        }

        if(frame->clip.h == 0)
            frame->clip.h = frame->graphic->h - frame->clip.y;
        else if(frame->clip.h > (frame->graphic->h - frame->clip.y))
        {
            //printf("Fixing height clip of %d to %d\n", frame->clip.h, (frame->graphic->h - frame->clip.y));
            frame->clip.h = (frame->graphic->h - frame->clip.y);
        }
    }

    return;
}

void frame_SetClipToGraphic(Frame *frame)
{
    frame->clip.x = 0;
    frame->clip.y = 0;
    frame->clip.w = frame->graphic->w;
    frame->clip.h = frame->graphic->h;

    return;
}

/*
    Function: frame_SetMiddle

    Description -
    Sets the graphic of a frame to have its middle point in the middle point of the given boundry.
    The frame is offseted.

    1 argument:
    Frame *frame - The frame that contains the graphic.
*/
void frame_SetMiddle(Frame *frame, int w, int h)
{
    int middleX = 0;
    int middleY = 0;

    middleX = (w/2) - (frame->clip.w/2);
    middleY = (h/2) - (frame->clip.h/2);

    if(middleX < 0) /*If the graphic is too large to fit in the boundry*/
    {
        frame->offsetX = 0;
        frame->clip.x = abs(middleX);
        frame->clip.w = w;
    }
    else
    {
        frame->offsetX = middleX;
    }

    if(middleY < 0)
    {
        frame->offsetY = 0;
        frame->clip.y += abs(middleY);
        frame->clip.h = h;
    }
    else
    {
        frame->offsetY = middleY;
    }

    return;
}
