#!/usr/bin/env bash

# This script will build a minimal FOX-1.7 library on MINIX, 
# along with a few Fox applications. Works for me with the 
# 2011-03-03 snapshot, from:
#   http://www.fox-toolkit.org/ftp/fox-snapshot.tar.gz
#
# Requires the gcc, pth, and x11 packages.
#
# The script is intended to be run from inside the top-level FOX source directory.
# By default it is configured to install in /usr/local, but you can change
# the --prefix option below to /usr/pkg or anywhere you like.
# This script only builds stuff, the actual "make install" is up to you...

set -e 

CXX="g++ -I/usr/pkg/include" \
LDFLAGS='-L/usr/pkg/lib' \
./configure \
--disable-jpeg \
--disable-jp2 \
--disable-png \
--disable-tiff \
--disable-zlib \
--disable-bz2lib \
--without-xft \
--without-xshm \
--without-shape \
--without-xcursor \
--without-xrender \
--without-xrandr \
--without-xfixes \
--without-xinput \
--without-xim \
--without-opengl \
--prefix=/usr/local

gmake
