#ifndef SPINEENTITY_H
#define SPINEENTITY_H

#include "Button.h"
#include "ButtonChain.h"
#include "ButtonPaths.h"
#include "ButtonScroll.h"
#include "TextBox.h"
#include "TextInput.h"
#include "UI_Defines.h"

typedef struct
{
    Ui_Button *button;
    Ui_ButtonScroll *scroll;
    Ui_TextInput *textInput;
    Ui_TextBox *textBox;
    Sprite *sprite;
    struct ui_Spine *spine;

}   Spine_EntityType;

typedef union
{
    Ui_Button *button;
    Ui_ButtonScroll *scroll;
    Ui_TextInput *textInput;
    Ui_TextBox *textBox;
    Sprite *sprite;
    struct ui_Spine *spine;

}   Spine_EntityUnionType;

typedef struct ui_Spine_Entity
{
    char *name;

    void *entity;
    Spine_EntityUnionType entityType;

    int type;

    int update;
    int draw;

    int attached;

    int free;

    struct ui_Spine *owner;

    Dependency_Watcher depWatcher;

} Ui_Spine_Entity;

Ui_Spine_Entity *uiSpineEntity_Create(void *entity, const char *name, struct ui_Spine *owner, struct list **watchList, int type, int attach, int free);

void uiSpineEntity_Clean(Ui_Spine_Entity *entityDesc);

void uiSpineEntity_SetDraw(Ui_Spine_Entity *entityDesc, int drawUpdate);
void uiSpineEntity_SetLogic(Ui_Spine_Entity *entityDesc, int logicUpdate);

Vector2DInt uiSpineEntity_GetPosBase(Ui_Spine_Entity *entityDesc);
Ui_Rect uiSpineEntity_GetRectBase(Ui_Spine_Entity *entityDesc);


#endif
