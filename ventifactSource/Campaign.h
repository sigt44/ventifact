#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <stdio.h>

#include "Common/List.h"

#include "Defines.h"
#include "CampaignScores.h"

typedef struct Vent_Campaign
{
	char name[VENT_MAXNAME];
	int ID;
	int totalLevels;
	int upToLevel;
	int playingLevel;

	int cash;
	int unitLevel[UNIT_ENDTYPE];
    int playerLevel;

	int isCurrent; /*Is this the campaign the player is currently on*/

    struct list *lvlInfo;

} Vent_Campaign;

typedef struct Vent_CampaignLvl
{
    char name[VENT_MAXNAME]; /*The name of the level as it appears on the campaign list*/
    char fileName[VENT_MAXNAME]; /*The file name of the actual level data*/

    /*Player specific stats*/
	int hasCompleted;
	int timesPlayed;
	int timesWon;

	int moneyReward;
	int playDuration; /*Total for every game in minutes*/
	int quickestDuration; /*Quickest time completed*/
	int leastHealthLost; /*Least amount of health lost*/
	int score; /*Player specific score of the level*/

} Vent_CaLvl;

Vent_CaLvl *vCaLvl_Create(char *name, char *fileName, int hasCompleted, int timesPlayed, int timesWon, int moneyReward);

Vent_Campaign *vCampaign_Create(char *name, int ID, int startAtLvl, int playingLvl, int cash, int totalLvls, ...);
void vCampaign_SaveBase(Vent_Campaign *c);

void vCampaign_Save(Vent_Campaign *c, FILE *openedSave);

int vCampaign_GetTimePlayed(Vent_Campaign *c);

int vCampaign_GetScore(Vent_Campaign *c);
int vCampaign_GetQuickestTime(Vent_Campaign *c);
int vCampaign_GetLeastHealth(Vent_Campaign *c);

Vent_Campaign *vCampaign_LoadBase(char *campaignName);

Vent_Campaign *vCampaign_Load(FILE *load);

Vent_CaLvl *vCampaign_GetLvl(Vent_Campaign *c, char *lvlName);

void vCampaign_Clean(Vent_Campaign *c);


#endif
