#include "includes.h"

bool isNTSC;
const u8 palFadeTime = 30;
const u8 mdsFadePeak = 0x7F;
const u8 mdsFadeDecay = 4;
fix16* cycleTimer;

/// @brief Converts seconds to NTSC/PAL frames (depending on console region).
/// @param seconds Time, in seconds.
/// @return Time, in frames.
s32 secToFrames(fix32 seconds)
{
    s32 secondsInt = fix32ToRoundedInt(seconds);
    if (isNTSC)
    {
        return (secondsInt * 60) + (palFadeTime << 1);
    }
    else
    {
        return (secondsInt * 50) + (palFadeTime << 1);
    }
}

/// @brief Converts tile coordinate to pixel coordinate.
/// @param pixel Value to convert.
/// @return Equivalent of a tile coordinate in pixels.
u16 pixelToTile(u16 pixel)
{
	return pixel << 3;
}