#include "includes.h"

enum playerAnimIndexes
{
    animIdle,
    animWaitStart,
    animWaitLoop,
    animLookUp,
    animCrouch,
    animSpring,
    animWalkGround,
    animWalk45Deg,
    animWalk90Deg,
    animRunGround,
    animRun45Deg,
    animRun90Deg,
    animSkid,
    animRoll,
    animSpindash,
    animBalance,
    animHurt,
    animDie,
    animDrown,
    animBreathe,
    animPush,
    animFanSpin,
    animPoleHang,
};

Map* lvlBG;
Map* lvlFG;
u8 level[2] = {0,0};
u8 lives = 1;
u16 rings = 0;
u8 timeTimer = 0;
u8 gameTimer[3] = {8,50,0};
u32 score = 0;
s16 lvlVRAMIndex;
bool startGameTimer = true;
bool paused = false;
PlayerMData playerObj;
const AnimMData playerAnimData[23] = 
{
    // {index, frameAmount, frameTime}
    {0,1,1},{1,2,5},{2,12,30},{3,1,1},{4,1,1},{5,1,1},{6,8,5},{7,8,5},{8,8,5},{9,4,5},{10,4,5},{11,4,5},{12,2,5},{13,7,0},{14,10,0},{15,2,30},{16,2,15},{17,1,1},{18,1,1},{19,1,1},{20,4,5},{21,5,5},{22,2,1}
};
u8 frameTimer;
u8 animIndex;
u8 frameIndex;
u8 flashTimer;
Sprite* titleCardText;
Sprite* titleCardEmblem;
Sprite* titleCardZone;
u16 cardXs[] = {100,0,0,0,0,0,0};

static void manageAnim()
{
    if (frameTimer < playerAnimData[animIndex].frameTime)
    {
        frameTimer++;
    }
    else
    {
        frameTimer = 0;
        if (frameIndex < playerAnimData[animIndex].frameAmount - 1)
        {
            frameIndex++;
        }
        else
        {
            frameIndex = 0;
        }
        SPR_setAnimAndFrame(playerObj.sprite,animIndex,frameIndex);
    }
    
}

static Vect2D_s32 f32VectToIntVect(Vect2D_f32 vector)
{
    s32 cvtX = fix32ToInt(vector.x);
    s32 cvtY = fix32ToInt(vector.y);
    Vect2D_s32 cvtVector = {cvtX, cvtY};
    return cvtVector;
}

static void spawnplayer()
{
    u16 basetile = TILE_ATTR(PAL3,FALSE,FALSE,FALSE);
    Vect2D_s32 vector = f32VectToIntVect(playerObj.positions);
    playerObj.sprite = SPR_addSprite(&plrSonic,vector.x,vector.y,basetile);
    SPR_setVRAMTileIndex(playerObj.sprite,lvlVRAMIndex);
    lvlVRAMIndex += plrSonic.maxNumTile;
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
    bool isLeftPressed = state & BUTTON_LEFT;
    bool isRightPressed = state & BUTTON_RIGHT;
    if (isLeftPressed)
    {
        animIndex = animWalkGround;
        SPR_setHFlip(playerObj.sprite,true);
    }
    else if (isRightPressed)
    {
        animIndex = animWalkGround;
        SPR_setHFlip(playerObj.sprite,false);
    }
    else
    {
        animIndex = animIdle;
    }
    bool isUpPressed = state & BUTTON_UP;
    bool isDownPressed = state & BUTTON_DOWN;
    if (isUpPressed)
    {
        animIndex = animLookUp;
    }
    else if (isDownPressed)
    {
        animIndex = animCrouch;
        if (changed & BUTTON_A)
        {
            animIndex = animSpindash;
            return;
        }
    }
    else
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
    char scoreStr[11];
    char ringsStr[6];
    char secondStr[3];
    char minuteStr[2];
    char livesStr[4];
    char framesStr[3];
    uintToStr(score,scoreStr,10);
    VDP_drawText(scoreStr,6,0);
    uintToStr(rings,ringsStr,5);
    VDP_drawText(ringsStr,11,2);
    uintToStr(gameTimer[0],minuteStr,1);
    VDP_drawText(minuteStr,9,1);
    uintToStr(gameTimer[1],secondStr,2);
    VDP_drawText(secondStr,11,1);
    uintToStr(lives,livesStr,3);
    VDP_drawText(livesStr,13,3);
    uintToStr(gameTimer[2],framesStr,2);
    VDP_drawText(framesStr,14,1);
}

static void spawnHUD()
{
    VDP_drawText("SCORE",0,0);
    VDP_drawText("TIME",0,1);
    VDP_drawText("RINGS",0,2);
    VDP_drawText("LIVES",0,3);
    VDP_drawText(":",13,1);
    VDP_drawText(":",10,1);
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
        VDP_drawText(":",10,1);
    }
}

static void flashHUD()
{
    u16 basetileNormal = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
    u16 basetileFlash = TILE_ATTR(PAL0,TRUE,FALSE,FALSE);
    u8 flashDelay = 30;
    flashTimer++;
    if (gameTimer[0] == 9)
    {
        if (flashTimer == flashDelay)
        {
            VDP_drawTextEx(WINDOW,"TIME",basetileFlash,0,1,DMA);
        }
        else if (flashTimer == flashDelay << 1)
        {
            VDP_drawTextEx(WINDOW,"TIME",basetileNormal,0,1,DMA);
        }
    }
    if (rings == 0)
    {
        if (flashTimer == flashDelay)
        {
            VDP_drawTextEx(WINDOW,"RINGS",basetileFlash,0,2,DMA);
        }
        else if (flashTimer == flashDelay << 1)
        {
            VDP_drawTextEx(WINDOW,"RINGS",basetileNormal,0,2,DMA);
        }
    }
    else
    {
        VDP_drawTextEx(WINDOW,"RINGS",basetileNormal,0,2,DMA);
    }
    if (lives <= 1)
    {
        if (flashTimer == flashDelay)
        {
            VDP_drawTextEx(WINDOW,"LIVES",basetileFlash,0,3,DMA);
        }
        else if (flashTimer == flashDelay << 1)
        {
            VDP_drawTextEx(WINDOW,"LIVES",basetileNormal,0,3,DMA);
        }
    }
    else
    {
        VDP_drawTextEx(WINDOW,"LIVES",basetileNormal,0,3,DMA);
    }
    if (flashTimer == flashDelay << 1)
    {
        flashTimer = 0;
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
    SPR_releaseSprite(titleCardEmblem);
    SPR_releaseSprite(titleCardText);
    SPR_releaseSprite(titleCardZone);
    spawnplayer();
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
            manageAnim();
            flashHUD();
            if (level[0] == 0)
            {
                paletteCycle_GHZ();
            }
        }
    }
}

static void cardUpdate()
{
    SPR_setPosition(titleCardText,cardXs[level[0]],80);
    SPR_setPosition(titleCardEmblem,cardXs[level[0]]+72,76);
    SPR_setPosition(titleCardZone,cardXs[level[0]],96);
}

void gameInit()
{
    s32 timer = secToFrames(FIX32(2.96));
    VDP_setScreenHeight240();
    VDP_clearPlane(BG_A,TRUE);
    VDP_clearPlane(BG_B,TRUE);
    SPR_reset();
    VDP_setWindowVPos(FALSE,4);
    VDP_setTextPalette(PAL3);
	VDP_setTextPlane(WINDOW);
    VDP_setTextPriority(TRUE);
    VDP_loadFont(&gameFont,DMA);
    SYS_setVIntCallback(NULL);
    PAL_interruptFade();
    PAL_setPalette(PAL3,sonicPalette,DMA);
    JOY_setEventHandler(NULL);
    s16 indBG = TILE_USER_INDEX;
    u16 basetileCards = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
    titleCardEmblem = SPR_addSprite(&titleDash,376,76,basetileCards);
    SPR_setVRAMTileIndex(titleCardEmblem,indBG);
    lvlVRAMIndex = titleDash.maxNumTile + indBG;
    titleCardZone = SPR_addSprite(&titleZone,384,92,basetileCards);
    SPR_setVRAMTileIndex(titleCardZone,lvlVRAMIndex);
    lvlVRAMIndex += titleZone.maxNumTile;
    titleCardText = SPR_addSprite(&titleCards,464,76,basetileCards);
    SPR_setVRAMTileIndex(titleCardText,lvlVRAMIndex);
    lvlVRAMIndex += titleCards.maxNumTile;
    SPR_setAnim(titleCardText,level[0]);
    SPR_setDepth(titleCardText,titleCardEmblem->depth-1);
    switch (level[0])
    {
    case 0:
    {
        VDP_loadTileSet(&ghzBG_TS,lvlVRAMIndex,DMA);
        u16 basetileBG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,lvlVRAMIndex);
        lvlVRAMIndex += ghzBG_TS.numTile;
        cycleTimer = MEM_alloc(sizeof(fix16));
        lvlBG = MAP_create(&ghzBG_MAP,BG_B,basetileBG);
        MAP_scrollTo(lvlBG,0,0);
        MEM_free(lvlBG);
        MDS_request(MDS_BGM,BGM_MUS_CLI2);
        switch (level[1])
        {
        case 0:
        {
            s16 indFG = lvlVRAMIndex;
            u16 basetileFG = TILE_ATTR_FULL(PAL3,FALSE,FALSE,FALSE,indFG);
            playerObj.positions.x = FIX32(0);
            playerObj.positions.y = FIX32(64);
            VDP_loadTileSet(&ghzFG_Act1TS,indFG,DMA);
            lvlFG = MAP_create(&ghzFG_Act1Map,BG_A,basetileFG);
            MAP_scrollTo(lvlFG,0,0);
            MEM_free(lvlFG);
            lvlVRAMIndex = indFG + ghzFG_Act1TS.numTile;
            break;
        }
        default:
        {
            killExec(lvlOutOfRange);
            break;
        }
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
        cardUpdate();
        SYS_doVBlankProcess();
        if (timer == 0)
        {
            initLevel();
        }
    }
}