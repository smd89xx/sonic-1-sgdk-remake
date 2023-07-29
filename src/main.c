#include "includes.h"
#define NUM_OPTS_LS 3

enum regions {ntscJPN = 0x20, ntscUSA = 0xA0, palEUR = 0xE0, palJPN = 0x60};
enum maxVals {lvlMax = 7, actMax = 3, livesMax = 99};
unsigned short* scroll;
unsigned char* startTxtTimer;
unsigned char* sndIndex;
unsigned char* menuIndex;
const unsigned char lsX = 4;
const unsigned char lsY = 14;
const unsigned char sfxStart = 4;
const unsigned char palFadeTime = 30;
const char lvlNames[lvlMax][17] = {"GREEN HILL ZONE","MARBLE ZONE","SPRING YARD ZONE","LABYRINTH ZONE","STAR LIGHT ZONE","SCRAP BRAIN ZONE","FINAL ZONE"};
fix16* cycleTimer;
Sprite* titleSonic;
Sprite* hid[6];
Sprite* startText;

static void joyEvent_LS(u16 joy, u16 changed, u16 state)
{
	if (joy != JOY_1)
	{
		return;
	}
}

static void lvlSelect()
{
	short indEmblem = TILE_USER_INDEX;
	unsigned short basetileEmblem = TILE_ATTR_FULL(PAL0,TRUE,FALSE,FALSE,indEmblem);
	unsigned short basetileText = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
	unsigned short basetileTextHot = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
	VDP_loadFont(&lsFont,DMA);
	VDP_clearPlane(BG_B,TRUE);
	VDP_clearTileMapRect(BG_A,0,0,40,25);
	VDP_setTextPriority(TRUE);
	VDP_setTextPalette(PAL2);
	VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
	VDP_setHorizontalScroll(BG_B,0);
	VDP_drawImageEx(BG_B,&emblem,basetileEmblem,4,5,FALSE,TRUE);
	PAL_fadeInAll(lvlSelectPalette,palFadeTime,TRUE);
	sndIndex = MEM_alloc(sizeof(char));
	menuIndex = MEM_alloc(sizeof(char));
	*sndIndex = BGM_MUS_S2BLVS;
	*menuIndex = 0;
	MDS_request(MDS_BGM,BGM_MUS_S2BLVS);
	JOY_setEventHandler(joyEvent_LS);
	while (1)
	{
		SYS_doVBlankProcess();
		MDS_update();
	}
}

static void joyEvent_Title(u16 joy, u16 changed, u16 state)
{
	if (joy != JOY_1)
	{
		return;
	}
	if (changed & state & BUTTON_START)
	{
		unsigned char timer = palFadeTime;
		PAL_fadeOutAll(palFadeTime,TRUE);
		MDS_fade(0x7F,4,TRUE);
		while (1)
		{
			timer--;
			MDS_update();
			SPR_update();
			SYS_doVBlankProcess();
			if (timer == 0 && (state & BUTTON_A))
			{
				MEM_free(scroll);
				MEM_free(cycleTimer);
				MEM_free(startTxtTimer);
				SYS_setVIntCallback(NULL);
				PAL_interruptFade();
				lvlSelect();
			}
			else if (timer == 0 && !(state & BUTTON_A))
			{
				MEM_free(scroll);
				MEM_free(cycleTimer);
				MEM_free(startTxtTimer);
				SYS_setVIntCallback(NULL);
				PAL_interruptFade();
				gameInit();
			}
		}
	}
}

static short pixelToTile(short pixel)
{
	return pixel << 3;
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

static void title()
{
	VDP_clearPlane(BG_A,TRUE);
	SPR_init();
	short indEmblem = TILE_USER_INDEX;
	short indBG = indEmblem + emblem.tileset->numTile;
	unsigned short basetileEmblem = TILE_ATTR_FULL(PAL0,TRUE,FALSE,FALSE,indEmblem);
	unsigned short basetileBG = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,indBG);
	unsigned short basetileSpr = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	startText = SPR_addSprite(&pressStartText,pixelToTile(11),pixelToTile(22),basetileEmblem - indEmblem);
	SPR_setAnim(startText,0);
	SPR_setFrame(startText,0);
	titleSonic = SPR_addSprite(&tsSonic,pixelToTile(16),pixelToTile(7),basetileSpr);
	for (unsigned char i = 0; i < 6; i++)
	{
		hid[i] = SPR_addSprite(&spriteHider,pixelToTile(12+(i << 2)),pixelToTile(14)-1,basetileSpr);
		SPR_setDepth(hid[i],SPR_MIN_DEPTH);
		SPR_setPriority(hid[i],FALSE);
	}
	fix16 sonicYPos = FIX16(72);
	unsigned char loopTimer = 21;
	bool startAnim = FALSE;
	fix16 sonicAnim = FIX16(0);
	unsigned char fadeTimer = palFadeTime;
	VDP_setScrollingMode(HSCROLL_TILE,VSCROLL_PLANE);
	SPR_setPosition(hid[4],pixelToTile(18),139);
	SPR_setPosition(hid[5],pixelToTile(21)-3,129);
	SPR_setPriority(titleSonic,TRUE);
	VDP_drawImageEx(BG_A,&emblem,basetileEmblem,4,5,FALSE,TRUE);
	VDP_loadTileSet(&ghzBG_TS,indBG,DMA);
	lvlBG = MAP_create(&ghzBG_MAP,BG_B,basetileBG);
	MEM_free(lvlBG);
	MAP_scrollTo(lvlBG,0,0);
	VDP_drawText("}SEGA 1991",29,25);
	VDP_drawText("}THEWINDOWSPRO98 2023",18,26);
	scroll = MEM_alloc(sizeof(short));
	cycleTimer = MEM_alloc(sizeof(fix16));
	startTxtTimer = MEM_alloc(sizeof(char));
	*startTxtTimer = 60;
	SYS_setVIntCallback(titleVInt);
	PAL_fadeInAll(titlePalette,palFadeTime,TRUE);
	JOY_setEventHandler(joyEvent_Title);
	MDS_request(MDS_BGM,BGM_MUS_CLI2);
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
		if (sonicYPos >= FIX16(29))
		{
			sonicYPos -= FIX16(1.75);
			SPR_setPosition(titleSonic,pixelToTile(16)+1,fix16ToRoundedInt(sonicYPos));
		}
		else if (sonicYPos <= FIX16(29))
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

static void teamCredits()
{
	VDP_clearPlane(BG_A,TRUE);
	PAL_fadeInPalette(PAL1,sonicPalette,palFadeTime,TRUE);
	short ind = TILE_USER_INDEX;
	unsigned short basetileVRAM = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,ind);
	VDP_drawImageEx(BG_A,&presentingLogo,basetileVRAM,7,12,FALSE,TRUE);
	unsigned char timer = 100 + palFadeTime;
	while (1)
	{
		timer--;
		MDS_update();
		SYS_doVBlankProcess();
		if (timer == palFadeTime)
		{
			PAL_fadeOutPalette(PAL1,palFadeTime,TRUE);
		}
		else if (timer == 0)
		{
			title();
		}
	}
	
}

static void segaScreen()
{
	short ind = TILE_USER_INDEX;
	unsigned short basetileVRAM = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,ind);
	unsigned char timer = 126 + palFadeTime;
	PAL_fadeInPalette(PAL0,segaPalette,palFadeTime,TRUE);
	VDP_drawImageEx(BG_A,&segaLogo,basetileVRAM,13,12,FALSE,TRUE);
	while (1)
	{
		timer--;
		MDS_update();
		SYS_doVBlankProcess();
		if (timer == 126)
		{
			MDS_request(MDS_BGM,BGM_SFX_SEGA);
		}
		else if (timer == palFadeTime)
		{
			PAL_fadeOutPalette(PAL0,palFadeTime,TRUE);
		}
		else if (timer == 0)
		{
			teamCredits();
		}
	}
}

int main(bool resetType)
{
	for (unsigned char i = 0; i < 4; i++)
	{
		PAL_setPalette(i,palette_black,DMA);
	}
	Z80_unloadDriver();
	VDP_loadFont(&titleFont,DMA);
	MDS_init(mdsseqdat,mdspcmdat);
	segaScreen();
	while(TRUE)
	{
		MDS_update();
		SYS_doVBlankProcess();
	}
	return 0;
}
