#include "Vector2DInt.h"

#include "Memory.h"

Vector2DInt *vector2DInt_Create(int x, int y)
{
    Vector2DInt *newVector = (Vector2DInt *)mem_Alloc(sizeof(Vector2DInt));

    newVector->x = x;
    newVector->y = y;

    return newVector;
}

Vector2DInt vector2DInt_Add(Vector2DInt *a, Vector2DInt *b)
{
    Vector2DInt result;

    result.x = a->x + b->x;
    result.y = a->y + b->y;

    return result;
}

Vector2DInt vector2DInt_Subtract(Vector2DInt *a, Vector2DInt *b)
{
    Vector2DInt result;

    result.x = a->x - b->x;
    result.y = a->y - b->y;

    return result;
}

Vector2DInt vector2DInt_Multiply(Vector2DInt *a, Vector2DInt *b)
{
    Vector2DInt result;

    result.x = a->x * b->x;
    result.y = a->y * b->y;

    return result;
}

Vector2DInt vector2DInt_Divide(Vector2DInt *a, Vector2DInt *b)
{
    Vector2DInt result;

    result.x = a->x / b->x;
    result.y = a->y / b->y;

    return result;
}

int vector2DInt_MagSqr(Vector2DInt *a)
{
    return (a->x * a->x) + (a->y * a->y);
}

void vector2DInt_SetZero(Vector2DInt *a)
{
    a->x = a->y = 0;

    return;
}
