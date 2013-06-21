#ifndef VECTOR_H
#define VECTOR_H

typedef struct fVector2D
{
	float x;
	float y;
}   Vector2D;

void vector_Split_Int(Vector2D *srcVector,int *x,int *y);

void vector_Clear(Vector2D *vec1);

Vector2D vector_Subtract(Vector2D *vec1,Vector2D *vec2);

Vector2D vector_SubtractBy(Vector2D *vec1,float amount);

Vector2D vector_Add(Vector2D *vec1,Vector2D *vec2);

Vector2D vector_AddBy(Vector2D *vec1,float amount);

Vector2D vector_Divide(Vector2D *vec1,Vector2D *vec2);

Vector2D vector_DivideBy(Vector2D *vec1,float amount);

Vector2D vector_Multiply(Vector2D *vec1,Vector2D *vec2);

Vector2D vector_MultiplyBy(Vector2D *vec1,float amount);

Vector2D vector_Normalise(Vector2D *vec1);

Vector2D vector_Reverse(Vector2D *vec1);

float vector_Dot(Vector2D *vec1,Vector2D *vec2);

Vector2D vector_Reflect(Vector2D *objVelocity,Vector2D *planeNormal);

Vector2D vector_Perp(Vector2D *vec1);

int vector_Is_Zero(Vector2D *vect);

float vector_Magnitude(Vector2D *vec1);

float vector_Distance(Vector2D *vec1, Vector2D *vec2);
float vector_DistanceSqr(Vector2D *vec1, Vector2D *vec2);

#endif
