/* lexname=bash lexpfx=SCE_SH_ lextag=SCLEX_BASH */

static StyleDef bash_style[] = {
  { "default", SCE_SH_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "error", SCE_SH_ERROR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentline", SCE_SH_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "number", SCE_SH_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "word", SCE_SH_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "string", SCE_SH_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_SH_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "operator", SCE_SH_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "identifier", SCE_SH_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "scalar", SCE_SH_SCALAR, SCALAR_FG, _DEFLT_BG, Normal },
  { "param", SCE_SH_PARAM, SCALAR_FG, _DEFLT_BG, Normal },
  { "backticks", SCE_SH_BACKTICKS, __ORANGE_, _DEFLT_BG, Normal },
  { "heredelim", SCE_SH_HERE_DELIM, _DEFLT_FG, _HERE_BG_, Normal },
  { "hereq", SCE_SH_HERE_Q, _DEFLT_FG, _HERE_BG_, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* bash_words[]= {
  "break case continue do done elif else esac exit fi for function if in select then until while time",
  NULL
};


static const char* bash_mask = "*.sh|*.bsh|configure";


static const char* bash_apps = "ash|bash|dash|sh";


static LangStyle LangBash = {
  "bash",
  SCLEX_BASH,
  bash_style,
  (char**)bash_words,
  (char*)bash_mask,
  (char*)bash_apps,
  0,
  TABS_DEFAULT
};


