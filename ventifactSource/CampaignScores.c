#include "CampaignScores.h"

#include "File.h"

#include "Campaign.h"
/*
    Function: vCaScoreRecord_Setup
    Description - Setup a campaign score record.

    2 arguments:
    Vent_CaScoreRecord *record - The record to setup.
    char *name - The name of the lvl or campaign the score record corresponds to. Length must not exceed VENT_MAXNAME - 1 characters
*/
void vCaScoreRecord_Setup(Vent_CaScoreRecord *record, char *name)
{
    strncpy(record->name, name, VENT_MAXNAME - 1);
    vCaScoreRecord_SetDefaultScore(record);

    return;
}

/*
    Function: vCaScoreRecord_SetDefaultScore
    Description - Sets all the scores and scorer names of a record to defaults.

    1 argument:
    Vent_CaScoreRecord *record - The record to set default scores to.
*/
void vCaScoreRecord_SetDefaultScore(Vent_CaScoreRecord *record)
{
    int x = 0;
    for(x = 0; x < VENT_MAXSCORES; x++)
    {
        record->topScores[x] = 0;
        record->topScoreNames[x][0] = record->topScoreNames[x][1] = record->topScoreNames[x][2] = '*';
        record->topScoreNames[x][3] = '\0';
    }

    return;
}

/*
    Function: vCaScoreRecord_Update
    Description - Checks if a score is high enough to go in the record and if so
    it places/replaces it in. Returns 1 if a new high score has been made, otherwise
    returns 0;

    3 arguments:
    Vent_CaScoreRecord *record - The record to check the score in.
    char *pName - The name of the player whose score is to be checked, max length VENT_MAXPLAYERNAME - 1.
    int score - The score that is being checked.
*/
int vCaScoreRecord_Update(Vent_CaScoreRecord *record, char *pName, int score)
{
    int x = 0;

    /*Highest score at index 0*/
    for(x = 0; x < VENT_MAXSCORES; x++)
    {
        if(score > record->topScores[x])
        {
            record->topScores[x] = score;
            strncpy(record->topScoreNames[x], pName, VENT_MAXPLAYERNAME - 1);
            return 1;
        }
    }

    /*Score was not high enough*/
    return 0;
}

void vCaScoreRecord_Save(Vent_CaScoreRecord *record, FILE *openedFile)
{
    int x = 0;

    /*Write out the record name*/
    file_Log(openedFile, 0, "%s\n", record->name);

    /*Write out the top scores*/
    for(x = 0; x < VENT_MAXSCORES; x++)
    {
        file_Log(openedFile, 0, "%s %d\n", record->topScoreNames[x], record->topScores[x]);
    }

    return;
}

void vCaScoreRecord_Load(Vent_CaScoreRecord *record, FILE *openedFile)
{
    int x = 0;

    /*Read the record name*/
    file_GetString(record->name, VENT_MAXNAME, openedFile);

    /*Read the top scores*/
    for(x = 0; x < VENT_MAXSCORES; x++)
    {
        file_GetSubString(record->topScoreNames[x], VENT_MAXNAME, ' ', openedFile);
        fscanf(openedFile, "%d\n", &record->topScores[x]);
    }

    return;
}

void vCaScoreRecord_Report(Vent_CaScoreRecord *record)
{
    int x = 0;

    /*Write out the record name*/
    file_Log(ker_Log(), P_OUT, "%s:\n", record->name);

    /*Write out the top scores*/
    for(x = 0; x < VENT_MAXSCORES; x++)
    {
        file_Log(ker_Log(), P_OUT, "\t%s - %d\n", record->topScoreNames[x], record->topScores[x]);
    }

    return;
}

void vCaScores_Setup(Vent_CaScores *scores, void *vCampaign)
{
    int x = 0;
    Vent_Campaign *campaign = vCampaign;
    struct list *lvlList = NULL;
    Vent_CaLvl *lvl = NULL;

    scores->setup = 0;

    if(vCampaign == NULL)
    {
        vCaScoreRecord_Setup(&scores->campaignRecord, "No Campaign");
        scores->totalLevelRecords = 0;
    }
    else
    {
        lvlList = campaign->lvlInfo;
        scores->totalLevelRecords = campaign->totalLevels;

        /*Make sure that the campaign knows how many levels it has*/
        if(list_Size(lvlList) != campaign->totalLevels)
        {
            printf("Error corrupt campaign with %d lvls verses %d lvl list size\n", campaign->totalLevels, list_Size(lvlList));
            return;
        }

        /*Setup the campaign total high score record*/
        vCaScoreRecord_Setup(&scores->campaignRecord, campaign->name);
    }

    if(scores->totalLevelRecords <= 0)
    {
        scores->records = NULL;
    }
    else
    {
        scores->records = (Vent_CaScoreRecord *)mem_Malloc(sizeof(Vent_CaScores) * scores->totalLevelRecords, __LINE__, __FILE__);

        /*Fill the scores for each level with default values*/
        for(x = 0; x < scores->totalLevelRecords; x++)
        {
            lvl = lvlList->data;
            vCaScoreRecord_Setup(&scores->records[x], lvl->name);
            lvlList = lvlList->next;
        }
    }

    scores->setup = 1;

    return;
}

void vCaScores_Save(Vent_CaScores *scores)
{
    int x = 0;
    char *saveDIR = "../Campaigns/";

    char *scoreSave = (char *)mem_Malloc(strlen(saveDIR) + strlen(scores->campaignRecord.name) + strlen(".scr") + 1, __LINE__, __FILE__);

    FILE *saveFile = NULL;

    strcpy(scoreSave, saveDIR);
    strcat(scoreSave, scores->campaignRecord.name);
    strcat(scoreSave, ".scr");

    saveFile = fopen(scoreSave, "w");

    if(saveFile == NULL)
    {
        printf("Error cannot save campaign scores for %s\n", scoreSave);
    }
    else
    {
        /*Write out the total level records*/
        file_Log(saveFile, 0, "%d\n", scores->totalLevelRecords);
        /*Main campaign total score*/
        vCaScoreRecord_Save(&scores->campaignRecord, saveFile);

        /*Each level scores*/
        for(x = 0; x < scores->totalLevelRecords; x++)
        {
            vCaScoreRecord_Save(&scores->records[x], saveFile);
        }

        fclose(saveFile);
    }

    mem_Free(scoreSave);

    return;
}

/*
    Function: vCaScores_Load
    Description - Attempts to load in the high scores for a campaign.
    The Vent_CaScores structure must have already been setup correctly so it is
    associated with a campaign.

    Returns 1 if the scores save file for the campaign was not found.

    1 argument:
    Vent_CaScores *scores - The high score structure that will be updated froma save file.
*/
int vCaScores_Load(Vent_CaScores *scores)
{
    int x = 0;
    char *saveDIR = "../Campaigns/";

    char *scoreSave = (char *)mem_Malloc(strlen(saveDIR) + strlen(scores->campaignRecord.name) + strlen(".scr") + 1, __LINE__, __FILE__);

    FILE *saveFile = NULL;
    int totalLvls = 0;
    int state = 0; /*Return state, 0 if loaded, 1 if save not found*/

    strcpy(scoreSave, saveDIR);
    strcat(scoreSave, scores->campaignRecord.name);
    strcat(scoreSave, ".scr");

    saveFile = fopen(scoreSave, "r");

    if(saveFile == NULL)
    {
        file_Log(ker_Log(), P_OUT, "Warning cannot load campaign scores from %s\n", scoreSave);
        state = 1;
    }
    else
    {
        /*First read in the total levels the campaign has*/
        fscanf(saveFile, "%d\n", &totalLvls);

        /*If this does not match what the high score structure has been setup with, then
        there is some error. Maybe the campaign is not correct or the high score save refers to
        an old out of date version*/
        if(totalLvls != scores->totalLevelRecords)
        {
            file_Log(ker_Log(), P_OUT, "Error: campaign high score save from %s does not match campaigns total levels (%d vs %d)\n", scoreSave, totalLvls, scores->totalLevelRecords);
        }
        else
        {
            /*Load in the main campaign total score*/
            vCaScoreRecord_Load(&scores->campaignRecord, saveFile);

            /*Now load in the individual level scores*/
            for(x = 0; x < scores->totalLevelRecords; x++)
            {
                vCaScoreRecord_Load(&scores->records[x], saveFile);
            }
        }

        state = 0;

        fclose(saveFile);
    }

    mem_Free(scoreSave);

    return state;
}

/*
    Function: vCaScores_GetRecord
    Description - Returns a pointer to the record corresponding
    to the level with name 'levelName'.

    Returns NULL if it cannot find a level with that name.

    2 argument2:
    Vent_CaScores *scores - The high score structure that contains the level record.
    char *levelName - The name of the level to get the high score record for.
*/
Vent_CaScoreRecord *vCaScores_GetRecord(Vent_CaScores *scores, char *levelName)
{
    int x = 0;

    for(x = 0; x < scores->totalLevelRecords; x++)
    {
        if(strcmp(scores->records[x].name, levelName) == 0)
            return &scores->records[x];
    }

    return NULL;
}

void vCaScores_Report(Vent_CaScores *scores)
{
    int x = 0;
    file_Log(ker_Log(), P_OUT, "High Score report:\n");
    vCaScoreRecord_Report(&scores->campaignRecord);
    file_Log(ker_Log(), P_OUT, "\n");

    for(x = 0; x < scores->totalLevelRecords; x++)
    {
        vCaScoreRecord_Report(&scores->records[x]);
    }

    return;
}

void vCaScores_Clean(Vent_CaScores *scores)
{
   if(scores->records != NULL)
   {
       mem_Free(scores->records);
       scores->records = NULL;
   }

   scores->totalLevelRecords = 0;
   scores->setup = 0;

   return;
}
