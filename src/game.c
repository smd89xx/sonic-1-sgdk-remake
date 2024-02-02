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
#define maxLevel 7
#define maxAct   3
u8 lives = 1;
u16 rings = 0;
u8 timeTimer = 0;
Timer gameTimer = {0,0,0};
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
u16 cardXs[] = {72};
Vect2D_s32 cameraPositions;
Vect2D_s32 mapSizes[maxLevel][maxAct] = {{{1360,768}}};
enum cameraBoundaries {leftCamBnd = 144, rightCamBnd = 144, topCamBnd = 104, bottomCamBnd = 104};
Sprite* shieldSpr;
bool hasShield = false;
s32 waterLocations[maxLevel][maxAct] = {{380}};
u8 waterTimer = 30;
u8 waterSubTimer = 60;
Sprite* waves;
Sprite* titleCardAct;
CollisionMData* collisionArray;
u8 collisionIndex;
const u8 collisionAmounts[maxLevel][maxAct] = {{7}};
bool isJumping;
fix32 gravity = FIX32(0.1375);
fix32 jumpStrength = FIX32(6);
enum hitboxPoints
{
    boxLeft = 18,
    boxRight = 44,
    boxTop = 15,
    boxBottom = 53,
};

static void camPos()
{
    s32 px = fix32ToInt(playerObj.positions.x);
    s32 py = fix32ToInt(playerObj.positions.y);
    s32 px_scrn = px - cameraPositions.x;
    s32 py_scrn = py - cameraPositions.y;
    if (px_scrn > rightCamBnd)
    {
        cameraPositions.x = px - rightCamBnd;
    }
    else if (px_scrn < leftCamBnd)
    {
        cameraPositions.x = px - leftCamBnd;
    }
    if (py_scrn > bottomCamBnd)
    {
        cameraPositions.y = py - bottomCamBnd;
    }
    else if (py_scrn < topCamBnd)
    {
        cameraPositions.y = py - topCamBnd;
    }
    if (cameraPositions.x < 0)
    {
        cameraPositions.x = 0;
    }
    else if (cameraPositions.x > mapSizes[level[0]][level[1]].x - screenWidth)
    {
        cameraPositions.x = mapSizes[level[0]][level[1]].x - screenWidth;
    }
    if (cameraPositions.y < -32)
    {
        cameraPositions.y = -32;
    }
    else if (cameraPositions.y > mapSizes[level[0]][level[1]].y - screenHeight)
    {
        cameraPositions.y = mapSizes[level[0]][level[1]].y - screenHeight;
    }
    MAP_scrollTo(lvlFG,cameraPositions.x,cameraPositions.y);
    VDP_setHorizontalScroll(BG_B,-cameraPositions.x >> 1);
    VDP_setVerticalScroll(BG_B,cameraPositions.y / (mapSizes[level[0]][level[1]].y >> 2));
}

static void updatePlayer()
{
    playerObj.positions.x += playerObj.velocities.x;
    playerObj.velocities.y += gravity;
    playerObj.positions.y += playerObj.velocities.y;
    s16 px = fix32ToInt(playerObj.positions.x);
    s16 py = fix32ToInt(playerObj.positions.y);
    if (px < 0)
    {
        playerObj.positions.x = FIX32(0);
    }
    SPR_setPosition(playerObj.sprite,px - cameraPositions.x,py - cameraPositions.y);
    if (hasShield)
    {
        SPR_setPosition(shieldSpr,(px - cameraPositions.x) + 8,(py - cameraPositions.y) + 8);
    }
}

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

static void death(bool type)
{
    playerObj.velocities.y = FIX32(0);
    playerObj.velocities.x = FIX32(0);
    s32 timer = secToFrames(FIX32(1));
    JOY_setEventHandler(NULL);
    hasShield = false;
    SPR_releaseSprite(shieldSpr);
    if (!type)
    {
        MDS_request(MDS_SE1,BGM_SFX_S1DEATH);
        playerObj.velocities.y -= jumpStrength;
        animIndex = animDie;
    }
    else
    {
        MDS_request(MDS_SE1,BGM_SFX_S1DEATH);
        animIndex = animDrown;
    }
    while (true)
    {
        SYS_doVBlankProcess();
        SPR_update();
        MDS_update();
        updatePlayer();
        manageAnim();
        camPos();
        if (playerObj.positions.y >= FIX32(mapSizes[level[0]][level[1]].y))
        {
            SPR_setPosition(playerObj.sprite,0,mapSizes[level[0]][level[1]].y);
            if (timer == palFadeTime)
            {
                lives--;
                if (lives == 0)
                {
                    killExec(genericErr);
                    return;
                }
                PAL_fadeOutAll(palFadeTime,true);
                MDS_fade(mdsFadePeak,mdsFadeDecay,true);
            }
            else if (timer == 0)
            {
                gameInit();
            }
            timer--;
        }
    }
}


static void updateWater()
{
    u8 maxFrame = 60;
    if (!isNTSC)
    {
        maxFrame = 50;
    }
    if (playerObj.positions.y == FIX32(waterLocations[level[0]][level[1]]))
    {
        MDS_request(MDS_SE1,BGM_SFX_SPLASH);
    }
    if (playerObj.positions.y <= FIX32(waterLocations[level[0]][level[1]]))
    {
        waterTimer = 30;
        waterSubTimer = maxFrame;
    }
    else
    {
        if (waterSubTimer == 0)
        {
            waterSubTimer = maxFrame;
            waterTimer--;
        }
        else
        {
            waterSubTimer--;
        }
        if (waterTimer % 5 == 0 && waterTimer > 10)
        {
            MDS_request(MDS_SE2,BGM_SFX_S1DING);
        }
        else if (waterTimer % 2 == 0 && waterTimer <= 10)
        {
            MDS_request(MDS_SE2,BGM_SFX_S1DING);
        }
        if (waterTimer == 0 && waterSubTimer == 0)
        {
            death(true);
        }
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

static void jump()
{
    if (isJumping)
    {
        return;
    }
    animIndex = animRoll;
    isJumping = true;
    MDS_request(MDS_SE1,BGM_SFX_SMWJUMP);
    playerObj.positions.y -= FIX32(8);
    playerObj.velocities.y -= jumpStrength;
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
        u16 basetile = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
        hasShield = !hasShield;
        if (hasShield)
        {
            shieldSpr = SPR_addSprite(&shield,-48,0,basetile);
            SPR_setVRAMTileIndex(shieldSpr,TILE_USER_INDEX);
            SPR_setDepth(shieldSpr,SPR_MIN_DEPTH);
            MDS_request(MDS_SE1,BGM_SFX_SHIELD);
        }
        else
        {
            SPR_releaseSprite(shieldSpr);
            MDS_request(MDS_SE1,BGM_SFX_S1DEATH);
        }
    }
    else if (changed & state & BUTTON_C)
    {
        jump();
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
        playerObj.velocities.x = FIX32(-2.5);
        SPR_setHFlip(playerObj.sprite,true);
    }
    else if (isRightPressed)
    {
        animIndex = animWalkGround;
        playerObj.velocities.x = FIX32(2.5);
        SPR_setHFlip(playerObj.sprite,false);
    }
    else
    {
        animIndex = animIdle;
        playerObj.velocities.x = FIX32(0);
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
    char minuteStr[4];
    char livesStr[4];
    char framesStr[3];
    u16 ram = MEM_getFree();
    u8 spr = SPR_getNumActiveSprite();
    u8 sprVDP = SPR_getUsedVDPSprite();
    uintToStr(ram,scoreStr,10);
    VDP_drawText(scoreStr,6,0);
    uintToStr(spr,ringsStr,5);
    VDP_drawText(ringsStr,11,2);
    uintToStr(gameTimer.minutes,minuteStr,3);
    VDP_drawText(minuteStr,7,1);
    uintToStr(gameTimer.seconds,secondStr,2);
    VDP_drawText(secondStr,11,1);
    uintToStr(sprVDP,livesStr,3);
    VDP_drawText(livesStr,13,3);
    uintToStr(gameTimer.frames,framesStr,2);
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
        gameTimer.frames++;
    }
    if (gameTimer.frames == maxFrame)
    {
        gameTimer.frames = 0;
        gameTimer.seconds++;
    }
    else if (gameTimer.seconds == 60)
    {
        gameTimer.minutes++;
        gameTimer.seconds = 0;
    }
    else if (gameTimer.minutes == 10)
    {
        death(false);
    }
}

static void flashHUD()
{
    u16 basetileNormal = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
    u16 basetileFlash = TILE_ATTR(PAL0,TRUE,FALSE,FALSE);
    u8 flashDelay = 30;
    flashTimer++;
    if (gameTimer.minutes == 9)
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


static void chkCollision()
{
    u8 collisionType = collisionArray[collisionIndex].type;
    s32 left,right,top,bottom;
    left = collisionArray[collisionIndex].left;
    right = left + collisionArray[collisionIndex].width;
    top = collisionArray[collisionIndex].top;
    bottom = top + collisionArray[collisionIndex].height;
    s32 px = fix32ToRoundedInt(playerObj.positions.x);
    s32 py = fix32ToRoundedInt(playerObj.positions.y);
    s32 bottomBox = boxBottom + py;
    s32 topBox = boxTop + py;
    s32 leftBox = boxLeft + px;
    s32 rightBox = boxRight + px;
    bool rcoll = rightBox >= left && rightBox <= right;
    bool lcoll = leftBox >= left && leftBox <= right;
    bool tcoll = topBox >= top && topBox <= bottom;
    bool bcoll = bottomBox >= top && bottomBox <= bottom;
    if ((rcoll || lcoll) && (tcoll || bcoll))
    {
        switch (collisionType)
        {
            case ctTop:
            {
                playerObj.positions.y = FIX32(top - boxBottom);
                playerObj.velocities.y = FIX32(0);
                isJumping = false;
                break;
            }
            case ctLeft:
            {
                playerObj.positions.x = FIX32(right - boxLeft);
                playerObj.velocities.x = FIX32(0);
                animIndex = animPush;
                break;
            }
            case ctVSpring:
            {
                MDS_request(MDS_SE1,BGM_SFX_SMWJUMP);
                playerObj.velocities.y = FIX32(-9);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    if (topBox >= mapSizes[level[0]][level[1]].y)
    {
        death(false);
    }
    collisionIndex++;
    if (collisionIndex == collisionAmounts[level[0]][level[1]])
    {
        collisionIndex = 0;
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
    SPR_releaseSprite(titleCardAct);
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
            updatePlayer();
            camPos();
            updateWater();
            chkCollision();
            if (level[0] == 0)
            {
                //paletteCycle_GHZ();
            }
        }
    }
}

static void cardUpdate()
{
    SPR_setPosition(titleCardText,cardXs[level[0]],80);
    SPR_setPosition(titleCardEmblem,cardXs[level[0]]+96,76);
    SPR_setPosition(titleCardZone,cardXs[level[0]]+80,96);
    SPR_setPosition(titleCardAct,cardXs[level[0]]+112,96);
}

void gameInit()
{
    s32 timer = secToFrames(FIX32(2.96));
    VDP_setScreenHeight240();
    VDP_clearPlane(BG_A,TRUE);
    VDP_clearPlane(BG_B,TRUE);
    SPR_reset();
    VDP_clearPlane(WINDOW,true);
    VDP_setWindowVPos(FALSE,4);
    VDP_setTextPalette(PAL3);
	VDP_setTextPlane(WINDOW);
    VDP_setTextPriority(TRUE);
    VDP_loadFont(&gameFont,DMA);
    SYS_setVIntCallback(NULL);
    PAL_interruptFade();
    PAL_setPalette(PAL2,actTextPalette,DMA);
    PAL_setPalette(PAL3,sonicPalette,DMA);
    JOY_setEventHandler(NULL);
    lvlVRAMIndex = TILE_USER_INDEX;
    u16 basetileCards = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
    titleCardEmblem = SPR_addSprite(&titleDash,376,76,basetileCards);
    SPR_setVRAMTileIndex(titleCardEmblem,lvlVRAMIndex);
    lvlVRAMIndex += titleDash.maxNumTile;
    titleCardZone = SPR_addSprite(&titleZone,384,92,basetileCards);
    SPR_setVRAMTileIndex(titleCardZone,lvlVRAMIndex);
    lvlVRAMIndex += titleZone.maxNumTile;
    titleCardText = SPR_addSprite(&titleCards,464,76,basetileCards);
    SPR_setVRAMTileIndex(titleCardText,lvlVRAMIndex);
    lvlVRAMIndex += titleCards.maxNumTile;
    u16 basetileAct = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
    titleCardAct = SPR_addSprite(&actText,360,76,basetileAct);
    SPR_setVRAMTileIndex(titleCardAct,lvlVRAMIndex);
    lvlVRAMIndex += actText.maxNumTile;
    SPR_setAnim(titleCardText,level[0]);
    SPR_setFrame(titleCardAct,level[1]);
    SPR_setDepth(titleCardText,titleCardEmblem->depth-1);
    SPR_setDepth(titleCardZone,titleCardEmblem->depth-2);
    SPR_setDepth(titleCardAct,titleCardEmblem->depth-3);
    VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
    animIndex = animIdle;
    gameTimer.minutes = 0;
    gameTimer.seconds = 0;
    gameTimer.frames = 0;
    rings = 0;
    switch (level[0])
    {
    case 0:
    {
        
        u16 basetileBG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,lvlVRAMIndex);
        VDP_drawImageEx(BG_B,&testMapBG,basetileBG,0,0,false,true);
        lvlVRAMIndex += testMapBG.tileset->numTile;
        u16 basetileFG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,lvlVRAMIndex);
        VDP_loadTileSet(&testTiles,lvlVRAMIndex,DMA);
        lvlVRAMIndex += testTiles.numTile;
        cycleTimer = MEM_alloc(sizeof(fix16));
        MDS_request(MDS_BGM,BGM_MUS_CLI2);
        switch (level[1])
        {
        case 0:
        {
            playerObj.positions.x = FIX32(0);
            playerObj.positions.y = FIX32(58);
            lvlFG = MAP_create(&testMapFG,BG_A,basetileFG);
            MAP_scrollTo(lvlFG,-320,0);
            MEM_free(lvlFG);
            collisionArray = &ghz1CollisionData;
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