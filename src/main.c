#include "includes.h"
#define NUM_OPTS_LS 3

enum regions {ntscJPN = 0x20, ntscUSA = 0xA0, palEUR = 0xE0, palJPN = 0x60};
enum maxVals {lvlMax = 6, actMax = 3, livesMax = 99};
unsigned short* scroll;
unsigned char* startTxtTimer;
unsigned char* sndIndex;
unsigned char* menuIndex;
const unsigned char lsX = 5;
const unsigned char lsY = 14;
const unsigned char sfxStart = 3;
fix16* cycleTimer;
Sprite* titleSonic;
Sprite* hid[6];
Sprite* startText;
typedef struct
{
	unsigned char x;
	unsigned char y;
}Option;

const Option lsOptions[NUM_OPTS_LS] = 
{
	{lsX,lsY},
	{lsX+7,lsY},
	{lsX+12,lsY},
};

static void joyEvent_LS(u16 joy, u16 changed, u16 state)
{
	if (joy != JOY_1)
	{
		return;
	}
	if (changed & state & BUTTON_LEFT)
	{
		if (*menuIndex > 0)
		{
			*menuIndex -= 1;
		}
		else if (*menuIndex == 0)
		{
			*menuIndex = NUM_OPTS_LS-1;
		}
	}
	else if (changed & state & BUTTON_RIGHT)
	{
		if (*menuIndex < NUM_OPTS_LS-1)
		{
			*menuIndex += 1;
		}
		else if (*menuIndex == NUM_OPTS_LS-1)
		{
			*menuIndex = 0;
		}
	}
	if (changed & state & BUTTON_UP)
	{
		switch (*menuIndex)
		{
			case 0:
			{
				if (level[0] < lvlMax-1)
				{
					level[0]++;
				}
				else
				{
					level[0] = 0;
				}
				break;
			}
			case 1:
			{
				if (level[1] < actMax-1)
				{
					level[1]++;
				}
				else
				{
					level[1] = 0;
				}
				break;
			}
			case 2:
			{
				*sndIndex += 1;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else if (changed & state & BUTTON_DOWN)
	{
		switch (*menuIndex)
		{
			case 0:
			{
				if (level[0] > 0)
				{
					level[0]--;
				}
				else
				{
					level[0] = lvlMax-1;
				}
				break;
			}
			case 1:
			{
				if (level[1] > 0)
				{
					level[1]--;
				}
				else
				{
					level[1] = actMax-1;
				}
				break;
			}
			case 2:
			{
				*sndIndex -= 1;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	if (changed & state & BUTTON_START)
	{
		if (*menuIndex == NUM_OPTS_LS-1)
		{
			if (*sndIndex < sfxStart)
			{
				MDS_request(MDS_BGM,*sndIndex);
			}
			else
			{
				MDS_request(MDS_SE1,*sndIndex);
			}
		}
		else
		{
			unsigned char timer = 30;
			PAL_fadeOutAll(30,TRUE);
			MDS_fade(0x7F,4,TRUE);
			while(1)
			{
				timer--;
				MDS_update();
				SPR_update();
				SYS_doVBlankProcess();
				if (timer == 0)
				{
					gameInit();
				}
			}
		}
	}
}

static void lvlSelect()
{
	short indEmblem = TILE_USER_INDEX;
	unsigned short basetileEmblem = TILE_ATTR_FULL(PAL0,TRUE,FALSE,FALSE,indEmblem);
	VDP_clearPlane(BG_B,TRUE);
	VDP_clearPlane(BG_A,TRUE);
	VDP_setTextPriority(TRUE);
	VDP_loadFont(&lsFont,DMA);
	VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
	VDP_setHorizontalScroll(BG_B,0);
	VDP_drawImageEx(BG_B,&emblem,basetileEmblem,4,5,FALSE,TRUE);
	PAL_setColors(0,lvlSelectPalette,64,DMA);
	char lvlStr[2] = "0";
	char actStr[2] = "0";
	char sndStr[4] = "00";
	VDP_setTextPalette(PAL2);
	VDP_drawText("LEVEL:",lsX,lsY);
	VDP_drawText("ACT:",lsX+9,lsY);
	VDP_drawText("SOUND TEST:",lsX+16,lsY);
	unsigned short basetile[NUM_OPTS_LS] = {TILE_ATTR(PAL3,TRUE,FALSE,FALSE),TILE_ATTR(PAL2,TRUE,FALSE,FALSE),TILE_ATTR(PAL2,TRUE,FALSE,FALSE)};
	sndIndex = MEM_alloc(sizeof(char));
	menuIndex = MEM_alloc(sizeof(char));
	*sndIndex = 0;
	*menuIndex = 0;
	MDS_request(MDS_BGM,BGM_MUS_CLI2);
	JOY_setEventHandler(joyEvent_LS);
	while (1)
	{
		SPR_update();
		MDS_update();
		SYS_doVBlankProcess();
		uintToStr(level[0],lvlStr,1);
		VDP_drawTextEx(BG_A,lvlStr,basetile[0],lsX+7,lsY,DMA);
		uintToStr(level[1],actStr,1);
		VDP_drawTextEx(BG_A,actStr,basetile[1],lsX+14,lsY,DMA);
		uintToStr(*sndIndex,sndStr,2);
		VDP_drawTextEx(BG_A,sndStr,basetile[2],lsX+28,lsY,DMA);
		switch (*menuIndex)
		{
		case 0:
		{
			basetile[0] = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
			basetile[1] = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
			basetile[2] = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
			break;
		}
		case 1:
		{
			basetile[0] = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
			basetile[1] = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
			basetile[2] = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
			break;
		}
		case 2:
		{
			basetile[0] = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
			basetile[1] = TILE_ATTR(PAL2,TRUE,FALSE,FALSE);
			basetile[2] = TILE_ATTR(PAL3,TRUE,FALSE,FALSE);
			break;
		}
		default:
		{
			break;
		}
		}
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
		unsigned char timer = 30;
		PAL_fadeOutAll(30,TRUE);
		MDS_fade(0x7F,4,TRUE);
		while (1)
		{
			timer--;
			MDS_update();
			SPR_update();
			SYS_doVBlankProcess();
			if (timer == 0)
			{
				MEM_free(scroll);
				MEM_free(cycleTimer);
				MEM_free(startTxtTimer);
				SYS_setVIntCallback(NULL);
				PAL_interruptFade();
				lvlSelect();
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
		SPR_setDepth(hid[i],-0x8000);
		SPR_setPriority(hid[i],FALSE);
	}
	fix16 sonicYPos = FIX16(72);
	unsigned char loopTimer = 21;
	bool startAnim = FALSE;
	fix16 sonicAnim = FIX16(0);
	unsigned char fadeTimer = 30;
	VDP_setScrollingMode(HSCROLL_TILE,VSCROLL_PLANE);
	SPR_setPosition(hid[4],pixelToTile(18),139);
	SPR_setPosition(hid[5],pixelToTile(21)-3,129);
	SPR_setPriority(titleSonic,TRUE);
	VDP_drawImageEx(BG_A,&emblem,basetileEmblem,4,5,FALSE,TRUE);
	VDP_loadTileSet(&ghzBG_TS,indBG,DMA);
	lvlBG = MAP_create(&ghzBG_MAP,BG_B,basetileBG);
	MAP_scrollTo(lvlBG,0,0);
	scroll = MEM_alloc(sizeof(short));
	cycleTimer = MEM_alloc(sizeof(fix16));
	startTxtTimer = MEM_alloc(sizeof(char));
	*startTxtTimer = 60;
	SYS_setVIntCallback(titleVInt);
	PAL_fadeInAll(titlePalette,30,TRUE);
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
	PAL_fadeInPalette(PAL1,sonicPalette,30,TRUE);
	short ind = TILE_USER_INDEX;
	unsigned short basetileVRAM = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,ind);
	VDP_drawImageEx(BG_A,&presentingLogo,basetileVRAM,7,12,FALSE,TRUE);
	unsigned char timer = 130;
	while (1)
	{
		timer--;
		MDS_update();
		SYS_doVBlankProcess();
		if (timer == 30)
		{
			PAL_fadeOutPalette(PAL1,30,TRUE);
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
	unsigned char timer = 156;
	PAL_fadeInPalette(PAL0,segaPalette,30,TRUE);
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
		else if (timer == 30)
		{
			PAL_fadeOutPalette(PAL0,30,TRUE);
		}
		else if (timer == 0)
		{
			teamCredits();
		}
	}
}

int main(bool resetType)
{
	Z80_unloadDriver();
	MDS_init(mdsseqdat,mdspcmdat);
	for (unsigned char i = 0; i < 4; i++)
	{
		PAL_setPalette(i,palette_black,DMA);
	}
	segaScreen();
	while(TRUE)
	{
		MDS_update();
		SYS_doVBlankProcess();
	}
	return 0;
}
