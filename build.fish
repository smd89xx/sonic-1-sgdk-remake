#!/usr/bin/fish
make mdsdrv
make release
if test -e /usr/bin/kega-fusion == 0
{
    kega-fusion ./out/rom.bin
}
