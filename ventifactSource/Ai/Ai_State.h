#ifndef AI_STATE_H
#define AI_STATE_H

#include "Common/List.h"

enum
{
    AI_NO_STATE = -1, /*There is no ai state*/
    AI_NOT_COMPLETE = 0, /*The ai state is not complete*/
    AI_COMPLETE_SINGLE = 1, /*The ai state is complete and can be deleted*/
    AI_COMPLETE_GROUP = 2, /*The ai state is complete should be deleted along with all other states in the same group*/
    AI_COMPLETE_WAIT = 3, /*The ai state is complete but should not be deleted yet*/
    AI_COMPLETE_STACK = 4, /*The ai state that once complete is reset and placed at the end of the ai list*/
    AI_COMPLETE_DESTROY = 5 /*The ai state is going to be deleted.*/
};

typedef struct
{
    /*Initialising function of the ai state*/
    void (*init)(void *AiState);/*This prepares the ai state for main use. Making sure to obtain any non user inputted information the ai goal
                                needs to get started.*/

    /*Main function of the ai state*/
    void (*mainLoop)(void *AiState);/*This function contains the logic for the ai goal to be achieved.*/


    /*Exiting function of the ai state*/
    void (*exit)(void *AiState);/*This function should confirm the state is complete so that it can be deleted and also
                                cleans up any dynamic memory that has been used.*/

    /*The function to call that updates the state, will point to the function at either init, main or exit*/
    void (*update)(void *AiState);

    /*Group ID of the ai state*/
    int group;
    /*If true then the state has been initialised*/
    int hasInit;
    /*Priority of the state, if low it might get deleted in favor of another state*/
    int priority;
    /*Tracks in the state has completed its target*/
    int complete;
    /*Determins how the state should be treated once it has been completed*/
    int completeType;

    /*If 1 then the state will not update*/
    int paused;

    /*Points to the object that the ai is to be based on*/
    void *entity;

    /*Points to a structure that contains any extra attributes needed by the ai state in order to function*/
    void *extraStructure;
} Ai_State;

Ai_State *aiState_Create(void);

void aiState_Setup(Ai_State *a,
    int group,
    int priority,
    int completeType,
    void *entity,
    void *extraStructure,
    void (*init)(Ai_State *AiState),
    void (*mainLoop)(Ai_State *AiState),
    void (*exit)(Ai_State *AiState)
    );

void aiState_Init(Ai_State *a);

int aiState_Update(Ai_State *a);

void aiState_Insert(struct list **aiList, Ai_State *a);

void aiState_SetDestroyedAll(struct list *aiStates);

void aiState_SetDestroyedGroup(struct list *aiStates, int group);

void aiState_UpdateList(struct list **states);

int aiState_SetComplete(Ai_State *a);

void aiState_ForceComplete(Ai_State *a, int complete);

int aiState_CompleteState(Ai_State *a, struct list **headList);

void aiState_Clean(Ai_State *a);

void *aiState_Delete(Ai_State *a);

#endif
