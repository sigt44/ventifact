#ifndef DIJKSTRASEARCH_H
#define DIJKSTRASEARCH_H

#include "../Node.h"
#include "../Graph.h"
#include "../../Common/List.h"

typedef struct Dijkstra_Search
{
    /*The index of the starting node for the search*/
    int startIndex;
    /*Index for the ending node of the search*/
    int endIndex;

    /*A list of D_Node structure, each represents a node to solve the search*/
    struct list *D_Nodes;

    /*A list of nodes that need to be visited by the search*/
    struct list *toVisitPath;

    /*This is the list the search returns, it contains the nodes that form the shortest path in order
    of first to last*/
    struct list *sortedPath;

} D_GraphSearch;

typedef struct Dijkstra_Node
{
    B_Node *base;
    int sumRoute; /*The current least summed weight of the node the search has found so far*/

    struct list *connectedNodes; /*Connected nodes*/

    unsigned int visited;/*if true then this node has been sorted and had its least summed weight calculated to the starting node*/

    struct Dijkstra_Node *previous;
} Dijkstra_Node;

struct list *Dijkstra_Search(D_GraphSearch *d, B_Graph *graph, int startIndex, int endIndex);

void Dijkstra_Setup(D_GraphSearch *d);

void Dijkstra_SetupNodes(D_GraphSearch *d, B_Graph *graph);

void Dijkstra_Clean(D_GraphSearch *d);

#endif
