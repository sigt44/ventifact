#include "main.h"
#include "./Graphics/Surface.h"
#include "Controls.h"
#include "./Graphics/Graphics.h"
#include "./Graphics/Surface.h"
#include "./Graphics/Sprite.h"
#include "./Graphics/Animation.h"
#include "File.h"
#include "Font.h"
#include "./Common/ListMM.h"
#include "./Time/Timer.h"
#include "./Time/Fps.h"
#include "Gamestate.h"
#include "Controls.h"
#include "Vector2D.h"
#include "./Kernel/Kernel.h"
#include <string.h>
#include <math.h>
#include "Maths.h"
#include "keyTest.h"
#include "spinning.h"

	    struct gamestate pe_Test_State;
        struct gamestate KT_State;
        struct gamestate SP_State;

struct pe_Tester
{
    int frameCap;
    float framerate;
    float totalFrames;
    float totalNoFrames;

    struct timer gameTimer;
    struct timer gameTimer2;
    int daysPassed;
    int numberSort[10];
    struct list *numberSortList;

    SDL_Surface *sea[4];
    struct sprite seaSprite;

    struct control_Event c_Quit;
    struct control_Event c_HelloWorld;
    struct control_Event c_ToKeyTest;
    struct control_Event c_ToSpinner;

};

void pe_Init(void *info)
{
    struct gamestate *gState = info;
    struct pe_Tester *peTest = (struct pe_Tester *)mem_Malloc(sizeof(struct pe_Tester),__LINE__,__FILE__);
    int x;
    puts("INIT PE");
    gState->info = peTest;
    gState->callInit = 0;

    peTest->totalFrames = 0.00f;
    peTest->totalNoFrames = 0.00f;
    peTest->frameCap = 0;
    peTest->framerate = 0;

    /*peTest->background = surf_Create(SDL_SWSURFACE,ker_Screen_Width(),ker_Screen_Height());
    fill_Rect(0,0,ker_Screen_Width(),ker_Screen_Height(),&colourBlack,peTest->background);*/

    for(x = 0; x < 10; x++)
    {
        peTest->numberSort[x] = rand() % 100;
    }
    peTest->numberSortList = NULL;
    for(x = 0; x < 2; x++)
        list_Push_SortL_MM(&peTest->numberSortList,&peTest->numberSort[x],0,0);


    surf_Load(&peTest->sea[0],"Sea_F1.png",0);
    surf_Load(&peTest->sea[1],"Sea_F2.png",0);
    surf_Load(&peTest->sea[2],"Sea_F3.png",0);
    surf_Load(&peTest->sea[3],"Sea_F4.png",0);

    peTest->gameTimer = timer_Setup(NULL,0,0,1);
    timer_Calc(&peTest->gameTimer);
    file_Log(ker_Log(),1,"\n Timer.base(SDL_GetTicks()) %d, Timer.current %d\n",peTest->gameTimer.base_Time,peTest->gameTimer.current_Time);
    peTest->gameTimer2 = timer_Setup(&peTest->gameTimer,0,24000,1);

    peTest->c_HelloWorld = control_Setup("Hello World",&peTest->gameTimer,1000);
    control_AddKey(&peTest->c_HelloWorld,C_KEYBOARD,1,SDLK_p);
    control_AddKey(&peTest->c_HelloWorld,C_MOUSE,1,C_MOUSELEFT);

    peTest->c_Quit = control_Setup("Quit",&peTest->gameTimer,0);
    control_AddKey(&peTest->c_Quit,C_KEYBOARD,1,SDLK_ESCAPE);

    peTest->c_ToKeyTest = control_Setup("To KeyTest",&peTest->gameTimer,0);
    control_AddKey(&peTest->c_ToKeyTest,C_KEYBOARD,1,SDLK_RETURN);

    peTest->c_ToSpinner = control_Setup("To Spinner",&peTest->gameTimer,0);
    control_AddKey(&peTest->c_ToSpinner,C_KEYBOARD,1,SDLK_s);

    #ifdef GP2X
    control_AddKey(&peTest->c_ToKeyTest,C_JOYSTICK,1,GP2X_BUTTON_B);
    control_AddKey(&peTest->c_ToSpinner,C_JOYSTICK,1,GP2X_BUTTON_A);
    control_AddKey(&peTest->c_Quit,C_JOYSTICK,1,GP2X_BUTTON_L);
    #endif

    sprite_Setup(&peTest->seaSprite,0,&peTest->gameTimer,16,
        frame_Create(270,peTest->sea[0]),
        frame_Create(150,peTest->sea[1]),
        frame_Create(220,peTest->sea[2]),
        frame_Create(150,peTest->sea[3]),
        frame_Create(270,peTest->sea[2]),
        frame_Create(180,peTest->sea[0]),
        frame_Create(200,peTest->sea[1]),
        frame_Create(150,peTest->sea[2]),
        frame_Create(250,peTest->sea[0]),
        frame_Create(150,peTest->sea[1]),
        frame_Create(220,peTest->sea[2]),
        frame_Create(150,peTest->sea[1]),
        frame_Create(270,peTest->sea[3]),
        frame_Create(180,peTest->sea[0]),
        frame_Create(300,peTest->sea[2]),
        frame_Create(150,peTest->sea[0]));

    frame_Report(peTest->seaSprite.frameList);

    peTest->daysPassed = 0;

    return;
}

void pe_RenderPause(void *info)
{

    fill_Rect(0,0,ker_Screen_Width(),ker_Screen_Height(),&colourBlack,ker_Screen());

    text_Draw_Arg(ker_Screen_Width()/2 - 50,ker_Screen_Height()/2,ker_Screen(),fontArial[LARGE],&tColourWhite,0,"Paused");

    SDL_Flip(ker_Screen());

    return;
}

void pe_RenderO(void *info)
{
    struct gamestate *gState = info;
    struct pe_Tester *peTest = gState->info;
    struct timer renderTime = timer_Setup(NULL,0,0,1);

    struct vector vClockCentre = {ker_Screen_Width()/2,ker_Screen_Height()/2};
    struct vector vClockHand;
    float clockPoints = (2 * PI) / 24;

    //surf_Blit(0,0,peTest->background,ker_Screen(),NULL);
    fill_Rect(0,0,ker_Screen_Width(),ker_Screen_Height(),&colourBlack,ker_Screen());

    sprite_DrawAtPos(25, 100,&peTest->seaSprite,ker_Screen());

    //vClockHand.x = (cos((3.14159265 * 2)/(peTest->gameTimer2.current_Time/1000 + 1)) * 50) + vClockCentre.x;
    vClockHand.x = (-cos(clockPoints * (int)(peTest->gameTimer2.current_Time/1000) ) * 50) + vClockCentre.x;
    //vClockHand.y = (sin((3.14159265 * 2)/(peTest->gameTimer2.current_Time/1000 + 1)) * 50) + vClockCentre.y;
    vClockHand.y = (-sin(clockPoints * (int)(peTest->gameTimer2.current_Time/1000) ) * 50) + vClockCentre.y;
    draw_Line_Vector(&vClockCentre,&vClockHand,5,1,&colourLightGreen,ker_Screen());
    text_Draw(10,20,"Timer test",ker_Screen(),fontArial[MEDIUM],&tColourWhite,0);

    text_Draw_Arg(ker_Screen_Width()- 50,60,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0," %f", peTest->framerate);
    text_Draw_Arg(ker_Screen_Width() - 50,80,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"%d", renderTime.start_Time);

    text_Draw_Arg(ker_Screen_Width()/2 - 150,140,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T1 Base %d", peTest->gameTimer.base_Time);
    text_Draw_Arg(ker_Screen_Width()/2 - 150,160,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T1 Current %d", peTest->gameTimer.current_Time);
    text_Draw_Arg(ker_Screen_Width()/2,140,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T2 base %d", peTest->gameTimer2.base_Time);
    text_Draw_Arg(ker_Screen_Width()/2,160,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T2 start %d", peTest->gameTimer2.start_Time);
    text_Draw_Arg(ker_Screen_Width()/2,180,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T2 end %d", peTest->gameTimer2.end_Time);
    text_Draw_Arg(ker_Screen_Width()/2,200,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T2 current %d", peTest->gameTimer2.current_Time);
    text_Draw_Arg(ker_Screen_Width()/2,220,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T2 remaining %d", timer_Get_Remain(&peTest->gameTimer2));
    text_Draw_Arg(ker_Screen_Width()/2,240,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"T2 time stopped %d", peTest->gameTimer2.stop_Time);
    text_Draw_Arg(25,60,ker_Screen(),fontArial[MEDIUM],&tColourWhite,0,"Hours: %d   Days passed: %d", peTest->gameTimer2.current_Time/1000 + 1, peTest->daysPassed);

    if(control_IsActivated(&peTest->c_HelloWorld))
        peTest->daysPassed += 1;

    if(timer_Get_Remain(&peTest->gameTimer2) < 1)
    {
        timer_Start(&peTest->gameTimer2);
        peTest->daysPassed += 1;
    }
    timer_Calc(&peTest->gameTimer);
    timer_Calc(&peTest->gameTimer2);

    SDL_Flip(ker_Screen());

    if(peTest->frameCap)
        fps_Cap(&renderTime,16);
    peTest->framerate = fps_Calc_Now(&renderTime);
    peTest->totalFrames += peTest->framerate;
    peTest->totalNoFrames += 1;

    /*if(SDL_GetTicks() >= 10000)
        gamestate_CallQuit(gState);*/
    return;
}

void pe_Pause(void *info)
{
    struct gamestate *gs = info;
    struct pe_Tester *peTest = gamestate_Data(info);

    timer_Pause(&peTest->gameTimer);
    gs->render = pe_RenderPause;

    return;
}

void pe_Resume(void *info)
{
    struct gamestate *gs = info;
    struct pe_Tester *peTest = gamestate_Data(info);

    timer_Resume(&peTest->gameTimer);
    gs->render = pe_RenderO;

    return;
}

void pe_Controls(void *info)
{
    struct gamestate *gState = info;
    struct pe_Tester *peTest = gState->info;

    //if(!SDL_PollEvent(&cEvent))
       // return;

    SDL_PumpEvents();

    if(control_IsActivated(&peTest->c_Quit))
        gamestate_CallQuit(gState);
    else if(control_IsActivated(&peTest->c_ToKeyTest))
    {
        //gamestate_Pause(gState);

        gamestate_Push(ker_GamestateList(),gamestate_CreateFrom(&KT_State));
    }
    else if(control_IsActivated(&peTest->c_ToSpinner))
    {
        //gamestate_Pause(gState);

        gamestate_Push(ker_GamestateList(),gamestate_CreateFrom(&SP_State));
    }
    /*else if (control_Keystate()[ SDLK_f ])
        peTest->frameCap = 1;
    else if (control_Keystate()[ SDLK_g ])
        peTest->frameCap = 0;
    else if (control_Keystate()[ SDLK_h ])
    {
        timer_Stop(&peTest->gameTimer,5000);
        SDL_Delay(200);
    }
    else if (control_Keystate()[ SDLK_u ])
    {
        timer_Calc(&peTest->gameTimer);
        SDL_Delay(200);
    }
    else if (control_Keystate()[ SDLK_j ])
        timer_Pause(&peTest->gameTimer);
    else if (control_Keystate()[ SDLK_k ])
        timer_Resume(&peTest->gameTimer);
    else if (control_Keystate()[ SDLK_b ])
    {
        timer_Stop(&peTest->gameTimer2,5000);
        SDL_Delay(200);
    }
    else if (control_Keystate()[ SDLK_n ])
        timer_Pause(&peTest->gameTimer2);
    else if (control_Keystate()[ SDLK_m ])
        timer_Resume(&peTest->gameTimer2);
    else if (control_Keystate()[ SDLK_x ])
        gamestate_Resume(info);
    else if (control_Keystate()[ SDLK_z ])
        gamestate_Pause(info);*/
    return;
}

void pe_Exit(void *info)
{
    struct gamestate *gState = info;
    struct pe_Tester *peTest = gState->info;

    frame_Clean(&peTest->seaSprite.frameList);
    control_Clear(&peTest->c_Quit);
    control_Clear(&peTest->c_HelloWorld);
    control_Clear(&peTest->c_ToKeyTest);
    control_Clear(&peTest->c_ToSpinner);
    fill_Rect(0,0,ker_Screen_Width(),ker_Screen_Height(),&colourBlack,ker_Screen());
    text_Draw_Arg(ker_Screen_Width() - 50,60,ker_Screen(),fontArial[MEDIUM],&tColourWhite,1,"%f",peTest->totalFrames);
    text_Draw_Arg(ker_Screen_Width() - 50,80,ker_Screen(),fontArial[MEDIUM],&tColourWhite,1,"%f",peTest->totalFrames/peTest->totalNoFrames);
    text_Draw(10,20,"Timer test",ker_Screen(),fontArial[MEDIUM],&tColourWhite,1);
    text_Draw(15,40,"Quitting",ker_Screen(),fontArial[MEDIUM],&tColourWhite,1);
    printf("Average Framerate %f\n",peTest->totalFrames/peTest->totalNoFrames);

    SDL_FreeSurface(peTest->sea[0]);
    SDL_FreeSurface(peTest->sea[1]);
    SDL_FreeSurface(peTest->sea[2]);
    SDL_FreeSurface(peTest->sea[3]);

    list_Clear_MM(&peTest->numberSortList);
    SDL_Delay(200);
    mem_Free(peTest);

    return;
}

int main(int argc, char *argv[])
{
	printf("Engine\n");
	printf("\tStart\n");

	if(kernel_Init("Engine Test"))
	{
	    SDL_Quit();
        return 1;
	}
	else
	{
		gamestate_Setup(&pe_Test_State,pe_Init,pe_Controls,NULL,pe_RenderO,pe_Pause,pe_Resume,pe_Exit,&pe_Test_State);
		gamestate_Setup(&KT_State,KT_Init,KT_Controls,NULL,KT_Render,NULL,NULL,KT_Exit,&KT_State);
		gamestate_Setup(&SP_State,SP_Init,SP_Controls,NULL,SP_Render,NULL,NULL,SP_Exit,&SP_State);
		gamestate_Push(ker_GamestateList(),&pe_Test_State);
		//gamestate_Push(ker_GamestateList(),&KT_State);

        /*Main loop*/
		while(!gamestate_Process(ker_Gamestate()) && *ker_GamestateList() != NULL)
		{
            gamestate_Process_List(ker_GamestateList());
		}

		kernel_Quit();
	}
	return 0;
}
