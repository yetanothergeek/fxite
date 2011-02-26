#!/bin/sh

case "$(uname)" in
  Linux)
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
  ;;
  *)
    case "$(pwd)" in
      *fxite*)
         if [ -f fxscintilla/FXScintilla.h ] \
          && [ -d src/languages ] \
          && [ -f lua/Makefile.am ] \
          && [ -f util/cleanup.sh ] \
          && diff -q "${0}" util/cleanup.sh
         then
         :
         else
          echo "***FATAL***\nI do not recognize this directory structure." 1>&2
          exit 1
         fi
      ;;
      *)
        echo "***FATAL***\nWorking directory name must match *fxite*" 1>&2
        exit 1
      ;;
    esac
  ;;
esac

for DIR in $(echo fxite-*.*)
do
  if [ -d "${DIR}" ]
  then
    chmod +w $(find "${DIR}")
    rm -r "${DIR}"
  fi
done

rm -f fxite-*.tar.gz

rm -f po/*.pot 'po/remove-potcdate.sed' 'po/POTFILES' 'aclocal.m4'

rm -f 'configure' 'src/fxite' 'sl.obj' 'helptext.h' 'help_lua.h' 'tags' 'gmon.out'
rm -f callgrind.out.[0-9]* *.log

find . -name 'Makefile' \
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
  rm -rf "${name}"
done
