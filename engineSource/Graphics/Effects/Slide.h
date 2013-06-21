#ifndef SLIDE_H
#define SLIDE_H
#include "../Sprite.h"

#define SPE_SLIDE_ARGS 4

/* Requires 4 arguments,
    1. The speed of movement per second.
    2. The direction of movement in degrees.
    3. The distance.
    4. Should it reset back to its starting position once finished? (1 = yes, 0 = no)
*/

void spriteSE_Slide(Sprite *sprite, int speed, int angle, int distance, int resetOnCompletion);

#endif
