#include "includes.h"

static void teamCredits()
{
	VDP_clearPlane(BG_A,TRUE);
	PAL_fadeInPalette(PAL1,sonicPalette,palFadeTime,TRUE);
	s16 ind = TILE_USER_INDEX;
	u16 basetileVRAM = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,ind);
	VDP_drawImageEx(BG_A,&presentingLogo,basetileVRAM,7,12,FALSE,TRUE);
	s32 timer = secToFrames(FIX32(1.19));
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

void segaScreen()
{
	s16 ind = TILE_USER_INDEX;
	u16 basetileVRAM = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,ind);
	s32 timer = secToFrames(FIX32(1.643));
	PAL_fadeInPalette(PAL0,segaPalette,palFadeTime,TRUE);
	VDP_drawImageEx(BG_A,&segaLogo,basetileVRAM,13,12,FALSE,TRUE);
	while (1)
	{
		timer--;
		MDS_update();
		SYS_doVBlankProcess();
		if (timer == 126)
		{
			MDS_request(MDS_SE1,BGM_SFX_SEGA);
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