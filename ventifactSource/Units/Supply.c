#include "Supply.h"

#include "Graphics/SurfaceManager.h"
#include "Kernel/Kernel_State.h"
#include "Memory.h"
#include "Collision.h"
#include "File.h"

#include "Units.h"
#include "../Ai/Units/Ai_UnitAttack.h"
#include "../Ai/Units/Ai_UnitFunction.h"
#include "../Ai/Units/Ai_UnitMove.h"

Data_Struct *vSupply_AiToData_AttackPlayer(void)
{
    return NULL;
}

Data_Struct *vSupply_AiToData_AttackBuilding(void)
{
    return NULL;
}

Data_Struct *vSupply_AiToData_DefendPlayer(void)
{
    return NULL;
}

Data_Struct *vSupply_AiToData_Side(void)
{
    return NULL;
}

Data_Struct *vSupply_AiToData_Hold(int originX, int originY, int rangeSqr)
{
    Data_Struct *aiData = dataStruct_CreateType("ddd", originX, originY, rangeSqr);

    return aiData;
}

Data_Struct *vSupply_AiToData_Patrol(int loopType, struct list *patrolList)
{
    int totalData = (list_Size(patrolList) * 3) + 1;
    int index = 0;

    AiUnitPatrol *patrol = NULL;

    Data_Struct *aiData = dataStruct_CreateEmpty(totalData);

    /*First insert the loop type of the patrol*/
    dataStruct_InsertDataType(aiData, &index, "d", loopType);

    /*Then insert the patrol positions and type*/
    while(patrolList != NULL)
    {
        patrol = patrolList->data;

        dataStruct_InsertDataType(aiData, &index, "ddd", patrol->positionX, patrol->positionY, patrol->pauseTime);

        patrolList = patrolList->next;
    }

    return aiData;
}

Data_Struct *vSupply_GetDefaultAiData(int aiState)
{
    Data_Struct *aiData = NULL;

    switch(aiState)
    {
        default:
        case SUPPLY_AI_NONE:

        aiData = NULL;

        break;

        case SUPPLY_AI_HOLD:

        aiData = vSupply_AiToData_Hold(0, 0, 0);

        break;

        case SUPPLY_AI_ATTACKPLAYER:

        aiData = vSupply_AiToData_AttackPlayer();

        break;

        case SUPPLY_AI_ATTACKBUILDING:

        aiData = vSupply_AiToData_AttackBuilding();

        break;

        case SUPPLY_AI_DEFENDPLAYER:

        aiData = vSupply_AiToData_DefendPlayer();

        break;

        case SUPPLY_AI_PATROL:

        aiData = vSupply_AiToData_Patrol(0, NULL);

        break;

        case SUPPLY_AI_SIDE:

        aiData = vSupply_AiToData_Side();

        break;
    }

    return aiData;
}

Data_Struct *vSupply_CopyAi(Vent_Supply *s)
{
    Data_Struct *aiData = NULL;

    switch(s->aiState)
    {
        default:
        case SUPPLY_AI_NONE:

        aiData = NULL;

        break;

        case SUPPLY_AI_HOLD:

        aiData = dataStruct_Copy(s->aiData);

        break;

        case SUPPLY_AI_ATTACKPLAYER:

        aiData = vSupply_AiToData_AttackPlayer();

        break;

        case SUPPLY_AI_ATTACKBUILDING:

        aiData = vSupply_AiToData_AttackBuilding();

        break;

        case SUPPLY_AI_DEFENDPLAYER:

        aiData = vSupply_AiToData_DefendPlayer();

        break;

        case SUPPLY_AI_PATROL:

        aiData = dataStruct_Copy(s->aiData);

        break;

        case SUPPLY_AI_SIDE:

        aiData = vSupply_AiToData_Side();

        break;
    }

    return aiData;
}

static int vSupply_GiveUnitAi(Vent_Supply *s, Vent_Unit *unit)
{
    int x = 0;
    struct list *patrolList = NULL;

    switch(s->aiState)
    {
        default:
        case SUPPLY_AI_NONE:

        break;

        case SUPPLY_AI_HOLD:

        vUnit_GiveAi(unit,
            aiState_UnitAttackArea_Setup(aiState_Create(),
            AI_GLOBAL_STATE + 1,    /*group*/
            1,                      /*priority*/
            AI_COMPLETE_SINGLE,     /*complete type*/
            unit,                      /*entity*/
            s->game,                   /*game*/
            *(int *)s->aiData->dataArray[0],            /*x-axis origin of area*/
            *(int *)s->aiData->dataArray[1],            /*y-axis origin of area*/
            *(int *)s->aiData->dataArray[2], /*range of area (squared)*/
            -1                      /*max time to wait at area*/
            )
        );

        break;

        case SUPPLY_AI_ATTACKPLAYER:

        vUnit_GiveAi(unit,
            aiState_UnitAttackUnit_Setup(aiState_Create(),
            AI_GLOBAL_STATE + 1,          /*group*/
            1,                      /*priority*/
            AI_COMPLETE_SINGLE,      /*complete type*/
            unit,                      /*entity*/
            s->game,               /*game*/
            s->game->playerUnit    /*player unit*/
            )
        );

        break;

        case SUPPLY_AI_ATTACKBUILDING:

        vUnit_GiveAi(unit,aiState_UnitAttackBuilding_Setup(aiState_Create(),
            AI_GLOBAL_STATE + 1,    /*group*/
            1,                      /*priority*/
            AI_COMPLETE_SINGLE,     /*complete type*/
            unit,                   /*entity*/
            s->game                 /*game*/
            )
        );

        break;

        case SUPPLY_AI_DEFENDPLAYER:

        vUnit_GiveAi(unit,
             aiState_UnitDefendEntity_Setup(aiState_Create(),
             AI_GLOBAL_STATE + 1,    /*group*/
             1,                      /*priority*/
             AI_COMPLETE_SINGLE,     /*complete type*/
             unit,                      /*Entity*/
             s->game->playerUnit,       /*Entity to defend*/
             ENTITY_UNIT,            /*If entity is unit or building*/
             1,                      /*Should the unit always follow the entity? (1)*/
             s->game->level,            /*Level the unit is in*/
             s->game)                   /*Current game*/
        );

        break;

        case SUPPLY_AI_PATROL:

        patrolList = NULL;

        /*For the total amount of patrol points*/
        for(x = 0; x < (s->aiData->amount - 1)/3; x++)
        {
            /*Insert the point to a list*/
            list_Stack(&patrolList, aiPatrol_Create(*(int *)s->aiData->dataArray[(x*3) + 1], *(int *)s->aiData->dataArray[(x*3) + 2], *(int *)s->aiData->dataArray[(x*3) + 3]), 0);
        }

        vUnit_GiveAi(unit,
            aiState_UnitPatrol_Setup(aiState_Create(),
                    AI_GLOBAL_STATE + 1,    /*group*/
                    1,                      /*priority*/
                    AI_COMPLETE_SINGLE,     /*complete type*/
                    unit,                      /*Entity*/
                    patrolList,                 /*List of patrol points*/
                    -1,                             /*Number of patrol loops (-1) for infinite*/
                    *(int *)s->aiData->dataArray[0],       /*How should the patrol find the route back to the start*/
                    &s->game->gTimer) /*Source time for the pause timer*/
        );

        break;

        case SUPPLY_AI_SIDE:

        /*Let the side ai choose what to do with the unit*/
        //list_Push(&s->game->side[unit->team].unitsNeedAi, unit, 0);
        unit->trackStat = 1;

        break;
    }

    if(s->aiState != SUPPLY_AI_NONE)
    {
        /*If any of the states above get completed, then allow the side ai to take over*/
        vUnit_GiveAi(unit,
            aiState_UnitReportAi_Setup(aiState_Create(),
            AI_GLOBAL_STATE + 2,    /*group*/
            2,                  /*priority*/
            AI_COMPLETE_SINGLE, /*complete type*/
            unit,                  /*entity*/
            NULL,         /*ai type tracking*/
            &s->game->side[unit->team],       /*side of the unit*/
            0             /*exit as soon as it is run*/
            )
        );
    }

    return 0;
}

Vent_Supply *vSupply_Create(Vent_Game *game, Timer *srcTime, int amount, int respawnTime, int startSpawned, int waitForDeath, int team, int type, int xLoc, int yLoc,  int level, int aiState, Data_Struct *aiData)
{
    Vent_Supply *newSupply = (Vent_Supply *)mem_Malloc(sizeof(Vent_Supply), __LINE__, __FILE__);

    vSupply_Setup(newSupply, game, srcTime, amount, respawnTime, startSpawned, waitForDeath, team, type, xLoc, yLoc, level, aiState, aiData);

    return newSupply;
}

int vSupply_CreateUnit(Vent_Supply *s)
{
    s->sponserUnit = vUnit_Create(s->oX, s->oY, s->team, s->type, 0, 0, &s->game->side[s->team].unitAttributes[s->type], s->supplyTime.srcTime);

    vSupply_GiveUnitAi(s, s->sponserUnit);

    vGame_AddUnit(s->game, s->sponserUnit);

    return 0;
}

void vSupply_Setup(Vent_Supply *s, Vent_Game *game, Timer *srcTime, int amount, int respawnTime, int startSpawned, int waitForDeath, int team, int type, int xLoc, int yLoc, int level, int aiState, Data_Struct *aiData)
{
    s->game = game;
    s->respawnAmount = amount;

    /*if the supply only spawns one unit, no need for further updates*/
    if(s->respawnAmount == 0)
        s->finished = 1;
    else
        s->finished = 0;

    s->supplyTime = timer_Setup(srcTime, 0, respawnTime, 1);

    s->deathWait = waitForDeath;

    s->team = team;
    s->type = type;

    s->oX = xLoc;
    s->oY = yLoc;

    s->level = level;

    s->aiState = aiState;
    s->aiData = aiData;

    s->startSpawned = startSpawned;

    if(startSpawned == 0)
        s->sponserUnit = NULL;
    else if(game != NULL)
    {
        vSupply_CreateUnit(s);
    }

    return;
}

int vSupply_Update(Vent_Supply *s)
{
    if(s->finished == 1)
        return 0;
    /*if(s->sponserUnit != NULL && s->oX == 957 && s->oY == 1345)
        printf("supply %d , %d\n", timer_Get_Remain(&s->supplyTime), s->sponserUnit->destroyed);*/
    /*if the supply needs to wait until a unit has been destroyed and that unit has not been*/
    if(s->sponserUnit != NULL && s->deathWait == 1)
    {
        if(s->sponserUnit->destroyed != UNIT_DEATH)
        {
            timer_Pause(&s->supplyTime);
            return 0;
        }
        else if(timer_Get_Paused(&s->supplyTime))
        {
            s->sponserUnit = NULL;
            timer_Resume(&s->supplyTime);
        }
    }

    timer_Calc(&s->supplyTime);

    if(timer_Get_Remain(&s->supplyTime) < 0)
    {
        /*supply is ready to spawn another unit*/
        timer_Start(&s->supplyTime);

        if(s->respawnAmount > 0)
            s->respawnAmount --;

        if(s->respawnAmount == 0)
            s->finished = 1;

        vSupply_CreateUnit(s);

        return 1;
    }

    return 0;
}

int vSupply_CheckFinished(Vent_Supply *s, int free)
{
    if(s->finished == 1)
    {
        if(free == 1)
        {
            vSupply_Clean(s);
            mem_Free(s);
        }

        return 1;
    }

    return 0;
}

void vSupply_Draw(Vent_Supply *s, Camera_2D *c, SDL_Surface *destination)
{
    SDL_Surface *unitSurface = ve_Surfaces.units[s->type][s->team][D_UP];

    if(c != NULL)
    {
        surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, camera2D_RelativeX(c, s->oX), camera2D_RelativeY(c, s->oY), unitSurface, destination, NULL);
    }
    else
    {
        surfaceMan_Push(gKer_DrawManager(), 0, VL_HUD, 0, s->oX, s->oY, unitSurface, destination, NULL);
    }

    return;
}

Vent_Supply *vSupply_GetCollision(struct list *supplyList, int colX, int colY, int colW, int colH)
{
    Vent_Supply *supply = NULL;
    int supW = 0;
    int supH = 0;

    while(supplyList != NULL)
    {
        supply = supplyList->data;
        supW = ve_Surfaces.units[supply->type][supply->team][D_UP]->w;
        supH = ve_Surfaces.units[supply->type][supply->team][D_UP]->h;

        if(1 == collisionStatic_Rectangle2(supply->oX, supply->oY, supW, supH, colX, colY, colW, colH))
        {
            return supply;
        }

        supplyList = supplyList->next;
    }

    return NULL;
}

void vSupply_Copy(Vent_Supply *copyTo, Vent_Supply *copy)
{
    Data_Struct *aiData = vSupply_CopyAi(copy);

    vSupply_Setup(copyTo, copy->game, copy->supplyTime.srcTime, copy->respawnAmount, copy->supplyTime.end_Time, copy->startSpawned, copy->deathWait, copy->team, copy->type, copy->oX, copy->oY, copy->level, copy->aiState, aiData);

    return;
}

Vent_Supply *vSupply_Clone(Vent_Supply *copy)
{
    Data_Struct *aiData = vSupply_CopyAi(copy);

    return vSupply_Create(copy->game, copy->supplyTime.srcTime, copy->respawnAmount, copy->supplyTime.end_Time, copy->startSpawned, copy->deathWait, copy->team, copy->type, copy->oX, copy->oY, copy->level, copy->aiState, aiData);
}

void vSupply_Save(Vent_Supply *s, FILE *save)
{
    int x = 0;

    file_Log(save, 0, "%d %d %d %d %d %d %d %d %d %d", s->oX, s->oY, s->team, s->type, s->deathWait, s->respawnAmount, s->supplyTime.end_Time, s->startSpawned, s->level, s->aiState);

    if(s->aiState != SUPPLY_AI_NONE)
    {
        if(s->aiData != NULL)
        {
            /*If there is an ai state given to the supply then first save the amount of variables needed to set it up*/
            file_Log(save, 0, " %d ", s->aiData->amount);

            for(x = 0; x < s->aiData->amount; x++)
            {
                /*Save each variable as int*/
                file_Log(save, 0, "%d ", *(int *)s->aiData->dataArray[x]);
            }
        }
        else
        {
            file_Log(save, 0, " %d ", 0);
        }
    }

    file_Log(save, 0, "\n");

    return;
}

Vent_Supply *vSupply_Load(Vent_Game *game, Timer *srcTime, FILE *load)
{
    Vent_Supply *loadSupply = (Vent_Supply *)mem_Malloc(sizeof(Vent_Supply), __LINE__, __FILE__);
    int respawnTime = 0;
    int aiData = 0;
    int x = 0;

    fscanf(load, "%d %d %d %d %d %d %d %d %d %d",
        &loadSupply->oX,
        &loadSupply->oY,
        &loadSupply->team,
        &loadSupply->type,
        &loadSupply->deathWait,
        &loadSupply->respawnAmount,
        &respawnTime,
        &loadSupply->startSpawned,
        &loadSupply->level,
        &loadSupply->aiState);

    if(loadSupply->aiState != SUPPLY_AI_NONE)
    {
        /*Load in how many variables there are*/
        fscanf(load, "%d", &aiData);

        if(aiData > 0)
        {
            loadSupply->aiData = dataStruct_CreateEmpty(aiData);

            for(x = 0; x < loadSupply->aiData->amount; x = x)
            {
                fscanf(load, "%d", &aiData);

                /*x should increment with this function*/
                dataStruct_InsertDataType(loadSupply->aiData, &x, "d", aiData);
            }
        }
        else
            loadSupply->aiData = NULL;
    }
    else
        loadSupply->aiData = NULL;

    loadSupply->finished = 0;
    loadSupply->supplyTime = timer_Setup(srcTime, 0, respawnTime, 1);
    loadSupply->game = game;

    if(loadSupply->startSpawned == 0)
        loadSupply->sponserUnit = NULL;
    else if(loadSupply->game != NULL) /*Make sure this has been loaded by the game, not the editor*/
    {
        vSupply_CreateUnit(loadSupply);
    }

    return loadSupply;
}

void vSupply_Clean(Vent_Supply *s)
{
    if(s->aiData != NULL)
    {
        dataStruct_Clean(s->aiData);
        mem_Free(s->aiData);
    }

    return;
}




