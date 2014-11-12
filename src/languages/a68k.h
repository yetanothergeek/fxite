/* lexname=a68k lexpfx=SCE_A68K_ lextag=SCLEX_A68K */

static StyleDef a68k_style[] = {
  { "comment", SCE_A68K_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentdoxygen", SCE_A68K_COMMENT_DOXYGEN, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentspecial", SCE_A68K_COMMENT_SPECIAL, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentword", SCE_A68K_COMMENT_WORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "cpuinstruction", SCE_A68K_CPUINSTRUCTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "default", SCE_A68K_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "directive", SCE_A68K_DIRECTIVE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "extinstruction", SCE_A68K_EXTINSTRUCTION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_A68K_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "label", SCE_A68K_LABEL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "macroarg", SCE_A68K_MACRO_ARG, _DEFLT_FG, _DEFLT_BG, Normal },
  { "macrodeclaration", SCE_A68K_MACRO_DECLARATION, _DEFLT_FG, _DEFLT_BG, Normal },
  { "numberbin", SCE_A68K_NUMBER_BIN, NUMBER_FG, _DEFLT_BG, Normal },
  { "numberdec", SCE_A68K_NUMBER_DEC, NUMBER_FG, _DEFLT_BG, Normal },
  { "numberhex", SCE_A68K_NUMBER_HEX, NUMBER_FG, _DEFLT_BG, Normal },
  { "operator", SCE_A68K_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "register", SCE_A68K_REGISTER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string1", SCE_A68K_STRING1, STRING_FG, _DEFLT_BG, Normal },
  { "string2", SCE_A68K_STRING2, STRING_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* a68k_words[]= {
  "",
  "",
  "",
  "",
  "",
  "",
  NULL
};


static const char* a68k_mask = "";


static const char* a68k_apps = "";


static LangStyle LangA68k = {
  "a68k",
  SCLEX_A68K,
  a68k_style,
  (char**)a68k_words,
  (char*)a68k_mask,
  (char*)a68k_apps,
  0,
  TABS_DEFAULT,
  0
};


