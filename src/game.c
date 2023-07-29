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
unsigned short player_x = 0;
unsigned short player_y = 0;
short lvlVRAMIndex;
Sprite* player;

static void spawnPlayer()
{
    short basetile = TILE_ATTR(PAL3,FALSE,FALSE,FALSE);
    player = SPR_addSprite(&plrSonic,player_x,player_y,basetile);
}

static void setLevelPalette(const unsigned short* palette)
{
    PAL_fadeIn(0,47,palette,palFadeTime,TRUE);
}

static void drawLevel()
{
    player_x = 0;
    switch (level[0])
    {
    case 0:
    {
        short indBG = TILE_USER_INDEX;
        short indFG = indBG;
        lvlVRAMIndex = ghzBG_TS.numTile;
        unsigned short basetileBG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indBG);
        unsigned short basetileFG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indFG);
        cycleTimer = MEM_alloc(sizeof(fix16));
        VDP_loadTileSet(&ghzBG_TS,indBG,DMA);
        setLevelPalette(ghzPalette);
        lvlBG = MAP_create(&ghzBG_MAP,BG_B,basetileBG);
        MAP_scrollTo(lvlBG,0,0);
        MEM_free(lvlBG);
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
    uintToStr(score,scoreStr,7);
    VDP_drawText(scoreStr,6,0);
    uintToStr(rings,ringsStr,3);
    VDP_drawText(ringsStr,10,2);
    uintToStr(gameTimer[0],minuteStr,1);
    VDP_drawText(minuteStr,9,1);
    uintToStr(gameTimer[1],secondStr,2);
    VDP_drawText(secondStr,11,1);
    uintToStr(lives,livesStr,2);
    VDP_drawText(livesStr,38,1);
}

static void spawnHUD()
{
    unsigned short basetileHUD_Image = TILE_ATTR_FULL(PAL3,TRUE,FALSE,FALSE,lvlVRAMIndex+TILE_USER_INDEX);
    VDP_loadFont(&gameFont,DMA);
    VDP_setWindowVPos(FALSE,3);
    VDP_setTextPalette(PAL3);
	VDP_setTextPlane(WINDOW);
    VDP_drawText("SCORE",0,0);
    VDP_drawText("TIME",0,1);
    VDP_drawText("RINGS",0,2);
    VDP_drawText(":",10,1);
    VDP_drawImageEx(WINDOW,&livesHUD,basetileHUD_Image,34,0,FALSE,TRUE);
}

static void initLevel()
{
    drawLevel();
    spawnPlayer();
    spawnHUD();
    while (1)
    {
        SYS_doVBlankProcess();
        MDS_update();
        SPR_update();
        updateHUD();
        if (level[0] == 0)
        {
            paletteCycle_GHZ();
        }
    }
}

void gameInit()
{
    unsigned char timer = 197;
    VDP_clearPlane(BG_A,TRUE);
    VDP_clearPlane(BG_B,TRUE);
    SPR_reset();
    SYS_setVIntCallback(NULL);
    PAL_interruptFade();
    PAL_setPalette(PAL3,sonicPalette,DMA);
    switch (level[0])
    {
    case 0:
    {
        MDS_request(MDS_BGM,BGM_MUS_CLI2);
        break;
    }    
    default:
    {
        killExec(lvlOutOfRange);
        break;
    }
    }
    JOY_setEventHandler(joyEvent_Game);
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