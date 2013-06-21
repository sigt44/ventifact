#include <math.h>
#include "Ai_Steering.h"

#include "Maths.h"

/*
    Function: vSteering_Setup

    Description -
    Sets up a steering structure for safe use of the steering functions.

    6 arguments:
    Vent_Steering *steer - The steering structure to setup.
    Vector2D *position - The position of the entity that will be steered.
    Vector2D *velocity - The velocity of the entity that will be steered.
    Vector2D *force - The force of the entity that will be steered.
    Vector2D *targetPosition - The position of a target entity.
    Vector2D *targetVelocity - The velocity of a target entity.
*/
void vSteering_Setup(Vent_Steering *steer,
                    Vector2D *position,
                    Vector2D *velocity,
                    Vector2D *force,
                    Vector2D *targetPosition,
                    Vector2D *targetVelocity
                    )
{
    steer->position = position;
    steer->velocity = velocity;
    steer->force = force;

    steer->targetPosition = targetPosition;
    steer->targetVelocity = targetVelocity;

    vector_Clear(&steer->offset);
    vector_Clear(&steer->movePosition);

    steer->xDis = 0.0f;
    steer->yDis = 0.0f;
    steer->distance = 0.0f;

    return;
}

/*
    Function: vAi_SteerAccumulate

    Description -
    Adds a force to a net force. Makes sure the net force does
    not exceed a specified amount.

    3 argument:
    Vector2D *netForce - The net force that will accumulate the force.
    Vector2D *force - The force to add to the net force.
    float maxForce - The maximum that each component of the net force can be.
*/
void vAi_SteerAccumulate(Vector2D *netForce, Vector2D *force, float maxForce)
{
    float nX = (float)fabs(netForce->x);
    float nY = (float)fabs(netForce->y);

    float remain = 0.0f;

    /*If the net force is not full*/
    if(nX < maxForce)
    {
        remain = maxForce - nX;

        /*If the force to add will not overfill the net force*/
        if(remain >= fabs(force->x))
            netForce->x += force->x;
        else
        {
            /*Add in as much force as possible*/
            if(force->x > 0)
                netForce->x += remain;
            else
                netForce->x -= remain;
        }

    }

    if(nY < maxForce)
    {
        remain = maxForce - nY;
        if(remain >= fabs(force->y))
            netForce->y += force->y;
        else
        {
            if(force->y > 0)
                netForce->y += remain;
            else
                netForce->y -= remain;
        }

    }

    return;
}

/*
    Function: vAi_SteerArrive

    Description -
    Returns a force value that will be directed towards a point on a single plane.
    If xDis is smaller or equal to brakeRange the force will decrease in proportion to the (distance remaining/brakerange).

    4 arguments:
    float xVel - Current velocity of the entity
    float xDis - Distance to the point the entity wants to move to
    float moveForce - The base movement force to give the entity
    float brakeRange - At what range the unit will begin to slow down towards its point to move to.
*/
float vAi_SteerArrive(float xVel, float xDis, float moveForce, float brakeRange)
{
    float xMod = 1.0f; /*Used to lower the force if the distance (xDis) is lower or equal to the
    brake range*/

    float fXDis = (float)fabs(xDis);
    float fXVel = (float)fabs(xVel);

    /*Make sure not to divide by zero*/
    if(fXDis < 0.0001f)
        return 0.0f;

    if(fXDis < brakeRange) /*If entity is in brake range*/
    {
        xMod = (((fXVel/fXDis) + fXDis)/xDis);

        /*If the velocity is larger than the distance required to move to*/
        if(mth_SameSignF(xVel, xDis) == 1 && fXVel > fXDis)
        {
            /*Reverse the direction of force*/
            xMod = -xMod/2.0f;
        }

    }
    else
    {
        return vAi_SteerMove(xDis, moveForce, 0, 0); /*entity is not at brake range so move it closer*/
    }

    return xMod * moveForce;
}

/*
    Function: vAi_SteerMove

    Description -
    Returns a force that is towards a point on a single plane.

    3 arguments:
    float xDis - Distance to the point the entity wants to move to
    float moveForce - The magnitude of force to give the entity
    float relaxDistance - The distance that there will be a reduced force
    float relaxBy - The proportionate amount of force that there will be if within relaxDistance
*/
float vAi_SteerMove(float xDis, float moveForce, float relaxDistance, float relaxBy)
{
    if(xDis > 0)
    {
        if(xDis > relaxDistance)
            return moveForce;
        else
            return moveForce * relaxBy;
    }
    else
    {
        if(xDis < -relaxDistance)
            return -moveForce;
        else
            return -moveForce * relaxBy;
    }
}

/*
    Function: vAi_SteerTerror

    Description -
    Returns a force value that will be directed away from a point on a single plane.
    If xDis is smaller or equal to terrorRange the force will increase in proportion to the (terrorRange/distance).

    3 arguments:
    float xDis - Distance to the point the entity wants to move away
    float moveForce - The base movement force to give the entity
    float terrorRange - At what range the unit will begin to increase in force from its point to move to.
*/
float vAi_SteerTerror(float xDis, float moveForce, float terrorRange)
{
    float xMod = 0.0f; /*Used to increase the force if the distance (xDis) is lower or equal to the
    terror range*/

    float fXDis = (float)fabs(xDis);

    if(fXDis < terrorRange) /*If entity is in terror range*/
    {
        xMod = 1.0f - (fXDis/terrorRange); /*At what proportion is the terrorRange to the current distance*/

        if(xMod < 0.0001f)
            return 0.0f;

        if(xDis > 0)
        {
            xMod = -xMod;
        }
    }
    else
    {
        return 0.0f;
    }

    return xMod * moveForce;
}

/*
    Function: vAi_SteerRepel

    Description -
    Returns a force that is away from a point on a single plane.

    3 arguments:
    float xDis - Distance to the point the entity wants to away from
    float moveForce - The magnitude of force to give the entity
    float extraDistance - The distance that there will be an extra force
    float extraBy - The proportionate amount of force that there will be if within extraDistance
*/
float vAi_SteerRepel(float xDis, float moveForce, float extraDistance, float extraBy)
{
    if(xDis > 0)
    {
        if(xDis > extraDistance)
            return -moveForce;
        else
            return -moveForce * extraBy;
    }
    else
    {
        if(xDis < -extraDistance)
            return moveForce;
        else
            return moveForce * extraBy;
    }
}

/*
    Function: vAi_SteerPerpVelocity

    Description -
    Sets the vector direction *moveLocation to be perpendicular to the *targetVelocity vector.
    Then sets the vector magnitude to be that of 'distance'.

    4 arguments:
    Vector2D *moveLocation - The vector that will have its values changed.
    Vector2D *targetVelocity - The vector that moveLocation will be perpendicular to.
    float distance - The magnitude to set the moveLocation vector.
    int reverse - If 1 then the moveLocation vector will have its values reversed.
*/
void vAi_SteerPerpVelocity(Vector2D *moveLocation,
                           Vector2D *targetVelocity,
                           float distance,
                           int reverse)
{
        /*Set the move location to be perpendicular to the targets heading by the amount in the variable of distance*/
        *moveLocation = vector_Perp(targetVelocity);

        *moveLocation = vector_Normalise(moveLocation);
        *moveLocation = vector_MultiplyBy(moveLocation, distance);

        if(reverse == 1)
        {
            /*Reverse the perpendicular heading*/
            *moveLocation = vector_Reverse(moveLocation);
        }

        return;
}

/*
    Function: vAi_SteerFlank

    Description -
    Sets the force from the steering structure in the direction of flanking a target.
    Uses the following variables from the steering structure -
    *targetPosition, *targetVelocity, *position, *force, offset, movePosition.

    5 arguments:
    Vent_Steering *steer - The steering structure that has been setup.
    float maxForce - The maximum force the entity can perform.
    float offsetDistance - The distance to flank.
    float errorRange - The maximum range that is allowed to be away from the exact move target.
    Timer *updateTimer - A timer (With an end time set) to decide when to update the flank offset.
*/
void vAi_SteerFlank(Vent_Steering *steer,
                         float maxForce,
                         float offsetDistance,
                         float errorRange,
                         Timer *updateTimer)
{
    float xDis = 0;
    float yDis = 0;
    Vector2D toTarget;

    timer_Calc(updateTimer);

    if(timer_Get_Remain(updateTimer) <= 0) /*Make sure not to update too frequently*/
    {
        timer_Start(updateTimer);

        /*Get the vector from the units position to its targets position*/
        toTarget = vector_Subtract(steer->targetPosition, steer->position);

        /*Get the offset from the targets position for the unit to move to. It is based on the targets velocity*/
        vAi_SteerPerpVelocity(&steer->offset, steer->targetVelocity, offsetDistance, 0);

        /*Make sure that the unit stays on the same side of the target*/
        if(mth_SameSignF(steer->offset.y, toTarget.y))
            steer->offset.y = -steer->offset.y;

        if(mth_SameSignF(steer->offset.x, toTarget.x))
            steer->offset.x = -steer->offset.x;
    }

    /*Add the heading to the base position*/
    steer->movePosition = vector_Add(&steer->offset, steer->targetPosition);

    xDis = steer->movePosition.x - steer->position->x;
    yDis = steer->movePosition.y - steer->position->y;

    if(fabs(xDis) > errorRange)
        steer->force->x += vAi_SteerMove(xDis, maxForce, 20.0f, 0.2f);
    if(fabs(yDis) > errorRange)
        steer->force->y += vAi_SteerMove(yDis, maxForce, 20.0f, 0.2f);

    return;
}

/*
    Function: vAi_SteerSwoop

    Description -
    Sets the force from the steering structure in the direction of swooping a target.
    Uses the following variables from the steering structure -
    *targetPosition, *position, *velocity, *force, offset, movePosition.

    4 arguments:
    Vent_Steering *steer - The steering structure that has been setup.
    float maxForce - The maximum force the entity can perform.
    float offsetDistance - The distance to swoop.
    Timer *updateTimer - A timer (With an end time set) to decide when to update the swoop offset.
*/
void vAi_SteerSwoop(Vent_Steering *steer,
                         float maxForce,
                         float offsetDistance,
                         Timer *updateTimer)
{
    float xDis = 0;
    float yDis = 0;

    /*Choose a position ahead of where the target is and swoop towards it*/

    timer_Calc(updateTimer);

    if(timer_Get_Remain(updateTimer) <= 0) /*Make sure not to update too frequently*/
    {
        timer_Start(updateTimer);

        /*Get the offset from the targets position for the unit to move to*/

        steer->offset = vector_Subtract(steer->targetPosition, steer->position); /*Based upon the current position difference of the unit to the target*/
        steer->offset = vector_Normalise(&steer->offset);
        steer->offset = vector_MultiplyBy(&steer->offset, offsetDistance);
    }

    /*Add the heading to the base position*/
    steer->movePosition = vector_Add(&steer->offset, steer->targetPosition);

    xDis = steer->movePosition.x - steer->position->x;
    yDis = steer->movePosition.y - steer->position->y;

    /*only travel if the axis distance is larger than 40*/
    if(fabs(xDis) > 40)
        steer->force->x += vAi_SteerArrive(steer->velocity->x, xDis, maxForce, 150);
    if(fabs(yDis) > 40)
        steer->force->y += vAi_SteerArrive(steer->velocity->y, yDis, maxForce, 150);

    return;
}
