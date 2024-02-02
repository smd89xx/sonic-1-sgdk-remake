#include "includes.h"

const string stopStrings[4] = {"ERROR_USER_GENERIC","ERROR_VALUE_OUT_OF_RANGE","ERROR_XGM_Z80_OVERLOAD","ERROR_FUNCTION_UNIMPLEMENTED"};
u32* stopcode_public;
const string bsodStrings[14] = {"A problem has been detected and the","current process has ended to prevent","damage to your console.","If this is the first time you've seen","this screen, reset your console. If you","see this screen again, press the","START button. This will make an attempt","to fix any problems you are having.","If problems continue, contact the","developer for a potential fix, if any","can be provided, or erase all save data","and start over. A backup is recommended.","Technical Information:","***** STOP: 0x"};
static void joyEvent_BSOD(u16 joy, u16 changed, u16 state)
{
    u16 basetile = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
    if (changed & state & BUTTON_START)
    {
        VDP_drawTextEx(BG_A,"Fixing errors...",basetile,0,22,DMA);
        MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
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
    VDP_setScreenHeight240();
    VDP_setScrollingMode(HSCROLL_PLANE,VSCROLL_PLANE);
    VDP_setHorizontalScroll(BG_A,0);
    VDP_setVerticalScroll(BG_A,0);
    SPR_end();
    PAL_interruptFade();
    u16 basetile = TILE_ATTR(PAL1,FALSE,FALSE,FALSE);
    u8 i = 0;
    for (i; i < 4; i++)
    {
        PAL_setPalette(i,palette_black,DMA);
    }
    for (i = 0; i < 3; i++)
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
    string scStr = MEM_alloc(9*sizeof(u8));
    intToHex(stopcode,scStr,8);
    VDP_drawTextEx(BG_A,scStr,basetile,14,18,DMA);
    PAL_setColor(0,0x0800);
    PAL_setPalette(PAL1,sonicPalette,DMA);
    stopcode_public = &stopcode;
    MDS_request(MDS_BGM,BGM_MUS_SOR2GAMEOVER);
    JOY_setEventHandler(joyEvent_BSOD);
    while (1)
    {
        MDS_update();
        SYS_doVBlankProcess();
    }
}