#include <stdio.h>
#include <string.h>
#include "BaseState.h"

#include "Memory.h"
#include "File.h"
#include "Kernel/Kernel.h"

Base_State *baseState_CreateFrom(Base_State *copyState)
{
    Base_State *newState = (Base_State *)mem_Malloc(sizeof(Base_State),__LINE__,__FILE__);

    baseState_Setup(newState, copyState->name, copyState->init, copyState->controls, copyState->logic, copyState->render, copyState->exit, copyState->info);

    newState->malloced = 1;

    return newState;
}

Base_State *baseState_Create(char *name,
    void (*initFunc)(void *),
    void (*controlsFunc)(void *),
    void (*logicFunc)(void *),
    void (*renderFunc)(void *),
    void (*exitFunc)(void *),
    void *info)
{
    Base_State *newState = (Base_State *)mem_Malloc(sizeof(Base_State),__LINE__,__FILE__);

    baseState_Setup(newState, name, initFunc, controlsFunc, logicFunc, renderFunc, exitFunc, info);

    newState->malloced = 1;

    return newState;
}

void baseState_Setup(Base_State *bState,
    char *name,
    void (*initFunc)(void *),
    void (*controlsFunc)(void *),
    void (*logicFunc)(void *),
    void (*renderFunc)(void *),
    void (*exitFunc)(void *),
    void *info)
{
    bState->name = name;
	bState->startInit = bState->init = initFunc;
	bState->controls = controlsFunc;
	bState->logic = logicFunc;
	bState->render = renderFunc;
	bState->exit = exitFunc;

	bState->info = info;

	bState->callInit = 1;

	bState->onActivate = NULL;

    bState->status = STATE_INIT;

    bState->malloced = 0;

	return;
}

void baseState_Edit(Base_State *bState,
    void (*initFunc)(void *),
    void (*controlsFunc)(void *),
    void (*logicFunc)(void *),
    void (*renderFunc)(void *),
    void (*exitFunc)(void *),
    void *info,
    int callInit)
{
    /*The previous state is presumed to have already been set up*/
    if(initFunc != NULL)
        bState->init = initFunc;
    if(controlsFunc != NULL)
        bState->controls = controlsFunc;
    if(logicFunc != NULL)
        bState->logic = logicFunc;
    if(renderFunc != NULL)
        bState->render = renderFunc;
    if(exitFunc != NULL)
        bState->exit = exitFunc;

    if(bState->info != NULL)
        bState->info = info;

    if(callInit == 1)
    {
        bState->status = STATE_INIT;
    }

	bState->callInit = callInit;

    return;
}

void baseState_SetOnActivate(Base_State *bState, void (*onActivate)(void *))
{
	bState->onActivate = onActivate;
	return;
}

void baseState_Push(struct list **stateList, Base_State *bState)
{
    list_Push(stateList, bState, 0);

    baseState_Activate(bState);

    return;
}

void baseState_Stack(struct list **stateList, Base_State *bState)
{
    list_Stack(stateList, bState, 0);

    return;
}

void baseState_Reset(Base_State *bState)
{
    int malloced = bState->malloced;

    if(bState->status != STATE_COMPLETE && bState->exit != NULL && bState->callInit == 0)
        bState->exit(bState);

    baseState_Setup(bState, bState->name, bState->startInit, bState->controls, bState->logic, bState->render, bState->exit, NULL);

    bState->malloced = malloced;

    baseState_Activate(bState);

    return;
}

void baseState_Process_List(struct list **head)
{
    int status = baseState_Process((*head)->data);

    switch(status)
    {
        case STATE_EXIT:
        case STATE_COMPLETE:
        baseState_Quit(head, 1);
        break;

        case STATE_STACK:

        baseState_StackList(head, (*head)->data);

        break;

        default:
        break;
    }

    return;
}

int baseState_Process(Base_State *bState)
{
    if(bState->controls != NULL)
        bState->controls(bState);

    if(bState->logic != NULL)
        bState->logic(bState);

    if(bState->render != NULL)
        bState->render(bState);

    return baseState_CheckStatus(bState);
}

int baseState_CheckStatus(Base_State *bState)
{
    if(bState->status == STATE_EXIT)
    {
        if(bState->exit != NULL && bState->callInit == 0)
        {
            bState->exit(bState);
        }

        bState->status = STATE_COMPLETE;
    }

    return bState->status;
}


void baseState_Activate(Base_State *bState)
{
	if(bState != NULL)
	{
	    file_Log(ker_Log(), 0, "Changeing state: %s, Init = %d, Status %d \n", bState->name, bState->callInit, bState->status);

		if(bState->init != NULL && bState->callInit == 1)
		{
            bState->init(bState); /*Call starting function unless there isnt one or it has been already set up and manually paused*/
            bState->callInit = 0;
		}
		else if(bState->onActivate != NULL)
		{
			bState->onActivate(bState);
		}

	bState->status = STATE_MAIN;
	}

	return;
}

int baseState_ActivateList(struct list **stateList, Base_State *activeState)
{
    struct list *listNode = NULL;

    listNode = list_MatchCheck(*stateList, activeState);

    if(listNode == NULL)
    {
        printf("Warning cant find %s state in statelist to activate\n", activeState->name);

        return 0;
    }

    list_Unlink_Node(stateList, listNode);

    list_Link_Node(stateList, NULL, listNode);

    baseState_Activate(activeState);

    return 1;
}

int baseState_StackList(struct list **stateList, Base_State *activeState)
{
    struct list *listNode = NULL;
    struct list *endNode = (*stateList)->previous;

    listNode = list_MatchCheck(*stateList, activeState);

    if(listNode == NULL)
    {
        file_Log(ker_Log(), P_OUT, "Warning cant find %s state in statelist to stack\n", activeState->name);

        return 0;
    }

    list_Unlink_Node(stateList, listNode);

    list_Link_Node(stateList, endNode, listNode);

    activeState->status = STATE_MAIN;

    return 1;
}

void baseState_Quit(struct list **stateList, int activateNext)
{
    Base_State *bState = (*stateList)->data;

    file_Log(ker_Log(), 0, "Quitting %s\n", bState->name);

    /*If there is an exit state (Which hasnt already been called) and the current state is passed the initialise state*/
	if(bState->status != STATE_COMPLETE && bState->exit != NULL && bState->callInit == 0)
    {
        file_Log(ker_Log(), 0, "Forcing %s to exit\n", bState->name);

        bState->exit(bState);
    }

    if(bState->malloced)
        mem_Free(bState);

    list_Pop(stateList);

    if(*stateList != NULL && activateNext == 1)
        baseState_Activate((*stateList)->data);

	return;
}

void baseState_Quit_List(struct list **stateList)
{
    while(*stateList != NULL)
    {
        baseState_Quit(stateList, 0);
    }

    return;
}

void baseState_CallQuit(Base_State *bState, int fullQuit)
{
    if(fullQuit == 1)
    {
        bState->status = STATE_EXIT;
    }
    else
    {
        bState->status = STATE_STACK;
    }

    return;
}

Base_State *baseState_GetState(struct list *stateList, char *stateName)
{
    Base_State *checkState = NULL;

    while(stateList != NULL)
    {
        checkState = stateList->data;

        if(strcmp(checkState->name, stateName) == 0)
        {
            return checkState;
        }

        stateList = stateList->next;
    }

    return NULL;
}
