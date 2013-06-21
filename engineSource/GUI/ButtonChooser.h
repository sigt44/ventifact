#ifndef BUTTONCHOOSER_H
#define BUTTONCHOOSER_H

#include "../Common/List.h"
#include "../Common/ListWatcher.h"
#include "../Controls.h"
#include "ButtonScroll.h"

enum
{
    BC_FORWARDS = 0,
    BC_BACKWARDS = 1,
    BC_NEXT = 2,
    BC_PREV = 3

} BUTTONCHOOSER_DIRECTIONS;

enum
{
    BC_BUTTON = 0,
    BC_BUTTONLIST = 1,
    BC_SCROLL = 2

} BUTTONCHOOSER_TYPES;

typedef struct Ui_ButtonChooserSegment
{
    const char *name; /*Name of the segment, used to find it.*/

    struct list **buttonList; /*Points to the head of a list of buttons*/

    int atButton; /*Keeps track of how far along the current button is from the starting one*/

    struct list *currentButton; /*Points to the button that is being chosen, will force button->hover on*/

    int rotate; /*Should the chooser rotate back to start/end?*/

    int active; /*Is the segment active or not*/
    Control_Event c_Enter; /*Enter into the segment and make it active*/
    Control_Event c_Exit;   /*Exit out of the segment and make it inactive*/

    Control_Event c_Forwards; /*Move the current button forwards in the list*/
    Control_Event c_Backwards; /*Move the current button backwards in the list*/

    struct list *neighbourList; /*Holds list of neighbouring segments from this one*/
    struct list *linkedList;    /*Holds list of segment(neighbour structures) that have their neighbour set as this segment*/

    int type; /*Used to keep compatibility with sepecific type such as button scrolls*/
    Data_Struct *extraInfo; /*Points to extra data needed for specific segment type*/

    Sprite *sHover; /*Used to display a sprite when the segment is being hovered over*/

} Ui_BCSegment;

typedef struct Ui_ButtonChooserNeighbour
{
    Ui_BCSegment *segment;

    int remove; /*If true then this neighbour structure should be deleted*/

    Control_Event c_Go;

    Dependency_Watcher dW;

} Ui_BCNeighbour;

typedef struct Ui_ButtonChooser
{
    struct list *buttonSegments; /*Holds the address of the heads of lists of buttons*/
    struct list *currentSegment; /*Points to the current button segment.*/

    Timer *srcTime; /*Source time to use for the control events in the chooser*/

    int busy; /*1 if the chooser is busy*/

} Ui_ButtonChooser;

Ui_BCSegment *uiButtonChooserSegment_Create(struct list **buttonListHead,
                                            const char *name, Timer *srcTimer,
                                            int changeTime,
                                            Control_Event *copyForward, Control_Event *copyBackward,
                                            Control_Event *copyEnter, Control_Event *copyExit,
                                            int type, void *extraInfo,
                                            Sprite *hoverSprite);

void uiButtonChooserNeighbour_Clean(Ui_BCNeighbour *neighbour);

void uiButtonChooser_AddNeighbour(Ui_ButtonChooser *bc, Ui_BCSegment *baseSegment, Ui_BCSegment *neighbourSegment, int changeTime, Control_Event *copyEvent);

void uiButtonChooser_AddNeighbourBoth(Ui_ButtonChooser *bc, Ui_BCSegment *baseSegment, Ui_BCSegment *neighbourSegment, int changeTime, Control_Event *copyEventBase, Control_Event *copyEventNeighbour);


void uiButtonChooser_StopSegmentControls(Ui_BCSegment *segment);

void uiButtonChooser_SetSegmentName(Ui_ButtonChooser *bc, const char *segmentName);

void uiButtonChooser_SetSegment(Ui_ButtonChooser *bc, Ui_BCSegment *segment);

Ui_BCSegment *uiButtonChooser_GetSegment(Ui_ButtonChooser *bc, const char *name);

int uiButtonChooser_RemoveSegment(Ui_ButtonChooser *bc, const char *name);

void uiButtonChooser_ForceOffSegment(Ui_BCSegment *segment, int ignoreCurrent);

void uiButtonChooser_CheckChangeSegment(Ui_ButtonChooser *bc, Ui_BCSegment *segment);


void uiButtonChooser_Setup(Ui_ButtonChooser *bc, Timer *srcTimer);

void uiButtonChooser_AddButtons(Ui_ButtonChooser *bc,
                               const char *name,
                               void *buttonObj,
                               int type,
                               Sprite *hoverSprite,
                               int changeTime,
                               Control_Event *copyForward, Control_Event *copyBackward,
                               Control_Event *copyEnter, Control_Event *copyExit);

void uiButtonChooser_Choose(Ui_ButtonChooser *bc, int activate);

void uiButtonChooser_Update(Ui_ButtonChooser *bc);

void uiButtonChooser_Clean(Ui_ButtonChooser *bc);



#endif
