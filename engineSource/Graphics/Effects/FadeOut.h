#ifndef FADEOUT_H
#define FADEOUT_H
#include "../Sprite.h"

#define SPE_FADEOUT_ARGS 2

/* Requires 2 arguments,
    1. The percentage decrease of alpha per second.
    2. Should the sprite reset its alpha value after completion. ( 1/0 = yes/no)
*/

void spriteSE_FadeOut(Sprite *sprite, int percentageLossPS, int resetOnCompletion);

#endif
