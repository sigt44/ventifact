#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "Edge.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Pixel.h"
#include "../Graphics/PixelManager.h"
#include "../Kernel/Kernel_State.h"

#include "../Memory.h"
#include "../Collision.h"

B_Edge *edge_Create(void *startNode, void *endNode, int weight)
{
	B_Edge *newEdge = (B_Edge *)mem_Malloc(sizeof(B_Edge), __LINE__, __FILE__);

	if(newEdge == NULL)
		return NULL;

    edge_Setup(newEdge, startNode, endNode, weight);

	return newEdge;
}

B_Edge *edge_Create_Index(struct list *nodeList, int startIndex, int endIndex, int weight)
{
	B_Edge *newEdge = (B_Edge *)mem_Malloc(sizeof(B_Edge), __LINE__, __FILE__);

	B_Node *startNode = NULL;
	B_Node *endNode = NULL;
	B_Node *compNode = NULL;

	if(newEdge == NULL)
		return NULL;

    /*Find the required node indexes in the list and assign them as normal*/
    while(nodeList != NULL && (startNode == NULL || endNode == NULL))
    {
        compNode = nodeList->data;

        if(compNode->index == startIndex)
        {
            startNode = compNode;
        }

        if(compNode->index == endIndex)
        {
            endNode = compNode;
        }

        nodeList = nodeList->next;
    }

    if(startNode == NULL)
        printf("Error: Edge(%d -> %d : %d) Could not find start node in list\n", startIndex, endIndex, weight);
    if(endNode == NULL)
        printf("Error: Edge(%d -> %d : %d) Could not find end node in list\n", startIndex, endIndex, weight);

    edge_Setup(newEdge, startNode, endNode, weight);

	return newEdge;
}

void edge_Setup(B_Edge *edge, void *startNode, void *endNode, int weight)
{
    edge->startNode = startNode;
    node_AddEdge(startNode, edge);

	edge->endNode = endNode;
	node_AddEdge(endNode, edge);

	edge->weight = weight;

    return;
}

void edge_Draw(B_Edge *edge, int layer, Uint16P *colour, Camera_2D *camera, SDL_Surface *destination)
{
    B_Node *s = edge->startNode;
    B_Node *e = edge->endNode;

    pixelMan_PushLine(gKer_DrawManager(), layer, camera2D_RelativeX(camera, s->x), camera2D_RelativeY(camera, s->y), camera2D_RelativeX(camera, e->x), camera2D_RelativeY(camera, e->y), 2, *colour, destination);

	return;
}

void edge_Unlink(B_Edge *edge)
{
    if(edge->startNode != NULL)
    {
        node_RemoveEdge(edge->startNode, edge);
        edge->startNode = NULL;
    }

    if(edge->endNode != NULL)
    {
        node_RemoveEdge(edge->endNode, edge);
        edge->endNode = NULL;
    }

    return;
}

/*
    Function: edge_GetClosest

    Description -
    Returns the closest edge in the linked list from the point at (x, y).
    The distance from the edge to that point is based of the perpendicular length of
    the point to the line.

    4 arguments:
    struct list *edgeList - The list of edges.
    float x - The x position of the point to get the closest edge from.
    float y - The y position of the point to get the closest edge from.
    int *oi_DistanceSqr - If not NULL then will set this to be the closest distance squared of the closest edge.
*/
B_Edge *Edge_GetClosest(struct list *edgeList, float x, float y, int *oi_DistanceSqr)
{
    B_Edge *checkEdge = NULL;
    B_Edge *closestEdge = NULL;

    Vector2D edgeStart = {0.0f, 0.0f};
    Vector2D edgeEnd = {0.0f, 0.0f};

    Vector2D point = {0.0f, 0.0f};
    Vector2D intersection = {0.0f, 0.0f};

    float closestSoFar = -1.0f;
    float distanceSqr = 0.0f;

    point.x = x;
    point.y = y;

    while(edgeList != NULL)
    {
        checkEdge = edgeList->data;

        /*Get the starting and ending positions of the edges line*/
        edgeStart.x = (float)checkEdge->startNode->x;
        edgeStart.y = (float)checkEdge->startNode->y;

        edgeEnd.x = (float)checkEdge->endNode->x;
        edgeEnd.y = (float)checkEdge->endNode->y;

        /*Get the intersection point of the line and the perpendicular line from (x, y)*/
        intersection = collision_Point_PerpLine(&edgeStart, &edgeEnd, &point);

        /*Check if the intersection is inside of the edge line*/
        if(collision_Point_IsInRange(edgeStart.x, edgeEnd.x, intersection.x) == 1 ||
           collision_Point_IsInRange(edgeStart.y, edgeEnd.y, intersection.y) == 1)
        {
            /*Calculate the distance between the intersection point and (x, y)*/
            distanceSqr = vector_DistanceSqr(&intersection, &point);

            /*If the distance to the edge is the smallest so far*/
            if(closestSoFar == -1.0f || closestSoFar > distanceSqr)
            {
                closestSoFar = distanceSqr;
                closestEdge = checkEdge; /*Set the closest edge*/
            }
        }

        edgeList = edgeList->next;
    }

    if(oi_DistanceSqr != NULL)
    {
        *oi_DistanceSqr = (int)closestSoFar;
    }

    return closestEdge;
}

void edge_Clean(B_Edge *edge)
{
	edge->weight = 0;

    edge_Unlink(edge);

	return;
}
