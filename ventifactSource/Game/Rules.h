#ifndef RULES_H
#define RULES_H

#include "../Defines.h"

#include "../Player.h"

enum
{
    VR_GAME_PROGRESS = 0,
    VR_GAME_FINISHED = 1
} VR_STATES;

typedef struct Vent_Rules
{
    int teamWon; /*The team that has won the game*/

    int teamOwnsCitadels; /*The team that currently owns all citadels in the game*/
    int destroyedTiles[TEAM_END]; /*Which team has destroyed all important tiles*/

    struct list *importantTiles; /*List of important tiles to check through*/

    int state; /*Has the game finished already?*/

    Vector2DInt keyPosition; /*Location of the most recent important thing that has happened in game*/

} Vent_Rules;

void vRules_Setup(Vent_Rules *rules, void *g);

int vRules_Update(Vent_Rules *rules, void *g);

int vRules_UpdateStats(Vent_Rules *rules, void *g, Vent_Player *player);




#endif
