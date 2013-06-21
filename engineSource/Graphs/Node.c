#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "../Common/List.h"

#include "Node.h"
#include "Edge.h"

#include "../Kernel/Kernel_State.h"
#include "../Graphics/SurfaceManager.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Pixel.h"
#include "../Font.h"

/*
    Function - node_Create

    Description -
    Returns a pointer to a new base node of structure B_Node.

    3 arguments:
    int index - this is the unique index to give the node
    int x - this is the x location of the node
    int y - the y location of the node
*/
B_Node *node_Create(int index, int x, int y)
{
	B_Node *newNode = (B_Node *)mem_Malloc(sizeof(B_Node), __LINE__, __FILE__);

	if(newNode == NULL)
		return NULL;

	newNode->index = index;
	newNode->x = x;
	newNode->y = y;

	newNode->degree = 0;
	newNode->edges = NULL;

	newNode->adv = NULL;

	return newNode;
}

/*
    Function - node_Draw

    Description -
    Draws the specified node.
    The graphic will be a simple box if nodeGraphic is NULL.

    5 arguments:
    B_Node *node - the node to draw
    int layer - layer to draw the node on.
    SDL_Surface *nodeGraphic - The graphic of the node, if NULL then will be a box.
    Camera_2D *camera - The camera to draw the node in.
    SDL_Surface *destination - the surface to draw the node on
*/
void node_Draw(B_Node *node, int layer, SDL_Surface *nodeGraphic, Camera_2D *camera, SDL_Surface *destination)
{
    if(nodeGraphic != NULL)
    {
        surfaceMan_Push(gKer_DrawManager(), 0, layer, 0, camera2D_RelativeX(camera, node->x) - nodeGraphic->w/2, camera2D_RelativeY(camera, node->y) - nodeGraphic->h/2, nodeGraphic, destination, NULL);
    }
    else
    {
        draw_Box(camera2D_RelativeX(camera, node->x) - 2, camera2D_RelativeY(camera, node->y) - 2, 4, 4, 1, &colourLightBlue, destination);
    }

	return;
}

/*
    Function - node_DrawInfo

    Description -
    A function that draws some information of a node.
    Its index and degree.

    5 arguments:
    B_Node *node - the node to draw information of.
    int layer - the layer to draw the node info on.
    TTF_Font *font - the font of the text.
    Camera_2D *camera - the camera to draw the information in.
    SDL_Surface *destination - the surface to draw the information on.
*/
void node_DrawInfo(B_Node *node, int layer, TTF_Font *font, Camera_2D *camera, SDL_Surface *destination)
{
    if(node->degree > 0)
        text_Draw_Arg(camera2D_RelativeX(camera, node->x) + 5, camera2D_RelativeY(camera, node->y) - 15, destination, font, &tColourWhite, layer, 0, "%d (%d)", node->index, node->degree);
    else
        text_Draw_Arg(camera2D_RelativeX(camera, node->x) + 5, camera2D_RelativeY(camera, node->y) - 15, destination, font, &tColourWhite, layer, 0, "%d", node->index);

    return;
}

/*
    Function - node_Get

    Description -
    A function that searches in a linked list for a node with specified index
    and then returns a pointer to it. Returns NULL if no node is found.

    2 arguments:
    struct list *nodeList - the linked list that contains the node to search for
    int index - the index of the node to search for
*/
B_Node *node_Get(struct list *nodeList, int index)
{
    B_Node *returnNode = NULL;

    while(nodeList != NULL)
    {
        returnNode = nodeList->data;

        if(returnNode->index == index)
        {
            return returnNode;
        }

        nodeList = nodeList->next;
    }

    return NULL;
}

/*
    Function: node_GetClosest

    Description -
    Returns the closest node in the linked list from the point at (x, y).

    4 arguments:
    struct list *nodeList - The list of nodes.
    int x - The x position of the point to get the closest node from.
    int y - The y position of the point to get the closest node from.
    int *oi_DistanceSqr - If not NULL then will set this to be the closest distance squared of the closest node.
*/
B_Node *node_GetClosest(struct list *nodeList, int x, int y, int *oi_DistanceSqr)
{
    B_Node *checkNode = NULL;
    B_Node *closestNode = NULL;

    int closestSoFar = -1;
    int xDis = 0;
    int yDis = 0;
    int distanceSqr = 0;

    while(nodeList != NULL)
    {
        checkNode = nodeList->data;

        xDis = checkNode->x - x;
        yDis = checkNode->y - y;

        distanceSqr = (xDis * xDis) + (yDis * yDis);

        /*If the distance to the node is the smallest so far*/
        if(closestSoFar == -1 || closestSoFar > distanceSqr)
        {
            closestSoFar = distanceSqr;
            closestNode = checkNode; /*Set the closest node*/
        }

        nodeList = nodeList->next;
    }

    if(oi_DistanceSqr != NULL)
    {
        *oi_DistanceSqr = closestSoFar;
    }

    return closestNode;
}

/*
    Function - node_Report_List

    Description -
    Prints to stdout the attributes of all the nodes in a linked list.

    1 argument:
    struct list *nodeList - the list that contains the nodes
*/
void node_Report_List(struct list *nodeList)
{
    while(nodeList != NULL)
    {
        node_Report(nodeList->data);
        nodeList = nodeList->next;
    }

    return;
}

/*
    Function - node_Report

    Description -
    Prints to stdout the attributes of a node.

    1 argument:
    B_Node *node - the node to print the attributes of
*/
void node_Report(B_Node *node)
{
    printf("Node %d: (x %d y %d) degree %d advNode %p\n", node->index, node->x, node->y, node->degree, node->adv);
    return;
}

/*
    Function - node_AddEdge

    Description -
    Adds a link of an edge to a node.

    2 arguments -
    B_Node *node - The node that should be connected to the edge.
    B_Edge *edge - The edge to add.
*/
void node_AddEdge(B_Node *node, void *edge)
{
    node->degree ++;

    list_Stack(&node->edges, edge, node->degree);

    return;
}

/*
    Function - node_RemoveEdge

    Description -
    Removes a link of an edge from a node. This function does not
    delete the edge.

    2 arguments -
    B_Node *node - The node that is linked to the edge.
    B_Edge *edge - The edge to remove from the node.
*/
void node_RemoveEdge(B_Node *node, void *edge)
{
    node->degree --;

    list_Delete_NodeFromData(&node->edges, edge);

    return;
}

/*
    Function - node_GetEdge

    Description -
    Retrieve an edge from a node.

    2 arguments -
    B_Node *node - The node that is linked to the edge.
    int index - The position of the edge that was placed in the nodes edges list.
*/
void *node_GetEdge(B_Node *node, int index)
{
    struct list *edgeList = node->edges;
    int x = 0;

    for(x = 0; x < index; x++)
    {
        edgeList = edgeList->next;
    }

    return edgeList->data;
}

/*
    Function - node_Clean

    Description -
    Removes all connected edges to the node and resets its degree to 0.

    1 argument -
    B_Node *node - the node to clean
*/
void node_Clean(B_Node *node)
{
    B_Edge *edge = NULL;
	node->degree = 0;

    while(node->edges != NULL)
    {
        edge = node->edges->data;

        edge_Unlink(edge);
    }

	return;
}
