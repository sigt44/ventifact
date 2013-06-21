#ifndef FPS_H
#define FPS_H

#include "Timer.h"

/* Return the rate based on the delta time of a timer */
float fps_Calc_Now(Timer *startTime);

/* Do nothing until timer has passed rate */
void fps_Cap(Timer *Time,unsigned int RateMS);

#endif
