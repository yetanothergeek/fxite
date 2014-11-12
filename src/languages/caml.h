/* lexname=caml lexpfx=SCE_CAML_ lextag=SCLEX_CAML */

static StyleDef caml_style[] = {
  { "char", SCE_CAML_CHAR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_CAML_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "comment1", SCE_CAML_COMMENT1, COMMNT_FG, _DEFLT_BG, Italic },
  { "comment2", SCE_CAML_COMMENT2, COMMNT_FG, _DEFLT_BG, Italic },
  { "comment3", SCE_CAML_COMMENT3, COMMNT_FG, _DEFLT_BG, Italic },
  { "default", SCE_CAML_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_CAML_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_CAML_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "keyword2", SCE_CAML_KEYWORD2, _WORD2_FG, _DEFLT_BG, Bold },
  { "keyword3", SCE_CAML_KEYWORD3, _WORD1_FG, _DEFLT_BG, Bold },
  { "linenum", SCE_CAML_LINENUM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_CAML_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_CAML_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "string", SCE_CAML_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "tagname", SCE_CAML_TAGNAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "white", SCE_CAML_WHITE, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* caml_words[]= {
  "and as assert asr begin class constraint do done downto else end exception external false for fun function functor if in include inherit initializer land lazy let lor lsl lsr lxor match method mod module mutable new object of open or private rec sig struct then to true try type val virtual when while with",
  "option Some None ignore ref lnot succ pred parser",
  "array bool char float int list string unit",
  NULL
};


static const char* caml_mask = "*.ml|*.mli";


static const char* caml_apps = "";


static LangStyle LangCaml = {
  "caml",
  SCLEX_CAML,
  caml_style,
  (char**)caml_words,
  (char*)caml_mask,
  (char*)caml_apps,
  0,
  TABS_DEFAULT,
  0
};


