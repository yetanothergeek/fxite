#!/bin/bash -e

SCITE_PROPS="/usr/share/scite/"

function sciterip
{
awk '{if (/\\$/) {gsub(/\\$/,""); printf("%s",$0)} else {print $0}}' \
"${SCITE_PROPS}"/[a-z]*.properties \
  | awk '/./ && !/^[ \t]*#/ {gsub(/[ \t]+/," "); gsub(/^ /,"");  print}' \
    | awk '
     !( /^style\./ ||
        /^comment\./ ||
        /\$\(/ ||
        /^import / ||
        /^[a-zA-Z0-9.]+=$/ ||
        /^lexer\./ ||
        /^shbang\./ ||
        /^braces\./ ) {
          print
        }'
}

if [ $# -gt 0  ]
then
  case "$1" in 
  "file")
  echo "filetypes={"
  sciterip | awk '/^file\.patterns\./ {
    gsub(/^file\.patterns\./,"");
    gsub(/;/,"|");
    sub(/=/, "=\"");
    sub(/$/,"\"");
    sub(/^web=/, "html=");
    while (/\..*=/) {sub(/\./,"_")}
    printf("%s,\n",$0);
    }' | sort
  echo "}"
  ;;
  "words")

cat << EOF
module("wordlist")
keywords={}
keywords2={}
keywords3={}
keywords4={}
keywords5={}
keywords6={}
keywords7={}
keywords8={}
keywordclass={}
keywordclass2={}
keywordclass3={}
keywordclass4={}
keywordoptional1={}
keywordoptional2={}
docbook={}
hypertext={}
html5={}
vxml={}
EOF

    for A in "ada" "metapost" "metafun" "pascal" "spice" "primitives" "macros" "tex" "etex" "latex" "pdflatex" "context"
    do
      echo "keywordclass.$A={}"
    done
    echo "local keywordstandard={}"
    echo "keywords.pascal={}"

cat << EOF
keywords.tcl={}
keywords2.tcl={}
keywords3.tcl={}
keywords4.itcl={}
keywords5.tcl={}
keywordclass.context.all={}
keywordclass.macros.context={}
keywordclass.macros.context.undocumented={}
keywordclass.macros.eplain={}
keywordclass.macros.latex={}
keywordclass.macros.plain={}
keywordclass.primitives.pdftex={}
EOF
  
     awk '{
       if (/\\$/) {
         gsub(/\\$/,"");
         printf("%s",$0)
       } else {
         print $0
       }
     }'  "./util/Embedded.properties" \
     | awk '{ 
         gsub(/[ \t]+/, " ");
         sub(/^ /,"");
         sub(/ $/,"");
         if (/./) {print} }' | \
         awk \
           '! ( /^style\./   || /^file\./ \
             || /^word\./    || /^filter\./ \
             || /^comment\./ || /^preprocessor\./ \
             || /^indent\./  || /^braces\./ \
             || /^tabsize\./ || /^use\./ \
             || /^command\./ || /^calltip\./ \
             || /^fold\./    || /^shbang\./ \
             || /^tab\./     || /^colour\./ \
             || /^source\./  || /^menu\./ \
             || /caret\./    || /^statement\./ \
             || /^export\./  || /^position\./ \
             || /^tabbar\./  || /^selection\./ \
             || /^tabsize\./ || /^statusbar\./ \
             || /^font\./    || /^print\./ \
             || /^import\>/  || /^if\>/ || /^#/ \
             ) {
               if (/\.\$\(file\.patterns\./) {
                 $0=gensub(/(.*)\.\$\(file\.patterns\.([^)]+)\)/, "\\1.\\2",1)
               }
               sub(/= */, "=");
               sub(/\.\*\./, ".");
               gsub(/"/,"\\\"")
               gsub(/\\\\"/,"\\\"")
               print;
             }' | uniq | awk '{
               if (/=\$\(.*\)$/) {
                 printf("%s\n", gensub(/([^=]+)=\$\((.*)\)$/,"\\1=\\2",1))
               } else {
                 printf("%s\n", gensub(/([^=]+)=(.*)$/,"\\1=\"\\2\"",1))
               }
             }' | awk '\
                     /^keyw/ || /^[cf]pu_instruction/ \
                  || /^register/  || /^ext_instruction/ \
                  || /^directive/ || /^docbook\./ \
                  || /^bash_keywords/ || /^vxml\./ \
                  || /^hypertext\./ || /^html5\./ \
               {
               gsub(/\) \$\(/, "..");
               gsub(/\)\|\$\(/," or ");
               sub(/="\$\(/, "=");
               sub(/\.attributes\) public !doctype/, ".attributes..\" public !doctype");
               sub(/^keywords=/, "keywords.something=");
               sub(/^keywordclass\.pascal=/,"keywordclass.pascal.something=");
               sub(/^keywords\.pascal=keywordclass\.pascal/,"keywords.pascal.something=keywordclass.pascal.something");
               sub(/^keywords\.pascal\.package=keywordclass\.pascal/,"keywords.pascal.package=keywordclass.pascal.something");
               sub(/^keywordclass\.primitives\.pdftex=/, "keywordclass.primitives.pdftex.something=");
               sub(/\.\.keywordclass\.primitives.pdftex\.\./,"..keywordclass.primitives.pdftex.something..");
               sub(/^keywordclass\.context\.all=/ , "keywordclass.context.all.something=");
               print
               }' 
cat << EOF
keywords.docbook=docbook.attributes
keywords2.docbook=docbook.elements42
docbook=nil
keywords.bash=bash_keywords1
keywords2.bash=bash_keywords2
bash_keywords1=nil
bash_keywords2=nil
keywords1=keywords
keywords=nil
keywordoptional1=nil
keywordoptional2=nil
keywordclass=nil
keywordclass2=nil
keywordclass3=nil
keywordclass4=nil
ext_instruction=nil
cpu_instruction=nil
directive_nasm=nil
directive=nil
fpu_instruction=nil
directive_operand_nasm=nil
cpu_instruction2=nil
directive_operand=nil
register=nil
EOF

  ;;
"html")

echo 'ht_vals={'
awk '/^val[ \t]+SCE_H[^A]/ {sub(/val[ \t]+/,""); printf("%s,\n", $0) }' ../fxscintilla/scintilla/include/Scintilla.iface
echo '}'

  echo "hypertext={}"
  awk  '/^style\.hypertext\.[0-9]+/ \
{
  if  (/\<fore:#.*\<back:#/) {
    $0=gensub(/^style\.hypertext\.([0-9]+).*\<fore:#([0-9A-Fa-f]+).*\<back:#([0-9A-Fa-f]+).*/,"\\1 \"#\\2\" \"#\\3\"",1)
  } else {
    if  (/\<back:#/) {
      $0=gensub(/^style\.hypertext\.([0-9]+).*\<back:#([0-9A-Fa-f]+).*/,"\\1 _DEFLT_FG \"#\\2\"",1)
    } else {
      if  (/\<fore:#/) {
      $0=gensub(/^style\.hypertext\.([0-9]+).*\<fore:#([0-9A-Fa-f]+).*/,"\\1 \"#\\2\" _DEFLT_BG",1)
      } else {
        $0=gensub(/^style\.hypertext\.([0-9]+).*/,"\\1 _DEFLT_FG _DEFLT_BG", 1)
      }
    }
  }
  printf("hypertext[%d]={\047%s\047,\047%s\047}\n", $1+1, $2, $3)
}' "${SCITE_PROPS}html.properties"
  ;;
  esac
else
  sciterip| awk '!/^file\.patterns\./ {gsub(/^file\.patterns\./,""); print }' | sort
fi




