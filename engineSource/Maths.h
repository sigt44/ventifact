#ifndef MATH_H
#define MATH_H

#include <float.h>

#define MAXSQRTLOOKUP 1000
#define PI 3.141593f
#define PI_2 1.570796f
#define PI3_2 4.712389f
#define PI2 6.283185f
#define RAD2DEG PI/180

#define MTH_FILLANGLE 360

extern float f_Angles[MTH_FILLANGLE][2];

void mth_FillAngles(void);

int mth_FitAngleBoundry_Deg(int angle);

float mth_DistanceSqrF(float xDis, float yDis);

int mth_DistanceSqr(int xDis,int yDis);

int mth_Distance(int xDis,int yDis);

int mth_SameSignF(float a, float b);

int is_WithinBoundary(float *axis, float boundary);

int mth_GetQuadrant(float xDis, float yDis);

int mth_GetQuadrant_Angle(float angle);

float mth_Segment(float source, float segment);

#endif
