#include "includes.h"

#define NUM_OPTS_LS 27

const u8 sfxStart = 5;
const u8 lsX = 3;
const u8 lsY = 4;
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
	{lsX+19, lsY+18, "SOUND TEST"},
};
u8* sndIndex;

static void updateSndInt()
{
	u16 basetileTextHot = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	VDP_drawTextEx(BG_A,"{  {",basetileTextHot,lsX+30,lsY+18,DMA);
	char sndTxt[3] = "00";
	intToHex(*sndIndex,sndTxt,2);
	VDP_drawTextEx(BG_A,sndTxt,basetileTextHot,lsX+31,lsY+18,DMA);
}

static void moveCursor(bool direction)
{
	u16 basetileText = TILE_ATTR(PAL0,FALSE,FALSE,FALSE);
	u16 basetileTextHot = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	char sndTxt[3] = "00";
	VDP_drawTextEx(BG_A,"{  {",basetileText,lsX+30,lsY+18,DMA);
	intToHex(*sndIndex,sndTxt,2);
	VDP_drawTextEx(BG_A,sndTxt,basetileText,lsX+31,lsY+18,DMA);
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
	if (*menuIndex == 26)
	{
		updateSndInt();
	}
	VDP_clearTileMapRect(BG_A,0,0,40,1);
}

static void freeLSMem()
{
	MEM_free(menuIndex);
	menuIndex = NULL;
	if (sndIndex != NULL)
	{
		MEM_free(sndIndex);
		sndIndex = NULL;
	}
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
	if ((changed & state & BUTTON_LEFT) && *menuIndex == 26)
	{
		MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
		(*sndIndex)--;
		updateSndInt();
	}
	else if ((changed & state & BUTTON_RIGHT) && *menuIndex == 26)
	{
		MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
		(*sndIndex)++;
		updateSndInt();
	}
	if ((changed & state & BUTTON_A) && *menuIndex == 26)
	{
		MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
		(*sndIndex) -= 0x10;
		updateSndInt();
	}
	else if ((changed & state & BUTTON_B) && *menuIndex == 26)
	{
		MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
		(*sndIndex) += 0x10;
		updateSndInt();
	}
	if ((changed & state & BUTTON_C) && *menuIndex == 26)
	{
		if (*sndIndex < sfxStart)
			{
				MDS_request(MDS_BGM,*sndIndex);
			}
			else
			{
				MDS_request(MDS_SE2,*sndIndex);
			}
	}
	if (changed & state & BUTTON_START)
	{
		if (*menuIndex == 26)
		{
			if (*sndIndex < sfxStart)
			{
				MDS_request(MDS_BGM,*sndIndex);
			}
			else
			{
				MDS_request(MDS_SE2,*sndIndex);
			}
			return;
		}
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
						freeLSMem();
						gameInit();
						break;
					}
					default:
					{
						freeLSMem();
						killExec(featureNotFound);
						break;
					}
				}
			}
		}
	}
}

void lvlSelect()
{
	s16 indEmblem = TILE_USER_INDEX;
	u16 basetileEmblem = TILE_ATTR_FULL(PAL2,FALSE,FALSE,FALSE,indEmblem);
	u16 basetileTextHot = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
	VDP_clearPlane(BG_B,TRUE);
	VDP_clearPlane(BG_A,TRUE);
	SPR_reset();
	SPR_update();
	VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
	VDP_setHorizontalScroll(BG_B,0);
	VDP_drawImageEx(BG_B,&lsBG,basetileEmblem,0,0,FALSE,TRUE);
	for (u8 i = 0; i < NUM_OPTS_LS; i++)
	{
		Option o = lsOptions[i];
		VDP_drawText(o.label,o.x,o.y);
	}
	VDP_drawTextEx(BG_A,lsOptions[0].label,basetileTextHot,lsOptions[0].x,lsOptions[0].y,DMA);
	VDP_drawText("{00{",lsX+30,lsY+18);
	PAL_fadeInAll(lvlSelectPalette,palFadeTime,TRUE);
	menuIndex = MEM_alloc(sizeof(u8));
	*menuIndex = 0;
	sndIndex = MEM_alloc(sizeof(u8));
	*sndIndex = 0;
	MDS_request(MDS_BGM,BGM_MUS_S2BLVS);
	JOY_setEventHandler(joyEvent_LS);
	while (1)
	{
		SYS_doVBlankProcess();
		MDS_update();
	}
}