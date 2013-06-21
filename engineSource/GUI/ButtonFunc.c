#include "ButtonFunc.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Pixel.h"

/*Standard button functions*/

/*Draw a line to the left of the button*/
void BFunc_HoverLine(Ui_Button *button)
{
    draw_Line(button->rect.x - 15, button->rect.y + 5, button->rect.x - 15, button->rect.y + 20, 4, 0, &colourWhite, ker_Screen());

    return;
}

/*Draw a box around the button*/
void BFunc_HoverBox(Ui_Button *button)
{
    draw_Box(button->rect.x, button->rect.y, button->rect.w - 1, button->rect.h -1, 1, &colourWhite, ker_Screen());

    return;
}

/*Give the button basic input*/
void BFuncSet_BasicControls(Ui_Button *button)
{
    //control_AddKey(&button->cActivate[BC_POINTER], C_MOUSE, 1, C_MOUSELEFT);
    control_AddKey(&button->cActivate[BC_KEY], C_KEYBOARD, 1,SDLK_RETURN);

    #ifdef GP2X
    control_AddKey(&button->cActivate[BC_KEY], C_JOYSTICK, 1, GP2X_BUTTON_B);
    #endif

	#ifdef DINGOO
    control_AddKey(&button->cActivate[BC_KEY], C_KEYBOARD, 1, DINKEY_A);
	#endif

    return;
}

/*Toggle value of int between 1 and 0*/
void BFunc_BoolInt(Ui_Button *button)
{
    int *boolean = button->info->dataArray[0];

    if(*boolean == 0)
        *boolean = 1;
    else
        *boolean = 0;

    return;
}

/*Rotate value of int*/
void BFunc_RotateInt(Ui_Button *button)
{
    int *intVar = button->info->dataArray[0];
    int rotateTo = (int)button->info->dataArray[1];
    int direction = (int)button->info->dataArray[2];

    if(direction == 0)
    {
        *intVar = (*intVar + 1) % rotateTo;
    }
    else
    {
        if(*intVar == 0)
            *intVar = rotateTo - 1;
        else
            *intVar = *intVar - 1;
    }

    return;
}

/*Move the value of int by +1 or -1*/
void BFunc_MoveInt(Ui_Button *button)
{
    int *intVar = (int *)button->info->dataArray[0];
    int limit = (int)button->info->dataArray[1];
    int direction = (int)button->info->dataArray[2];

    if(button->info->amount < 3)
        printf("Error %d values in data array for BFunc_MoveInt\n", button->info->amount);

    if(*intVar == limit)
        return;

    if(direction == 0)
    {
        *intVar = *intVar + 1;
    }
    else
    {
        *intVar = *intVar - 1;
    }

    return;
}

/*Change value of int by specific amount*/
void BFunc_ChangeInt(Ui_Button *button)
{
    int *intVar = button->info->dataArray[0];
    int changeBy = (int)button->info->dataArray[1];

    *intVar = *intVar + changeBy;

    return;
}

/*Set the value of int to a specific amount*/
void BFunc_SetInt(Ui_Button *button)
{
    int *intSet = button->info->dataArray[0];
    int setTo = (int)button->info->dataArray[1];

    *intSet = setTo;

    return;
}

/*Toggle a flag in a variable between on and off*/
void BFunc_ToggleFlag(Ui_Button *button)
{
    unsigned int *flags = (unsigned int *)button->info->dataArray[0];
    unsigned int toggleFlag = (unsigned int)button->info->dataArray[1];

    flag_Toggle(flags, toggleFlag);

    return;
}

/*Sets a char pointer to point to a specific string*/
void BFunc_SetString(Ui_Button *button)
{
    char *string = (char *)button->info->dataArray[0];
    char **charPointer = (char **)button->info->dataArray[1];

    *charPointer = string;

    return;
}
