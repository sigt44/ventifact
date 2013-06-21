#include "Rules.h"

#include "VentGame.h"
#include "../Buildings/Buildings.h"
#include "../Options.h"

/*
    Function: vRules_Setup

    Description -
    Set up the variables needed for the game rules.

    2 arguments:
    Vent_Rules *rules - The game rule structure.
    void *g - The current Vent_Game structure.
*/
void vRules_Setup(Vent_Rules *rules, void *g)
{
    Vent_Game *game = g;
    int x = 0;

    rules->teamWon = -1;
    rules->teamOwnsCitadels = -1;

    for(x = 0; x < game->totalTeams; x++)
    {
        rules->destroyedTiles[x] = 0;
    }

    rules->importantTiles = game->level->importantTiles;

    rules->state = VR_GAME_PROGRESS;

    vector2DInt_SetZero(&rules->keyPosition);

    return;
}


/*
    Function: vRules_CheckTiles

    Description -
    Returns 1 if there are still important tiles to destroy. 0 if not.

    2 arguments:
    int team - the team that is checked for tiles it needs to destroy.
    struct list *tileList - list of tiles that should contain any important tiles.
*/
static int vRules_CheckTiles(int team, struct list *tileList)
{
    Vent_Tile *tile = NULL;

    while(tileList != NULL)
    {
        tile = tileList->data;

        /*If there is still an important tile left to destroy*/
        if(tile->base.team != team && tile->destroyed != 1 && flag_Check(&tile->base.stateFlags, TILESTATE_IMPORTANT) == 1)
        {
            return 1;
        }

        tileList = tileList->next;
    }

    return 0;
}

/*
    Function: vRules_Update

    Description -
    Decide whether the game has been completed and choose side has won.

    2 arguments:
    Vent_Rules *rules - The game rule structure.
    void *g - The current Vent_Game structure.
*/
int vRules_Update(Vent_Rules *rules, void *g)
{
    Vent_Game *game = g;
    Vent_Building *citadel = NULL;

    int x = 0;

    /*If the game is already complete then return early*/
    if(rules->state == VR_GAME_FINISHED)
    {
        return VR_GAME_FINISHED;
    }

    /*Check if player is detroyed*/
    if(game->playerUnit->destroyed == UNIT_DEATH)
    {
        rules->state = VR_GAME_FINISHED;
        rules->teamWon = game->side[game->playerUnit->team].enemy->team;

        rules->keyPosition = game->playerUnit->iPosition;

        return VR_GAME_FINISHED;
    }

    /*Check if team has all citadels on the map*/
    rules->teamOwnsCitadels = -1;

    for(x = 0; x < game->totalTeams; x++)
    {
        if(game->side[x].captures >= game->totalCitadels)
        {
            if(game->side[x].citadels != NULL)
            {
                citadel = game->side[x].citadels->data;

                rules->keyPosition = citadel->middlePosition;
            }

            rules->teamOwnsCitadels = x;
        }
    }

    /*Check if team has destroyed all important tiles on the map*/
    for(x = 0; x < game->totalTeams; x++)
    {
        rules->destroyedTiles[x] = 0;

        if(vRules_CheckTiles(x, rules->importantTiles) == 0)
        {
            rules->destroyedTiles[x] = 1;
        }
    }

    /*Check if any team has won the game*/
    for(x = 0; x < game->totalTeams; x++)
    {
        if(rules->destroyedTiles[x] == 1 && rules->teamOwnsCitadels == x)
        {
            rules->teamWon = x;
            rules->state = VR_GAME_FINISHED;

            return VR_GAME_FINISHED;
        }
    }

    return VR_GAME_PROGRESS;
}

int vRules_UpdateStats(Vent_Rules *rules, void *g, Vent_Player *player)
{
    int score = 0;
    Vent_Game *game = g;
    Vent_Stats *stats = &game->stats;
    Vent_CaLvl *caLvl = NULL;
    Vent_CaScoreRecord *record = NULL;

    vStats_Report(stats);

    vStats_Add(&player->gameStats, stats);

    printf("Player total stats - \n");
    vStats_Report(&player->gameStats);

    /*Game exited without finishing, no need to update campaign stats*/
    if(rules->state == VR_GAME_PROGRESS)
    {
        return 1;
    }

    /*If the player is not in a campaign return early*/
    if(player->playCampaign == 0)
    {
        return 1;
    }

    caLvl = vCampaign_GetLvl(player->campaign, player->levelChosen);

    if(caLvl == NULL)
    {
        printf("Error invalid lvl in campaign %s\n", player->levelChosen);
        return 1;
    }

    caLvl->timesPlayed ++;
    caLvl->playDuration += stats->playDuration;

    /*If the player won*/
    if(rules->teamWon == game->playerUnit->team)
    {
        player->campaign->cash += caLvl->moneyReward;
        caLvl->moneyReward = (caLvl->moneyReward/2) - ((caLvl->moneyReward/2) % 10);

        player->gamesWon ++;

        if(caLvl->hasCompleted == 0)
        {
            player->campaign->upToLevel ++;
        }

        caLvl->hasCompleted = 1;
        caLvl->timesWon ++;

        if(caLvl->quickestDuration < 0 || (stats->playDuration < caLvl->quickestDuration))
        {
            caLvl->quickestDuration = stats->playDuration;
        }

        if(caLvl->leastHealthLost < 0 || (stats->healthLost < caLvl->leastHealthLost))
        {
            caLvl->leastHealthLost = stats->healthLost;
        }

        score = vStats_CalcScore(stats);

        if(caLvl->score < 0 || (score > caLvl->score))
        {
            caLvl->score = score;
        }

        player->playedGame = 2;
    }
    else
    {
        player->playedGame = 1;
        player->gamesLost ++;
    }

    /*Check for high score updates*/

    /*Make sure player does not have cheats on*/
    if(ve_Options.cheat == 0)
    {
        record = vCaScores_GetRecord(&player->campaignScores, caLvl->name);

        if(record == NULL)
        {
            file_Log(ker_Log(), P_OUT, "Error: Cannot get score record for %s\n", caLvl->name);
        }
        else if(vCaScoreRecord_Update(record, player->pName, caLvl->score) == 1)
        {
            file_Log(ker_Log(), P_OUT, "New high score record for %s!\n", caLvl->name);
            vCaScores_Save(&player->campaignScores);
        }
    }


    return 0;
}
