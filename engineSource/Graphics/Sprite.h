#ifndef SPRITE_H
#define SPRITE_H

#include "SDL/SDL.h"

#include "../Time/Timer.h"
#include "Animation.h"
#include "../Vector2DInt.h"

/*extern Sprite_Effect;*/

typedef struct sprite
{
    unsigned short int layer; /*Layer to be drawn on*/
    unsigned short int totalFrames; /*Total amount of animation frames it holds*/

    int useDirectDraw; /*If true then the sprite will not use the draw manager, instead it will just draw directly on screen ignoring any layer information*/

    Timer frameTime; /*Timer to track animation changes*/

    struct list *frameList; /*Holds all the frames in the list as a reference*/
    struct list *frameListCurrent; /*Only points to the list that contains the current frame*/
    Frame *frameCurrent; /*Points to current frame*/
    int currentFrameIndex;

    int x; /*x location*/
    int y; /*y location*/

    SDL_Rect fitClip; /*Each frame must fit into this clip*/

    struct list *effectList; /*The effect/s to be applied to the sprite each time it is drawn*/
    struct sprite_Effect *currentEffect; /*The current effect that the sprite is updating*/

    unsigned int alpha; /*Transparency value of sprite*/
    unsigned int alphaOriginal; /*Original transparency value of sprite*/

    unsigned int local; /*Does the sprite need to be removed by its owner? 1 if it has been malloced.*/
} Sprite;

/*Sets up the sprite to configure its current frame*/
void sprite_ActivateFrame(Sprite *sprite);

/*Makes the current frame the sprite is at enabled (1) / disabled (0)*/
void sprite_EnableFrame(Sprite *sprite, int enable);

/*Force sprite to change to the next frame*/
void sprite_Next_Frame(Sprite *sprite);
void sprite_Previous_Frame(Sprite *sprite);

/*Set sprite to change to specific frame*/
void sprite_SetFrame(Sprite *sprite, int index);

/*Set sprite to change to specific frame for a certain amount of updates*/
void sprite_ForceFrame(Sprite *sprite, int index, int updateCount);

/*If the frameTime is larger than the delta time of the frame then change it to the next one*/
int sprite_Get_Frame(Sprite *sprite);

/*Draw the frame onto the destination surface, it will also check to see if it needs to change frame*/
void sprite_DrawAtPos(int x, int y, Sprite *sprite, SDL_Surface *destination);

void sprite_DrawAtPosFit(int x, int y, SDL_Rect *clip, Sprite *sprite, SDL_Surface *destination);

void sprite_Draw(Sprite *sprite,SDL_Surface *destination);

/*Set the position of the sprite*/
Sprite *sprite_SetPos(Sprite *sprite, int x, int y);

/*Change the position of the sprite*/
Sprite *sprite_ChangePos(Sprite *sprite, int xDis, int yDis);

/*Get position vector of sprite*/
Vector2DInt sprite_GetPos(Sprite *sprite);

/*Set the clip of the the current sprite's frame*/
void sprite_SetCurrentClip(Sprite *sprite, int x, int y, int w, int h);

/*Set up the sprite, inputting the frames last*/
Sprite *sprite_Setup(Sprite *sprite, int local, unsigned int layer, Timer *srcTimer, int numberOfFrames, ...);

/*This is called in the sprite_Setup function to set up the frames*/
void sprite_Frame_Setup(Sprite *sprite, Timer *srcTimer, unsigned int layer);

/*Add a frame to the sprite*/
void sprite_AddFrame(Sprite *sprite, Frame *frame);

/*Replace a frame at a certain position of the sprite*/
void sprite_ReplaceFrame(Sprite *sprite, Frame *frame, int framePos);

/*Remove a frame at a certain position in the sprite*/
void sprite_RemoveFrame(Sprite *sprite, int frameIndex);

/*Returns the frame located at a certain position in the sprite*/
Frame *sprite_RetrieveFrame(Sprite *sprite, int frameIndex);

/*Return the surface that the sprite is currently at*/
SDL_Surface *sprite_CurrentSurface(Sprite *sprite);

/*Free any dynamic memory used by the sprite*/
void sprite_Clean(Sprite *sprite);

/*Return width of the current frame that the sprite is at*/
unsigned int sprite_Width(Sprite *sprite);

/*Return height of the current frame that the sprite is at*/
unsigned int sprite_Height(Sprite *sprite);

unsigned int spriteFrame_MaxWidth(Sprite *sprite);

unsigned int spriteFrame_MaxHeight(Sprite *sprite);

/*Return the index of the current frame*/
int sprite_GetFrameIndex(Sprite *sprite);

/*Set the layer of the sprite*/
void sprite_SetLayer(Sprite *sprite, unsigned int layer);

/*Set alpha of all the sprite*/
void sprite_SetAlphaAll(Sprite *sprite, unsigned int alpha);

/*Set alpha of current sprite surface*/
void sprite_SetAlpha(Sprite *sprite, unsigned int alpha);

/*Remove all effects on sprite*/
void sprite_ClearEffects(Sprite *sprite);

/*Create a new sprite, must setup manually*/
Sprite *sprite_Create(void);

/*Create a copy of a sprite*/
Sprite *sprite_Copy(Sprite *sprite, unsigned int local);

#endif
