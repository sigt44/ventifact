#include "Campaign.h"

#include <stdarg.h>
#include <string.h>

#include "Memory.h"
#include "File.h"

Vent_CaLvl *vCaLvl_Create(char *name, char *fileName, int hasCompleted, int timesPlayed, int timesWon, int moneyReward)
{
    Vent_CaLvl *caLvl = (Vent_CaLvl *)mem_Malloc(sizeof(Vent_CaLvl), __LINE__, __FILE__);

    strncpy(caLvl->name, name, VENT_MAXNAME);
    strncpy(caLvl->fileName, fileName, VENT_MAXNAME);

    caLvl->hasCompleted = hasCompleted;

    caLvl->timesPlayed = timesPlayed;

    caLvl->timesWon = timesWon;

    caLvl->moneyReward = moneyReward;

    caLvl->playDuration = 0;
    caLvl->score = -1;
    caLvl->quickestDuration = -1;
    caLvl->leastHealthLost = -1;

    return caLvl;
}

Vent_Campaign *vCampaign_Create(char *name, int ID, int startAtLvl, int playingLvl, int cash, int totalLvls, ...)
{
    Vent_Campaign *c = (Vent_Campaign *)mem_Malloc(sizeof(Vent_Campaign), __LINE__, __FILE__);

    va_list vLevels;
    int x;

    strncpy(c->name, name, VENT_MAXNAME);

    c->ID = ID;

    c->upToLevel = startAtLvl;

    c->playingLevel = playingLvl;

    c->totalLevels = totalLvls;

    c->lvlInfo = NULL;

    c->isCurrent = 0;

    c->cash = cash;
    c->playerLevel = 0;

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        c->unitLevel[x] = 0;
    }

    va_start(vLevels, totalLvls);

    for(x = 0; x < totalLvls; x++)
    {
        list_Stack(&c->lvlInfo, va_arg(vLevels, Vent_CaLvl *), 0);
    }

    va_end(vLevels);

    return c;
}

int vCampaign_GetTimePlayed(Vent_Campaign *c)
{
    Vent_CaLvl *caLvl = NULL;
    struct list *lvlList = c->lvlInfo;

    int timePlayed = 0;

    while(lvlList != NULL)
    {
        caLvl = lvlList->data;

        if(caLvl->playDuration > 0)
        {
            timePlayed += caLvl->playDuration;
        }

        lvlList = lvlList->next;
    }

    return timePlayed;
}

int vCampaign_GetScore(Vent_Campaign *c)
{
    Vent_CaLvl *caLvl = NULL;
    struct list *lvlList = c->lvlInfo;

    int scoreSum = 0;

    while(lvlList != NULL)
    {
        caLvl = lvlList->data;

        if(caLvl->score > 0)
            scoreSum += caLvl->score;

        lvlList = lvlList->next;
    }

    return scoreSum;
}

int vCampaign_GetQuickestTime(Vent_Campaign *c)
{
    Vent_CaLvl *caLvl = NULL;
    struct list *lvlList = c->lvlInfo;

    int timeSum = 0;

    while(lvlList != NULL)
    {
        caLvl = lvlList->data;

        if(caLvl->quickestDuration > 0)
            timeSum += caLvl->quickestDuration;

        lvlList = lvlList->next;
    }

    return timeSum;
}

int vCampaign_GetLeastHealth(Vent_Campaign *c)
{
    Vent_CaLvl *caLvl = NULL;
    struct list *lvlList = c->lvlInfo;

    int healthSum = 0;

    while(lvlList != NULL)
    {
        caLvl = lvlList->data;

        if(caLvl->leastHealthLost > 0)
            healthSum += caLvl->leastHealthLost;

        lvlList = lvlList->next;
    }

    return healthSum;
}

Vent_Campaign *vCampaign_LoadBase(char *campaignName)
{
    char *loadDIR = "../Campaigns/";

    char *campaignLoad = (char *)mem_Malloc(strlen(loadDIR) + strlen(campaignName) + strlen(".cam") + 1, __LINE__, __FILE__);

    FILE *loadFile = NULL;
    Vent_Campaign *campaign = NULL;

    strcpy(campaignLoad, loadDIR);
    strcat(campaignLoad, campaignName);
    strcat(campaignLoad, ".cam");

    loadFile = fopen(campaignLoad, "r");

    if(loadFile == NULL)
    {
        printf("Error cannot load campaign %s\n", campaignLoad);

        return NULL;
    }
    else
    {
        campaign = vCampaign_Load(loadFile);
    }

    fclose(loadFile);

    mem_Free(campaignLoad);

    return campaign;
}

void vCampaign_SaveBase(Vent_Campaign *c)
{
    char *saveDIR = "../Campaigns/";

    char *campaignSave = (char *)mem_Malloc(strlen(saveDIR) + strlen(c->name) + strlen(".cam") + 1, __LINE__, __FILE__);

    FILE *saveFile = NULL;

    strcpy(campaignSave, saveDIR);
    strcat(campaignSave, c->name);
    strcat(campaignSave, ".cam");

    saveFile = fopen(campaignSave, "w");

    if(saveFile == NULL)
    {
        printf("Error cannot save campaign %s\n", campaignSave);
    }
    else
    {
        vCampaign_Save(c, saveFile);
        fclose(saveFile);
    }

    mem_Free(campaignSave);

    return;
}

void vCampaign_Save(Vent_Campaign *c, FILE *openedSave)
{
    struct list *l = NULL;
    Vent_CaLvl *cl = NULL;

    if(c == NULL)
        return;

    l = c->lvlInfo;

    /*Name, ID, total levels, up to level, playing level, is current campaign*/
    file_Log(openedSave, 0, "%s\n%d\n%d\n%d\n%d\n%d\n",
        c->name,
        c->ID,
        c->totalLevels,
        c->upToLevel,
        c->playingLevel,
        c->isCurrent
        );

    /*Money, base player level, base unit level*/
    file_Log(openedSave, 0, "%d\n%d\n%d\n%d\n%d\n%d\n",
        c->cash,
        c->playerLevel,
        c->unitLevel[UNIT_TANK],
        c->unitLevel[UNIT_INFANTRY],
        c->unitLevel[UNIT_AIR],
        c->unitLevel[UNIT_TOWER]);

    /*Save all the level details*/
    while(l != NULL)
    {
        cl = l->data;
        /*Make sure the name contains no spaces when saved*/
        string_ReplaceChar(cl->name, ' ', '%');

        /*Save the details*/
        file_Log(openedSave, 0, "%s %s %d %d %d %d %d %d %d %d\n",
            cl->name,
            cl->fileName,
            cl->hasCompleted,
            cl->timesPlayed,
            cl->timesWon,
            cl->moneyReward,
            cl->playDuration,
            cl->score,
            cl->quickestDuration,
            cl->leastHealthLost
            );

        /*Revert back to the proper name*/
        string_ReplaceChar(cl->name, '%', ' ');

        l = l->next;
    }

    return;
}

Vent_Campaign *vCampaign_Load(FILE *load)
{
    Vent_Campaign *c = (Vent_Campaign *)mem_Malloc(sizeof(Vent_Campaign), __LINE__, __FILE__);

    int x = 0;
    Vent_CaLvl *caLvl = NULL;

    fscanf(load, "%s\n%d\n%d\n%d\n%d\n%d\n",
        c->name,
        &c->ID,
        &c->totalLevels,
        &c->upToLevel,
        &c->playingLevel,
        &c->isCurrent
        );

    /*Money, base player level, base unit level*/
    fscanf(load, "%d\n%d\n%d\n%d\n%d\n%d\n",
        &c->cash,
        &c->playerLevel,
        &c->unitLevel[UNIT_TANK],
        &c->unitLevel[UNIT_INFANTRY],
        &c->unitLevel[UNIT_AIR],
        &c->unitLevel[UNIT_TOWER]);

    c->lvlInfo = NULL;

    for(x = 0; x < c->totalLevels; x++)
    {
        caLvl = (Vent_CaLvl *)mem_Malloc(sizeof(Vent_CaLvl), __LINE__, __FILE__);

        fscanf(load, "%s %s %d %d %d %d %d %d %d %d\n",
            caLvl->name,
            caLvl->fileName,
            &caLvl->hasCompleted,
            &caLvl->timesPlayed,
            &caLvl->timesWon,
            &caLvl->moneyReward,
            &caLvl->playDuration,
            &caLvl->score,
            &caLvl->quickestDuration,
            &caLvl->leastHealthLost
            );

        /*Convert the % character back to a space*/
        string_ReplaceChar(caLvl->name, '%', ' ');

        list_Stack(&c->lvlInfo, caLvl, 0);
    }

    return c;
}

/*
    Function: vCampaign_GetLvl

    Description -
    Returns a campain level information that has the same file name as fileName.
    If a level is not found then returns NULL.

    2 arguments:
    Vent_Campaign *c - The campaign that the level is in.
    char *fileName - The name of the level file.
*/
Vent_CaLvl *vCampaign_GetLvl(Vent_Campaign *c, char *fileName)
{
    struct list *lvlList = c->lvlInfo;
    Vent_CaLvl *caLvl = NULL;

    //char *fullName = (char *)mem_Malloc(strlen(lvlName) + 1, __LINE__, __FILE__);

    //strcpy(fullName, lvlName);
    //strcat(fullName, ".map");

    while(lvlList != NULL)
    {
        caLvl = lvlList->data;

        /*Matching name*/
        if(strcmp(caLvl->fileName, fileName) == 0)
        {
            break;
        }
        else
            caLvl = NULL;

        lvlList = lvlList->next;
    }

    //mem_Free(fullName);
    return caLvl;
}

void vCampaign_Clean(Vent_Campaign *c)
{
    list_ClearAll(&c->lvlInfo);

    return;
}
