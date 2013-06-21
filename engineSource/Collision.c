#include <math.h>
#include "Collision.h"

#include "Maths.h"

unsigned int collision_Circle(Vector2D *pA, Vector2D *vA, float radiusA, Vector2D *pB, Vector2D *vB, float radiusB, int fixVelocity)
{
    Vector2D vMovement = vector_Subtract(vB,vA);
    Vector2D vDistance = vector_Subtract(pA,pB);
    Vector2D vNormedMovement;

    float distanceSqr = (vDistance.x * vDistance.x) + (vDistance.y * vDistance.y);
    float speedSqr = (vMovement.x * vMovement.x) + (vMovement.y * vMovement.y);
    float totalSpeed;
    float sumRadius = (radiusA + radiusB);
    float sumRadiusSqr;
    float dotMovement;
    float distancePerpSqr;
    float tSqr;
    float distanceTravel;
    float adjustTime;

    /*First check if the distance between each circle is larger than the total frame speed of the circles*/

    if((sqrt(distanceSqr) - sumRadius) > sqrt(speedSqr))
    {
        return 0; /*No collision*/
    }

    vNormedMovement = vector_Normalise(&vMovement);

    dotMovement = vector_Dot(&vNormedMovement,&vDistance);

    /*Is the circle is moving towards the other circle?*/

    if(dotMovement <= 0)
    {
        return 0; /*No collision*/
    }

    /*Find the distance distanceCos, from the centre of circle b to the perpendicular to the closest point of the vector vMovement */
    distancePerpSqr = distanceSqr - (dotMovement * dotMovement);

    /*Is distancePerp larger than the sum of the radius?*/
    sumRadiusSqr = sumRadius * sumRadius;
    if(distancePerpSqr >= sumRadiusSqr)
    {
        return 0; /*No collision*/
    }

    tSqr = sumRadiusSqr - distancePerpSqr;
    if(tSqr < 0)
    {
        return 0;
    }

    distanceTravel = dotMovement - (float)sqrt(tSqr);

    /*Will circle A move enough so that it will touch the circle*/
    totalSpeed = (float)sqrt(speedSqr);
    if(totalSpeed < distanceTravel)
    {
        return 0; /*No collision*/
    }
    /*printf("%f < %f\n", speedSqr, distanceTravel * distanceTravel); why would this check not work properly?*/

    /*The circles have collided!!*/
    if(fixVelocity)
    {
        if(vector_Is_Zero(vB))
        {
            vNormedMovement = vector_Normalise(vA);
            *vA = vector_MultiplyBy(&vNormedMovement,distanceTravel);
        }
        else if(vector_Is_Zero(vA))
        {
            vNormedMovement = vector_Normalise(vB);
            *vB = vector_MultiplyBy(&vNormedMovement,distanceTravel);
        }
        else
        {
            adjustTime = distanceTravel / totalSpeed;
            *vA = vector_MultiplyBy(vA, adjustTime);
            *vB = vector_MultiplyBy(vB, adjustTime);
        }
    }

    return 1;
}

unsigned int collision_CircleLine(Vector2D *lineStart, Vector2D *lineEnd, Vector2D *circlePos, Vector2D *velocity, float radius, Vector2D *cPoint)
{
    Vector2D A = *velocity;
    Vector2D B = vector_Subtract(lineEnd,lineStart); /*Line vector from the origin*/

    Vector2D localPos; /*Local position of circle to line vector*/
    Vector2D localPosRadius; /*Local position of circle to line vector a radius towards the circle*/
    Vector2D circleToCollision;

    Vector2D collisionPoint;
    Vector2D collisionPointLine;

    Vector2D perpLine;
    Vector2D nPerpLine;
    Vector2D newLine;

    Vector2D temp;

    Vector2D fakeStart;
    Vector2D fakeEnd;
    Vector2D normB;
    Vector2D zeroVel = {0,0};

    float aGradient = 0; /*Gradient of velocity*/
    float bGradient = 0; /*Gradient of line*/
    float xCollision = 0; /*Calculate the position of collision from the x axis, y = mx + c*/
    float posSide,pvSide; /* posSide - which side the circle is perpendicular to the line, pvSide - which side the circle + velocity is perpendicular to the line*/

    int collision = 0;

    if(B.x < 0.0f)
    {
        fakeStart = *lineEnd;
        fakeEnd = *lineStart;
        B = vector_Reverse(&B);
    }
    else
    {
        fakeStart = *lineStart;
        fakeEnd = *lineEnd;
    }

    normB = vector_Normalise(&B);
    nPerpLine = vector_Perp(&normB); /*Get the perpendicular line*/

    localPos = vector_Subtract(circlePos,&fakeStart); /*Get position to circle relative to line B*/

    posSide = vector_Dot(&nPerpLine,&localPos);

    if(posSide < 0.0f) /*Now add or subtract depending on which side the circle is along the line*/
    {

        perpLine = vector_MultiplyBy(&nPerpLine,radius - 0.0001f); /*Make it the length of the ball radius + small tweak to radius to cover errors in float(I think ;)*/
        newLine = vector_Subtract(&fakeStart,&perpLine); /* This creates a new line that is a radius towards the circle*/

        localPosRadius = vector_Subtract(circlePos,&newLine);
        temp = vector_Add(&localPosRadius,velocity);
        pvSide = vector_Dot(&nPerpLine,&temp);

        if(pvSide < 0.0f)
            return 0;
        if(posSide > (-radius + 0.0001f))
        {
            A = perpLine;
        }
    }
    else if(posSide > 0.0f)
    {
        perpLine = vector_MultiplyBy(&nPerpLine,-radius + 0.0001f); /*Make it the length of the ball radius + small tweak to radius to cover errors in float(I think ;)*/
        newLine = vector_Subtract(&fakeStart,&perpLine);

        localPosRadius = vector_Subtract(circlePos,&newLine);
        temp = vector_Add(&localPosRadius,velocity);
        pvSide = vector_Dot(&nPerpLine,&temp);

        if(pvSide > 0.0f)
            return 0;
        if(posSide < (radius - 0.0001f))
        {
            A = perpLine;
        }
    }
    else
    {
        /*Circle is exactly on the line, lets give up*/
        return 0;
    }

    if(B.y == 0.0f)
        bGradient = 0.0f;
    else if(B.x == 0.0f)
        bGradient = 999999.0f; /*Not fully tested*/
    else
        bGradient = -B.y/B.x;


    if(A.y == 0.0f)
        aGradient = 0.0f;
    else if(A.x == 0.0f)
        aGradient = 999999.0f; /*Not fully tested*/
    else
        aGradient = -A.y/A.x;

    if(bGradient != aGradient)
    {
        if(A.x == 0.0f)
            xCollision = localPosRadius.x;
        else
        {
            xCollision = (aGradient * -localPosRadius.x) - localPosRadius.y;
            xCollision = (xCollision) / ( bGradient - aGradient ); /*Find where velocity A collides on the infinite line B*/
        }
        collisionPoint.x = xCollision;
        collisionPoint.y = -xCollision*bGradient;

        collisionPointLine = vector_Add(&collisionPoint,&newLine); /*Get the actual point of collision*/

        temp = collisionPoint;
        if(vector_Dot(&temp,&normB) < 0.0f)
        {
            /*Give the end points a very small radius so that they keep the line smooth*/
            if(collision_Circle(&fakeStart, &zeroVel, 0.0001f, circlePos, velocity, radius, 1) == 1)
            {
                if(cPoint != NULL)
                    *cPoint = vector_Subtract(circlePos,&fakeStart);
                return 1;
            }
            return 0;
        }

        temp = vector_Subtract(&temp,&B);
        if(vector_Dot(&temp,&normB) > 0.0f)
        {
            if(collision_Circle(&fakeEnd, &zeroVel, 0.0001f, circlePos, velocity, radius, 1) == 1)
            {
                if(cPoint != NULL)
                    *cPoint = vector_Subtract(circlePos,&fakeEnd);
                return 1;
            }
            return 0;
        }

        collision = 1;

    }

    if(collision)
    {
        circleToCollision = vector_Subtract(&collisionPointLine,circlePos);

        if(cPoint != NULL)
            *cPoint = perpLine; /*This vector is assigned so that a responce function can use it to reflect the circle*/

        *velocity = circleToCollision;

        return 1;

    }

    return 0;
}

int collisionStatic_Circle(int aX, int aY, int aRadius, int bX, int bY, int bRadius)
{
	int xDis = (aX - bX);
	int yDis = (aY - bY);
	int tDis = mth_DistanceSqr(xDis,yDis);

    int sum_RadiusSqr = (aRadius + bRadius) * (aRadius + bRadius);

	if(tDis < sum_RadiusSqr)
	{
        return tDis;
	}

	return 0;
}

int collisionStatic_SemiCircle(int aX,int aY,int aRadius,int bX,int bY,int bRadius)
{
	int xDis = (int)(aX - bX);
	int yDis = (int)(aY - bY);
	int tDis = mth_DistanceSqr(xDis,yDis);

    int sum_RadiusSqr = (aRadius + bRadius) * (aRadius + bRadius);

	if(tDis < sum_RadiusSqr)
	{
	    if(aX < bX)
            return tDis;
	}

	return 0;
}

int collisionStatic_CircRect(Vector2D *posCircle, int *radius, Vector2D *posRect, SDL_Rect *box)
{
	int left_Circ_V = (int)posCircle->x;
	int right_Circ_V = (int)posCircle->x;
	int down_Circ_V = (int)posCircle->y + *radius;
	int up_Circ_V = (int)posCircle->y - *radius;

	int left_Circ_H = (int)posCircle->x - *radius;
	int right_Circ_H = (int)posCircle->x + *radius;
	int down_Circ_H = (int)posCircle->y;
	int up_Circ_H = (int)posCircle->y;

	int left_Box = (int)posRect->x;
	int right_Box = (int)posRect->x + box->w;
	int top_Box = (int)posRect->y;
	int down_Box = (int)posRect->y + box->h;


	if( ( ( down_Circ_V <= top_Box ) || ( up_Circ_V >= down_Box ) || ( right_Circ_V <=  left_Box) || ( left_Circ_V >=  right_Box ) ) == 0 )
	{
		return 1;
	}
	if( ( ( down_Circ_H <= top_Box ) || (up_Circ_H >= down_Box ) || ( right_Circ_H <=  left_Box) || ( left_Circ_H >=  right_Box) ) == 0 )
	{
		return 1;
	}

	return 0;
}

int collisionStatic_CircRectInt(int circX, int circY, int *radius, int rectX, int rectY, int rectW, int rectH)
{
    int left_Circ_V = circX;
	int right_Circ_V = circX;
	int down_Circ_V = circY + *radius;
	int up_Circ_V = circY - *radius;

	int left_Circ_H = circX - *radius;
	int right_Circ_H = circX + *radius;
	int down_Circ_H = circY;
	int up_Circ_H = circY;

	int left_Box = rectX;
	int right_Box = rectX + rectW;
	int top_Box = rectY;
	int down_Box = rectY + rectH;

	if( ( ( down_Circ_V <= top_Box ) || ( up_Circ_V >= down_Box ) || ( right_Circ_V <=  left_Box) || ( left_Circ_V >=  right_Box ) ) == 0 )
	{
		return 1;
	}
	if( ( ( down_Circ_H <= top_Box ) || (up_Circ_H >= down_Box ) || ( right_Circ_H <=  left_Box) || ( left_Circ_H >=  right_Box) ) == 0 )
	{
		return 1;
	}

	return 0;
}


int collisionStatic_Rectangle2(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    int rightA = x1 + w1;
    int rightB = x2 + w2;

    int bottomA = y1 + h1;
    int bottomB = y2 + h2;

    /*If any of the sides from A are outside of B*/

    if( bottomA <= y2 )
    {
        return 0;
    }

    if( y1 >= bottomB )
    {
        return 0;
    }

    if( rightA <= x2 )
    {
        return 0;
    }

    if( x1 >= rightB )
    {
        return 0;
    }

    return 1;
}


int collisionStatic_Rectangle(SDL_Rect *A, SDL_Rect *B)
{
    /*The sides of the rectangles*/
    int rightA, rightB;

    int bottomA, bottomB;

    /*Calculate the sides of rect A*/
    rightA = A->x + A->w;

    bottomA = A->y + A->h;

    /*Calculate the sides of rect B*/

    rightB = B->x + B->w;

    bottomB = B->y + B->h;

    /*If any of the sides from A are outside of B*/

    if( bottomA <= B->y )
    {
        return 0;
    }

    if( A->y >= bottomB )
    {
        return 0;
    }

    if( rightA <= B->x )
    {
        return 0;
    }

    if( A->x >= rightB )
    {
        return 0;
    }

    return 1;
}

/*
    Function: collisionFit_Rect

    Description -
    Returns a clipping structure that would
    fit the rectangle B inside of A.

    This function assumes that those two rectangles collide with each other.

    Each SDL_Rect A and B should represent rectangles with
    x, y, width and height components.

    2 arguments:
    SDL_Rect *A - The rectangle that will contain B.
    SDL_Rect *B - This rectangle will be fitted into A.
*/
SDL_Rect collisionFit_Rect(SDL_Rect *A, SDL_Rect *B)
{
    SDL_Rect C = {0, 0, 0, 0};
    int topRightCornerA = A->x + A->w;
    int topRightCornerB = B->x + B->w;
    int bottomLeftCornerA = A->y + A->h;
    int bottomLeftCornerB = B->y + B->h;

    /*First solve the x-axis clipping*/

    /*If B is to the right of A*/
    if(B->x > A->x)
    {
        /*Since they are colliding the x clipping will be 0*/
        C.x = 0;

        /*The width of the clipping will be the remaining width of the rectangle B*/
        C.w = B->w;

        /*If this width + position of B ends up reaching outside of A then limit the width*/
        if(topRightCornerB > topRightCornerA)
        {
           C.w = C.w - (topRightCornerB) + (topRightCornerA);
        }
    }
    else /*B is to the left or equal to A*/
    {
        /*The x clipping will be the distance A is to the right of B*/
        C.x = A->x - B->x;

        /*The width will be the remaining width of the rectangle B*/
        C.w = B->w - C.x;

        /*If this width of B + position of A is greater than A + position of A's width.
        Need to reduce the width of B further since it reaches outside of A*/
        if((A->x + C.w) > topRightCornerA)
        {
            /*Since the clipped B starts at the beggining of A then its width will
            be A's width*/
            C.w = A->w;
        }
    }

    /*Solve the y-axis clipping using the same method as x-axis*/

    /*If B is below A*/
    if(B->y > A->y)
    {
        /*Since they are colliding the y clipping will be 0*/
        C.y = 0;

        /*The height of the clipping will be the remaining height of the rectangle B*/
        C.h = B->h;

        /*If this height + position of B ends up reaching outside of A then limit the height*/
        if(bottomLeftCornerB > bottomLeftCornerA)
        {
            C.h = C.h - (bottomLeftCornerB) + (bottomLeftCornerA);
        }
    }
    else /*B is above or equal to A*/
    {
        /*The y clipping will be the distance A is above of B*/
        C.y = A->y - B->y;

        /*The height will be the remaining height of the rectangle B*/
        C.h = B->h - C.y;

        /*If this height of B + position of A is greater than A + position of A's height.
        Need to reduce the width of B further since it reaches outside of A*/
        if((A->y + C.h) > bottomLeftCornerA)
        {
            /*Since the clipped B starts at the beggining of A then its height will
            be A's height*/
            C.h = A->h;
        }
    }

    return C;
}

/*
    Function: collision_Point_PerpLine

    Description -
    This function returns the intercept point between a line and the line that
    is perpendicular to it. This perpendicular line must also pass through a certain
    point.

    3 arguments:
    Vector2D *startA - Starting point of the line.
    Vector2D *endA - Ending point of the line.
    Vector2D *point - The point which the perpendicular line must pass through.
*/
Vector2D collision_Point_PerpLine(Vector2D *startA, Vector2D *endA, Vector2D *point)
{
    float gradientA = 0.0f;
    float gradientPA = 0.0f;
    float xDis = endA->x - startA->x;
    float yDis = endA->y - startA->y;

    float A_yIntercept = 0.0f;
    float PA_yIntercept = 0.0f;

    Vector2D V = {0.0f, 0.0f};

    /*Check if the line is vertical*/
    if(xDis == 0.000f)
    {
        /*the line x = c*/
        V.y = point->y;
        V.x = startA->x;
    }
    else if(yDis == 0.000f) /*Check if the line is horizontal*/
    {
        /*the line y = c*/
        V.x = point->x;
        V.y = startA->y;
    }
    else
    {
        /*find the gradient of the line*/
        gradientA = (endA->y - startA->y) / (endA->x - startA->x);
        gradientPA = -1/gradientA; /*then the gradient of the line perpendicular to it*/

        /*find y intercepts of each line*/
        A_yIntercept = startA->y - (startA->x * gradientA);
        PA_yIntercept = point->y - (point->x * gradientPA);

        /*Then solve to find the point of intercept*/
        V.x = (PA_yIntercept - A_yIntercept)/(gradientA - gradientPA);
        V.y = (V.x * gradientA) + A_yIntercept;
    }

    return V;
}

int collision_Point_IsInRange(float startX, float endX, float pointX)
{
    float hold = 0.0f;

    /*Make sure that startX is the smaller limit of the range*/
    if(startX > endX)
    {
        hold = startX;
        startX = endX;
        endX = hold;
    }

    if(pointX >= startX && pointX <= endX)
    {
        return 1;
    }

    return 0;
}



