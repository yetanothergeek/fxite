#!/bin/bash

abs=$(readlink -f "${0}")
rel="./util/${0##*/}"

if [ -f "${rel}" ] && [ -f "${abs}" ] && [ $(stat -c "%i" ${abs}) -eq $(stat -c "%i" ${rel}) ]
then
  unset abs
  unset rel
else
  dir=${abs%/*}
  printf "***FATAL***\nWorking directory must be:\n  %s/\n" "${dir%/*}" 1>&2
  exit 1
fi



find "." -maxdepth 1 -type d -name 'fxite-*.*.*' | while read DIR
do
  chmod +w $(find "${DIR}")
  rm -r "${DIR}"
done

rm -f fxite-*.tar.gz

rm -f po/*.pot

rm -f 'configure' 'src/fxite' 'sl.obj' 'helptext.h' 'help_lua.h' 'tags' 'gmon.out'
rm -f callgrind.out.[0-9]* *.log

find  -name 'Makefile' \
  -or -name 'Makefile.in' \
  -or -name '*.o' \
  -or -name '*.lo' \
  -or -name '*.la' \
  -or -name '*.a' \
  -or -name '*.exe' \
  -or -name '.deps' \
  -or -name '.libs' \
  -or -name 'autom4te.cache' \
  -or -name 'config.guess' \
  -or -name 'config.sub' \
  -or -name 'depcomp' \
  -or -name 'install-sh' \
  -or -name 'ltmain.sh' \
  -or -name 'missing' \
  -or -name 'missing' \
  -or -name 'config.log' \
  -or -name 'config.status' \
  -or -name 'libtool' \
  -or -name '*~' \
| while read name
do
  rm -rf ${name}
done
