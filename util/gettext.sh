#!/bin/bash -e

[ -e src/Makefile.am ] || ls src/Makefile.am # silly test for working dir.

rm -rf po m4 ABOUT-NLS config.rpath *~

sed -i 's# *po/Makefile.in *# #g' configure.in


echo 'SUBDIRS=src' > Makefile.am

mv ChangeLog ChangeLog.TMP

gettextize .

cp po/Makevars.template po/Makevars

rm -f *~

grep -l '"intl\.h"' src/*.{c,cpp,h} | sort > po/POTFILES.in

sed \
  's/^COPYRIGHT_HOLDER.*$/COPYRIGHT_HOLDER = Jeffrey Pohlmeyer <yetanothergeek@gmail.com>/' \
  po/Makevars.template > po/Makevars


mv ChangeLog.TMP ChangeLog
