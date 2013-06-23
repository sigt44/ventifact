#include "Player.h"
#include <string.h>

#include "File.h"

#include "Options.h"

void vPlayer_Setup(Vent_Player *p, char *name3Char)
{
    int x = 0;

    strncpy(p->pName, name3Char, 4);

    vStats_Setup(&p->gameStats);
    p->gamesWon = 0;
    p->gamesLost = 0;

    p->totalTime = timer_Setup(NULL, 0, 0, 1);

    p->playedGame = 0;

    p->startingUnit = UNIT_TANK;

    p->levelChosen = "None";

    p->campaignList = NULL;
    p->totalCampaigns = 0;

    p->playCampaign = 0;
    p->campaign = NULL;
    strcpy(p->lastPlayedCampaign, "None");

    vCaScores_Setup(&p->campaignScores, NULL);

    return;
}

/*
    Function: vPlayer_AddCampaign

    Description -
    Adds a campaign for the player to select. If isCurrent == 1 then this
    campaign will be the current one the player will play.

    3 arguments:
    Vent_Player *p - Player to add the campaign to
    Vent_Campaign *c - Campaign to add
    int isCurrent - If 1 then campaign will be set as the current one
*/
void vPlayer_AddCampaign(Vent_Player *p, Vent_Campaign *c, int isCurrent)
{
    p->totalCampaigns ++;

    list_Push(&p->campaignList, c, 0);

    if(isCurrent == 1)
    {
        vPlayer_SetCampaign(p, c);
    }

    return;
}

/*
    Function: vPlayer_GetCampaign

    Description -
    Finds and returns a campaign a player is on based on the campaign name.
    If no campaigns with that name are found then returns NULL.

    2 arguments:
    Vent_Player *p - The player to find the campaign in.
    char *campaignName - The campaigns name to find.
*/
Vent_Campaign *vPlayer_GetCampaign(Vent_Player *p, char *campaignName)
{
    Vent_Campaign *campaign = NULL;
    struct list *campaignList = p->campaignList;

    while(campaignList != NULL)
    {
        campaign = campaignList->data;

        if(strcmp(campaign->name, campaignName) == 0)
            return campaign;

        campaignList = campaignList->next;
    }

    return NULL;
}


void vPlayer_SetCampaign(Vent_Player *p, Vent_Campaign *c)
{
    /*Turn off current campaign*/
    if(p->campaign != NULL)
    {
        p->campaign->isCurrent = 0;
    }

    p->campaign = c;

    if(p->campaign != NULL)
    {
        c->isCurrent = 1;
        p->playCampaign = 1;

        strcpy(p->lastPlayedCampaign, c->name);
    }
    else
    {
        p->playCampaign = 0;
        strcpy(p->lastPlayedCampaign, "None");
    }

    return;
}

void vPlayer_LoadCampaignScores(Vent_Player *player)
{
    /*Clear the high score table*/
    if(player->campaignScores.setup == 1)
    {
        vCaScores_Clean(&player->campaignScores);
    }

    /*Setup and if possible load the high score table for the chosen campaign*/
    if(player->playCampaign == 1)
    {
        vCaScores_Setup(&player->campaignScores, player->campaign);

        if(vCaScores_Load(&player->campaignScores) == 1)
        {
            /*If no high score save was found for the campaign, create one*/
            vCaScores_Save(&player->campaignScores);
        }

        vCaScores_Report(&player->campaignScores);
    }

    return;
}

void vPlayer_LoadCampaign(Vent_Player *player, char *campaignName)
{
    Vent_Campaign *campaign = NULL;

    printf("Selected %s\n", campaignName);

    if(strcmp("None", campaignName) == 0)
    {
        player->playCampaign = 0;
    }
    else
    {
        /*See if the player has already played that campaign*/
        campaign = vPlayer_GetCampaign(player, campaignName);

        if(campaign != NULL)
        {
            /*If so just set that campaign as active*/
            vPlayer_SetCampaign(player, campaign);

            player->playCampaign = 1;

            printf("%s loaded from player\n", campaign->name);
        }
        else
        {
            /*Otherwise load it into the player save and set it as active*/
            campaign = vCampaign_LoadBase(campaignName);

            if(campaign != NULL)
            {
                vPlayer_AddCampaign(player, campaign, 1);

                player->playCampaign = 1;
                printf("%s loaded from file\n", campaign->name);
            }
            else
            {
                player->playCampaign = 0;
                printf("Unabled to load %s campaign\n", campaignName);
            }
        }

    }

    vPlayer_LoadCampaignScores(player);

    return;
}

void vPlayer_Save(Vent_Player *p)
{
    const int saveID = 2;
    FILE *save = NULL;
    char saveDIR[136];
    char saveEXT[6] = {".save"};

    int x = 0;
    struct list *campaignList = p->campaignList;
    strncpy(saveDIR, kernel_GetPath("PTH_VentSaves"), 127);
    strcat(saveDIR, p->pName);
    strcat(saveDIR, saveEXT);

    printf("Saving player %s\n", saveDIR);

    save = file_Open(saveDIR, "w");

    /*Save ID, Name*/
    file_Log(save, 0, "%d %s\n", saveID, p->pName);

    /*Game stats*/
    vStats_Save(&p->gameStats, save);

    /*Games won, games lost, time played*/
    file_Log(save, 0, "\n%d\n%d\n%d\n",
        p->gamesWon,
        p->gamesLost,
        timer_Get_Passed(&p->totalTime, 1)
        );

    /*Total campaigns, last played*/
    file_Log(save, 0, "%d %s\n", p->totalCampaigns, p->lastPlayedCampaign);

    /*Save all the campaign data*/
    for(x = 0; x < p->totalCampaigns; x++)
    {
        vCampaign_Save(campaignList->data, save);

        campaignList = campaignList->next;
    }

    file_Close(save);

    strcpy(ve_Options.lastSave, p->pName);

    return;
}

int vPlayer_CheckSaved(char *name3Char)
{
    FILE *load = NULL;
    char saveDIR[136];
    char saveEXT[6] = ".save";

    strncpy(saveDIR, kernel_GetPath("PTH_VentSaves"), 127);
    strcat(saveDIR, name3Char);
    strcat(saveDIR, saveEXT);

    load = fopen(saveDIR, "r");

    if(load == NULL)
    {
        return 0;
    }

    fclose(load);

    return 1;
}

int vPlayer_Load(Vent_Player *p, char *name3Char)
{
    const int loadID = 2;
    int saveID = 0;
    FILE *load = NULL;
    char saveDIR[136];
    char saveEXT[6] = {".save"};
    int timePassed = 0;

    int x = 0;

    Vent_Campaign *campaign = NULL;

    strncpy(saveDIR, kernel_GetPath("PTH_VentSaves"), 127);
    strcat(saveDIR, name3Char);
    strcat(saveDIR, saveEXT);

    vPlayer_Setup(p, "000");

    load = fopen(saveDIR, "r");

    if(load == NULL)
    {
        printf("Error: Could not load %s\n", saveDIR);
        return 0;
    }

    /*ID, Name*/
    fscanf(load, "%d %s\n", &saveID, p->pName);

    if(saveID != loadID)
    {
        printf("Warning saveID (%d) is not the same as loadID (%d)\n", saveID, loadID);
    }

    /*Game stats*/
    vStats_Load(&p->gameStats, load);

    /*Stats, bullets fired, units destroyed, games won, games lost, time played*/
    fscanf(load, "%d\n%d\n%d\n",
        &p->gamesWon,
        &p->gamesLost,
        &timePassed
        );

    //printf("Time passed %d\n", timePassed);

    /*Total campaigns, last played campaign*/
    fscanf(load, "%d ", &p->totalCampaigns);
    file_GetString(p->lastPlayedCampaign, VENT_MAXNAME, load);

    p->campaignList = NULL;

    /*Load in the campaigns data*/
    if(p->totalCampaigns == 0)
    {
        vPlayer_SetCampaign(p, NULL);
    }
    else
    {
        for(x = 0; x < p->totalCampaigns; x++)
        {
            campaign = vCampaign_Load(load);

            if(campaign == NULL)
            {
                printf("Error failed to load a campaign\n");
            }

            p->totalCampaigns --;
            vPlayer_AddCampaign(p, campaign, campaign->isCurrent);
        }
    }

    /*Setup the timer with game played length*/
    p->totalTime = timer_Setup(NULL, -timePassed, 0, 1);

    p->levelChosen = "None";

    /*Load the campaign scores*/
    vPlayer_LoadCampaignScores(p);

    fclose(load);

    return 1;
}

void vPlayer_Clean(Vent_Player *p)
{
    /*Clear the high score table*/
    if(p->campaignScores.setup == 1)
    {
        vCaScores_Clean(&p->campaignScores);
    }

    while(p->campaignList != NULL)
    {
        vCampaign_Clean(p->campaignList->data);

        mem_Free(p->campaignList->data);

        list_Pop(&p->campaignList);
    }

    p->campaign = NULL;

    return;
}
