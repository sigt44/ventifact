#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "GUI/Spine.h"
#include "Time/Timer.h"
#include "Controls.h"
#include "Camera.h"

#include "../Level/Level.h"
#include "../Level/Tiles.h"
#include "../Units/Supply.h"

#define VLE_NUM_SHORTCUTS 5

enum VLEVEL_EDITOR_STATES
{
    VLE_STATE_BASE = 0, /*At the base menu*/
    VLE_STATE_PLACE = 1, /*Placing an object*/
    VLE_STATE_REPLACE = 2, /*Replacing an object*/
    VLE_STATE_REMOVE = 3, /*Removing an object*/
    VLE_STATE_COPY = 4, /*Copy an object*/
    VLE_NUMSTATES = 5,
};

enum VLEVEL_OBJECTTYPES
{
    VLE_OBJECT_TILE = 0,
    VLE_OBJECT_UNIT = 1,
    VLE_OBJECT_NODE = 2,
    VLE_OBJECT_PLAYERSTART = 3,
    VLE_OBJECT_ENEMYSTART = 4,
    VLE_OBJECT_POSITION = 5,
    VLE_OBJECT_EDGE = 6,
    VLE_OBJECT_NUMTYPES = 7,
};

typedef struct Vent_Level_TileEditor
{
    struct list *graphicNames;
    struct list *loadedTiles; /*Loaded tiles that can be placed into the level*/
    Vent_Tile *customTile;    /*Tile that has custom attributes*/
    Vent_Tile *selectedTile; /*The tile that can be edited/placed*/
    Vent_Tile templateTile; /*Tile used to edit variables*/

    int chosenGraphicID; /*Used when selecting a new graphic for the tile*/
    int customGraphicID;    /*Used to choose a new graphic ID for the tile*/
    char *customGraphicName;
    int tileTemplateType;

} Vent_Level_TileEditor;

typedef struct Vent_Level_AttributesEditor
{
    Vent_Level_Header previousHeader;

    char *playerUnitName;

    struct list *levelNames; /*List of names of the level files to load*/
    char *selectedLevel; /*Points to the name of the selected level to load*/

} Vent_Level_AttributesEditor;

typedef struct Vent_Level_UnitEditor
{
    Vent_Supply templateSupply;

    Data_Struct *aiData[SUPPLY_AI_TOTAL]; /*Holds the current data for all types of unit ai*/

    struct list *patrolList; /*Holds the nodes for the patrol ai state*/


} Vent_Level_UnitEditor;

typedef struct Vent_Level_NodeEditor
{
    int showGlobalNodes;
    int linkPreviousNode;

    B_Graph *levelGraph;
    B_Node *selectedNodeA; /*Nodes that are selected to create an edge*/
    B_Node *selectedNodeB;

} Vent_Level_NodeEditor;

typedef struct Vent_Level_MiscEditor
{
    int useGrid;
    int gridWidth;
    int gridHeight;

    int useObjectGrid;
    int objGridWidth;
    int objGridHeight;

    int cameraSpeed;

} Vent_Level_MiscEditor;


typedef struct Vent_Level_Editor
{
    Base_State *bState; /*Points to the base state that contains the editor*/
    Vent_Level level; /*Current level being edited*/
    char *levelName;

    Ui_Spine spine; /*Spine to hold the menus*/

    Camera_2D camera; /*Camera used to move around the level*/
    int cameraSpeed;

    Vector2D pntPos; /*Position of the pointer*/
    Vector2DInt cPntPos; /*Position of the pointer and camera*/
    Vector2DInt tilePos; /*Start position of the line that will be filled with objects*/
    int tileObject;
    int gridObject;

    Vent_Sector *currentSector; /*Current sector the camera is located at*/

    Vent_Level_TileEditor tileEdit;
    Vent_Level_AttributesEditor levelEdit;
    Vent_Level_UnitEditor unitEdit;
    Vent_Level_NodeEditor nodeEdit;
    Vent_Level_MiscEditor miscEdit;

    char shortcutValues[VLE_NUM_SHORTCUTS][127]; /*Buffers to display shortcut keys*/

    Vent_Player testPlayer;

    B_Graph positionGraph;
    int maxPositions;

    int state; /*Main state of the menu*/

    int objectType; /*Type of object being edited in the level*/
    int *graphicID; /*Used by the graphic select menu to change the graphic ID of an object*/

    Control_Event cRotateState;

    Timer fpsTimer;
    float fps;
    float deltaTime;

} Vent_Level_Editor;

extern char *leTileTemplateNames[TILEG_TOTALGRAPHICTYPES];
extern char *leLayerNames[6];
extern char *leTeamNames[3];
extern char *leAiNames[7];

void vLE_SetupTiles(Vent_Level_Editor *editor);

Ui_Button *vLE_ButtonOption(char *name, Data_Struct *nameVars, int layer, void (*onActivate)(Ui_Button *button), Data_Struct *optionsVars);
Ui_Button *vLE_ButtonCheckBox(char *text, int layer, void (*onActivate)(Ui_Button *button), void (*onHover)(Ui_Button *button), Data_Struct *info);
Ui_Button *vLE_CloseButton(void (*onActivate)(Ui_Button *button), Data_Struct *optionVars);

void vLE_InitLevel(Vent_Level_Editor *editor);

void vLE_SaveLevel(Vent_Level_Editor *editor);

void vLE_SetObjectType(Vent_Level_Editor *editor, int objectType);
void vLE_SetState(Base_State *gState, int state);

void vLE_InitBase(Base_State *gState);
void vLE_InitPlace(Base_State *gState);
void vLE_InitReplace(Base_State *gState);
void vLE_InitRemove(Base_State *gState);
void vLE_InitCopy(Base_State *gState);

void vLE_InitLevelSectors(Vent_Level_Editor *editor);
void vLE_Init(void *info);

void vLE_Controls(void *info);
void vLE_ControlsPlace(void *info);
void vLE_ControlsReplace(void *info);
void vLE_ControlsRemove(void *info);
void vLE_ControlsCopy(void *info);

void vLE_Logic(void *info);
void vLE_LogicPlace(void *info);

void vLE_Render(void *info);
void vLE_RenderPlace(void *info);
void vLE_RenderRemove(void *info);

void vLE_Exit(void *info);

#endif
