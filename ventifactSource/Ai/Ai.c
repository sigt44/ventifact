#include "Ai.h"

#include "Memory.h"

#include "Ai_State.h"

void aiModule_Setup(Ai_Module *aiMod)
{
    aiMod->localList = NULL;
    aiMod->globalList = NULL;

    aiMod->lowestPriority = NULL;
    aiMod->highestPriority = NULL;

    aiMod->priorityCheck = 0;

    return;
}

int aiModule_UpdatePriority(Ai_Module *aiMod, Ai_State *state)
{
    if(aiMod->lowestPriority == NULL)
    {
        aiMod->lowestPriority = state;
    }

    if(aiMod->highestPriority == NULL)
    {
        aiMod->highestPriority = state;
    }

    if(aiMod->lowestPriority->priority < state->priority)
    {
        aiMod->lowestPriority = state;

        return 1;
    }

    if(aiMod->highestPriority->priority > state->priority)
    {
        aiMod->highestPriority = state;
        return 2;
    }

    return 0;
}

void aiModule_PushState(Ai_Module *aiMod, Ai_State *state)
{
    if(state->group != AI_GLOBAL_STATE)
    {
        aiState_Insert(&aiMod->localList, state);
    }
    else
    {
        aiState_Insert(&aiMod->globalList, state);
    }

    aiModule_UpdatePriority(aiMod, state);

    return;
}

void aiModule_StackState(Ai_Module *aiMod, Ai_State *state)
{
    if(state->group != AI_GLOBAL_STATE)
    {
        list_Stack(&aiMod->localList, state, state->priority);
    }
    else
    {
        list_Stack(&aiMod->globalList, state, state->priority);
    }

    aiModule_UpdatePriority(aiMod, state);

    return;
}

int aiModule_DestroyState(Ai_Module *aiMod, struct list **headList, struct list *currentNode, Ai_State *a)
{
    list_Delete_Node(headList, currentNode);/*Remove the state from the list*/

    /*Make sure the state is not in the lowest/highest priority tracking in the ai module*/
    if(a == aiMod->highestPriority)
    {
        aiMod->highestPriority = NULL;
    }

    if(a == aiMod->lowestPriority)
    {
        aiMod->lowestPriority = NULL;
    }


    /*Release any memory allocated for the state*/
    aiState_Clean(a);
    mem_Free(a); /*and delete the state*/

    return 0;
}

int aiModule_DestroyStateList(Ai_Module *aiMod, struct list **headList)
{
    struct list *aiStates = *headList;
    struct list *next = NULL;
    int numDestroyed = 0;

    while(aiStates != NULL)
    {
        next = aiStates->next;

        /*Decide what to do with the complete type of the state*/
        if(aiModule_CompleteState(aiMod, headList, aiStates->data) == AI_COMPLETE_DESTROY)
        {
            aiModule_DestroyState(aiMod, headList, aiStates, aiStates->data);
            numDestroyed ++;
        }

        aiStates = next;
    }

    return numDestroyed;
}


/*
    Function: aiModule_CompleteState

    Description - Decides how the state should be handled once it has been completed.

    3 arguments:
    Ai_Module *aiMode - The ai module the state is in.
    struct list **headlist - The head of the list that the ai state is in.
    Ai_State *a - The ai state that is checked.
*/
int aiModule_CompleteState(Ai_Module *aiMod, struct list **headList, Ai_State *a)
{
    int x = 0;
    switch(a->complete)
    {
        case AI_NOT_COMPLETE:
        case AI_COMPLETE_WAIT:

        break;

        case AI_COMPLETE_GROUP:
            aiState_SetDestroyedGroup(*headList, a->group);
        break;

        case AI_COMPLETE_SINGLE:
            a->complete = AI_COMPLETE_DESTROY;
        break;

        case AI_COMPLETE_STACK:

            /*Reset the ai state*/
            aiState_Setup(a,
                a->group,
                a->priority,
                a->completeType,
                a->entity,
                a->extraStructure,
                (void *)(Ai_State *)a->init,
                (void *)(Ai_State *)a->mainLoop,
                (void *)(Ai_State *)a->exit
                );

            list_Delete_NodeFromData(headList, a);/*Remove the state from the list*/

            aiModule_StackState(aiMod, a); /*Place it at the end of the list*/

        break;
    }

    return a->complete;
}

void aiModule_Update(Ai_Module *aiMod)
{
    /*Update all the states*/
    aiState_UpdateList(&aiMod->globalList);
    aiState_UpdateList(&aiMod->localList);

    /*Clean up any states that need to be deleted*/
    aiModule_DestroyStateList(aiMod, &aiMod->localList);
    aiModule_DestroyStateList(aiMod, &aiMod->globalList);

    return;
}

int aiModule_HasAi(Ai_Module *aiMod, void (*initFunction)(Ai_State *AiState))
{
    struct list *aiStates = NULL;

    int x = 0;
    Ai_State *a = NULL;

    for(x = 0; x < 2; x ++)
    {
        if(x == 0)
            aiStates = aiMod->localList;
        else if(x == 1)
            aiStates = aiMod->globalList;

        while(aiStates != NULL)
        {
            a = aiStates->data;

            if(a->init == (void *)initFunction)
            {
                return a->complete;
            }

            aiStates = aiStates->next;
        }
    }

    return AI_NO_STATE;
}

void aiModule_Report(Ai_Module *aiMod)
{
    struct list *aiStates = NULL;
    Ai_State *a = NULL;
    int x = 0;

    printf("Reporting ai module -[ListAddress] (StateAddress) - (completeState) (group) (priority(listPriority))\n");

    for(x = 0; x < 2; x ++)
    {
        if(x == 0)
        {
            printf("[%p]Global states -\n", aiMod->globalList);
            aiStates = aiMod->globalList;

        }
        else if(x == 1)
        {
            printf("[%p]Local states -\n", aiMod->localList);
            aiStates = aiMod->localList;
        }

        while(aiStates != NULL)
        {
            a = aiStates->data;

            printf("[%p]%p - %d %d %d(%d)\n", aiStates, a, a->complete, a->group, a->priority, aiStates->info);
            aiStates = aiStates->next;
        }
    }

    return;
}

void aiModule_Clean(Ai_Module *aiMod)
{
    while(aiMod->globalList != NULL)
    {
        aiState_Clean(aiMod->globalList->data);
        mem_Free(aiMod->globalList->data);

        list_Pop(&aiMod->globalList);
    }

    while(aiMod->localList != NULL)
    {
        aiState_Clean(aiMod->localList->data);
        mem_Free(aiMod->localList->data);

        list_Pop(&aiMod->localList);
    }

    aiMod->highestPriority = aiMod->lowestPriority = NULL;

    return;
}

