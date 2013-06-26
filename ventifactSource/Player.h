#ifndef PLAYER_H
#define PLAYER_H

#include "Defines.h"
#include "Campaign.h"
#include "Time/Timer.h"

#include "Level/Level.h"
#include "Game/Stats.h"

typedef struct playerState
{
    char pName[VENT_MAXPLAYERNAME];

    Vent_Campaign *campaign;
    char lastPlayedCampaign[VENT_MAXNAME];
    Vent_CaScores campaignScores;

    Vent_Stats gameStats;

    unsigned int gamesWon;
    unsigned int gamesLost;

    Timer totalTime;

    int playedGame;

    int startingUnit;

    unsigned int playCampaign;
    int totalCampaigns;

    char *levelChosen;
    char *levelDirectory;

    struct list *campaignList; /*holds the data for the current state of all campaigns the player has played*/

} Vent_Player;

void vPlayer_Setup(Vent_Player *p, char *name3Char);

void vPlayer_AddCampaign(Vent_Player *p, Vent_Campaign *c, int isCurrent);

void vPlayer_SetCampaign(Vent_Player *p, Vent_Campaign *c);

Vent_Campaign *vPlayer_GetCampaign(Vent_Player *p, char *campaignName);

void vPlayer_LoadCampaignScores(Vent_Player *player);

void vPlayer_LoadCampaign(Vent_Player *player, char *campaignFileName);


void vPlayer_Save(Vent_Player *p);

int vPlayer_CheckSaved(char *name3Char);

int vPlayer_Load(Vent_Player *p, char *name3Char);

void vPlayer_Clean(Vent_Player *p);

#endif
