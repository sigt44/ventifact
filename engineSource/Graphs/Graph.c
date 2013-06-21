#include <stdio.h>

#include "Graph.h"
#include "Node.h"
#include "Edge.h"

#include "../Memory.h"
#include "../Maths.h"

B_Graph *graph_Create(void)
{
    B_Graph *newGraph = (B_Graph *)mem_Malloc(sizeof(B_Graph), __LINE__, __FILE__);

    graph_Setup(newGraph);

    return newGraph;
}

void graph_Setup(B_Graph *graph)
{
    graph->numEdges = 0;
    graph->numNodes = 0;

    graph->currentIndex = 0;
    graph->topNodeIndex = 0;

    graph->nodes = NULL;
    graph->edges = NULL;

    return;
}

void graph_Reset(B_Graph *graph)
{
    graph_Clean(graph);
    graph_Setup(graph);

    return;
}

int graph_AssignIndex(B_Graph *graph)
{
    graph->currentIndex = graph->topNodeIndex + 1;

    return graph->currentIndex;
}

int graph_CurrentIndex(B_Graph *graph)
{
    return graph->currentIndex;
}

int graph_PreviousIndex(B_Graph *graph, int fromIndex)
{
    struct list *nodes = graph->nodes;
    B_Node *nodeCmp = NULL;
    int highestSoFar = -1;

    while(nodes != NULL)
    {
        nodeCmp = nodes->data;

        if(nodeCmp->index < fromIndex && nodeCmp->index > highestSoFar)
        {
            highestSoFar = nodeCmp->index;
        }

        nodes = nodes->next;
    }

    return highestSoFar;
}

void graph_Draw(B_Graph *graph, int layer, TTF_Font *font, SDL_Surface *nodeGraphic, Camera_2D *camera, SDL_Surface *destination)
{
    struct list *g = NULL;

    g = graph->edges;
    while(g != NULL)
    {
        edge_Draw(g->data, layer, &colourBlack, camera, destination);
        g = g->next;
    }

    g = graph->nodes;
    while(g != NULL)
    {
        node_Draw(g->data, layer, nodeGraphic, camera, destination);
        node_DrawInfo(g->data, layer, font, camera, destination);
        g = g->next;
    }

    return;
}

void graph_Add_Node(B_Graph *graph, B_Node *node)
{
    graph->numNodes ++;

    list_Stack(&graph->nodes, node, 0);

    if(node->index > graph->topNodeIndex)
    {
        graph->topNodeIndex = node->index;
    }

    return;
}

void graph_Remove_Node(B_Graph *graph, B_Node *node)
{
    graph->numNodes --;

    list_Delete_NodeFromData(&graph->nodes, node);

    while(node->edges != NULL)
    {
        graph_Remove_Edge(graph, node->edges->data);
    }

    node_Clean(node);

    mem_Free(node);

    return;
}

void graph_Clear_Nodes(B_Graph *graph)
{
    while(graph->nodes != NULL)
    {
        node_Clean(graph->nodes->data);

        mem_Free(graph->nodes->data);

        list_Pop(&graph->nodes);
    }

    return;
}

void graph_Add_Edge(B_Graph *graph, B_Edge *edge)
{
    graph->numEdges ++;

    list_Stack(&graph->edges, edge, 0);

    return;
}

void graph_Remove_Edge(B_Graph *graph, B_Edge *edge)
{
    graph->numEdges --;

    list_Delete_NodeFromData(&graph->edges, edge);

    edge_Clean(edge);

    mem_Free(edge);

    return;
}

int graph_Remove_EdgesByNodes(B_Graph *graph, B_Node *nodeA, B_Node *nodeB)
{
    int numRemoved = 0;
    struct list *edges = nodeA->edges;
    struct list *nextEdge = NULL;
    B_Edge *edge = NULL;

    while(edges != NULL)
    {
        edge = edges->data;
        nextEdge = edges->next;

        if(edge->startNode == nodeB || edge->endNode == nodeB)
        {
            graph_Remove_Edge(graph, edge);

            numRemoved ++;
        }

        edges = nextEdge;
    }

    return numRemoved;
}

void graph_Clear_Edges(B_Graph *graph)
{
    while(graph->edges != NULL)
    {
        edge_Clean(graph->edges->data);

        mem_Free(graph->edges->data);

        list_Pop(&graph->edges);
    }

    return;
}

void graph_Add_Edge_DistanceWeight(B_Graph *graph, B_Edge *edge)
{
    int distanceSqr = 0;

    distanceSqr = mth_DistanceSqr((edge->endNode->x - edge->startNode->x), (edge->endNode->y - edge->startNode->y));

    edge->weight = distanceSqr;

    graph_Add_Edge(graph, edge);

    return;
}

void graph_Add_EdgeAroundNode(B_Graph *graph, B_Edge *edgeA, B_Edge *edgeB, int weight)
{
    if(edgeA->endNode == edgeB->startNode)
    {
        graph_Add_Edge(graph, edge_Create(edgeA->startNode, edgeB->endNode, weight));
    }
    else if(edgeA->endNode == edgeB->endNode)
    {
        graph_Add_Edge(graph, edge_Create(edgeA->startNode, edgeB->startNode, weight));
    }
    else if(edgeA->startNode == edgeB->startNode)
    {
        graph_Add_Edge(graph, edge_Create(edgeA->endNode, edgeB->endNode, weight));
    }
    else
    {
        graph_Add_Edge(graph, edge_Create(edgeA->endNode, edgeB->startNode, weight));
    }

    return;
}

void graph_Clean(B_Graph *graph)
{
    graph_Clear_Nodes(graph);

    graph_Clear_Edges(graph);

    return;
}

