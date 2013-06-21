#ifndef TARGET_H
#define TARGET_H

#include "Defines.h"

typedef struct Vent_Target {
    void *entity;
    int type;
    int distanceSqr;
    int distance;
} Vent_Target;

extern const char *entityNames[ENTITY_END];

void vTarget_Setup(Vent_Target *target, void *entity, int type);
void vTarget_SetupRange(Vent_Target *target, void *entity, int type, int dis, int disSqr);

void vTarget_Report(Vent_Target *target);

void vTarget_SetupDefaultsArray(Vent_Target *targetArray, int numArray);
void vTarget_ClearArray(Vent_Target *targetArray, int numArray);

int vTarget_SameTeam(Vent_Target *target, int team);
int vTarget_SetLink(Vent_Target *target, int link);

#endif
