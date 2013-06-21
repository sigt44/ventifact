#ifndef UNITS_H
#define UNITS_H

#include <SDL/SDL.h>
#include "Graphics/Sprite.h"
#include "Vector2D.h"
#include "Common/ListWatcher.h"
#include "Vector2DInt.h"
#include "Camera.h"

#include "../Ai/Ai.h"
#include "../Ai/Ai_State.h"
#include "../Weapon.h"
#include "../Level/Sector.h"
#include "Attributes.h"
#include "../Game/Sides.h"
#include "../Target.h"
#include "../Level/Level.h"

#define UNIT_FINISHED -1
#define UNIT_ALIVE 0
#define UNIT_DELETE 1
#define UNIT_DEATH 2

#define UNIT_MAX_TARGETS 12

struct steeringForces {
    Vector2DInt seek;
    Vector2DInt flee;
    Vector2DInt wander;
    Vector2DInt avoid;
    Vector2DInt control;
};

typedef struct Vent_Unit {

    Sprite sUnitBase; /*Points to images for all four directions of the unit*/
    Sprite sUnitDamaged; /*Points to damaged images for all four directions of the unit*/
    Sprite *sUnit; /*Points to the current sprite of the unit*/

    Vector2DInt spawnPosition;

    char name[32]; /*Name of unit*/

    int ID; /*ID of unit*/

    int health; /*Actual health of unit*/
    int healthStarting; /*Starting health of unit*/
    int healthPrev; /*Health of the unit the previous frame*/

    int width;
    int height;

    Vector2D heading;
    Vector2D velocity;
    Vector2D position;
    Vector2D prevPosition;
    Vector2DInt middlePosition;
    Vector2DInt iPosition; /*To prevent multiple typecasting, the int version of position*/

    Vector2D collisionForce;
    Vector2D seperationForce;
    float seperationWeight;

    Vector2D targetForce;
    float targetWeight;


    float friction; /*simple friction coefficient*/

    int graphicTurn;
    int pGraphicTurn;
    float changeSpeed; /*The speed the unit must be at to change direction*/

    unsigned int direction; /* Direction of unit*/

    float maxSpeed; /* The max speed of the unit*/
    float maxForce; /* The max force of the unit*/

    float steeringForce; /* The steering force of the unit*/

    /*Used to fix the friction*/
    float forceMaxSpeedX;
    float forceMaxSpeedY;

    Vector2D steerFixX;
    Vector2D steerFixY;

    Vector2D prevForce;

    unsigned int destroyed;

    Timer *srcTimer;

    Ai_Module ai;

    Vent_Target targetEnemy[UNIT_MAX_TARGETS];
    Vent_Target targetAlly[UNIT_MAX_TARGETS];

    Vent_Weapon weapon;

    void *attributes;

    Vent_Sector *inSector;

    struct list *onTiles;

    unsigned int trackStat;

    unsigned int type;
    int team;

    unsigned int invulnerability;

    unsigned int collide;

    int viewDistance;
    unsigned int isPlayer;

    unsigned int oldDirection;
    unsigned int layer;

    Dependency_Watcher depWatcher;

} Vent_Unit;

char *vUnitNames[UNIT_ENDTYPE];

int vUnit_MiddleX(Vent_Unit *u);

int vUnit_MiddleY(Vent_Unit *u);

int vUnit_Move(Vent_Unit *u, void *vg, float dt);

int vUnit_HasMoved(Vent_Unit *u);

void vUnit_Ai(Vent_Unit *u);

void vUnit_GiveTankProp(Vent_Unit *u);

void vUnit_GiveInfantryProp(Vent_Unit *u);

void vUnit_GiveTowerProp(Vent_Unit *u);

void vUnit_GiveAirProp(Vent_Unit *u);

Vent_Unit *vUnit_Create(unsigned int x, unsigned int y,
                        unsigned int team, unsigned int type,
                        unsigned int isPlayer, unsigned int trackStat,
                        Vent_Attributes *extraAttributes, Timer *srcTime);

void vUnit_Setup(Vent_Unit *u,
                unsigned int x, unsigned int y,
                unsigned int team, unsigned int type,
                unsigned int isPlayer, unsigned int trackStat,
                Vent_Attributes *extraAttributes, Timer *srcTime);

void vUnit_GiveProperties(Vent_Unit *u, int type);

int vUnit_IsDestroyed(Vent_Unit *u);

Vent_Unit *vUnit_Buy(int x, int y,
                    unsigned int type, unsigned int team,
                    unsigned int isPlayer,
                    Vent_Side *side, Timer *srcTime);

Ai_State *vUnit_GiveAi(Vent_Unit *u, Ai_State *ai);

void vUnit_GiveDefaultAi(Vent_Unit *u, void *game);

int vUnit_NeedsDelete(Vent_Unit *u, void *vg);

void vUnit_DestroyList(struct list **units, void *vg);

int vUnit_IsDamaged(Vent_Unit *u, int update);

void vUnit_DamagedCheck(Vent_Unit *u);

void vUnit_Draw(Vent_Unit *u, Camera_2D *c, SDL_Surface *destination);

void vUnit_Collision(Vent_Unit *u, Vent_Level *l);

void vUnit_TileCollision(Vent_Unit *u, void *t);

int vUnit_TileScan(Vent_Unit *u);

void vUnit_SetSector(Vent_Unit *u, struct list *sectors);

void vUnit_WatcherAdd(Vent_Unit *u, struct list **head);

void vUnit_WatcherRemove(Vent_Unit *u, struct list **head);

void vUnit_Clean(Vent_Unit *u);

#endif
