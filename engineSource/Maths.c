#include <math.h>
#include "Maths.h"

float f_Angles[MTH_FILLANGLE][2];

void mth_FillAngles(void)
{
    float angle = 0.0f;
    float scale = (PI2/MTH_FILLANGLE);

    int x = 0;

    for(x = 0; x < MTH_FILLANGLE; x++)
    {
        angle = (x*scale);

        f_Angles[x][0] = sinf(angle);
        f_Angles[x][1] = cosf(angle);
    }

    return;
}

/*
	Function: mth_FitAngleBoundry

	Description -
    Returns the equivalent parameter angle in degrees in the range [0, 360).

	1 argument:
	int angle - The angle to fit inside of [0, 360)
*/
int mth_FitAngleBoundry_Deg(int angle)
{
    if(angle >= 0)
        angle = angle % 360;
    else
        angle = 360 - (-angle % 360);

    return angle;
}

float mth_DistanceSqrF(float xDis, float yDis)
{
    return (xDis * xDis) + (yDis * yDis);
}

int mth_DistanceSqr(int xDis, int yDis)
{
	return (xDis * xDis) + (yDis * yDis);
}

int mth_Distance(int xDis,int yDis)
{
    /*if(distanceSqr < MAXSQRTLOOKUP)
       return sqrt_LU(distanceSqr);
    else*/
        return (int)sqrt((xDis * xDis) + (yDis * yDis));
}

/*
	Function: mth_SameSignF

	Description -
	Function returns 1 if the two floats (a and b) entered are the same sign, otherwise returns 0.

	2 arguments:
	float a - The first float
	float b - The second float
*/
int mth_SameSignF(float a, float b)
{
	if((a > 0 && b > 0) || (a < 0 && b < 0) )
		return 1;
	else
		return 0;
}

/*
    Function: mth_GetQuadrant

    Description -
    Returns the quarter of a circle that a point is located at. Using the xDis and yDis distances from
    the origin.

    2 arguments:
    float xDis - The x-axis distance of the point from the origin.
    float yDis - The y-axis distance of the point from the origin.
*/
int mth_GetQuadrant(float xDis, float yDis)
{
    if(xDis >= 0.0000f && yDis >= 0.000f)
    {
        return 0; /*First quarter*/
    }
    else if(xDis < 0 && yDis >= 0.0000f)
    {
        return 1; /*Second quarter*/
    }
    else if(xDis < 0 && yDis <= -0.0000f)
    {
        return 2; /*Third quarter*/
    }
    else
    {
        return 3; /*Fourth quarter*/
    }
}

/*
    Function: mth_GetQuadrant

    Description -
    Returns the quarter of a circle that an angle is in.

    1 argument:
    float angle - The angle from (0, 2*PI)
*/
int mth_GetQuadrant_Angle(float angle)
{
    if(angle <= PI_2)
        return 0;

    if(angle <= PI)
        return 1;

    if(angle <= PI3_2)
        return 2;
    else
        return 3;
}

float mth_Segment(float source, float segment)
{
    return source - (segment * (int)(source/segment));
}

int is_WithinBoundary(float *axis, float boundary)
{
	if(*axis > boundary || *axis < -boundary)
		return 0;

	return 1;
}
