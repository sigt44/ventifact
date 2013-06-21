#ifndef CONTROLS_H
#define CONTROLS_H

/*
	Controls.h
	Structures and functions to be used to handle input events. Uses SDL.
*/

#include "SDL/SDL.h"

#include "Time/Timer.h"
#include "Common/List.h"
#include "Common/ListWatcher.h"
#include "File.h"

#define C_KEYBOARD 2
#define C_JOYSTICK 4
#define C_MOUSE 8

/*Define some known key codes*/

#define C_MOUSELEFT 1
#define C_MOUSEMIDDLE 2
#define C_MOUSERIGHT 3

#ifdef GP2X

#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (15)
#define GP2X_BUTTON_Y               (14)
#define GP2X_BUTTON_L               (11)
#define GP2X_BUTTON_R               (10)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

#endif

#ifdef PSP
#define PSP_TRIANGLE 0
#define PSP_CIRCLE 1
#define PSP_SQUARE 3
#define PSP_L_TRIGGER 4
#define PSP_R_TRIGGER 5
#define PSP_DOWN 6
#define PSP_LEFT 7
#define PSP_UP 8
#define PSP_RIGHT 9
#define PSP_SELECT 10
#define PSP_START 11
#define PSP_HOME 12
#define PSP_HOLD 13
#endif

#ifdef DINGOO
#define DINKEY_UP 273
#define DINKEY_DOWN 274
#define DINKEY_LEFT 276
#define DINKEY_RIGHT 275
#define DINKEY_START 13
#define DINKEY_SELECT 27
#define DINKEY_A 306
#define DINKEY_B 308
#define DINKEY_X 32
#define DINKEY_Y 304
#define DINKEY_L_SHOULDER 9
#define DINKEY_R_SHOULDER 8
#endif

#ifdef PANDORA
#define PND_LEFT SDLK_LEFT
#define PND_RIGHT SDLK_RIGHT
#define PND_UP SDLK_UP
#define PND_DOWN SDLK_DOWN
#define PND_LOGO SDLK_MENU
#define PND_START SDLK_LALT
#define PND_SELECT SDLK_LCTRL
#define PND_Y SDLK_PAGEUP
#define PND_A SDLK_HOME
#define PND_X SDLK_PAGEDOWN
#define PND_B SDLK_END
#define PND_RSHOULDER SDLK_RCTRL
#define PND_LSHOULDER SDLK_RSHIFT
#endif

#define C_ANY_TYPE -1
#define C_BUTTON_DOWN -2
#define C_BUTTON_UP -3
#define C_BUTTON_STATES -4
#define C_BUTTON_NORMAL -5

enum control_Direction
{
    RIGHT = 0,
    LEFT = 1,
    UP = 2,
    DOWN = 3
};

struct control_Component
{
    struct list *events;

    SDL_Event c_Event[32]; /*Holds the 32 events at top of stack from pollEvent*/
    int eventsPolled; /*Holds how many events where polled in the last update*/
    Timer pauseTimer; /*Used to ignore events for a certain period of time*/
    int paused; /*Says whether control events are being ignored*/

    Uint8 *keystates; /*Points to array that says whether a specific key is pressed or not*/

    Uint8 flags; /*Holds which type of events this component will poll, ie Keyboard/mouse/joystick/all*/

    #ifdef DINGOO
	void *joystick;
    #else
    SDL_Joystick *joystick;
    #endif
};

typedef struct control_Event
{
    const char *name;

    /*Each node in the list holds a Control_keys structure to determine which keys need to be pressed*/
    struct list *eventKeys;

    int references; /*How many references are there to this control. Prevents multiple releasing of memory*/

    struct list *timerLinks;
    Dependency_Watcher dW;

    Timer repeater;
} Control_Event;

typedef struct control_Keys
{
    int numKeys;
    int useKeystates;
    int eventType;
    int pollType;

    int *keys;
} Control_Keys;

int control_Init(Uint8 flags);

void control_PrintTypes(void);

int control_Quit(void);

void control_Update(void);

int control_SameEvent(SDL_Event *firstEvent, SDL_Event *secondEvent);

int control_IsGeneralType(int checkType, int isType);

int control_GetKeyValue(SDL_Event *event);

struct list *control_TakeEvents(int eventType, unsigned int pollTime);

int control_PeepEvent(SDL_eventaction action,Uint32 mask);

Uint8 *control_Keystate(void);

SDL_Event *control_GetEventType(int eventType);

int control_GetPollType(int eventType);

int control_Setup(Control_Event *event, const char *name, Timer *src_Timer, unsigned int repeater);

void control_SetRepeat(Control_Event *event, unsigned int repeater);

int control_AddTimerLink(Control_Event *event, Control_Event *linkEvent, int linkBothWays);

void control_AddKey(Control_Event *event, unsigned int pollType, int eventType, unsigned int numKey, ...);

void control_AddKeyList(Control_Event *event, unsigned int pollType, int eventType, struct list *keyList);

void control_CopyKey(Control_Event *dest, Control_Event *source);

void control_Pause(int pauseTime);
void control_Resume(void);

void control_Stop(Control_Event *c, int stopTime);
int control_StopLinked(Control_Event *c);

int control_CheckLink(Control_Event *c);
int control_CheckTimers(Control_Event *c);

unsigned int control_IsActivated(Control_Event *c);
unsigned int control_IsActivatedType(Control_Event *c, int eventType);

int control_KeyCheck(struct list *keyList, int eventType);

int control_Clear(Control_Event *c);

int control_Clean(Control_Event *event);

void control_Report(Control_Event *event);

char *control_GetKeyName(int key, int pollType);
int control_GenerateNameList(Control_Event *event, char *stringBuffer, int maxLength);

void control_Save(Control_Event *c, FILE *save);

void control_LoadName(char *controlName, FILE *load);
void control_Load(Control_Event *c, FILE *load);


#endif
