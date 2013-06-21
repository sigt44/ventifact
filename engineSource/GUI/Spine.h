#ifndef SPINE_H
#define SPINE_H

#include "../Common/List.h"
#include "../Common/ListWatcher.h"
#include "../Graphics/Animation.h"
#include "../Graphics/Sprite.h"
#include "../Time/Timer.h"
#include "Pointer.h"

#include "Button.h"
#include "ButtonChain.h"
#include "ButtonPaths.h"
#include "ButtonScroll.h"
#include "TextBox.h"
#include "TextInput.h"
#include "UI_Defines.h"

enum
{
    UI_SPINE_DISABLED = 0,
    UI_SPINE_DRAW = 1,
    UI_SPINE_DRAWSPINES = 2,
    UI_SPINE_UPDATE = 4,
    UI_SPINE_UPDATESPINES = 8,
    UI_SPINE_UPDATEPOINTER = 16,
    UI_SPINE_UPDATECHOOSER = 32,
    UI_SPINE_ALLFLAGS = UI_SPINE_DRAW | UI_SPINE_DRAWSPINES | UI_SPINE_UPDATE | UI_SPINE_UPDATESPINES | UI_SPINE_UPDATEPOINTER | UI_SPINE_UPDATECHOOSER

} UI_SPINE_FLAGS;

enum
{
    SPINE_STATE_IDLE = 0,
    SPINE_STATE_SPINE = 1,
    SPINE_STATE_CHOOSER = 2,
    SPINE_STATE_BUSY = 3
} UI_SPINE_STATES;

enum
{
    UIS_ENTITY_DELETE = -1,
    UIS_ENTITY_NO_UPDATE = 0,
    UIS_ENTITY_UPDATE = 1
} UI_ENTITY_STATES;

typedef struct ui_Spine_TemplateEntity
{
    char *entityName;
    int updateState;
    int drawState;
} Ui_Spine_TemplateEntity;

typedef struct ui_Spine_Template
{
    int ID;
    char *name;

    unsigned int flags; /*flags to give the spine*/

    int numEntity;

    Ui_Spine_TemplateEntity **entityArray;

    Ui_Map map;

}   Ui_Spine_Template;

typedef struct ui_Spine
{
    Ui_Rect rect;

    Camera_2D *camera;

    struct list *entityList; /*Contains a list of all the ui entities (Ui_Spine_Entity) in the spine*/

    struct list *baseList[UIE_TOTALTYPES]; /*List of all the actual objects in the spine*/

    struct list *templateList; /*Contains a list of all the templates of the spine*/
    Ui_Spine_Template *currentTemplate;

    unsigned int pausedFlags;
    unsigned int startingFlags;
    unsigned int flags;

    int paused;

    int layer;

    int state;

    Ui_Pointer *pnt;
    int pntOwner; /*If true then the spine owns the pointer, so it must release it from memory once finsihed*/

    //Ui_ButtonChooser buttonChooser;
    Ui_Map map;

    Sprite background;

    Timer sTimer;

    Data_Struct content; /*Data content that the spine can use in custom functions*/
    void (*onUpdate)(struct ui_Spine *spine);
    void (*onDelete)(struct ui_Spine *spine);

    int spinesOpened; /*Total number of spines open from this one*/
    struct ui_Spine *openedFrom; /*The spine that this one was opened from*/
    struct ui_Spine *owner; /*Spine that owns this one*/

} Ui_Spine;

Ui_Spine_Entity *uiSpine_GetEntityBase(Ui_Spine *spine, const char *entityName);

Ui_Spine_Entity *uiSpine_GetEntityBase2(Ui_Spine *spine, void *entity);

void *uiSpine_GetEntity(Ui_Spine *spine, const char *entityName);

void *uiSpine_RemoveEntity(Ui_Spine *spine, const char *entityName);

void uiSpine_DeleteEntity(Ui_Spine *spine, Ui_Spine_Entity *entityDesc);

Ui_Spine_Entity *uiSpine_SetEntityPos(Ui_Spine *spine, const char *entityName, int x, int y);

Vector2DInt uiSpine_GetEntityPos(Ui_Spine *spine, const char *entityName);

void uiSpine_AlignEntities(Ui_Spine *spine, int xStart, int yStart, int padding, int direction, int numEntity, ...);

void uiSpine_ReportEntity(Ui_Spine *spine, int recursive);

void uiSpine_MapEntity(Ui_Spine *spine);

void uiSpine_SetEntityUpdate(Ui_Spine *spine, const char *entityName, int logicUpdate, int drawUpdate);
void uiSpine_SetEntityUpdate2(Ui_Spine *spine, void *entity, int logicUpdate, int drawUpdate);

void uiSpine_SetEntityDelete(Ui_Spine *spine, const char *entityName);
void uiSpine_SetEntityDelete2(Ui_Spine *spine, void *entity);

void uiSpine_SetUpdateAllEntity(Ui_Spine *spine, int logicUpdate, int drawUpdate);

void uiSpine_AddEntity(Ui_Spine *spine, void *entity, int attach, int autoFree, const char *entityName);

void uiSpine_AddButton(Ui_Spine *spine, Ui_Button *button, const char *name);
void uiSpine_AddSprite(Ui_Spine *spine, Sprite *sprite, int autoFree, const char *name);
void uiSpine_AddScroll(Ui_Spine *spine, Ui_ButtonScroll *scroll, const char *name);
void uiSpine_AddSpine(Ui_Spine *spine, Ui_Spine *addSpine, int attached, const char *name);
void uiSpine_AddTextBox(Ui_Spine *spine, Ui_TextBox *textBox, const char *name);
void uiSpine_AddTextInput(Ui_Spine *spine, Ui_TextInput *textInput, const char *name);

Ui_Spine *uiSpine_Create(int layer, int x, int y, Frame *background, Ui_Pointer *pnt, int pntOwner, Timer *src_Timer, unsigned int flags);

void uiSpine_Setup(Ui_Spine *spine, int layer, int x, int y, Frame *background, Ui_Pointer *pnt, int pntOwner, Timer *src_Timer, unsigned int flags);

void uiSpine_SetupCamera(Ui_Spine *spine, Camera_2D *camera, unsigned int width, unsigned int height, int maxWidth, int maxHeight);

void uiSpine_SetPos(Ui_Spine *spine, int x, int y);

void uiSpine_ChangePos(Ui_Spine *spine, int xDis, int yDis);

Vector2DInt uiSpine_GetPos(Ui_Spine *spine);
Ui_Rect *uiSpine_GetRect(Ui_Spine *spine);

void uiSpine_SetOwner(Ui_Spine *spine, Ui_Spine *owner);

void uiSpine_Draw(Ui_Spine *spine, SDL_Surface *destination);

void uiSpine_Update(Ui_Spine *spine, float deltaTime);

void uiSpine_Clean(Ui_Spine *spine);

void uiSpine_SetFlags(Ui_Spine *spine, unsigned int flags);
void uiSpine_Enable(Ui_Spine *spine);
void uiSpine_Disable(Ui_Spine *spine);
void uiSpine_DisableBase(Ui_Spine *spine);

void uiSpine_Open(Ui_Spine *spine, Ui_Spine *openedFrom);
void uiSpine_Close(Ui_Spine *spine);
void uiSpine_Pause(Ui_Spine *spine);
void uiSpine_Resume(Ui_Spine *spine);

Ui_Spine_TemplateEntity *uiSpine_CreateTemplateEntity(char *entityName, int updateState, int drawState);
void uiSpine_CreateTemplate(Ui_Spine *spine, int ID, char *templateName, unsigned int flags, int numEntity, ...);
void uiSpine_CleanTemplate(Ui_Spine_Template *sTemplate);
Ui_Spine_Template *uiSpine_GetTemplate(Ui_Spine *spine, char *templateName);
Ui_Spine_Template *uiSpine_GetTemplate2(Ui_Spine *spine, int templateID);
void uiSpine_OpenTemplate(Ui_Spine *spine, char *templateName, int selectState);
void uiSpine_OpenTemplate2(Ui_Spine *spine, int templateID, int selectState);


#endif
