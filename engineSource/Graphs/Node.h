#ifndef NODE_H
#define NODE_H
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "../Common/List.h"
#include "../Camera.h"
#include "../Graphics/Pixel.h"

typedef struct basic_Node
{
	int index;

	int x; /*Positions of the node, only needed to display the graph and not solve it*/
	int y;

	int degree; /*Number of edges connected to the node*/
	struct list *edges;

	void *adv; /*Used for more detailed information of the node*/

} B_Node;

B_Node *node_Create(int index, int x, int y);

void node_Draw(B_Node *node, int layer, SDL_Surface *nodeGraphic, Camera_2D *camera, SDL_Surface *destination);

void node_DrawInfo(B_Node *node, int layer, TTF_Font *font, Camera_2D *camera, SDL_Surface *destination);

B_Node *node_GetClosest(struct list *nodeList, int x, int y, int *oi_DistanceSqr);

B_Node *node_Get(struct list *nodeList, int index);

void node_AddEdge(B_Node *node, void *edge);

void node_RemoveEdge(B_Node *node, void *edge);

void *node_GetEdge(B_Node *node, int index);

void node_Clean(B_Node *node);

void node_Report_List(struct list *nodeList);

void node_Report(B_Node *node);

#endif
