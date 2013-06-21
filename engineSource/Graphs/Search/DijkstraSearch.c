#include <math.h>

#include "DijkstraSearch.h"
#include "../Edge.h"
#include "../Graph.h"

#include "../../Kernel/Kernel.h"
#include "../../File.h"

/*
    Function - Dijkstra_Setup

    Description -
    This function sets the values of a Dijstra_Search structure to their defaults, making it ready
    to be used.

    1 argument:
    Dijstra_Search *d - the structure to set the default values of
*/
void Dijkstra_Setup(D_GraphSearch *d)
{
    d->startIndex = 0;
    d->endIndex = 0;

    d->D_Nodes = NULL;
    d->toVisitPath = NULL;

    d->sortedPath = NULL;

    return;
}

/*
    Function - Dijkstra_Reset

    Description -
    This function prepares the values needed to start a new Dijkstra search.

    1 argument:
    Dijstra_Search *d - the structure to reset the values of
*/
void Dijkstra_Reset(D_GraphSearch *d)
{
    struct list *nodeList = d->D_Nodes;

    Dijkstra_Node *dNode = NULL;

    d->startIndex = 0;
    d->endIndex = 0;

    if(d->toVisitPath != NULL)
    {
        list_Clear(&d->toVisitPath);
    }

    while(nodeList != NULL)
    {
        dNode = nodeList->data;
        dNode->visited = 0;
        dNode->previous = NULL;

        nodeList = nodeList->next;
    }

    d->toVisitPath = NULL;

    d->sortedPath = NULL;

    return;
}

/*
    Function - Dijkstra_CreateNode

    Description -
    This function creates a new custom node specified for the dijkstra search proccess.

    3 arguments:
    B_Node *base - The most basic node that this one is to be derived from.
    int weight - This helps the search to determine how hard it is in its current state to get to this node
    Dijkstra_Node *previous - Keeps track of the node that is found to be the best one to connect to
                    for the shortest path. (You only need to find the end node and then keep
                    traversing through this variable until it becomes NULL, the node that containes
                    previous = NULL will be the starting one.
*/
Dijkstra_Node *Dijkstra_CreateNode(B_Node *base, int weight, Dijkstra_Node *previous)
{
    Dijkstra_Node *newNode = (Dijkstra_Node *)mem_Malloc(sizeof(Dijkstra_Node), __LINE__, __FILE__);

    newNode->base = base;
    newNode->sumRoute = weight;

    newNode->connectedNodes = NULL;

    newNode->visited = 0;

    newNode->previous = previous;

    base->adv = newNode;

    return newNode;
}

/*
    Function - Dijkstra_NodeScan

    Description -
    Searches through all the edges in the graph, if any of them are connected to the supplied node n
    then its adds the other node of the edge to n->cNode.

    2 arguments:
    struct list *edges - The base list of all the edges in the graph
    Dijkstra_Node *n - The dijkstra node that is to have its connected nodes found from the list of edges.
*/
void Dijkstra_NodeScan(struct list *edges, Dijkstra_Node *n)
{
    B_Edge *e = NULL;

    /*Iterate through all the edges for the search*/
    while(edges != NULL)
    {
        e = edges->data;

        /*If the edges start node is equal to the supplied node n*/
        if(e->startNode == n->base)
            list_Push_Sort(&n->connectedNodes, e->endNode->adv, e->weight); /*Add in the node at the end of the edge to n*/
        else if(e->endNode == n->base) /*Else if the edges end node is n*/
            list_Push_Sort(&n->connectedNodes, e->startNode->adv, e->weight); /*Add in the node at the start of the edge to n*/

        edges = edges->next;
    }

    return;
}

/*Setup the nodes in a basic graph for use with Dijkstra's algorithm*/
/*
    Function - Dijkstra_SetupNodes

    Description -
    This function creates and sets up Dijkstra versions of basic nodes so that they can be used to
    perform the search.

    2 arguments:
    Dijkstra_Search *d - The search structure to setup the nodes in.
    B_Graph *graph - The basic graph of nodes and edges that are to be used.
*/
void Dijkstra_SetupNodes(D_GraphSearch *d, B_Graph *graph)
{
    struct list *nodes;

    Dijkstra_Node *n = NULL;
    B_Node *b = NULL;


    /*For all the nodes in the base graph, create a Dijkstra version of that node and
    push it into the Dijstra search node list*/
    nodes = graph->nodes;
    while(nodes != NULL)
    {
        b = nodes->data;

        n = Dijkstra_CreateNode(b, 0, NULL);

        list_Push(&d->D_Nodes, n, 0);

        nodes = nodes->next;
    }

    /*For all the Dijkstra nodes in the search, perform a scan of the edges and create a direct link to the nodes
    that are connected to the Dijkstra node.*/
    nodes = d->D_Nodes;
    while(nodes != NULL)
    {
        n = nodes->data;
        Dijkstra_NodeScan(graph->edges, n);

        nodes = nodes->next;
    }

    return;
}

void Dijkstra_UpdateSearch(struct list *l, Dijkstra_Node *currentNode)
{
}

void Dijkstra_Node_Report(Dijkstra_Node *node)
{
    printf("DNode %d: W %d\n", node->base->index, node->sumRoute);
    return;
}

void Dijkstra_Node_Report_List(struct list *nodeList)
{
    int x = 0;
    while(nodeList != NULL)
    {
        Dijkstra_Node_Report(nodeList->data);
        x++;
        printf("%d %d\n",x,nodeList->info);
        nodeList = nodeList->next;
    }

    return;
}

void Dijkstra_Node_Clean(Dijkstra_Node *node)
{
    while(node->connectedNodes != NULL)
    {
        list_Pop(&node->connectedNodes);
    }

    if(node->base != NULL)
        node->base->adv = NULL;

    return;
}


/*
    Function - Dijkstra_CalcWeight

    Description -
    This function returns the weight cost of traversing two specified nodes.

    2 arguments:
    Dijkstra_Node *currentNode - The node that will be traversed from.
    Dijkstra_Node *checkNode - The node that will be traversed to.
*/
int Dijkstra_CalcWeight(Dijkstra_Node *currentNode, Dijkstra_Node *checkNode)
{
    B_Node *cur, *ch;
    int xDis, yDis;

    cur = currentNode->base;
    ch = checkNode->base;

    /*Calculate the weight by Pythagoras*/
    xDis = ch->x - cur->x;
    yDis = ch->y - cur->y;
    return (xDis * xDis) + (yDis * yDis);
}


/*
    Function - search_Dijkstra

    Description -
    This function calculates the quickest route of two nodes in a graph using Dijkstra's shortest path algorithm.
    It returns the shortest route of nodes in order of first to last as a linked list structure.

    First push the starting node with summed weight 0 into the toVisit list.

    While (the search node (A) has not found the end index) and (the toVisit list has nodes left)
        obtain the node (A) with the least summed weight from the toVisit list
        set node (A) as visited

        For all the nodes that are connected to node (A)
            check if that connected node has already been visited
            if not
                check if that connected node has already been placed in the toVisit list
                if so
                    check if the summed weight from node (A) to this connected node is less than its current summed weight
                    if so
                        update this connected nodes summedWeight and place it back into the toVisit list
                if not
                    place it in the toVisit list, with its summedWeight value as node (A)'s summedWeight + edge weight

        end For
    end While


    3 arguments:
    B_Graph *graph - The base graph of nodes and edges that the search is to be performed on.
    int startIndex - The starting index of the node to begin the search with.
    int endIndex - The ending index of the node that the search ends with.
*/
struct list *Dijkstra_Search(D_GraphSearch *d, B_Graph *graph, int startIndex, int endIndex)
{
    struct list *l = NULL;
    struct list *cl = NULL;
    Dijkstra_Node *n = NULL;
    Dijkstra_Node *connectedNode = NULL;

    B_Node *startNode = NULL;

    int summedWeight = 0;

    /*Reset the values from the previous search*/
    Dijkstra_Reset(d);

    /*Setup the nodes so that they know exactly which other nodes are connected to them*/
    /*Dijkstra_SetupNodes(d, graph); Should have already been done to save time*/

    /*Setup the first node to start the search*/
    startNode = node_Get(graph->nodes, startIndex);

    if(startNode == NULL)
    {
        printf("Error: Could not find start node of index %d in graph\n", startIndex);
        return NULL;
    }

    n = startNode->adv;

    /*Push in the starting node*/
    list_Push(&d->toVisitPath, n, 0);

    /*While there are nodes still left to visit*/
    while(d->toVisitPath != NULL)
    {
        //file_Log(ker_Log(), 1, "STATUS: Getting the smallest route to the next node\n");

        /*Obtain the node with the least summed weight that needs to be visited*/
        n = d->toVisitPath->data; /*This is always the head of the toVisitPath linked list as the
                                    nodes get sorted as they are pushed in*/

        n->sumRoute = d->toVisitPath->info;

        list_Pop(&d->toVisitPath);

        /*Set the node as visited*/
        n->visited = 1;

        /*If the node is the ending one then exit the loop*/
        if(n->base->index == endIndex)
        {
            //file_Log(ker_Log(), 1, "END INDEX FOUND! %d %p\n", endIndex, d->toVisitPath);
            list_Clear(&d->toVisitPath);
        }
        else
        {
            l = n->connectedNodes;

            //file_Log(ker_Log(), 1, "STATUS: Located at %d of weight %d\n", n->base->index, n->sumRoute);

            /*Search through all the connected nodes*/
            while(l != NULL)
            {
                connectedNode = l->data;

                summedWeight = n->sumRoute + l->info; //+ Dijkstra_CalcWeight(n, connectedNode);

                //file_Log(ker_Log(), 1, "STATUS: %d is connected to %d with weight %d (%d)\n",connectedNode->base->index, n->base->index, summedWeight, l->info);

                /*If the node has not already been visited*/
                if(connectedNode->visited == 0)
                {
                    /*Is this node already in the toVisit list?*/
                    cl = list_MatchCheck(d->toVisitPath, connectedNode);

                    if(cl != NULL) /*If it is*/
                    {
                        //file_Log(ker_Log(), 1, "STATUS: %d is already in the path, is the new route smaller? %d < %d?\n", connectedNode->base->index, summedWeight, cl->info);

                        /*Is the connection to this node found to be the shortest so far? If so then update it*/
                        if(summedWeight < cl->info)
                        {
                            //file_Log(ker_Log(), 1,"STATUS: Yes, replace\n");
                            /*First take it out of the toVisit list*/
                            list_Delete_Node(&d->toVisitPath, cl);
                            /*Then push it back in but in the new correct order*/
                            list_Push_Sort(&d->toVisitPath, connectedNode, summedWeight);

                            /*Update the nodes shortest path link so that it can be traversed back to the starting node*/
                            connectedNode->previous = n;
                        }
                        else
                        {
                            //file_Log(ker_Log(), 1,"STATUS: Not smaller, ignore\n");
                        }
                    }
                    else
                    {
                        //file_Log(ker_Log(), 1, "STATUS: There is no %d in the path, so pushing it in\n", connectedNode->base->index);
                        /*Push the node in the toVisit list in sorted order, the one with the least summed weight will be at the head of the list*/
                        list_Push_Sort(&d->toVisitPath, connectedNode, summedWeight);

                        /*Update the nodes shortest path link so that it can be traversed back to the starting node*/
                        connectedNode->previous = n;
                    }
                }
                else
                {
                    //file_Log(ker_Log(), 1,"STATUS: %d has already been sorted\n", connectedNode->base->index);
                }

                l = l->next;
            }
            //file_Log(ker_Log(), 1, "\n");
        }
    }

    //file_Log(ker_Log(), 1, "STATUS: No more nodes to process\n");

    /*Push the nodes that made the shortest route into a linked list*/
    if(n != NULL)
    {
        if(n->base->index == endIndex)
        {
            while(n->previous != NULL)
            {
                list_Push(&d->sortedPath, n->base, n->sumRoute);
                n = n->previous;
            }
        }
        else
            file_Log(ker_Log(),1, "Search Warning: Could not find the end index of a graph (start = %d, end = %d)\n",startIndex, endIndex);

        list_Push(&d->sortedPath, startNode, 0);
    }

    //node_Report_List(d->sortedPath);

    return d->sortedPath;
}

void Dijkstra_Clean(D_GraphSearch *d)
{
    d->sortedPath = NULL;

    while(d->toVisitPath != NULL)
    {
        list_Pop(&d->toVisitPath);
    }

    while(d->D_Nodes != NULL)
    {
        Dijkstra_Node_Clean(d->D_Nodes->data);

        mem_Free(d->D_Nodes->data);

        list_Pop(&d->D_Nodes);
    }

    return;
}


