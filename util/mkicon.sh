#!/bin/bash -e

cd src

convert icon32x32.xpm icon32x32.ico

~/mingw32/bin/i386-mingw32-windres -i icon32x32.rc -o icon32x32.coff

