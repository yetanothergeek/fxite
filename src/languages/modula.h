/* lexname=modula lexpfx=SCE_MODULA_ lextag=SCLEX_MODULA */

static StyleDef modula_style[] = {
  { "default", SCE_MODULA_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "comment", SCE_MODULA_COMMENT, COMMNT_FG, _DEFLT_BG, Italic },
  { "doxycomm", SCE_MODULA_DOXYCOMM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "doxykey", SCE_MODULA_DOXYKEY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_MODULA_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "reserved", SCE_MODULA_RESERVED, _DEFLT_FG, _DEFLT_BG, Normal },
  { "number", SCE_MODULA_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "basenum", SCE_MODULA_BASENUM, _DEFLT_FG, _DEFLT_BG, Normal },
  { "float", SCE_MODULA_FLOAT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "string", SCE_MODULA_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "strspec", SCE_MODULA_STRSPEC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "char", SCE_MODULA_CHAR, _DEFLT_FG, _DEFLT_BG, Normal },
  { "charspec", SCE_MODULA_CHARSPEC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "proc", SCE_MODULA_PROC, _DEFLT_FG, _DEFLT_BG, Normal },
  { "pragma", SCE_MODULA_PRAGMA, _DEFLT_FG, _DEFLT_BG, Normal },
  { "prgkey", SCE_MODULA_PRGKEY, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_MODULA_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "badstr", SCE_MODULA_BADSTR, _DEFLT_FG, _DEFLT_BG, Normal },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* modula_words[]= {
  "",
  "",
  "",
  "",
  "",
  "",
  NULL
};


static const char* modula_mask = "";


static const char* modula_apps = "";


static LangStyle LangModula = {
  "modula",
  SCLEX_MODULA,
  modula_style,
  (char**)modula_words,
  (char*)modula_mask,
  (char*)modula_apps,
  0,
  TABS_DEFAULT,
  0
};


