/* lexname=haskell lexpfx=SCE_HA_ lextag=SCLEX_HASKELL */

static StyleDef haskell_style[] = {
  { "default", SCE_HA_DEFAULT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "identifier", SCE_HA_IDENTIFIER, _DEFLT_FG, _DEFLT_BG, Normal },
  { "keyword", SCE_HA_KEYWORD, _WORD1_FG, _DEFLT_BG, Bold },
  { "number", SCE_HA_NUMBER, NUMBER_FG, _DEFLT_BG, Normal },
  { "string", SCE_HA_STRING, STRING_FG, _DEFLT_BG, Normal },
  { "character", SCE_HA_CHARACTER, STRING_FG, _DEFLT_BG, Normal },
  { "class", SCE_HA_CLASS, _WORD3_FG, _DEFLT_BG, Bold },
  { "module", SCE_HA_MODULE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "capital", SCE_HA_CAPITAL, _DEFLT_FG, _DEFLT_BG, Normal },
  { "data", SCE_HA_DATA, _DEFLT_FG, _DEFLT_BG, Normal },
  { "import", SCE_HA_IMPORT, _DEFLT_FG, _DEFLT_BG, Normal },
  { "operator", SCE_HA_OPERATOR, OPERTR_FG, _DEFLT_BG, Bold },
  { "instance", SCE_HA_INSTANCE, _DEFLT_FG, _DEFLT_BG, Normal },
  { "commentline", SCE_HA_COMMENTLINE, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentblock", SCE_HA_COMMENTBLOCK, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentblock2", SCE_HA_COMMENTBLOCK2, COMMNT_FG, _DEFLT_BG, Italic },
  { "commentblock3", SCE_HA_COMMENTBLOCK3, COMMNT_FG, _DEFLT_BG, Italic },
  { NULL, 0, _DEFLT_FG, _DEFLT_BG, Normal }
};


static const char* haskell_words[]= {NULL};


static const char* haskell_mask = "";


static const char* haskell_apps = "";


static LangStyle LangHaskell = {
  "haskell",
  SCLEX_HASKELL,
  haskell_style,
  (char**)haskell_words,
  (char*)haskell_mask,
  (char*)haskell_apps,
  0,
  TABS_DEFAULT
};


