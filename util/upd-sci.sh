#!/bin/bash -e


fail ()
{
  echo "${0##*/}: ERROR"
  printf "$1" >&2
  echo
  exit 1
}

abs=$(readlink -f "${0}")
rel="./util/${0##*/}"


if [ -f "${rel}" ] && [ -f "${abs}" ] && [ $(stat -c "%i" ${abs}) -eq $(stat -c "%i" ${rel}) ]
then
  unset abs
  unset rel
else
  dir=${abs%/*}
  fail "Working directory should be:\n ${dir%/*}/"
fi


if [ ${#} -ne 1 ]
then
  echo "Usage: ${0} <fxscintilla-directory>"
  exit 1
fi


[ -d "${1}" ] || fail "Source object is not a directory:\n ${1}"
[ -x "${1}" ] || fail "Search permission denied for directory:\n ${1}"
[ "${1##*/}" = "fxscintilla" ] || fail "Source directory must be named 'fxscintilla'"

FXSCI_DIR='fxscintilla'

rm -rf "${FXSCI_DIR}"

mkdir -p "${FXSCI_DIR}"

find ${1} -name '*.cxx' \
-or -name '*.h' \
-or -name 'version.txt' \
-or -name 'License.txt' \
-or -name 'COPYRIGHTS' \
-or -name 'ChangeLog' \
-or -name 'COPYING' \
-or -name 'Scintilla.iface' \
| while read FILE
do
  cp "${FILE}" "${FXSCI_DIR}"
done

cd "${FXSCI_DIR}"
chmod 644 *


for UNUSED in \
APDL \
AU3 \
AVE \
AVS \
Abaqus \
Baan \
Bullant \
CoffeeScript \
Crontab \
Csound \
DMIS \
ECL \
EScript \
Eiffel \
Forth \
GAP \
Gui4Cli \
Inno \
Kix \
Lout \
MMIXAL \
MSSQL \
Matlab \
MySQL \
Nimrod \
Nsis \
Opal \
PowerPro \
PowerShell \
Progress \
Rebol \
Scriptol \
Specman \
Spice \
TACL \
TADS3 \
TAL \
TCMD \
VB \
Eiffelkw \
ESCRIPT \
Nncrontab \
Octave \
REBOL \
VBScript \
OScript \
VisualProlog \
KVIrc \
STTXT \
DMAP \
Rust \
Hex \
IHex \
Srec \
TEHex
do
  rm -f "Lex${UNUSED}.cxx"
  awk '!/^\tLINK_LEXER\(lm'${UNUSED}'\);$/' Catalogue.cxx > Catalogue.tmp
  mv Catalogue.tmp Catalogue.cxx
done

sed -i 's#setSingleCharOp\.Contains(sc\.chNext)# (0) /* setSingleCharOp.Contains(sc.chNext) */ #' LexBash.cxx

echo 'noinst_LIBRARIES = libfxscintilla.a' > "Makefile.am"
echo 'libfxscintilla_a_CXXFLAGS = $(AM_CXXFLAGS) $(FOX_CFLAGS) -DSCI_LEXER -DFOX' >> "Makefile.am"
printf '\nlibfxscintilla_a_SOURCES = \\\nversion.h'  >> "Makefile.am"


/bin/ls -1 --color=never *.cxx | awk '/\.cxx$/ {printf(" \\\n%s", $1)}' >> "Makefile.am"

echo  >> "Makefile.am"

cat << EOF > CMakeLists.txt
FILE (GLOB SOURCES RELATIVE "\${CMAKE_CURRENT_SOURCE_DIR}" *.cxx)

INCLUDE_DIRECTORIES (\${FOX_INCLUDE_DIRS})
ADD_DEFINITIONS (-DSCI_LEXER -DFOX)

ADD_LIBRARY (
    fxscintilla
    STATIC
    \${SOURCES}
)

EOF



printf '\nnoinst_HEADERS =  \\\nversion.h'  >> "Makefile.am"

/bin/ls -1 --color=never *.h | awk '/\.h$/ {printf(" \\\n%s", $1)}' >> "Makefile.am"

echo >> "Makefile.am"



cd ..
for TAG in MAJOR_VERSION MINOR_VERSION PATCH_LEVEL
do
  export $TAG=`awk -F= '/^[ \t]*'${TAG}'=[0-9]*[ \t]*$/ {gsub(/[ \t]*/,""); print $2}' "${1}/configure.ac"`
done


printf 'FXString FXScintilla::version()\n{\n  return "%s.%s.%s";\n}\n\n' \
"${MAJOR_VERSION}" "${MINOR_VERSION}" "${PATCH_LEVEL}" > "${FXSCI_DIR}/version.h"


