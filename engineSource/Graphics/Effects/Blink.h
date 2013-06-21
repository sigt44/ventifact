#ifndef BLINK_H
#define BLINK_H

#include "../Sprite.h"

#define SPE_BLINK_ARGS 3

/* Requires 3 arguments,
    1. The time when the graphic is not drawn.
    2. The time when the graphic is drawn.
    3. The amount of times the sprite will blink.
*/

void spriteSE_Blink(Sprite *sprite, int timeDrawn, int timeVanished, int numBlinks);

#endif
