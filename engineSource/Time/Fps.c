#include "SDL/SDL.h"
#include "Fps.h"

#include "../Kernel/Kernel.h"

float fps_Calc_Now(Timer *startTime)
{
    float div = (float)timer_Get_Passed(startTime, 1);

    if(div != 0.0f)
        return (1000.0f/div);
    else
        return 0.0f;
}

void fps_Cap(Timer *frameTime, unsigned int frameRateMS)
{
    while(timer_Get_Passed(frameTime, 1) < frameRateMS)
    {
        SDL_Delay(1);
    }

    return;
}
