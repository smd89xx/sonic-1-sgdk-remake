#include "includes.h"

enum maxVals {ringMax = 999, livesMax = 99, scoreMax = 9999990};

Map* lvlBG;
Map* lvlFG;
unsigned char level[2] = {0,0};
unsigned char lives = 3;
unsigned short rings = 0;
unsigned char timeTimer = 0;
unsigned char gameTimer[3] = {0,0,0};
unsigned long score = 0;
unsigned short player_x = 0;
unsigned short player_y = 0;
short lvlVRAMIndex;
Sprite* player;
bool startGameTimer = true;
bool paused = false;

static void spawnPlayer()
{
    u16 basetile = TILE_ATTR(PAL3,FALSE,FALSE,FALSE);
    player = SPR_addSprite(&plrSonic,player_x,player_y,basetile);
}

static void setLevelPalette(const u16* palette)
{
    PAL_fadeIn(0,47,palette,palFadeTime,TRUE);
}

static void joyEvent_Paused(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (changed & state & BUTTON_A)
    {
        u8 timer = palFadeTime;
        PAL_fadeOutAll(palFadeTime,true);
        while(1)
        {
            timer--;
            SYS_doVBlankProcess();
            MDS_update();
            if (timer == 0)
            {
                SYS_hardReset();
            }
        }
    }
    if (changed & state & BUTTON_START)
    {
        MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
        paused = false;
        MDS_pause(MDS_BGM,paused);
        JOY_setEventHandler(joyEvent_Game);
    }
}

static void joyEvent_Game(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (changed & state & BUTTON_A)
    {
        MDS_request(MDS_SE1,BGM_SFX_S1RING);
        rings++;
        if ((rings % 100) == 0 && rings > 1)
        {
            lives++;
            MDS_request(MDS_SE2,BGM_SFX_SCD1UP);
        }
    }
    else if (changed & state & BUTTON_B)
    {
        score += 10000;
        if ((score % 50000) == 0)
        {
            lives++;
            MDS_request(MDS_SE2,BGM_SFX_SCD1UP);
        }
    }
    if (changed & state & BUTTON_START)
    {
        MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
        paused = true;
        MDS_pause(MDS_BGM,paused);
        JOY_setEventHandler(joyEvent_Paused);
    }
}

static void paletteCycle_GHZ()
{
	*cycleTimer += FIX16(0.150);
	if (*cycleTimer >= FIX16(4))
	{
		*cycleTimer = FIX16(0);
	}
	PAL_setColors(24,titleWater[fix16ToInt(*cycleTimer)],4,DMA);
}

static void updateHUD()
{
    char scoreStr[8] = "0000000";
    char ringsStr[4] = "000";
    char secondStr[3] = "00";
    char minuteStr[2] = "0";
    char livesStr[3] = "00";
    char framesStr[3] = "00";
    uintToStr(score,scoreStr,7);
    VDP_drawText(scoreStr,6,0);
    uintToStr(rings,ringsStr,3);
    VDP_drawText(ringsStr,10,2);
    uintToStr(gameTimer[0],minuteStr,1);
    VDP_drawText(minuteStr,6,1);
    uintToStr(gameTimer[1],secondStr,2);
    VDP_drawText(secondStr,8,1);
    uintToStr(lives,livesStr,2);
    VDP_drawText(livesStr,38,1);
    uintToStr(gameTimer[2],framesStr,2);
    VDP_drawText(framesStr,11,1);
}

static void spawnHUD()
{
    u16 basetileHUD_Image = TILE_ATTR_FULL(PAL3,TRUE,FALSE,FALSE,lvlVRAMIndex+TILE_USER_INDEX);
    VDP_drawText("SCORE",0,0);
    VDP_drawText("TIME",0,1);
    VDP_drawText("RINGS",0,2);
    VDP_drawText(":",7,1);
    VDP_drawText(":",10,1);
    VDP_drawImageEx(WINDOW,&livesHUD,basetileHUD_Image,34,0,FALSE,TRUE);
}

static void updateTimer()
{
    u8 maxFrame = 60;
    if (!isNTSC)
    {
        maxFrame = 50;
    }
    if (startGameTimer)
    {
        gameTimer[2]++;
    }
    if (gameTimer[2] == maxFrame)
    {
        gameTimer[2] = 0;
        gameTimer[1]++;
    }
    else if (gameTimer[1] == 60)
    {
        gameTimer[0]++;
        gameTimer[1] = 0;
    }
    else if (gameTimer[0] == 10)
    {
        startGameTimer = false;
        gameTimer[0] = 9;
        gameTimer[1] = 59;
        gameTimer[2] = maxFrame-1;
        VDP_drawText(":",7,1);
    }
}

static void initLevel()
{
    switch (level[0])
    {
        case 0:
        {
            setLevelPalette(ghzPalette);
            break;
        }
        default:
        {
            break;
        }
    }
    spawnPlayer();
    spawnHUD();
    JOY_setEventHandler(joyEvent_Game);
    while (1)
    {
        SYS_doVBlankProcess();
        MDS_update();
        if (!paused)
        {
            SPR_update();
            updateHUD();
            updateTimer();
            if (level[0] == 0)
            {
                paletteCycle_GHZ();
            }
        }
    }
}

void gameInit()
{
    u8 timer = 197;
    VDP_setScreenHeight240();
    VDP_clearPlane(BG_A,TRUE);
    VDP_clearPlane(BG_B,TRUE);
    SPR_reset();
    VDP_setWindowVPos(FALSE,3);
    VDP_setTextPalette(PAL3);
	VDP_setTextPlane(WINDOW);
    VDP_loadFont(&gameFont,DMA);
    SYS_setVIntCallback(NULL);
    PAL_interruptFade();
    PAL_setPalette(PAL3,sonicPalette,DMA);
    JOY_setEventHandler(NULL);
    switch (level[0])
    {
    case 0:
    {
        short indBG = TILE_USER_INDEX;
        short indFG = indBG;
        VDP_loadTileSet(&ghzBG_TS,indBG,DMA);
        lvlVRAMIndex = ghzBG_TS.numTile;
        unsigned short basetileBG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indBG);
        unsigned short basetileFG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indFG);
        cycleTimer = MEM_alloc(sizeof(fix16));
        lvlBG = MAP_create(&ghzBG_MAP,BG_B,basetileBG);
        MAP_scrollTo(lvlBG,0,0);
        MEM_free(lvlBG);
        MDS_request(MDS_BGM,BGM_MUS_CLI2);
        switch (level[1])
        {
        case 0:
        {
            break;
        }
        default:
            break;
        }
        break;
    }    
    default:
    {
        killExec(lvlOutOfRange);
        break;
    }
    }
    while(1)
    {
        timer--;
        MDS_update();
        SPR_update();
        SYS_doVBlankProcess();
        if (timer == 0)
        {
            initLevel();
        }
    }
}