/* lexname=ruby lexpfx=SCE_RB_ lextag=SCLEX_RUBY */

static StyleDef ruby_style[] = {
  { "default", SCE_RB_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "error", SCE_RB_ERROR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentline", SCE_RB_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "pod", SCE_RB_POD, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_RB_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_RB_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_RB_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_RB_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "classname", SCE_RB_CLASSNAME, _WORD3_FG, _DEFLT_BG, Bold },
  { "defname", SCE_RB_DEFNAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_RB_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_RB_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "regex", SCE_RB_REGEX, _DEFLT_FG, _DEFLT_BG, Normal },
  { "global", SCE_RB_GLOBAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "symbol", SCE_RB_SYMBOL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "modulename", SCE_RB_MODULE_NAME, _DEFLT_FG, _DEFLT_BG, Normal },
  { "instancevar", SCE_RB_INSTANCE_VAR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "classvar", SCE_RB_CLASS_VAR, _WORD3_FG, _DEFLT_BG, Bold },
  { "backticks", SCE_RB_BACKTICKS, __ORANGE_, _DEFLT_BG, Normal },
  { "datasection", SCE_RB_DATASECTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "heredelim", SCE_RB_HERE_DELIM, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereq", SCE_RB_HERE_Q, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereqq", SCE_RB_HERE_QQ, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereqx", SCE_RB_HERE_QX, _DEFLT_FG, _HERE_BG_, Normal },
  { "stringq", SCE_RB_STRING_Q, STRING_FG, _DEFLT_BG, Normal },
  { "stringqq", SCE_RB_STRING_QQ, STRING_FG, _DEFLT_BG, Normal },
  { "stringqx", SCE_RB_STRING_QX, STRING_FG, _DEFLT_BG, Normal },
  { "stringqr", SCE_RB_STRING_QR, STRING_FG, _DEFLT_BG, Normal },
  { "stringqw", SCE_RB_STRING_QW, STRING_FG, _DEFLT_BG, Normal },
  { "worddemoted", SCE_RB_WORD_DEMOTED, _WORD1_FG, _DEFLT_BG, Bold },
  { "stdin", SCE_RB_STDIN, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stdout", SCE_RB_STDOUT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "stderr", SCE_RB_STDERR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "upperbound", SCE_RB_UPPER_BOUND, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* ruby_words[]= {
  "__FILE__ and def end in or self unless __LINE__ begin defined? ensure module redo super until BEGIN break do false next rescue then when END case else for nil retry true while alias class elsif if not return undef yield",
  NULL
};


static const char* ruby_mask = "*.rb";


static const char* ruby_apps = "ruby";


static LangStyle LangRuby = {
  "ruby",
  SCLEX_RUBY,
  ruby_style,
  (char**)ruby_words,
  (char*)ruby_mask,
  (char*)ruby_apps,
  0,
  TABS_DEFAULT,
  0
};


