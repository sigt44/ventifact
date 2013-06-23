#include "Spine.h"

#include "SpineEntity.h"
#include "../Graphics/Surface.h"
#include "../Graphics/Sprite.h"
#include "../Maths.h"
#include "ButtonPaths.h"

char *uiSpine_EntityNames[UIE_TOTALTYPES] =
{
    "Button",
    "Scroll",
    "ButtonChain",
    "Spine",
    "TextBox",
    "Sprite",
    "TextInput"
};

char uiSpine_EntityDivider = ':';


/*
    Function: uiSpine_GetEntityBase

    Description -
    Finds and returns a ui spine entity base in the spine that has the same name.
    If nothing is found then it returns NULL.

    2 arguments:
    Ui_Spine *spine - The spine that should contain the entity base.
    const char *entityName - The entities name.
*/
Ui_Spine_Entity *uiSpine_GetEntityBase(Ui_Spine *spine, const char *entityName)
{
    struct list *entityList = spine->entityList;

    Ui_Spine_Entity *entityDesc = NULL;

    while(entityList != NULL)
    {
        entityDesc = entityList->data;

        if(strcmp(entityDesc->name, entityName) ==  0)
        {
            return entityDesc;
        }

        entityList = entityList->next;
    }

    printf("Warning could not find (%s) in spine list.\n", entityName);

    return NULL;
}

/*
    Function: uiSpine_GetEntityBase2

    Description -
    Finds and returns a ui spine entity base in the spine that has the same entity
    in the parameter.
    If nothing is found then it returns NULL.

    2 arguments:
    Ui_Spine *spine - The spine that should contain the entity base.
    void *entity - Pointer to the entity that should match that of the entity's description structure.
*/
Ui_Spine_Entity *uiSpine_GetEntityBase2(Ui_Spine *spine, void *entity)
{
    struct list *entityList = spine->entityList;

    Ui_Spine_Entity *entityDesc = NULL;

    while(entityList != NULL)
    {
        entityDesc = entityList->data;

        if(entityDesc->entity ==  entity)
        {
            return entityDesc;
        }

        entityList = entityList->next;
    }

    printf("Warning could not find entity object at(%p) in spine list.\n", entity);

    return NULL;
}

/*
    Function: uiSpine_GetEntity

    Description -
    Finds and returns an entity in the ui spine that has the same name.
    If nothing is found then it returns NULL.

    2 arguments:
    Ui_Spine *spine - The spine that should contain the entity.
    const char *entityName - The entities name.
*/
void *uiSpine_GetEntity(Ui_Spine *spine, const char *entityName)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase(spine, entityName);

    if(entityDesc != NULL)
    {
        return entityDesc->entity;
    }

    return NULL;
}

/*
    Function: uiSpine_RemoveEntity

    Description -
    Completely remove all references to an entity in a spine.
    Returns the entity that has been removed.

    2 arguments:
    Ui_Spine *spine - The spine that should contain the entity.
    const char *entityName - The entities name.
*/
void *uiSpine_RemoveEntity(Ui_Spine *spine, const char *entityName)
{
    struct list *entityList = spine->entityList;

    Ui_Spine_Entity *entityDesc = NULL;
    Ui_Path *path = NULL;

    void *entity = NULL;

    while(entityList != NULL)
    {
        entityDesc = entityList->data;

        if(strcmp(entityDesc->name, entityName) == 0)
        {
            /*obtain pointer to the entity*/
            entity = entityDesc->entity;

            /*If the entity was placed in the button chooser, remove reference to it*/
            //uiButtonChooser_RemoveSegment(&spine->buttonChooser, entityName);
            while((path = uiMap_GetPath(&spine->map, -1, entityDesc)) != NULL)
            {
                uiMap_RemovePath(&spine->map, path);
            }

            /*delete the list link*/
            list_Delete_Node(&spine->entityList, entityList);

            /*remove the entity description*/
            uiSpineEntity_Clean(entityDesc);
            mem_Free(entityDesc);

            /*Return the entity*/
            return entity;
        }

        entityList = entityList->next;
    }

    printf("Warning could not find (%s) to remove in spine list.\n", entityName);

    return NULL;
}

/*
    Function: uiSpine_DeleteEntity

    Description -
    Completely remove all references to an entity in a spine.
    Also cleans up the entity and frees any allocated memory used for it.

    2 arguments:
    Ui_Spine *spine - The spine that should contain the entity.
    Ui_Spine_Entity *entityDesc - Pointer to the entity description.
*/
void uiSpine_DeleteEntity(Ui_Spine *spine, Ui_Spine_Entity *entityDesc)
{
    int entityType = entityDesc->type;
    int free = entityDesc->free;
    void *entity = NULL;

    entity = uiSpine_RemoveEntity(spine, entityDesc->name);
    entityDesc = NULL;

    switch(entityType)
    {
        case UIE_SPINE:

        uiSpine_Clean(entity);

        break;

        case UIE_BUTTON:

        uiButton_Clean(entity);

        break;

        case UIE_BUTTONCHAIN:

        uiButtonChain_Clean(entity);

        break;

        case UIE_SCROLL:

        uiButtonScroll_Clean(entity);

        break;

        case UIE_SPRITE:

        if(free == A_FREE)
        {
            sprite_Clean(entity);
        }

        break;

        case UIE_TEXTBOX:

        uiTextBox_Clean(entity);

        break;

        case UIE_TEXTINPUT:

        uiTextInput_Clean(entity);

        break;

        default:
        printf("Error: Unknown entity type to delete %d\n", entityType);
        break;
    }

    if(free == A_FREE)
    {
        mem_Free(entity);
    }

    return;
}

/*
    Function: uiSpine_SetEntityUpdate

    Description -
    Enable/Disable an entity in the spine to be updated / drawn

    4 arguments:
    Ui_Spine *spine - The spine the entity is in.
    const char *entityName - The name of the entity
    int logicUpdate - UIS_ENTITY_UPDATE if the entity should be updated, UIS_ENTITY_NO_UPDATE for no update, UIS_ENTITY_DELETE to delete.
    int drawUpdate - 1 if the entity should be drawn
*/
void uiSpine_SetEntityUpdate(Ui_Spine *spine, const char *entityName, int logicUpdate, int drawUpdate)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase(spine, entityName);

    uiSpineEntity_SetLogic(entityDesc, logicUpdate);
    uiSpineEntity_SetDraw(entityDesc, drawUpdate);

    return;
}

/*
    Function: uiSpine_SetEntityUpdate2

    Description -
    Enable/Disable an entity in the spine to be updated / drawn.

    4 arguments:
    Ui_Spine *spine - The spine the entity is in.
    void *entity - The entity to update.
    int logicUpdate - UIS_ENTITY_UPDATE if the entity should be updated, UIS_ENTITY_NO_UPDATE for no update, UIS_ENTITY_DELETE to delete.
    int drawUpdate - 1 if the entity should be drawn
*/
void uiSpine_SetEntityUpdate2(Ui_Spine *spine, void *entity, int logicUpdate, int drawUpdate)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase2(spine, entity);

    uiSpineEntity_SetLogic(entityDesc, logicUpdate);
    uiSpineEntity_SetDraw(entityDesc, drawUpdate);

    return;
}

/*
    Function: uiSpine_SetEntityDelete

    Description -
    Set an entity to be deleted in the spine.

    2 arguments:
    Ui_Spine *spine - The spine the entity is in.
    const char *entityName - The name of the entity to be deleted
*/
void uiSpine_SetEntityDelete(Ui_Spine *spine, const char *entityName)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase(spine, entityName);

    uiSpineEntity_SetLogic(entityDesc, UIS_ENTITY_DELETE);

    return;
}

/*
    Function: uiSpine_SetEntityDelete2

    Description -
    Set an entity to be deleted in the spine.

    2 arguments:
    Ui_Spine *spine - The spine the entity is in.
    void *entity - The entity to be deleted
*/
void uiSpine_SetEntityDelete2(Ui_Spine *spine, void *entity)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase2(spine, entity);

    uiSpineEntity_SetLogic(entityDesc, UIS_ENTITY_DELETE);

    return;
}

/*
    Function: uiSpine_SetEntityPosition

    Description -
    Sets the relative position of an entity in the spine.
    Returns the entity that is having its position changed.

    4 arguments:
    Ui_Spine *spine - The spine the entity is in.
    const char *entityName - The name of the entity to have its position set.
    int x - The x-axis position of the entity, relative to the spines position.
    int y - The y-axis position of the entity, relative to the spines position.
*/
Ui_Spine_Entity *uiSpine_SetEntityPos(Ui_Spine *spine, const char *entityName, int x, int y)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase(spine, entityName);

    if(entityDesc->attached == 1)
    {
        x += spine->rect.x;
        y += spine->rect.y;
    }

    switch(entityDesc->type)
    {
        case UIE_SPINE:

        uiSpine_SetPos(entityDesc->entity, x, y);

        break;

        case UIE_BUTTON:

        uiButton_SetPos(entityDesc->entity, x, y);

        break;

        case UIE_BUTTONCHAIN:

        //uiButtonChain_SetPos(entityDesc->entity, x, y);

        break;

        case UIE_SCROLL:

        uiButtonScroll_SetPosition(entityDesc->entity, x, y);

        break;

        case UIE_SPRITE:

        sprite_SetPos(entityDesc->entity, x, y);

        break;

        case UIE_TEXTBOX:

        uiTextBox_SetPos(entityDesc->entity, x, y);

        break;

        case UIE_TEXTINPUT:

        uiTextInput_SetPos(entityDesc->entity, x, y);

        break;

        default:

        printf("Error: Unknown entity type to set position %d\n", entityDesc->type);

        break;
    }

    return entityDesc;
}

/*
    Function: uiSpine_ChangeEntityPosition

    Description -
    Changes the position of an entity in the spine.

    4 arguments:
    Ui_Spine *spine - The spine the entity is in.
    const char *entityName - The name of the entity to have its position changed.
    int xDis - Change in the x-axis.
    int yDis - Change in the y-axis.
*/
void uiSpine_ChangeEntityPos(Ui_Spine *spine, const char *entityName, int xDis, int yDis)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase(spine, entityName);

    switch(entityDesc->type)
    {
        case UIE_SPINE:

        uiSpine_ChangePos(entityDesc->entity, xDis, yDis);

        break;

        case UIE_BUTTON:

        uiButton_ChangePos(entityDesc->entity, xDis, yDis);

        break;

        case UIE_SCROLL:

        uiButtonScroll_ChangePosition(entityDesc->entity, xDis, yDis);

        break;

        case UIE_TEXTBOX:

        uiTextBox_ChangePos(entityDesc->entity, xDis, yDis);

        break;

        case UIE_SPRITE:

        sprite_ChangePos(entityDesc->entity, xDis, yDis);

        break;

        case UIE_TEXTINPUT:

        uiTextInput_ChangePos(entityDesc->entity, xDis, yDis);

        break;

        default:
        printf("Error: Unknown entity type to change position %d\n", entityDesc->type);
        break;

    }

    return;
}

Vector2DInt uiSpine_GetEntityPos(Ui_Spine *spine, const char *entityName)
{
    Ui_Spine_Entity *entityDesc = uiSpine_GetEntityBase(spine, entityName);

    return uiSpineEntity_GetPosBase(entityDesc);
}

/*
    Function: uiSpine_SetUpdateAllEntity

    Description -
    Sets the update state for all entities in the spine.

    3 arguments:
    Ui_Spine *spine - The spine the entities are in.
    int logicUpdate - UIS_ENTITY_UPDATE if the entity should be updated, UIS_ENTITY_NO_UPDATE for no update, UIS_ENTITY_DELETE to delete.
    int drawUpdate - 1 if the entity should be drawn
*/
void uiSpine_SetUpdateAllEntity(Ui_Spine *spine, int logicUpdate, int drawUpdate)
{
    struct list *entityList = spine->entityList;

    while(entityList != NULL)
    {
        uiSpineEntity_SetLogic(entityList->data, logicUpdate);
        uiSpineEntity_SetDraw(entityList->data, drawUpdate);

        entityList = entityList->next;
    }

    return;
}

/*
    Function: uiSpine_ReportEntity

    Description -
    Log/Print all entities in the spine.

    2 arguments:
    Ui_Spine *spine - The spine the entities are in.
    int recursive - If (1) then will report the entities of spines that are themselves entities of a spine.
*/
void uiSpine_ReportEntity(Ui_Spine *spine, int recursive)
{
    struct list *entityList = spine->entityList;
    Ui_Spine_Entity *entityDesc = NULL;

    file_Log(ker_Log(), 1, "Reporting entities in spine(%p):\n", spine);

    while(entityList != NULL)
    {
        entityDesc = entityList->data;
        file_Log(ker_Log(), 1, "%s %p %d %d %d %d\n", entityDesc->name, entityDesc->entity, entityDesc->type, entityDesc->update, entityDesc->draw, entityDesc->free);

        if(entityDesc->type == UIE_SPINE && recursive == 1)
        {
            file_Log(ker_Log(), 1, "-- Entering %s --\n", entityDesc->name);
            uiSpine_ReportEntity(entityDesc->entity, recursive);
            file_Log(ker_Log(), 1, "-- Exiting %s --\n", entityDesc->name);
        }

        entityList = entityList->next;
    }

    return;
}

void uiSpine_AlignEntities(Ui_Spine *spine, int xStart, int yStart, int padding, int direction, int numEntity, ...)
{
    int x = 0;
    int y = 0;
    int height = 0;
    int width = 0;
    va_list args;
    Ui_Spine_Entity *entityDesc = NULL;
    Ui_Rect rect;
    char *entityName = NULL;

    va_start(args, numEntity);

    for(x = 0; x < numEntity; x++)
    {
        entityName = va_arg(args, char *);

        if(direction == SCR_V)
        {
            y = height + (padding * x);

            entityDesc = uiSpine_SetEntityPos(spine, entityName, xStart, yStart + y);

            rect = uiSpineEntity_GetRectBase(entityDesc);
            height += rect.h;
        }
        else
        {
            y = width + (padding * x);

            entityDesc = uiSpine_SetEntityPos(spine, entityName, xStart + y, yStart);

            rect = uiSpineEntity_GetRectBase(entityDesc);
            width += rect.w;
        }

    }

    va_end(args);

    return;
}

Ui_Spine *uiSpine_Create(int layer, int x, int y, Frame *background, Ui_Pointer *pnt, int pntOwner, Timer *src_Timer, unsigned int flags)
{
    Ui_Spine *newSpine = (Ui_Spine *)mem_Malloc(sizeof(Ui_Spine), __LINE__, __FILE__);

    uiSpine_Setup(newSpine, layer, x, y, background, pnt, pntOwner, src_Timer, flags);

    return newSpine;
}

void uiSpine_Setup(Ui_Spine *spine, int layer, int x, int y, Frame *background, Ui_Pointer *pnt, int pntOwner, Timer *src_Timer, unsigned int flags)
{
    int i = 0;

    uiRect_Setup(&spine->rect, x, y, 0, 0);

    spine->entityList = NULL;
    spine->templateList = NULL;
    spine->currentTemplate = NULL;

    for(i = 0; i < UIE_TOTALTYPES; i++)
    {
        spine->baseList[i] = NULL;
    }

    spine->pausedFlags = 0;
    spine->startingFlags = flags;
    spine->flags = flags;

    //uiSpine_SetFlags(spine, UI_SPINE_ALLFLAGS);

    spine->layer = layer;

    spine->state = SPINE_STATE_IDLE;

    spine->sTimer = timer_Setup(src_Timer, 0, 0, 1);

    if(background == NULL)
    {
        background = frame_CreateEmpty();
    }

    sprite_Setup(&spine->background, 0, spine->layer, src_Timer, 1,
                 background
    );

    uiRect_SetDim(&spine->rect, sprite_Width(&spine->background), sprite_Height(&spine->background));

    sprite_SetPos(&spine->background, x, y);

    spine->camera = NULL;

    spine->pnt = pnt;
    spine->pntOwner = pntOwner;

    spine->spinesOpened = 0;
    spine->openedFrom = NULL;
    spine->owner = NULL;

    spine->paused = 0;

    spine->onUpdate = NULL;
    spine->onDelete = NULL;

    //uiButtonChooser_Setup(&spine->buttonChooser, &spine->sTimer);
    uiMap_Setup(&spine->map, &spine->sTimer);

    return;
}

void uiSpine_SetupCamera(Ui_Spine *spine, Camera_2D *camera, unsigned int width, unsigned int height, int maxWidth, int maxHeight)
{
    spine->camera = camera;

    printf("Setting up camera at %d %d\n", spine->rect.x, spine->rect.y);
    camera2D_Setup(spine->camera, spine->rect.x, spine->rect.y, width, height, maxWidth, maxHeight, 0, 0);

    camera2D_SetConstraints(spine->camera, spine->rect.x, spine->rect.y, maxWidth, maxHeight);

    return;
}

int uiSpine_VerifyEntityName(const char *entityName)
{
    int x = 0;
    char typeString[128];

    strncpy(typeString, entityName, 127);

    if(strlen(typeString) == 127)
    {
        printf("Error: Ui entity name is too long (>= 127) %s\n", typeString);

        return -1;
    }

    string_ReplaceChar(typeString, uiSpine_EntityDivider, '\0');

    for(x = 0; x < UIE_TOTALTYPES; x++)
    {
        if(strcmp(typeString, uiSpine_EntityNames[x]) == 0)
        {
            return x;

            break;
        }
    }

    printf("Error: Unknown entity type name %s\n", typeString);

    return -1;
}

void uiSpine_AddEntity(Ui_Spine *spine, void *entity, int attach, int autoFree, const char *entityName)
{
    Ui_Spine_Entity *entityDesc = NULL;

    int x = 0;

    int type = uiSpine_VerifyEntityName(entityName);

    if(type == -1)
        return;

    entityDesc = uiSpineEntity_Create(entity, entityName, spine, &spine->baseList[type], type, attach, autoFree);

    list_Push(&spine->entityList, entityDesc, 0);

    /*Make the entity relative to the spine*/
    if(attach == 1)
    {
        uiSpine_ChangeEntityPos(spine, entityName, spine->rect.x, spine->rect.y);
    }

    return;
}

void uiSpine_AddButton(Ui_Spine *spine, Ui_Button *button, const char *name)
{
    uiSpine_AddEntity(spine, button, 1, A_FREE, name);

    return;
}

void uiSpine_AddScroll(Ui_Spine *spine, Ui_ButtonScroll *scroll, const char *name)
{
    uiSpine_AddEntity(spine, scroll, 1, A_FREE, name);

    return;
}

void uiSpine_AddSprite(Ui_Spine *spine, Sprite *sprite, int autoFree, const char *name)
{
    uiSpine_AddEntity(spine, sprite, 1, autoFree, name);

    return;
}

void uiSpine_AddSpine(Ui_Spine *spine, Ui_Spine *addSpine, int attached, const char *name)
{
    uiSpine_AddEntity(spine, addSpine, attached, A_FREE, name);

    return;
}

void uiSpine_AddTextBox(Ui_Spine *spine, Ui_TextBox *textBox, const char *name)
{
    uiSpine_AddEntity(spine, textBox, 1, A_FREE, name);

    return;
}

void uiSpine_AddTextInput(Ui_Spine *spine, Ui_TextInput *textInput, const char *name)
{
    uiSpine_AddEntity(spine, textInput, 1, A_FREE, name);

    return;
}


void uiSpine_Draw(Ui_Spine *spine, SDL_Surface *destination)
{
    struct list *entityList = spine->entityList;
    struct list *spineList = spine->baseList[UIE_SPINE];
    Sprite *sprite = NULL;

    Ui_Spine_Entity *entityDesc = NULL;

    if(flag_Check(&spine->flags, UI_SPINE_DRAWSPINES) == 1)
    {
        while(spineList != NULL)
        {
            entityDesc = spineList->data;

            if(entityDesc->draw == 1)
            {
                uiSpine_Draw(entityDesc->entity, destination);
            }

            spineList = spineList->next;
        }
    }

    if(spine->pnt != NULL && flag_Check(&spine->flags, UI_SPINE_UPDATEPOINTER) == 1)
    {
        uiPointer_Draw(spine->pnt, destination);
    }

    if(flag_Check(&spine->flags, UI_SPINE_DRAW) == 0)
    {
        return;
    }

    sprite_Draw(&spine->background, destination);

    while(entityList != NULL)
    {
        entityDesc = entityList->data;

        if(entityDesc->draw == 1)
        {
            switch(entityDesc->type)
            {
                case UIE_SPINE:

                break;

                case UIE_BUTTON:

                uiButton_Draw(entityDesc->entity, spine->camera, destination);

                break;

                case UIE_SCROLL:

                uiButtonScroll_Draw(entityDesc->entity, spine->camera, destination);

                break;

                case UIE_SPRITE:

                sprite = entityDesc->entity;

                if(spine->camera == NULL)
                {
                    sprite_Draw(entityDesc->entity, destination);
                }
                else if(camera2D_Collision(spine->camera, sprite->x, sprite->y, sprite_Width(sprite), sprite_Height(sprite)) == 1)
                {
                    sprite_DrawAtPosFit(camera2D_RelativeX(spine->camera, sprite->x), camera2D_RelativeY(spine->camera, sprite->y), &spine->camera->originRect, sprite, destination);
                }

                break;

                case UIE_TEXTBOX:

                uiTextBox_Draw(entityDesc->entity, spine->camera, destination);

                break;

                case UIE_TEXTINPUT:

                uiTextInput_Draw(entityDesc->entity, spine->camera, destination);

                break;

                default:
                printf("Error: Unknown entity type to draw %d\n", entityDesc->type);
                break;
            }
        }

        entityList = entityList->next;
    }

    return;
}

void uiSpine_Update(Ui_Spine *spine, float deltaTime)
{
    struct list *entityList = NULL;
    struct list *next = NULL;

    struct list *spines = NULL;

    Ui_Spine_Entity *entityDesc = NULL;

    int pntMoved = 0;
    Ui_Map *map = &spine->map;

    Ui_Pointer *updatePointer = spine->pnt;

    Vector2D lastPos = {0.0f, 0.0f};

    spine->state = SPINE_STATE_IDLE;

    timer_Calc(&spine->sTimer);

    if(spine->pnt != NULL)
    {
        lastPos = uiPointer_GetPos(spine->pnt);
    }

    if(flag_Check(&spine->flags, UI_SPINE_UPDATESPINES) == 1)
    {
        spines = spine->baseList[UIE_SPINE];

        while(spines != NULL)
        {
            next = spines->next;
            entityDesc = spines->data;

            if(entityDesc->update == 1)
            {
                spine->state = SPINE_STATE_SPINE;
                uiSpine_Update(entityDesc->entity, deltaTime);
            }

            spines = next;
        }
    }

    if(spine->currentTemplate != NULL)
    {
        map = &spine->currentTemplate->map;
    }

    if(spine->pnt != NULL)
    {
        if(flag_Check(&spine->flags, UI_SPINE_UPDATEPOINTER) == 1)
        {
            uiPointer_Update(spine->pnt, deltaTime);
        }

        /*If the pointer moved*/
        if(spine->pnt->moved == 1 && flag_Check(&spine->flags, UI_SPINE_UPDATECHOOSER) == 1)
        {
            /*Unselected the button chosen by the chooser*/
            uiMap_Select(map, 0);
            map->state = UI_MAPSTATE_OFF;

            pntMoved = 1;
        }
    }

    if(pntMoved == 0 && flag_Check(&spine->flags, UI_SPINE_UPDATECHOOSER) == 1)
    {
        /*update the button chooser*/
        uiMap_Update(map, spine->pnt);

        if(map->currentPath != NULL && map->currentPath->selected > 0)
        {
            spine->state = SPINE_STATE_CHOOSER;
        }
    }

    if(flag_Check(&spine->flags, UI_SPINE_UPDATE) == 0)
    {
        return;
    }

    /*Camera compatibility*/
    if(spine->camera != NULL)
    {
        if(spine->pnt != NULL)
        {
            spine->pnt->position.x -= camera2D_RelativeX(spine->camera, 0);
            spine->pnt->position.y -= camera2D_RelativeY(spine->camera, 0);

            if(camera2D_Collision(spine->camera, (int)(spine->pnt->position.x - spine->pnt->halfWidth), (int)(spine->pnt->position.y - spine->pnt->halfHeight), sprite_Width(spine->pnt->graphic), sprite_Height(spine->pnt->graphic)) == 1)
            {

            }
            else
            {
                updatePointer = NULL;
            }
        }
    }

    entityList = spine->entityList;

    while(entityList != NULL)
    {
        next = entityList->next;

        entityDesc = entityList->data;

        if(entityDesc->update == UIS_ENTITY_UPDATE)
        {
            switch(entityDesc->type)
            {
                case UIE_SPINE:

                break;

                case UIE_BUTTON:

                uiButton_Update(entityDesc->entity, updatePointer);

                break;

                case UIE_SCROLL:

                uiButtonScroll_Update(entityDesc->entity, updatePointer);

                break;

                case UIE_SPRITE:

                break;

                case UIE_TEXTBOX:

                uiTextBox_UpdateParse(entityDesc->entity);

                break;

                case UIE_TEXTINPUT:

                uiTextInput_Update(entityDesc->entity, updatePointer);

                break;

                default:
                printf("Error: Unknown entity type to update %d\n", entityDesc->type);
                break;
            }
        }
        else if(entityDesc->update == UIS_ENTITY_DELETE)
        {
            uiSpine_DeleteEntity(spine, entityDesc);
        }

        entityList = next;
    }

    /*Camera compatibility*/
    if(spine->camera != NULL)
    {
        if(spine->pnt != NULL)
        {
            spine->pnt->position.x += camera2D_RelativeX(spine->camera, 0);
            spine->pnt->position.y += camera2D_RelativeY(spine->camera, 0);
        }
    }

    if(spine->onUpdate != NULL)
        spine->onUpdate(spine);

    return;
}

/*
    Function: uiSpine_SetFlags

    Description -
    Sets the flags of the spine to determine its behaviour.

    UI_SPINE_DRAW = 1,          - Spine will draw all components apart from other spines
    UI_SPINE_DRAWSPINES = 2,    - Spine will (draw/also draw) other spines
    UI_SPINE_UPDATE = 4,        - Spine will (update/also update) other spines.
    UI_SPINE_UPDATESPINES = 8  - Spine will update all components apart from other spines
    UI_SPINE_UPDATEPOINTER = 16 - Spine will update the mouse pointer
    UI_SPINE_UPDATECHOOSER = 32 - Spine will update the button chooser
    UI_SPINE_ALLFLAGS - Spine will perform all of the above.

    2 arguments:

    Ui_Spine *spine - The spine to set the flags of.
    unsigned flags - The flags to set to the ui spine.
*/
void uiSpine_SetFlags(Ui_Spine *spine, unsigned int flags)
{
    spine->flags = flags;

    return;
}

void uiSpine_Enable(Ui_Spine *spine)
{
    if(spine->paused == 0)
    {
        /*Reset the spine to its default flags*/
        spine->flags = spine->startingFlags;
    }
    else if(spine->paused == 1) /*Make sure to open the spine as being paused*/
    {
        spine->flags = spine->pausedFlags;
        uiSpine_Pause(spine);
    }

    return;
}

void uiSpine_Disable(Ui_Spine *spine)
{
    spine->flags = 0;

    return;
}

void uiSpine_DisableBase(Ui_Spine *spine)
{
    unsigned int baseFlags = UI_SPINE_UPDATE | UI_SPINE_UPDATEPOINTER | UI_SPINE_UPDATECHOOSER;

    spine->flags = spine->flags - (spine->flags & baseFlags);

    return;
}

void uiSpine_Pause(Ui_Spine *spine)
{
    spine->pausedFlags = spine->flags;
    uiSpine_DisableBase(spine);

    spine->paused = 1;

    return;
}

void uiSpine_Resume(Ui_Spine *spine)
{
    Ui_Map *map = NULL;
    spine->flags = spine->pausedFlags;
    spine->paused = 0;

    /*Make a delay to the button mapping*/
    if(spine->currentTemplate != NULL)
    {
        map = &spine->currentTemplate->map;
    }
    else
    {
        map = &spine->map;
    }
    uiMap_ResetMoveTime(map);

    return;
}

Vector2DInt uiSpine_GetPos(Ui_Spine *spine)
{
    Vector2DInt pos = {spine->rect.x, spine->rect.y};

    return pos;
}

Ui_Rect *uiSpine_GetRect(Ui_Spine *spine)
{
    return &spine->rect;
}

/*
    Function: uiSpine_Open

    Description -
    This enables the spine, making sure it will be drawn and updated.
    If the openedFrom variable is not NULL, that spine will be paused. It will
    resume once uiSpine_Close has been called on the spine that has been opened.

    2 arguments:

    Ui_Spine *spine - The spine to open.
    Ui_Spine *openedFrom - The spine that the first argument is being opened from. (Can be NULL).
*/
void uiSpine_Open(Ui_Spine *spine, Ui_Spine *openedFrom)
{
    uiSpine_Enable(spine);

    if(spine->owner != NULL)
    {
        uiSpine_SetEntityUpdate2(spine->owner, spine, UIS_ENTITY_UPDATE, UIS_ENTITY_UPDATE);
    }

    spine->openedFrom = openedFrom;

    if(spine->openedFrom != NULL)
    {
        uiSpine_Pause(spine->openedFrom);
        spine->openedFrom->spinesOpened ++;
    }

    return;
}

void uiSpine_Close(Ui_Spine *spine)
{
    uiSpine_Disable(spine);

    if(spine->owner != NULL)
    {
        uiSpine_SetEntityUpdate2(spine->owner, spine, UIS_ENTITY_NO_UPDATE, UIS_ENTITY_NO_UPDATE);
    }

    if(spine->openedFrom != NULL)
    {
        spine->openedFrom->spinesOpened --;

        if(spine->openedFrom->spinesOpened == 0)
        {
            uiSpine_Resume(spine->openedFrom);
        }

        spine->openedFrom = NULL;
    }

    return;
}

void uiSpine_SetPos(Ui_Spine *spine, int x, int y)
{
    Ui_Spine_Entity *entityDesc = NULL;
    struct list *entityList = spine->entityList;

    int xChange = x - spine->rect.x;
    int yChange = y - spine->rect.y;

    printf("Moving spine to (%d %d) from (%d %d) change (%d %d)\n", x, y, spine->rect.x, spine->rect.y, xChange, yChange);

    uiRect_SetPos(&spine->rect, x, y);

    sprite_SetPos(&spine->background, x, y);

    if(spine->camera != NULL)
    {
        camera2D_SetConstraints(spine->camera, spine->camera->minX + xChange, spine->camera->minY + yChange, spine->camera->maxX + xChange, spine->camera->maxY + yChange);
        camera2D_ChangePosition(spine->camera, (float)xChange, (float)yChange);
        camera2D_SetOrigin(spine->camera, spine->camera->originRect.x + xChange, spine->camera->originRect.y + yChange);
    }

    while(entityList != NULL)
    {
        entityDesc = entityList->data;

        if(entityDesc->attached == 1)
        {
            uiSpine_ChangeEntityPos(spine, entityDesc->name, xChange, yChange);
        }

        entityList = entityList->next;
    }

    return;
}

void uiSpine_ChangePos(Ui_Spine *spine, int xDis, int yDis)
{
    Ui_Spine_Entity *entityDesc = NULL;
    struct list *entityList = spine->entityList;

    uiRect_ChangePos(&spine->rect, xDis, yDis);

    sprite_ChangePos(&spine->background, xDis, yDis);

    if(spine->camera != NULL)
    {
        camera2D_SetConstraints(spine->camera, spine->camera->minX + xDis, spine->camera->minY + yDis, spine->camera->maxX + xDis, spine->camera->maxY + yDis);
        camera2D_ChangePosition(spine->camera, (float)xDis, (float)yDis);
        camera2D_SetOrigin(spine->camera, spine->camera->originRect.x + xDis, spine->camera->originRect.y + yDis);
    }

    while(entityList != NULL)
    {
        entityDesc = entityList->data;

        if(entityDesc->attached == 1)
        {
            uiSpine_ChangeEntityPos(spine, entityDesc->name, xDis, yDis);
        }

        entityList = entityList->next;
    }

    return;
}

void uiSpine_SetOwner(Ui_Spine *spine, Ui_Spine *owner)
{
    spine->owner = owner;

    return;
}

void uiSpine_Clean(Ui_Spine *spine)
{
    Ui_Spine_Entity *entityDesc = NULL;

    /*Clean up the button chooser*/
    uiMap_Clean(&spine->map);

    /*Remove all the templates*/
    while(spine->templateList != NULL)
    {
        uiSpine_CleanTemplate(spine->templateList->data);

        mem_Free(spine->templateList->data);

        list_Pop(&spine->templateList);
    }

    /*Remove all the entities*/
    while(spine->entityList != NULL)
    {
        entityDesc = spine->entityList->data;

        uiSpine_DeleteEntity(spine, entityDesc);
    }

    /*Clear background*/
    sprite_Clean(&spine->background);

    /*Clear the pointer, if it belongs to this spine*/
    if(spine->pntOwner == 1)
    {
        uiPointer_Clean(spine->pnt);
        mem_Free(spine->pnt);
    }

    if(spine->camera != NULL)
    {
        mem_Free(spine->camera);
    }

    return;
}

Ui_Spine_TemplateEntity *uiSpine_CreateTemplateEntity(char *entityName, int updateState, int drawState)
{
    Ui_Spine_TemplateEntity *newTemplateEntity = (Ui_Spine_TemplateEntity *)mem_Alloc(sizeof(Ui_Spine_TemplateEntity));

    newTemplateEntity->entityName = entityName;
    newTemplateEntity->updateState = updateState;
    newTemplateEntity->drawState = drawState;

    return newTemplateEntity;
}

void uiSpine_CreateTemplate(Ui_Spine *spine, int ID, char *templateName, unsigned int flags, int numEntity, ...)
{
    Ui_Spine_Template *newTemplate = (Ui_Spine_Template *)mem_Alloc(sizeof(Ui_Spine_Template));

    va_list args;
    int x = 0;
    Ui_Spine_TemplateEntity *entityTemplate = NULL;
    Ui_Spine_Entity *entityDesc = NULL;

    newTemplate->name = templateName;
    newTemplate->numEntity = numEntity;
    newTemplate->ID = ID;
    newTemplate->flags = flags;

    uiMap_Setup(&newTemplate->map, &spine->sTimer);

    if(numEntity > 0)
    {
        /*Create the memory for an array of pointers to entities in a spine*/
        newTemplate->entityArray = (Ui_Spine_TemplateEntity **)mem_Alloc(sizeof(Ui_Spine_TemplateEntity *) * numEntity);

        va_start(args, numEntity);

        for(x = 0; x < numEntity; x++)
        {
            entityTemplate = va_arg(args, Ui_Spine_TemplateEntity *);

            entityDesc = uiSpine_GetEntityBase(spine, entityTemplate->entityName);

            if(uiPath_SupportedType(entityDesc->entity, entityDesc->type) == 1)
                uiMap_AddSinglePath(&newTemplate->map, NULL, 0, entityDesc);

            /*Assign the pointers to entities passed into the function*/
            newTemplate->entityArray[x] = entityTemplate;
        }

        va_end(args);
    }
    else
    {
        newTemplate->entityArray = NULL;
    }

    uiMap_CreateGeometricLinks(&newTemplate->map, 0);

    if(spine->pnt != NULL)
    {
       control_AddTimerLink(&newTemplate->map.cMove[UI_ENTER], &spine->pnt->cActivate, 0);
    }

    list_Stack(&spine->templateList, newTemplate, ID);

    return;
}

void uiSpine_CleanTemplate(Ui_Spine_Template *sTemplate)
{
    int x = 0;

    if(sTemplate->entityArray != NULL)
    {
        for(x = 0; x < sTemplate->numEntity; x++)
        {
            /*free each entity template*/
            mem_Free(sTemplate->entityArray[x]);
        }

        /*free the pointer array holder*/
        mem_Free(sTemplate->entityArray);
    }

    uiMap_Clean(&sTemplate->map);

    return;
}

Ui_Spine_Template *uiSpine_GetTemplate(Ui_Spine *spine, char *templateName)
{
    struct list *templateList = spine->templateList;
    Ui_Spine_Template *checkTemplate = NULL;

    while(templateList != NULL)
    {
        checkTemplate = templateList->data;

        if(strcmp(checkTemplate->name, templateName) == 0)
        {
            return checkTemplate;
        }

        templateList = templateList->next;
    }


    printf("Warning no template found with name '%s'\n", templateName);

    return NULL;
}

Ui_Spine_Template *uiSpine_GetTemplate2(Ui_Spine *spine, int templateID)
{
    struct list *templateList = spine->templateList;

    while(templateList != NULL)
    {
        if(templateList->info == templateID)
        {
            return templateList->data;
        }

        templateList = templateList->next;
    }

    printf("Warning no template found with ID '%d'\n", templateID);

    return NULL;
}

void uiSpine_OpenTemplateBase(Ui_Spine *spine, Ui_Spine_Template *sTemplate, int selectState)
{
    int x = 0;
    Ui_Spine_TemplateEntity *entityTemplate = NULL;

    if(spine->currentTemplate != NULL)
    {
        uiMap_Select(&spine->currentTemplate->map, SELECT_OFF);
    }
    else
    {
        uiMap_Select(&spine->map, SELECT_OFF);
    }

    uiMap_Select(&sTemplate->map, selectState);
    timer_Start(&sTemplate->map.moveTimer);

    spine->currentTemplate = sTemplate;


    uiSpine_SetFlags(spine, sTemplate->flags);

    uiSpine_SetUpdateAllEntity(spine, UIS_ENTITY_NO_UPDATE, 0);

    for(x = 0; x < sTemplate->numEntity; x++)
    {
        entityTemplate = sTemplate->entityArray[x];

        uiSpine_SetEntityUpdate(spine, entityTemplate->entityName, entityTemplate->updateState, entityTemplate->drawState);
    }

    return;
}

void uiSpine_OpenTemplate(Ui_Spine *spine, char *templateName, int selectState)
{
    Ui_Spine_Template *sTemplate = uiSpine_GetTemplate(spine, templateName);

    uiSpine_OpenTemplateBase(spine, sTemplate, selectState);

    return;
}

void uiSpine_OpenTemplate2(Ui_Spine *spine, int templateID, int selectState)
{
    Ui_Spine_Template *sTemplate = uiSpine_GetTemplate2(spine, templateID);

    uiSpine_OpenTemplateBase(spine, sTemplate, selectState);

    return;
}

void uiSpine_MapEntity(Ui_Spine *spine)
{
    struct list *entityList = spine->entityList;
    Ui_Spine_Entity *e = NULL;

    uiMap_Reset(&spine->map);

    /*Go through all the entities in the spine*/
    while(entityList != NULL)
    {
        e = (Ui_Spine_Entity *)entityList->data;

        if(uiPath_SupportedType(e->entity, e->type) == 1)
        {
            //printf("Pushing %s into map\n", e->name);
            uiMap_AddSinglePath(&spine->map, NULL, 0, e);
        }

        entityList = entityList->next;
    }

    uiMap_CreateGeometricLinks(&spine->map, 0);

    if(spine->pnt != NULL)
    {
       control_AddTimerLink(&spine->map.cMove[UI_ENTER], &spine->pnt->cActivate, 0);
    }
    //uiMap_FillPathLinks(&spine->map);

    /*Reset the select states of all the buttons*/
    uiMap_ForceUnselect(&spine->map);

    return;
}


