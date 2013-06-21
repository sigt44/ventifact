#ifndef TIME_H
#define TIME_H

typedef struct timer
{
    int started; /*boolean to say whether the timer has been started or not*/
	unsigned int start_Time; /* The starting time in milliseconds */
	int stop_Time; /* How long the timer is stopped for */
	unsigned int current_Time; /* Holds the time that has passed since the start time was set */
	unsigned int base_Time; /* Holds the how long the timer has been going */
	unsigned int end_Time; /* The time when it stops based on current time */
	unsigned int pause; /* Is the timer paused or not */
	unsigned int skip; /*Should the timer skip an update, making the time difference 0*/
	struct timer *srcTime; /* The timer used to calculate the base_Time variable, if NULL then it will use SDL_GetTicks() to receive the time */
} Timer;

/* Returns the starting time */
unsigned int timer_Get_Start(Timer *timer);

/* Returns the amount of time passed since the timer was started */
unsigned int timer_Get_Passed(Timer *timer,int calc);

/* Returns the time that the timer should be running for*/
unsigned int timer_Get_End(Timer *timer);

/* Returns the amount of time remaining until the timer ends */
int timer_Get_Remain(Timer *timer);

/* Returns how long the timer is stopped for, always positive if paused*/
int timer_Get_Stopped(Timer *timer);

/* Is the timer paused?*/
int timer_Get_Paused(Timer *timer);

/* Stop the timer for amount specified in milliseconds */
void timer_Stop(Timer *timer, int amount);

/* Make the timer skip an update*/
void timer_Skip(Timer *timer, int amount);

/* Make the timer stop and skip*/
void timer_StopSkip(Timer *timer, int stopAmount, int skipAmount);

/* Pause timer */
void timer_Pause(Timer *timer);

/* Unpause timer */
void timer_Resume(Timer *timer);

/* Restart the timer, same as timer_Start*/
void timer_Restart(Timer *timer);

/* Start the timer (Or reset it) */
void timer_Start(Timer *timer);

/* Setup the timer */
Timer timer_Setup(Timer *sourceTime, int delay, unsigned int endTime, unsigned int start);

/* Calculate the current time of the timer */
int timer_Calc(Timer *timer);

#endif
