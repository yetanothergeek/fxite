#!/bin/sh -e

./util/cleanup.sh

for CMD in \
  'aclocal --force -I m4' \
  'autoconf' \
  'automake --gnu --add-missing --copy' 
do
  echo "Running ${CMD}"
  $CMD
done


[ "${CFLAGS}" = "" ] && CFLAGS="-Wall -ggdb3"
[ "${CXXFLAGS}" = "" ] && CXXFLAGS=$CFLAGS

export CFLAGS
export CXXFLAGS

./configure ${@}

