#ifndef EDGE_H
#define EDGE_H

#include "Node.h"

typedef struct basic_Edge
{
	B_Node *startNode;
	B_Node *endNode;

	int weight;
} B_Edge;

B_Edge *edge_Create(void *startNode, void *endNode, int weight);

B_Edge *edge_Create_Index(struct list *nodeList, int startIndex, int endIndex, int weight);

void edge_Setup(B_Edge *edge, void *startNode, void *endNode, int weight);

void edge_Draw(B_Edge *edge, int layer, Uint16P *colour, Camera_2D *camera, SDL_Surface *destination);

void edge_Unlink(B_Edge *edge);

B_Edge *Edge_GetClosest(struct list *edgeList, float x, float y, int *oi_DistanceSqr);

void edge_Clean(B_Edge *edge);

#endif
