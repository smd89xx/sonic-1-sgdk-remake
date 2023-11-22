#include "includes.h"

u16* scroll;
u8* startTxtTimer;
u8* menuIndex;
Sprite* titleSonic;
Sprite* hid[2];
Sprite* startText;

static void clearTitleMem()
{
	MEM_free(scroll);
	MEM_free(cycleTimer);
	MEM_free(startTxtTimer);
	scroll = NULL;
	cycleTimer = NULL;
	startTxtTimer = NULL;
	SYS_setVIntCallback(NULL);
	PAL_interruptFade();
}

static void joyEvent_Title(u16 joy, u16 changed, u16 state)
{
	if (joy != JOY_1)
	{
		return;
	}
	if (changed & state & BUTTON_START)
	{
		u8 timer = palFadeTime;
		PAL_fadeOutAll(palFadeTime,TRUE);
		MDS_fade(mdsFadePeak,mdsFadeDecay,TRUE);
		while (1)
		{
			timer--;
			MDS_update();
			SPR_update();
			SYS_doVBlankProcess();
			if (timer == 0 && (state & BUTTON_A))
			{
				clearTitleMem();
				VDP_setScreenHeight224();
				lvlSelect();
			}
			else if (timer == 0 && !(state & BUTTON_A))
			{
				clearTitleMem();
				gameInit();
			}
		}
	}
}

static void titleVInt()
{
	*scroll += 1;
	MAP_scrollTo(lvlBG,*scroll,0);
	*startTxtTimer -= 1;
	if (*startTxtTimer > 30)
	{
		SPR_setVisibility(startText,HIDDEN);
	}
	else if (*startTxtTimer == 30)
	{
		SPR_setVisibility(startText,VISIBLE);
	}
	else if (*startTxtTimer <= 0)
	{
		*startTxtTimer = 60;
	}
}

static void paletteCycle_Title()
{
	*cycleTimer += FIX16(0.150);
	if (*cycleTimer >= FIX16(4))
	{
		*cycleTimer = FIX16(0);
	}
	PAL_setColors(40,titleWater[fix16ToInt(*cycleTimer)],4,DMA);
}

void title()
{
	u8 yOffset = 0;
	fix16 maxSonicY = FIX16(29);
	if (!isNTSC)
	{
		yOffset = 1;
		maxSonicY = FIX16(37);
	}
	VDP_setScreenHeight240();
	VDP_clearPlane(BG_A,TRUE);
	SPR_init();
	s16 indEmblem = TILE_USER_INDEX;
	s16 indBG = indEmblem + emblem.tileset->numTile;
	u16 basetileEmblem = TILE_ATTR_FULL(PAL0,TRUE,FALSE,FALSE,indEmblem);
	u16 basetileBG = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,indBG);
	u16 basetileSpr = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	startText = SPR_addSprite(&pressStartText,pixelToTile(11),pixelToTile(22+yOffset),basetileEmblem - indEmblem);
	SPR_setAnim(startText,0);
	SPR_setFrame(startText,0);
	titleSonic = SPR_addSprite(&tsSonic,pixelToTile(16),pixelToTile(7+yOffset),basetileSpr);
	hid[0] = SPR_addSprite(&spriteHider,pixelToTile(4),pixelToTile(13+yOffset),basetileSpr);
	hid[1] = SPR_addSprite(&spriteHider,pixelToTile(20),pixelToTile(13+yOffset),basetileSpr);
	SPR_setHFlip(hid[1],TRUE);
	for (u8 i = 0; i < 2; i++)
	{
		SPR_setDepth(hid[i],SPR_MIN_DEPTH);
		SPR_setVRAMTileIndex(hid[i],0x1D4);
	}
	SPR_setVRAMTileIndex(startText,0x236);
	SPR_setVRAMTileIndex(titleSonic,0x247);
	fix16 sonicYPos = FIX16(72);
	u8 loopTimer = 21;
	bool startAnim = FALSE;
	fix16 sonicAnim = FIX16(0);
	u8 fadeTimer = palFadeTime;
	VDP_setScrollingMode(HSCROLL_TILE,VSCROLL_PLANE);
	SPR_setPriority(titleSonic,TRUE);
	VDP_drawImageEx(BG_A,&emblem,basetileEmblem,4,5+yOffset,FALSE,TRUE);
	VDP_loadTileSet(&ghzBG_TS,indBG,DMA);
	lvlBG = MAP_create(&ghzBG_MAP,BG_B,basetileBG);
	MEM_free(lvlBG);
	MAP_scrollTo(lvlBG,0,0);
	VDP_drawText("}SEGA 1991",29,25+yOffset);
	VDP_drawText("}THEWINDOWSPRO98 2023",18,26+yOffset);
	scroll = MEM_alloc(sizeof(short));
	cycleTimer = MEM_alloc(sizeof(fix16));
	startTxtTimer = MEM_alloc(sizeof(char));
	*startTxtTimer = 60;
	PAL_fadeInAll(titlePalette,palFadeTime,TRUE);
	JOY_setEventHandler(joyEvent_Title);
	MDS_request(MDS_BGM,BGM_MUS_S1TITLE);
	while(!startAnim)
	{
		MDS_update();
		SPR_update();
		SYS_doVBlankProcess();
		if (fadeTimer != 0)
		{
			fadeTimer--;
		}
		else
		{
			paletteCycle_Title();
		}
		if (sonicYPos >= maxSonicY)
		{
			sonicYPos -= FIX16(1.75);
			SPR_setPosition(titleSonic,pixelToTile(16),fix16ToRoundedInt(sonicYPos));
		}
		else if (sonicYPos <= maxSonicY)
		{
			startAnim = TRUE;
		}
	}
	while(startAnim)
	{
		SPR_update();
		MDS_update();
		SYS_doVBlankProcess();
		paletteCycle_Title();
		titleVInt();
		if (sonicAnim < FIX16(5.8))
		{
			sonicAnim += FIX16(0.125);
			SPR_setFrame(titleSonic,fix16ToInt(sonicAnim));	
		}
		else
		{
			loopTimer--;
			if (loopTimer == 20)
			{
				SPR_setAnimAndFrame(titleSonic,1,0);
				SPR_setFrame(startText,0);
			}
			else if (loopTimer == 10)
			{
				SPR_setFrame(titleSonic,1);
			}
			else if (loopTimer == 0)
			{
				loopTimer = 21;
			}
		}
	}
}