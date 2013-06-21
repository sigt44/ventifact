#include "ShortcutMenu.h"

#include "LevelEditor.h"
#include "../Menus/MenuAttributes.h"
#include "../ControlMap.h"


void vLE_SetupShortcutMenu(Vent_Level_Editor *editor)
{
    Ui_Spine *shortcutSpine = NULL;
    SDL_Surface *background = NULL;
	static char testShortText[32] = {"Save Level"};

	/*Control_Event cTileObjects;
    Control_Event cGridObjects;
    Control_Event cNudgePointer;
    Control_Event cRotateObject;
    Control_Event cEditorSave;
    Control_Event cEditorHelp;*/

    /*Setup the main window*/
    background = surf_SimpleBox(500, 300, &ve_Menu.colourBackgroundBasic, &colourBlack, 1);

    /*Add in the title text*/
    text_Draw_Arg(20, 15, background, font_Get(2, 16), &tColourBlack, editor->spine.layer + 1, 0, "Shortcuts:");

    shortcutSpine = uiSpine_Create(editor->spine.layer + 2, (ker_Screen_Width()/2) - (background->w/2), (ker_Screen_Height()/2) - (background->h/2), frame_CreateBasic(0, background, A_FREE), editor->spine.pnt, 0, NULL, UI_SPINE_ALLFLAGS - UI_SPINE_UPDATEPOINTER);

    uiSpine_AddSpine(&editor->spine, shortcutSpine, 0, "Spine:Shortcuts");

    /*Add in the text boxes for each control shortcut to display*/
    control_GenerateNameList(&ve_Controls.cEditorSave, editor->shortcutValues[0], 127);
    control_GenerateNameList(&ve_Controls.cRotateObject, editor->shortcutValues[1], 127);
    control_GenerateNameList(&ve_Controls.cNudgePointer, editor->shortcutValues[2], 127);
    control_GenerateNameList(&ve_Controls.cGridObjects, editor->shortcutValues[3], 127);
    control_GenerateNameList(&ve_Controls.cTileObjects, editor->shortcutValues[4], 127);

    uiSpine_AddTextBox(shortcutSpine,
       uiTextBox_CreateBase(25, 50, shortcutSpine->layer, 0, NULL, font_Get(2, 13), tColourBlack, &shortcutSpine->sTimer),
       "TextBox:ShortcutSave");

    uiTextBox_AddText(uiSpine_GetEntity(shortcutSpine,"TextBox:ShortcutSave"),
                     0, 0, "%a: %a\n%a: %a\n%a: %a\n%a: %a\n%a: %a", dataStruct_CreateType("pppppppppp", "Save level", editor->shortcutValues[0], "Rotate object", editor->shortcutValues[1], "Nudge Pointer", editor->shortcutValues[2], "Set grid (Hold)", editor->shortcutValues[3], "Tile object (Hold)", editor->shortcutValues[4]));
	

    return;
}
