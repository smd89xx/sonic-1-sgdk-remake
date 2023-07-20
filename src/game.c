#include "includes.h"

enum maxVals {ringMax = 999, livesMax = 99, scoreMax = 9999990};

Map* lvlBG;
Map* lvlFG;
unsigned char level[2] = {0,0};
unsigned char lives = 3;
unsigned short rings = 0;
unsigned char timeTimer = 0;
unsigned char gameTimer[2] = {0,0};
unsigned long score = 0;

static void spawnPlayer()
{

}

static void drawLevel()
{
    switch (level[0])
    {
    case 0:
    {
        switch (level[1])
        {
        case 0:
        {
            break;
        }
        default:
        {
            break;
        }
        break;
        }
    }
    default:
    {
        break;
    }
    }
}

static void joyEvent_Game(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
}

void gameInit()
{
    VDP_clearPlane(BG_A,TRUE);
    VDP_clearPlane(BG_B,TRUE);
    SPR_reset();
    unsigned char timer = 200;
    JOY_setEventHandler(joyEvent_Game);
    while(1)
    {
        MDS_update();
        SPR_update();
        SYS_doVBlankProcess();
        if (timer == 0)
        {
            spawnPlayer();
            drawLevel();
            break;
        }
    }
}