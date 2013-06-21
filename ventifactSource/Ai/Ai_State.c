#include <stdio.h>

#include "Ai_State.h"

#include "Memory.h"

#include "Ai.h"

/*
    Function: aiState_Create

    Description -
    Allocates memory for a new ai state.

    0 arguments:
*/
Ai_State *aiState_Create(void)
{
    Ai_State *ai = (Ai_State *)mem_Malloc(sizeof(Ai_State), __LINE__, __FILE__);

    return ai;
}

/*
    Function: aiState_Setup

    Description -
    This function sets up the variables for an ai state.

    9 arguments:
    Ai_State *a - The ai state that is to be setup
    int group - The group ID of the ai state
    int priority - The ai states priority
    int completeType - How the state should be treated once its complete.
    void *entity - Points to the object that the ai is to be based on.
    void *extraStructure - Points to the specific structure that the ai state makes use of.
    void (*init)(void *SiState) - Points to the initialise function for the ai state.
    void (*mainLoop)(void *AiState) - Points to the main updating function of the ai state.
    void (*exit)(void *AiState) - Points to the exit function of the ai state.
*/
void aiState_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    void *entity,
    void *extraStructure,
    void (*init)(Ai_State *AiState),
    void (*mainLoop)(Ai_State *AiState),
    void (*exit)(Ai_State *AiState)
    )
{
    /*Set user parameters*/
    a->group = group;
    a->priority = priority;
    a->entity = entity;
    a->completeType = completeType;
    a->hasInit = 0;

    a->extraStructure = extraStructure;

    a->init = (void *)init;
    a->mainLoop = (void *)mainLoop;
    a->exit = (void *)exit;

    /*Set base values*/
    a->update = a->init;

    a->paused = 0;

    a->complete = AI_NOT_COMPLETE;

    return;
}

/*
    Function: aiState_Init

    Description -
    This should be called from within a custom init function of the state. It
    will confirm that the state has been initialised and will move the update function
    to the main loop.

*/
void aiState_Init(Ai_State *a)
{
    a->update = a->mainLoop;
    a->hasInit = 1;

    return;
}


/*
    Function: aiState_Insert

    Description -
    Inserts an ai state into a linked list in order of its priority.

*/
void aiState_Insert(struct list **aiList, Ai_State *a)
{
    /*Push the ai state into the list in order of its priority*/
    list_Push_Sort(aiList, a, a->priority);
}

/*
    Function aiState_Update

    Description -
    Updates the ai state.
    Returns > 0 if it has been deleted.

    1 argument:
    Ai_State *a - The ai state to update.
*/
int aiState_Update(Ai_State *a)
{
    /*If the ai state can be updated and is not paused*/
    if(a->update != NULL && !a->paused && a->complete == AI_NOT_COMPLETE)
        a->update(a);

    return a->complete;
}

/*
    Function: aiState_SetDestroyedGroup

    Description -
    Set any ai states of group as being destroyed.

    2 arguments:
    struct list *states - The head of the linked list that contains the ai states.
    int group - The group of ai states that should be deleted/destroyed.
*/
void aiState_SetDestroyedGroup(struct list *aiStates, int group)
{
    Ai_State *a = NULL;

    while(aiStates != NULL)
    {
        a = aiStates->data;

        if(a->group == group)
        {
            /*Exit the state early if it hasnt already*/
            if(a->complete == AI_NOT_COMPLETE)
            {
                a->exit(a);
            }

            a->complete = AI_COMPLETE_DESTROY;
        }

        aiStates = aiStates->next;
    }
}

void aiState_SetDestroyedAll(struct list *aiStates)
{
    Ai_State *a = NULL;

    while(aiStates != NULL)
    {
        a = aiStates->data;

        /*Exit the state early if it hasnt already*/
        if(a->complete == AI_NOT_COMPLETE)
        {
            a->exit(a);
        }

        a->complete = AI_COMPLETE_DESTROY;

        aiStates = aiStates->next;
    }
}

/*
    Function: aiState_UpdateList

    Description -
    Function that updates all ai states of the highest priority group contained in a linked list.

    1 argument:
    struct list **states - Pointer to the address of the head of the linked list.
*/
void aiState_UpdateList(struct list **states)
{
    struct list *aiStates = *states;
    struct list *next = NULL;

    Ai_State *a = NULL;
    const int maxGroups = 1;
    int currentGroup = AI_GLOBAL_STATE;
    int foundGroup = 0;

    while(aiStates != NULL)
    {
        a = aiStates->data;

        if(a->group != currentGroup)
        {
            currentGroup = a->group;

            foundGroup ++;
        }

        if(foundGroup <= maxGroups)
        {
            aiState_Update(a);
        }
        else
            return;

        aiStates = aiStates->next;
    }

    return;
}

/*
    Function: aiState_SetComplete

    Description -
    This function sets the ai state to be completed.
    Returns whether the ai state has actually been initialised.

    1 argument:
    Ai_State *a - The ai state to complete.
*/
int aiState_SetComplete(Ai_State *a)
{
    a->complete = a->completeType;

    return a->hasInit;
}

/*
    Function: aiState_ForceComplete

    Description -
    This function sets the ai state to be completed.
    The type of completion is specified by the complete variable and
    not by the one given originally to the ai state.

    2 arguments:
    Ai_State *a - The ai state to complete.
    int complete - The type of completion to give the ai state.
*/
void aiState_ForceComplete(Ai_State *a, int complete)
{
    a->complete = complete;

    return;
}

/*
    Function: aiState_Clean

    Description -
    Releases any dynamically allocated memory used by the ai state.

    1 argument:
    Ai_State *a - The ai state that is to have its dynamic memory released.
*/
void aiState_Clean(Ai_State *a)
{
    if(a->complete == AI_NOT_COMPLETE)
    {
        if(a->update == a->init)
        {
            //printf("Removing ai state without it finishing or being initialised.\n");
        }
        a->exit(a);
    }

    a->update = NULL;

    if(a->extraStructure != NULL)
    {
        mem_Free(a->extraStructure);
        a->extraStructure = NULL;
    }

    return;
}

/*
    Function: aiState_Delete

    Description -
    Releases any dynamically allocated memory used by the ai state.
    Also frees the state itself. Always returns NULL.

    1 argument:
    Ai_State *a - The ai state that is to have its dynamic memory released.
*/
void *aiState_Delete(Ai_State *a)
{
    aiState_Clean(a);
    mem_Free(a);

    return NULL;
}
