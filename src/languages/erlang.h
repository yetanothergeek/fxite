/* lexname=erlang lexpfx=SCE_ERLANG_ lextag=SCLEX_ERLANG */

static StyleDef erlang_style[] = {
  { "default", SCE_ERLANG_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_ERLANG_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "variable", SCE_ERLANG_VARIABLE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_ERLANG_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_ERLANG_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_ERLANG_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_ERLANG_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "atom", SCE_ERLANG_ATOM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "functionname", SCE_ERLANG_FUNCTION_NAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "character", SCE_ERLANG_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "macro", SCE_ERLANG_MACRO, _DEFLT_FG, _DEFLT_BG, Normal },
  { "record", SCE_ERLANG_RECORD, _DEFLT_FG, _DEFLT_BG, Normal },
  { "separator", SCE_ERLANG_SEPARATOR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "nodename", SCE_ERLANG_NODE_NAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "unknown", SCE_ERLANG_UNKNOWN, STREOL_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* erlang_words[]= {
  "after begin case catch cond end fun if let of query receive when define record export import include include_lib ifdef ifndef else endif undef apply attribute call do in letrec module primop try",
  NULL
};


static const char* erlang_mask = "*.erl";


static const char* erlang_apps = "";


static LangStyle LangErlang = {
  "erlang",
  SCLEX_ERLANG,
  erlang_style,
  (char**)erlang_words,
  (char*)erlang_mask,
  (char*)erlang_apps,
  0,
  TABS_DEFAULT
};


