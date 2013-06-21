#include "ButtonChooser.h"

#include "Button.h"

/*
    Function: uiButtonChooserSegment_Create

    Description -
    Creates a segment that contains a list of buttons to traverse over using the keys instead
    of a mouse.

    11 arguments:
    struct list **buttonListHead - Points to the address of the head of the list of buttons to traverse through.
    const char *name - The name of the segment, so that it can be found again easily.
    Timer *srcTimer - Source timer for the controls.
    int changeTime - How often will the controls fire if activated.
    Control_Event *copyForward - The forward control will have the same keys as the control supplied here.
    Control_Event *copyBackward - The backward control will have the same keys as the control supplied here.
    Control_Event *copyEnter - The enter control will have the same keys as the control supplied here.
    Control_Event *copyExit - The exit control will have the same keys as the control supplied here.
    int type - What type of object owns the list of buttons, to give compatibility for scrolls.
    void *extraInfo - The object that owns the list of buttons.
    Sprite *hoverSprite - The sprite that is displayed when the segment is hovered over.
*/
Ui_BCSegment *uiButtonChooserSegment_Create(struct list **buttonListHead,
                                            const char *name, Timer *srcTimer,
                                            int changeTime,
                                            Control_Event *copyForward, Control_Event *copyBackward,
                                            Control_Event *copyEnter, Control_Event *copyExit,
                                            int type, void *extraInfo,
                                            Sprite *hoverSprite)
{
    Ui_BCSegment *segment = (Ui_BCSegment *)mem_Malloc(sizeof(Ui_BCSegment), __LINE__, __FILE__);

    Ui_ButtonScroll *scroll = NULL;

    segment->name = name;

    segment->buttonList = buttonListHead;
    segment->type = type;
    segment->extraInfo = extraInfo;
    segment->rotate = 0;
    segment->active = 1;

    segment->neighbourList = NULL;
    segment->linkedList = NULL;

    segment->atButton = 0;

    segment->currentButton = *segment->buttonList;

    control_Setup(&segment->c_Enter, "Enter", srcTimer, changeTime);
    control_Setup(&segment->c_Exit, "Exit", srcTimer, changeTime);

    control_Setup(&segment->c_Forwards, "Forwards", srcTimer, changeTime);
    control_Setup(&segment->c_Backwards, "Backwards", srcTimer, changeTime);

    if(copyForward != NULL)
        control_CopyKey(&segment->c_Forwards, copyForward);

    if(copyBackward != NULL)
        control_CopyKey(&segment->c_Backwards, copyBackward);

    if(copyEnter != NULL && copyExit != NULL)
    {
        segment->active = 0;

        control_CopyKey(&segment->c_Enter, copyEnter);
        control_CopyKey(&segment->c_Exit, copyExit);
    }

    segment->sHover = hoverSprite;

    switch(segment->type)
    {
        case BC_SCROLL:
        scroll = segment->extraInfo->dataArray[0];

        /*Set the segment to rotate if the scroll does*/
        segment->rotate = scroll->rotate;

        break;
    }

    return segment;
}

/*
    Function: uiButtonChooserSegment_Clean

    Description -
    Cleans all allocated memory in the segment.

    1 argument:
    Ui_BCSegment *segment - The segment to clean.
*/
void uiButtonChooserSegment_Clean(Ui_BCSegment *segment)
{
    control_Clean(&segment->c_Enter);
    control_Clean(&segment->c_Exit);
    control_Clean(&segment->c_Forwards);
    control_Clean(&segment->c_Backwards);

    list_Clear(&segment->linkedList);

    while(segment->neighbourList != NULL)
    {
        uiButtonChooserNeighbour_Clean(segment->neighbourList->data);
        mem_Free(segment->neighbourList->data);

        list_Pop(&segment->neighbourList);
    }

    if(segment->sHover != NULL)
    {
        sprite_Clean(segment->sHover);
        mem_Free(segment->sHover);
    }

    if(segment->extraInfo != NULL)
    {
        switch(segment->type)
        {
            case BC_BUTTON:
            list_Clear((struct list **)&segment->extraInfo->dataArray[0]);
            break;
        }

        dataStruct_Clean(segment->extraInfo);
        mem_Free(segment->extraInfo);
    }

    return;
}

/*
    Function: uiButtonChooserNeighbour_Create

    Description -
    Create a structure that holds a link to another segment. This
    link allows one segment to move to another provided the control
    is activated.

    3 arguments:
    Ui_BCSegment *segment - The segment this neighbour links to.
    Timer *srcTime - The source timer for the control.
    nt changeTime - The update time of the control.
*/
Ui_BCNeighbour *uiButtonChooserNeighbour_Create(Ui_BCSegment *segment, Timer *srcTime, int changeTime)
{
    Ui_BCNeighbour *neighbour = (Ui_BCNeighbour *)mem_Malloc(sizeof(Ui_BCNeighbour), __LINE__, __FILE__);

    neighbour->segment = segment;
    control_Setup(&neighbour->c_Go, "Go", srcTime, changeTime);

    neighbour->remove = 0;

    depWatcher_Setup(&neighbour->dW, neighbour);

    return neighbour;
}

/*
    Function: uiButtonChooserNeighbour_Clean

    Description -
    Clean up the neighbour structure.

    1 arguments:
    Ui_BCNeighbour *neighbour - The neigbhour structure to be cleaned.
*/
void uiButtonChooserNeighbour_Clean(Ui_BCNeighbour *neighbour)
{
    control_Clean(&neighbour->c_Go);

    depWatcher_Clean(&neighbour->dW);

    return;
}

/*
    Function: uiButtonChooser_AddNeighbour

    Description -
    This function adds a neighbour to a segment in a button chooser.

    5 arguments:
    Ui_ButtonChooser *bc - The button chooser which contains the segments.
    Ui_BCSegment *baseSegment - The segment which gets the neighbour added to it.
    Ui_BCSegment *neighbourSegment - The segment which is the neighbour.
    int changeTime - The update time of the control to move to the neighbour.
    Control_Event *copyEvent - The control that moves to the neighbour will have the same keys as this event.
*/
void uiButtonChooser_AddNeighbour(Ui_ButtonChooser *bc, Ui_BCSegment *baseSegment, Ui_BCSegment *neighbourSegment, int changeTime, Control_Event *copyEvent)
{
    /*Create the neighbour structure*/
    Ui_BCNeighbour *neighbour = uiButtonChooserNeighbour_Create(neighbourSegment, bc->srcTime, changeTime);

    /*Add it into the base segment neighbour list*/
    list_Push(&baseSegment->neighbourList, neighbour, 0);

    /*Also add in a reference to this neighbour to the neighbour segment, so that if that segment is removed the neighbour structure pointing to it can also be removed*/
    list_Push(&neighbourSegment->linkedList, neighbour, 0);
    depWatcher_Add(&neighbour->dW, &neighbourSegment->linkedList);

    control_CopyKey(&neighbour->c_Go, copyEvent);

    return;
}

/*
    Function: uiButtonChooser_AddNeighbour

    Description -
    This function adds two neighbours linking each segment in each direction.

    6 arguments:
    Ui_ButtonChooser *bc - The button chooser which contains the segments.
    Ui_BCSegment *baseSegment - The segment which gets a neighbour added to it, linking to neighbourSegment.
    Ui_BCSegment *neighbourSegment - The segment which gets a neighbour added to it, linking to baseSegment.
    int changeTime - The update time of the control to move to the neighbour.
    Control_Event *copyEventBase - The control that moves from the base to the neighbour segment contains the same keys as this.
    Control_Event *copyEventNeighbour - The control that moves from the neighbour to the base segment contains the same keys as this.
*/
void uiButtonChooser_AddNeighbourBoth(Ui_ButtonChooser *bc, Ui_BCSegment *baseSegment, Ui_BCSegment *neighbourSegment, int changeTime, Control_Event *copyEventBase, Control_Event *copyEventNeighbour)
{
    uiButtonChooser_AddNeighbour(bc, baseSegment, neighbourSegment, changeTime, copyEventBase);
    uiButtonChooser_AddNeighbour(bc, neighbourSegment, baseSegment, changeTime, copyEventNeighbour);

    return;
}

/*
    Function: uiButtonChooser_Setup

    Description -
    Setup a button chooser structure so that buttons can be selected using keys.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser to setup.
    Timer *srcTimer - Timer for controls to be based on.
*/
void uiButtonChooser_Setup(Ui_ButtonChooser *bc, Timer *srcTimer)
{
    bc->buttonSegments = NULL;
    bc->currentSegment = NULL;

    bc->srcTime = srcTimer;
    bc->busy = 0;

    return;
}

/*
    Function: uiButtonChooser_AddButtons

    Description -
    Add a list of buttons from an object of certain type to the button chooser.

    10 arguments:
    Ui_ButtonChooser *bc - The button chooser to add a list of buttons from the scroll.
    const char *name - The same of the segment created, so that it can be found easily.
    void *buttonObj - The object that contains the list of buttons to add.
    int type - The type of object that contains the buttons.
    Sprite *hoverSprite - The sprite that will be displayed when the segment is hovered over.
    int changeTime - How often will the controls fire if activated.
    Control_Event *copyForward - The forward control will have the same keys as the control supplied here.
    Control_Event *copyBackward - The backward control will have the same keys as the control supplied here.
    Control_Event *copyEnter - Keys to copy for the enter state. If NULL then there is no enter state.
    Control_Event *copyExit - Keys to copy for the exit state. If NULL then there is no exit state.
*/
void uiButtonChooser_AddButtons(Ui_ButtonChooser *bc,
                               const char *name,
                               void *buttonObj,
                               int type,
                               Sprite *hoverSprite,
                               int changeTime,
                               Control_Event *copyForward, Control_Event *copyBackward,
                               Control_Event *copyEnter, Control_Event *copyExit)
{
    Data_Struct *extraInfo = NULL;
    Ui_BCSegment *segment = NULL;

    struct list **buttonListHead = NULL;
    struct list *buttonList = NULL;

    switch(type)
    {
        default:
        printf("Warning: Unknown button object type attempt in button chooser %d\n", type);

        return;
        break;

        case BC_SCROLL:

        buttonListHead = &((Ui_ButtonScroll *)buttonObj)->buttons;
        extraInfo = dataStruct_Create(2, buttonObj, ((Ui_ButtonScroll *)buttonObj)->originButtonList);

        break;

        case BC_BUTTONLIST:

        buttonListHead = buttonObj;

        extraInfo = NULL;

        break;

        case BC_BUTTON:

        buttonList = NULL;

        list_Push(&buttonList, buttonObj, type);

        extraInfo = dataStruct_Create(1, buttonList);

        buttonListHead = (struct list **)&extraInfo->dataArray[0];

        break;

    }

    segment = uiButtonChooserSegment_Create(buttonListHead, name, bc->srcTime, changeTime, copyForward, copyBackward, copyEnter, copyExit, type, extraInfo, hoverSprite);

    list_Stack(&bc->buttonSegments, segment, type);

    if(bc->currentSegment == NULL)
    {
        bc->currentSegment = bc->buttonSegments;

        uiButtonChooser_Choose(bc, 1);
    }

    return;
}

/*
    Function: uiButtonChooser_GetSegment

    Description -
    Returns a segment in a button chooser that has the same name as 'name'.

    2 arguments -
    Ui_ButtonChooser *bc - The button chooser that the segment is in.
    const char *name - The name of the segment to get.
*/
Ui_BCSegment *uiButtonChooser_GetSegment(Ui_ButtonChooser *bc, const char *name)
{
    struct list *segmentList = bc->buttonSegments;

    Ui_BCSegment *segment = NULL;

    while(segmentList != NULL)
    {
        segment = segmentList->data;

        if(strcmp(segment->name, name) == 0)
        {
            return segment;
        }

        segmentList = segmentList->next;
    }

    //printf("Warning unable to find segment in button chooser named %s\n", name);

    return NULL;
}

/*
    Function: uiButtonChooser_RemoveSegment

    Description -
    Removes a segment from the chooser.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser to remove the buttons from.
    struct list **buttonList - The head of the list of buttons that are to be removed.
*/
int uiButtonChooser_RemoveSegment(Ui_ButtonChooser *bc, const char *name)
{
    Ui_BCSegment *segment = NULL;
    Ui_BCNeighbour *neighbour = NULL;

    segment = uiButtonChooser_GetSegment(bc, name);

    if(segment != NULL)
    {
        /*Check if the current segment is the one to remove*/
        if(bc->currentSegment->data == segment)
        {
            /*Turn off any button hover forcing*/
            uiButtonChooser_Choose(bc, 0);

            /*Revert to previous segment*/
            if(bc->currentSegment->previous != NULL)
                bc->currentSegment = bc->currentSegment->previous;
            else
                bc->currentSegment = bc->currentSegment->next;
        }

        /*Set all links to this segment to be destroyed*/
        while(segment->linkedList != NULL)
        {
            neighbour = segment->linkedList->data;
            printf("Letting neighbour %p know to remove %p from list (%d)\n", neighbour, segment, neighbour->remove);

            neighbour->remove = 1;

            list_Pop(&segment->linkedList);
        }

        list_Delete_NodeFromData(&bc->buttonSegments, segment);

        uiButtonChooserSegment_Clean(segment);
        mem_Free(segment);

        return 1;
    }

    return 0;
}

/*
    Function: uiButtonChooser_Choose

    Description -
    Sets(or unsets) the current button that the chooser is at to act as if it is being selected.

    2 arguments:
    Ui_ButtonChooser *bc - Button chooser to set/unset its current button.
    int activate - if 0 then unsets the button, if 1 then sets it.
*/
void uiButtonChooser_Choose(Ui_ButtonChooser *bc, int activate)
{
    struct list *currentButton = NULL;
    Ui_BCSegment *segment = NULL;

    if(bc->currentSegment != NULL)
    {
        segment = bc->currentSegment->data;

        currentButton = segment->currentButton;
    }

    if(currentButton != NULL)
    {
        uiButton_ForceHover(currentButton->data, activate);
    }

    return;
}

/*
    Function: uiButtonChooser_ForceOffSegment

    Description -
    Forces off all the buttons in a segment.

    2 arguments:
    Ui_BCSegment *segment - The segment to force off all the buttons.
    int ignoreCurrent - If true then force off the current button the segment is at as well.
*/
void uiButtonChooser_ForceOffSegment(Ui_BCSegment *segment, int ignoreCurrent)
{
    struct list *buttonList = buttonList = *segment->buttonList;

    while(buttonList != NULL)
    {
        /*If the current button should be ignored or its not the current button*/
        if(ignoreCurrent == 1 || buttonList != segment->currentButton)
        {
            /*Force the hover ability of the button off so that it cant be selected*/
            uiButton_ForceHover(buttonList->data, -1);
        }

        buttonList = buttonList->next;
    }

    return;
}

void uiButtonChooser_ForceOffAllSegment(Ui_ButtonChooser *bc, int ignoreCurrent)
{
    struct list *segmentList = bc->buttonSegments;

    while(segmentList != NULL)
    {
        if(segmentList != bc->currentSegment)
        {
            uiButtonChooser_ForceOffSegment(segmentList->data, 1);
        }
        else
        {
            uiButtonChooser_ForceOffSegment(segmentList->data, ignoreCurrent);
        }

        segmentList = segmentList->next;
    }

    return;
}

void uiButtonChooser_StopControls(Ui_BCSegment *segment, int stopTime)
{
    control_Stop(&segment->c_Enter, stopTime);
    control_Stop(&segment->c_Exit, stopTime);
    control_Stop(&segment->c_Forwards, stopTime);
    control_Stop(&segment->c_Backwards, stopTime);

    return;
}

/*
    Function: uiButtonChooser_TravForwards

    Description -
    Move the current button that is selected forwards in the button list of the current segment.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser that the segment is in.
    Ui_BCSegment *segment - The current segment of the button chooser.
*/
void uiButtonChooser_TravForwards(Ui_ButtonChooser *bc, Ui_BCSegment *segment)
{
    Ui_Button *button = NULL;
    Ui_ButtonScroll *scroll = NULL;

    button = segment->currentButton->data;

    /*If the current button that the segment is pointed to is not selected, then select that first instead of moving*/
    if(button->forceHover == 0)
    {
        switch(segment->type)
        {
            case BC_BUTTONLIST:
            default:

            break;

            case BC_SCROLL:
            scroll = segment->extraInfo->dataArray[0];

            segment->atButton = 0;

            segment->extraInfo->dataArray[1] = segment->currentButton = scroll->originButtonList;

            break;
        }

        uiButtonChooser_Choose(bc, 1);
    }
    else if(segment->currentButton->next != NULL || segment->rotate == 1) /*If it is actually possible to move forwards in the list*/
    {
        /*unselected the current button*/
        uiButtonChooser_Choose(bc, 0);

        if(segment->currentButton->next == NULL)
        {
            segment->currentButton = *segment->buttonList;
        }
        else
        {
            segment->currentButton = segment->currentButton->next;
        }

        /*select the next button*/
        uiButtonChooser_Choose(bc, 1);

        segment->atButton ++;
    }

    switch(segment->type)
    {
        case BC_BUTTONLIST:
        default:

        break;

        case BC_SCROLL:
        scroll = segment->extraInfo->dataArray[0];

        /*If the segment selected button is outside of range of the button scroll then move the scroll forwards*/
        if(segment->atButton >= scroll->maxShowButtons)
        {
            if(scroll->traverseForwards != NULL)
            {
                uiButton_TempActive(scroll->traverseForwards, 1);

                uiButton_Update(scroll->traverseForwards, NULL);
            }

            segment->extraInfo->dataArray[1] = scroll->originButtonList;

            segment->atButton --;
        }

        break;
    }

    return;
}

/*
    Function: uiButtonChooser_TravBackwards

    Description -
    Move the current button that is selected backwards in the button list of the current segment.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser that the segment is in.
    Ui_BCSegment *segment - The current segment of the button chooser.
*/
void uiButtonChooser_TravBackwards(Ui_ButtonChooser *bc, Ui_BCSegment *segment)
{
    Ui_Button *button = NULL;
    Ui_ButtonScroll *scroll = NULL;

    button = segment->currentButton->data;

    /*If the current button that the segment is pointed to is not selected, then select that first instead of moving*/
    /*This is often the case when the pointer moves in a ui spine*/
    if(button->forceHover == 0)
    {
        switch(segment->type)
        {
            case BC_BUTTONLIST:
            default:

            break;

            case BC_SCROLL:
            scroll = segment->extraInfo->dataArray[0];

            segment->atButton = 0;

            segment->extraInfo->dataArray[1] = segment->currentButton = scroll->originButtonList;

            break;
        }

        uiButtonChooser_Choose(bc, 1);
    }
    else if(segment->currentButton != *segment->buttonList || segment->rotate == 1) /*If its possible for the current button to move backwards*/
    {
        if(segment->currentButton->previous != segment->currentButton && segment->currentButton->previous != NULL)
        {
            /*unselect the current button*/
            uiButtonChooser_Choose(bc, 0);

            segment->currentButton = segment->currentButton->previous;

            /*select the previous button*/
            uiButtonChooser_Choose(bc, 1);

            segment->atButton --;
        }
    }

    switch(segment->type)
    {
        case BC_BUTTONLIST:
        default:

        break;

        case BC_SCROLL:
        scroll = segment->extraInfo->dataArray[0];

        /*If the current button of the segment is outside of range of the scroll, move the scroll backwards*/
        if(segment->atButton < 0)
        {
            if(scroll->traverseBackwards != NULL)
            {
                uiButton_TempActive(scroll->traverseBackwards, 1);

                uiButton_Update(scroll->traverseBackwards, NULL);
            }

            segment->extraInfo->dataArray[1] = scroll->originButtonList;


            segment->atButton ++;
        }

        break;
    }

    return;
}

/*
    Function: uiButtonChooser_SetSegmentName

    Description -
    Sets the current active segment in the button chooser to be the
    segment with name segmentName.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser that the segment is in.
    const char *segmentName - The name of the segment to set active.
*/
void uiButtonChooser_SetSegmentName(Ui_ButtonChooser *bc, const char *segmentName)
{
    Ui_BCSegment *segment = uiButtonChooser_GetSegment(bc, segmentName);

    if(segment != NULL)
    {
        uiButtonChooser_SetSegment(bc, segment);
    }

    return;
}

void uiButtonChooser_StopSegmentControls(Ui_BCSegment *segment)
{
    Ui_BCNeighbour *neighbour = NULL;

    struct list *neighbourList = NULL;
    struct list *next = NULL;

    /*Stop all neighbouring controls for a while*/
    neighbourList = segment->neighbourList;

    while(neighbourList != NULL)
    {
        neighbour = neighbourList->data;

        control_Stop(&neighbour->c_Go, neighbour->c_Go.repeater.end_Time);

        neighbourList = neighbourList->next;
    }

    return;
}

/*
    Function: uiButtonChooser_SetSegment

    Description -
    Sets the current active segment in the button chooser to be the
    segment as supplied.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser that the segment is in.
    Ui_BcSegment *segment - The segment to set active.
*/
void uiButtonChooser_SetSegment(Ui_ButtonChooser *bc, Ui_BCSegment *segment)
{
    Ui_ButtonScroll *scroll = NULL;
    struct list *segmentList = NULL;

    if(segment != NULL)
    {
        /*Disable the currently forced hover button*/
        uiButtonChooser_Choose(bc, 0);

        /*Change the current segment of the button chooser to this new segment*/
        segmentList = bc->buttonSegments;
        while(segmentList != NULL)
        {
            if(segmentList->data == segment)
            {
                bc->currentSegment = segmentList;

                break;
            }

            segmentList = segmentList->next;
        }

        /*Stop the control that would change the segment to any neighbours for a while*/
        uiButtonChooser_StopSegmentControls(segment);

        //printf("Segment active = %d\n", segment->active);

        switch(segment->type)
        {
            case BC_BUTTONLIST:
            default:

            /*Enable the currently selected button if the segment is active*/
            if(segment->active != 0)
                uiButtonChooser_Choose(bc, 1);

            break;

            case BC_SCROLL:
            scroll = segment->extraInfo->dataArray[0];

            segment->atButton = 0;

            segment->extraInfo->dataArray[1] = segment->currentButton = scroll->originButtonList;

            if(segment->active != 0)
                uiButtonChooser_Choose(bc, 1);

            break;
        }
    }

    return;
}


/*
    Function: uiButtonChooser_CheckChangeSegment

    Description -
    Checks if the current segment should be changed to a neighbour. If so
    then it changes it.

    2 arguments:
    Ui_ButtonChooser *bc - The button chooser that the segment is in.
    Ui_BCSegment *segment - The current segment of the button chooser.
*/
void uiButtonChooser_CheckChangeSegment(Ui_ButtonChooser *bc, Ui_BCSegment *segment)
{
    Ui_BCNeighbour *neighbour = NULL;

    struct list *neighbourList = NULL;
    struct list *next = NULL;

    /*Check all neighbouring segments to the current one.
    If the control to move to that neighbour is activated then move to it.*/
    neighbourList = segment->neighbourList;

    while(neighbourList != NULL)
    {
        next = neighbourList->next;

        neighbour = neighbourList->data;

        /*If the link to the neighbour should be removed*/
        if(neighbour->remove == 1)
        {
            /*Delete the neighbour link*/
            uiButtonChooserNeighbour_Clean(neighbour);
            mem_Free(neighbour);

            list_Delete_Node(&segment->neighbourList, neighbourList);
        }
        else if(control_IsActivated(&neighbour->c_Go) == 1) /*If the control to go to that segment is active*/
        {
            uiButtonChooser_SetSegment(bc, neighbour->segment);
        }

        neighbourList = next;
    }

    return;
}

void uiButtonChooser_UpdateType(Ui_ButtonChooser *bc)
{
    Ui_BCSegment *segment = bc->currentSegment->data;
    Ui_ButtonScroll *scroll = NULL;
    struct list *originButton = NULL;

    switch(segment->type)
    {
        case BC_BUTTONLIST:
        default:

        break;

        case BC_SCROLL:
        scroll = segment->extraInfo->dataArray[0];
        originButton = segment->extraInfo->dataArray[1];

        /*Has the scroll changed its state so that the segment no longer points to the correct button*/
        if(scroll->originButtonList != originButton)
        {
            if(segment->active != 0)
                uiButtonChooser_Choose(bc, 0);

            segment->atButton = 0;

            segment->extraInfo->dataArray[1] = segment->currentButton = scroll->originButtonList;
        }

        break;
    }

    return;
}

/*
    Function: uiButtonChooser_Update

    Description -
    Checks if the keys to move to the next button to choose have been pressed. If
    so then it changes the current button that is being chosen.

    1 argument:
    Ui_ButtonChooser *bc - The button chooser to update.
*/
void uiButtonChooser_Update(Ui_ButtonChooser *bc)
{
    Ui_BCSegment *segment = NULL;

    Ui_ButtonScroll *scroll = NULL;
    Ui_Button *button = NULL;

    bc->busy = 0;

    if(bc->buttonSegments == NULL || bc->currentSegment == NULL)
    {
        //printf("Warning buttonSegment = %p, currentSegment = %p\n", bc->buttonSegments, bc->currentSegment);
        return;
    }

    /*Obtain the current segment of buttons*/
    segment = bc->currentSegment->data;

    /*If the segment is not active, check if the enter control is active*/
    if(segment->active == 0)
    {
        /*Force off any hover functions for all of the buttons in this segment*/
        uiButtonChooser_ForceOffAllSegment(bc, 1);

        if(control_IsActivated(&segment->c_Enter) == 1)
        {
            segment->active = 2;

            /*Pause the segment a bit to allow another key press*/
            uiButtonChooser_StopControls(segment, 250);

            /*Choose the first button in the segment*/
            uiButtonChooser_Choose(bc, 1);

            if(segment->currentButton != NULL)
            {
                button = segment->currentButton->data;
                timer_StopSkip(&button->updateFreq, 250, 1);
            }
        }
    }
    else
    {
        /*Ensure compatibility*/
        uiButtonChooser_UpdateType(bc);

        if(segment->currentButton != NULL)
        {
            button = segment->currentButton->data;

            /*Check if that button is being forced active, if so then set all the other buttons
            in the segment list to being forced inactive*/
            if(button->forceHover == 1)
            {
                uiButtonChooser_ForceOffAllSegment(bc, 0);
            }
        }

        /*If the control to move forwards is pressed*/
        if(control_IsActivated(&segment->c_Forwards) == 1 && segment->currentButton != NULL)
        {
            uiButtonChooser_TravForwards(bc, segment);
        }

        /*If the control to move backwards if pressed*/
        if(control_IsActivated(&segment->c_Backwards) == 1 && segment->currentButton != NULL)
        {
            uiButtonChooser_TravBackwards(bc, segment);
        }

        /*If the control to exit out of the segment is pressed*/
        if(control_IsActivated(&segment->c_Exit) == 1)
        {
            segment->active = 0;
            uiButtonChooser_Choose(bc, 0);
        }

        if(segment->active == 2)
        {
            bc->busy = 1;
        }
    }

    /*If the segment is not being entered into*/
    if(segment->active < 2)
    {
        if(segment->sHover != NULL)
        {
            sprite_Draw(segment->sHover, ker_Screen());
        }

        /*check if the current segment should be changed, if so change it*/
        uiButtonChooser_CheckChangeSegment(bc, segment);
    }

    return;
}

/*
    Function: uiButtonChooser_Clean

    Description -
    Releases any memory created for the button chooser.

    1 argument:
    Ui_ButtonChooser *bc - The button chooser to clean.
*/
void uiButtonChooser_Clean(Ui_ButtonChooser *bc)
{
    while(bc->buttonSegments != NULL)
    {
        uiButtonChooserSegment_Clean(bc->buttonSegments->data);

        mem_Free(bc->buttonSegments->data);

        list_Pop(&bc->buttonSegments);
    }

    return;
}
