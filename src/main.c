#include "includes.h"
#define NUM_OPTS_LS 27

enum regions {ntscJPN = 0x20, ntscUSA = 0xA0, palEUR = 0xE0, palJPN = 0x60};
enum maxVals {lvlMax = 7, actMax = 3, livesMax = 99};

typedef struct
{
	unsigned char x;
	unsigned char y;
	char* label;
}Option;

unsigned short* scroll;
unsigned char* startTxtTimer;
unsigned char* sndIndex;
unsigned char* menuIndex;
const unsigned char lsX = 3;
const unsigned char lsY = 4;
const unsigned char sfxStart = 4;
const unsigned char palFadeTime = 30;
const unsigned char mdsFadePeak = 0x7F;
const unsigned char mdsFadeDecay = 4;
fix16* cycleTimer;
Sprite* titleSonic;
Sprite* hid[6];
Sprite* startText;
const Option lsOptions[NUM_OPTS_LS] =
{
	{lsX, lsY, "GREEN HILL    1"},
	{lsX, lsY+1, "GREEN HILL    2"},
	{lsX, lsY+2, "GREEN HILL    3"},
	{lsX, lsY+4, "MARBLE        1"},
	{lsX, lsY+5, "MARBLE        2"},
	{lsX, lsY+6, "MARBLE        3"},
	{lsX, lsY+8, "SPRING YARD   1"},
	{lsX, lsY+9, "SPRING YARD   2"},
	{lsX, lsY+10, "SPRING YARD   3"},
	{lsX, lsY+12, "LABYRINTH     1"},
	{lsX, lsY+13, "LABYRINTH     2"},
	{lsX, lsY+14, "LABYRINTH     3"},
	{lsX, lsY+16, "STAR LIGHT    1"},
	{lsX, lsY+17, "STAR LIGHT    2"},
	{lsX, lsY+18, "STAR LIGHT    3"},
	{lsX+19, lsY, "SCRAP BRAIN   1"},
	{lsX+19, lsY+1, "SCRAP BRAIN   2"},
	{lsX+19, lsY+2, "SCRAP BRAIN   3"},
	{lsX+19, lsY+3, "SCRAP BRAIN   4"},
	{lsX+19, lsY+5, "SPECIAL STAGE 1"},
	{lsX+19, lsY+6, "SPECIAL STAGE 2"},
	{lsX+19, lsY+7, "SPECIAL STAGE 3"},
	{lsX+19, lsY+8, "SPECIAL STAGE 4"},
	{lsX+19, lsY+9, "SPECIAL STAGE 5"},
	{lsX+19, lsY+10, "SPECIAL STAGE 6"},
	{lsX+19, lsY+11, "SPECIAL STAGE 7"},
	{lsX+19, lsY+18, "SOUND TEST {  {"},
};

static short pixelToTile(short pixel)
{
	return pixel << 3;
}

static void moveCursor(bool direction)
{
	unsigned short basetileText = TILE_ATTR(PAL0,FALSE,FALSE,FALSE);
	unsigned short basetileTextHot = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	if (*menuIndex == 0 && direction)
	{
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex].label,basetileText,lsOptions[*menuIndex].x,lsOptions[*menuIndex].y,DMA);
		*menuIndex = NUM_OPTS_LS;
	}
	else if (*menuIndex >= NUM_OPTS_LS-1 && !direction)
	{
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex].label,basetileText,lsOptions[*menuIndex].x,lsOptions[*menuIndex].y,DMA);
		*menuIndex = 0;
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex].label,basetileTextHot,lsOptions[*menuIndex].x,lsOptions[*menuIndex].y,DMA);
		return;
	}
	if (*menuIndex != 0 && direction)
	{
		*menuIndex -= 1;
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex].label,basetileTextHot,lsOptions[*menuIndex].x,lsOptions[*menuIndex].y,DMA);
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex+1].label,basetileText,lsOptions[*menuIndex+1].x,lsOptions[*menuIndex+1].y,DMA);
	}
	else if (*menuIndex < NUM_OPTS_LS && !direction)
	{
		*menuIndex += 1;
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex].label,basetileTextHot,lsOptions[*menuIndex].x,lsOptions[*menuIndex].y,DMA);
		VDP_drawTextEx(BG_A,lsOptions[*menuIndex-1].label,basetileText,lsOptions[*menuIndex-1].x,lsOptions[*menuIndex-1].y,DMA);
	}
	VDP_clearTileMapRect(BG_A,0,0,40,1);
}

static void joyEvent_LS(u16 joy, u16 changed, u16 state)
{
	if (joy != JOY_1)
	{
		return;
	}
	if (changed & state & BUTTON_UP)
	{
		MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
		moveCursor(TRUE);
	}
	else if (changed & state & BUTTON_DOWN)
	{
		MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
		moveCursor(FALSE);
	}
	if (changed & state & BUTTON_START)
	{
		unsigned char fadeTimer = palFadeTime;
		PAL_fadeOutAll(palFadeTime,TRUE);
		MDS_fade(mdsFadePeak,mdsFadeDecay,TRUE);
		while (1)
		{
			fadeTimer--;
			SYS_doVBlankProcess();
			MDS_update();
			if (fadeTimer == 0)
			{
				PAL_interruptFade();
				switch (*menuIndex)
				{
					case 0:
					{
						level[0] = 0;
						level[1] = 0;
						gameInit();
						break;
					}
					default:
					{
						killExec(featureNotFound);
						break;
					}
				}
			}
		}
	}
}

static void lvlSelect()
{
	short indEmblem = TILE_USER_INDEX;
	unsigned short basetileEmblem = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indEmblem);
	unsigned short basetileTextHot = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	VDP_clearPlane(BG_B,TRUE);
	VDP_clearPlane(BG_A,TRUE);
	SPR_reset();
	VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
	VDP_setHorizontalScroll(BG_B,0);
	VDP_drawImageEx(BG_B,&lsBG,basetileEmblem,0,0,FALSE,TRUE);
	for (unsigned char i = 0; i < NUM_OPTS_LS; i++)
	{
		Option o = lsOptions[i];
		VDP_drawText(o.label,o.x,o.y);
	}
	VDP_drawTextEx(BG_A,lsOptions[0].label,basetileTextHot,lsOptions[0].x,lsOptions[0].y,DMA);
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
		SPR_update();
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
		MDS_fade(mdsFadePeak,mdsFadeDecay,TRUE);
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
