#pragma once

enum collisionTypes
{
    ctTop,
    ctBottom,
    ctLeft,
    ctRight,
    ctHSpring,
    ctVSpring,
    ctSlopeSE,
    ctSlopeSW,
};

extern Map* lvlBG;
extern Map* lvlFG;
extern unsigned char level[2];
static void joyEvent_Game(u16 joy, u16 changed, u16 state);
void gameInit();