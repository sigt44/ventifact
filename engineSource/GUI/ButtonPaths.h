#ifndef BUTTONPATHS_H
#define BUTTONPATHS_H

#include "SpineEntity.h"
#include "Button.h"
#include "ButtonScroll.h"
#include "TextInput.h"
#include "../Controls.h"
#include "../Common/List.h"
#include "../Common/ListWatcher.h"
#include "UI_Defines.h"

enum
{
    UI_MAPSTATE_OFF = 0,
    UI_MAPSTATE_ENTER = 1,
    UI_MAPSTATE_ON = 2,
} UI_MAPSTATES;

enum
{
    SELECT_OFF = 0,
    SELECT_ON = 1
} UI_PATH_SELECT;

/*Supported entity types*/
typedef union
{
    Ui_Button *button;
    Ui_ButtonScroll *scroll;
    Ui_TextInput *textInput;

}   Path_Entity;

typedef struct Ui_Path
{
    /*Type of entity (UI_ENTITY_TYPES)*/
    int type;

    /*The link layer of the path*/
    int layer;

    /*Pointer to the spine entity*/
    struct ui_Spine_Entity *spEntity;

    /*Pointer to the ui entity*/
    void *entity;

    /*Holds the proper typecasted pointer to the entity*/
    Path_Entity typeEntity;

    /*The path to go to when a directional control is pressed*/
    struct Ui_Path *direction[UI_DIRECTIONS];

    /*If the owner path is removed, so should this path*/
    struct Ui_Path *owner;

    int selected;

    /*If 1 then the path should be removed from the map*/
    int remove;

} Ui_Path;

typedef struct Ui_Map
{
    /*Controls to move to a different ui element in a certain direction*/
    Control_Event cMove[UI_DIRECTIONS];

    /*Direction of the last move*/
    int directionMoved;

    /*Timer to control the waiting time before another move can be made*/
    Timer moveTimer;

    /*list of entities that are connected together*/
    struct list *paths;

    /*The current path that is being activated*/
    Ui_Path *currentPath;

    /*Path that is chosen if currentPath is NULL*/
    Ui_Path *defaultPath;

    /*The current state of the map*/
    int state;

} Ui_Map;

Ui_Path *uiPath_Create(struct ui_Spine_Entity *spEntity, int layer, Ui_Path *owner);

int uiPath_Clean(Ui_Path *path);

void uiPath_Disable(Ui_Path *path);

int uiPath_GetDirection(float tan, float xDis, float yDis);

int uiPath_SupportedType(void *entity, int type);

int uiPath_ReverseLink(int linkDirection);

int uiPath_IsConnected(Ui_Path *basePath, Ui_Path *connectedPath);

int uiPath_IsValid(Ui_Path *path);

int uiPath_Link(int linkDirection, int linkBoth, Ui_Path *pathA, Ui_Path *pathB);

int uiPath_FillLink(Ui_Path *path);

int uiPath_Unlink(Ui_Path *path, int linkDirection);

int uiPath_Select(Ui_Path *path, int state);

int uiPath_SelectButton(Ui_Button *button, int state);

int uiPath_SelectScroll(Ui_ButtonScroll *scroll, int state);




int uiMap_Setup(Ui_Map *map, Timer *srcTimer);

void uiMap_Init(Ui_Map *map);

void uiMap_SetControl(Ui_Map *map, int direction, Control_Event *copyEvent);

void uiMap_SetDefaultPath(Ui_Map *map, struct ui_Spine_Entity *spEntity);

int uiMap_PushPath(Ui_Map *map, Ui_Path *path);

int uiMap_PushButtonScroll(Ui_Map *map, Ui_Path *path);

void uiMap_CreateGeometricLinks(Ui_Map *map, int layer);

Ui_Path *uiMap_AddSinglePath(Ui_Map *map, Ui_Path *owner, int layer, struct ui_Spine_Entity *spEntity);

Ui_Path *uiMap_AddPath(Ui_Map *map, Ui_Path *owner, int layer, struct ui_Spine_Entity *spEntity);

int uiMap_AddPath2(Ui_Map *map, void *spine, Ui_Path *owner, int layer, char *entityName);

int uiMap_FillPathLinks(Ui_Map *map);

Ui_Path *uiMap_GetPath(Ui_Map *map, int layer, struct ui_Spine_Entity *spEntity);

Ui_Path *uiMap_GetConnected(Ui_Map *map, Ui_Path *path);

Ui_Path *uiMap_GetOwned(Ui_Map *map, Ui_Path *path);

int uiMap_RemovePath(Ui_Map *map, Ui_Path *path);

void uiMap_Clean(Ui_Map *map);

void uiMap_Reset(Ui_Map *map);

int uiMap_Select(Ui_Map *map, int state);

int uiMap_ForceUnselect(Ui_Map *map);

int uiMap_ResetMoveTime(Ui_Map *map);

int uiMap_MovePathScroll(Ui_Map *map, int direction);

int uiMap_MovePathBase(Ui_Map *map, int direction);

int uiMap_MovePath(Ui_Map *map, int direction);

int uiMap_Update(Ui_Map *map, Ui_Pointer *pnt);

#endif
