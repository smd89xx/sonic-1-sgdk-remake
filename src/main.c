#include "includes.h"

enum regions {ntscJPN = 0x20, ntscUSA = 0xA0, palEUR = 0xE0, palJPN = 0x60};

static void joyEvent_Title(u16 joy, u16 changed, u16 state)
{
	if (joy != JOY_1)
	{
		return;
	}
	if (changed & state & BUTTON_START)
	{
		PAL_fadeOutAll(30,TRUE);
		MDS_fade(0x7F,4,TRUE);
	}
}

static void title()
{
	VDP_clearPlane(BG_A,TRUE);
	SPR_init();
	VDP_drawText("snik embelm goes here",0,0);
	PAL_fadeInAll(titlePalette,30,TRUE);
	JOY_setEventHandler(joyEvent_Title);
	MDS_request(MDS_BGM,BGM_MUS_CLI2);
	while(1)
	{
		MDS_update();
		SYS_doVBlankProcess();
	}
}

static void teamCredits()
{
	VDP_clearPlane(BG_A,TRUE);
	PAL_fadeInPalette(PAL1,sonicPalette,30,TRUE);
	VDP_setTextPalette(PAL1);
	VDP_drawText("snick teem pesents text goe here",0,0);
	unsigned char timer = 176;
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
	segaScreen();
	while(TRUE)
	{
		MDS_update();
		SYS_doVBlankProcess();
	}
	return 0;
}
