#ifndef AI_STEERING_H
#define AI_STEERING_H

#include "Vector2D.h"
#include "Time/Timer.h"

/*Common steering variables structure*/
typedef struct
{
    Vector2D *position; /*Should point to the position of the entity that is to be steered*/
    Vector2D *velocity;
    Vector2D *force;

    Vector2D *targetPosition;
    Vector2D *targetVelocity;

    Vector2D offset; /*Hold an offset position for the entity to move to*/
    Vector2D movePosition; /*Hold the previous or current position that the entity is moving to*/

    float xDis;
    float yDis;
    float distance;
} Vent_Steering;


void vSteering_Setup(Vent_Steering *steer,
                    Vector2D *position,
                    Vector2D *velocity,
                    Vector2D *force,
                    Vector2D *targetPosition,
                    Vector2D *targetVelocity
                    );

void vAi_SteerAccumulate(Vector2D *netForce, Vector2D *force, float maxForce);

float vAi_SteerArrive(float xVel, float xDis, float moveForce, float brakeRange);
float vAi_SteerMove(float xDis, float moveForce, float relaxDistance, float relaxBy);

float vAi_SteerTerror(float xDis, float moveForce, float terrorRange);
float vAi_SteerRepel(float xDis, float moveForce, float extraDistance, float extraBy);


void vAi_SteerPerpVelocity(Vector2D *moveLocation,
                           Vector2D *targetVelocity,
                           float distance,
                           int reverse);

void vAi_SteerFlank(Vent_Steering *steer,
                         float maxForce,
                         float offsetDistance,
                         float errorRange,
                         Timer *updateTimer);

void vAi_SteerSwoop(Vent_Steering *steer,
                         float maxForce,
                         float offsetDistance,
                         Timer *updateTimer);
#endif
