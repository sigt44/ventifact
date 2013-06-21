#ifndef FADEIN_H
#define FADEIN_H
#include "../Sprite.h"

#define SPE_FADEIN_ARGS 2

/* Requires 2 arguments,
    1. The percentage increase of alpha per second.
    2. Should the sprite reset its alpha value after completion. ( 1/0 = yes/no)
*/

void spriteSE_FadeIn(Sprite *sprite, int percentageIncreasePS, int resetOnCompletion);

#endif
