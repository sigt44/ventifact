#ifndef UI_DEFINES_H
#define UI_DEFINES_H

enum
{
    UIE_BUTTON = 0,
    UIE_SCROLL = 1,
    UIE_BUTTONCHAIN = 2,
    UIE_SPINE = 3,
    UIE_TEXTBOX = 4,
    UIE_SPRITE = 5,
    UIE_TEXTINPUT = 6,
    UIE_TOTALTYPES = 7
} UI_ENTITY_TYPES;

enum UI_DIRECTION {
    UI_UP = 0,
    UI_DOWN = 1,
    UI_LEFT = 2,
    UI_RIGHT = 3,
    UI_UPRIGHT = 4,
    UI_UPLEFT = 5,
    UI_DOWNRIGHT = 6,
    UI_DOWNLEFT = 7,
    UI_BASEDIRECTIONS = 8,
    UI_ENTER = 8,
    UI_EXIT = 9,
    UI_DIRECTIONS = 10,
};

enum SCROLL_DIRECTIONS
{
    SCR_N = -1, /*For no direction scrolling, leaving original positions along*/
    SCR_V = 0, /*For verticle scrolling*/
    SCR_H = 1 /*Horizontal scrolling*/
};

#endif
