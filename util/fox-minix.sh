#!/usr/bin/env bash

# This script will modify the FOX-1.7 sources, and build the
# Fox library, along with a few Fox applications.
# Works for me with the 2011-01-26 snapshot, from:
#   http://www.fox-toolkit.org/ftp/fox-snapshot.tar.gz
#
# Requires:
#   autoconf, automake, bash, binutils, gcc, gettext-lib,
#   gmake, gsed, libiconv, libtool, m4, pth, x11
#
# The script is intended to be run from inside the top-level FOX source directory.
# By default it is configured to install in /usr/local, but you can change
# the MY_PREFIX variable below to /usr/pkg or anywhere you like.
# This script only builds stuff, the actual "make install" is up to you...



MY_PREFIX="/usr/local"

set -e

[ "$1" = "-t" ] && SKIP_TESTS="no" || SKIP_TESTS="yes"

TOP_DIR_OK=no
case $PWD in
  /*/fox-1.[78].*)
    [ -d lib ] && [ -d include ] && [ -d adie ] && TOP_DIR_OK=yes
  ;;
esac

if [ x$TOP_DIR_OK != xyes ]
then
  echo "This does not look like a FOX-1.7 source directory to me."
  exit 1
fi

printf "Tweaking some files..."
rm -f $(find . -type f -name Makefile.in)

gsed -i 's/^LIBGL=.*/LIBGL=/' configure.ac

gsed -i 's/^LIBGLU=.*/LIBGLU=/' configure.ac

gsed -i 's/^CXXFLAGS=""//' configure.ac

gsed -i 's/\<chart\>/ /' Makefile.am

gsed -i 's/\<shutterbug\>/ /' Makefile.am

gsed -i 's/FXDLL\.h//' include/Makefile.am

gsed -i 's/.*FXDLL\.h.*//' include/fx.h

gsed -i 's/FXDLL\.cpp//' lib/Makefile.am

gsed -i 's/FXMemMap\.h//' include/Makefile.am

gsed -i 's/.*FXMemMap\.h.*//' include/fx.h

gsed -i 's/FXMemMap\.cpp//' lib/Makefile.am

gsed -i 's/.*semaphore\.h.*//'  include/xincs.h

gsed -i 's#\(.*\)\<sem_\(.*\)#// \1sem_\2#' lib/FXThread.cpp

gsed -i 's/\<__APPLE__\>/__minix/g' lib/FXThread.cpp

gsed -i 's/\<if.*sysctlbyname.*activecpu.*{/if(1){/' lib/FXThread.cpp

gsed -i 's/\<WIN32\>/__minix/' lib/FXExpression.cpp

gsed -i 's/\<WIN32\>/__minix/' lib/FXRealSlider.cpp

gsed -i 's/\<WIN32\>/__minix/' lib/FXRealSpinner.cpp

echo "  done."

for CMD in \
 'aclocal --force' \
 'libtoolize --force --copy' \
 'automake --foreign --add-missing --copy' \
 'autoconf' \
 'autoreconf'
do
  echo "Running $CMD"
  $CMD
done


echo "Running ./configure"

CXXFLAGS="-I/usr/pkg/include -DGenericEvent=35" \
LDFLAGS='-L/usr/pkg/lib -lpthread' \
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
--prefix="$MY_PREFIX"


find . -type f -name Makefile | while read M
do
  gsed -i 's/-rdynamic\>/ /g' $M
done


echo 'int main(void) { return 1; }' > tests/memmap.cpp


if [ "$SKIP_TESTS" = "yes" ]
then
  mv tests/Makefile tests/Makefile.orig
cat << EOF  > tests/Makefile
all:
install:
install-strip:
clean:
	\$(MAKE) -f Makefile.orig clean
distclean:
	\$(MAKE) -f Makefile.orig distclean
dist:
	\$(MAKE) -f Makefile.orig dist
distcheck:
	\$(MAKE) -f Makefile.orig distcheck
%:
	\$(MAKE) -f Makefile.orig \$@
EOF
  gmake
  echo "Build of FOX library and applications is complete."
  echo 'To build the test programs, do:'
  echo "  gmake -C tests -f Makefile.orig"
else
  gmake
fi

# gmake install-strip DESTDIR=/usr/tmp/fox.pkg
