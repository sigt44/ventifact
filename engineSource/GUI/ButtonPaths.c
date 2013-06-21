#include "ButtonPaths.h"

#include "Spine.h"
#include "SpineEntity.h"

#include "../Common/List.h"
#include "../Common/ListWatcher.h"
#include "../Controls.h"
#include "../Maths.h"

/*
    Function: uiPath_Create

    Description -
    Creates a path structure for an entity with no links.

    4 arguments:
    Ui_Spine_Entity *spEntity - The spine entity of the path.
    int layer - The link layer.
    Ui_Path *owner - The owner of this path, can be NULL for no owner.
*/
Ui_Path *uiPath_Create(Ui_Spine_Entity *spEntity, int layer, Ui_Path *owner)
{
    Ui_Path *newPath = (Ui_Path *)mem_Alloc(sizeof(Ui_Path));
    int x = 0;

    newPath->spEntity = spEntity;
    newPath->entity = spEntity->entity;
    newPath->type = spEntity->type;
    newPath->layer =layer;
    newPath->owner = owner;
    newPath->remove = 0;
    newPath->selected = 0;

    for(x = 0; x < UI_DIRECTIONS; x++)
    {
        newPath->direction[x] = NULL;
    }

    switch(newPath->type)
    {
        default:
        printf("Error unsupported path entity type %d\n", newPath->type);
        break;

        case UIE_BUTTON:
        newPath->typeEntity.button = newPath->entity;

        /*Add a link to the path so that if the entity was to be deleted,
        the path->entity pointer would be set to NULL*/
        depWatcher_AddPnt(&newPath->typeEntity.button->dW, &newPath->entity);
        break;

        case UIE_SCROLL:
        newPath->typeEntity.scroll = newPath->entity;

        depWatcher_AddPnt(&newPath->typeEntity.scroll->dW, &newPath->entity);
        break;

        case UIE_TEXTINPUT:
        newPath->typeEntity.textInput = newPath->entity;

        depWatcher_AddPnt(&newPath->typeEntity.textInput->dW, &newPath->entity);
        break;
    }

    return newPath;
}

int uiPath_Clean(Ui_Path *path)
{
    if(path->entity != NULL)
    {
        switch(path->type)
        {
            case UIE_BUTTON:
            depWatcher_RemovePnt(&path->typeEntity.button->dW, &path->entity);
            break;

            case UIE_SCROLL:
            depWatcher_RemovePnt(&path->typeEntity.scroll->dW, &path->entity);
            break;

            case UIE_TEXTINPUT:
            depWatcher_RemovePnt(&path->typeEntity.textInput->dW, &path->entity);
            break;
        }
    }

    return 0;
}

void uiPath_Disable(Ui_Path *path)
{
    path->remove += 1;

    return;
}

int uiPath_SupportedType(void *entity, int type)
{
    Spine_EntityUnionType entityType;

    switch(type)
    {
        default:
        return 0;
        break;

        case UIE_BUTTON:
        entityType.button = entity;

        if(entityType.button->graphic == NULL || entityType.button->rect.w == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }

        break;

        case UIE_SCROLL:
        case UIE_TEXTINPUT:
        return 1;
        break;
    }

    return 0;
}

int uiPath_GetDirection(float tan, float xDis, float yDis)
{
    const float tan225 = 0.966f;/* An estimate of the tangent(40 degrees) to 3dp*/
    const float tan675 = 1.036f; /* An estimate of the tangent(50 degrees) to 3dp*/
    int quadrant = mth_GetQuadrant(xDis, yDis);
    //printf("Tan %.2f, xDis %.2f, yDis %.2f, Quadrant %d\n", tan, xDis, yDis, quadrant);

    switch(quadrant)
    {
        /*First quadrant*/
        case 0:

        if(tan < tan225) /*When tan(angle) = 0.414 angle is close to 22.5*/
            return UI_RIGHT;
        else if(tan > tan675) /*tan(angle) = 2.414, angle is close to 67.5*/
            return UI_UP;
        else    /*tan(angle) lies between 0.414 and 2.414 therefore its closer to the diagonal, 45 degrees*/
            return UI_UPRIGHT;

        break;

        /*Second quadrant*/
        case 1:

        if(tan > -tan225)
            return UI_LEFT;
        else if(tan < -tan675)
            return UI_UP;
        else
            return UI_UPLEFT;

        break;

        /*Third quadrant*/
        case 2:

        if(tan < tan225)
            return UI_LEFT;
        else if(tan > tan675)
            return UI_DOWN;
        else
            return UI_DOWNLEFT;

        break;

        /*Fourth quadrant*/
        case 3:

        if(tan > -tan225)
            return UI_RIGHT;
        else if(tan < -tan675)
            return UI_DOWN;
        else
            return UI_DOWNRIGHT;

        break;
    }

    return -1;
}

/*
    Function: uiPath_ReverseLink

    Description -
    Returns the link type in the other direction as the parameter link type.

    1 argument:
    int linkDirection - The link type to reverse.
*/
int uiPath_ReverseLink(int linkDirection)
{
    switch(linkDirection)
    {
        default:break;

        case UI_UP:
        return UI_DOWN;
        break;

        case UI_DOWN:
        return UI_UP;
        break;

        case UI_LEFT:
        return UI_RIGHT;
        break;

        case UI_RIGHT:
        return UI_LEFT;
        break;

        case UI_UPLEFT:
        return UI_DOWNRIGHT;
        break;

        case UI_DOWNRIGHT:
        return UI_UPLEFT;
        break;

        case UI_UPRIGHT:
        return UI_DOWNLEFT;
        break;

        case UI_DOWNLEFT:
        return UI_UPRIGHT;
        break;

        case UI_ENTER:
        return UI_EXIT;
        break;

        case UI_EXIT:
        return UI_ENTER;
        break;
    }

    /*If unknown return the same type*/
    return linkDirection;
}

/*
    Function: uiPath_IsConnected

    Description -
    Checks if a path is connected to another. If so it
    returns the direction in which it is. Otherwise if no
    connections are found, returns < 0.

    2 arguments:
    Ui_Path *basePath - The path that will be checked for any links to it.
    Ui_Path *connectedPath - The path to check if it contains any links to basePath.
*/
int uiPath_IsConnected(Ui_Path *basePath, Ui_Path *connectedPath)
{
    int x = 0;

    for(x = 0; x < UI_DIRECTIONS; x++)
    {
        if(connectedPath->direction[x] == basePath)
        {
            return x;
        }
    }

    return -1;
}

/*
    Function: uiPath_IsValid

    Description -
    Checks if a path is valid, meaning that it can be selected by the map.

    1 argument:
    Ui_Path *path - The path that will be checked if it is valid.
*/
int uiPath_IsValid(Ui_Path *path)
{
    if(path == NULL)
        return 0;

    if(path->spEntity->draw == 0)
    {
        return 0;
    }

    return 1;
}

/*
    Function: uiPath_Link

    Description -
    Links two paths together, if linkBoth == 0 then
    only the link from pathA to pathB will be made, otherwise
    the link from pathB to pathA will also be mde.

    Returns 0 if all linked successfully, -1 on error, > 0 if
    the link overrided another.

    4 arguments:
    int linkDirection - The direction to link pathA to pathB.
    int linkBoth - if > 0, also link pathB to pathA in the opposite direction.
    Ui_Path *pathA - first path to link.
    Ui_Path *pathB - second path to link.
*/
int uiPath_Link(int linkDirection, int linkBoth, Ui_Path *pathA, Ui_Path *pathB)
{
    int overrided = 0;

    if(linkDirection < UI_DIRECTIONS)
    {
        if(pathA->direction[linkDirection] != NULL)
        {
            //printf("Warning path link already used for direction %d\n", linkDirection);
            overrided ++;
        }

        pathA->direction[linkDirection] = pathB;

        if(linkBoth > 0)
        {
            /*Call this function again with the reverse link and paths*/
            overrided += uiPath_Link(uiPath_ReverseLink(linkDirection), 0, pathB, pathA);
        }
    }
    else
    {
        printf("Unknown linkDirection %d\n", linkDirection);
        return -1;
    }

    return overrided;
}

int uiPath_FillLink(Ui_Path *path)
{
    int x = 0;

    for(x = 0; x < UI_BASEDIRECTIONS; x++)
    {
        if(path->direction[x] == NULL)
        {
            switch(x)
            {
                case UI_UP:
                if(path->direction[UI_UPLEFT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_UPLEFT]);
                }
                else if(path->direction[UI_UPRIGHT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_UPRIGHT]);
                }
                break;

                case UI_UPLEFT:
                if(path->direction[UI_UP] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_UP]);
                }
                else if(path->direction[UI_LEFT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_LEFT]);
                }
                break;

                case UI_LEFT:
                if(path->direction[UI_UPLEFT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_UPLEFT]);
                }
                else if(path->direction[UI_DOWNLEFT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_DOWNLEFT]);
                }
                break;

                case UI_DOWNLEFT:
                if(path->direction[UI_LEFT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_LEFT]);
                }
                else if(path->direction[UI_DOWN] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_DOWN]);
                }
                break;

                case UI_DOWN:
                if(path->direction[UI_DOWNLEFT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_DOWNLEFT]);
                }
                else if(path->direction[UI_DOWNRIGHT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_DOWNRIGHT]);
                }
                break;

                case UI_DOWNRIGHT:
                if(path->direction[UI_DOWN] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_DOWN]);
                }
                else if(path->direction[UI_RIGHT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_RIGHT]);
                }
                break;

                case UI_RIGHT:
                if(path->direction[UI_DOWNRIGHT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_DOWNRIGHT]);
                }
                else if(path->direction[UI_UPRIGHT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_UPRIGHT]);
                }
                break;

                case UI_UPRIGHT:
                if(path->direction[UI_UP] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_UP]);
                }
                else if(path->direction[UI_RIGHT] != NULL)
                {
                    uiPath_Link(x, 0, path, path->direction[UI_RIGHT]);
                }
                break;
            }
        }
    }

    return 0;
}

/*
    Function: uiPath_UnLink

    Description -
    Unlink the path going to a certain direction, or all directions if
    linkDirection < 0.

    2 arguments:
    Ui_Path *path - The path to unlink a direction from.
    int linkDirection - The direction to unlink. (< 0 if all directions)
*/
int uiPath_Unlink(Ui_Path *path, int linkDirection)
{
    int x = 0;

    if(linkDirection < 0)
    {
        for(x = 0; x < UI_DIRECTIONS; x++)
        {
            path->direction[x] = NULL;
        }
    }
    else
    {
        path->direction[linkDirection] = NULL;
    }

    return 0;
}

/*
    Function: uiPath_Select

    Description -
    Set the path entity to its hover state if state == 1.
    Otherwise turn of the enititys hovers state.

    2 arguments:
    Ui_Path *path - The path to select.
    int state - If 1 then select, if 0 then deselect.
*/
int uiPath_Select(Ui_Path *path, int state)
{
    path->selected = state;

    switch(path->type)
    {
        default:
        printf("Unknown path type %d to select\n", path->type);
        break;

        case UIE_BUTTON:
        uiPath_SelectButton(path->entity, state);
        break;

        case UIE_SCROLL:
        uiPath_SelectScroll(path->entity, state);
        break;

        case UIE_TEXTINPUT:
        uiPath_SelectButton(&path->typeEntity.textInput->button, state);
        break;
    }

    return 0;
}

int uiPath_SelectButton(Ui_Button *button, int state)
{
    /*Just force the button to its hover state*/
    if(state > 0)
        uiButton_ForceHover(button, 1);
    else
        uiButton_ForceHover(button, 0);

    return 0;
}

int uiPath_SelectScroll(Ui_ButtonScroll *scroll, int state)
{
    /*force the scroll button to its hover state*/
    if(state == 1)
        uiButton_ForceHover(&scroll->scrollButton, 1);
    else
        uiButton_ForceHover(&scroll->scrollButton, 0);

    /*reset any buttons in the scroll from being selected*/
    uiButtonScroll_ForceSelect(scroll, -1);

    return 0;
}

/*
    Function: uiMap_Setup

    Description -
    Setup the default values for the ui map.

    2 arguments:
    Ui_Map *map - The ui map to give the default values to.
    Timer *srcTimer - Source timer for the map to be based on.
*/
int uiMap_Setup(Ui_Map *map, Timer *srcTimer)
{
    int x = 0;
    const char *controlName;

    /*Setup the control events*/
    for(x = 0; x < UI_DIRECTIONS; x++)
    {
        switch(x)
        {
            default: break;
            case UI_LEFT: controlName = "Left"; break;
            case UI_RIGHT: controlName = "Right"; break;
            case UI_UP: controlName = "Up"; break;
            case UI_DOWN: controlName = "Down"; break;
            case UI_UPLEFT: controlName = "UpLeft"; break;
            case UI_UPRIGHT: controlName = "UpRight"; break;
            case UI_DOWNLEFT: controlName = "DownLeft"; break;
            case UI_DOWNRIGHT: controlName = "DownRight"; break;
            case UI_ENTER: controlName = "Enter"; break;
            case UI_EXIT: controlName = "Exit"; break;
        }

        control_Setup(&map->cMove[x], controlName, srcTimer, 0);
    }

    map->moveTimer = timer_Setup(srcTimer, 0, 200, 1);

    uiMap_Init(map);

    return 0;
}

void uiMap_Init(Ui_Map *map)
{
    map->paths = NULL;
    map->currentPath = NULL;
    map->defaultPath = NULL;
    map->directionMoved = -1;
    map->state = UI_MAPSTATE_OFF;

    return;
}


void uiMap_SetControl(Ui_Map *map, int direction, Control_Event *copyEvent)
{
    if(direction < UI_DIRECTIONS && direction >= 0)
    {
        control_CopyKey(&map->cMove[direction], copyEvent);
    }
    else
    {
        printf("Error unknown direction to set control in ui map %d\n", direction);
    }

    return;
}

int uiMap_PushPath(Ui_Map *map, Ui_Path *path)
{
    list_Push(&map->paths, path, path->type);

    switch(path->type)
    {
        case UIE_BUTTON:

        break;

        case UIE_SCROLL:
        uiMap_PushButtonScroll(map, path);
        break;

        case UIE_TEXTINPUT:

        break;
    }

    return 0;
}

int uiMap_PushButtonScroll(Ui_Map *map, Ui_Path *path)
{
    uiPath_Link(UI_ENTER, 1, path, path);

    return 0;
}

Ui_Path *uiMap_AddSinglePath(Ui_Map *map, Ui_Path *owner, int layer, Ui_Spine_Entity *spEntity)
{
    return uiMap_AddPath(map, owner, layer, spEntity);
}

Ui_Path *uiMap_AddPath(Ui_Map *map, Ui_Path *owner, int layer, Ui_Spine_Entity *spEntity)
{
    Ui_Path *pathA = uiMap_GetPath(map, layer, spEntity);

    if(pathA == NULL)
    {
        pathA = uiPath_Create(spEntity, layer, owner);
        uiMap_PushPath(map, pathA);
    }
    else
    {
        printf("Warning %s already inserted into map at layer %d\n", spEntity->name, layer);
    }

    return pathA;
}

int uiMap_AddPath2(Ui_Map *map, void *spine, Ui_Path *owner, int layer, char *entityName)
{
    Ui_Spine_Entity *spineEntityA = uiSpine_GetEntityBase(spine, entityName);

    uiMap_AddPath(map, owner, layer, spineEntityA);

    return 0;
}

int uiMap_FillPathLinks(Ui_Map *map)
{
    struct list *pathList = map->paths;

    while(pathList != NULL)
    {
        uiPath_FillLink(pathList->data);

        pathList = pathList->next;
    }

    return 0;
}

Ui_Path *uiMap_GetPath(Ui_Map *map, int layer, Ui_Spine_Entity *spEntity)
{
    struct list *pathList = map->paths;
    Ui_Path *path = NULL;

    while(pathList != NULL)
    {
        path = pathList->data;
        if(path->layer == layer || layer == -1)
        {
            if(path->spEntity == spEntity && path->remove == 0)
            {
                return path;
            }
        }

        pathList = pathList->next;
    }

    //printf("Warning: Could not find path for entity %s\n", spEntity->name);
    return NULL;
}

Ui_Path *uiMap_GetConnected(Ui_Map *map, Ui_Path *path)
{
    struct list *pathList = map->paths;
    Ui_Path *connectedPath = NULL;

    while(pathList != NULL)
    {
        if(path != pathList->data && uiPath_IsConnected(path, pathList->data) >= 0 && path->remove == 0)
        {
            return pathList->data;
        }

        pathList = pathList->next;
    }

    return NULL;
}

Ui_Path *uiMap_GetOwned(Ui_Map *map, Ui_Path *path)
{
    struct list *pathList = map->paths;
    Ui_Path *checkPath = NULL;

    while(pathList != NULL)
    {
        checkPath = pathList->data;

        if(path != checkPath && checkPath->remove == 0 && path == checkPath->owner)
        {
            return checkPath;
        }

        pathList = pathList->next;
    }

    return NULL;
}

int uiMap_RemovePath(Ui_Map *map, Ui_Path *path)
{
    Ui_Path *cPath = NULL;
    int cDirection = -1;
    int x = 0;

    /*Removes any links to this path in the map*/
    while((cPath = uiMap_GetConnected(map, path)) != NULL)
    {
        while((cDirection = uiPath_IsConnected(path, cPath)) >= 0)
        {
            cPath->direction[cDirection] = NULL;
        }
    }

    /*If this path is an owner to another, remove those paths it owns*/
    while((cPath = uiMap_GetOwned(map, path)) != NULL)
    {
        uiPath_Disable(cPath);
    }

    /*Check if the path to remove is the one that is currently active*/
    if(path == map->currentPath)
    {
       map->currentPath = NULL;

       for(x = 0; x < UI_DIRECTIONS; x++)
       {
           if(path->direction[x] != NULL)
           {
               map->currentPath = path->direction[x];
               break;
           }
       }
    }

    /*Remove the list watcher link to this path*/
    uiPath_Clean(path);

    /*Finally remove the path node from the maps list and delete the path*/
    list_Delete_NodeFromData(&map->paths, path);

    mem_Free(path);

    return 0;
}

void uiMap_Clean(Ui_Map *map)
{
    int x = 0;

    while(map->paths != NULL)
    {
        uiPath_Clean(map->paths->data);
        mem_Free(map->paths->data);

        list_Pop(&map->paths);
    }

    map->currentPath = NULL;

    for(x = 0; x < UI_DIRECTIONS; x++)
    {
        control_Clean(&map->cMove[x]);
    }

    return;
}

void uiMap_Reset(Ui_Map *map)
{
    while(map->paths != NULL)
    {
        uiPath_Clean(map->paths->data);
        mem_Free(map->paths->data);

        list_Pop(&map->paths);
    }

    uiMap_Init(map);

    return;
}

void uiMap_SetDefaultPath(Ui_Map *map, Ui_Spine_Entity *spEntity)
{
    map->defaultPath = uiMap_GetPath(map, 0, spEntity);
    return;
}

void uiMap_CreateGeometricLinks(Ui_Map *map, int layer)
{
    struct list *pathList = map->paths;
    struct list *pathCompareList = map->paths;
    Ui_Path *p = NULL;
    Ui_Path *pCompare = NULL;

    Vector2DInt aPos = {0, 0};
    Vector2DInt bPos = {0, 0};
    Path_Entity type;

    int xDis = 0;
    int yDis = 0;

    int direction = 0;
    int disSqr = 0;
    float tan = 0.0f;
    int closestDis[UI_BASEDIRECTIONS];
    Ui_Path *closestPath[UI_BASEDIRECTIONS];

    int x = 0;

    /*Go through all the paths in the map*/
    while(pathList != NULL)
    {
        p = pathList->data;

        if(p->layer == layer)
        {
            aPos = uiSpineEntity_GetPosBase(p->spEntity);

            for(x = 0; x < UI_BASEDIRECTIONS; x++)
            {
                closestDis[x] = -1;
                closestPath[x] = NULL;
            }

            /*Compare all the other paths in the map*/
            pathCompareList = map->paths;
            while(pathCompareList != NULL)
            {
                pCompare = pathCompareList->data;

                if(pCompare != p && pCompare->layer == p->layer)
                {
                    bPos = uiSpineEntity_GetPosBase(pCompare->spEntity);

                    xDis = bPos.x - aPos.x;
                    yDis = -(bPos.y - aPos.y);
                    disSqr = mth_DistanceSqr(xDis, yDis);

                    if(xDis == 0)
                        tan = (float)yDis/0.00001f;
                    else
                        tan = (float)yDis/xDis;

                    direction = uiPath_GetDirection(tan, (float)xDis, (float)yDis);

                    //printf("Checking %s[%d, %d] against %s[%d, %d]:\nxDis %d, yDis %d, disSqr %d, direction %d\n", p->spEntity->name, aPos.x, aPos.y, pCompare->spEntity->name, bPos.x, bPos.y, xDis, yDis, disSqr, direction);

                    if(closestPath[direction] == NULL || closestDis[direction] == -1 || closestDis[direction] > disSqr)
                    {
                        //printf("Pushing %s in as closest to direction: %d\n", pCompare->spEntity->name, direction);
                        closestDis[direction] = disSqr;
                        closestPath[direction] = pCompare;
                    }
                }

                pathCompareList = pathCompareList->next;
            }

            for(x = 0; x < UI_BASEDIRECTIONS; x++)
            {
                if(closestPath[x] != NULL)
                {
                    uiPath_Link(x, 0, p, closestPath[x]);
                }
            }
        }

        pathList = pathList->next;
    }

    return;
}

/*
    Function: uiMap_Select

    Description -
    First checks if the map has a current path. If it dosnt it tries
    to assign the default path as the current path.

    It then sets the current path to be selected if
    state == SELECT_ON. Otherwise if state == SELECT_OFF
    it makes sure the current path is not selected.

    2 arguments:
    Ui_Map *map - The ui map to select the current path with.
    int state - if SELECT_ON, the current path is selected, otherwise it is unselected.
*/
int uiMap_Select(Ui_Map *map, int state)
{
    if(map->currentPath == NULL && map->defaultPath != NULL)
    {
        map->currentPath = map->defaultPath;
    }

    if(map->currentPath != NULL)
    {
       uiPath_Select(map->currentPath, state);
    }

    return 0;
}

int uiMap_ForceUnselect(Ui_Map *map)
{
    struct list *pathList = map->paths;

    while(pathList != NULL)
    {
        if(pathList->data != map->currentPath)
        {
            uiPath_Select(pathList->data, -1);
        }
        pathList = pathList->next;
    }

    return 0;
}

int uiMap_MovePathScroll(Ui_Map *map, int direction)
{
    Ui_ButtonScroll *scroll = map->currentPath->typeEntity.scroll;
    Ui_Button *button = NULL;
    int moved = 0;

    /*If the scroll is currently selected and the enter control is pressed,
    select the first button shown in the scroll, also change the selected state to show this*/
    if(direction == UI_ENTER)
    {
        if(map->currentPath->selected == 1)
        {
            uiMap_Select(map, 2);

            uiButtonScroll_ForceSelect(scroll, 0);
            button = scroll->currentForced;

            if(button != NULL)
            {
                /*Make sure the button does not activate incase the control to do that is the same
                as the enter control*/
                control_Stop(&button->cActivate[BC_KEY], 250);
            }

            moved = 1;
        }
    }
    else if(direction == UI_EXIT)
    {
        /*If the exit control is pressed and the state of the scroll is selecting the buttons*/
        if(map->currentPath->selected == 2)
        {
            /*Reset the state of the scroll to just be selected and not the buttons*/
            uiMap_Select(map, 1);

            moved = 1;
        }
    }
    else if(map->currentPath->selected == 2)
    {
        //printf("Moving to %d, selected %d, activate %d\n", direction, map->currentPath->selected, scroll->activateOnTraverse);

        if(scroll->forceHover == -1)
             uiButtonScroll_ForceSelect(scroll, 0);
        /*If the scroll is in button select mode then use the direction controls to change the scrolling position*/
        moved = uiButtonScroll_MoveSelected(scroll, direction);
    }
    else
    {
        /*If the scroll is selected and the standard directions have been pressed then treat it as a simple button in the map*/
        moved = uiMap_MovePathBase(map, direction);
    }

    return moved;
}

int uiMap_MovePathBase(Ui_Map *map, int direction)
{
    int x = 0;
    Ui_Path *startPath = map->currentPath;

    if(map->currentPath == NULL) /*Try and find the default path to move to, otherwise the head of the path list*/
    {
        if(map->paths != NULL)
        {
            if(map->defaultPath == NULL)
            {
                map->currentPath = map->paths->data;
            }
            else
            {
                map->currentPath = map->defaultPath;
            }

            printf("Getting first path %s\n", map->currentPath->spEntity->name);

            return 1;
        }
    }
    else if(direction < 0) /*Find a new adjacent path to move to*/
    {
       for(x = 0; x < UI_DIRECTIONS; x++)
       {
           if(uiPath_IsValid(map->currentPath->direction[x]) == 1)
           {
               map->currentPath = map->currentPath->direction[x];
           }
       }

       if(x == UI_DIRECTIONS && map->paths != NULL)
       {
           map->currentPath = map->defaultPath;
       }
       else
       {
           map->currentPath = NULL;
       }
    }
    else if(map->currentPath->direction[direction] != NULL) /*Standard condition, move in the direction as specified*/
    {
       /*Traverse over any paths that are disabled*/
       do
       {
           map->currentPath = map->currentPath->direction[direction];
       }
       while(map->currentPath->direction[direction] != NULL && uiPath_IsValid(map->currentPath) == 0);
    }

    if(map->currentPath == startPath || map->currentPath == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}



/*
    Function: uiMap_MovePath

    Description -
    Moves the current path of the map to a new path in a certain direction. If
    this direction cannot be done then just returns.

    If the direction given is < 0 then the function will attempt to find a path
    adjacent to the current one. If it still cant find one it will set the path
    to the first path entered into the map. Otherwise it will set the current path to NULL.

    Returns whether the path has been moved (1) or not (0).

    2 arguments:
    Ui_Map *map - The ui map to move the selected path.
    int direction - The direction to move the selected path.
*/
int uiMap_MovePath(Ui_Map *map, int direction)
{
    int x = 0;
    int moved = 0;
    Ui_Path *tempPath = map->currentPath;

    if(map->currentPath == NULL)
    {
        moved = uiMap_MovePathBase(map, direction);
    }
    else
    {
        /*Moving logic is dependant on the type of path currently selected*/
        switch(map->currentPath->type)
        {
            default:
            case UIE_BUTTON:
            case UIE_TEXTINPUT:
            moved = uiMap_MovePathBase(map, direction);
            break;

            case UIE_SCROLL:
            moved = uiMap_MovePathScroll(map, direction);
            break;
        }
    }

    if(moved == 1)
    {
        if(tempPath != map->currentPath)
        {
            if(tempPath != NULL)
                uiPath_Select(tempPath, 0);

            uiMap_Select(map, 1);
        }

        map->directionMoved = direction;
    }

    if(map->currentPath != NULL)
    {
        //printf("At path with name %s, selected %d, layer %d, owner %p\n", map->currentPath->spEntity->name, map->currentPath->selected, map->currentPath->layer, map->currentPath->owner);
        for(x = 0; x < UI_DIRECTIONS; x++)
        {
            //printf("Path [%d] = %p\n", x, map->currentPath->direction[x]);
        }
    }

    return moved;
}

int uiMap_CheckValidity(Ui_Map *map)
{
    struct list *pathList = map->paths;
    struct list *temp = NULL;
    Ui_Path *path = NULL;
    int numRemoved = 0;

    while(pathList != NULL)
    {
        temp = pathList->next;
        path = pathList->data;

        /*If the path has entity that has been destroyed or has requested to be removed*/
        if(path->entity == NULL || path->remove > 0)
        {
            /*Remove it from the map*/
            uiMap_RemovePath(map, path);
            numRemoved ++;
        }

        pathList = temp;
    }

    return numRemoved;
}

int uiMap_ResetMoveTime(Ui_Map *map)
{
    timer_Start(&map->moveTimer);

    return 0;
}

int uiMap_UpdatePath(Ui_Map *map)
{
    int x = 0;

    /*Exit early if there is no current path*/
    if(map->currentPath == NULL)
    {
        return 0;
    }

    return 0;
}

int uiMap_Update(Ui_Map *map, Ui_Pointer *pnt)
{
    int x = 0;
    int moved = 0;

    timer_Calc(&map->moveTimer);

    if(timer_Get_Remain(&map->moveTimer) <= 0)
    {
        /*Check if any of the move keys are activated*/
        for(x = 0; x < UI_DIRECTIONS; x++)
        {
            if(control_IsActivated(&map->cMove[x]) > 0)
            {
                //printf("[%p] Activate on [%d]\n", map, x);
                switch(map->state)
                {
                    default:
                    case UI_MAPSTATE_OFF:
                    map->state = UI_MAPSTATE_ON;
                    timer_Start(&map->moveTimer);
                    break;

                    case UI_MAPSTATE_ON:

                    moved = uiMap_MovePath(map, x);

                    if(moved != 0)
                    {
                        //printf("Moved = %d\n", moved);
                        //uiMap_UpdatePath(map);
                        timer_Start(&map->moveTimer);

                        if(map->currentPath != NULL)
                        {
                            //uiMap_ForceUnselect(map);
                        }

                        /*Disable the pointer if the current path has been moved*/
                        if(pnt != NULL)
                        {
                            //printf("Disabling pointer due to move in direction %d\n", x);
                            uiPointer_Disable(pnt);
                        }
                    }

                    break;
                }

                break;
            }
        }
    }

    /*done in the ui spine update function
    if(pnt != NULL && uiPointer_IsEnabled(pnt) == 1 && uiPointer_HasMoved(pnt) == 1)
    {
        uiMap_Select(map, 0);
    }*/


    if(uiMap_CheckValidity(map) > 0)
    {
        //printf("Removed paths from map\n");
    }


    return moved;
}
