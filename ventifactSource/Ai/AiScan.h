#ifndef AISCAN_H
#define AISCAN_H

#include "SDL/SDL.h"

#include "Common/List.h"
#include "../Units/Units.h"

#include "../Target.h"

Vent_Target vAi_ScanUnits(struct list *unitList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int freshSearch);

Vent_Target vAi_GetUnit_Closest(struct list *unitList,
                                 int fromX,
                                 int fromY,
                                 int maxDistanceSqr,
                                 int freshSearch);

Vent_Target vAi_GetUnit_ClosestType(struct list *unitList,
                                 int fromX,
                                 int fromY,
                                 int maxDistanceSqr,
                                 int team,
                                 int type);

int vAi_UnitIsValid(struct Vent_Unit *u, int team, int type);

Vent_Target vAi_ScanTiles(struct list *tileList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int freshSearch);

Vent_Target vAi_GetTile_Closest(struct list *tileList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int type,
                         int freshSearch);

Vent_Target vAi_ScanBuildings(struct list *buildingList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int freshSearch);

Vent_Target vAi_GetBuilding_Closest(struct list *buildingList,
                         int fromX,
                         int fromY,
                         int maxDistanceSqr,
                         int type,
                         int freshSearch);



#endif
