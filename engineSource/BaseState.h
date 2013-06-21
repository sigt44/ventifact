#ifndef BASESTATE_H
#define BASESTATE_H

#include "Common/List.h"

enum
{
    STATE_INIT = 0,
    STATE_MAIN = 1,
    STATE_EXIT = 2,
    STATE_STACK = 3,
    STATE_COMPLETE = 4
} statusTypes;

typedef struct base_State
{
    char *name;

	void (*startInit)(void *info);
	void (*init)(void *info);
	void (*controls)(void *info);
	void (*logic)(void *info);
	void (*render)(void *info);
	void (*exit)(void *info);

	void (*onActivate)(void *info);

	void *info;

	unsigned int status;
	unsigned int callInit;
	unsigned int malloced;
} Base_State;


void baseState_Setup(Base_State *bState,
    char *name,
    void (*initFunc)(void *),
    void (*controlsFunc)(void *),
    void (*logicFunc)(void *),
    void (*renderFunc)(void *),
    void (*exitFunc)(void *),
    void *info);

void baseState_Edit(Base_State *bState,
    void (*initFunc)(void *),
    void (*controlsFunc)(void *),
    void (*logicFunc)(void *),
    void (*renderFunc)(void *),
    void (*exitFunc)(void *),
    void *info,
    int callInit);

void baseState_SetOnActivate(Base_State *bState, void (*onActivate)(void *));

Base_State *baseState_Create(char *name,
    void (*initFunc)(void *),
    void (*controlsFunc)(void *),
    void (*logicFunc)(void *),
    void (*renderFunc)(void *),
    void (*exitFunc)(void *),
    void *info);

Base_State *baseState_CreateFrom(Base_State *copyState);

void baseState_Push(struct list **stateList, Base_State *bState);

void baseState_Stack(struct list **stateList, Base_State *bState);

void baseState_Reset(Base_State *bState);

void baseState_Process_List(struct list **head);

int baseState_Process(Base_State *bState);

int baseState_CheckStatus(Base_State *bState);

void baseState_Activate(Base_State *bState);

int baseState_ActivateList(struct list **stateList, Base_State *activeState);

int baseState_StackList(struct list **stateList, Base_State *activeState);

void baseState_Quit(struct list **stateList, int activateNext);

void baseState_Quit_List(struct list **stateList);

void baseState_CallQuit(Base_State *bState, int fullQuit);

Base_State *baseState_GetState(struct list *stateList, char *stateName);

#endif
