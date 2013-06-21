#include <math.h>
#include "Vector2D.h"

#include "Maths.h"

void vector_Split_Int(Vector2D *srcVector,int *x,int *y)
{
    *x = (int)srcVector->x;
    *y = (int)srcVector->y;

    return;
}

void vector_Clear(Vector2D *vec1)
{
	vec1->x = 0.0f;
	vec1->y = 0.0f;

	return;
}

Vector2D vector_Subtract(Vector2D *vec1,Vector2D *vec2)
{
	Vector2D newVector;

	newVector.x = vec1->x - vec2->x;
	newVector.y = vec1->y - vec2->y;

	return newVector;
}

Vector2D vector_SubtractBy(Vector2D *vec1,float amount)
{
	Vector2D newVector;

	newVector.x = vec1->x - amount;
	newVector.y = vec1->y - amount;

	return newVector;
}

Vector2D vector_Add(Vector2D *vec1,Vector2D *vec2)
{
	Vector2D newVector;

	newVector.x = vec1->x + vec2->x;
	newVector.y = vec1->y + vec2->y;

	return newVector;
}

Vector2D vector_AddBy(Vector2D *vec1,float amount)
{
	Vector2D newVector;

	newVector.x = vec1->x + amount;
	newVector.y = vec1->y + amount;

	return newVector;
}

Vector2D vector_Divide(Vector2D *vec1,Vector2D *vec2)
{
	Vector2D newVector = {0.0f,0.0f};

	if(vec2->x != 0.0f)
		newVector.x = vec1->x / vec2->x;
	if(vec2->y != 0.0f)
		newVector.y = vec1->y / vec2->y;

	return newVector;
}

Vector2D vector_DivideBy(Vector2D *vec1,float amount)
{
	Vector2D newVector = {0.0f,0.0f};

	if(amount == 0.0f)
		return newVector;

	newVector.x = vec1->x / amount;
	newVector.y = vec1->y / amount;

	return newVector;
}

Vector2D vector_Multiply(Vector2D *vec1,Vector2D *vec2)
{
	Vector2D newVector;

	newVector.x = vec1->x * vec2->x;
	newVector.y = vec1->y * vec2->y;

	return newVector;
}

Vector2D vector_MultiplyBy(Vector2D *vec1,float amount)
{
	Vector2D newVector = {0.0f,0.0f};

	newVector.x = vec1->x * amount;
	newVector.y = vec1->y * amount;

	return newVector;
}

Vector2D vector_Normalise(Vector2D *vec1)
{
	Vector2D newVector = {0.0f,0.0f};
	float magnitude = (float)sqrt((vec1->x * vec1->x) + (vec1->y * vec1->y));

	if(magnitude != 0.0f)
	{
		magnitude = 1.0f / magnitude;
		newVector.x = vec1->x * magnitude;
		newVector.y = vec1->y * magnitude;
	}

	return newVector;
}

Vector2D vector_Reverse(Vector2D *vec1)
{
	Vector2D newVector = {0.0f,0.0f};

	newVector.x = -vec1->x;
	newVector.y = -vec1->y;

	return newVector;
}



float vector_Dot(Vector2D *vec1,Vector2D *vec2)
{
    /* u.w = |u||w|*cos(theta) */

	return (vec1->x * vec2->x) + (vec1->y * vec2->y);
}

Vector2D vector_Reflect(Vector2D *objVelocity,Vector2D *planeNormal)
{

	Vector2D normVector = vector_Normalise(planeNormal);
	Vector2D newVector;

	float dot = vector_Dot(objVelocity,&normVector);

	newVector = vector_MultiplyBy(&normVector, 2 * dot); /*Multiply the normal by the dot angle to find the direction of angle to reflect*/

	newVector = vector_Subtract(objVelocity,&newVector);
	return newVector;
}

Vector2D vector_Perp(Vector2D *vec1)
{
	Vector2D newVector = {0.0f,0.0f};

	newVector.x = -vec1->y;
	newVector.y = vec1->x;

	return newVector;
}

int vector_Is_Zero(Vector2D *vect)
{
	if(vect->x == 0.0f && vect->y == 0.0f)
		return 1;

	return 0;
}

float vector_Magnitude(Vector2D *vec1)
{
    return (float)sqrt((vec1->x * vec1->x) + (vec1->y * vec1->y));
}

float vector_Distance(Vector2D *vec1, Vector2D *vec2)
{
    Vector2D dV = vector_Subtract(vec2, vec1);

    return vector_Magnitude(&dV);
}

float vector_DistanceSqr(Vector2D *vec1, Vector2D *vec2)
{
    Vector2D dV = vector_Subtract(vec2, vec1);

    return (dV.x * dV.x) + (dV.y *dV.y);
}
