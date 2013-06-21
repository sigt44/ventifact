#ifndef STATS_H
#define STATS_H

#include <stdio.h>

typedef struct Vent_Stats
{
    int bulletsFired;
    int bulletsHit;

    int unitsDestroyed;
    int unitsLost;

    int healthLost;
    int creditsSpent;

    int timeStarted;
    int playDuration; /*Excluding pause*/

} Vent_Stats;

void vStats_Setup(Vent_Stats *s);

void vStats_Add(Vent_Stats *base, Vent_Stats *add);

void vStats_Save(Vent_Stats *s, FILE *openedSave);
void vStats_Load(Vent_Stats *s, FILE *openedSave);

void vStats_Update(Vent_Stats *s, void *game);

int vStats_CalcScore(Vent_Stats *s);

void vStats_Report(Vent_Stats *s);

#endif
