#!/bin/bash -e

cd src

convert fxite.xpm icon32x32.ico

windres -i icon32x32.rc -o icon32x32.coff

