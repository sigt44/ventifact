#ifndef VECTORINT2D_H
#define VECTORINT2D_H

typedef struct Vector2DInt {
    int x;
    int y;
} Vector2DInt;

Vector2DInt *vector2DInt_Create(int x, int y);

Vector2DInt vector2DInt_Add(Vector2DInt *a, Vector2DInt *b);

Vector2DInt vector2DInt_Subtract(Vector2DInt *a, Vector2DInt *b);

Vector2DInt vector2DInt_Multiply(Vector2DInt *a, Vector2DInt *b);

Vector2DInt vector2DInt_Divide(Vector2DInt *a, Vector2DInt *b);

int vector2DInt_MagSqr(Vector2DInt *a);

void vector2DInt_SetZero(Vector2DInt *a);

#endif
