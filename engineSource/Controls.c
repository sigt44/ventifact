#include "Controls.h"

#include "Kernel/Kernel.h"

/*Create the main control data used by this file*/
static struct control_Component controlData;

/*
	Function: control_Init

	Description -
	This function initialises the main control structure so that control events
	can be used later in a program.
	Returns 0 upon completion.

	1 argument:
	Uint8 flags - The value given to flags will determine whether events will be
	checked for the mouse, keyboard or joystick.
	C_KEYBOARD, C_JOYSTICK, C_MOUSE can all be given as the value using the bitwise operators.
*/
int control_Init(Uint8 flags)
{
    controlData.flags = flags;

    if(flags & C_KEYBOARD)
    {
        controlData.keystates = SDL_GetKeyState(NULL); /*Point keystates to the array of keys referenced by a SDL key index*/
    }
    else
    {
        file_Log(ker_Log(),1,"\nKeyboard disabled\n");
        SDL_EventState(SDL_KEYUP, SDL_IGNORE);
        SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
        controlData.keystates = NULL;
    }

	/*If a joystick is asked for and SDL can detect an attached joystick*/
    if(flags & C_JOYSTICK && SDL_NumJoysticks())
    {
        controlData.joystick = SDL_JoystickOpen(0); /*Try and open the first available joystick*/

		if(controlData.joystick != NULL)
        	file_Log(ker_Log(),1,"\nJoystick detected and opened - %s\n", SDL_JoystickName(0));
		else
			file_Log(ker_Log(),1,"\nError: Joystick could not be opened\n");
    }
    else
    {
        file_Log(ker_Log(),1,"\nJoystick disabled or not found\n");
        controlData.joystick = NULL;
        SDL_EventState(SDL_JOYBUTTONUP, SDL_IGNORE);
        SDL_EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
    }

    if(flags & C_MOUSE)
    {

    }
    else
    {
        SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
        SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
        SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
        file_Log(ker_Log(),1,"\nMouse disabled\n");
    }

    controlData.pauseTimer = timer_Setup(NULL, 0, 0, 1);
    controlData.paused = 0;

    //control_PrintTypes();


    /*Ignore any states
     SDL_EventState(Uint8 type,SDL_IGNORE);*/

    return 0;
}

void control_PrintTypes(void)
{
    printf("Event type %d is SDL_ACTIVEEVENT\n", SDL_ACTIVEEVENT);
    printf("Event type %d is SDL_KEYDOWN\n", SDL_KEYDOWN);
    printf("Event type %d is SDL_KEYUP\n", SDL_KEYUP);
    printf("Event type %d is SDL_MOUSEMOTION\n", SDL_MOUSEMOTION);
    printf("Event type %d is SDL_MOUSEBUTTONDOWN\n", SDL_MOUSEBUTTONDOWN);
    printf("Event type %d is SDL_MOUSEBUTTONUP\n", SDL_MOUSEBUTTONUP);
    printf("Event type %d is SDL_JOYAXISMOTION\n", SDL_JOYAXISMOTION);
    printf("Event type %d is SDL_JOYBALLMOTION\n", SDL_JOYBALLMOTION);
    printf("Event type %d is SDL_JOYHATMOTION\n", SDL_JOYHATMOTION);
    printf("Event type %d is SDL_JOYBUTTONDOWN\n", SDL_JOYBUTTONDOWN);
    printf("Event type %d is SDL_JOYBUTTONUP\n", SDL_JOYBUTTONUP);
    printf("Event type %d is SDL_VIDEORESIZE\n", SDL_VIDEORESIZE);
    printf("Event type %d is SDL_VIDEOEXPOSE\n", SDL_VIDEOEXPOSE);
    printf("Event type %d is SDL_QUIT\n", SDL_QUIT);
    printf("Event type %d is SDL_USEREVENT\n", SDL_USEREVENT);
    printf("Event type %d is SDL_SYSWMEVENT\n", SDL_SYSWMEVENT);

    return;
}

/*
	Function: control_Quit

	Description -
	Cleans up the control component. Returns 0 on completion.
*/
int control_Quit(void)
{
    if(controlData.joystick != NULL)
        SDL_JoystickClose(controlData.joystick);

    return 0;
}

/*
	Function: control_SameEvent

	Description -
	Returns 1 if the two events supplied are the same state.

	2 arguments:
    SDL_Event *firstEvent - The first event to compare.
    SDL_Event *secondEvent - The second event to compare.
*/
int control_SameEvent(SDL_Event *firstEvent, SDL_Event *secondEvent)
{
    /*First make sure the event type is the same*/
    if(firstEvent->type == secondEvent->type)
    {
        /*Then make sure that the key is the same*/
        switch(firstEvent->type)
        {
            case SDL_KEYUP:
            case SDL_KEYDOWN:

            if(firstEvent->key.keysym.sym == secondEvent->key.keysym.sym)
            {
                return 1;
            }

            break;

            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:

            if(firstEvent->button.button == secondEvent->button.button)
            {
                return 1;
            }

            break;

            case SDL_JOYBUTTONUP:
            case SDL_JOYBUTTONDOWN:

            if(firstEvent->jbutton.button == secondEvent->jbutton.button)
            {
                return 1;
            }

            break;
        }
    }

    return 0;
}

SDL_Event *control_GetEventType(int eventType)
{
    int x = 0;

    for(x = 0; x < controlData.eventsPolled; x++)
    {
        if(controlData.c_Event[x].type == eventType || control_IsGeneralType(controlData.c_Event[x].type, eventType) == 1)
        {
            return &controlData.c_Event[x];
        }
    }

    return NULL;
}

/*
	Function: control_CopyEvent

	Description -
    Create a new SDL_Event and copy the contents of the perameter event to it.
    Return the new event.

	1 argument:
    SDL_Event *copyEvent - The event to be copied.
*/
SDL_Event *control_CopyEvent(SDL_Event *copyEvent)
{
    SDL_Event *newEvent = (SDL_Event *)mem_Malloc(sizeof(SDL_Event), __LINE__, __FILE__);

    *newEvent = *copyEvent;

    return newEvent;
}

/*
	Function: control_GetKeyValue

	Description -
    Returns the key symbol value of the event passed. If it is an
    event which does not contain one it returns -1.

	1 argument:
    SDL_Event *event - The event to get the key symbol value from.
*/
int control_GetKeyValue(SDL_Event *event)
{
    switch(event->type)
    {
        case SDL_KEYUP:
        case SDL_KEYDOWN:

        return event->key.keysym.sym;

        break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:

        return event->button.button;

        break;

        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:

        return event->jbutton.button;

        break;
    }

    return -1;
}

int control_GetSDLEventType(int pollType, int generalType)
{
    switch(generalType)
    {
        case C_BUTTON_DOWN:

        switch(pollType)
        {
            case C_KEYBOARD:

            return SDL_KEYDOWN;

            break;

            case C_JOYSTICK:

            return SDL_JOYBUTTONDOWN;

            break;

            case C_MOUSE:

            return SDL_MOUSEBUTTONDOWN;

            break;
        }

        break;

        case C_BUTTON_UP:

        switch(pollType)
        {
            case C_KEYBOARD:

            return SDL_KEYUP;

            break;

            case C_JOYSTICK:

            return SDL_JOYBUTTONUP;

            break;

            case C_MOUSE:

            return SDL_MOUSEBUTTONUP;

            break;
        }

        break;
    }

    return -1;
}

int control_GetPollType(int eventType)
{
    switch(eventType)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:

        return C_KEYBOARD;

        break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:

        return C_MOUSE;

        break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:

        return C_JOYSTICK;

        break;

        default:

        file_Log(ker_Log(), 1, "Unknown event type (%d) to convert to poll type.\n", eventType);

        break;
    }

    return -1;
}

/*
	Function: control_IsGeneralType

	Description -
    Returns 1 if the value of checkType is a member of the general type supplied by isType.

	2 argument:
    int checkType - The SDL type of an event.
    int isType - The general type of an event. (C_ANY_TYPE, C_BUTTON_DOWN, C_BUTTON_UP)
*/
int control_IsGeneralType(int checkType, int isType)
{
    switch(isType)
    {
        case C_ANY_TYPE:

        return 1;

        break;

        case C_BUTTON_DOWN:

        if(checkType == SDL_KEYDOWN || checkType == SDL_MOUSEBUTTONDOWN || checkType == SDL_JOYBUTTONDOWN)
        {
            return 1;
        }

        break;

        case C_BUTTON_UP:

        if(checkType == SDL_KEYUP || checkType == SDL_MOUSEBUTTONUP || checkType == SDL_JOYBUTTONUP)
        {
            return 1;
        }

        break;
    }

    return 0;
}

/*
	Function: control_TakeEvents

	Description -
    Returns a list of events that have been detected. (That will need to be released, since they are malloced)
    The events must match the given event type, unless it is -1 which will result in all events being placed in the list.

    If pollTime is > 0 then the events will be continually polled over that time.
    Otherwise only the last control update will be used.

	2 arguments:
    int eventType - Only push in events matching this type. (-1) for all events.
    unsigned int pollTime - Time to keep checking for new events.
*/
struct list *control_TakeEvents(int eventType, unsigned int pollTime)
{
    unsigned int timer = 0;
    struct list *eventList = NULL;
    struct list *checkList = NULL;

    int x = 0;

    timer = SDL_GetTicks();

    do
    {
        /*For the amount of events detected by SDL*/
        for(x = 0; x < controlData.eventsPolled; x++)
        {
            if(controlData.c_Event[x].type == eventType || control_IsGeneralType(controlData.c_Event[x].type, eventType) == 1)
            {
                /*Make sure the event is not already in the list*/
                checkList = eventList;
                while(checkList != NULL)
                {
                    if(control_SameEvent(checkList->data, &controlData.c_Event[x]) == 1)
                    {
                        break;
                    }

                    checkList = checkList->next;
                }

                /*If it isnt then add it in*/
                if(checkList == NULL)
                {
                    list_Stack(&eventList, control_CopyEvent(&controlData.c_Event[x]), controlData.c_Event[x].type);
                }
            }
        }

        /*If the pollTime is 0 then just base the checks on the last update*/
        if(pollTime > 0)
        {
            /*Otherwise keep checking for new events*/
            control_Update();
        }
    }
    while((SDL_GetTicks() - timer) < pollTime);

    return eventList;
}

/*
	Function: control_Pause

	Description -
	Pauses the control polling, so that no control detection will take place.

	1 argument:
	int pauseTime - The length of time that the control polling should be paused (milliseconds), if 0 then
	paused until the control_Resume function is called.
*/
void control_Pause(int pauseTime)
{
    controlData.pauseTimer = timer_Setup(NULL, 0, pauseTime, 1);

    controlData.paused = 1;

    return;
}

/*
	Function: control_Resume

	Description -
	Resumes the control polling.
*/
void control_Resume(void)
{
    controlData.paused = 0;

    controlData.pauseTimer = timer_Setup(NULL, 0, 0, 1);

    return;
}

/*
	Function: control_Update

	Description -
	Places the latest events detected by SDL into c_Event. Does not return a value.
*/
void control_Update(void)
{
    SDL_Event ignoredEvent;
    int x = 0;
    int eventsLeft = 1;

    /*If the control detection is paused then just check the timer*/
    if(controlData.paused == 1)
    {
        if(timer_Get_Remain(&controlData.pauseTimer) > 0)
        {
            timer_Calc(&controlData.pauseTimer);
        }
        else if(timer_Get_End(&controlData.pauseTimer) > 0) /*If the timer actually has an end time, otherwise must manually call control_Resume*/
        {
            control_Resume();
        }
    }
    else
    {
        /*The latest events will now be stored in controlData.c_Event and not in SDL's internal data*/
        for(x = 0; x < 32; x++)
        {
            eventsLeft = SDL_PollEvent(&controlData.c_Event[x]);

            if(eventsLeft == 0)
            {
                break;
            }
        }
    }

    controlData.eventsPolled = x;

    /*Dump the remaining events*/
    while(eventsLeft > 0)
    {
        eventsLeft = SDL_PollEvent(&ignoredEvent);
    }

    return;
}

SDL_Event *control_FindEventKey(int eventType, int key)
{
    int x = 0;

    for(x = 0; x < controlData.eventsPolled; x++)
    {
        if(controlData.c_Event[x].type == eventType)
        {
            if(control_GetKeyValue(&controlData.c_Event[x]) == key)
            {
                return &controlData.c_Event[x];
            }
        }
    }

    return NULL;
}

int control_PeepEvent(SDL_eventaction action, Uint32 mask)
{
    return SDL_PeepEvents(NULL, 1, action, mask);
}

/*
	Function: control_Keystate

	Description -
	Returns an array which contains the values of whether a key has been pressed.
*/
Uint8 *control_Keystate(void)
{
    return controlData.keystates;
}

/*
	Function: control_Setup

	Description -
	This function sets up the default values of a control event to be used.

	4 arguments:
	Control_Event *event - the event to setup.
	const char *name - the name to be given to the event.
	Timer *src_Timer - pointer to a TIMER structure that the event will use, if NULL it will use its own timer.
	unsigned int repeater - the value given to this will determine how fast the event waits before it can be activated again.
*/
int control_Setup(Control_Event *event, const char *name, Timer *src_Timer, unsigned int repeater)
{
    event->name = name;

	/*Default to no keys being pressed for this event*/
    event->eventKeys = NULL;

    event->repeater = timer_Setup(src_Timer, 0, repeater, 1);

    event->references = 0;

    event->timerLinks = NULL;

    depWatcher_Setup(&event->dW, event);

    /*file_Log(ker_Log(),0,"\nCreating new control event (%s)\n",event->name);*/
    /*printf("\nCreating new control event (%s)\n",event->name);*/

    return 0;
}

/*
	Function: control_Reference

	Description -
	Adds an extra reference to a control event.

	1 arguments:
	Control_Event *event - the event that will receive an extra reference, meaning it can
	be cleaned up more than once.
*/
Control_Event *control_Reference(Control_Event *event)
{
    event->references ++;

    return event;
}

/*
	Function: control_SetRepeat

	Description -
	Set the repeating time of the event. Returns no value

	2 arguments:
	Control_Event *event - the event that will have its repeating time changed.
	unsigned int repeater - the new repeating time of the event.
*/
void control_SetRepeat(Control_Event *event, unsigned int repeater)
{
    event->repeater.end_Time = repeater;

    return;
}

/*
	Function: control_AddTimerLink

	Description -
	Add in a link to from one control event to another so that
	when the first one is activated and resets its update timer, the second's update timer is also reset.

	3 arguments:
	Control_Event *event - the event that will reset linkEvent update timer when it is activated.
	Control_Event *linkEvent - reset this events timer when the first is activated.
	int linkBothWays - if > 0 then the link will be made in both directions, so when linkEvent is activated, the timer of event is reset.
*/
int control_AddTimerLink(Control_Event *event, Control_Event *linkEvent, int linkBothWays)
{
    struct list *timerLinks = event->timerLinks;
    int alreadyLinked = 0;

    while(timerLinks != NULL)
    {
        if(timerLinks->data == linkEvent)
        {
            alreadyLinked ++;
            break;
        }

        timerLinks = timerLinks->next;
    }

    if(alreadyLinked == 0)
    {
        list_Stack(&event->timerLinks, linkEvent, 0);

        /*When linkEvent is destroyed, the reference to it in the linked list above will be removed*/
        depWatcher_Add(&linkEvent->dW, &event->timerLinks);
    }

    if(linkBothWays > 0)
    {
        alreadyLinked += control_AddTimerLink(linkEvent, event, 0);
    }

    return alreadyLinked;
}

Control_Keys *control_CreateKeys(int numKeys, int pollType, int eventType)
{
    Control_Keys *newKeys = (Control_Keys *)mem_Malloc(sizeof(Control_Keys), __LINE__, __FILE__);

    newKeys->numKeys = numKeys;
    newKeys->pollType = pollType;

    if(eventType < 0)
    {
        if(eventType == C_BUTTON_STATES)
        {
            newKeys->useKeystates = 1;
            newKeys->eventType = eventType;
        }
        else
        {
            newKeys->eventType = control_GetSDLEventType(pollType, eventType);
            newKeys->useKeystates = 0;
        }
    }
    else
    {
        newKeys->useKeystates = 0;
        newKeys->eventType = eventType;
    }

    if(numKeys > 0)
        newKeys->keys = (int *)mem_Malloc(sizeof(int) * numKeys, __LINE__, __FILE__);
    else
        newKeys->keys = NULL;

    return newKeys;
}

void control_CopyKeys(Control_Keys *destKeys, Control_Keys *sourceKeys)
{
    int x = 0;

    destKeys->numKeys = sourceKeys->numKeys;
    destKeys->pollType = sourceKeys->pollType;
    destKeys->useKeystates = sourceKeys->useKeystates;
    destKeys->eventType = sourceKeys->eventType;

    if(destKeys->numKeys > 0)
        destKeys->keys = (int *)mem_Malloc(sizeof(int) * destKeys->numKeys, __LINE__, __FILE__);
    else
        destKeys->keys = NULL;

    for(x = 0; x < destKeys->numKeys; x++)
    {
        destKeys->keys[x] = sourceKeys->keys[x];
    }

    return;
}

void control_CleanKeys(Control_Keys *keys)
{
    if(keys->keys != NULL)
    {
        mem_Free(keys->keys);
        keys->keys = NULL;
    }

    keys->numKeys = 0;

    return;
}

/*
	Function: control_AddKey

	Description -
	A function used to add one or multiple keys to trigger an event. These keys will determine whether
	the event should be activated. You can add multiple series of keys, for example using this
	function to add the letter 'a' and then calling it again to add the left mouse button would result
	in the event being activated if either of those were pressed.
	Returns no value.

	5 arguments:
	Control_Event *event - the event to add keys to.
	unsigned int pollType - the type of event the keys need to be checked from,
	unsigned int event - the event type the keys need to be checked from,
	keyboard (C_KEYBOARD), mouse (C_MOUSE), or joystick (C_JOYSTICK).
	unsigned int numKey - the number of keys to be added.
	... - here you must pass the values of any keys to be added. These values can be found from SDLKey defines for keyboards.
*/
void control_AddKey(Control_Event *event, unsigned int pollType, int eventType, unsigned int numKey, ...)
{
    unsigned int x = 0;
    Control_Keys *cKeys = NULL;

    va_list var;

	/*Check if the control component should actually poll for this type of control method*/
    if((controlData.flags & pollType) != pollType)
        return;

    va_start(var, numKey);

    cKeys = control_CreateKeys(numKey, pollType, eventType);

    for(x = 0; x < numKey; x++)
    {
        cKeys->keys[x] = va_arg(var, int);
    }

	/*Push the index/es into the list so that another function can detect whether the keys are pressed*/
    switch (pollType)
    {
        case C_KEYBOARD:;
        case C_JOYSTICK:
        case C_MOUSE:
        list_Push(&event->eventKeys, cKeys, numKey);
        break;

        default:
        file_Log(ker_Log(), 1, "Error: key not added to list, undefined poll type %d.\n", pollType);
        break;
    }

    va_end(var);

    return;
}

void control_AddKeyList(Control_Event *event, unsigned int pollType, int eventType, struct list *keyList)
{
    int *key = NULL;
    int *keyValue = NULL;
    int numKey = list_Size(keyList);
    Control_Keys *cKeys = NULL;

    int x = 0;

    cKeys = control_CreateKeys(numKey, pollType, eventType);

    while(keyList != NULL)
    {
        keyValue = keyList->data;
        cKeys->keys[x] = *keyValue;

        x++;
        keyList = keyList->next;
    }

    switch (pollType)
    {
        case C_KEYBOARD:
        case C_JOYSTICK:
        case C_MOUSE:
        list_Push(&event->eventKeys, cKeys, numKey);
        break;

        default:
        file_Log(ker_Log(), 1, "Error: key not added to list, undefined poll type %d.\n", pollType);
        break;
    }

    return;
}

/*
    Function: control_CopyKey

    This function will copy all the keys that are used to trigger the source event onto the dest
    event.

    2 arguments:
    Control_Event *dest - The control event that obtains the keys from the source.
    Control_Event *source - The event that contains the keys to be copied to dest.
*/
void control_CopyKey(Control_Event *dest, Control_Event *source)
{
    int x = 0;
    Control_Keys *destKeys = NULL;
    Control_Keys *sourceKeys = NULL;

    struct list *sourceKeyList = NULL;

    sourceKeyList = source->eventKeys;

    while(sourceKeyList != NULL)
    {
        sourceKeys = sourceKeyList->data;

        destKeys = control_CreateKeys(0, 0, 0);
        control_CopyKeys(destKeys, sourceKeys);

        list_Stack(&dest->eventKeys, destKeys, destKeys->numKeys);

        sourceKeyList = sourceKeyList->next;
    }

    return;
}

/*
	Function: control_Clear

	Description -
	Removes all keys in an event. Returns the number of keys removed.

	1 argument:
	Control_Event *c - should point to the event that needs the keys removed.
*/
int control_Clear(Control_Event *c)
{
    Control_Keys *keys = NULL;
	int numRemoved = 0;

    while(c->eventKeys != NULL)
    {
        keys = c->eventKeys->data;

        if(keys != NULL)
        {
            control_CleanKeys(keys);
            mem_Free(keys);
        }

        list_Pop(&c->eventKeys);

		numRemoved ++;
    }

    return numRemoved;
}

/*
	Function: control_Clean

	Description -
	Cleans up a control event, removing any malloced memory used.
	This will only happen if the control has a single reference left. Otherwise
	the reference count will be decreased by 1.

	1 argument:
	Control_Event *event - the event to clean up.
*/
int control_Clean(Control_Event *event)
{
    Control_Event *linkedEvent = NULL;

    if(event->references > 0)
    {
        printf("Removing reference to event %s (%d)\n", event->name, event->references);
        event->references --;
        return (event->references + 1);
    }

    control_Clear(event);

    while(event->timerLinks != NULL)
    {
        linkedEvent = event->timerLinks->data;

        depWatcher_Remove(&linkedEvent->dW, &event->timerLinks);

        list_Pop(&event->timerLinks);
    }


    depWatcher_Clean(&event->dW);

    return 0;
}

void control_Report(Control_Event *event)
{
    struct list *keyList = event->eventKeys;
    Control_Keys *key = NULL;
    int x = 1;
    int y = 0;

    printf("Report %s\n", event->name);
    printf("Num key combinations %d, Activated %d (Control structure paused %d)\n", list_Size(event->eventKeys), control_KeyCheck(event->eventKeys, C_BUTTON_NORMAL), controlData.paused);

    while(keyList != NULL)
    {
        key = keyList->data;

        printf("[%d] numKeys %d, eventType %d, pollType %d, useStates %d:\n", x, key->numKeys, key->eventType, key->pollType, key->useKeystates);
        for(y = 0; y < key->numKeys; y++)
        {
            printf("\t%d\n", key->keys[y]);
        }

        x++;

        keyList = keyList->next;
    }

    printf("\n");

    return;
}

char *control_GetKeyName(int key, int pollType)
{
    static char keyName[128];

    if(pollType == C_KEYBOARD)
        strncpy(keyName, SDL_GetKeyName(key), 127);
    else
        snprintf(keyName, 127, "%d", key);

    return keyName;
}

/*
	Function: control_GenerateNameList

	Description -
    Fills a string with the details of the keys needed to activate the inputted
    control event.

	3 arguments:
	Control_Event *event - the control event that is used to create the list of keys needed it activate it.
	char *stringBuffer - the string to fill with the control key details.
	int maxLength - the maximum length of the string that holds the key details.
*/
int control_GenerateNameList(Control_Event *event, char *stringBuffer, int maxLength)
{
    struct list *keyList = event->eventKeys;
    Control_Keys *keys = NULL;
    int combination = 1;
    int length = maxLength;
    char *textCat = NULL;

    int x = 0;

    stringBuffer[0] = '\0';

    while(keyList != NULL && length > 0)
    {
        keys = keyList->data;

        switch(keys->pollType)
        {
            default:
            string_CatLimit(stringBuffer, "Unknown: ", &length);
            break;

            case C_KEYBOARD:
            string_CatLimit(stringBuffer, "Keyboard: ", &length);
            break;

            case C_JOYSTICK:
            string_CatLimit(stringBuffer, "Joystick: ", &length);
            break;

            case C_MOUSE:
            string_CatLimit(stringBuffer, "Mouse: ", &length);
            break;
        }

        for(x = 0; x < keys->numKeys; x++)
        {
            //printf("[%d] %d -> ", x, keys->keys[x]);
            textCat = control_GetKeyName(keys->keys[x], keys->pollType);
            //printf("%s\n", textCat);

            string_CatLimit(stringBuffer, textCat, &length);

            if(x != keys->numKeys - 1)
            {
                string_CatLimit(stringBuffer, " & ", &length);
            }
        }

        if(keyList->next != NULL)
            string_CatLimit(stringBuffer, " | ", &length);

        keyList = keyList->next;
    }

    //printf("%s\n", stringBuffer);

    return maxLength - length;
}

/*
    Function: control_Stop

    Description -
    Prevents a control event from activating for a certain time, if stopTime is -1 then
    it will pause the event until there is a resume requested.

    2 arguments:
    Control_Event *c - the event to be stopped.
    int stopTime - How long the event should be stopped for, if -1 then forever.
*/
void control_Stop(Control_Event *c, int stopTime)
{
    if(stopTime < 0)
    {
        timer_Pause(&c->repeater);
    }
    else
    {
        timer_Stop(&c->repeater, stopTime);
    }

    return;
}

/*
    Function: control_StopLinked
    Description -
    Stops all controls that are linked to the one passed.

    1 argument:
    Control_Event *c - the event with the links to stop.
*/
int control_StopLinked(Control_Event *c)
{
    Control_Event *linked = NULL;
    struct list *timerLinks = c->timerLinks;

    while(timerLinks != NULL)
    {
        linked = timerLinks->data;

        //printf("Stopping linked timer %s\n", linked->name);

        control_Stop(linked, c->repeater.end_Time);

        timerLinks = timerLinks->next;
    }

    return 0;
}

/*
    Function: control_CheckLink

    Description -
    Returns 1 if a linked timer has been activated.

    1 argument:
    Control_Event *c - the event with the links to stop.
*/
int control_CheckLink(Control_Event *c)
{
    Control_Event *linked = NULL;
    struct list *timerLinks = c->timerLinks;

    while(timerLinks != NULL)
    {
        linked = timerLinks->data;

        if(timer_Get_Stopped(&linked->repeater) > 0 || timer_Get_Passed(&linked->repeater, 0) < linked->repeater.end_Time)
        {
            timer_Calc(&linked->repeater);
            //printf("Linked timer activated %s\n", linked->name);

            return 1;
        }

        timerLinks = timerLinks->next;
    }

    return 0;
}

int control_CheckTimers(Control_Event *c)
{
    /*Is the control ready to be checked for yet?*/
    if(c->repeater.srcTime != NULL && timer_Get_Stopped(c->repeater.srcTime) > 0)
    {
        return 1; /*Source timer is paused*/
    }

    if(timer_Get_Stopped(&c->repeater) > 0 || (timer_Get_Passed(&c->repeater, 0) < c->repeater.end_Time))
    {
        timer_Calc(&c->repeater);

        return 1; /*Timer has not been on long enough for another check, return no key pressed*/
    }

    return 0;
}

/*
	Function: control_IsActivated

	Description -
	Returns 1 if the event is activated (All key presses required have
	been found and the repeating time is ready), otherwise returns 0.

	1 argument:
	Control_Event *c - the event to be checked if activated.
*/
unsigned int control_IsActivated(Control_Event *c)
{
    return control_IsActivatedType(c, C_BUTTON_NORMAL);
}

/*
	Function: control_IsActivatedType

	Description -
	Returns 1 if the event is activated (All key presses required have
	been found and the repeating time is ready), otherwise returns 0.

	2 arguments:
	Control_Event *c - the event to be checked if activated.
	int eventType - The general event type the control keys must be in. (C_ANY_TYPE) (C_BUTTON_DOWN) (C_BUTTON_UP) (C_BUTTON_STATES)
	if (C_BUTTON_NORMAL) then use the event given to the keys when they were first added.
*/
unsigned int control_IsActivatedType(Control_Event *c, int eventType)
{
    if(control_CheckTimers(c) == 1)
    {
        return 0;
    }

	/*Check if the correct keys have been pressed*/
    if(control_CheckLink(c) == 0 && control_KeyCheck(c->eventKeys, eventType) == 1)
    {
        timer_Start(&c->repeater); /*Timer has been on long enough, restart it*/

        control_StopLinked(c);

        return 1;
    }

    return 0;
}

/*
	Function: control_KeyCheck

	Description -
	Checks if the control keys are in the correct state to activate the control.
    If so returns 1 otherwise 0.

	2 arguments:
	struct list *keyList - a list containing the control keys to check for.
	int eventType - The general event type the key must be in. (C_ANY_TYPE) (C_BUTTON_DOWN) (C_BUTTON_UP) (C_BUTTON_STATES)
	if (C_BUTTON_NORMAL) then use the event given to the key when it was first added.
*/
int control_KeyCheck(struct list *keyList, int eventType)
{
    int x = 0;
    int SDLEvent = 0;
    int keyStates = 0;
    Control_Keys *cKeys = NULL;

    if(controlData.paused == 1)
    {
        return 0;
    }

    switch(eventType)
    {
        case C_ANY_TYPE:
        case C_BUTTON_DOWN:
        case C_BUTTON_UP:
        case C_BUTTON_STATES:
        case C_BUTTON_NORMAL:
        break;

        default:
        printf("Unknown type to poll for %d\n", eventType);
        return 0;
        break;
    }

    while(keyList != NULL)
    {
        cKeys = keyList->data;

        if(eventType == C_BUTTON_NORMAL)
        {
            keyStates = cKeys->useKeystates;
            SDLEvent = cKeys->eventType;
        }
        else if(eventType != C_BUTTON_STATES)
        {
            keyStates = 0;
            SDLEvent = control_GetSDLEventType(cKeys->pollType, eventType);
        }
        else
        {
            keyStates = 1;
        }

        switch(cKeys->pollType)
        {
            case C_KEYBOARD:
            for(x = 0; x < cKeys->numKeys; x++)
            {
                if(keyStates == 1 && controlData.keystates[cKeys->keys[x]] == 0)
                {
                    break; /*Then exit the loop early*/
                }
                else if(keyStates == 0 && control_FindEventKey(SDLEvent, cKeys->keys[x]) == NULL)
                {
                    break;
                }
            }
            break;

            case C_MOUSE:

            for(x = 0; x < cKeys->numKeys; x++)
            {
                if(keyStates == 1 && (SDL_GetMouseState(NULL, NULL) & (Uint8)cKeys->keys[x]) != (Uint8)cKeys->keys[x])
                {
                    break;
                }
                else if(keyStates == 0 && control_FindEventKey(SDLEvent, cKeys->keys[x]) == NULL)
                {
                    break;
                }
            }

            break;

            case C_JOYSTICK:

            for(x = 0; x < cKeys->numKeys; x++)
            {
                if(keyStates == 1 && SDL_JoystickGetButton(controlData.joystick, cKeys->keys[x]) == 0)
                {
                    break; /*Then exit the loop early*/
                }
                else if(keyStates == 0 && control_FindEventKey(SDLEvent, cKeys->keys[x]) == NULL)
                {
                    break;
                }
            }

            break;
        }

		/*If the last key checked was pressed then all the keys needed in order for this event to activate have been pressed*/
        if(x == cKeys->numKeys)
        {
            return 1;
        }

        keyList = keyList->next;
    }

    return 0;
}

/*
    Function: control_Save

    Description -
    Saves the keys of a control to a file.

    2 arguments:
    Control_Event *c - The control to be saved.
    FILE *save - The FILE location to save it at.
*/
void control_Save(Control_Event *c, FILE *save)
{
    int totalControls = list_Size(c->eventKeys);

    Control_Keys *keys = NULL;
    struct list *controlList = NULL;

    char controlName[255];

    int x = 0;

    strncpy(controlName, c->name, 254);

    string_ReplaceChar(controlName, ' ', '%');

    /*First save the name and total controls held*/
    file_Log(save, 0, "%d %s\n%d\n",
    strlen(controlName),
    controlName,
    totalControls
    );

    /*Then save each control event*/
    controlList = c->eventKeys;

    while(controlList != NULL)
    {
        keys = controlList->data;

        /*Save the description of the key/s*/
        file_Log(save, 0, "%d %d %d\n",
        keys->numKeys,
        keys->eventType,
        keys->pollType
        );

        /*Save the key values*/
        for(x = 0; x < keys->numKeys; x++)
        {
            file_Log(save, 0, "%d ", keys->keys[x]);
        }

        if(keys->numKeys > 0)
            file_Log(save, 0, "\n");

        controlList = controlList->next;
    }

    return;
}

/*
    Function: control_LoadName

    Description -
    Load the name of a control from a file.
    The FILE pointer should be located at the start of where the control was saved.
    Once the name has been loaded, the FILE offset position is unchanged from when the
    function was first called.

    2 arguments:
    char controlName - Pointer to where the controls name will be copied. (Max 255 chars)
    FILE *load - The FILE must point to the start of where the control was saved.
*/
void control_LoadName(char *controlName, FILE *load)
{
    fpos_t originalPos;
    Control_Keys cKey;
    struct list *controlList = NULL;

    char *fName = NULL;
    int nameLength = 0;

    fgetpos (load, &originalPos);

    /*First load in the name and controls*/
    fscanf(load, "%d ", &nameLength);

    if(nameLength < 0 || nameLength > 253)
    {
        nameLength = 0;
    }
    else
        nameLength += 1;

    fgets(controlName , nameLength, load);

    string_ReplaceChar(controlName, '%', ' ');

    fsetpos(load, &originalPos);

    return;
}

/*
    Function: control_Load

    Description -
    Load the keys of a control from a file.
    The FILE pointer should be located at the start of where the control was saved.

    2 arguments:
    Control_Event *c - The control that has been setup already and is to have keys loaded in from the file.
    FILE *load - The FILE holding the keys, must point to the start of where the control was saved.
*/
void control_Load(Control_Event *c, FILE *load)
{
    Control_Keys cKey;
    struct list *controlList = NULL;
    int nameLength = 0;
    int x = 0;
    int y = 0;
    char controlName[255];
    int totalControls = 0;

    int *key = NULL;

    /*First load in the name and controls*/
    fscanf(load, "%d ", &nameLength);

    if(nameLength < 0 || nameLength > 253)
    {
        nameLength = 0;
    }
    else
        nameLength += 1;

    fgets(controlName , nameLength, load);

    fscanf(load, "%d", &totalControls);

    string_ReplaceChar(controlName, '%', ' ');

    if(strcmp(c->name, controlName) != 0)
    {
        printf("Error loading keys for  (%s) ment for (%s)\n", c->name, controlName);
        return;
    }

    /*Now load in each control event*/
    for(x = 0; x < totalControls; x++)
    {
        /*Load in the details of the key/s*/
        fscanf(load, "%d %d %d", &cKey.numKeys, &cKey.eventType, &cKey.pollType);

        /*Load in the values of each key*/
        for(y = 0; y < cKey.numKeys; y++)
        {
            key = (int *)mem_Malloc(sizeof(int), __LINE__, __FILE__);

            fscanf(load, "%d", key);

            /*Insert each value into a list so that they can all be added*/
            list_Stack(&controlList, key, 0);
        }

        /*Place the loaded keys into the control*/
        control_AddKeyList(c, cKey.pollType, cKey.eventType, controlList);

        list_ClearAll(&controlList);
    }

    return;
}
