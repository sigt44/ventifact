#include "SpineEntity.h"

#include "Spine.h"

/*
    Function: uiSpineEntity_Create

    Description -
    Creates a returns a new entity description structure. Used so that
    an entity in a ui spine can be easily found just by typing its name.

    2 arguments:
    void *entity - The entity in the spine.
    const char *name - The name of the entity, used to find it later.
*/
Ui_Spine_Entity *uiSpineEntity_Create(void *entity, const char *name, struct ui_Spine *owner, struct list **watchList, int type, int attach, int free)
{
    Ui_Spine_Entity *entityDesc = (Ui_Spine_Entity *)mem_Malloc(sizeof(Ui_Spine_Entity), __LINE__, __FILE__);

    entityDesc->entity = entity;

    entityDesc->name = (char *)mem_Malloc(128 * sizeof(char), __LINE__, __FILE__);
    strncpy(entityDesc->name, (char *)name, 127);

    entityDesc->type = type;

    entityDesc->update = 1;
    entityDesc->draw = 1;

    entityDesc->attached = attach;

    entityDesc->free = free;

    entityDesc->owner = owner;

    if(type == UIE_SPINE)
    {
        depWatcher_Setup(&entityDesc->depWatcher, entityDesc);

        if(owner != NULL)
        {
            uiSpine_SetOwner(entity, owner);
            entityDesc->update = 0;
            entityDesc->draw = 0;
        }
    }
    else
    {
        depWatcher_Setup(&entityDesc->depWatcher, entity);
    }

    switch(type)
    {
        case UIE_SPINE: entityDesc->entityType.spine = entity; break;
        case UIE_BUTTON: entityDesc->entityType.button = entity; break;
        case UIE_SCROLL: entityDesc->entityType.scroll = entity; break;
        case UIE_SPRITE: entityDesc->entityType.sprite = entity; break;
        case UIE_TEXTINPUT: entityDesc->entityType.textInput = entity; break;
        case UIE_TEXTBOX: entityDesc->entityType.textBox = entity; break;
    }


    depWatcher_AddBoth(&entityDesc->depWatcher, watchList, type);

    return entityDesc;
}

/*
    Function: uiSpineEntity_Clean

    Description -
    Cleans up an entity description structure.

    1 argument:
    Ui_Spine_Entity *entityDesc - The entity description structure to be cleaned.
*/
void uiSpineEntity_Clean(Ui_Spine_Entity *entityDesc)
{
    depWatcher_Clean(&entityDesc->depWatcher);

    mem_Free(entityDesc->name);

    return;
}

/*
    Function: uiSpineEntity_SetLogic

    Description -
    Sets whether the entity should be updated by the spine or deleted.

    2 arguments:
    Ui_Spine_Entity *entityDesc - The entity to set the logic of.
    int logicUpdate - UIS_ENTITY_UPDATE if the entity should be updated, UIS_ENTITY_NO_UPDATE for no update, UIS_ENTITY_DELETE to delete.
*/
void uiSpineEntity_SetLogic(Ui_Spine_Entity *entityDesc, int logicUpdate)
{
    entityDesc->update = logicUpdate;

    return;
}

/*
    Function: uiSpineEntity_SetDraw

    Description -
    Sets whether the entity should be drawn or not.

    2 arguments:
    Ui_Spine_Entity *entityDesc - The entity to set the draw state of.
    int drawUpdate - The entity will be drawn if this is set to 1.
*/
void uiSpineEntity_SetDraw(Ui_Spine_Entity *entityDesc, int drawUpdate)
{
    entityDesc->draw = drawUpdate;

    return;
}

Vector2DInt uiSpineEntity_GetPosBase(Ui_Spine_Entity *entityDesc)
{
    Vector2DInt pos = {0, 0};

    switch(entityDesc->type)
    {
        case UIE_SPINE:
        pos = uiSpine_GetPos(entityDesc->entity);
        break;

        case UIE_BUTTON:
        pos = uiButton_GetPosText(entityDesc->entity);
        break;

        case UIE_SCROLL:
        pos = uiButtonScroll_GetPos(entityDesc->entity);
        break;

        case UIE_TEXTINPUT:
        pos = uiTextInput_GetPos(entityDesc->entity);
        break;

        case UIE_TEXTBOX:
        pos = uiTextBox_GetPos(entityDesc->entity);
        break;

        case UIE_SPRITE:
        pos = sprite_GetPos(entityDesc->entity);
        break;
    }

    return pos;
}

Ui_Rect uiSpineEntity_GetRectBase(Ui_Spine_Entity *entityDesc)
{
    Ui_Rect rect;

    switch(entityDesc->type)
    {
        case UIE_SPINE:
        rect = *uiSpine_GetRect(entityDesc->entity);
        break;

        case UIE_BUTTON:
        rect = *uiButton_GetRect(entityDesc->entity);
        break;

        case UIE_SCROLL:
        rect = *uiButtonScroll_GetRect(entityDesc->entity);
        break;

        case UIE_TEXTINPUT:
        rect = *uiTextInput_GetRect(entityDesc->entity);
        break;

        case UIE_TEXTBOX:
        rect = *uiTextBox_GetRect(entityDesc->entity);
        break;

        case UIE_SPRITE:
        uiRect_Setup(&rect, entityDesc->entityType.sprite->x, entityDesc->entityType.sprite->y, sprite_Width(entityDesc->entityType.sprite), sprite_Height(entityDesc->entityType.sprite));
        break;
    }

    return rect;
}
