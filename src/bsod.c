#include "includes.h"

const char stopStrings[4][41] = {"ERROR_USER_GENERIC","ERROR_VALUE_OUT_OF_RANGE","ERROR_XGM_Z80_OVERLOAD","ERROR_FUNCTION_UNIMPLEMENTED"};
u32* stopcode_public;
static void joyEvent_BSOD(u16 joy, u16 changed, u16 state)
{
    u16 basetile = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
    if (changed & state & BUTTON_START)
    {
        VDP_drawTextEx(BG_A,"Fixing errors...",basetile,0,22,DMA);
        switch (*stopcode_public)
        {
        default:
        {
            SYS_hardReset();
            break;
        }
        }
    }
}

void killExec(u32 stopcode)
{
    VDP_loadFont(&bsodFont,DMA);
    VDP_clearPlane(BG_A,TRUE);
    VDP_clearPlane(BG_B,TRUE);
    VDP_setWindowVPos(FALSE,0);
    SPR_end();
    PAL_setColor(0,0x0800);
    PAL_setPalette(PAL1,sonicPalette,DMA);
    u16 basetile = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
    char bsodStrings[14][41] = {"A problem has been detected and the","current process has ended to prevent","damage to your console.","If this is the first time you've seen","this screen, reset your console. If you","see this screen again, press the","START button. This will make an attempt","to fix any problems you are having.","If problems continue, contact the","developer for a potential fix, if any","can be provided, or erase all save data","and start over. A backup is recommended.","Technical Information:","***** STOP: 0x"};
    u8 i = 0;
    u32 errPtr;
    u32 errVal;
    for (i; i < 3; i++)
    {
        VDP_drawTextEx(BG_A,bsodStrings[i],basetile,0,i,DMA);
    }
    VDP_drawTextEx(BG_A,stopStrings[stopcode],basetile,0,4,DMA);
    for (i = 3; i < 8; i++)
    {
        VDP_drawTextEx(BG_A,bsodStrings[i],basetile,0,i+3,DMA);
    }
    for (i = 8; i < 12; i++)
    {
        VDP_drawTextEx(BG_A,bsodStrings[i],basetile,0,i+4,DMA);
    }
    for (i = 12; i < 14; i++)
    {
        VDP_drawTextEx(BG_A,bsodStrings[i],basetile,0,i+5,DMA);
    }
    switch (stopcode)
    {
    case 1:
    {
        errPtr = &level;
        errVal = (level[0] << 8) + level[1];
        break;
    }    
    default:
    {
        errPtr = NULL;
        errVal = NULL;
        break;
    }
    }
    char scStr[9] = "00000000";
    intToHex(stopcode,scStr,8);
    VDP_drawTextEx(BG_A,scStr,basetile,14,18,DMA);
    intToHex(errPtr,scStr,8);
    VDP_drawTextEx(BG_A,scStr,basetile,14,19,DMA);
    intToHex(errVal,scStr,8);
    VDP_drawTextEx(BG_A,scStr,basetile,14,20,DMA);
    VDP_drawTextEx(BG_A,"** ADDRESS: 0x",basetile,0,19,DMA);
    VDP_drawTextEx(BG_A,"** CONTENT: 0x",basetile,0,20,DMA);
    MDS_fade(0x7F,4,TRUE);
    stopcode_public = MEM_alloc(sizeof(u32));
    JOY_setEventHandler(joyEvent_BSOD);
    while (1)
    {
        MDS_update();
        SYS_doVBlankProcess();
    }
}