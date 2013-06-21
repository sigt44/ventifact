#ifndef ANIMATION_H
#define ANIMATION_H

#include "SDL/SDL.h"

#include "../Common/List.h"

typedef struct anim_Frame
{
    int deltaTime; /*Time between frame*/
    int updateCount; /*Max amount of update this should be drawn*/

    SDL_Surface *graphic; /*Pointer to surface*/
    unsigned int freeSurface; /*A_FREE if the surface should be released once the frame is deleted*/

    int enabled; /*Should the frame be drawn*/

    SDL_Rect clip; /*Only the surface that is inside this rectangle will be displayed*/
    int offsetX; /*Offset of the frame from the sprite*/
    int offsetY;
} Frame;

/*Force the next frame*/
void frame_Next(struct list **frame,struct list *frameStart);
void frame_Previous(struct list **frame,struct list *frameStart);

/*Reset to the starting frame*/
void frame_Reset(struct list **frame,struct list *frameStart);

/*Creates new frame*/
Frame *frame_CreateEmpty(void);
Frame *frame_CreateBasic(unsigned int holdTime, SDL_Surface *frameSurface, unsigned int freeSurface);
Frame *frame_CreateClip(unsigned int holdTime, SDL_Surface *frameSurface, int oX, int oY, int oW, int oH, unsigned int freeSurface);
Frame *frame_CreateOffset(unsigned int holdTime, SDL_Surface *frameSurface, int ofX, int ofY, unsigned int freeSurface);
Frame *frame_Create(unsigned int holdTime, SDL_Surface *frameSurface, int ofX, int ofY, int oX, int oY, int oW, int oH, unsigned int freeSurface);

/*Setup frame*/
void frame_Setup(Frame *frame, unsigned int holdTime, SDL_Surface *frameSurface, int ofX, int ofY, int oX, int oY, int oW, int oH, unsigned int freeSurface);

/*Writes the details of the frame to file*/
void frame_Report(struct list *frameStart);

/*Cleans and deletes all the frames in a list.*/
void frame_CleanList(struct list **frameStart);

/*Clean up a frame*/
void frame_Clean(Frame *frame);

unsigned int frame_Width(Frame *frame);

unsigned int frame_Height(Frame *frame);

unsigned int frame_MaxWidth(Frame *frame);

unsigned int frame_MaxHeight(Frame *frame);

void frame_SetClipToGraphic(Frame *frame);

void frame_CorrectClip(Frame *frame);

/*Set the graphic to be in the middle of the frame's clip*/
void frame_SetMiddle(Frame *frame, int w, int h);


#endif
