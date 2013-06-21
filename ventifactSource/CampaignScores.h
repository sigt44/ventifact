#ifndef CAMPAIGNSCORES_H
#define CAMPAIGNSCORES_H

#include <stdio.h>

#include "Defines.h"

typedef struct Vent_CampaignScoreRecord
{
    char name[VENT_MAXNAME]; /*Name of the level or campaign*/
    int topScores[VENT_MAXSCORES]; /*Top 5 scores for this lvl/campaign.*/
    char topScoreNames[VENT_MAXSCORES][VENT_MAXPLAYERNAME]; /*Top 5 scorer names*/
} Vent_CaScoreRecord;

/*Used in the campaign structure to manage the scores for each lvl and the total campaign score.*/
typedef struct Vent_CampaignScores
{
    int setup; /*If true then the structure has been setup*/
    int totalLevelRecords;
    Vent_CaScoreRecord *records;
    Vent_CaScoreRecord campaignRecord;
} Vent_CaScores;


void vCaScoreRecord_Setup(Vent_CaScoreRecord *record, char *name);

void vCaScoreRecord_SetDefaultScore(Vent_CaScoreRecord *record);

void vCaScoreRecord_Save(Vent_CaScoreRecord *record, FILE *openedFile);

void vCaScoreRecord_Load(Vent_CaScoreRecord *record, FILE *openedFile);

int vCaScoreRecord_Update(Vent_CaScoreRecord *record, char *pName, int score);

void vCaScoreRecord_Report(Vent_CaScoreRecord *record);


void vCaScores_Setup(Vent_CaScores *scores, void *vCampaign);

void vCaScores_Save(Vent_CaScores *scores);

int vCaScores_Load(Vent_CaScores *scores);

void vCaScores_Clean(Vent_CaScores *scores);

Vent_CaScoreRecord *vCaScores_GetRecord(Vent_CaScores *scores, char *levelName);

void vCaScores_Report(Vent_CaScores *scores);

#endif
