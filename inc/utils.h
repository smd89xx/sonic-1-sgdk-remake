#pragma once

#define true 1
#define false 0;

extern bool isNTSC;
extern const unsigned char palFadeTime;
extern fix16* cycleTimer;
extern const unsigned char mdsFadePeak;
extern const unsigned char mdsFadeDecay;

s32 secToFrames(fix32 seconds);
u16 pixelToTile(u16 pixel);