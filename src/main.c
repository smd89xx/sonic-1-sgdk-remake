#include "includes.h"

enum regions {ntscJPN = 0x20, ntscUSA = 0xA0, palEUR = 0xE0, palJPN = 0x60};
enum maxVals {lvlMax = 7, actMax = 3, livesMax = 99};
const u8* region = (u8 *)0x00A10001;

int main(bool resetType)
{
	if (!resetType)
	{
		SYS_hardReset();
	}
	for (u8 i = 0; i < 4; i++)
	{
		PAL_setPalette(i,palette_black,DMA);
	}
	if (*region == ntscJPN || *region == ntscUSA)
	{
		isNTSC = true;
	}
	else
	{
		isNTSC = false;
	}
	Z80_unloadDriver();
	VDP_loadFont(&titleFont,DMA);
	MDS_init(mdsseqdat,mdspcmdat);
	segaScreen();
	while(true)
	{
		MDS_update();
		SYS_doVBlankProcess();
	}
	return 0;
}
