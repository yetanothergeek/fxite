-- The stuff defined in this file is either missing from SciTE's config files, 
-- or there is something I didn't like about SciTE's definitions, or I'm just
-- trying to match the way the previous versions of FXiTe did things.

named_fg_styles={
  colour_number='NUMBER_FG',
  colour_operator='OPERTR_FG',
  colour_other_comment='COMMNT_FG',
  colour_other_operator='OPERTR_FG',
  colour_preproc='PREPRC_FG',
  colour_string='STRING_FG',
  colour_text_comment='COMMNT_FG',
  colour_char='STRING_FG',
  colour_code_comment_box='COMMNT_FG',
  colour_code_comment_doc='COMMNT_FG',
  colour_code_comment_document='COMMNT_FG',
  colour_code_comment_line='COMMNT_FG',
  colour_code_comment_nested='COMMNT_FG',
  colour_embedded_comment='COMMNT_FG',
}



shbang_apps={
  ruby   = "ruby",
  bash   = "ash|bash|dash|sh",
  lua    = "lua",
  c      = "tcc",
  perl   = "perl",
  python = "python",
}



masks={
po='*.po|*.pot',
d='*.d',
a68k='',
smalltalk='*.st',
ada='*.ads|*.adb',
css='*.css',
errorlist='',
asm='*.asm',
properties='',
haskell='',
java='*.java|*.jad|*.pde',
ruby='*.rb',
markdown='',
bash='*.sh|*.bsh|configure|*.ksh',
lua='*.lua',
latex='*.tex|*.sty',
javascript='*.js',
vhdl='*.vhd|*.vhdl',
xml='*.xml|*.xsl|*.svg|*.xul|*.xsd|*.dtd|*.xslt|*.axl|*.xrc|*.rdf',
html='*.html|*.htm|*.asp|*.shtml|*.htd|*.jsp|*.php3|*.phtml|*.php',
tex='*.tex|*.sty',
makefile='[Mm]akefile*',
sql='*.sql',
caml='*.ml|*.mli',
cmake='CMakeLists.txt|*.cmake|*.cmake.in|*.ctest|*.ctest.in',
metapost='*.mp|*.mpx',
tcl='*.tcl|*.exp',
cobol='',
ps='*.ps',
flagship='*.prg',
batch='*.bat|*.cmd|*.nt',
modula='',
fortran='',
verilog='*.v|*.vh',
c='*.c',
erlang='*.erl|*.hrl',
docbook='*.docbook',
conf='*.conf|.htaccess',
lisp='*.lsp|*.lisp',
diff='*.diff|*.patch',
pascal='*.dpr|*.pas|*.dfm|*.inc|*.pp',
perl='*.pl|*.pm|*.pod',
freebasic='*.bas|*.bi',
cpp='*.cc|*.cpp|*.cxx|*.hh|*.hpp|*.hxx|*.ipp|*.sma',
f77='*.f|*.for',
python='*.py',
r='*.R|*.rsource|*.S|',
txt2tags='*.t2t',
}


style_aliases={
  c='cpp',
  docbook='html',
  java='cpp',
  javascript='cpp',
  xml='html',
}


extra_props={
  a68k={
    keywords=nil
  },
  bash={
    keywords='break case continue do done elif else esac exit fi for function if in select then until while time',
    keywords1='',
    keywords2=''
  },
  c={
    keywords='asm auto break case char const continue default do double else enum extern float for goto if inline int long register return short signed sizeof static struct switch typedef union unsigned void volatile while',
  },
  cpp={
    keywords='and and_eq asm auto bitand bitor bool break case catch char class compl const const_cast continue default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new not not_eq operator or or_eq private protected public register reinterpret_cast return short signed sizeof static static_cast struct switch template this throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq ',
    keywords2='alignas alignof char16_t char32_t constexpr decltype noexcept nullptr static_assert thread_local'
  },
  docbook={
    keywords='abbrev abstract accel ackno acronym action address affiliation alt anchor answer appendix appendixinfo application area areaset areaspec arg article articleinfo artpagenums attribution audiodata audioobject author authorblurb authorgroup authorinitials beginpage bibliocoverage bibliodiv biblioentry bibliography bibliographyinfo biblioid bibliomisc bibliomixed bibliomset bibliorelation biblioset bibliosource blockinfo blockquote book bookinfo bridgehead callout calloutlist caption caution chapter chapterinfo citation citebiblioid citerefentry citetitle city classname classsynopsis classsynopsisinfo cmdsynopsis co collab collabname colophon colspec command computeroutput confdates confgroup confnum confsponsor conftitle constant constraint constraintdef constructorsynopsis contractnum contractsponsor contrib copyright coref corpauthor corpname country database date dedication destructorsynopsis edition editor email emphasis entry entrytbl envar epigraph equation errorcode errorname errortext errortype example exceptionname fax fieldsynopsis figure filename firstname firstterm footnote footnoteref foreignphrase formalpara funcdef funcparams funcprototype funcsynopsis funcsynopsisinfo function glossary glossaryinfo glossdef glossdiv glossentry glosslist glosssee glossseealso glossterm graphic graphicco group guibutton guiicon guilabel guimenu guimenuitem guisubmenu hardware highlights holder honorific htm imagedata imageobject imageobjectco important index indexdiv indexentry indexinfo indexterm informalequation informalexample informalfigure informaltable initializer inlineequation inlinegraphic inlinemediaobject interface interfacename invpartnumber isbn issn issuenum itemizedlist itermset jobtitle keycap keycode keycombo keysym keyword keywordset label legalnotice lhs lineage lineannotation link listitem iteral literallayout lot lotentry manvolnum markup medialabel mediaobject mediaobjectco member menuchoice methodname methodparam methodsynopsis mm modespec modifier ousebutton msg msgaud msgentry msgexplan msginfo msglevel msgmain msgorig msgrel msgset msgsub msgtext nonterminal note objectinfo olink ooclass ooexception oointerface option optional orderedlist orgdiv orgname otheraddr othercredit othername pagenums para paramdef parameter part partinfo partintro personblurb personname phone phrase pob postcode preface prefaceinfo primary primaryie printhistory procedure production productionrecap productionset productname productnumber programlisting programlistingco prompt property pubdate publisher publishername pubsnumber qandadiv qandaentry qandaset question quote refclass refdescriptor refentry refentryinfo refentrytitle reference referenceinfo refmeta refmiscinfo refname refnamediv refpurpose refsect1 refsect1info refsect2 refsect2info refsect3 refsect3info refsection refsectioninfo refsynopsisdiv refsynopsisdivinfo releaseinfo remark replaceable returnvalue revdescription revhistory revision revnumber revremark rhs row sbr screen screenco screeninfo screenshot secondary secondaryie sect1 sect1info sect2 sect2info sect3 sect3info sect4 sect4info sect5 sect5info section sectioninfo see seealso seealsoie seeie seg seglistitem segmentedlist segtitle seriesvolnums set setindex setindexinfo setinfo sgmltag shortaffil shortcut sidebar sidebarinfo simpara simplelist simplemsgentry simplesect spanspec state step street structfield structname subject subjectset subjectterm subscript substeps subtitle superscript surname sv symbol synopfragment synopfragmentref synopsis systemitem table tbody term tertiary tertiaryie textdata textobject tfoot tgroup thead tip title titleabbrev toc tocback tocchap tocentry tocfront toclevel1 toclevel2 toclevel3 toclevel4 toclevel5 tocpart token trademark type ulink userinput varargs variablelist varlistentry varname videodata videoobject void volumenum warning wordasword xref yeararch condition conformance id lang os remap role revision revisionflag security userlevel vendor xreflabel status label endterm linkend space width',
  },
  errorlist={
    keywords=nil
  },
  f77={
    keywords=nil
  },
  java={
    keywords='abstract assert boolean break byte case catch char class const continue default do double else enum extends final finally float for goto if implements import instanceof int interface long native new package private protected public return short static strictfp super switch synchronized this throw throws transient try var void volatile while'
  },
  javascript={
    keywords='abstract boolean break byte case catch char class const continue debugger default delete do double else enum export extends final finally float for function goto if implements import in instanceof int interface long native new package private protected public return short static super switch synchronized this throw throws transient try typeof var void volatile while with',
  },
  markdown={
    keywords=nil
  },
  po={
    keywords=nil
  },
  properties={
    keywords=nil
  },
  python={
    keywords='and as assert break class continue def del elif else except exec finally for from global if import in is lambda None not or pass print raise return try while with yield'
  },
  xml={
    keywords=nil
  },
}


custom_styles = {
  markdown = {
    blockquote = { '"#00c000"', 'Normal' },
    code =       { '"#00aaaa"', 'Normal' },
    code2 =      { '"#00bbbb"', 'Normal' },
    codebk =     { '"#00cccc"', 'Normal' },
    default =    { '_DEFLT_FG', 'Normal' },
    em1 =        { '"#808080"', 'Bold'   },
    em2 =        { '_DEFLT_FG', 'Bold'   },
    header1 =    { '"#ddb0b0"', 'Bold'   },
    header2 =    { '"#dda0a0"', 'Bold'   },
    header3 =    { '"#dd8080"', 'Bold'   },
    header4 =    { '"#dd6060"', 'Bold'   },
    header5 =    { '"#cc6060"', 'Bold'   },
    header6 =    { '"#bb4040"', 'Bold'   },
    hrule =      { '"#ffaa00"', 'Normal' },
    line_begin = { '_DEFLT_FG', 'Normal' },
    link =       { '"#0000ee"', 'Bold'   },
    olist_item = { '"#4040e0"', 'Normal' },
    prechar =    { '"#cc00cc"', 'Normal' },
    strikeout =  { '"#ee0000"', 'Normal' },
    strong1 =    { '"#808080"', 'Bold'   },
    strong2 =    { '_DEFLT_FG', 'Bold'   },
    ulist_item = { '"#4040e0"', 'Normal' },
  },
  po = {
    comment =            { "__GREEN__", "Italic" },
    flags =              { "__ORANGE_", "Normal" },
    fuzzy =              { "STREOL_FG", "Bold"   },
    msgctxt =            { "_DEFLT_FG", "Bold"   },
    msgctxt_text =       { "_DEFLT_FG", "Bold"   },
    msgctxt_text_eol =   { "_DEFLT_FG", "Bold"   },
    msgid =              { "_DEFLT_FG", "Bold"   },
    msgid_text =         { "__PURPLE_", "Normal" },
    msgid_text_eol =     { "__PURPLE_", "Normal" },
    msgstr  =            { "_DEFLT_FG", "Bold"   },
    msgstr_text =        { "___BLUE__", "Normal" },
    msgstr_text_eol =    { "___BLUE__", "Normal" },
    programmer_comment = { "__GREEN__", "Italic" },
    reference =          { "_DEFLT_FG", "Normal" },
  }
}



function fixup_lexer_name(name)
  local realnames = {
    "A68k",
    "Ada",
    "Asm",
    "Bash",
    "Batch",
    "C",
    "Caml",
    "CMAKE",
    "COBOL",
    "Conf",
    "Cpp",
    "CSS",
    "D",
    "Diff",
    "Docbook",
    "Erlang",
    "ErrorList",
    "F77",
    "FlagShip",
    "Fortran",
    "Freebasic",
    "Haskell",
    "HTML",
    "Java",
    "Javascript",
    "LaTeX",
    "Lisp",
    "Lua",
    "MakeFile",
    "Markdown",
    "Metapost",
    "Modula",
    "Pascal",
    "Perl",
    "Po",
    "Properties",
    "PS",
    "Python",
    "R",
    "Ruby",
    "Smalltalk",
    "SQL",
    "TCL",
    "TeX",
    "Txt2tags",
    "Verilog",
    "VHDL",
    "XML",
    "NULL"
  }
  local lowername=name:lower()
  for i,v in ipairs(realnames) do
    if v:lower()==lowername then return v end
  end
  return name:sub(1,1):upper()..name:sub(2,-1)
end



html_normal_styles={
  ba_word=1,
  b_word=1,
  ja_default=1,
  ja_keyword=1,
  ja_symbols=1,
  ja_word=1,
  j_default=1,
  j_keyword=1,
  j_number=1,
  j_symbols=1,
  j_word=1,
  pa_classname=1,
  pa_defname=1,
  pa_operator=1,
  pa_word=1,
  p_classname=1,
  p_defname=1,
  php_operator=1,
  php_word=1,
  p_operator=1,
  p_word=1,
  sgml_command=1,
}

html_default_backgrounds={
  j_comment=1,
  j_commentdoc=1,
  j_commentline=1,
  j_default=1,
  j_doublestring=1,
  j_keyword=1,
  j_number=1,
  j_singlestring=1,
  j_symbols=1,
  j_word=1,
  ja_doublestring=1,
  ja_singlestring=1,
  sgml_doublestring=1,
}



