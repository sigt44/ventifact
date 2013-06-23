#include "Config.h"

#include "Options.h"
#include "ControlMap.h"

void vConfig_Save(void)
{
    struct list *controlList = ve_Controls.controlList;

    FILE *save = fopen(kernel_GetPath("PTH_VentConfig"), "w");

    if(save == NULL)
    {
        printf("Error cannot save config file\n");

        return;
    }

    /*Save options*/
    file_Log(save, 0, "%d %d %d %d %d %s\n",
             ve_Options.mute,
             ve_Options.volume,
             ve_Options.teamAi,
             ve_Options.difficulty,
             ve_Options.cheat,
             ve_Options.lastSave
    );

    /*Save controls*/
    file_Log(save, 0, "%d\n", ve_Controls.numControls);

    while(controlList != NULL)
    {
        control_Save(controlList->data, save);
        controlList = controlList->next;
    }

    return;
}

int vConfig_Load(void)
{
    int numLoad = 0;
    char controlName[255];
    Control_Event *controlFind = NULL;
    FILE *load = fopen(kernel_GetPath("PTH_VentConfig"), "r");

    if(load == NULL)
    {
        return 0;
    }

    /*Load options*/
    fscanf(load, "%d %d %d %d %d %s\n",
             &ve_Options.mute,
             &ve_Options.volume,
             &ve_Options.teamAi,
             &ve_Options.difficulty,
             &ve_Options.cheat,
             ve_Options.lastSave
    );

    /*Load controls*/
    fscanf(load, "%d\n", &numLoad);

    for(numLoad = numLoad; numLoad > 0; numLoad --)
    {
        control_LoadName(controlName, load);

        controlFind = vControl_Find(controlName);
        if(controlFind == NULL)
        {
            file_Log(ker_Log(), 1, "Error: Cannot load control with name %s\n", controlName);
            return 0;
        }
        else
        {
            control_Load(controlFind, load);
        }
    }

    return 1;
}
