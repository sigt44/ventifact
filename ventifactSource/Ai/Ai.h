#ifndef AI_H
#define AI_H

#include "SDL/SDL.h"

#include "Ai_State.h"

#define AI_GLOBAL_STATE 0

typedef struct Ai_Module
{
    struct list *localList; /*List of local ai states*/
    Ai_State *lowestPriority; /*State with the lowest priority of the local states*/
    Ai_State *highestPriority;

    int priorityCheck; /*1 if a priority check is needed*/

    struct list *globalList; /*List of global ai states*/
} Ai_Module;

void aiModule_Setup(Ai_Module *aiMod);

int aiModule_UpdatePriority(Ai_Module *aiMod, Ai_State *state);

void aiModule_PushState(Ai_Module *aiMod, Ai_State *state);

void aiModule_StackState(Ai_Module *aiMod, Ai_State *state);

int aiModule_DestroyState(Ai_Module *aiMod, struct list **headList, struct list *currentNode, Ai_State *a);

int aiModule_DestroyStateList(Ai_Module *aiMod, struct list **headList);

int aiModule_CompleteState(Ai_Module *aiMod, struct list **headList, Ai_State *a);

int aiModule_HasAi(Ai_Module *aiMod, void (*initFunction)(Ai_State *AiState));

void aiModule_Report(Ai_Module *aiMod);

void aiModule_Update(Ai_Module *aiMod);

void aiModule_Clean(Ai_Module *aiMod);

#endif
