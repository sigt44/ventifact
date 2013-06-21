#ifndef GRAPH_H
#define GRAPH_H

#include "Node.h"
#include "Edge.h"

typedef struct B_Graph
{
	struct list *nodes;
	struct list *edges;

	int numNodes;
	int numEdges;

	int currentIndex;
	int topNodeIndex;

} B_Graph;

B_Graph *graph_Create(void);

void graph_Setup(B_Graph *graph);

void graph_Reset(B_Graph *graph);

void graph_Draw(B_Graph *graph, int layer, TTF_Font *font, SDL_Surface *nodeGraphic, Camera_2D *camera, SDL_Surface *destination);

void graph_Add_Node(B_Graph *graph, B_Node *node);

void graph_Remove_Node(B_Graph *graph, B_Node *node);

int graph_Remove_EdgesByNodes(B_Graph *graph, B_Node *nodeA, B_Node *nodeB);

void graph_Clear_Nodes(B_Graph *graph);

void graph_Add_Edge(B_Graph *graph, B_Edge *edge);

void graph_Add_Edge_DistanceWeight(B_Graph *graph, B_Edge *edge);

void graph_Add_EdgeAroundNode(B_Graph *graph, B_Edge *edgeA, B_Edge *edgeB, int weight);

void graph_Remove_Edge(B_Graph *graph, B_Edge *edge);

void graph_Clear_Edges(B_Graph *graph);

int graph_AssignIndex(B_Graph *graph);

int graph_CurrentIndex(B_Graph *graph);

int graph_PreviousIndex(B_Graph *graph, int fromIndex);

void graph_Clean(B_Graph *graph);

#endif
