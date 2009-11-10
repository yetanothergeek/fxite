#!/bin/bash -e

LANGTMP="./src/languages/"


LANGLIST_H="./src/langlist.h"

mkdir -p "${LANGTMP}"

./util/scite-rip.sh "file" > ./util/wildcards.lua
./util/scite-rip.sh "words" > ./util/wordlist.lua
./util/scite-rip.sh "html" > ./util/htmlstyle.lua


#freebasic  vbscript

#fxite --dump-lexers 2>&1 | while read ID NAME NDESC
cat "./util/numdesc.list" | while read NAME NDESC
do
  case "${NAME}" in
    null)
      continue
    ;;
    sorcins|clarionnocase|magiksf)
      continue
    ;;
    blitzbasic|powerbasic|vb|purebasic|opal|mmixal|nncrontab)
      continue
    ;;
    abaqus|powerpro|powershell|mssql|gui4cli|kix|progress|au3)
      continue
    ;;
    escript|php|phpscript|bullant|cppnocase|clarion|mysql|baan|matlab|octave)
      continue
    ;;
    csound|tads3|apdl|nimrod|ave|gap|lout|lot|TAL|TACL|spice|specman|rebol|scriptol)
      continue
    ;;
    inno|nsis|vbscript|asp)
      continue
    ;;
    eiffel|eiffelkw|forth)
      continue
    ;;
    pov|po|asn1|SML|yaml)
      #tex|metapost|
      continue
    ;;
    props)
      NAME="Properties"
    ;;
    asy)
      NAME="Asymptote"
      continue
    ;;
    hypertext)
      NAME="html"
    ;;
    PL/M)
      NAME="PLM"
      continue
    ;;
  esac
  OUTFILE="${LANGTMP}"$(echo "$NAME" | awk '{gsub(/[^A-Za-z0-9]/,"_"); print(tolower($0))}')".h"
  [ -e "${OUTFILE}" ] && echo "File exists: ${OUTFILE}" && exit 1
  ./util/lexgen.lua "${NAME}" "${NDESC}" > "${OUTFILE}"
done

if false
then

cat << EOF > "${LANGTMP}/c.h"
#define c_style cpp_style
static const char* c_words[]= {
  "",
  "",
  "",
  "",
  "",
  NULL
};
static const char* c_mask = "*.c|*.h";
static const char* c_apps = "tcc";
static LangStyle LangC = {
  "c",
  SCLEX_CPP,
  c_style,
  (char**)c_words,
  (char*)c_mask,
  (char*)c_apps,
  0,
  TABS_DEFAULT
};
EOF


cat << EOF > "${LANGTMP}/javascript.h"
#define javascript_style cpp_style
static const char* javascript_words[]= {
  "",
  "",
  "",
  "",
  "",
  NULL
};
static const char* javascript_mask = "*.js";
static const char* javascript_apps = NULL;
static LangStyle LangJavaScript = {
  "javascript",
  SCLEX_CPP,
  javascript_style,
  (char**)javascript_words,
  (char*)javascript_mask,
  (char*)javascript_apps,
  0,
  TABS_DEFAULT
};
EOF

cat << EOF > "${LANGTMP}/java.h"
#define java_style cpp_style
static const char* java_words[]= {
  "",
  "",
  "",
  "",
  "",
  NULL
};
static const char* java_mask = "*.java";
static const char* java_apps = NULL;
static LangStyle LangJava = {
  "java",
  SCLEX_CPP,
  java_style,
  (char**)java_words,
  (char*)java_mask,
  (char*)java_apps,
  0,
  TABS_DEFAULT
};
EOF

fi


(
cd "./src/"
LANGTMP="./languages/"
printf "#include \"%shtml.h\"\n" "${LANGTMP}"
printf "#include \"%scpp.h\"\n" "${LANGTMP}"
find "${LANGTMP}" -name '*.h' -not -name 'html.h' -not -name 'cpp.h' | sort | awk '{printf("#include \"%s\"\n", $0)}'
echo
echo 'LangStyle languages[]={'
awk '/^static LangStyle / {printf("  %s,\n", $3)}' "${LANGTMP}/"*".h"
printf '  LangNULL\n};\n\n'
) > "$LANGLIST_H"
