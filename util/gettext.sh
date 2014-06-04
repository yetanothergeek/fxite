#!/bin/bash -e

[ -e src/Makefile.am ] || ls src/Makefile.am # silly test for working dir.

rm -rf po m4 ABOUT-NLS config.rpath *~

mv ChangeLog ChangeLog.TMP

gettextize .

sed -i 's/ po po / po /' Makefile.am
sed -i 's/^EXTRA_DIST =.*/EXTRA_DIST = \\/' Makefile.am
sed -i 's/AC_OUTPUT(.*/AC_OUTPUT( /' configure.ac

cp po/Makevars.template po/Makevars

rm -f *~

grep -l '"intl\.h"' {jef,mnumgr,src}/*.{c,cpp,h} | sort > po/POTFILES.in

sed \
  's/^COPYRIGHT_HOLDER.*$/COPYRIGHT_HOLDER = Jeffrey Pohlmeyer <yetanothergeek@gmail.com>/' \
  po/Makevars.template > po/Makevars


mv ChangeLog.TMP ChangeLog

