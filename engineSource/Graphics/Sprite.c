#include <stdarg.h>
#include "Sprite.h"

#include "Animation.h"
#include "Surface.h"
#include "SurfaceManager.h"
#include "SpriteEffects.h"
#include "Effects/FadeIn.h"
#include "../Kernel/Kernel.h"
#include "../Kernel/Kernel_State.h"
#include "../File.h"
#include "../Memory.h"
#include "../Collision.h"

void sprite_ActivateFrame(Sprite *sprite)
{
    /*Set the current frame pointer*/
    sprite->frameCurrent = sprite->frameListCurrent->data;

    /*Setup the frame timer*/
    sprite->frameTime = timer_Setup(sprite->frameTime.srcTime, 0, sprite->frameCurrent->deltaTime, 1);

    /*Set the correct alpha*/
    //sprite_SetAlpha(sprite, sprite->alpha);

    return;
}

void sprite_Next_Frame(Sprite *sprite)
{
    //sprite_SetAlpha(sprite, sprite->alphaOriginal);

    frame_Next(&sprite->frameListCurrent, sprite->frameList);

    sprite->currentFrameIndex ++;

    sprite_ActivateFrame(sprite);

    return;
}

void sprite_SetFrame(Sprite *sprite, int index)
{
    int x = 0;

    sprite->frameListCurrent = sprite->frameList;

    for(x = 0; x < index; x++)
    {
        frame_Next(&sprite->frameListCurrent, sprite->frameList);
    }

    sprite->currentFrameIndex = index;

    //sprite_SetAlpha(sprite, sprite->alphaOriginal);

    sprite_ActivateFrame(sprite);

    return;
}

void sprite_ForceFrame(Sprite *sprite, int index, int updateCount)
{
    sprite_SetFrame(sprite, index);

    sprite->frameCurrent->updateCount = updateCount;

    return;
}

void sprite_Previous_Frame(Sprite *sprite)
{
    //sprite_SetAlpha(sprite, sprite->alphaOriginal);

    frame_Previous(&sprite->frameListCurrent, sprite->frameList);

    sprite->currentFrameIndex --;

    sprite_ActivateFrame(sprite);

    return;
}

int sprite_Get_Frame(Sprite *sprite)
{
    Frame *currentFrame = sprite->frameCurrent;

    if(currentFrame->updateCount > 0)
    {
        currentFrame->updateCount --;

        if(currentFrame->updateCount == 0)
            return 1;
    }

    /*Frame change does not rely on time*/
    if(currentFrame->deltaTime == 0)
    {
        return 0;
    }

    timer_Calc(&sprite->frameTime);

    if(timer_Get_Remain(&sprite->frameTime) < 1)
    {
        return 1;
    }

    return 0;
}

SDL_Surface *sprite_CurrentSurface(Sprite *sprite)
{
    return sprite->frameCurrent->graphic;
}

void sprite_DrawAtPos(int x, int y, Sprite *sprite, SDL_Surface *destination)
{
    int oldX = sprite->x;
    int oldY = sprite->y;

    sprite->x = x;
    sprite->y = y;

    sprite_Draw(sprite, destination);

    sprite->x = oldX;
    sprite->y = oldY;

    return;
}

void sprite_DrawAtPosFit(int x, int y, SDL_Rect *clip, Sprite *sprite, SDL_Surface *destination)
{
    int oldX = sprite->x;
    int oldY = sprite->y;

    sprite->x = x;
    sprite->y = y;

    sprite->fitClip = *clip;

    sprite_Draw(sprite, destination);

    sprite->x = oldX;
    sprite->y = oldY;

    sprite->fitClip.w = 0;

    return;
}

void sprite_Draw(Sprite *sprite, SDL_Surface *destination)
{
    struct list *iterate = NULL;
	struct list *temp = NULL;
    struct list *frames = sprite->frameList;
    Frame *frame = NULL;

    int drawFrame = 0;
    SDL_Rect drawRect;
    SDL_Rect clip;

    while(frames != NULL)
    {
        frame = frames->data;

        /*If the frame is a global one*/
        if(frame->deltaTime < 0)
        {
            if(frame == sprite->frameCurrent)
            {
                sprite_Next_Frame(sprite);
            }

            drawFrame = 1;
        }
        else if(frame == sprite->frameCurrent)
        {
            /*Update all the effects on the sprite*/
            if(frame == sprite->frameCurrent)
            {
                iterate = (struct list *)sprite->effectList;
                while(iterate != NULL)
                {
					temp = iterate->next;
                    sprite->currentEffect = iterate->data;
                    sprite->currentEffect->update(sprite);
                    iterate = temp;
                }
				sprite->currentEffect = NULL;
            }

            drawFrame = 1;
        }

        if(drawFrame == 1 && destination != NULL && frame->enabled == 1)
        {
            /*Setup the rectangle representation of the frame*/
            drawRect.x = sprite->x + frame->offsetX;
            drawRect.y = sprite->y + frame->offsetY;
            drawRect.w = frame->clip.w;
            drawRect.h = frame->clip.h;

            /*If this frame needs to fit within a certain rectangle*/
            if(sprite->fitClip.w > 0)
            {
                /*First make sure it collides in the rectangle*/
                if(collisionStatic_Rectangle(&sprite->fitClip, &drawRect) == 1)
                {
                    /*Fit the frame within the required clip*/
                    clip = collisionFit_Rect(&sprite->fitClip, &drawRect);

                    /*If the x/y-axis of the clip has increased, you should also increase the position
                    of the frame by that amount. So that the position of the frame is to be inside of the
                    rectangle*/
                    drawRect.x += clip.x;
                    drawRect.y += clip.y;

                    clip.x += frame->clip.x;
                    clip.y += frame->clip.y;
                }
                else
                    drawFrame = 0;
            }
            else
            {
                clip = frame->clip;
            }

            if(drawFrame == 1)
            {
                if(sprite->useDirectDraw == 0)
                    surfaceMan_Push(gKer_DrawManager(), 0, sprite->layer, 0, drawRect.x, drawRect.y, frame->graphic, destination, &clip);
                else
                    surf_Blit(drawRect.x, drawRect.y, frame->graphic, destination, &clip);
            }
        }

        drawFrame = 0;

        frames = frames->next;
    }

    /*Check if current frames time is up*/
    if(sprite->frameCurrent != NULL)
    {
        if(sprite_Get_Frame(sprite) == 1)
        {
            sprite_Next_Frame(sprite);
        }
    }

    return;
}

void sprite_AddFrame(Sprite *sprite, Frame *frame)
{
    list_Stack(&sprite->frameList, frame, 0);

    if(sprite->frameListCurrent == NULL)
    {
        sprite->frameListCurrent = sprite->frameList;
        sprite->frameCurrent = frame;

        sprite->frameTime = timer_Setup(sprite->frameTime.srcTime, 0, sprite->frameCurrent->deltaTime,1);
    }

    sprite->totalFrames ++;

    return;
}

/*
    Function: sprite_ReplaceFrame

    Description -
    Replaces a frame in a sprite at given index.

    3 aguments:
    Sprite *sprite - the sprite to have its frame replaced.
    Frame *frame - The new frame to replace the old one.
    int frameIndex - The current position of the old frame in the sprite.
*/
void sprite_ReplaceFrame(Sprite *sprite, Frame *frame, int frameIndex)
{
    int x = 0;
    struct list *frameList = sprite->frameList;

    /*Find the frame to replace*/
    for(x = 0; x < frameIndex; x++)
    {
        frameList = frameList->next;
        if(frameList == NULL)
        {
            printf("Error: trying to replace sprite frame %d but reached NULL\n", frameIndex);
            return;
        }
    }

    /*Replace the frame*/
    if(sprite->frameCurrent == frameList->data)
        sprite->frameCurrent = frame;

    frame_Clean(frameList->data);
    mem_Free(frameList->data);

    frameList->data = frame;

    return;
}

/*
    Function: sprite_RetrieveFrame

    Description -
    Returns the frame in a sprite at given index.

    2 aguments:
    Sprite *sprite - the sprite that contains the frame.
    int frameIndex - The position of the frame to return.
*/
Frame *sprite_RetrieveFrame(Sprite *sprite, int frameIndex)
{
    int x = 0;
    struct list *frameList = sprite->frameList;

    for(x = 0; x < frameIndex; x++)
    {
        frameList = frameList->next;
    }

    return frameList->data;
}

/*
    Function: sprite_RemoveFrame

    Description -
    Remove a frame in a sprite at given index.

    3 aguments:
    Sprite *sprite - the sprite to have its frame removed.
    int frameIndex - The current position of the frame to remove in the sprite.
*/
void sprite_RemoveFrame(Sprite *sprite, int frameIndex)
{
    int x = 0;
    Frame *removedFrame = NULL;
    struct list *frameList = sprite->frameList;

    /*Find the frame to remove*/
    for(x = 0; x < frameIndex; x++)
    {
        frameList = frameList->next;
    }

    /*Replace the frame*/
    if(sprite->frameCurrent == frameList->data)
    {
        if(frameList->previous != NULL)
        {
            sprite->frameListCurrent = frameList->previous;
            sprite->frameCurrent = frameList->previous->data;
        }
        else
        {
            sprite->frameCurrent = NULL;
            sprite->frameListCurrent = NULL;
        }
    }

    frame_Clean(frameList->data);
    mem_Free(frameList->data);

    list_Delete_Node(&sprite->frameList, frameList);

    sprite->totalFrames --;

    return;
}

void sprite_EnableFrame(Sprite *sprite, int enable)
{
    if(sprite->frameCurrent != NULL)
    {
        sprite->frameCurrent->enabled = enable;
    }

    return;
}

Sprite *sprite_Create(void)
{
    Sprite *returnSprite = (Sprite *)mem_Malloc(sizeof(Sprite), __LINE__, __FILE__);

    return returnSprite;
}

Sprite *sprite_Copy(Sprite *sprite, unsigned int local)
{
	struct list *iterate = NULL;
	Frame *frame = NULL;
	Frame *cFrame = NULL;
    Sprite *returnSprite = (Sprite *)mem_Malloc(sizeof(Sprite), __LINE__, __FILE__);

	*returnSprite = *sprite;

	iterate = sprite->frameList;

    returnSprite->frameListCurrent = NULL;
    returnSprite->frameList = NULL;

	while(iterate != NULL)
	{
		frame = iterate->data;
        cFrame = frame_Create(frame->deltaTime, frame->graphic, frame->offsetX, frame->offsetY, frame->clip.x, frame->clip.y, frame->clip.w, frame->clip.h, frame->freeSurface);

		if(frame->freeSurface == A_FREE)
		{
			//printf("Warning: Copying a sprite with surfaces that are local to the source sprite\n");
			cFrame->graphic = surf_Copy(frame->graphic);
		}

		list_Stack(&returnSprite->frameList, cFrame, 0);

		iterate = iterate->next;
	}

	sprite_Frame_Setup(returnSprite, returnSprite->frameTime.srcTime, returnSprite->layer);

    returnSprite->effectList = NULL;
    returnSprite->currentEffect = NULL;

	if(local > 0)
        returnSprite->local = 1;
    else
        returnSprite->local = 0;

    return returnSprite;
}

Sprite *sprite_Setup(Sprite *sprite, int local, unsigned int layer, Timer *srcTimer, int numberOfFrames, ...)
{
    int x;
    va_list args;
    Frame *frameCurrent = NULL;

    va_start(args, numberOfFrames); /* Get the number of arguments needed from reading numberOfFrames. ?*/

    sprite->frameList = sprite->frameListCurrent =  NULL;
    sprite->x = 0;
    sprite->y = 0;

    sprite->fitClip.x = 0;
    sprite->fitClip.y = 0;
    sprite->fitClip.w = 0;
    sprite->fitClip.h = 0;

    sprite->local = local;

    sprite->totalFrames = numberOfFrames;

    sprite->useDirectDraw = 0;

    sprite->effectList = NULL;
    sprite->currentEffect = NULL;
    sprite->alphaOriginal = sprite->alpha = SDL_ALPHA_OPAQUE;

    for(x = 0; x < numberOfFrames; x++)
    {
        frameCurrent = va_arg( args, Frame *);
        list_Stack(&sprite->frameList, frameCurrent, 0);
    }

    va_end(args); /*Always call this in the same function as va_start.*/

    sprite_Frame_Setup(sprite, srcTimer, layer);

    return sprite;
}

void sprite_Frame_Setup(Sprite *sprite, Timer *srcTimer, unsigned int layer)
{
    sprite->frameListCurrent = sprite->frameList;
    sprite->layer = layer;
    sprite->currentFrameIndex = 0;

    if(sprite->frameList == NULL)
    {
        sprite->frameCurrent = NULL;
        sprite->frameTime = timer_Setup(srcTimer, 0, 0, 1);
    }
    else
    {
        sprite->frameCurrent = sprite->frameList->data;
        sprite->frameTime = timer_Setup(srcTimer, 0, sprite->frameCurrent->deltaTime, 1);
    }

    return;
}

Sprite *sprite_SetPos(Sprite *sprite, int x, int y)
{
    sprite->x = x;
    sprite->y = y;

    return sprite;
}

Sprite *sprite_ChangePos(Sprite *sprite, int xDis, int yDis)
{
    sprite->x += xDis;
    sprite->y += yDis;

    return sprite;
}

Vector2DInt sprite_GetPos(Sprite *sprite)
{
    Vector2DInt pos = {sprite->x, sprite->y};

    return pos;
}

void sprite_SetCurrentClip(Sprite *sprite, int x, int y, int w, int h)
{
    if(sprite->frameCurrent != 0)
    {
        sprite->frameCurrent->clip.x = x;
        sprite->frameCurrent->clip.y = y;
        sprite->frameCurrent->clip.w = w;
        sprite->frameCurrent->clip.h = h;

        frame_CorrectClip(sprite->frameCurrent);
    }

    return;
}

void sprite_SetAlphaAll(Sprite *sprite, unsigned int alpha)
{
    struct list *frameList = sprite->frameList;
    Frame *frame = NULL;

    sprite->alpha = alpha;

    while(frameList != NULL)
    {
        frame = frameList->data;

        if(frame->graphic != NULL)
        {
            if(alpha == SDL_ALPHA_OPAQUE)
                SDL_SetAlpha(frame->graphic, 0, alpha);
            else
                SDL_SetAlpha(frame->graphic, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
        }

        frameList = frameList->next;
    }

    return;
}

void sprite_SetAlpha(Sprite *sprite, unsigned int alpha)
{
    //sprite->alpha = alpha;

    if(sprite_CurrentSurface(sprite) != NULL)
    {
        if(alpha == SDL_ALPHA_OPAQUE)
            SDL_SetAlpha(sprite_CurrentSurface(sprite), 0, alpha);
        else
            SDL_SetAlpha(sprite_CurrentSurface(sprite), SDL_SRCALPHA | SDL_RLEACCEL, alpha);
    }

    return;
}

void sprite_Clean(Sprite *sprite)
{
    frame_CleanList(&sprite->frameList);

    sprite->frameList = sprite->frameListCurrent = NULL;
    sprite->frameCurrent = NULL;
    sprite->frameTime = timer_Setup(sprite->frameTime.srcTime,0,0,1);

    sprite_ClearEffects(sprite);

    return;
}

void sprite_ClearEffects(Sprite *sprite)
{
    while(sprite->effectList != NULL)
    {
        spriteEffect_Delete(sprite->effectList->data);
        list_Pop(&sprite->effectList);
    }
    return;
}

unsigned int sprite_Width(Sprite *sprite)
{
    if(sprite->frameCurrent != NULL)
        return frame_Width(sprite->frameCurrent);
    else
        return 0;
}

unsigned int sprite_Height(Sprite *sprite)
{
    if(sprite->frameCurrent != NULL)
        return frame_Height(sprite->frameCurrent);
    else
        return 0;
}

unsigned int spriteFrame_MaxWidth(Sprite *sprite)
{
    if(sprite->frameCurrent != NULL)
        return frame_MaxWidth(sprite->frameCurrent);
    return 0;
}

unsigned int spriteFrame_MaxHeight(Sprite *sprite)
{
    if(sprite->frameCurrent != NULL)
        return frame_MaxHeight(sprite->frameCurrent);
    else
        return 0;
}

int sprite_GetFrameIndex(Sprite *sprite)
{
    return sprite->currentFrameIndex;
}

void sprite_SetLayer(Sprite *sprite, unsigned int layer)
{
    sprite->layer = layer;
    return;
}
