#include "SDL/SDL.h"
//#define USE_C_TIMER
#define USE_SDL_TIMER

#ifdef USE_C_TIMER
#include <time.h>

#define CLOCK_MS_MULTI 1000/CLOCKS_PER_SEC

#endif

#include "Timer.h"
#include "../Kernel/Kernel.h"
#include "../File.h"

static unsigned int timer_Base_Calc(void)
{
    #ifdef USE_SDL_TIMER
    return SDL_GetTicks();
    #else

    #ifdef USE_C_TIMER
    clock_t baseTime = clock()/1000;

    printf("CLOCKS_PER_SEC %d:: %d :: %d\n", CLOCKS_PER_SEC, baseTime, (unsigned int)(baseTime/1000));
    return (unsigned int)baseTime;
    #endif

    #endif

    printf("Error no timer specified");
    return 1;
}

unsigned int timer_Get_Start(Timer *timer)
{
	return timer->start_Time;
}

unsigned int timer_Get_Passed(Timer *timer,int calc)
{
	if(calc)
		timer_Calc(timer);

	return timer->current_Time;
}

unsigned int timer_Get_End(Timer *timer)
{
    return timer->end_Time;
}

int timer_Get_Remain(Timer *timer)
{
	return timer->end_Time - timer->current_Time;
}

int timer_Get_Stopped(Timer *timer)
{
    if(timer->pause == 1)
        return timer->pause;

	return timer->stop_Time;
}

int timer_Get_Paused(Timer *timer)
{
	return timer->pause;
}

void timer_Stop(Timer *timer, int amount)
{
    /*If the timer has not been calculated recently then the stop time may be affected*/
	timer_Calc(timer);

	timer->stop_Time = amount;

	/*if(timer->end_Time != timer->start_Time) There is no need for this anymore since current time effectivly freezes, thus end time dosnt need to change
		timer->end_Time += amount;*/

	return;
}

/*
    Function: timer_Skip

    Description -
    This will make the timer ignore any time differences between a certain amount of
    frames.

    2 arguments:
    Timer *timer - The timer to make skip.
    int amount - The amount of frames/updates to skip.
*/
void timer_Skip(Timer *timer, int amount)
{
    timer->skip = amount;

    return;
}

/*
    Function: timer_StopSkip

    Description -
    This will make the timer ignore any time differences between a certain amount of
    frames. It will also stop the timer for a specified amount.

    So for the timer to resume it must first ignore a certain amount of update frames
    then wait until stopAmount time has passed.

    Useful for delaying a timer after it returns into scope.

    3 arguments:
    Timer *timer - The timer to skip and stop.
    int stopAmount - The amount of time to stop the timer after it has skipped the frames.
    int skipAmount - The amount of frames/updates to skip.
*/
void timer_StopSkip(Timer *timer, int stopAmount, int skipAmount)
{
    timer_Stop(timer, stopAmount);
    timer_Skip(timer, skipAmount);

    return;
}

Timer timer_Setup(Timer *sourceTime, int delay, unsigned int endTime, unsigned int start)
{
    Timer t;

    t.started = 0;
	t.stop_Time = delay;

	t.srcTime = sourceTime;

	t.end_Time = endTime;

	t.pause = 0;

	t.skip = 0;

	t.base_Time = 0;
	t.start_Time = 0;
	t.current_Time = 0;

	if(start > 0)
	{
	    if(sourceTime != NULL && sourceTime->started != 1)
	    {
	        printf("Warning: sourceTime %p for timer has not started yet %d\n", sourceTime, sourceTime->started);
	    }

        timer_Start(&t);
	}

	return t;
}

void timer_Restart(Timer *timer)
{
    timer_Start(timer);

    return;
}

void timer_Start(Timer *timer)
{
    timer->current_Time = 0;
    timer->started = 1;

    if(timer->srcTime == NULL)
        timer->base_Time = timer->start_Time = timer_Base_Calc();
    else
        timer->base_Time = timer->start_Time = timer->srcTime->current_Time;

	return;
}

int timer_Calc(Timer *timer)
{
    unsigned int timeDifference = 0; /*Time passed since last called*/
    unsigned int srcBaseTime = 0;

    if(timer == NULL)
        return 0;
    else if(timer->started != 1)
    {
        file_Log(ker_Log(), P_OUT, "Warning: [%p] Timer has not been started %d\n", timer, timer->started);
        timer_Start(timer);
        return 0;
    }

    if(timer->srcTime == NULL)
    {
        srcBaseTime = timer_Base_Calc();
        timeDifference = srcBaseTime - timer->base_Time; /*Find time difference based on SDL timer.*/
        if(srcBaseTime < timer->base_Time) /*Check for error*/
        {
            timer->base_Time = timer_Base_Calc();
            timeDifference = 0;
        }
    }
    else
    {
        /*timer_Calc(timer->srcTime);*/
        timeDifference = timer->srcTime->current_Time - timer->base_Time; /*Find time difference based off another timer.*/

        if(timer->srcTime->current_Time < timer->base_Time)
        {
            timer->base_Time = timer->srcTime->current_Time;
            timeDifference = 0;
        }
    }


    timer->base_Time += timeDifference; /*Update the amount of time passed since it was first created */

    /*If the timer is skipping an update*/
    if(timer->skip > 0)
    {
        timeDifference = 0;
        timer->skip --;
    }

    if(timer->pause == 0)
    {
        if(timer->stop_Time > 0) /*If the timer is stopped then remove the amount of the time passed*/
        {
            timer->stop_Time -= timeDifference;
            if(timer->stop_Time < 0)
                timer->stop_Time = 0;
        }
        else if(timer->stop_Time < 0)
        {
            timer->current_Time += (-timer->stop_Time);
            timer->stop_Time = 0;
        }
        else /* Else the timer is ok to update*/
            timer->current_Time += timeDifference;
    }

	return timeDifference;
}

void timer_Pause(Timer *timer)
{
	timer->pause = 1;
}

void timer_Resume(Timer *timer)
{
    timer_Calc(timer); /* Need to recalculate the timer before it is un-paused, otherwise it will act as if it has never been paused. */

	timer->pause = 0;
}
