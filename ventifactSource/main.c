#include "BaseState.h"
#include "Kernel/Kernel.h"
#include "Kernel/Kernel_State.h"
#include "Sound.h"

#include "Menus/MenuAttributes.h"
#include "Menus/MainMenu.h"
#include "Images.h"
#include "Options.h"
#include "ControlMap.h"
#include "Units/Attributes.h"
#include "Config.h"
#include "Sounds.h"

static int vDirectory_Init(void)
{
    if(kernel_CheckPath("PTH_VentLevels") == 0)
        kernel_AddPath("VentLevels", "../Levels/");
    if(kernel_CheckPath("PTH_VentCampaigns") == 0)
        kernel_AddPath("VentCampaigns", "../Campaigns/");
    if(kernel_CheckPath("PTH_VentConfig") == 0)
        kernel_AddPath("VentConfig", "../config.cfg");
    if(kernel_CheckPath("PTH_VentSaves") == 0)
        kernel_AddPath("VentSaves", "../Saves/");
    if(kernel_CheckPath("PTH_VentScores") == 0)
        kernel_AddPath("VentScores", "../Scores/");
    if(kernel_CheckPath("PTH_VentCustomLevels") == 0)
        kernel_AddPath("VentCustomLevels", "../");
    if(kernel_CheckPath("PTH_VentCustomTextures") == 0)
        kernel_AddPath("VentCustomTextures", "../");

    return 0;
}

static int vInit(void)
{
    /*Setup the correct directories*/
    vDirectory_Init();

    /*Load in the images*/
    image_Init();

    /*Load the sounds*/
    vSounds_Load();

    /*Setup some default sprites*/
    vSprite_Init();

    /*Setup controls*/
    vControls_Init();

    /*Try and load the config file*/
    if(vConfig_Load() == 0) /*If it fails to load, setup defaults*/
    {
        /*Set default options*/
        options_SetDefaults();

        /*Setup the default controls*/
        vControls_SetDefaults();
    }

    /*Setup the default attributes of a menu*/
    menuAttributes_Setup();

    /*Setup attributes for unit upgrades*/
    vAttributes_SetupDefaults();

    /*Set the framerate to 30fps*/
    gKer_SetFPS(30);

    return 0;
}

static void vClean(void)
{
    /*Clean up*/
    menuAttributes_Clean();

    vSprite_Clean();

    vControls_Clean();

    return;
}

int main( int argc, char *argv[] )
{
    Base_State mainState;
    int init = 0;
	Data_Struct *testStruct = NULL;

    /*Load up the base SDL engine*/
    init = kernel_Init("Ventifact");

    if(init != 0)
    {
        printf("Failed to load engine. %d", init);
        return init;
    }
    else
	{
	    if((init = vInit()) != 0)
        {
            printf("Error could not initialise ventifact [%d]\n", init);
            return init;
        }

        /*Setup and push in the main menu state*/
        baseState_Setup(&mainState, "Main menu", Menu_Main_Init, Menu_Main_Controls, Menu_Main_Logic, Menu_Main_Render, Menu_Main_Exit, &mainState);

        baseState_Push(ker_BaseStateList(), &mainState);

        /*Start the main loop of the program*/
        kernel_State_Loop();

        vClean();

        kernel_Quit();
	}

    return 0;
}


