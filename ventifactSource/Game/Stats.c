#include "Stats.h"
#include <math.h>
#include <SDL/SDL.h>

#include "VentGame.h"
#include "../Options.h"

void vStats_Setup(Vent_Stats *s)
{
    s->bulletsFired = 0;
    s->bulletsHit = 0;

    s->unitsDestroyed = 0;
    s->unitsLost = 0;

    s->healthLost = 0;
    s->creditsSpent = 0;

    s->timeStarted = SDL_GetTicks();
    s->playDuration = 0;

    return;
}

void vStats_Update(Vent_Stats *s, void *game)
{
    int x = 0;
    Vent_Game *g = game;
    Vent_Side *playerSide = &g->side[g->playerUnit->team];

    s->unitsLost = 0;
    s->unitsDestroyed = 0;

    for(x = 0; x < UNIT_ENDTYPE; x++)
    {
        s->unitsDestroyed +=  playerSide->killedUnits[x];
        s->unitsLost +=  playerSide->lostUnits[x];
    }

    s->creditsSpent = playerSide->creditsSpent;

    s->playDuration = timer_Get_Passed(&g->gTimer, 0);

    return;
}

void vStats_Add(Vent_Stats *base, Vent_Stats *add)
{
    base->bulletsFired += add->bulletsFired;
    base->bulletsHit += add->bulletsHit;

    base->unitsDestroyed += add->unitsDestroyed;
    base->unitsLost += add->unitsLost;

    base->healthLost += add->healthLost;

    base->creditsSpent += add->creditsSpent;

    base->playDuration += add->playDuration;

    return;
}

void vStats_Save(Vent_Stats *s, FILE *openedSave)
{
    file_Log(openedSave, 0, "%d %d %d %d %d %d %d\n",
             s->bulletsFired,
             s->bulletsHit,
             s->unitsDestroyed,
             s->unitsLost,
             s->healthLost,
             s->creditsSpent,
             s->playDuration
    );

    return;
}

void vStats_Load(Vent_Stats *s, FILE *openedSave)
{
    fscanf(openedSave, "%d %d %d %d %d %d %d\n",
         &s->bulletsFired,
         &s->bulletsHit,
         &s->unitsDestroyed,
         &s->unitsLost,
         &s->healthLost,
         &s->creditsSpent,
         &s->playDuration
        );

    return;
}

void vStats_Report(Vent_Stats *s)
{
    printf("Player game stats:\n");
    printf("Bullets(F:H) (%d:%d)\n", s->bulletsFired, s->bulletsHit);
    printf("Units(D:L) (%d:%d)\n", s->unitsDestroyed, s->unitsLost);
    printf("Health lost (%d)\n", s->healthLost);
    printf("Credits spent (%d)\n", s->creditsSpent);
    printf("Time played (%d)\n", s->playDuration);

    return;
}

int vStats_CalcScore(Vent_Stats *s)
{
    float score = 0.0f;
    float numerator = 300000.0f;
    float denominator = (s->playDuration/1000) + (s->healthLost/10) + 1.0f;
	int iScore = 0;

    score = numerator/denominator;

    printf("%.2f / %.2f = score %.4f\n", numerator, denominator, score);
    score = (float)((int)score - ((int)score % 100));

	iScore = (int)score;

	if(ve_Options.difficulty == 0)
	{
		iScore /= 2;
	}

    return iScore;
}
